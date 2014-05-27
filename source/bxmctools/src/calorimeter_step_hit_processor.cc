/** mctools/calorimeter_step_hit_processor.cc */

// Ourselves:
#include <mctools/calorimeter_step_hit_processor.h>

// Standard library:
#include <algorithm>
#include <cstdlib>

// Third party:
// - Boost:
#include <boost/algorithm/string.hpp>
// - Bayeux/datatools:
#include <datatools/ioutils.h>
#include <datatools/exception.h>

// This project:
#include <mctools/utils.h>

namespace mctools {

  MCTOOLS_STEP_HIT_PROCESSOR_REGISTRATION_IMPLEMENT (calorimeter_step_hit_processor,
                                                     "mctools::calorimeter_step_hit_processor");

  void calorimeter_step_hit_processor::set_mapping_category (const std::string & sc_)
  {
    _mapping_category_ = sc_;
    return;
  }

  const std::string & calorimeter_step_hit_processor::get_mapping_category () const
  {
    return _mapping_category_;
  }

  // Constructor:
  calorimeter_step_hit_processor::calorimeter_step_hit_processor ()
    : base_step_hit_processor ()
  {
    _scintillation_cluster_time_range_  = 1.0 * CLHEP::ns;
    _scintillation_cluster_space_range_ = 1.0 * CLHEP::cm;
    _mapping_    = 0;
    _categories_ = 0;
    _calo_block_type_ = geomtools::geom_id::INVALID_TYPE;
    _alpha_quenching_ = true;
    return;
  }

  calorimeter_step_hit_processor::~calorimeter_step_hit_processor ()
  {
    // std::cerr << "DEVEL: " << "calorimeter_step_hit_processor::DTOR: Entering..." << std::endl;
    // std::cerr << "DEVEL: " << "calorimeter_step_hit_processor::DTOR: Exiting." << std::endl;
    return;
  }

  // void calorimeter_step_hit_processor::_reset()
  // {
  //   return;
  // }

  void calorimeter_step_hit_processor::initialize(const ::datatools::properties & config_,
                                                  ::datatools::service_manager & service_mgr_)
  {
    _init(config_, service_mgr_);
    return;
  }

  void calorimeter_step_hit_processor::_init(const ::datatools::properties & config_,
                                             ::datatools::service_manager & service_mgr_)
  {
     this->base_step_hit_processor::initialize(config_, service_mgr_);

    // The geometry manager is mandatory for this processor:
    DT_THROW_IF (! has_geom_manager (), std::logic_error,
                 "Missing geometry manager for processor '" << get_name () << "' !");

    if (config_.has_key ("mapping.category")) {
      std::string geom_cat = config_.fetch_string ("mapping.category");
      set_mapping_category (geom_cat);
    }

    std::vector<std::string> any_addresses_labels;
    if (config_.has_key ("mapping.category.any_addresses")) {
      config_.fetch ("mapping.category.any_addresses", any_addresses_labels);
    }

    // A sensitive category is mandatory for this processor,
    // i.e. the one defined for the target calorimeter blocks:
    DT_THROW_IF (_mapping_category_.empty (), std::logic_error,
                 "Missing 'mapping_category' string property for processor '" << get_name () << "' !");

    // set the time tolerance for hit clusterization:
    if (config_.has_key ("cluster.time_range")) {
      _scintillation_cluster_time_range_ = config_.fetch_real ("cluster.time_range");
      if (! config_.has_explicit_unit("cluster.time_range")) {
        _scintillation_cluster_time_range_ *= CLHEP::ns;
      }
    }

    // set the space tolerance for hit clusterization:
    if (config_.has_key ("cluster.space_range")) {
      _scintillation_cluster_space_range_ = config_.fetch_real ("cluster.space_range");
      if (! config_.has_explicit_unit("cluster.space_range")) {
        _scintillation_cluster_space_range_ *= CLHEP::mm;
      }
    }

    // set the flag for alpha quenching:
    if (config_.has_key ("alpha_quenching")) {
      _alpha_quenching_ = config_.fetch_boolean ("alpha_quenching");
    }

    DT_LOG_DEBUG (get_logging_priority (), "Parsed setup properties for processor '" << get_name () << "' ...");

    // pickup the ID mapping from the geometry manager:
    _mapping_ = &_geom_manager->get_mapping ();

    // check if the sensitive category is known:
    _categories_ = &(_geom_manager->get_id_mgr ().categories_by_name ());
    geomtools::id_mgr::categories_by_name_col_type::const_iterator icat
      = _categories_->find (_mapping_category_);
    DT_THROW_IF (icat == _categories_->end (), std::logic_error,
                 "Cannot find geometry ID category '"
                 << _mapping_category_ << "' string property for processor '"
                 << get_name () << "' !");

    const geomtools::id_mgr::category_info & mapping_cat_info = icat->second;

    // Initialize the smart ID locator for this category of volumes:
    _calo_block_type_ = mapping_cat_info.get_type ();
    _calo_block_locator_.set_gmap (*_mapping_);
    _calo_block_locator_.initialize (_calo_block_type_);

    for (size_t i = 0; i < any_addresses_labels.size (); i++) {
      std::vector<std::string> strs;
      boost::split (strs, any_addresses_labels[i], boost::is_any_of("@"));
      DT_THROW_IF (strs.size () > 2, std::logic_error,
                   "Invalid format for 'mapping.category.any_addresses' property ' from "
                   << any_addresses_labels[i] << "' !");
      const std::string & label = strs[0];
      int label_count = 1;
      if (strs.size () == 2) {
        const std::string & count_str = strs[1];
        std::istringstream iss (count_str);
        iss >> label_count;
        DT_THROW_IF (! iss, std::logic_error,
                     "Invalid format for 'mapping.category.any_addresses' ! "
                     << "Cannot parse subaddress label count from "
                     << count_str << "' !");
      }
      DT_THROW_IF ((! mapping_cat_info.has_subaddress (label, label_count)),
                   std::logic_error,
                   "Category '" << mapping_cat_info.get_category ()
                   << "' has no subaddress labelled '" << label << "' !");
      _mapping_category_any_addresses_.push_back (mapping_cat_info.get_subaddress_index (label, label_count));
    }
    if (_mapping_category_any_addresses_.size ()) {
      std::sort (_mapping_category_any_addresses_.begin (),
                 _mapping_category_any_addresses_.end ());
    }

    return;
  }

  const std::vector<int> & calorimeter_step_hit_processor::get_mapping_category_any_addresses () const
  {
    return _mapping_category_any_addresses_;
  }

  bool calorimeter_step_hit_processor::locate_calorimeter_block(const geomtools::vector_3d & position_,
                                                                geomtools::geom_id & gid_) const
  {
    gid_ = _calo_block_locator_.get_geom_id(position_, _calo_block_type_);
    return gid_.is_valid();
  }

  /** Check if a 'step hit' could be aggregated to an existing 'scintillation hit'.
   *  Any new step hit must:
   * - belong to the same geometry element than the scintillation hit (geom ID location)
   * - be an alpha or a delta ray if it is a scintillation hit with an alpha
   *   in order to be able to collect the ionization energy deposited
   *   along an alpha track and then apply some realistic quenching factor
   * - be in time with the cluster time window @ requested precision (~1 ns)
   * - be located within the volume of a fiducial sphere centered around
   *   the existing cluster hit @ requested precision (~1 cm)
   *
   */
  bool calorimeter_step_hit_processor::match_scintillation_hit (const base_step_hit & scintillation_hit_,
                                                                const base_step_hit & step_hit_) const
  {
    // Check if the hits are in the same volume (same [effective] geometry ID):
    /*
      if (_mapping_category_any_addresses.size ())
      {
      // Some subaddresses of the geometry category are marked as 'any' :
      geomtools::geom_id gid1 = scintillation_hit_.get_geom_id ();
      geomtools::geom_id gid2 = step_hit_.get_geom_id ();
      for (int i = 0; i < _mapping_category_any_addresses.size (); i++)
      {
      gid1.set_any (_mapping_category_any_addresses[i]);
      gid2.set_any (_mapping_category_any_addresses[i]);
      }
      if (! geomtools::geom_id::match (gid1, gid2, false))
      {
      return false;
      }
      }
      else
      {
      if (! geomtools::geom_id::match (scintillation_hit_.get_geom_id (), step_hit_.get_geom_id (), true))
      {
      return false;
      }
      }
    */
    if (! geomtools::geom_id::match (scintillation_hit_.get_geom_id (), step_hit_.get_geom_id (), false)) {
      return false;
    }

    // check the case of alpha particle:
    const std::string & hit_pname = step_hit_.get_particle_name ();

    // Default value:
    bool track_match = true;

    if (_alpha_quenching_) {
      // Check the special case of an alpha scintillation hit:
      track_match = false;
      if (scintillation_hit_.get_particle_name () == "alpha") {
        if (hit_pname == "e-") {
          // search for delta rays originated from an alpha track:
          const bool is_delta_ray_from_alpha
            = step_hit_.get_auxiliaries ().has_flag(mctools::track_utils::DELTA_RAY_FROM_ALPHA_FLAG);
          if (is_delta_ray_from_alpha) {
            /*  delta ray production along the alpha track:
             *
             *       e-   e-     e-
             *      /    /      /
             *  ---+----+-+----+---> alpha
             *                                  \
             *              e-
             *
             *  the resulting ionization of the primary alpha
             *  plus its associated delta rays has some quenching effect: all these step
             *  hits should share the same scintillation cluster hit.
             */
            track_match = true;
          }
        } else if (hit_pname == "alpha") {
          track_match = true;
          /** pile-up of different alpha particles within the same scintillator
           *  volume is not supported, despite it should lead to
           *  two coexisting quenching effects because in principle:
           *
           *     1 MeV alpha + 1 MeV alpha != 2 MeV alpha
           *
           *  Anyway such event is out the scope of the SN physics...
           *
           * Note: to support this effect, we should use the track ID of the scintillation hit
           * and compare it to the track ID of the step hit
           *
           *            / alpha 1
           *           /
           *  --------+------ alpha 2
           *         /
           *        /
           *
           * Both alpha track have their own quenching effect and should not share
           * the same cluster.
           * Special treatment would be needed for delta rays too. Oh my !
           */
        }
        else {
          track_match = false;
        }
      }
      else {
        // e-/e+/gamma
        if (hit_pname == "e-") {
          // reject delta rays originated from an alpha track for they should be clusterized
          // within the scintillation cluster of an alpha particle:
          const bool is_delta_ray_from_alpha
            = step_hit_.get_auxiliaries ().has_flag (mctools::track_utils::DELTA_RAY_FROM_ALPHA_FLAG);
          if (is_delta_ray_from_alpha) {
            track_match = false;
          }
          else {
            track_match = true;
          }
        }
        else if (hit_pname == "alpha") {
          track_match = false;
        }
        else {
          track_match = true;
        }
      }
    }

    // no particle type match:
    if (! track_match) return false;

    /** check the time:
     *
     *           t1                 t2
     *    --------+-----------------+--------------------------> cluster time
     *
     *    ------------------------+-----+----------------------> hit time
     *                           ta     tb
     *
     *   for gamma
     */
    const double t1 = scintillation_hit_.get_time_start() - _scintillation_cluster_time_range_;
    const double t2 = scintillation_hit_.get_time_stop() + _scintillation_cluster_time_range_;
    double ta = step_hit_.get_time_start();
    if (hit_pname == "gamma") {
      ta = step_hit_.get_time_stop();
    }
    const double tb = step_hit_.get_time_stop();
    bool time_match = false;
    if ((ta > t1) && (ta < t2)) time_match = true;
    else if ((tb > t1) && (tb < t2)) time_match = true;
    else if ((t1 > ta) && (t1 < tb)) time_match = true;
    else if ((t2 > ta) && (t2 < tb)) time_match = true;

    // no timing match:
    if (! time_match) return false;

    /* check the position:
     *  a matching step hit has to be located in a fiducial sphere
     *  that wraps the scintillation cluster hit.
     */
    const double cluster_dx = std::abs (scintillation_hit_.get_position_stop ().x () - scintillation_hit_.get_position_start ().x ());
    const double cluster_dy = std::abs (scintillation_hit_.get_position_stop ().y () - scintillation_hit_.get_position_start ().y ());
    const double cluster_dz = std::abs (scintillation_hit_.get_position_stop ().z () - scintillation_hit_.get_position_start ().z ());
    const double cluster_radius = 0.5 * std::sqrt (cluster_dx * cluster_dx + cluster_dy * cluster_dy + cluster_dz * cluster_dz);

    // define the cluster fiducial spherical region:
    geomtools::sphere cluster_sphere;
    double factor = 1.0;
    if (cluster_radius < _scintillation_cluster_space_range_) {
      factor = 2.0;
    }
    cluster_sphere.set_radius (cluster_radius + factor * _scintillation_cluster_space_range_);

    const double cluster_x = 0.5 * (scintillation_hit_.get_position_stop ().x () + scintillation_hit_.get_position_start ().x ());
    const double cluster_y = 0.5 * (scintillation_hit_.get_position_stop ().y () + scintillation_hit_.get_position_start ().y ());
    const double cluster_z = 0.5 * (scintillation_hit_.get_position_stop ().z () + scintillation_hit_.get_position_start ().z ());
    geomtools::vector_3d cluster_center;
    cluster_center.set (cluster_x, cluster_y, cluster_z);

    bool pos_match = false;
    if (hit_pname == "gamma") {
      // only check the interaction point at end of the step:
      if (cluster_sphere.is_inside(step_hit_.get_position_stop () - cluster_center)) {
        pos_match = true;
      }
    } else {
      // both start and stop of the step are checked:
      if (cluster_sphere.is_inside (step_hit_.get_position_start () - cluster_center)) {
        pos_match = true;
      } else if (cluster_sphere.is_inside (step_hit_.get_position_stop () - cluster_center)) {
        pos_match = true;
      }
    }

    // no spatial match:
    if (! pos_match) return false;

    return true;
  }

  void calorimeter_step_hit_processor::process(
    const ::mctools::base_step_hit_processor::step_hit_ptr_collection_type & the_base_step_hits,
    ::mctools::simulated_data::hit_collection_type & the_plain_calo_hits)
  {
    _process(the_base_step_hits, (simulated_data::hit_handle_collection_type *) 0, &the_plain_calo_hits);
    return;
  }

  void calorimeter_step_hit_processor::process(
    const ::mctools::base_step_hit_processor::step_hit_ptr_collection_type & the_base_step_hits,
    ::mctools::simulated_data::hit_handle_collection_type & the_calo_hits)
  {
    _process(the_base_step_hits, &the_calo_hits, (simulated_data::hit_collection_type *) 0);
    return;
  }

  void calorimeter_step_hit_processor::_process(const base_step_hit_processor::step_hit_ptr_collection_type & shpc_,
                                                simulated_data::hit_handle_collection_type * scintillation_hits_,
                                                simulated_data::hit_collection_type        * plain_scintillation_hits_)
  {
    DT_LOG_TRACE (get_logging_priority (), "Entering...");
    // Check the type of output collection (handles or plain hits) :
    bool use_handles = false;
    if (scintillation_hits_ != 0) {
      use_handles = true;
    } else if (plain_scintillation_hits_ == 0) {
      DT_THROW_IF(true, std::logic_error, "Missing hit collection type (NULL pointer) !");
    }
    base_step_hit * current_scintillation_cluster = 0;
    uint32_t scintillation_hit_count = 0;

    // Prereservation :
    if (use_handles) {
      scintillation_hits_->reserve (20);
    }
    else {
      plain_scintillation_hits_->reserve (20);
    }
    for (base_step_hit_processor::step_hit_ptr_collection_type::const_iterator ihit = shpc_.begin ();
         ihit != shpc_.end ();
         ihit++) {
      //const base_step_hit & the_step_hit = *(*ihit);
      base_step_hit & the_step_hit = const_cast<base_step_hit &> (*(*ihit));

      double                       hit_time_start     = the_step_hit.get_time_start ();
      double                       hit_time_stop      = the_step_hit.get_time_stop ();
      const geomtools::vector_3d & hit_position_start = the_step_hit.get_position_start ();
      const geomtools::vector_3d & hit_position_stop  = the_step_hit.get_position_stop ();
      geomtools::vector_3d         hit_position_mean  = 0.5 * (hit_position_start + hit_position_stop);
      const std::string &          hit_particle_name  = the_step_hit.get_particle_name ();
      double                       hit_energy_deposit = the_step_hit.get_energy_deposit ();

      // skip sterile hits:
      if (hit_energy_deposit < 1.e-10 * CLHEP::keV) continue;

      // locate the hit using the mean position through the smart locator:
      geomtools::geom_id gid;
      locate_calorimeter_block(hit_position_mean, gid);
      if (! gid.is_valid ()) {
        // 2014-04-23, FM: Development/debugging:
        /*
        if (get_logging_priority () >= datatools::logger::PRIO_WARNING) {
          the_step_hit.tree_dump(std::clog, "Current step hit: ", "WARNING: ");
        }
        DT_LOG_WARNING (get_logging_priority (), "Calo block locator : " );
        _calo_block_locator_.dump(std::clog);
        DT_LOG_WARNING (get_logging_priority (), "Calo block type    = " << _calo_block_type_);
        DT_LOG_WARNING (get_logging_priority (), "Hit position start = " << std::setprecision(15) << hit_position_start / CLHEP::mm);
        DT_LOG_WARNING (get_logging_priority (), "Hit position stop  = " << std::setprecision(15) << hit_position_stop  / CLHEP::mm);
        DT_LOG_WARNING (get_logging_priority (), "Hit position mean  = " << std::setprecision(15) << hit_position_mean  / CLHEP::mm);
        setenv("geomtools_geom_map_check_inside_devel", "1", 1);
        geomtools::geom_id gid_start;
        locate_calorimeter_block(hit_position_start, gid_start);
        DT_LOG_WARNING (get_logging_priority (), "GID start = " << gid_start);
        geomtools::geom_id gid_stop;
        locate_calorimeter_block(hit_position_stop, gid_stop);
        unsetenv("geomtools_geom_map_check_inside_devel");
        DT_LOG_WARNING (get_logging_priority (), "GID stop  = " << gid_stop);
        */
        // we do not process such a hit:
        DT_LOG_WARNING (get_logging_priority (),
                        "We skip this hit for one cannot locate it through the locator attached to the '"
                        << get_mapping_category() << "' ! "
                        << " This may be due to a roundoff error while checking the geometry of the volume "
                        << "or to another mapping category registered in the current '"
                        << get_hit_category() << "' hit category "
                        << "that may generate its own step hits ! Consider to write your own hit processor able "
                        << "to handle several mapping categories (using several suitable locators) !");
        continue;
      }

      // Set 'any' for some GID subaddresses :
      for (size_t i = 0; i < _mapping_category_any_addresses_.size (); i++) {
        gid.set_any(_mapping_category_any_addresses_[i]);
      }
      the_step_hit.set_geom_id(gid);

      // first search match with the current cluster (if any):
      base_step_hit * matching_scintillation_cluster = 0;
      if (current_scintillation_cluster != 0) {
        if (match_scintillation_hit (*current_scintillation_cluster, the_step_hit)) {
          matching_scintillation_cluster = current_scintillation_cluster;
        }
      }
      // else: scan the whole list of clusters :
      if (matching_scintillation_cluster == 0) {
        if (use_handles) {
          for (simulated_data::hit_handle_collection_type::iterator icluster
                 = scintillation_hits_->begin ();
               icluster != scintillation_hits_->end ();
               icluster++) {
            if (! icluster->has_data ()) continue;
            base_step_hit & matching_hit = icluster->grab ();
            if (match_scintillation_hit (matching_hit, the_step_hit)) {
              // pick up the first matching cluster :
              matching_scintillation_cluster = &matching_hit;
              break;
            }
          }
        } else {
          for (simulated_data::hit_collection_type::iterator icluster
                 = plain_scintillation_hits_->begin ();
               icluster != plain_scintillation_hits_->end ();
               icluster++) {
            base_step_hit & matching_hit = *icluster;
            if (match_scintillation_hit (matching_hit, the_step_hit)) {
              // pick up the first matching cluster :
              matching_scintillation_cluster = &matching_hit;
              break;
            }
          }
        }
      }

      // if the step hit does not match any cluster:
      if (matching_scintillation_cluster == 0) {
        if (use_handles) {
          // insert a new clusterized hit:
          add_new_hit (*scintillation_hits_);
          // optimization using a reference to this last inserted hit :
          current_scintillation_cluster = &(scintillation_hits_->back ().grab ());
        } else {
          // add a new hit in the plain collection :
          base_step_hit dummy;
          plain_scintillation_hits_->push_back (dummy);
          // get a reference to the last inserted scintillation cluster :
          current_scintillation_cluster = &(plain_scintillation_hits_->back ());
        }

        // update the attributes of the cluster :
        current_scintillation_cluster->set_hit_id (scintillation_hit_count);
        current_scintillation_cluster->set_geom_id (gid);

        // store primary particle information
        const bool is_primary_particle
          = the_step_hit.get_auxiliaries ().has_flag (mctools::track_utils::PRIMARY_PARTICLE_FLAG);
        if (is_primary_particle) {
          current_scintillation_cluster->grab_auxiliaries ().store_flag (mctools::track_utils::PRIMARY_PARTICLE_FLAG);
        }

        if (hit_particle_name == "gamma") {
          // for gamma, only interaction point makes sense (ie. photoelectric effect)
          current_scintillation_cluster->set_time_start (hit_time_stop);
          current_scintillation_cluster->set_time_stop (hit_time_stop);
        } else {
          current_scintillation_cluster->set_time_start (hit_time_start);
          current_scintillation_cluster->set_time_stop (hit_time_stop);
        }
        current_scintillation_cluster->set_energy_deposit (hit_energy_deposit);
        current_scintillation_cluster->set_particle_name (hit_particle_name);

        // compute limits of the cluster bounding box:
        double xmin, ymin, zmin;
        double xmax, ymax, zmax;
        if (hit_particle_name == "gamma") {
          // for gamma, we record the interaction point at step stop:
          xmin = the_step_hit.get_position_stop ().x ();
          ymin = the_step_hit.get_position_stop ().y ();
          zmin = the_step_hit.get_position_stop ().z ();
          xmax = the_step_hit.get_position_stop ().x ();
          ymax = the_step_hit.get_position_stop ().y ();
          zmax = the_step_hit.get_position_stop ().z ();
        } else {
          // compute the bounds of the step hit:
          xmin = std::min (the_step_hit.get_position_start ().x (), the_step_hit.get_position_stop ().x ());
          ymin = std::min (the_step_hit.get_position_start ().y (), the_step_hit.get_position_stop ().y ());
          zmin = std::min (the_step_hit.get_position_start ().z (), the_step_hit.get_position_stop ().z ());
          xmax = std::max (the_step_hit.get_position_start ().x (), the_step_hit.get_position_stop ().x ());
          ymax = std::max (the_step_hit.get_position_start ().y (), the_step_hit.get_position_stop ().y ());
          zmax = std::max (the_step_hit.get_position_start ().z (), the_step_hit.get_position_stop ().z ());
        }
        geomtools::vector_3d min_pos (xmin, ymin, zmin);
        current_scintillation_cluster->set_position_start (min_pos);
        geomtools::vector_3d max_pos (xmax, ymax, zmax);
        current_scintillation_cluster->set_position_stop (max_pos);

        //increment the cluster id:
        scintillation_hit_count++;
      } else {
        // add the step hit informations in the current cluster:
        current_scintillation_cluster = matching_scintillation_cluster;

        // increment energy deposit:
        const double cluster_energy_deposit = current_scintillation_cluster->get_energy_deposit ()
          + hit_energy_deposit;
        current_scintillation_cluster->set_energy_deposit (cluster_energy_deposit);
        if (hit_particle_name == "gamma") {
          if (hit_time_stop < current_scintillation_cluster->get_time_start ()) {
            current_scintillation_cluster->set_time_start (hit_time_stop);
          }
          if (hit_time_stop > current_scintillation_cluster->get_time_stop ()) {
            current_scintillation_cluster->set_time_stop (hit_time_stop);
          }
        } else {
          if (hit_time_start < current_scintillation_cluster->get_time_start ()) {
            current_scintillation_cluster->set_time_start (hit_time_start);
          }
          if (hit_time_stop > current_scintillation_cluster->get_time_stop ()) {
            current_scintillation_cluster->set_time_stop (hit_time_stop);
          }
        }
        geomtools::vector_3d cluster_min_pos = current_scintillation_cluster->get_position_start ();
        geomtools::vector_3d cluster_max_pos = current_scintillation_cluster->get_position_stop ();
        for (int ixyz = 0; ixyz < 3; ixyz++) {
          if (hit_particle_name != "gamma") {
            if (hit_position_start[ixyz] < cluster_min_pos[ixyz]) {
              cluster_min_pos[ixyz] = hit_position_start[ixyz];
            }
            if (hit_position_start[ixyz] > cluster_max_pos[ixyz]) {
              cluster_max_pos[ixyz] = hit_position_start[ixyz];
            }
          }
          if (hit_position_stop[ixyz] < cluster_min_pos[ixyz]) {
            cluster_min_pos[ixyz] = hit_position_stop[ixyz];
          }
          if (hit_position_stop[ixyz] > cluster_max_pos[ixyz]) {
            cluster_max_pos[ixyz] = hit_position_stop[ixyz];
          }
        }
        current_scintillation_cluster->set_position_start (cluster_min_pos);
        current_scintillation_cluster->set_position_stop (cluster_max_pos);
      }

    }

    DT_LOG_TRACE (get_logging_priority (), "Exiting.");
    return;
  }

  void calorimeter_step_hit_processor::merge_scintillation_hits (simulated_data::hit_handle_collection_type & /*scintillation_hits_*/)
  {
    // pickup a reference to the proper list of hits:
    //simulated_data::hit_handle_collection_type & scintillation_hits = scintillation_hits_;

    // NOT IMPLEMENTED...

    return;
  }

  void calorimeter_step_hit_processor::tree_dump (std::ostream & out_,
                                                  const std::string & title_,
                                                  const std::string & indent_,
                                                  bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty ()) {
      indent = indent_;
    }
    base_step_hit_processor::tree_dump (out_, title_, indent, true);
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Time range : " << _scintillation_cluster_time_range_ / CLHEP::ns << " (ns)" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Space range : " << _scintillation_cluster_space_range_ / CLHEP::mm << " (mm)" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Mapping category : '" <<_mapping_category_ << "'" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Mapping    : " << _mapping_ << std::endl;
    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Categories : " << _categories_ << " ";
      if (_categories_ != 0) {
        out_ << '[' << _categories_->size () << ']';
        /*
          for (geomtools::id_mgr::categories_by_name_col_t::const_iterator i = _categories_->begin ();
          i != _categories_->end ();
          i++)
          {
          geomtools::id_mgr::categories_by_name_col_t::const_iterator j = i;
          j++;
          out_ << indent << datatools::i_tree_dumpable::skip_tag;
          if (j == _categories_->end ())
          {
          out_ << datatools::i_tree_dumpable::last_tag;
          }
          else
          {
          out_ << datatools::i_tree_dumpable::tag;
          }
          out_ << "Category '" << i->first << "'" << std::endl;
          }
        */
      }
      out_ << std::endl;
    }
    out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
         << "Calorimeter block type : " << _calo_block_type_ << std::endl;

    return;
  }

  // static
  void calorimeter_step_hit_processor::init_ocd(datatools::object_configuration_description & ocd_)
  {

    // Inherits configuration properties from its base class:
    ::mctools::base_step_hit_processor::init_ocd(ocd_);


    {
      datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
      cpd.set_name_pattern("mapping.category")
        .set_terse_description("Specify the geometry category associated to the sensitive volume")
        .set_from("mctools::base_step_hit_processor")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(true)
        .add_example("Use a specific geometry category::            \n"
                     "                                              \n"
                     "  mapping.category : string = \"hcalo.block\" \n"
                     "                                              \n"
                     )
        .set_long_description("The processor needs to know, from the geometry Id mapping of the \n"
                           "virtual geometry setup, the geometry category that is associated \n"
                           "to the attached sensitive volume. This allows the computing of   \n"
                           "the geometry Id that must be associated to the final hit.        \n")
        ;
    }


    {
      datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
      cpd.set_name_pattern("mapping.category.any_addresses")
        .set_terse_description("Specify the subaddress of the geometry category that should be ignored")
        .set_from("mctools::base_step_hit_processor")
        .set_traits(datatools::TYPE_STRING,
                    datatools::configuration_property_description::ARRAY
                    )
        .set_mandatory(false)
        .add_example("Specify the name of the geometry Id subaddress that is irrelevant\n"
                     "while computing the Gid of the clustered hit::                   \n"
                     "                                                                 \n"
                     "  mapping.category.any_addresses : string[1] = \"part\"          \n"
                     "                                                                 \n"
                     )
        .set_long_description("Not documented yet\n")
        ;
    }


    {
      datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
      cpd.set_name_pattern("cluster.time_range")
        .set_terse_description("The time window to clusterize two step hits in the same cluster")
        .set_from("mctools::base_step_hit_processor")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_default_value_real(1.0 * CLHEP::ns, "ns")
        .add_example("Use a 1 ns delay between two consecutive hits to consider        \n"
                     "then as part of the same calorimeter cluster hit (within the     \n"
                     "same physical volume)::                                          \n"
                     "                                                                 \n"
                     "  cluster.time_range : real as time = 1 ns                       \n"
                     "                                                                 \n"
                     )
        .set_long_description("Not documented yet\n")
        ;
    }

    {
      datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
      cpd.set_name_pattern("cluster.space_range")
        .set_terse_description("The space window to clusterize two step hits in the same cluster")
        .set_from("mctools::base_step_hit_processor")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_default_value_real(1.0 * CLHEP::cm, "cm")
        .add_example("Use a 1 cm distance between two neighbour hits to consider       \n"
                     "then as part of the same calorimeter cluster hit (within the     \n"
                     "same physical volume)::                                          \n"
                     "                                                                 \n"
                     "  cluster.space_range : real as length = 1 cm                    \n"
                     "                                                                 \n"
                     )
        .set_long_description("Not documented yet\n")
        ;
    }

    {
      datatools::configuration_property_description & cpd = ocd_.add_configuration_property_info();
      cpd.set_name_pattern("alpha_quenching")
        .set_terse_description("Activate special algorithm to aggregate delta rays emitted by an alpha particle to the alpha cluster hit")
        .set_from("mctools::base_step_hit_processor")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .set_default_value_boolean(true)
        .add_example("Activate the alpha quenching::                                   \n"
                     "                                                                 \n"
                     "  alpha_quenching : boolean = 1                                  \n"
                     "                                                                 \n"
                     )
        .set_long_description("When an alpha particle of a few MeV traverses some special material       \n"
                              "(example: plastic scintillator), its total energy deposit may be quenched \n"
                              "because of the high local density of energy deposit. This includes the    \n"
                              "contribution of the energy deposit by secondary delta-rays emitted by the \n"
                              "*primary* alpha particle.                                                 \n"
                              "When activated, this flag triggers a specific algorithm that is able to   \n"
                              "score the sum of energy deposit along an alpha track. This makes possible \n"
                              "to take properly into account the energy quenching effects at further     \n"
                              "digitization stage, using difference response functions for alpha particle\n"
                              "compared to electrons for example. Within a calorimeter sensitive volume, \n"
                              "a separated output clustered hit is generated, distinct of other possible \n"
                              "hits from low ionization particles such as electrons or positrons. It will be \n"
                              "then up to the digitization algorithm to apply different yields on hits from  \n"
                              "different particles and score the effective visible energy in the detector.   \n"
                              )
        ;
    }

    return;
  }

} // end of namespace mctools


// OCD support for class '::mctools::calorimeter_step_hit_processor' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::mctools::calorimeter_step_hit_processor, ocd_)
{
  ocd_.set_class_name ("mctools::calorimeter_step_hit_processor");
  ocd_.set_class_description ("A Monte Carlo step hit post-processor that scores the total energy deposit in some specific sensitive volumes considered as *calorimeters*");
  ocd_.set_class_library ("mctools");
  ocd_.set_class_documentation ("A Monte-Carlo step hit post-processor that accumulates the many     \n"
                                "microscopic Monte Carlo truth hits generated from a sensitive       \n"
                                "detector and sums up the total energy deposit in the associated     \n"
                                "volumes. The processor typically produces a single *clustered* hit  \n"
                                "per sensitive volume from the collection of small primary step hits \n"
                                "traversing the volume, thus reducing the total amount of primary    \n"
                                "physics informations but preserving some essential parameters       \n"
                                "for further post-processing (for example : digitization).           \n"
                                "The output is an object of type ``mctools::base_step_hit`` and      \n"
                                "contains the following informations/attributes:                     \n"
                                "                                                                    \n"
                                "  * an unique hit Id                                             \n"
                                "  * the geometry Id of the geometry volume where the energy was  \n"
                                "    deposited                                                    \n"
                                "  * the name of the traversing particle (assuming the first aggregated  \n"
                                "    microscopic hit is associated to a some kind of primary track \n"
                                "    and that subsequent aggregated hits could come from secondary \n"
                                "    particle, the name of the more primary particle is stored)    \n"
                                "  * the total energy deposit                                      \n"
                                "  * the bounds of a bounding box enclosing the aggregated primary \n"
                                "    hits                                                          \n"
                                "  * the minimum and maximum times of the first and last hits      \n"
                                "    aggregated in the cluster                                     \n"
                                "                                                                  \n"
                                "                                                                  \n"
                                );
  ocd_.set_class_library ("mctools");

  // Load OCD support for this class:
  ::mctools::calorimeter_step_hit_processor::init_ocd(ocd_);

  ocd_.set_configuration_hints ("The calorimeter step hit processor factory uses a 'datatools::properties' \n"
                                "object to initialize its behaviour and contents.                  \n"
                                "                                                                  \n"
                                "Example of the configuration for a calorimeter sensitive detector \n"
                                "from which we want to score the total energy deposit::            \n"
                                "                                                                  \n"
                                "  sensitive.category    : string  = \"calo_SD\"                   \n"
                                "  use_private_pool      : boolean = 1                             \n"
                                "  private_pool_capacity : integer = 10                            \n"
                                "  hit.category          : string  = \"calo\"                      \n"
                                "  cluster.time_range    : real as time   = 1 ns                   \n"
                                "  cluster.space_range   : real as length = 1 cm                   \n"
                                "  mapping.category      : string  = \"calo.block\"                \n"
                                "  alpha_quenching       : boolean = 0                             \n"
                                "                                                                  \n"
                                "From the main configuration file of a *step hit processor factory*,               \n"
                                "one must use the 'datatools::multi_properties' format. Example of                 \n"
                                "a calorimeter step hit processor attached to some scintillator                    \n"
                                "blocks in an experimental setup::                                                 \n"
                                "                                                                                  \n"
                                "  [name=\"scin.hit_processor\" type=\"mctools::calorimeter_step_hit_processor\"]  \n"
                                "  sensitive.category    : string  = \"scin_SD\"                                   \n"
                                "  use_private_pool      : boolean = 1                                             \n"
                                "  private_pool_capacity : integer = 10                                            \n"
                                "  hit.category          : string  = \"scin\"                                      \n"
                                "  cluster.time_range    : real as time   = 1 ns                                   \n"
                                "  cluster.space_range   : real as length = 1 cm                                   \n"
                                "  mapping.category      : string  = \"scin.block\"                                \n"
                                "  alpha_quenching       : boolean = 1                                             \n"
                                )
    ;

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::mctools::calorimeter_step_hit_processor,
                               "mctools::calorimeter_step_hit_processor")
