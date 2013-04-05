// -*- mode: c++ ; -*-
/* calorimeter_step_hit_processor.cc
 */

#include <mctools/calorimeter_step_hit_processor.h>


#include <algorithm>

#include <boost/algorithm/string.hpp>

#include <datatools/ioutils.h>
#include <mctools/utils.h>

namespace mctools {

  using namespace std;

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
    _verbose_ = false;
    _scintillation_cluster_time_range_  = 1.0 * CLHEP::ns;
    _scintillation_cluster_space_range_ = 1.0 * CLHEP::cm;

    _mapping_    = 0;
    _categories_ = 0;
    _calo_block_type_ = geomtools::geom_id::INVALID_TYPE;
    return;
  }
 
  calorimeter_step_hit_processor::~calorimeter_step_hit_processor ()
  {
    return;
  }

  MCTOOLS_STEP_HIT_PROCESSOR_INITIALIZE_IMPLEMENT_HEAD(calorimeter_step_hit_processor,
                                                       config_,
                                                       service_mgr_)
  {
    if (is_debug ()) {
      clog << datatools::io::debug << "mctools::calorimeter_step_hit_processor::initialize: "
           << "Entering..." << endl;
    }
    
    this->base_step_hit_processor::initialize(config_,
                                              service_mgr_);
    
    // The geometry manager is mandatory for this processor:
    if (! has_geom_manager ()) {
      std::ostringstream message;
      message << "mctools::calorimeter_step_hit_processor::initialize: "
              << "Missing geometry manager for processor '" << get_name () << "' !"
              << endl;
      throw std::logic_error (message.str ());
    }

    if (config_.has_key ("mapping.category")) {
      std::string geom_cat = config_.fetch_string ("mapping.category");
      set_mapping_category (geom_cat);
    }

    std::vector<std::string> any_addresses_labels;
    if (config_.has_key ("mapping.category.any_addresses")) {
      config_.fetch ("mapping.category.any_addresses", any_addresses_labels);
    }

    // A sensitive category is mandatory for this processor,
    // i.e. the one defined for the target scintillator blocks:
    if (_mapping_category_.empty ()) {
      std::ostringstream message;
      message << "mctools::calorimeter_step_hit_processor::initialize: "
              << "Missing 'mapping_category' string property for processor '" << get_name () << "' !";
      throw std::logic_error (message.str ());
    }

    if (is_debug ()) {
      std::clog << datatools::io::debug << "mctools::calorimeter_step_hit_processor::initialize: "
                << "Parse setup properties for processor '" << get_name () << "' ..." << std::endl;
    }

    // set the verbosity flag:
    if (config_.has_flag ("verbose")) {
      _verbose_ = true;
    }

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

    // pickup the ID mapping from the geometry manager:
    _mapping_ = &_geom_manager->get_mapping ();

    // check if the sensitive category is known:
    _categories_ = &(_geom_manager->get_id_mgr ().categories_by_name ());
    geomtools::id_mgr::categories_by_name_col_type::const_iterator icat
      = _categories_->find (_mapping_category_);
    if (icat == _categories_->end ())
      {
        std::ostringstream message;
        message << "mctools::calorimeter_step_hit_processor::initialize: "
                << "Cannot find geometry ID category '"
                << _mapping_category_ << "' string property for processor '" << get_name () << "'  !";
        throw std::logic_error (message.str ());
      }

    const geomtools::id_mgr::category_info & mapping_cat_info = icat->second;

    // initialize the smart ID locator for this
    // category of volumes:
    _calo_block_type_ = mapping_cat_info.get_type ();
    _calo_block_locator_.set_gmap (*_mapping_);
    _calo_block_locator_.initialize (_calo_block_type_);

    for (int i = 0; i < any_addresses_labels.size (); i++) {
      std::vector<std::string> strs;
      boost::split (strs, any_addresses_labels[i], boost::is_any_of("@"));
      if (strs.size () > 2) {
        std::ostringstream message;
        message << "mctools::calorimeter_step_hit_processor::initialize: "
                << "Invalid format for 'mapping.category.any_addresses' property ' from "
                << any_addresses_labels[i] << "' !";
        throw std::logic_error (message.str ());
      }
      const std::string & label = strs[0];
      int label_count = 1;
      if (strs.size () == 2) {
        const string & count_str = strs[1]; 
        istringstream iss (count_str);
        iss >> label_count;
        if (! iss) {
          std::ostringstream message;
          message << "mctools::calorimeter_step_hit_processor::initialize: "
                  << "Invalid format for 'mapping.category.any_addresses', cannot parse subaddress label count from "
                  << count_str << "' !";
          throw std::logic_error (message.str ());
        }
      }
      if (! mapping_cat_info.has_subaddress (label, label_count)) {
        std::ostringstream message;
        message << "mctools::calorimeter_step_hit_processor::initialize: "
                << "Category '" << mapping_cat_info.get_category () << "' has no subaddress labelled '" << label << "' !";
        throw std::logic_error (message.str ());
      }
      _mapping_category_any_addresses_.push_back (mapping_cat_info.get_subaddress_index (label, label_count));
    }
    if (_mapping_category_any_addresses_.size ()) {
      std::sort (_mapping_category_any_addresses_.begin (), 
                 _mapping_category_any_addresses_.end ());
    }

    if (is_debug ()) {
      clog << datatools::io::debug << "calorimeter_step_hit_processor::initialize: "
           << "Exiting." << endl;
    }
    return;
  }

  const std::vector<int> & calorimeter_step_hit_processor::get_mapping_category_any_addresses () const
  {
    return _mapping_category_any_addresses_;
  }

  /** Check if a 'step hit' could be aggregated to an existing 'scintillation hit'.
   *  Any new step hit must:
   * - belong to the same geometry element than the scintillation hit (geom ID location)
   * - be an alpha or a delta ray if it is a scintillation hit with an alpha
   *   in order to be able to collect the ionization energy deposited
   *   along an alpha track and then apply some realistic quenching factor
   *   Note: the case for
   * - be in time with the cluster time window @ requested precision (~1ns)
   * - be located within the volume of a fiducial sphere centered around
   *   the existing cluster hit @ requested precision (~1cm)
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
    const string & hit_pname = step_hit_.get_particle_name ();

    bool track_match = false;
    // check the special case of an alpha scintillation hit:
    if (scintillation_hit_.get_particle_name () == "alpha") {
      if (hit_pname == "e-") {
        // search for delta rays originated from an alpha track:
        bool is_delta_ray_from_alpha = step_hit_.get_auxiliaries ().has_flag(mctools::track_utils::DELTA_RAY_FROM_ALPHA_FLAG);
        if (is_delta_ray_from_alpha) {
          /*  delta ray production along the alpha track:
           *
           *       e-   e-     e-
           *      /    /      /
           *  ---+----+-+----+---> alpha
           *             \
           *              e-
           *
           *  the resulting ionization of the primary alpha
           *  plus its associated delta rays has some quenching effect: all these step
           *  hits should share the same scintillation cluster hit.
           */
          track_match = true;
        }
      }
      else if (hit_pname == "alpha") {
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
         * the same cluster
         * Special treatment would be needed for delta rays too.
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
        // within the scintillation cluister of an alpha particle:
        bool is_delta_ray_from_alpha = step_hit_.get_auxiliaries ().has_flag (mctools::track_utils::DELTA_RAY_FROM_ALPHA_FLAG);
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
    double t1 = scintillation_hit_.get_time_start () - _scintillation_cluster_time_range_;
    double t2 = scintillation_hit_.get_time_stop () + _scintillation_cluster_time_range_;
    double ta = step_hit_.get_time_start ();
    if (hit_pname == "gamma") {
      ta = step_hit_.get_time_stop ();
    }
    double tb = step_hit_.get_time_stop ();
    bool time_match = false;
    if ((ta > t1) && (ta < t2)) time_match = true;
    else if ((tb > t1) && (tb < t2)) time_match = true;
    else if ((t1 > ta) && (t1 < tb)) time_match = true;
    else if ((t2 > ta) && (t2 < tb)) time_match = true;

    // no timing match:
    if (! time_match) return false;

    /** check the position:
     *  a matching step hit has to be located in a fiducial sphere
     *  that wraps the scintillation cluster hit.
     */
    double cluster_dx = abs (scintillation_hit_.get_position_stop ().x () - scintillation_hit_.get_position_start ().x ());
    double cluster_dy = abs (scintillation_hit_.get_position_stop ().y () - scintillation_hit_.get_position_start ().y ());
    double cluster_dz = abs (scintillation_hit_.get_position_stop ().z () - scintillation_hit_.get_position_start ().z ());
    double cluster_radius = 0.5 * sqrt (cluster_dx * cluster_dx + cluster_dy * cluster_dy + cluster_dz * cluster_dz);

    // define the cluster fiducial spherical region:
    geomtools::sphere cluster_sphere;
    double factor = 1.0;
    if (cluster_radius < _scintillation_cluster_space_range_) {
      factor = 2.0;
    }
    cluster_sphere.set_radius (cluster_radius + factor * _scintillation_cluster_space_range_);

    double cluster_x = 0.5 * (scintillation_hit_.get_position_stop ().x () + scintillation_hit_.get_position_start ().x ());
    double cluster_y = 0.5 * (scintillation_hit_.get_position_stop ().y () + scintillation_hit_.get_position_start ().y ());
    double cluster_z = 0.5 * (scintillation_hit_.get_position_stop ().z () + scintillation_hit_.get_position_start ().z ());
    geomtools::vector_3d cluster_center;
    cluster_center.set (cluster_x, cluster_y, cluster_z);

    bool pos_match = false;
    if (hit_pname == "gamma") {
      // only check the interaction point at end of the step:
      if (cluster_sphere.is_inside (step_hit_.get_position_stop () - cluster_center)) {
        pos_match = true;
      }
    }
    else {
      // both start and stop of the step are checked:
      if (cluster_sphere.is_inside (step_hit_.get_position_start () - cluster_center)) {
        pos_match = true;
      }
      else if (cluster_sphere.is_inside (step_hit_.get_position_stop () - cluster_center)) {
        pos_match = true;
      }
    }

    // no spatial match:
    if (! pos_match) return false;

    return true;
  }

  MCTOOLS_STEP_HIT_PROCESSOR_PROCESS_PLAIN_IMPLEMENT_HEAD(calorimeter_step_hit_processor,
                                                          the_base_step_hits,
                                                          a_plain_gg_hits)
  {
    _process(the_base_step_hits, (simulated_data::hit_handle_collection_type *) 0, &a_plain_gg_hits);
    return;
  }
      
  MCTOOLS_STEP_HIT_PROCESSOR_PROCESS_HANDLE_IMPLEMENT_HEAD(calorimeter_step_hit_processor,
                                                           the_base_step_hits,
                                                           the_calo_hits)
  {
    _process(the_base_step_hits, &the_calo_hits, (simulated_data::hit_collection_type *) 0);
    return;
  }

  void calorimeter_step_hit_processor::_process(const base_step_hit_processor::step_hit_ptr_collection_type & a_shpc,
                                                simulated_data::hit_handle_collection_type * the_scintillation_hits,
                                                simulated_data::hit_collection_type        * the_plain_scintillation_hits)
  {
    bool devel = false;
    //devel = true;

    // Check the type of output collection (handles or plain hits) :
    bool use_handles = false;
    if (the_scintillation_hits != 0) {
      use_handles = true; 
    }
    else if (the_plain_scintillation_hits == 0) {
      throw std::logic_error ("mctools::calorimeter_step_hit_processor::_process: Missing hit collection type (NULL pointer) !");
    }
    base_step_hit * current_scintillation_cluster = 0;
    uint32_t scintillation_hit_count = 0;

    // Prereservation :
    if (use_handles) {
      the_scintillation_hits->reserve (20);
    }
    else {
      the_plain_scintillation_hits->reserve (20);
    }
    for (base_step_hit_processor::step_hit_ptr_collection_type::const_iterator ihit = a_shpc.begin ();
         ihit != a_shpc.end ();
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
      geomtools::geom_id gid = _calo_block_locator_.get_geom_id (hit_position_mean, 
                                                                 _calo_block_type_);
      if (! gid.is_valid ()) {
        // we do not process such a hit:
        if (_verbose_) {
          std::clog << datatools::io::warning << "mctools::calorimeter_step_hit_processor::process: "
                    << "We skip this hit for one cannot locate it through the locator attached to the '"
                    << get_mapping_category() << "' ! "
                    << " This is probably due to another mapping category registered in the current '"
                    << get_hit_category() << "' hit category "
                    << "that may generate its own step hits ! Consider to write your own hit processor able "
                    << "to handle several mapping categories (using several suitable locators) !"
                    << std::endl;
        }
        continue;
      }
            
      // Set 'any' for some GID subaddresses :
      for (int i = 0; i < _mapping_category_any_addresses_.size (); i++) {
        gid.set_any (_mapping_category_any_addresses_[i]);
      }
      the_step_hit.set_geom_id (gid);

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
                 = the_scintillation_hits->begin ();
               icluster != the_scintillation_hits->end ();
               icluster++) {
            if (! icluster->has_data ()) continue;
            base_step_hit & matching_hit = icluster->get ();
            if (match_scintillation_hit (matching_hit, the_step_hit)) {
              // pick up the first matching cluster :
              matching_scintillation_cluster = &matching_hit;
              break;
            }
          }
        }
        else {
          for (simulated_data::hit_collection_type::iterator icluster 
                 = the_plain_scintillation_hits->begin ();
               icluster != the_plain_scintillation_hits->end ();
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
          add_new_hit (*the_scintillation_hits);
          // optimization using a reference to this last inserted hit :
          current_scintillation_cluster = &(the_scintillation_hits->back ().get ());
        }
        else  {
          // add a new hit in the plain collection :
          base_step_hit dummy;
          the_plain_scintillation_hits->push_back (dummy);
          // get a reference to the last inserted scintillation cluster :
          current_scintillation_cluster = &(the_plain_scintillation_hits->back ());
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
        }
        else {
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
        }
        else {
          // compute the bounds of the step hit:
          xmin = min (the_step_hit.get_position_start ().x (), the_step_hit.get_position_stop ().x ());
          ymin = min (the_step_hit.get_position_start ().y (), the_step_hit.get_position_stop ().y ());
          zmin = min (the_step_hit.get_position_start ().z (), the_step_hit.get_position_stop ().z ());
          xmax = max (the_step_hit.get_position_start ().x (), the_step_hit.get_position_stop ().x ());
          ymax = max (the_step_hit.get_position_start ().y (), the_step_hit.get_position_stop ().y ());
          zmax = max (the_step_hit.get_position_start ().z (), the_step_hit.get_position_stop ().z ());
        }
        geomtools::vector_3d min_pos (xmin, ymin, zmin);
        current_scintillation_cluster->set_position_start (min_pos);
        geomtools::vector_3d max_pos (xmax, ymax, zmax);
        current_scintillation_cluster->set_position_stop (max_pos);

        //increment the cluster id:
        scintillation_hit_count++;
      }
      else {
        // add the step hit informations in the current cluster:
        current_scintillation_cluster = matching_scintillation_cluster;

        // increment energy deposit:
        double cluster_energy_deposit = current_scintillation_cluster->get_energy_deposit ()
          + hit_energy_deposit;
        current_scintillation_cluster->set_energy_deposit (cluster_energy_deposit);
        if (hit_particle_name == "gamma") {
          if (hit_time_stop < current_scintillation_cluster->get_time_start ()) {
            current_scintillation_cluster->set_time_start (hit_time_stop);
          }
          if (hit_time_stop > current_scintillation_cluster->get_time_stop ()) {
            current_scintillation_cluster->set_time_stop (hit_time_stop);
          }
        }
        else {
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

    return;
  }

  void calorimeter_step_hit_processor::merge_scintillation_hits (simulated_data::hit_handle_collection_type & scintillation_hits_)
  {
    // pickup a reference to the proper list of hits:
    //simulated_data::hit_handle_collection_type & scintillation_hits = scintillation_hits_;

    // NOT IMPLEMENTED YET...

    return;
  }
 
  void calorimeter_step_hit_processor::tree_dump (std::ostream & a_out, 
                                                  const std::string & a_title,
                                                  const std::string & a_indent,
                                                  bool a_inherit) const
  {
    namespace du = datatools;
    std::string indent;
    if (! a_indent.empty ()) {
      indent = a_indent;
    }
    base_step_hit_processor::tree_dump (a_out, a_title, indent, true);
    a_out << indent << du::i_tree_dumpable::tag 
          << "Verbose : " << _verbose_ << std::endl;
    a_out << indent << du::i_tree_dumpable::tag 
          << "Time range : " << _scintillation_cluster_time_range_ / CLHEP::ns << " (ns)" << std::endl;
    a_out << indent << du::i_tree_dumpable::tag 
          << "Space range : " << _scintillation_cluster_space_range_ / CLHEP::mm << " (mm)" << std::endl;
    a_out << indent << du::i_tree_dumpable::tag 
          << "Mapping category : '" <<_mapping_category_ << "'" << std::endl;
    a_out << indent << du::i_tree_dumpable::tag 
          << "Mapping    : " << _mapping_ << std::endl;
    {
      a_out << indent << du::i_tree_dumpable::tag 
            << "Categories : " << _categories_ << " ";
      if (_categories_ != 0) {
        a_out << '[' << _categories_->size () << ']';
        /*
          for (geomtools::id_mgr::categories_by_name_col_t::const_iterator i = _categories_->begin ();
          i != _categories_->end ();
          i++)
          {
          geomtools::id_mgr::categories_by_name_col_t::const_iterator j = i;
          j++;
          a_out << indent << du::i_tree_dumpable::skip_tag;
          if (j == _categories_->end ())
          {
          a_out << du::i_tree_dumpable::last_tag;
          }
          else
          {
          a_out << du::i_tree_dumpable::tag;
          }
          a_out << "Category '" << i->first << "'" << std::endl;
          }
        */
      }
      a_out << std::endl;
    }
    a_out << indent << du::i_tree_dumpable::inherit_tag (a_inherit) 
          << "Calorimeter block type : " << _calo_block_type_ << std::endl;
        
    return;
  }

} // end of namespace mctools

// end of calorimeter_step_hit_processor.cc
