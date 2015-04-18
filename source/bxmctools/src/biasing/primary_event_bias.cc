// primary_event_bias.cc

// Ourselves:
#include <mctools/biasing/primary_event_bias.h>

// Standard library:

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/temporary_files.h>
// - Bayeux/geomtools:
#include <geomtools/manager.h>
#include <geomtools/utils.h>
#include <geomtools/geomtools_config.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_draw.h>
#include <geomtools/gnuplot_drawer.h>
#include <geomtools/gnuplot_i.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/sphere.h>
#include <geomtools/line_3d.h>
#include <geomtools/placement.h>
// - Bayeux/genbb_help:
#include <genbb_help/primary_event.h>
#include <genbb_help/primary_particle.h>

// // This project:
// #include <mctools/XXX.h>

namespace mctools {

  namespace biasing {

    // static
    const std::string & primary_event_bias::biased_event_status_key()
    {
      static const std::string _k("primary_biasing.event.status");
      return _k;
    }

    // static
    const std::string & primary_event_bias::normal_event_label()
    {
      static const std::string _lbl("normal");
      return _lbl;
    }

    // static
    const std::string & primary_event_bias::killed_event_label()
    {
      static const std::string _lbl("killed");
      return _lbl;
    }

    // static
    const std::string & primary_event_bias::truncated_event_label()
    {
      static const std::string _lbl("truncated");
      return _lbl;
    }

    // static
    const std::string & primary_event_bias::master_particle_flag()
    {
      static const std::string _lbl("master");
      return _lbl;
    }

    // static
    const std::string & primary_event_bias::dont_track_this_particle_flag()
    {
      static const std::string _flag("primary_biasing.particle.dont_track");
      return _flag;
    }

    primary_event_bias::stat_record::stat_record()
    {
      total_counts = 0;
      killed_counts = 0;
      truncated_counts = 0;
      normal_counts = 0;
      return;
    }

    void primary_event_bias::stat_record::reset()
    {
      total_counts = 0;
      killed_counts = 0;
      truncated_counts = 0;
      normal_counts = 0;
      return;
    }

    const primary_event_bias::stat_record & primary_event_bias::get_stats() const
    {
      return _stats_;
    }


    void primary_event_bias::reset_stats()
    {
      _stats_.reset();
      return;
    }

    primary_event_bias::biasing_info::biasing_info()
    {
      _status_ = BES_UNDEFINED;
      return;
    }

    void primary_event_bias::biasing_info::reset()
    {
      _status_ = BES_UNDEFINED;
      return;
    }


    void primary_event_bias::biasing_info::set_status(biased_event_status s_)
    {
      _status_ = s_;
      return;
    }

    primary_event_bias::biased_event_status
    primary_event_bias::biasing_info::get_status() const
    {
      return _status_;
    }

    bool primary_event_bias::biasing_info::is_normal() const
    {
      return _status_ == BES_NORMAL;
    }

    bool primary_event_bias::biasing_info::is_truncated() const
    {
      return _status_ == BES_TRUNCATED;
    }

    bool primary_event_bias::biasing_info::is_killed() const
    {
      return _status_ == BES_KILLED;
    }

    void primary_event_bias::_set_default()
    {
      _geom_mgr_ = 0;
      _bias_mode_ = BIAS_NONE;
      datatools::invalidate(_particle_min_energy_);
      datatools::invalidate(_particle_max_energy_);
      _master_particle_rank_ = INVALID_PARTICLE_RANK;
      _track_only_master_particle_ = false;
      datatools::invalidate(_total_min_energy_);
      datatools::invalidate(_total_max_energy_);
      return;
    }

    primary_event_bias::primary_event_bias()
    {
      _initialized_ = false;
      _logging_ = datatools::logger::PRIO_FATAL;
      _set_default();
      return;
    }

    primary_event_bias::~primary_event_bias()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      if (is_initialized()) {
        reset();
      }
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    void primary_event_bias::set_geometry_manager(const geomtools::manager & gm_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Operation prohibited ! Object is already initialized !");
      _geom_mgr_ = &gm_;
      return;
    }

    void primary_event_bias::set_mapping_name(const std::string & mn_)
    {
      _mapping_name_ = mn_;
      return;
    }

    const std::string & primary_event_bias::get_mapping_name() const
    {
      return _mapping_name_;
    }

    bool primary_event_bias::is_initialized() const
    {
      return _initialized_;
    }

    void primary_event_bias::initialize_simple()
    {
      datatools::properties dummy;
      initialize(dummy);
      return;
    }

    void primary_event_bias::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Operation prohibited ! Object is already initialized !");

      // if (_logging_ == datatools::logger::PRIO_UNDEFINED)
      {
        datatools::logger::priority p =
          datatools::logger::extract_logging_configuration(config_,
                                                           _logging_,
                                                           true);
        if (p > datatools::logger::PRIO_UNDEFINED) {
          set_logging(p);
        }
      }
      if (_logging_ == datatools::logger::PRIO_UNDEFINED) {
        _logging_ = datatools::logger::PRIO_FATAL;
      }

      double default_energy_unit = CLHEP::keV;

      if (config_.has_key("mode")) {
        const std::string & bm_label = config_.fetch_string("mode");
        if (bm_label == "none") {
          set_bias_mode(BIAS_NONE);
        } else if (bm_label == "master") {
          set_bias_mode(BIAS_ONLY_MASTER_PARTICLE);
          // } else if (bm_label == "all") {
          //   set_bias_mode(BIAS_ALL_PARTICLES);
        } else {
          DT_THROW(std::logic_error, "Invalid bias mode '" << bm_label << "'!");
        }
      }

      if (_bias_mode_ == BIAS_UNDEFINED) {
        set_bias_mode(BIAS_NONE);
      }

      // Candidate particles criteria:
      if (config_.has_key("particle.labels")) {
        std::vector<std::string> part_labels;
        config_.fetch("particle.labels", part_labels);
        for (int i = 0; i < (int) part_labels.size(); i++) {
          add_particle_by_label(part_labels[i]);
        }
      }

      if (config_.has_key("particle.min_energy")) {
        double min_ke = config_.fetch_real("particle.min_energy");
        if (! config_.has_explicit_unit("particle.min_energy")) {
          min_ke *= default_energy_unit;
        }
        set_particle_minimum_energy(min_ke);
      }

      if (config_.has_key("particle.max_energy")) {
        double max_ke = config_.fetch_real("particle.max_energy");
        if (! config_.has_explicit_unit("particle.max_energy")) {
          max_ke *= default_energy_unit;
        }
        set_particle_maximum_energy(max_ke);
      }

      if (datatools::is_valid(_particle_min_energy_) &&
          datatools::is_valid(_particle_max_energy_) ) {
        DT_THROW_IF(_particle_min_energy_ >= _particle_max_energy_,
                    std::range_error,
                    "Invalid master particle min energy !");
      }

      if (config_.has_key("total_min_energy")) {
        double min_ke = config_.fetch_real("total_min_energy");
        if (! config_.has_explicit_unit("total_min_energy")) {
          min_ke *= default_energy_unit;
        }
        set_total_minimum_energy(min_ke);
      }

      if (config_.has_key("total_max_energy")) {
        double max_ke = config_.fetch_real("total_max_energy");
        if (! config_.has_explicit_unit("total_max_energy")) {
          max_ke *= default_energy_unit;
        }
        set_total_maximum_energy(max_ke);
      }

      // if (! datatools::is_valid(_total_min_energy_)) {
      //   set_total_minimum_energy(0.0);
      // }

      if (datatools::is_valid(_total_max_energy_) && datatools::is_valid(_total_min_energy_)) {
        DT_THROW_IF(_total_max_energy_ < _total_min_energy_,
                    std::domain_error,
                    "Invalid min/max total kinetic energy!");
      }

      if (is_using_master_particle()) {

        if (config_.has_key("master_particle.rank")) {
          int rank = config_.fetch_integer("master_particle.rank");
          set_master_particle_rank(rank);
        }

        if (config_.has_key("master_particle.track_only")) {
          set_track_only_master_particle(config_.fetch_boolean("master_particle.track_only"));
        }

        // Force master particle rank:
        if (_master_particle_rank_ == INVALID_PARTICLE_RANK) {
          _master_particle_rank_ = DEFAULT_PARTICLE_RANK;
        }

      } // is_using_master_particle

      /*
        if (is_using_all_particles()) {

        } // is_using_all_particles
      */

      std::vector<std::string> poi_names;
      /* Syntax:
       *
       *  points_of_interest : string[2] = "detector0" "detector1"
       *  points_of_interest.detector0.position     : string = " 1.2 3.4 4.5 mm"
       *  points_of_interest.detector0.radius       : real as length = 20 cm
       *  points_of_interest.detector0.attractivity_label : string = "attractive"
       *  points_of_interest.detector1.position     : string = " -1.2 -3.4 4.5 mm"
       *  points_of_interest.detector1.radius       : real as length = 40 cm
       *  points_of_interest.detector1.attractivity_label : string = "repulsive"
       *
       */
      if (config_.has_key("points_of_interest")) {
        config_.fetch("points_of_interest", poi_names);
        for (int i = 0; i < (int) poi_names.size(); i++) {
          const std::string & poi_name = poi_names[i];
          point_of_interest poi;
          poi.set_name(poi_name);
          datatools::properties poi_config;
          std::ostringstream poi_config_prefix_oss;
          poi_config_prefix_oss << "points_of_interest." << poi_name << '.';
          config_.export_and_rename_starting_with(poi_config, poi_config_prefix_oss.str(), "");
          poi.initialize(poi_config, _geom_mgr_);
          add_point_of_interest(poi_name, poi);
        }
      }

      _initialized_ = true;
      return;
    }

    void primary_event_bias::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Operation prohibited ! Object is not initialized !");
      DT_LOG_TRACE(_logging_, "Total processed events = " << _stats_.total_counts);
      DT_LOG_TRACE(_logging_, " - killed events       = " << _stats_.killed_counts);
      DT_LOG_TRACE(_logging_, " - truncated events    = " << _stats_.truncated_counts);
      DT_LOG_TRACE(_logging_, " - normal events       = " << _stats_.normal_counts);
      _initialized_ = false;
      _mapping_name_.clear();
      _pois_.clear();
      _geom_mgr_ = 0;
      _particle_types_.clear();
      reset_stats();
      _set_default();
      return;
    }

    void primary_event_bias::set_bias_mode(bias_mode_type bm_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Operation prohibited ! Object is already initialized !");
      _bias_mode_ = bm_;
      return;
    }

    primary_event_bias::bias_mode_type primary_event_bias::get_bias_mode() const
    {
      return _bias_mode_;
    }

    bool primary_event_bias::is_using_no_bias() const
    {
      return _bias_mode_ == BIAS_NONE;
    }

    bool primary_event_bias::is_using_master_particle() const
    {
      return _bias_mode_ == BIAS_ONLY_MASTER_PARTICLE;
    }

    bool primary_event_bias::is_using_all_particles() const
    {
      return _bias_mode_ == BIAS_ALL_PARTICLES;
    }

    void primary_event_bias::add_particle_by_label(const std::string & label_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Operation prohibited ! Object is already initialized !");
      int type = genbb::primary_particle::particle_type_from_label(label_);
      DT_THROW_IF(type == genbb::primary_particle::PARTICLE_UNKNOWN,
                  std::logic_error,
                  "Unknown particle label '" << label_ << "'!");
      _particle_types_.insert(type);
      return;
    }

    void primary_event_bias::add_particle_by_type(int type_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Operation prohibited ! Object is already initialized !");
      std::string label = genbb::primary_particle::particle_label_from_type(type_);
      DT_THROW_IF(label.empty(),
                  std::logic_error,
                  "Unknown particle type '" << type_ << "'!");
      _particle_types_.insert(type_);
      return;
    }


    // void primary_event_bias::add_particle_by_pdg_code(int /*code_*/)
    // {
    //   DT_THROW_IF(is_initialized(), std::logic_error,
    //               "Operation prohibited ! Object is already initialized !");
    //   DT_THROW(std::logic_error, "Particle PDG code not supported yet!");
    //   // _particle_types_.insert(XXX);
    //   return;
    // }

    void primary_event_bias::set_particle_minimum_energy(double kemin_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Operation prohibited ! Object is already initialized !");
      DT_THROW_IF(kemin_ < 0.0, std::range_error,
                  "Invalid minimum kinetic energy !");
      _particle_min_energy_ = kemin_;
      return;
    }

    void primary_event_bias::set_particle_maximum_energy(double kemax_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Operation prohibited ! Object is already initialized !");
      DT_THROW_IF(kemax_ < 0.0, std::range_error,
                  "Invalid maximum kinetic energy !");
      _particle_max_energy_ = kemax_;
      return;
    }

    void primary_event_bias::set_master_particle_rank(int rank_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Operation prohibited ! Object is already initialized !");
      if (rank_ < 0) {
        _master_particle_rank_ = INVALID_PARTICLE_RANK;
      } else {
        _master_particle_rank_ = rank_;
      }
      return;
    }

    void primary_event_bias::set_track_only_master_particle(bool to_)
    {
      _track_only_master_particle_ = to_;
      return;
    }

    void primary_event_bias::set_total_minimum_energy(double kemin_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Operation prohibited ! Object is already initialized !");
      DT_THROW_IF(kemin_ < 0.0, std::range_error,
                  "Invalid minimum kinetic energy !");
      _total_min_energy_ = kemin_;
      return;
    }

    void primary_event_bias::set_total_maximum_energy(double kemax_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Operation prohibited ! Object is already initialized !");
      DT_THROW_IF(kemax_ < 0.0, std::range_error,
                  "Invalid maximum kinetic energy !");
      _total_max_energy_ = kemax_;
      return;
    }

    void primary_event_bias::set_logging(datatools::logger::priority p_)
    {
      _logging_ = p_;
      return;
    }

    datatools::logger::priority primary_event_bias::get_logging() const
    {
      return _logging_;
    }

    void primary_event_bias::_make_candidate_particles(genbb::primary_event & event_,
                                                       std::vector<genbb::primary_particle *> & candidates_) const
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      for (genbb::primary_event::particles_col_type::iterator i =
             event_.grab_particles().begin();
           i != event_.grab_particles().end();
           i++) {
        genbb::primary_particle & part = *i;
        bool candidate = true;

        // Only consider particles of some specific types:
        if (candidate && _particle_types_.size()) {
          if (_particle_types_.count(part.get_type()) == 0) {
            DT_LOG_TRACE(_logging_, "Particle of type '" << part.get_type() << "' is not used!");
            candidate = false;
          }
        }

        // Only consider particles with minimum kinetic energy:
        if (candidate && datatools::is_valid(_particle_min_energy_)) {
          if (part.get_kinetic_energy() < _particle_min_energy_) {
            DT_LOG_TRACE(_logging_, "Particle of kinetic energy '" << part.get_kinetic_energy() << "' is not used (too low)!");
            candidate = false;
          }
        }

        // Only consider particles with maximum kinetic energy:
        if (candidate && datatools::is_valid(_particle_max_energy_)) {
          if (part.get_kinetic_energy() > _particle_max_energy_) {
            DT_LOG_TRACE(_logging_, "Particle of kinetic energy '" << part.get_kinetic_energy() << "' is not used (too large)!");
            candidate = false;
          }
        }

        if (candidate) {
          // Tag the particle as a valid candidate:
          candidates_.push_back(&part);
          DT_LOG_TRACE(_logging_, "Tag this particle as a candidate for biasing...");
        }
      } // for
      DT_LOG_TRACE(_logging_, "Number of candidate particles: " << candidates_.size());
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    void primary_event_bias::add_point_of_interest(const std::string & poi_name_,
                                                   const point_of_interest & poi_)
    {
      DT_THROW_IF(_pois_.find(poi_name_) != _pois_.end(),
                  std::logic_error,
                  "Bias already has a point of interest registered with name '" << poi_name_ << "'!");
      {
        poi_entry_type dummy;
        dummy.poi = poi_;
        if (dummy.poi.get_name().empty()) {
          dummy.poi.set_name(poi_name_);
        }
        _pois_[poi_name_] = dummy;
      }
      return;
    }

    bool primary_event_bias::has_point_of_interest(const std::string & poi_name_) const
    {
      return _pois_.find(poi_name_) != _pois_.end();
    }

    const primary_event_bias::poi_entry_type &
    primary_event_bias::get_point_of_interest(const std::string & poi_name_) const
    {
      poi_dict_type::const_iterator found = _pois_.find(poi_name_);
      DT_THROW_IF(found == _pois_.end(), std::logic_error,
                  "Cannot find POI with name '" << poi_name_ << "'!");
      return found->second;
    }

    primary_event_bias::poi_entry_type &
    primary_event_bias::grab_point_of_interest(const std::string & poi_name_)
    {
      poi_dict_type::iterator found = _pois_.find(poi_name_);
      DT_THROW_IF(found == _pois_.end(), std::logic_error,
                  "Cannot find POI with name '" << poi_name_ << "'!");
      return found->second;
    }

    bool primary_event_bias::_validate_particle_direction(const genbb::primary_particle & part_,
                                                          const geomtools::vector_3d & vtx_) const
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      if (_pois_.size() == 0) {
        // Default to true if no PoIs are available:
        DT_LOG_TRACE(_logging_, "Particle is validate because no PoI is defined!");
        DT_LOG_TRACE_EXITING(_logging_);
        return true;
      }

      const geomtools::vector_3d * vtx = 0;
      if (geomtools::is_valid(vtx_)) {
        vtx = &vtx_;
      } else if (part_.has_vertex()) {
        vtx = &part_.get_vertex();
      } else {
        DT_THROW(std::logic_error, "No available vertex associated to the particle!");
      }

      // First check if the particle hits a repulsive PoI:
      int repulsive_counter = 0;
      int hit_repulsive_counter = 0;
      for (poi_dict_type::const_iterator i = _pois_.begin();
           i != _pois_.end();
           i++) {
        const poi_entry_type & pe = i->second;
        if (! pe.poi.is_repulsive()) {
          continue;
        }
        DT_LOG_TRACE(_logging_, "Checking particle direction w.r.t the '" << i->first << "' repulsive PoI...");
        repulsive_counter++;
        if (pe.poi.hit(*vtx, part_.get_momentum())) {
          hit_repulsive_counter++;
        } else {
          DT_LOG_TRACE(_logging_, "Particle does not hit repulsive PoI '" << i->first << "'!");
        }
      }
      DT_LOG_TRACE(_logging_, "Number of repulsive PoIs = " << repulsive_counter);
      if (repulsive_counter > 0) {
        // If some repulsive PoIs exist and at least one is hit,
        // then the particle is not validated:
        if (hit_repulsive_counter > 0) {
          DT_LOG_TRACE(_logging_, "Not validated because the particle hits a repulsive PoI!");
          DT_LOG_TRACE_EXITING(_logging_);
          return false;
        }
      }

      // Second check if the particle hits an attractive PoI:
      int attractive_counter = 0;
      int hit_attractive_counter = 0;
      for (poi_dict_type::const_iterator i = _pois_.begin();
           i != _pois_.end();
           i++) {
        const poi_entry_type & pe = i->second;
        if (! pe.poi.is_attractive()) {
          continue;
        }
        DT_LOG_TRACE(_logging_, "Checking particle direction w.r.t the '" << i->first << "' attractive PoI...");
        attractive_counter++;
        if (pe.poi.hit(*vtx, part_.get_momentum())) {
          hit_attractive_counter++;
        } else {
          DT_LOG_TRACE(_logging_, "Particle does not hit attractive PoI '" << i->first << "'!");
        }
      }
      DT_LOG_TRACE(_logging_, "Number of attractive PoIs = " << attractive_counter);
      if (attractive_counter > 0) {
        // If some attractive PoI exist and none is hit,
        // then the particle is not validated:
        if (hit_attractive_counter == 0) {
          DT_LOG_TRACE(_logging_, "Not validated because the particle does not hit any attractive PoI!");
          DT_LOG_TRACE_EXITING(_logging_);
          return false;
        }
      }

      // Otherwise, always accept the particle, whatever its direction is.
      DT_LOG_TRACE(_logging_, "Validated because the particle has a good direction!");
      DT_LOG_TRACE_EXITING(_logging_);
      return true;
    }

    void primary_event_bias::process(const geomtools::vector_3d & vertex_,
                                     genbb::primary_event & event_,
                                     biasing_info & bi_)
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      DT_THROW_IF(!is_initialized(), std::logic_error, "Object is not initialized !");

      bi_.reset();
      _stats_.total_counts++;
      if (is_using_no_bias()) {
        // No biasing is applied:
        event_.grab_auxiliaries().store(biased_event_status_key(), killed_event_label());
        bi_.set_status(BES_NORMAL);
        _stats_.normal_counts++;
        DT_LOG_TRACE_EXITING(_logging_);
        return;
      } // is_using_no_bias()

      // Build the list of candidate particles:
      std::vector<genbb::primary_particle *> candidate_particles;
      _make_candidate_particles(event_, candidate_particles);
      if (candidate_particles.size() == 0) {
        // No candidate particles are considered:
        event_.grab_auxiliaries().store(biased_event_status_key(), killed_event_label());
        bi_.set_status(BES_KILLED);
        _stats_.killed_counts++;
        DT_LOG_TRACE(_logging_, "No candidate particles!");
        DT_LOG_TRACE_EXITING(_logging_);
        return;
      }

      // Compute the total kinetic energy from candidate particles:
      double ekin_sum = -1.0;
      for (int i = 0; i < (int) candidate_particles.size(); i++) {
        if (ekin_sum < 0) {
          ekin_sum = 0.0;
        }
        const genbb::primary_particle & part = *candidate_particles[i];
        double ekin = part.get_kinetic_energy();
        ekin_sum += ekin;
      }

      if (datatools::is_valid(_total_min_energy_)) {
        if (ekin_sum < _total_min_energy_) {
          // Not enough total kinetic energy from candidate particles:
          event_.grab_auxiliaries().store(biased_event_status_key(), killed_event_label());
          bi_.set_status(BES_KILLED);
          _stats_.killed_counts++;
          DT_LOG_TRACE(_logging_, "Not enough energy from candidate particles!");
          DT_LOG_TRACE_EXITING(_logging_);
          return;
        }
      }

      if (datatools::is_valid(_total_max_energy_)) {
        if (ekin_sum > _total_max_energy_) {
          // Too much total kinetic energy from candidate particles:
          event_.grab_auxiliaries().store(biased_event_status_key(), killed_event_label());
          bi_.set_status(BES_KILLED);
          _stats_.killed_counts++;
          DT_LOG_TRACE(_logging_, "Too much energy from candidate particles!");
          DT_LOG_TRACE_EXITING(_logging_);
          return;
        }
      }

      if (is_using_master_particle()) {
        DT_LOG_TRACE(_logging_, "Entering master particle mode...");
        genbb::primary_particle * master_particle = 0;
        // Determine the master particle:
        for (int i = 0; i < (int) candidate_particles.size(); i++) {
          if (_master_particle_rank_ >= 0 && _master_particle_rank_ != ANY_PARTICLE_RANK) {
            if (_master_particle_rank_ < (int) candidate_particles.size()) {
              master_particle = candidate_particles[_master_particle_rank_];
            }
          }
        }
        if (! master_particle) {
          // Could not find the master particle with given criteria:
          event_.grab_auxiliaries().store(biased_event_status_key(), killed_event_label());
          bi_.set_status(BES_KILLED);
          _stats_.killed_counts++;
          DT_LOG_TRACE(_logging_, "No master particle was found!");
          DT_LOG_TRACE_EXITING(_logging_);
          return;
        }
        bool checked = true;

        // Check master momentum versus attractive/repulsive PoIs...
        checked = _validate_particle_direction(*master_particle, vertex_);
        if (! checked) {
          event_.grab_auxiliaries().store(biased_event_status_key(), killed_event_label());
          bi_.set_status(BES_KILLED);
          _stats_.killed_counts++;
          DT_LOG_TRACE(_logging_, "Invalid master particle direction!");
          DT_LOG_TRACE_EXITING(_logging_);
          return;
        } else {
          DT_LOG_TRACE(_logging_, "The master particle has a valid direction w.r.t. the distribution of PoIs");
          master_particle->grab_auxiliaries().store_flag(master_particle_flag());
          if (_track_only_master_particle_) {
            int not_tracked_counter = 0;
            // Tag the non-master particles not to be tracked:
            for (genbb::primary_event::particles_col_type::iterator i =
                   event_.grab_particles().begin();
                 i != event_.grab_particles().end();
                 i++) {
              genbb::primary_particle & part = *i;
              if (&part != master_particle) {
                part.grab_auxiliaries().store_flag(dont_track_this_particle_flag());
                not_tracked_counter++;
              }
            }
            if (not_tracked_counter > 0) {
              event_.grab_auxiliaries().store(biased_event_status_key(), truncated_event_label());
              bi_.set_status(BES_TRUNCATED);
              _stats_.truncated_counts++;
              DT_LOG_TRACE(_logging_, "Validation of the event (truncated).");
            } else {
              event_.grab_auxiliaries().store(biased_event_status_key(), normal_event_label());
              bi_.set_status(BES_NORMAL);
              _stats_.normal_counts++;
              DT_LOG_TRACE(_logging_, "Validation of the event.");
            }
            DT_LOG_TRACE_EXITING(_logging_);
            return;
          } else {
            event_.grab_auxiliaries().store(biased_event_status_key(), normal_event_label());
            bi_.set_status(BES_NORMAL);
            _stats_.normal_counts++;
            DT_LOG_TRACE(_logging_, "Validation of the event.");
            DT_LOG_TRACE_EXITING(_logging_);
            return;
          }
        }
        DT_LOG_TRACE(_logging_, "Exiting master particle mode.");
      } // is_using_master_particle()

      /*
      // Apply criteria on the full set of candidate particles:
      if (is_using_all_particles()) {
        DT_LOG_TRACE(_logging_, "Entering all particles mode...");
        bool checked = true;
        for (int i = 0; i < (int) candidate_particles.size(); i++) {
          const genbb::primary_particle & part = *candidate_particles[i];

          // Check momentum versus attractive/repulsive PoIs...

        }

        if (checked) {
          event_.grab_auxiliaries().store(biased_event_status_key(), normal_event_label());
          bi_.set_status(BES_NORMAL);
          _stats_.normal_counts++;
          return;
        }

        DT_LOG_TRACE(_logging_, "Exiting all particles mode.");
      } // is_using_all_particles()
      */

      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    void primary_event_bias::draw(const geomtools::vector_3d & vertex_,
                                  genbb::primary_event & event_) const
    {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      datatools::temp_file ftmp;
      ftmp.set_remove_at_destroy(true);
      ftmp.create("/tmp", "temp_");
      geomtools::rotation_3d irot;
      geomtools::create_rotation(irot, 0,0,0);

      int plot_index = 0;
      mygsl::min_max xmm;
      mygsl::min_max ymm;
      mygsl::min_max zmm;
      mygsl::min_max rmm;
      xmm.add(vertex_.x());
      ymm.add(vertex_.y());
      zmm.add(vertex_.z());

      std::map<std::string, int> plot_indexes;
      {
        int count = 0;
        for (poi_dict_type::const_iterator i = _pois_.begin();
             i != _pois_.end();
             i++) {
          const poi_entry_type & pe = i->second;
          xmm.add(pe.poi.get_position().x());
          ymm.add(pe.poi.get_position().y());
          zmm.add(pe.poi.get_position().z());
          rmm.add(pe.poi.get_radius());
          if (pe.poi.is_attractive()) {
            if (count == 0) {
              ftmp.out() << "#@Attractive_PoIs:" << std::endl;
            }
            geomtools::gnuplot_draw::draw_sphere(ftmp.out(), pe.poi.get_position(), irot, pe.poi.get_radius());
            count++;
          }
        }
        if (count) {
          plot_indexes["Attractive_PoIs"] = plot_index++;
        }
        ftmp.out() << std::endl;
      }

      {
        int count = 0;
        for (poi_dict_type::const_iterator i = _pois_.begin();
             i != _pois_.end();
             i++) {
          const poi_entry_type & pe = i->second;
          if (pe.poi.is_repulsive()) {
            if (count == 0) {
              ftmp.out() << "#@Repulsive_PoIs:" << std::endl;
            }
            geomtools::gnuplot_draw::draw_sphere(ftmp.out(), pe.poi.get_position(), irot, pe.poi.get_radius());
            count++;
          }
        }
        if (count) {
          plot_indexes["Repulsive_PoIs"] = plot_index++;
        }
        ftmp.out() << std::endl;
      }

      {
        int count = 0;
        if (geomtools::is_valid(vertex_)) {
          if (count == 0) {
            ftmp.out() << "#@Vertex:" << std::endl;
          }
          geomtools::gnuplot_draw::basic_draw_point(ftmp.out(), vertex_);
          count++;
        } else {
          for (genbb::primary_event::particles_col_type::const_iterator i =
                 event_.grab_particles().begin();
               i != event_.grab_particles().end();
               i++) {
            const genbb::primary_particle & part = *i;
            if (part.has_vertex()) {
              xmm.add(part.get_vertex().x());
              ymm.add(part.get_vertex().y());
              zmm.add(part.get_vertex().z());
              if (count == 0) {
                ftmp.out() << "#@Vertex:" << std::endl;
              }
              geomtools::gnuplot_draw::basic_draw_point(ftmp.out(), part.get_vertex());
              count++;
            }
          }
        }
        if (count) {
          plot_indexes["Vertex"] = plot_index++;
        }
        ftmp.out() << std::endl;
        ftmp.out() << std::endl;
      }

      double dx = xmm.get_max() - xmm.get_min();
      double dy = ymm.get_max() - ymm.get_min();
      double dz = zmm.get_max() - zmm.get_min();
      mygsl::min_max smm;
      smm.add(dx);
      smm.add(dy);
      smm.add(dz);
      double scale = smm.get_max();
      DT_LOG_TRACE(_logging_, "Plot scale = " << scale / CLHEP::mm << " mm");

      {
        int count = 0;
        for (genbb::primary_event::particles_col_type::const_iterator i =
               event_.grab_particles().begin();
             i != event_.grab_particles().end();
             i++) {
          const genbb::primary_particle & part = *i;
          geomtools::vector_3d vtx = vertex_;
          if (! geomtools::is_valid(vtx)) {
            if (part.has_vertex()) vtx = part.get_vertex();
          }
          if (geomtools::is_valid(vtx)) {
            if (count == 0) {
              ftmp.out() << "#@Primaries:" << std::endl;
            }
            geomtools::vector_3d dir = part.get_momentum().unit();
            geomtools::vector_3d end_traj = vtx + dir * 2 * scale;
            geomtools::gnuplot_draw::draw_line(ftmp.out(), vtx, end_traj);
            count++;
          }
        }
        if (count) {
          plot_indexes["Primaries"] = plot_index++;
        }
        ftmp.out() << std::endl;
      }

      DT_LOG_TRACE(_logging_, "Plot indexes: ");
      for (std::map<std::string, int>::const_iterator i = plot_indexes.begin();
           i != plot_indexes.end();
           i++) {
        DT_LOG_TRACE(_logging_, "  ==> Plot '" << i->first << "' at index " <<  i->second);
      }
      DT_LOG_TRACE(_logging_, "");

      Gnuplot g1;
      g1.cmd("set title 'mctools::biasing::primary_event_bias::draw' ");
      g1.cmd("set grid");
      g1.cmd("set size ratio -1");
      g1.cmd("set view equal xyz");
      g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");
      DT_LOG_TRACE(_logging_, "  X min = " << xmm.get_min() / CLHEP::mm << " mm");
      DT_LOG_TRACE(_logging_, "  X max = " << xmm.get_max() / CLHEP::mm << " mm");
      DT_LOG_TRACE(_logging_, "  Y min = " << ymm.get_min() / CLHEP::mm << " mm");
      DT_LOG_TRACE(_logging_, "  Y max = " << ymm.get_max() / CLHEP::mm << " mm");
      DT_LOG_TRACE(_logging_, "  Z min = " << zmm.get_min() / CLHEP::mm << " mm");
      DT_LOG_TRACE(_logging_, "  Z max = " << zmm.get_max() / CLHEP::mm << " mm");
      DT_LOG_TRACE(_logging_, "  R max = " << rmm.get_max() / CLHEP::mm << " mm");
      double safe =  1.5 * rmm.get_max();
      g1.set_xrange(xmm.get_min() - safe, xmm.get_max() + safe);
      g1.set_yrange(ymm.get_min() - safe, ymm.get_max() + safe);
      g1.set_zrange(zmm.get_min() - safe, zmm.get_max() + safe);
      {
        std::ostringstream plot_cmd;
        for (std::map<std::string, int>::const_iterator i = plot_indexes.begin();
            i != plot_indexes.end();
            i++) {
          DT_LOG_TRACE(_logging_, "Plot '" << i->first << "' at index " <<  i->second);
          if (i == plot_indexes.begin()) {
            plot_cmd << "splot ";
          } else {
            plot_cmd << ", ";
          }
          plot_cmd << " '" << ftmp.get_filename() << "' index " << i->second
                   << " title '" << i->first << "' with ";
          if (i->first == "Vertex") {
            plot_cmd << "point lt 5 pt 6 ps 1.0 ";
          } else if (i->first == "Primaries") {
            plot_cmd << "lines lt 3 lw 1.5 ";
          } else if (i->first == "Attractive_PoIs") {
            plot_cmd << "lines lt 2 lw 0.5 ";
          } else if (i->first == "Repulsive_PoIs") {
            plot_cmd << "lines lt 1 lw 0.5 ";
          } else {
            plot_cmd << "lines lt 0 ";
          }
        } // for
        DT_LOG_TRACE(_logging_, "plot_cmd = '" << plot_cmd.str() << "'");
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      return;
    }

    void primary_event_bias::tree_dump(std::ostream & out_,
                                       const std::string & title_,
                                       const std::string & indent_,
                                       bool inherit_) const
    {
      if (! title_.empty()) {
        out_ << indent_ << title_ << std::endl;
      }

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Initialized : " << _initialized_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Logging : " << _logging_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Geometry manager : " << _geom_mgr_ << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Mapping plugin name : '" << _mapping_name_ << "'" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Bias mode : '" << _bias_mode_ << "'" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Particle types : " << _particle_types_.size() << std::endl;
      for (std::set<int>::const_iterator i = _particle_types_.begin();
          i != _particle_types_.end();
          i++) {
        std::set<int>::const_iterator j = i;
        out_ << indent_ << datatools::i_tree_dumpable::skip_tag;
        if (++j == _particle_types_.end()) {
          out_ << datatools::i_tree_dumpable::last_tag;
        } else {
          out_ << datatools::i_tree_dumpable::tag;
        }
        out_ << "Type : " << *i << " : '" << genbb::primary_particle::particle_label_from_type(*i) << "'";
        out_ << std::endl;
      }
      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Particle min energy : "
           << _particle_min_energy_ / CLHEP::keV
           << " keV" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Particle max energy : "
           << _particle_max_energy_ / CLHEP::keV
           << " keV" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Total min energy : "
           << _total_min_energy_ / CLHEP::keV
           << " keV" << std::endl;

      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Total max energy : "
           << _total_max_energy_ / CLHEP::keV
           << " keV" << std::endl;

      if (is_using_master_particle()) {

        out_ << indent_ << datatools::i_tree_dumpable::tag
             << "Master particle rank : " << _master_particle_rank_ << std::endl;

        out_ << indent_ << datatools::i_tree_dumpable::tag
             << "Track only master particle : " << _track_only_master_particle_ << std::endl;
      }

      if (is_using_all_particles()) {
        // XXX
      }

      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Points of interest : "
           << _pois_.size() << std::endl;
      for (poi_dict_type::const_iterator i = _pois_.begin();
           i != _pois_.end();
           i++) {
        poi_dict_type::const_iterator j = i;
        out_ << indent_ << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
        if (++j == _pois_.end()) {
          out_ << datatools::i_tree_dumpable::last_tag;
        } else {
          out_ << datatools::i_tree_dumpable::tag;
        }
        out_ << "POI '" << i->first << "'";
        out_ << std::endl;
      }

      return;
    }

  } // namespace biasing

} // namespace mctools

/** Opening macro for implementation
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(mctools::biasing::primary_event_bias,ocd_)
{
  // The class name :
  ocd_.set_class_name("mctools::biasing::primary_event_bias");

  // The class terse description :
  ocd_.set_class_description("The primary event generation bias");

  // The library the class belongs to :
  ocd_.set_class_library("mctools");

  // The class detailed documentation :
  ocd_.set_class_documentation("This is a biasing algorithm used at primary event generation. \n");


  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Typical configuration is::                                             \n"
                               "                                                                       \n"
                               " #@description Set the action logging priority                         \n"
                               " logging.priority : string = \"fatal\"                                 \n"
                               "                                                                       \n"
                               );

  ocd_.set_validation_support(false);

  // Lock the description:
  ocd_.lock();

  // ... and we are done.
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation

// Registration macro :
DOCD_CLASS_SYSTEM_REGISTRATION(mctools::biasing::primary_event_bias,
                               "mctools::biasing::primary_event_bias")
