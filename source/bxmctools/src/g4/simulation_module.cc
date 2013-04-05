/* simulation_module.cc */

#include <mctools/g4/simulation_module.h>

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

#include <datatools/properties.h>
#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/service_manager.h>

#include <geomtools/geometry_service.h>

#include <mctools/simulated_data.h>
#include <mctools/utils.h>

#include <mctools/g4/manager.h>
#include <mctools/g4/event_action.h>
#include <mctools/g4/simulation_ctrl.h>

namespace mctools {

  namespace g4 {

    using namespace std;

    // Registration instantiation macro :
    DPP_MODULE_REGISTRATION_IMPLEMENT(simulation_module, "mctools::g4::simulation_module");

    void simulation_module::set_geometry_manager (const geomtools::manager & geometry_manager_)
    {
      if (_geometry_manager_ != 0 && _geometry_manager_->is_initialized ())
        {
          ostringstream message;
          message << "mctools::g4::simulation_module::set_geometry_manager: "
                  << "Embedded geometry manager is already initialized !";
          throw logic_error (message.str ());
        }
      _geometry_manager_ = &geometry_manager_;
      return;
    }

    // Constructor :
    simulation_module::simulation_module (int debug_level_)
      : base_module ("simulation_module",
                     "Generic Geant4 simulation module",
                     "0.1",
                     debug_level_)
    {
      _Geo_label_            = "";
      _SD_label_             = "";
      _erase_former_SD_bank_ = false;
      _geometry_manager_     = 0;
      _simulation_manager_   = 0;
      _simulation_ctrl_      = 0;
      return;
    }

    // Destructor :
    simulation_module::~simulation_module ()
    {
      // Make sure all internal resources are terminated
      // before destruction :
      if (is_initialized ()) reset ();
      return;
    }

    // Initialization :
    void simulation_module::initialize (const datatools::properties  & config_,
                                        datatools::service_manager   & service_manager_,
                                        dpp::module_handle_dict_type & module_dict_)
    {
      if (is_initialized ())
        {
          ostringstream message;
          message << "mctools::g4::simulation_module::initialize: "
                  << "Module '" << get_name () << "' is already initialized ! ";
          throw logic_error (message.str ());
        }

      // Parsing configuration starts here :

      if (! is_debug ())
        {
          if (config_.has_flag ("debug"))
            {
              set_debug (true);
            }
        }

      if (! simulation_ctrl::g_devel)
        {
          if (config_.has_flag ("ctrl.devel"))
            {
              simulation_ctrl::g_devel = true;
            }
        }

      if (! _erase_former_SD_bank_)
        {
          if (config_.has_flag ("erase_former_SD_bank"))
            {
              _erase_former_SD_bank_ = true;
            }
        }
      if (config_.has_key ("Geo_label"))
        {
          _Geo_label_ = config_.fetch_string ("Geo_label");
        }

      if (config_.has_key ("SD_label"))
        {
          _SD_label_ = config_.fetch_string ("SD_label");
        }

      // Special setup parameters for the sng4 simulation manager :

      _simulation_manager_params_.interactive = false;
      _simulation_manager_params_.g4_visu     = false;
      _simulation_manager_params_.g4_macro    = "";

      if (config_.has_flag ("manager.debug"))
        {
          _simulation_manager_params_.debug = true;
        }

      if (config_.has_flag ("manager.verbose"))
        {
          _simulation_manager_params_.verbose = true;
        }

      if (config_.has_key ("manager.configuration_filename"))
        {
          _simulation_manager_params_.manager_config_filename
            = config_.fetch_string ("manager.configuration_filename");
        }

      if (config_.has_key ("manager.seed"))
        {
          _simulation_manager_params_.mgr_seed
            = config_.fetch_integer ("manager.seed");
        }

      if (config_.has_key ("manager.vertex_generator_name"))
        {
          _simulation_manager_params_.vg_name
            = config_.fetch_string ("manager.vertex_generator_name");
        }

      if (config_.has_key ("manager.vertex_generator_seed"))
        {
          _simulation_manager_params_.vg_seed
            = config_.fetch_integer ("manager.vertex_generator_seed");
        }

      if (config_.has_key ("manager.event_generator_name"))
        {
          _simulation_manager_params_.eg_name
            = config_.fetch_string ("manager.event_generator_name");
        }

      if (config_.has_key ("manager.event_generator_seed"))
        {
          _simulation_manager_params_.eg_seed
            = config_.fetch_integer ("manager.event_generator_seed");
        }

      if (config_.has_key ("manager.shpf_seed"))
        {
          _simulation_manager_params_.shpf_seed
            = config_.fetch_integer ("manager.shpf_seed");
        }

      if (config_.has_key ("manager.input_prng_seeds_file"))
        {
          _simulation_manager_params_.input_prng_seeds_file
            = config_.fetch_string ("manager.input_prng_seeds_file");
        }

      if (config_.has_key ("manager.output_prng_seeds_file"))
        {
          _simulation_manager_params_.output_prng_seeds_file
            = config_.fetch_string ("manager.output_prng_seeds_file");
        }

      if (config_.has_key ("manager.input_prng_states_file"))
        {
          _simulation_manager_params_.input_prng_states_file
            = config_.fetch_string ("manager.input_prng_states_file");
        }

      if (config_.has_key ("manager.output_prng_states_file"))
        {
          _simulation_manager_params_.output_prng_states_file
            = config_.fetch_string ("manager.output_prng_states_file");
        }

      if (config_.has_key ("manager.prng_states_save_modulo"))
        {
          _simulation_manager_params_.prng_states_save_modulo
            = config_.fetch_integer ("manager.prng_states_save_modulo");
        }

      // More setup parameters can be added.

      // Parsing configuration stops here .

      // Initialization starts here :

      if (_geometry_manager_ == 0)
        {
          // Access to the Geometry Service :
          if (_Geo_label_.empty ())
            {
              ostringstream message;
              message << "mctools::g4::simulation_module::initialize: "
                      << "Module '" << get_name ()
                      << "' has no valid '" << "Geo_label" << "' property !";
              throw logic_error (message.str ());
            }
          if (service_manager_.has (_Geo_label_)
              && service_manager_.is_a<geomtools::geometry_service> (_Geo_label_))
            {
              // Fetch a reference to the geometry service :
              geomtools::geometry_service & Geo
                = service_manager_.get<geomtools::geometry_service> (_Geo_label_);
              // Request for a reference to the geometry manager and installation
              // in the simulation manager :
              this->set_geometry_manager (Geo.get_geom_manager ());
            }
          else
            {
              ostringstream message;
              message << "mctools::g4::simulation_module::initialize: "
                      << "Module '" << get_name ()
                      << "' has no '" << _Geo_label_ << "' geometry service !";
              throw logic_error (message.str ());
            }
        }

      if (_geometry_manager_ == 0)
        {
          throw logic_error ("mctools::g4::simulation_module::initialize: Missing geometry manager !");
        }

      if (_SD_label_.empty ())
        {
          // Use the default label for the 'Simulated data' bank :
          _SD_label_ = ::mctools::event_utils::EVENT_DEFAULT_SIMULATED_DATA_LABEL; // "SD"
        }

      // Allocate internal resources :
      _initialize_manager (service_manager_);

      clog << datatools::io::notice
           << "mctools::g4::simulation_module::initialize: Initialization done." << endl;
      // Initialization stops here.

      _set_initialized (true);
      return;
    }

    // Reset :
    void simulation_module::reset ()
    {
      if (simulation_ctrl::g_devel)
        {
          cerr << "DEVEL: "
               << "mctools::g4::simulation_module::reset: Entering..."
               << endl;
        }
      if (! is_initialized ())
        {
          ostringstream message;
          message << "simulation_module::initialize: "
                  << "Module '" << get_name () << "' is not initialized !";
          throw logic_error (message.str ());
        }

      if (_simulation_ctrl_ != 0)
        {
          // Destruction of the thread synchronization object :
          _simulation_ctrl_->set_stop_requested ();
          delete _simulation_ctrl_;
          _simulation_ctrl_ = 0;
        }

      // Destroy internal resources :
      _terminate_manager ();

      // Blank the module with default neutral values :
      _geometry_manager_ = 0;
      _SD_label_  = "";
      _Geo_label_ = "";
      _erase_former_SD_bank_ = false;

      _set_initialized (false);
      if (simulation_ctrl::g_devel)
        {
          cerr << "DEVEL: "
               << "mctools::g4::simulation_module::reset: Exiting."
               << endl;
        }
      return;
    }

    // Processing :
    int simulation_module::process (datatools::things & event_record_)
    {
      if (! is_initialized ())
        {
          ostringstream message;
          message << "simulation_module::process: "
                  << "Module '" << get_name () << "' is not initialized !";
          throw logic_error (message.str ());
        }

      /* Note: 'scm::event_record' is an alias for the 'datatools::things' class */

      int status = _simulate_event(event_record_);

      return status;
    }

    void simulation_module::_initialize_manager (datatools::service_manager & smgr_)
    {
      // Allocate the simulation manager :
      _simulation_manager_ = new manager;

      // 2012-04-30 FM: add support for a handle to the service manager
      _simulation_manager_->set_service_manager (smgr_);

      // Install the geometry manager accessed from the
      // Geometry service (bypassing the embedded geometry manager
      // in the simulation manager) :
      _simulation_manager_->set_external_geom_manager (*_geometry_manager_);

      // Setup :
      mctools::g4::manager_parameters::setup (_simulation_manager_params_,
                                            *_simulation_manager_);

      return;
    }

    void simulation_module::_terminate_manager ()
    {

      if (_simulation_ctrl_ != 0)
        {
          delete _simulation_ctrl_;
          _simulation_ctrl_ = 0;
        }

      if (_simulation_manager_ !=  0)
        {
          delete _simulation_manager_;
          _simulation_manager_ = 0;
        }
      _simulation_manager_params_.reset ();
      return;
    }

    int simulation_module::_simulate_event (datatools::things & event_record_)
    {
      // Fetch the simulated data :
      mctools::simulated_data * ptr_SD = 0;
      bool former_SD = false;
      if (event_record_.has (_SD_label_) &&
          event_record_.is_a<mctools::simulated_data> (_SD_label_))
        {
          ptr_SD = &event_record_.grab<mctools::simulated_data> (_SD_label_);
          former_SD = true;
        }
      else
        {
          // if (simulation_ctrl::g_devel)
          //   cerr << "DEVEL: " << "mctools::g4::simulation_module::simulate_event_: "
          //        << "Add the 'SD' bank in the event record..." << endl;
          ptr_SD = &event_record_.add<mctools::simulated_data> (_SD_label_);
        }
      mctools::simulated_data & SD = *ptr_SD;
      if (former_SD && ! _erase_former_SD_bank_)
        {
          ostringstream message;
          message << "simulation_module::simulate_event_: "
                  << "Module '" << get_name () << "' cannot remove former '" << _SD_label_ <<  "' bank from simulated data !";
          throw logic_error (message.str ());
        }
      // if (simulation_ctrl::g_devel)
      //   cerr << "DEVEL: " << "mctools::g4::simulation_module::simulate_event_: "
      //        << "Clear the '" << _SD_label_ << "' bank from the event record..." << endl;
      SD.clear ();

      // if (simulation_ctrl::g_devel)
      //   cerr << "DEVEL: " << "mctools::g4::simulation_module::simulate_event_: "
      //        << "Pass the 'simulated_data' '" << _SD_label_
      //        << "' object to the bank from the event record..." << endl;
      _simulation_manager_->grab_user_event_action ().set_external_event_data (SD);

      {
        if (_simulation_ctrl_ == 0)
          {
            // if (simulation_ctrl::g_devel)
            //   cerr << "DEVEL: " << "mctools::g4::simulation_module::simulate_event_: "
            //        << "Allocating the 'simulation_ctrl' object..." << endl;
            _simulation_ctrl_ = new simulation_ctrl (*_simulation_manager_);
            // if (simulation_ctrl::g_devel)
            //   cerr << "DEVEL: " << "mctools::g4::simulation_module::simulate_event_: "
            //        << "Install the 'simulation_ctrl' object in the G4 simulation manager..." << endl;
            _simulation_manager_->set_simulation_ctrl (*_simulation_ctrl_);
            // if (simulation_ctrl::g_devel)
            //   cerr << "DEVEL: " << "mctools::g4::simulation_module::simulate_event_: "
            //        << "New 'simulation_ctrl' object is allocated." << endl;
          }
        {
          // if (simulation_ctrl::g_devel)
          //   cerr << "DEVEL: " << "mctools::g4::simulation_module::simulate_event_: "
          //        << "Acquire the event control lock..." << endl;
          boost::mutex::scoped_lock lock (*_simulation_ctrl_->event_mutex);

          if (_simulation_ctrl_->simulation_thread == 0)
            {
              // if (simulation_ctrl::g_devel)
              //   cerr << "DEVEL: " << "mctools::g4::simulation_module::simulate_event_: "
              //        << "Starting the G4 simulation manager from its own thread..." << endl;
              _simulation_ctrl_->start ();
              // if (simulation_ctrl::g_devel)
              //   cerr << "DEVEL: " << "mctools::g4::simulation_module::simulate_event_: "
              //        << "G4 simulation manager thread started." << endl;
              // if (simulation_ctrl::g_devel)
              //   cerr << "DEVEL: " << "mctools::g4::simulation_module::simulate_event_: "
              //        << "Now wait for G4 to run an event..." << endl;
            }
          // if (simulation_ctrl::g_devel)
          //   cerr << "DEVEL: " << "mctools::g4::simulation_module::simulate_event_: "
          //        << "Notify that event control is now available for the G4 simulation thread..." << endl;
          _simulation_ctrl_->event_availability_status = simulation_ctrl::AVAILABLE_FOR_G4;
          _simulation_ctrl_->event_available_condition->notify_one ();
        }

        // Wait for the release of the event control by the G4 process :
        {
          // if (simulation_ctrl::g_devel)
          //   cerr << "DEVEL: " << "mctools::g4::simulation_module::simulate_event_: "
          //        << "Wait for the release of the event control by the G4 simulation thread..." << endl;
          boost::mutex::scoped_lock lock (*_simulation_ctrl_->event_mutex);
          while (_simulation_ctrl_->event_availability_status == simulation_ctrl::AVAILABLE_FOR_G4)
            {
              _simulation_ctrl_->event_available_condition->wait (*_simulation_ctrl_->event_mutex);
            }
          // if (simulation_ctrl::g_devel)
          //   cerr << "DEVEL: " << "mctools::g4::simulation_module::simulate_event_: "
          //        << "Ok ! The event control is released by the G4 simulation thread..." << endl;
        }

      }
      return SUCCESS;
    }

  }  // end of namespace g4

}  // end of namespace mctools

// end of simulation_module.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
