// primary_event_bias.cc

// Ourselves:
#include <mctools/biasing/primary_event_bias.h>

// Standard library:

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>
// - Bayeux/geomtools:
#include <geomtools/manager.h>
// - Bayeux/genbb_help:
#include <genbb_help/primary_event.h>
#include <genbb_help/primary_particle.h>

// // This project:
// #include <mctools/XXX.h>

namespace mctools {

  namespace biasing {

    // static
    const std::string & primary_event_bias::kill_this_event_flag()
    {
      static const std::string _f("biasing.kill_this_event");
      return _f;
    }

    // static
    const std::string & primary_event_bias::master_particle_flag()
    {
      static const std::string _f("biasing.master_particle");
      return _f;
    }

    // static
    const std::string & primary_event_bias::dont_track_this_particle_flag()
    {
      static const std::string _f("biasing.dont_track_this_particle");
      return _f;
    }

    primary_event_bias::biasing_info::biasing_info()
    {
      _status_ = BES_UNDEFINED;
      datatools::invalidate(_weight_);
      return;
    }

    void primary_event_bias::biasing_info::reset()
    {
      _status_ = BES_UNDEFINED;
      datatools::invalidate(_weight_);
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

    bool primary_event_bias::biasing_info::is_unbiased() const
    {
      return _status_ == BES_NORMAL;
    }

    bool primary_event_bias::biasing_info::is_biased() const
    {
      return _status_ != BES_NORMAL;
    }

    bool primary_event_bias::biasing_info::is_killed() const
    {
      return _status_ == BES_KILLED;
    }

    bool primary_event_bias::biasing_info::is_weighted() const
    {
      return _status_ == BES_WEIGHTED;
    }

    void primary_event_bias::biasing_info::set_weight(double w_)
    {
      if (! datatools::is_valid(w_)) {
        datatools::invalidate(_weight_);
        _status_ = BES_UNDEFINED;
      } else {
        DT_THROW_IF(w_ < 0.0, std::range_error, "Invalid biasing weight!");
        _weight_ = w_;
        _status_ = BES_WEIGHTED;
      }
      return;
    }

    double primary_event_bias::biasing_info::get_weight() const
    {
      return _weight_;
    }

    void primary_event_bias::_set_default()
    {
      _logging_ = datatools::logger::PRIO_FATAL;
      _geom_mgr_ = 0;
      _bias_mode_ = BIAS_NONE;
      _master_particle_type_ = genbb::primary_particle::PARTICLE_UNKNOWN;
      datatools::invalidate(_master_particle_min_energy_);
      _master_particle_rank_ = -1;
      _master_particle_track_only_ = false;
      datatools::invalidate(_total_min_energy_);
      datatools::invalidate(_total_max_energy_);
      return;
    }

    primary_event_bias::primary_event_bias()
    {
      _initialized_ = false;
      _set_default();
      return;
    }

    primary_event_bias::~primary_event_bias()
    {
      if (is_initialized()) {
        reset();
      }
      return;
    }

    void primary_event_bias::set_geometry_manager(const geomtools::manager & gm_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Operation prohibited ! Object is already initialized !");
      _geom_mgr_ = &gm_;
      return;
    }

    bool primary_event_bias::is_initialized() const
    {
      return _initialized_;
    }

    void primary_event_bias::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Operation prohibited ! Object is already initialized !");

      {
        datatools::logger::priority p =
          datatools::logger::extract_logging_configuration(config_,
                                                           datatools::logger::PRIO_FATAL,
                                                           true);
        if (p > datatools::logger::PRIO_UNDEFINED) {
          set_logging(p);
        }
      }

      double default_energy_unit = CLHEP::keV;

      if (config_.has_key("mode")) {
        const std::string & bm_label = config_.fetch_string("mode");
        if (bm_label == "none") {
          set_bias_mode(BIAS_NONE);
        } else if (bm_label == "master") {
          set_bias_mode(BIAS_ONLY_MASTER_PARTICLE);
        } else if (bm_label == "all") {
          set_bias_mode(BIAS_ALL_PARTICLES);
        } else {
          DT_THROW(std::logic_error, "Invalid bias mode '" << bm_label << "'!");
        }
      }

      if (_bias_mode_ == BIAS_UNDEFINED) {
        set_bias_mode(BIAS_NONE);
      }

      if (is_using_master_particle()) {

        if (config_.has_key("master_particle.type")) {
          std::string label = config_.fetch_string("master_particle.type");
          set_master_particle_by_label(label);
        }

        if (config_.has_key("master_particle.min_energy")) {
          double min_ke = config_.fetch_real("master_particle.min_energy");
          if (! config_.has_explicit_unit("master_particle.min_energy")) {
            min_ke *= default_energy_unit;
          }
          set_master_particle_minimum_energy(min_ke);
        }

        if (config_.has_key("master_particle.rank")) {
          int rank = config_.fetch_integer("master_particle.rank");
          set_master_particle_rank(rank);
        }

        if (config_.has_key("master_particle.track_only")) {
          set_master_particle_track_only(config_.fetch_boolean("master_particle.track_only"));
        }

        // Force master particle rank:
        if (_master_particle_rank_ < 0) {
          _master_particle_rank_ = 0;
        }

      } // is_using_master_particle(

      if (is_using_all_particles()) {

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

        if (! datatools::is_valid(_total_min_energy_)) {
          _total_min_energy_ = 0.0;
        }

        if (datatools::is_valid(_total_max_energy_)) {
          DT_THROW_IF(_total_max_energy_ < _total_min_energy_,
                      std::domain_error,
                      "Invalid min/max total kinetic energy!");
        }

      } // is_using_all_particles()

      _initialized_ = true;
      return;
    }

    void primary_event_bias::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Operation prohibited ! Object is not initialized !");
      _initialized_ = false;
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

    void primary_event_bias::set_master_particle_by_label(const std::string & label_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Operation prohibited ! Object is already initialized !");
      int type = genbb::primary_particle::particle_type_from_label(label_);
      DT_THROW_IF(type == genbb::primary_particle::PARTICLE_UNKNOWN,
                  std::logic_error,
                  "Unknown particle label '" << label_ << "'!");
      _master_particle_type_ = type;
      return;
    }

    void primary_event_bias::set_master_particle_by_type(int type_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Operation prohibited ! Object is already initialized !");
      std::string label = genbb::primary_particle::particle_label_from_type(type_);
      DT_THROW_IF(label.empty(),
                  std::logic_error,
                  "Unknown particle type '" << type_ << "'!");
      _master_particle_type_ = type_;
      return;
    }

    void primary_event_bias::set_master_particle_by_pdg_code(int /*code_*/)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Operation prohibited ! Object is already initialized !");
      DT_THROW(std::logic_error, "Particle PDG code not supported yet!");
      _master_particle_type_ = genbb::primary_particle::PARTICLE_UNKNOWN;
      return;
    }

    void primary_event_bias::set_master_particle_minimum_energy(double kemin_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Operation prohibited ! Object is already initialized !");
      DT_THROW_IF(kemin_ < 0.0, std::range_error,
                  "Invalid minimum kinetic energy !");
      _master_particle_min_energy_ = kemin_;
      return;
    }

    void primary_event_bias::set_master_particle_rank(int rank_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Operation prohibited ! Object is already initialized !");
      if (rank_ < -1) {
        _master_particle_rank_ = -1;
      } else {
        _master_particle_rank_ = rank_;
      }
      return;
    }

    void primary_event_bias::set_master_particle_track_only(bool to_)
    {
      _master_particle_track_only_ = to_;
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

    /*
      void primary_event_bias::add_target(const std::string & target_name_,
      const geomtools::vector_3 & p_,
      double radius_,
      bool avoid_)
      {
      // {
      //        target_type tt;
      //        _targets_.push_back(tt);
      // }
      // target_type & tgt = _targets_.
      return;
      }
    */

    genbb::primary_particle * primary_event_bias::_select_master_particle(genbb::primary_event & event_) const
    {
      genbb::primary_particle * master_particle = 0;
      std::vector<genbb::primary_particle *> master_candidates;
      for (genbb::primary_event::particles_col_type::iterator i =
             event_.grab_particles().begin();
           i != event_.grab_particles().end();
           i++) {
        genbb::primary_particle & part = *i;
        bool candidate = true;

        // Check particle type:
        if (candidate && _master_particle_type_ !=
            genbb::primary_particle::PARTICLE_UNKNOWN) {
          if (part.get_type() != _master_particle_type_) {
            candidate = false;
          }
        }

        // Check particle minimum kinetic energy:
        if (candidate && datatools::is_valid(_master_particle_min_energy_)) {
          if (part.get_kinetic_energy() < _master_particle_min_energy_) {
            candidate = false;
          }
        }

        if (candidate) {
          master_candidates.push_back(&part);
        }
      }

      if (_master_particle_rank_ >= 0) {
        if (_master_particle_rank_ < (int)master_candidates.size()) {
          master_particle = master_candidates[_master_particle_rank_];
        }
      }
      return master_particle;
    }

    void primary_event_bias::process(genbb::primary_event & event_, biasing_info & bi_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Object is not initialized !");

      bi_.reset();

      if (is_using_no_bias()) {
        // No biasing is applied:
        bi_.set_status(BES_NORMAL);
        bi_.set_weight(1.0);
        return;
      } // is_using_no_bias()

      // Pointer to the master particle:
      genbb::primary_particle * master_particle = 0;

      if (is_using_master_particle()) {

        // Pick-up the "master" particle:
        master_particle = _select_master_particle(event_);

        // Apply criteria only on the master particle if it exists:
        if (! master_particle) {
          event_.grab_auxiliaries().store_flag(kill_this_event_flag());
          bi_.set_status(BES_KILLED);
          return;
        } else {
          master_particle->grab_auxiliaries().store_flag(master_particle_flag());

          if (_master_particle_track_only_) {
            // Tag the non-master particles:
            for (genbb::primary_event::particles_col_type::iterator i =
                   event_.grab_particles().begin();
                 i != event_.grab_particles().end();
                 i++) {
              genbb::primary_particle & part = *i;
              if (&part != master_particle) {
                part.grab_auxiliaries().store_flag(dont_track_this_particle_flag());
              }
            }
          }
          bi_.set_status(BES_TRANSFORMED);
          return;
        }

        return;
      } // is_using_master_particle()

      // Apply criteria on the full set of particles:
      if (is_using_all_particles()) {

        // Compute the total energy:
        double kesum = 0.0;
        for (genbb::primary_event::particles_col_type::const_iterator i =
               event_.get_particles().begin();
             i != event_.get_particles().end();
             i++) {
          const genbb::primary_particle & part = *i;
          kesum += part.get_kinetic_energy();
        }

        bool ok = true;
        if (datatools::is_valid(_total_min_energy_)) {
          if (kesum < _total_min_energy_) {
            ok = false;
          }
        }
        if (datatools::is_valid(_total_max_energy_)) {
          if (kesum > _total_max_energy_) {
            ok = false;
          }
        }
        if (!ok) {
          event_.grab_auxiliaries().store_flag(kill_this_event_flag());
          bi_.set_status(BES_KILLED);
          return;
        }
      } // is_using_all_particles()

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
