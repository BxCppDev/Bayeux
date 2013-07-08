// -*- mode: c++ ; -*-
/* event_action.cc
 */

#include <mctools/g4/event_action.h>

#include <datatools/ioutils.h>
#include <datatools/properties.ipp>
#include <cstdlib>

#include <mctools/simulated_data.ipp>

#include <mctools/g4/run_action.h>
#include <mctools/g4/detector_construction.h>
#include <mctools/g4/simulation_ctrl.h>

#include <boost/algorithm/string/predicate.hpp>

// G4 stuff:
#include <globals.hh>
#include <G4ParticleTable.hh>
#include <G4RunManager.hh>
#include <G4Event.hh>
#include <G4EventManager.hh>
#include <G4TrajectoryContainer.hh>
#include <G4Trajectory.hh>
#include <G4VVisManager.hh>
#include <G4ios.hh>

#include <mctools/g4/run_action.h>
#include <mctools/g4/detector_construction.h>
#include <mctools/g4/sensitive_detector.h>
#include <mctools/g4/manager.h>

namespace mctools {

  namespace g4 {

    using namespace std;

    const run_action & event_action::get_run_action () const
    {
      return *_run_action_;
    }

    run_action & event_action::grab_run_action ()
    {
      return *_run_action_;
    }

    const event_action::sim_data_type & event_action::get_event_data () const
    {
      if (_external_event_data_ != 0) return *_external_event_data_;
      return _event_data_;
    }

    event_action::sim_data_type & event_action::grab_event_data ()
    {
      if (_external_event_data_ != 0) return *_external_event_data_;
      return _event_data_;
    }

    void event_action::set_external_event_data (sim_data_type & a_external_event_data)
    {
      _external_event_data_ = &a_external_event_data;
      return;
    }

    bool event_action::is_initialized () const
    {
      return _initialized_;
    }

    // ctor:
    event_action::event_action (run_action & run_action_,
                                const detector_construction & dctor_)
    {
      _initialized_           = false;
      _run_action_            = &run_action_;
      _detector_construction_ = &dctor_;
      _aborted_event_         = false;
      _external_event_data_   = 0;
      return;
    }

    // dtor:
    event_action::~event_action ()
    {
      if (is_initialized ()) {
        reset ();
      }
      DT_LOG_TRACE(_logprio(), "Done.");
      return;
    }

    void event_action::reset ()
    {
      DT_THROW_IF(! is_initialized (), logic_error, "Not initialized !");
      _at_reset_ ();
      _initialized_ = false;
      return;
    }

    void event_action::_at_init_ ()
    {
      return;
    }

    void event_action::_at_reset_ ()
    {
      return;
    }

    void event_action::initialize (const datatools::properties & config_)
    {
      DT_THROW_IF (is_initialized (), logic_error, "Already initialized !");

      loggable_support::_initialize_logging_support(config_);

      if (config_.has_key ("event_model.hit_collection_type")) {
        string event_model_collection_type
          = config_.fetch_string ("event_model.hit_collection_type");
        event_action::sim_data_type & event_data = this->grab_event_data ();
        event_data.reset_collection_type ();
        if (event_model_collection_type == "plain") {
          event_data.set_collection_type (sim_data_type::PLAIN_HIT_COLLECTION_TYPE);
        } else if (event_model_collection_type == "handle") {
          event_data.set_collection_type (sim_data_type::HANDLE_HIT_COLLECTION_TYPE);
        } else {
          DT_THROW_IF(true, logic_error, "Invalid hit collection type '" << event_model_collection_type << "' !");
        }
      }
      // end of fetching.

      _at_init_ ();

      _initialized_ = true;
      return;
    }

    bool event_action::is_aborted_event () const
    {
      return _aborted_event_;
    }

    void event_action::set_aborted_event (bool a_)
    {
      _aborted_event_ = a_;
      return;
    }

    void event_action::BeginOfEventAction (const G4Event * event_)
    {
      DT_LOG_TRACE(_logprio(), "Entering...");
      G4int event_id = event_->GetEventID ();
      DT_LOG_DEBUG(_logprio(), "Event #" << event_id << " starts.");
      if (_run_action_->has_number_of_events_modulo ()) {
        if ((event_id % _run_action_->get_number_of_events_modulo ()) == 0) {
          // Trace explicitely the event number :
          DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, "Event #" << event_id);
        }
      }
      if (is_aborted_event ()) {
        G4RunManager::GetRunManager ()->AbortEvent ();
        DT_LOG_WARNING(_logprio(), "Event #" << event_id << " is aborted. Exiting.");
        return;
      }

      /*
        if (G4RunManager::GetRunManager ()->runAborted)
        {
        G4RunManager::GetRunManager ()->AbortEvent ();
        }
      */

      bool record_prng_states = true;
      if (record_prng_states) {
        const manager & const_mgr = _run_action_->get_manager ();
        if (const_mgr.has_prng_state_save_modulo ()) {
          DT_LOG_DEBUG(_logprio(), "Record PRNG states for event #" << event_id);
          if ((event_id % const_mgr.get_prng_state_save_modulo ()) == 0) {
            manager * mgr = const_cast<manager *> (&const_mgr);
            mgr->record_current_prng_states ();
          }
        }
      }

      if (_run_action_->get_manager ().using_time_stat ()) {
        _run_action_->grab_manager ().grab_CT_map ()["EA"].start ();
      }

      DT_LOG_TRACE(_logprio(), "Exiting.");
      return;
    }

    void event_action::EndOfEventAction (const G4Event * event_)
    {
      DT_LOG_TRACE(_logprio(), "Entering...");

      if (_run_action_->get_manager ().using_time_stat ()) {
        _run_action_->grab_manager ().grab_CT_map ()["EA"].stop ();
      }

      G4int event_id = event_->GetEventID ();

      if (is_aborted_event ()) {
        DT_LOG_WARNING(_logprio(),  "Event is aborted. Exiting...");
        return;
      }
      DT_LOG_DEBUG(_logprio(),  "# Event " << event_id);
      bool draw_trajectory = _run_action_->get_manager ().is_interactive ();
      if (draw_trajectory) {
        // get number of stored trajectories:
        G4TrajectoryContainer * trajectory_col
          = event_->GetTrajectoryContainer ();
        int n_trajectories = 0;
        if (trajectory_col) n_trajectories = trajectory_col->entries ();

        // extract the trajectories and draw them:
        if (G4VVisManager::GetConcreteInstance ()) {
          for (int i = 0; i < n_trajectories; i++) {
            G4Trajectory* trj
              = (G4Trajectory*) ((*(event_->GetTrajectoryContainer ()))[i]);
            trj->DrawTrajectory ();
          }
        }
      }

      if (_run_action_->get_manager ().using_time_stat ()) {
        _run_action_->grab_manager ().grab_CT_map ()["HP"].start ();
      }

      // Process the list of sensitive hits:
      G4HCofThisEvent * HCE = event_->GetHCofThisEvent ();

      DT_LOG_DEBUG(_logprio(), "List of sensitive hit collections : ");
      for (int i = 0; i < (int) HCE->GetCapacity (); i++ ) {
        G4VHitsCollection * hc = HCE->GetHC (i);
        if (hc != 0) {
          DT_LOG_DEBUG(_logprio(), "Hit collection '" << hc->GetName()
                       << "' for sensitive detector '" << hc->GetSDname()<< "' @ " << hc);
        }
      }

      // Flag to discard events without sensitive hits:
      bool save_this_event = ! _run_action_->get_manager ().dont_save_no_sensitive_hit_events ();

      // Loop on the dictionnary of sensitive detectors:
      for (detector_construction::sensitive_detector_dict_type::const_iterator iSD
             = _detector_construction_->get_sensitive_detectors ().begin ();
           iSD != _detector_construction_->get_sensitive_detectors ().end ();
           iSD++) {
        const string & sensitive_category = iSD->first;
        sensitive_detector & the_detector = *iSD->second;

        DT_LOG_DEBUG(_logprio(), "Processing hits from sensitive detector '"
                     << sensitive_category << "'...");
        // sensitive_detector::make_hit_collection_name (sensitive_category)
        G4VHitsCollection * the_hits_collection = HCE->GetHC (the_detector.get_HCID ());
        if (the_hits_collection == 0) {
          DT_LOG_DEBUG(_logprio(),
                       "No hit to process from sensitive detector '"
                       << sensitive_category << "'...");
          continue;
        }

        /** Note:
         * MC hit categories with a name starting with '_' (double underscore)
         * are conventionnaly considered as 'private' hit categories, added by the user
         * for special purpose (debug, visualization...).
         * 'official' MC hit categories should never starts with '_' and should
         * be associated to some sensitive detector.
         */
        const bool is_private_category = boost::starts_with (sensitive_category, "__");
        if (! is_private_category) {
          // keep event with 'physical' sensitive hits (not visual
          // ones)
          DT_LOG_DEBUG(_logprio(), "Saving current event with 'physical' sensitive hits");
          save_this_event = true;
        }

        sensitive_hit_collection * SHC
          = dynamic_cast<sensitive_hit_collection *> (the_hits_collection);
        DT_LOG_DEBUG(_logprio(), "Collection has " << SHC->GetSize () << " hits.");

        /** Search for some step hit processors attached to the
         * sensitive detector: one can have several step hit
         * processors associated to a single sensitive detector.
         */
        //_phits_.clear ();
        if (the_detector.get_hit_processors ().size () > 0) {
          // Collect a 'phits' vector of pointers on 'base step hits':
          ::mctools::base_step_hit_processor::step_hit_ptr_collection_type phits;
          // 2011-04-05 FM : this should accelerate the process a few :
          phits.reserve (SHC->get_hits ().size ());
          for (size_t ihit = 0; ihit < SHC->get_hits ().size (); ihit++) {
            phits.push_back (&(SHC->grab_hits ().at (ihit)->grab_hit_data ()));
          }

          // 'phits' is used as the input of the 'process' method
          // from each processor attached to the sensitive
          // category/detector:
          for (sensitive_detector::hit_processor_dict_type::iterator iproc
                 = the_detector.grab_hit_processors ().begin ();
               iproc != the_detector.grab_hit_processors ().end ();
               iproc++) {
            /*
              const string & hit_proc_name = iproc->first;
              cerr << "DEVEL: " << "mctools::g4::event_action::EndOfEventAction: "
              << "hit_proc_name = `" << hit_proc_name << "'" << endl;
            */
            ::mctools::base_step_hit_processor * hit_proc
              = iproc->second;
            hit_proc->process (phits, grab_event_data ());
          }
        } else {
          // No hit processor for this sensitive
          // category/detector: step hits are dropped.
        }
      }
      if (_run_action_->get_manager ().using_time_stat ())  {
        _run_action_->grab_manager ().grab_CT_map ()["HP"].stop ();
      }

      DT_LOG_DEBUG(_logprio(), "Event data: ");
      if (is_debug()) {
        get_event_data ().tree_dump (clog);
        //get_event_data ().get_primary_event ().dump (clog, "Primary event (genbb format): ");
      }

      _run_action_->increment_number_of_processed_events ();
      if (_run_action_->save_data () && save_this_event) {
        if (_run_action_->get_manager ().using_time_stat ()) {
          _run_action_->grab_manager ().grab_CT_map ()["IO"].start ();
        }
        // Store the event if run_action has an initialized writer:
        if (_run_action_->get_brio_writer ().is_opened ()) {
          // first check 'brio::writer' :
          _run_action_->grab_brio_writer ().store (get_event_data ());
        } else if (_run_action_->get_writer ().is_initialized ()) {
          // then check 'datatools::serialization::data_writer' :
          _run_action_->grab_writer ().store (get_event_data ());
        }
        if (_run_action_->get_manager ().using_time_stat ()) {
          _run_action_->grab_manager ().grab_CT_map ()["IO"].stop ();
        }
        _run_action_->increment_number_of_saved_events ();
      }

      // Detach the hits collections from this event :
      for (int i = 0; i < (int) HCE->GetCapacity (); i++ )
        {
          G4VHitsCollection * hcol = HCE->GetHC (i);
          if (hcol != 0) {
            // clog << datatools::io::devel
            //           << "event_action::EndOfEventAction: Detach '"
            //           << hcol->GetName () << "' hits collection"
            //           << endl;
            HCE->AddHitsCollection (i, 0);
          }
        }

      // clog << datatools::io::devel << "event_action::EndOfEventAction: "
      //           << "List of sensitive hit collections:";
      // for (size_t i = 0; i < HCE->GetCapacity (); i++ )
      //        {
      //          clog << " " << HCE->GetHC (i);
      //        }
      // clog << endl;

      DT_LOG_DEBUG(_logprio(), "Check point for Event #" << event_id);

      // Debugging...
      bool must_abort_run = false;

      /*************************************************************/
      // External threaded run control :
      if (_run_action_->get_manager ().has_simulation_ctrl ())
        {
          DT_LOG_TRACE(_logprio(), "Using external run control...");
          simulation_ctrl & SimCtrl = _run_action_->grab_manager ().grab_simulation_ctrl ();
          {
            DT_LOG_TRACE(_logprio(), "Acquire the event control lock...");
            boost::mutex::scoped_lock lock (*SimCtrl.event_mutex);
            DT_LOG_TRACE(_logprio(), "Wait for event control to be available again...");
            while (SimCtrl.event_availability_status != simulation_ctrl::AVAILABLE_FOR_G4) {
              DT_LOG_TRACE(_logprio(), "Not yet...");
              SimCtrl.event_available_condition->wait (*SimCtrl.event_mutex);
            }
            DT_LOG_TRACE(_logprio(), "Ok let's go on...");
            if (SimCtrl.event_availability_status == simulation_ctrl::ABORT)
              {
                must_abort_run = true;
              }
            SimCtrl.event_availability_status = simulation_ctrl::NOT_AVAILABLE_FOR_G4;
            SimCtrl.counts++;
            DT_LOG_TRACE(_logprio(), "Notify the external simulation run manager...");
            SimCtrl.event_available_condition->notify_one ();
          }

          // Wait for the release of the event control by the external process :
          {
            DT_LOG_TRACE(_logprio(), "Wait for the release of the event control by the external process...");
            boost::mutex::scoped_lock lock (*SimCtrl.event_mutex);
            while (SimCtrl.event_availability_status == simulation_ctrl::NOT_AVAILABLE_FOR_G4) {
              DT_LOG_TRACE(_logprio(), "Not yet...");
              SimCtrl.event_available_condition->wait (*SimCtrl.event_mutex);
            }
            DT_LOG_TRACE(_logprio(), "Ok ! The event control is released by the external process...");
            if (SimCtrl.event_availability_status == simulation_ctrl::ABORT) {
              DT_LOG_WARNING(_logprio(),
                             "Detected an 'Abort' request from the external process...");
              must_abort_run = true;
            }
          }

          if (SimCtrl.is_stop_requested ()) {
            DT_LOG_TRACE(_logprio(), "is_stop_requested..." );
            must_abort_run = true;
          }

          if (SimCtrl.max_counts > 0 && (SimCtrl.counts > SimCtrl.max_counts)) {
            DT_LOG_TRACE(_logprio(), "Max counts was reached.");
            must_abort_run = true;
          }
        }
      /*************************************************************/

      // Abort run condition :
      if (must_abort_run) {
        G4RunManager::GetRunManager ()->AbortRun ();
      }

      DT_LOG_TRACE(_logprio(), "Exiting.");
      return;
    } // EndOfEventAction

  } // end of namespace g4

} // end of namespace mctools

/** Opening macro for implementation
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(mctools::g4::event_action,ocd_)
{
  // The class name :
  ocd_.set_class_name ("mctools::g4::event_action");

  // The class terse description :
  ocd_.set_class_description ("The Geant4 simulation mandatory event action");

  // The library the class belongs to :
  ocd_.set_class_library ("mctools_g4");

  // The class detailed documentation :
  ocd_.set_class_documentation ("This is Geant4 simulation engine embedded event action.      \n"
                                );

  {
    // Description of the 'logging.priority' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("logging.priority")
      .set_terse_description("Logging priority threshold")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("Allowed values are:                                    \n"
                            "                                                       \n"
                            " * ``\"fatal\"``       : print fatal error messages    \n"
                            " * ``\"critical\"``    : print critical error messages \n"
                            " * ``\"error\"``       : print error messages          \n"
                            " * ``\"warning\"``     : print warnings                \n"
                            " * ``\"notice\"``      : print notice messages         \n"
                            " * ``\"information\"`` : print informational messages  \n"
                            " * ``\"debug\"``       : print debug messages          \n"
                            " * ``\"trace\"``       : print trace messages          \n"
                            "                                                       \n"
                            "Default value: ``\"warning\"``                         \n"
                            "                                                       \n"
                            "Example::                                              \n"
                            "                                                       \n"
                            "  logging.priority : string = \"warning\"              \n"
                            "                                                       \n"
                            )
      ;
  }

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Typical configuration is::                                             \n"
                               "                                                                       \n"
                               " #@description Event action logging priority                           \n"
                               " logging.priority : string = \"warning\"                               \n"
                               "                                                                       \n"
                               );

  ocd_.set_validation_support(true);

  // Lock the description:
  ocd_.lock();

  // ... and we are done.
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation

// Registration macro for class 'mctools::g4::event_action' :
DOCD_CLASS_SYSTEM_REGISTRATION(mctools::g4::event_action,"mctools::g4::event_action")


// end of event_action.cc
