// -*- mode: c++ ; -*-
/* sensitive_detector.cc
 */

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>

#include <boost/algorithm/string.hpp>

#include <datatools/exception.h>

#include <mctools/g4/sensitive_detector.h>
#include <mctools/g4/manager.h>

#include <geomtools/sensitive.h>
#include <geomtools/material.h>

#include <mctools/utils.h>

// G4 stuff:
#include <G4Step.hh>
#include <G4ThreeVector.hh>
#include <G4HCofThisEvent.hh>
#include <G4SDManager.hh>
#include <G4Gamma.hh>
#include <G4Neutron.hh>
#include <G4VProcess.hh>

namespace mctools {

  namespace g4 {

    using namespace std;

    const size_t sensitive_detector::DEFAULT_HIT_BUFFER_CAPACITY = 1000;

    bool sensitive_detector::is_drop_zero_energy_deposit_steps () const
    {
      return _drop_zero_energy_deposit_steps_;
    }

    void sensitive_detector::set_drop_zero_energy_deposit_steps (bool drop_)
    {
      _drop_zero_energy_deposit_steps_ = drop_;
      return;
    }

    void sensitive_detector::set_track_gamma (bool track_gamma_)
    {
      _track_gamma_ = track_gamma_;
      return;
    }

    void sensitive_detector::set_track_neutron (bool track_neutron_)
    {
      _track_neutron_ = track_neutron_;
      return;
    }

    bool sensitive_detector::has_manager () const
    {
      return _manager_ != 0;
    }

    void sensitive_detector::set_manager (manager & manager_)
    {
      _manager_ = &manager_;
      return;
    }

    const datatools::properties &
    sensitive_detector::get_auxiliaries () const
    {
      return _aux_;
    }

    datatools::properties &
    sensitive_detector::grab_auxiliaries ()
    {
      return _aux_;
    }

    void sensitive_detector::set_flag_delta_ray_from_alpha (bool flag_)
    {
      _flag_delta_ray_from_alpha_ = flag_;
      return;
    }

    void sensitive_detector::set_record_track_id (bool record_)
    {
      _record_track_id_ = record_;
      return;
    }

    void sensitive_detector::set_record_primary_particle (bool record_)
    {
      _record_primary_particle_ = record_;
      return;
    }

    void sensitive_detector::set_record_major_track (bool record_)
    {
      _record_major_track_ = record_;
      return;
    }

    void sensitive_detector::set_major_track_minimum_energy (double major_)
    {
      _major_track_minimum_energy_ = major_;
      return;
    }

    void sensitive_detector::set_record_creator_process (bool record_)
    {
      _record_creator_process_ = record_;
      return;
    }

    void sensitive_detector::set_record_creator_category (bool record_)
    {
      _record_creator_category_ = record_;
      return;
    }

    void sensitive_detector::set_record_material (bool record_)
    {
      _record_material_ = record_;
      return;
    }

    void sensitive_detector::set_record_sensitive_category (bool record_)
    {
      _record_sensitive_category_ = record_;
      return;
    }

    void sensitive_detector::set_record_momentum (bool record_)
    {
      _record_momentum_ = record_;
      return;
    }

    void sensitive_detector::set_record_kinetic_energy (bool record_)
    {
      _record_kinetic_energy_ = record_;
      return;
    }

    int sensitive_detector::get_HCID () const
    {
      return _HCID_;
    }

    void sensitive_detector::set_HCID (int new_value_)
    {
      _HCID_ = new_value_;
      return;
    }

    sensitive_detector::hit_processor_dict_type &
    sensitive_detector::grab_hit_processors ()
    {
      return _hit_processors_;
    }

    const sensitive_detector::hit_processor_dict_type &
    sensitive_detector::get_hit_processors () const
    {
      return _hit_processors_;
    }

    mctools::base_step_hit_processor &
    sensitive_detector::grab_hit_processor (const string & name_)
    {
      hit_processor_dict_type::iterator found = _hit_processors_.find (name_);
      DT_THROW_IF (found == _hit_processors_.end (),
                   logic_error, "No '" << name_ << "' hit processor !");
      return *found->second;
    }

    void sensitive_detector::add_hit_processor (const string & name_,
                                                mctools::base_step_hit_processor & shp_)
    {
      DT_THROW_IF (_hit_processors_.find (name_) != _hit_processors_.end (),
                   logic_error,
                   "This sensitive detector '"
                   << _sensitive_category_
                   << "' already has a '"
                   << name_ << "' hit processor !");
      _hit_processors_[name_] = &shp_;
      return;
    }

    bool sensitive_detector::has_hit_processor (const string & name_) const
    {
      return (_hit_processors_.find (name_) != _hit_processors_.end ());
    }

    void sensitive_detector::attach_logical_volume (const string & log_volume_name_)
    {
      if (find (_attached_logical_volumes_.begin (),
                _attached_logical_volumes_.end (),
                log_volume_name_) != _attached_logical_volumes_.end ()) {
        DT_LOG_WARNING(_logprio(), "Logical volume '" << log_volume_name_
                       << "' is already attached to sensitive detector '"
                       << _sensitive_category_ << "' !");
        return;
      }
      _attached_logical_volumes_.push_back (log_volume_name_);
      return;
    }

    list<string> &
    sensitive_detector::grab_attached_logical_volumes ()
    {
      return _attached_logical_volumes_;
    }

    string sensitive_detector::make_hit_collection_name (const string & name_)
    {
      ostringstream oss;
      oss << name_ << ".hit_collection";
      return oss.str ();
    }

    void sensitive_detector::set_store_g4_volume_properties (bool flag_)
    {
      _store_g4_volume_properties_ = flag_;
      return;
    }

    bool sensitive_detector::is_store_g4_volume_properties () const
    {
      return _store_g4_volume_properties_;
    }

    const string & sensitive_detector::get_sensitive_category () const
    {
      return _sensitive_category_;
    }

    size_t sensitive_detector::get_hits_buffer_capacity () const
    {
      return _hits_buffer_capacity_;
    }

    void sensitive_detector::set_hits_buffer_capacity (size_t capacity_)
    {
      if (capacity_ > 0)
        {
          _hits_buffer_capacity_ = capacity_;
        }
      return;
    }

    void sensitive_detector::configure (const datatools::properties & config_)
    {
      _initialize_logging_support(config_);

      if (geomtools::sensitive::recording_track_id (config_)) {
        // this method enables the sensitive detector to record the track ID
        // as an auxiliary properties of the step hit
        // cerr << datatools::io::devel
        //      << "snemo::g4::sensitive_detector::configure: "
        //      << "<record the track ID>"
        //      << endl;
        set_record_track_id (true);
      }

      if (geomtools::sensitive::recording_primary_particle (config_)) {
        // this method enables the sensitive detector to record a dedicated flag
        // as an auxiliary properties of the step hit to flag a primary track
        // cerr << datatools::io::devel
        //      << "snemo::g4::sensitive_detector::configure: "
        //      << "<record primary particle flag>"
        //      << endl;
        set_record_primary_particle (true);
      }

      if (geomtools::sensitive::recording_alpha_quenching (config_)) {
        // this method enables to add a special flag to the step hit
        // for delta rays produced along the track of alpha particles;
        // this should make possible to take into account quenching effects
        // along alpha tracks by aggregation of the total energy deposit
        // by the alpha.
        // cerr << datatools::io::devel
        //      << "snemo::g4::sensitive_detector::configure: "
        //      << "<record alpha quenching flag>"
        //      << endl;
        set_flag_delta_ray_from_alpha (true);
      }

      // XG (28/05/2011): the way to set 'record' properties is only
      // working when class members are set to false by default (which
      // is the case for the following parameters). For tracking
      // gammas and neutrons, boolean flag is set to true by default
      // and then such construction is not working anymore. If default
      // behavior change, have a look to 'tracking gamma/neutron' to
      // be sure to set properties properly
      {
        // Record major track
        const string rmt_key = geomtools::sensitive::make_key ("record_major_track");
        if (config_.has_flag (rmt_key)) {
          set_record_major_track (true);
        }
      }

      {
        const string mtme_key = geomtools::sensitive::make_key ("major_track_minimum_energy");
        if (config_.has_key (mtme_key)) {
          double e_min = config_.fetch_real (mtme_key) * CLHEP::keV;
          DT_THROW_IF (e_min < 0, logic_error,
                       "Invalid minimum energy for major track mode "
                       << "for sensitive detector '"
                       << _sensitive_category_
                       << "' !");
          set_major_track_minimum_energy (e_min);
        }
      }

      {
        // Record creator process
        string record_creator_process_key = geomtools::sensitive::make_key ("record_creator_process");
        if (config_.has_flag (record_creator_process_key)) {
          set_record_creator_process (true);
        }
      }

      {
        // Record sensitive category where particle is created
        string record_creator_category_key = geomtools::sensitive::make_key ("record_creator_category");
        if (config_.has_flag (record_creator_category_key)) {
          set_record_creator_category (true);
        }
      }

      {
        // 2011-08-26 FM: new option : 'record material'
        string record_material_key = geomtools::sensitive::make_key ("record_material");
        if (config_.has_flag (record_material_key)) {
          set_record_material (true);
        }
      }

      {
        // 2011-08-26 FM: new option : 'record sensitive category'
        string record_sensitive_category_key = geomtools::sensitive::make_key ("record_category");
        if (config_.has_flag (record_sensitive_category_key)) {
          set_record_sensitive_category (true);
        }
      }

      {
        // Record momentum
        string record_momentum_key = geomtools::sensitive::make_key ("record_momentum");
        if (config_.has_flag (record_momentum_key)) {
          set_record_momentum (true);
        }
      }

      {
        // Record kinetic energy
        string record_kinetic_energy_key = geomtools::sensitive::make_key ("record_kinetic_energy");
        if (config_.has_flag (record_kinetic_energy_key)) {
          set_record_kinetic_energy (true);
        }
      }

      {
        string key = geomtools::sensitive::make_key ("hits_buffer_capacity");
        if (config_.has_key (key)) {
          int cap = config_.fetch_integer (key);
          DT_THROW_IF (cap < 0, logic_error,
                       "Invalid buffer size for sensitive detector '"
                       << _sensitive_category_
                       << "' !");
          size_t capacity = (size_t) cap;
          if (capacity > 0) {
            set_hits_buffer_capacity (capacity);
          }
        }
      }

      // Drop steps with no energy deposit
      if (geomtools::sensitive::has_key (config_, "drop_zero_energy_deposit_steps")) {
        const bool flag = geomtools::sensitive::has_flag (config_,
                                                          "drop_zero_energy_deposit_steps");
        set_drop_zero_energy_deposit_steps (flag);
      }

      // Storing Geant4 volume properties
      if (geomtools::sensitive::has_key (config_, "store_g4_volume")) {
        const bool flag = geomtools::sensitive::has_flag (config_,
                                                          "store_g4_volume");
        set_store_g4_volume_properties (flag);
      }

      // Tracking gamma
      if (geomtools::sensitive::has_key (config_, "track_gamma")) {
        const bool flag = geomtools::sensitive::has_flag (config_, "track_gamma");
        set_track_gamma (flag);
      }

      // Tracking neutron: if no key then use default behavior set in
      // _set_defaults method
      if (geomtools::sensitive::has_key (config_, "track_neutron")) {
        const bool flag = geomtools::sensitive::has_flag (config_, "track_neutron");
        set_track_neutron (flag);
      }

      return;
    }

    void sensitive_detector::_set_defaults ()
    {
      _drop_zero_energy_deposit_steps_ = false;
      _store_g4_volume_properties_     = false;
      _flag_delta_ray_from_alpha_      = false;
      _record_primary_particle_        = false;
      _record_track_id_                = false;
      _record_major_track_             = false;
      _major_track_minimum_energy_     = 10. * CLHEP::keV;
      _track_gamma_                    = true;
      _track_neutron_                  = true;
      _record_momentum_                = false;
      _record_kinetic_energy_          = false;
      _record_creator_process_         = false;
      _record_creator_category_        = false;
      _record_material_                = false;
      _record_sensitive_category_      = false;
      _hits_buffer_capacity_ = DEFAULT_HIT_BUFFER_CAPACITY;

      // G4 Stuff:
      _HCID_ = -1; // Initialized with an invalid value

      return;
    }

    // ctor:
    sensitive_detector::sensitive_detector (const string & sensitive_category_)
      : G4VSensitiveDetector (sensitive_category_)
    {
      _manager_ = 0;
      _sensitive_category_ = sensitive_category_;

      _set_defaults ();

      _track_info_ptr_ = 0;
      _parent_track_info_ptr_ = 0;

      _hits_collection_ = 0;
      _number_of_sensitive_steps_ = 0;

      _used_hits_count_ = 0;

      // G4 Stuff:
      G4String HCname;
      HCname = sensitive_detector::make_hit_collection_name (_sensitive_category_).c_str ();
      collectionName.insert (HCname);
      return;
    }

    // dtor:
    sensitive_detector::~sensitive_detector ()
    {
      // clog << datatools::io::notice
      //      << "snemo::g4::sensitive_detector::~sensitive_detector: "
      //      << "Number of sensitive steps for detector '" << _sensitive_category_ << "' = "
      //      << _number_of_sensitive_steps
      //      << endl;

      _hit_processors_.clear ();
      _track_info_ptr_ = 0;
      _parent_track_info_ptr_ = 0;
      if (_hits_collection_ != 0) {
        delete _hits_collection_;
        _hits_collection_ = 0;
      }
      return;
    }

    void sensitive_detector::clear ()
    {
      // Reset the internal ID if needed :
      for (int i = 0; i < _used_hits_count_; i++) {
        _hits_buffer_[i].grab_hit_data ().reset ();
      }
      _used_hits_count_ = 0;
      if (_hits_collection_ != 0) {
        _hits_collection_->grab_hits ().clear ();
      }
      return;
    }

    void sensitive_detector::Initialize (G4HCofThisEvent * some_hit_collections_)
    {
      //cerr << datatools::io::devel << "snemo::g4::sensitive_detector::Initialize: "
      //            << "Entering for detector '" << _sensitive_category_ << "'..." << endl;

      // First pass, initialize the buffer :
      if (_hits_buffer_.size () == 0) {
        _hits_buffer_.reserve (_hits_buffer_capacity_);
        // pre-allocate some hits :
        for (int i = 0; i < (int) _hits_buffer_capacity_; i++) {
          sensitive_hit a_hit;
          _hits_buffer_.push_back (a_hit);
          // make sure they have no valid ID :
          //_hits_buffer.back ().get_hit_data ().invalidate_hit_id ();
        }
        // No hit has been validated from this buffer yet :
        _used_hits_count_ = 0;
        // cerr << datatools::io::devel << "snemo::g4::sensitive_detector::Initialize: "
        //      << " Buffer size = " << _hits_buffer.size () << endl;
        // cerr << datatools::io::devel << "snemo::g4::sensitive_detector::Initialize: "
        //      << " Hit count = " << _used_hits_count << endl;
        // cerr << datatools::io::devel << "snemo::g4::sensitive_detector::Initialize: "
        //      << " Hit @: " << &_hits_buffer[0] << endl;
        //_hits_buffer.back ().get_hit_data ().tree_dump (cerr, "" , "DEVEL: ");
        DT_LOG_NOTICE(_logprio(), "Setup the buffer of hits for detector '" << _sensitive_category_ << "' "
                      << "with size = " << _hits_buffer_.size ());
      }

      // Reset the hits in the buffer if needed :
      for (int i = 0; i < _used_hits_count_; i++) {
        _hits_buffer_[i].grab_hit_data ().reset ();
        // cerr << datatools::io::devel << "snemo::g4::sensitive_detector::Initialize: "
        //      << "Reset base step hit #" << i << endl;
      }
      if (_hits_collection_ != 0) {
        _hits_collection_->grab_hits ().clear ();
      }

      // Reset the hit counter :
      _used_hits_count_ = 0;

      // Activates the track info mechanism :
      if (_flag_delta_ray_from_alpha_) _manager_->set_use_track_history (true);
      if (_record_track_id_)           _manager_->set_use_track_history (true);
      if (_record_primary_particle_)   _manager_->set_use_track_history (true);
      if (_record_major_track_)        _manager_->set_use_track_history (true);
      if (_record_creator_process_)    _manager_->set_use_track_history (true);
      if (_record_creator_category_)   _manager_->set_use_track_history (true);

      //cerr << datatools::io::devel << "snemo::g4::sensitive_detector::Initialize: " << "Exiting." << endl;
      return;
    }

    void sensitive_detector::EndOfEvent (G4HCofThisEvent * some_hit_collections_)
    {
      // cerr << datatools::io::devel << "snemo::g4::sensitive_detector::EndOfEvent: "
      //           << "Entering for detector '" << _sensitive_category_ << "'..." << endl;
      if ( _HCID_ < 0 ) {
        _HCID_ = G4SDManager::GetSDMpointer ()->GetCollectionID (collectionName[0]);
      }

      // Only if we have some hits :
      if (_used_hits_count_ > 0) {

        // Set the hits collection pointer :
        if (_hits_collection_ == 0) {
          _hits_collection_ = new sensitive_hit_collection (SensitiveDetectorName,
                                                            collectionName[0]);
          // cerr << datatools::io::devel << "snemo::g4::sensitive_detector::EndOfEvent: "
          //           << "Hits collection @ " << _hits_collection << endl;
          // Assign proper capacity to the 'hits collection' :
          _hits_collection_->grab_hits ().reserve (_hits_buffer_.size ());
        }

        sensitive_hit_collection::hit_collection_type & the_hits
          = _hits_collection_->grab_hits ();

        // fill it with the hits' addresses (pointer) :
        for (int i = 0; i < _used_hits_count_; i++) {
          the_hits.push_back (&_hits_buffer_[i]);
        }

        // Push the "hits collection" in the event's list of hits collections :
        some_hit_collections_->AddHitsCollection (_HCID_, _hits_collection_);
      }

      // clear tracks infos for this event:
      _track_info_ptr_ = 0;
      _parent_track_info_ptr_ = 0;
      _manager_->grab_track_history ().reset ();

      /*
        if ( _verbose > 1 )
        {
        sensitive_hits_collection * HC_tmp = 0;
        HC_tmp = (sensitive_hits_collection * ) ( some_hit_collections->GetHC (_HCID));
        for (size_t i = 0; i < HC_tmp->GetSize (); i++ )
        {
        clog << "DEBUG: sensitive_detector::EndOfEvent: "
        << "hit collection (" << i
        << ") is " << HC_tmp->GetVector ()->at (i)
        << "." << endl;
        }
        }
      */
      // cerr << datatools::io::devel
      //      << "snemo::g4::sensitive_detector::EndOfEvent: "
      //      << "Exiting." << endl;
      return;
    }

    G4bool sensitive_detector::ProcessHits (G4Step * step_,
                                            G4TouchableHistory * touchable_history_)
    {
      DT_LOG_TRACE(_logprio(),"Entering...");
      const bool do_process_hits = true;
      if (! do_process_hits) {
        DT_LOG_TRACE(_logprio(),"Exiting.");
        return false;
      }

      DT_LOG_TRACE(_logprio(),"Buffer size = " << _hits_buffer_.size ());
      DT_LOG_TRACE(_logprio(),"Hit count = " << _used_hits_count_);

      const double energy_deposit = step_->GetTotalEnergyDeposit ();
      if (energy_deposit <= 1.e-10 * CLHEP::keV){
        bool quit = true;
        if (step_->GetTrack ()->GetDefinition () == G4Gamma::GammaDefinition ()) {
          if (_track_gamma_) quit = false;
        } else if (step_->GetTrack ()->GetDefinition () == G4Neutron::NeutronDefinition ()) {
          if (_track_neutron_) quit = false;
        } else if (!_drop_zero_energy_deposit_steps_) {
          quit = false;
        }

        if (quit) {
          DT_LOG_TRACE(_logprio(),"Exiting.");
          return false;
        }
      }

      if (_manager_->using_time_stat ()) {
        _manager_->grab_CT_map ()["EA"].pause ();
        _manager_->grab_CT_map ()["SD"].start ();
      }

      const string track_particle_name = step_->GetTrack ()->GetDefinition ()->GetParticleName ();
      const int track_id        = step_->GetTrack ()->GetTrackID ();
      const int parent_track_id = step_->GetTrack ()->GetParentID ();

      bool primary_track           = false;
      bool delta_ray_from_an_alpha = false;
      bool major_track             = false;

      // Grabbing the track history to fill G4 track info
      track_history & the_track_history = _manager_->grab_track_history ();

      if (_manager_->has_track_history ()) {
        if ((_track_info_ptr_ == 0) || (_track_info_ptr_->get_id () != track_id)) {
          // Here we don't have current track info at hand:
          if (! the_track_history.has_track_info (track_id)) {
            // infos about this track are not registered yet,
            // we add a new record and link it :
            track_history::track_info dummy;
            the_track_history.add_track_info (track_id, dummy);
            track_history::track_info & ti = the_track_history.grab_track_info (track_id);
            ti.set_id (track_id);
            ti.set_parent_id (parent_track_id);
            ti.set_particle_name (track_particle_name);
            if (step_->GetTrack ()->GetCreatorProcess ()) {
              const std::string & process_name
                = step_->GetTrack ()->GetCreatorProcess ()->GetProcessName ();
              ti.set_creator_process_name (process_name);
            }
            const std::string & category = get_sensitive_category ();
            ti.set_creator_sensitive_category (category);
            _track_info_ptr_ = &ti;
          } else {
            // infos about this track have already been registered, we link it:
            _track_info_ptr_ = &the_track_history.grab_track_info (track_id);
          }
        }
        primary_track = _track_info_ptr_->is_primary ();

        // set the 'major_track' flag :
        const bool has_charge = (step_->GetTrack ()->GetDynamicParticle ()->GetCharge () != 0.0);
        if (has_charge) {
          if (primary_track) {
            major_track = true;
          }
          const double kinetic_energy= step_->GetTrack ()->GetKineticEnergy ();
          if (kinetic_energy >= _major_track_minimum_energy_) {
            major_track = true;
          }
        }

        if (_flag_delta_ray_from_alpha_) {
          /* Identify a delta-ray generated along
           * the track of an alpha particle:
           */
          if (track_particle_name == "e-" && ! primary_track) {
            // this is a secondary electron from a parent track:
            if ((_parent_track_info_ptr_ == 0) ||
                (_parent_track_info_ptr_->get_id () != parent_track_id)) {
              // we don't have current parent track info at hand
              // so we try to find it in the track info dictionnary:
              if (the_track_history.has_track_info (parent_track_id)) {
                // if found, make it the current link:
                _parent_track_info_ptr_ = &the_track_history.grab_track_info (parent_track_id);
              }
            }

            // if the parent track has been identified and it is an alpha particle:
            if ((_parent_track_info_ptr_ != 0) &&
                (_parent_track_info_ptr_->get_particle_name () == "alpha")) {
              // flag the delta_ray:
              delta_ray_from_an_alpha = true;
            }
          }
        }

      } // if (_using_track_infos)

      if (_used_hits_count_ == (int) _hits_buffer_.size ()) {
        //size_t osize = _hits_buffer.size ();
        sensitive_hit a_hit;
        _hits_buffer_.push_back (a_hit);
        //size_t nsize = _hits_buffer.size ();
        // cerr << datatools::io::devel
        //      << "snemo::g4::sensitive_detector::ProcessHits: "
        //      << "Increase the size of the buffer of sensitive hits for detector '"
        //      << _sensitive_category_ << "' from " << osize
        //      << " to " << nsize << " (capacity=" << _hits_buffer.capacity () << ")!"
        //      << endl;
      }
      // Increment the hit counter :
      _used_hits_count_++;

      sensitive_hit * new_hit = &_hits_buffer_[_used_hits_count_ - 1];

      DT_LOG_TRACE(_logprio(), "Buffer size = " << _hits_buffer_.size ());
      DT_LOG_TRACE(_logprio(), "Hit count = " << _used_hits_count_);
      DT_LOG_TRACE(_logprio(), "New hit @ " << new_hit << " : ");
      if (_logprio() == datatools::logger::PRIO_TRACE){
        new_hit->get_hit_data ().tree_dump (std::cerr);
      }

      _number_of_sensitive_steps_++;
      // 2011-05-26 FM : was using "step_->GetTrack ()->GetGlobalTime ()";
      const double time_start = step_->GetPreStepPoint ()->GetGlobalTime ();
      const double time_stop = step_->GetPostStepPoint ()->GetGlobalTime ();
      new_hit->grab_hit_data ().set_time_start (time_start);
      new_hit->grab_hit_data ().set_time_stop (time_stop);
      new_hit->grab_hit_data ().set_position_start (step_->GetPreStepPoint ()->GetPosition ());
      new_hit->grab_hit_data ().set_position_stop (step_->GetPostStepPoint ()->GetPosition ());
      new_hit->grab_hit_data ().set_energy_deposit (energy_deposit);
      new_hit->grab_hit_data ().set_particle_name (track_particle_name);

      // Add optional data :
      if (_record_momentum_) {
        new_hit->grab_hit_data ().set_momentum_start (step_->GetPreStepPoint ()->GetMomentum ());
        new_hit->grab_hit_data ().set_momentum_stop (step_->GetPostStepPoint ()->GetMomentum ());
      }

      // Grab base hit auxiliaries
      datatools::properties & hit_aux = new_hit->grab_hit_data ().grab_auxiliaries ();

      // Add auxiliary properties :
      if (_record_kinetic_energy_) {
        hit_aux.store (mctools::track_utils::START_KINETIC_ENERGY_KEY,
                       step_->GetPreStepPoint ()->GetKineticEnergy ());
        hit_aux.store (mctools::track_utils::STOP_KINETIC_ENERGY_KEY,
                       step_->GetPostStepPoint ()->GetKineticEnergy ());
      }

      const bool use_track_info = _manager_->has_track_history ();
      if (use_track_info) {
        // special features:
        if (_record_creator_process_ && ! _track_info_ptr_->get_creator_process_name ().empty ()) {
          hit_aux.store (mctools::track_utils::CREATOR_PROCESS_KEY,
                         _track_info_ptr_->get_creator_process_name ());
        }

        if (_record_creator_category_ && ! _track_info_ptr_->get_creator_sensitive_category ().empty ()) {
          hit_aux.store (mctools::track_utils::CREATOR_CATEGORY_KEY,
                         _track_info_ptr_->get_creator_sensitive_category ());
        }

        if (_record_primary_particle_ && primary_track) {
          hit_aux.store_flag (mctools::track_utils::PRIMARY_PARTICLE_FLAG);
        }

        if (_record_major_track_ && major_track) {
          hit_aux.store_flag (mctools::track_utils::MAJOR_TRACK_FLAG);
        }

        if (_flag_delta_ray_from_alpha_ && delta_ray_from_an_alpha) {
          hit_aux.store_flag (mctools::track_utils::DELTA_RAY_FROM_ALPHA_FLAG);
        }

        if (_record_track_id_) {
          hit_aux.store (mctools::track_utils::TRACK_ID_KEY, track_id);
          hit_aux.store (mctools::track_utils::PARENT_TRACK_ID_KEY,
                         parent_track_id);
        }
      }

      // 2011-08-26, FM: add
      //>>>
      if (_record_material_) {
        static string material_ref_key =
          geomtools::material::make_key (geomtools::material::constants::instance ().MATERIAL_REF_PROPERTY);
        const G4Material * the_g4_material = step_->GetTrack ()->GetMaterial ();
        string material_ref = the_g4_material->GetName ().data ();
        boost::replace_all (material_ref, "__" , "::");
        hit_aux.store (material_ref_key, material_ref);
      }
      //<<<

      if (_record_sensitive_category_) {
        static string sensitive_category_key =
          geomtools::sensitive::make_key (geomtools::sensitive::constants::instance ().SENSITIVE_CATEGORY_PROPERTY);
        hit_aux.store (sensitive_category_key, get_sensitive_category ());
      }

      if (_store_g4_volume_properties_) {
        G4VPhysicalVolume * volume = step_->GetTrack ()->GetVolume ();
        hit_aux.store ("g4_volume.name",
                       volume->GetLogicalVolume ()->GetName ());
      }

      if (_manager_->using_time_stat ()) {
        _manager_->grab_CT_map ()["SD"].stop ();
        _manager_->grab_CT_map ()["EA"].resume ();
      }

      DT_LOG_TRACE(_logprio(),"Exiting.");
      return true;
    }

    void sensitive_detector::tree_dump (ostream & out_,
                                        const string & title_,
                                        const string & indent_,
                                        bool inherit_) const
    {
      namespace du = datatools;
      string indent;
      if (! indent_.empty ()) indent = indent_;
      if (! title_.empty ()) {
        out_ << indent << title_ << endl;
      }
      out_ << indent << du::i_tree_dumpable::tag
           << "Logging priority : " << datatools::logger::get_priority_label(_logging_priority) << endl;
      out_ << indent << du::i_tree_dumpable::tag
           << "Category     : " << _sensitive_category_ << endl;
      // Logical volumes
      {
        out_ << indent << du::i_tree_dumpable::tag
             << "Attached logical volumes : " << _attached_logical_volumes_.size () << endl;
        for (list<string>::const_iterator
               ilog = _attached_logical_volumes_.begin ();
             ilog != _attached_logical_volumes_.end (); ++ilog) {
          out_ << indent << du::i_tree_dumpable::skip_tag;
          list<string>::const_iterator jlog = ilog;
          if (++jlog == _attached_logical_volumes_.end ()) {
            out_ << du::i_tree_dumpable::last_tag;
          } else {
            out_ << du::i_tree_dumpable::tag;
          }
          out_ << "Logical["
               << std::distance (_attached_logical_volumes_.begin (), ilog)
               << "]: '" << *ilog << "'" << endl;
        }
      }

      // Flags
      {
        out_ << indent << du::i_tree_dumpable::tag
             << "Store volume properties  : "
             <<  (_store_g4_volume_properties_ ? "Yes": "No") << endl;

        out_ << indent << du::i_tree_dumpable::tag
             << "Drop zero energy deposit : "
             <<  (_drop_zero_energy_deposit_steps_ ? "Yes": "No") << endl;

        out_ << indent << du::i_tree_dumpable::tag
             << "Record momentum          : "
             <<  (_record_momentum_ ? "Yes": "No") << endl;

        out_ << indent << du::i_tree_dumpable::tag
             << "Record kinetic energy    : "
             <<  (_record_kinetic_energy_ ? "Yes": "No") << endl;

        out_ << indent << du::i_tree_dumpable::tag
             << "Record primary particle  : "
             <<  (_record_primary_particle_ ? "Yes": "No") << endl;

        out_ << indent << du::i_tree_dumpable::tag
             << "Record track id          : "
             <<  (_record_track_id_ ? "Yes": "No") << endl;

        out_ << indent << du::i_tree_dumpable::tag
             << "Record creator process   : "
             <<  (_record_creator_process_ ? "Yes": "No") << endl;

        out_ << indent << du::i_tree_dumpable::tag
             << "Record major track       : "
             <<  (_record_major_track_ ? "Yes": "No");

        if (_record_major_track_) {
          out_ << " (Emin > "
               << _major_track_minimum_energy_ / CLHEP::keV << " keV)"
               << endl;
        } else {
          out_ << endl;
        }

        out_ << indent << du::i_tree_dumpable::tag
             << "Track gamma              : "
             <<  (_track_gamma_ ? "Yes": "No") << endl;

        out_ << indent << du::i_tree_dumpable::tag
             << "Track neutron            : "
             <<  (_track_neutron_ ? "Yes": "No") << endl;

        out_ << indent << du::i_tree_dumpable::tag
             << "Delta ray from alpha     : "
             <<  (_flag_delta_ray_from_alpha_ ? "Yes": "No") << endl;

        out_ << indent << du::i_tree_dumpable::tag
             << "Track info pointer       : ";
        if (_track_info_ptr_ != 0) out_ << _track_info_ptr_;
        else                       out_ << "Not allocated";
        out_ << endl;

        out_ << indent << du::i_tree_dumpable::tag
             << "Parent track info pointer : ";
        if (_parent_track_info_ptr_ != 0) out_ << _parent_track_info_ptr_;
        else                              out_ << "Not allocated";
        out_ << endl;

        out_ << indent << du::i_tree_dumpable::tag
             << "Hits buffer capacity     : "
             << _hits_buffer_capacity_ << endl;
      }

      // {
      //   out_ << indent << du::i_tree_dumpable::tag
      //        << "Properties : "
      //        << endl;
      //   {
      //     ostringstream indent_oss;
      //     indent_oss << indent;
      //     indent_oss << du::i_tree_dumpable::skip_tag;
      //     _aux.tree_dump (out_, "", indent_oss.str ());
      //   }
      // }

      // Associated step hit processor
      {
        out_ << indent << du::i_tree_dumpable::last_tag
             << "Step hit processors      : ";

        if (_hit_processors_.size () == 0) {
          out_ << "No" << endl;
        } else {
          out_ << _hit_processors_.size () << endl;
          for (hit_processor_dict_type::const_iterator ihp
                 = _hit_processors_.begin ();
               ihp != _hit_processors_.end ();
               ++ihp) {
            const string & hp_name = ihp->first;
            mctools::base_step_hit_processor * hp = ihp->second;

            out_ << indent << du::i_tree_dumpable::last_skip_tag;
            hit_processor_dict_type::const_iterator jhp = ihp;
            if (++jhp == _hit_processors_.end ()) {
              out_ << du::i_tree_dumpable::last_tag;
            } else {
              out_ << du::i_tree_dumpable::tag;
            }
            out_ << "Processor[" << std::distance (_hit_processors_.begin (), ihp)
                 << "]: '" << hp_name
                 << "' @ " << hp << " with hit category '"
                 << hp->get_hit_category () << "'" << endl;
          }
        }
      }
      return;
    }

  } // end of namespace g4

} // end of namespace mctools

// end of sensitive_detector.cc
