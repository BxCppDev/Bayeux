/* simulation_module.cc */

// Ourselves:
#include <mctools/g4/simulation_module.h>

// Standard library:
#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>

// Third party:
// - Boost:
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/service_manager.h>
#include <datatools/exception.h>
// - Bayeux/geomtools:
#include <geomtools/geometry_service.h>
// - Bayeux/mctools:
#include <mctools/simulated_data.h>
#include <mctools/utils.h>

// This project:
#include <mctools/g4/manager.h>
#include <mctools/g4/event_action.h>
#include <mctools/g4/simulation_ctrl.h>

namespace mctools {

  namespace g4 {

    // Registration instantiation macro :
    DPP_MODULE_REGISTRATION_IMPLEMENT(simulation_module, "mctools::g4::simulation_module");

    void simulation_module::set_geo_label(const std::string & geo_)
    {
      DT_THROW_IF (is_initialized (), std::logic_error,
                   "Module '" << get_name () << "' is already initialized !");
      _Geo_label_ = geo_;
      return;
    }

    const std::string & simulation_module::get_geo_label() const
    {
      return _Geo_label_;
    }

    void simulation_module::set_sd_label(const std::string & sd_)
    {
      DT_THROW_IF (is_initialized (), std::logic_error,
                   "Module '" << get_name () << "' is already initialized !");
      _SD_label_ = sd_;
      return;
    }

    const std::string & simulation_module::get_sd_label() const
    {
      return _SD_label_;
    }

    void simulation_module::set_erase_former_SD_bank(bool e_)
    {
      DT_THROW_IF (is_initialized (), std::logic_error,
                   "Module '" << get_name () << "' is already initialized !");
      _erase_former_SD_bank_ = e_;
      return;
    }

    bool simulation_module::is_erase_former_SD_bank() const
    {
      return _erase_former_SD_bank_;
    }

    void simulation_module::set_geometry_manager (const geomtools::manager & geometry_manager_)
    {
      DT_THROW_IF (_geometry_manager_ != 0 && _geometry_manager_->is_initialized (),
                   std::logic_error,
                   "Embedded geometry manager is already initialized !");
      _geometry_manager_ = &geometry_manager_;
      return;
    }

    void simulation_module::set_simulation_manager_params(const manager_parameters & params_)
    {
      DT_THROW_IF (is_initialized (), std::logic_error,
                   "Module '" << get_name () << "' is already initialized !");
      _simulation_manager_params_ = params_;
      return;
    }

    const manager_parameters & simulation_module::get_simulation_manager_params() const
    {
      return _simulation_manager_params_;
    }

    // Constructor :
    simulation_module::simulation_module (datatools::logger::priority logging_priority)
      : dpp::base_module(logging_priority)
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
                                        dpp::module_handle_dict_type & /*module_dict_*/)
    {
      DT_THROW_IF (is_initialized(), std::logic_error,
                   "Module '" << get_name() << "' is already initialized !");

      // Parsing configuration starts here :

      _common_initialize(config_);

      if (! _erase_former_SD_bank_) {
        if (config_.has_flag ("erase_former_SD_bank")) {
          _erase_former_SD_bank_ = true;
        }
      }
      if (_Geo_label_.empty()) {
        if (config_.has_key("Geo_label")) {
          _Geo_label_ = config_.fetch_string("Geo_label");
        }
      }

      if (_SD_label_.empty()) {
        if (config_.has_key("SD_label")) {
          _SD_label_ = config_.fetch_string("SD_label");
        }
      }

      // Special setup parameters for the mctools::g4 simulation manager :

      // Force non-interactive parameters:
      _simulation_manager_params_.interactive = false;
      _simulation_manager_params_.g4_visu     = false;
      _simulation_manager_params_.g4_macro    = "";
      _simulation_manager_params_.output_data_file.clear();

      if (_simulation_manager_params_.logging == "fatal") {
        if (config_.has_flag("manager.logging.priority")) {
          _simulation_manager_params_.logging = config_.fetch_string("manager.logging.priority");
        } else if (config_.has_flag("manager.debug")) {
          _simulation_manager_params_.logging = "debug";
        } else if (config_.has_flag("manager.verbose")) {
          _simulation_manager_params_.logging = "notice";
        }
      }

      if (_simulation_manager_params_.manager_config_filename.empty()) {
        if (config_.has_key("manager.configuration_filename")) {
          _simulation_manager_params_.manager_config_filename
            = config_.fetch_string("manager.configuration_filename");
        }
      }

      if (_simulation_manager_params_.mgr_seed == mygsl::random_utils::SEED_INVALID) {
        if (config_.has_key("manager.seed")) {
          _simulation_manager_params_.mgr_seed
            = config_.fetch_integer("manager.seed");
        }
      }

      if (_simulation_manager_params_.vg_name.empty()) {
        if (config_.has_key("manager.vertex_generator_name")) {
          _simulation_manager_params_.vg_name
            = config_.fetch_string("manager.vertex_generator_name");
        }
      }

      if (_simulation_manager_params_.vg_seed == mygsl::random_utils::SEED_INVALID) {
        if (config_.has_key("manager.vertex_generator_seed")) {
          _simulation_manager_params_.vg_seed
            = config_.fetch_integer("manager.vertex_generator_seed");
        }
      }

      if (_simulation_manager_params_.eg_name.empty()) {
        if (config_.has_key("manager.event_generator_name")) {
          _simulation_manager_params_.eg_name
            = config_.fetch_string("manager.event_generator_name");
        }
      }

      if (_simulation_manager_params_.eg_seed == mygsl::random_utils::SEED_INVALID) {
        if (config_.has_key("manager.event_generator_seed")) {
          _simulation_manager_params_.eg_seed
            = config_.fetch_integer("manager.event_generator_seed");
        }
      }

      if (_simulation_manager_params_.shpf_seed == mygsl::random_utils::SEED_INVALID) {
        if (config_.has_key("manager.shpf_seed")) {
          _simulation_manager_params_.shpf_seed
            = config_.fetch_integer("manager.shpf_seed");
        }
      }

      if (_simulation_manager_params_.input_prng_seeds_file.empty()) {
        if (config_.has_key("manager.input_prng_seeds_file")) {
          _simulation_manager_params_.input_prng_seeds_file
            = config_.fetch_string("manager.input_prng_seeds_file");
        }
      }

      if (_simulation_manager_params_.output_prng_seeds_file.empty()) {
        if (config_.has_key("manager.output_prng_seeds_file")) {
          _simulation_manager_params_.output_prng_seeds_file
            = config_.fetch_string("manager.output_prng_seeds_file");
        }
      }

      if (_simulation_manager_params_.input_prng_states_file.empty()) {
        if (config_.has_key("manager.input_prng_states_file")) {
          _simulation_manager_params_.input_prng_states_file
            = config_.fetch_string("manager.input_prng_states_file");
        }
      }

      if (_simulation_manager_params_.output_prng_states_file.empty()) {
        if (config_.has_key("manager.output_prng_states_file")) {
          _simulation_manager_params_.output_prng_states_file
            = config_.fetch_string("manager.output_prng_states_file");
        }
      }

      if (_simulation_manager_params_.prng_states_save_modulo == 0) {
        if (config_.has_key("manager.prng_states_save_modulo")) {
          _simulation_manager_params_.prng_states_save_modulo
            = config_.fetch_integer("manager.prng_states_save_modulo");
        }
      }

      if (_simulation_manager_params_.number_of_events_modulo == 0) {
        if (config_.has_key("manager.number_of_events_modulo")) {
          _simulation_manager_params_.number_of_events_modulo
            = config_.fetch_integer("manager.number_of_events_modulo");
        }
      }

      // More setup parameters can be added.

      if (get_logging_priority() >= datatools::logger::PRIO_TRACE) {
        _simulation_manager_params_.print(std::clog);
      }
      // Parsing configuration stops here .

      // Initialization starts here :

      if (_geometry_manager_ == 0)  {
        // Access to the Geometry Service :
        DT_THROW_IF (_Geo_label_.empty (), std::logic_error,
                     "Module '" << get_name ()
                     << "' has no valid '" << "Geo_label" << "' property !");
        if (service_manager_.has (_Geo_label_)
            && service_manager_.is_a<geomtools::geometry_service> (_Geo_label_)) {
            // Fetch a reference to the geometry service :
            geomtools::geometry_service & Geo
              = service_manager_.get<geomtools::geometry_service> (_Geo_label_);
            // Request for a reference to the geometry manager and installation
            // in the simulation manager :
            this->set_geometry_manager (Geo.get_geom_manager ());
          } else {
          DT_THROW_IF (true, std::logic_error,
                       "Module '" << get_name ()
                       << "' has no '" << _Geo_label_ << "' geometry service !");
        }
      }

      DT_THROW_IF (_geometry_manager_ == 0, std::logic_error, "Missing geometry manager !");

      if (_SD_label_.empty ()) {
        // Use the default label for the 'simulated data' bank :
        _SD_label_ = ::mctools::event_utils::EVENT_DEFAULT_SIMULATED_DATA_LABEL; // "SD"
      }

      // Allocate internal resources :
      _initialize_manager (service_manager_);

      DT_LOG_NOTICE(get_logging_priority(), "Initialization done.");
      // Initialization stops here.

      _set_initialized (true);
      return;
    }

    // Reset :
    void simulation_module::reset ()
    {
      DT_LOG_TRACE_ENTERING(get_logging_priority());
      DT_THROW_IF (! is_initialized (), std::logic_error,
                   "Module '" << get_name () << "' is not initialized !");

      _set_initialized (false);

      if (_simulation_ctrl_ != 0) {
        // Destruction of the thread synchronization object :
        _simulation_ctrl_->set_stop_requested();
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

      DT_LOG_TRACE_EXITING(get_logging_priority());
      return;
    }

    // Processing :
    dpp::base_module::process_status
    simulation_module::process(datatools::things & event_record_)
    {
      DT_THROW_IF (! is_initialized (), std::logic_error,
                   "Module '" << get_name () << "' is not initialized !");
      int status = _simulate_event(event_record_);
      return status == 0 ? dpp::base_module::PROCESS_OK : dpp::base_module::PROCESS_FATAL;
    }

    void simulation_module::_initialize_manager(datatools::service_manager & smgr_)
    {
      // Allocate the simulation manager :
      _simulation_manager_ = new manager;

      // 2012-04-30 FM: add support for a handle to the service manager
      _simulation_manager_->set_service_manager(smgr_);

      // Install the geometry manager accessed from the
      // Geometry service (bypassing the embedded geometry manager
      // in the simulation manager) :
      _simulation_manager_->set_external_geom_manager(*_geometry_manager_);

      // Install a simulation controler in the manager :
      if (_simulation_ctrl_ == 0) {
        DT_LOG_TRACE(get_logging_priority(),
                     "Allocating the 'simulation_ctrl' object...");
        _simulation_ctrl_ = new simulation_ctrl(*_simulation_manager_);
        DT_LOG_TRACE(get_logging_priority(),
                     "Install the 'simulation_ctrl' object in the G4 simulation manager...");
        _simulation_manager_->set_simulation_ctrl(*_simulation_ctrl_);
        DT_LOG_TRACE(get_logging_priority(),
                     "New 'simulation_ctrl' object is allocated.");
      }

      // Setup :
      mctools::g4::manager_parameters::setup (_simulation_manager_params_,
                                              *_simulation_manager_);

      return;
    }

    void simulation_module::_terminate_manager ()
    {

      if (_simulation_ctrl_ != 0) {
        delete _simulation_ctrl_;
        _simulation_ctrl_ = 0;
      }

      if (_simulation_manager_ !=  0) {
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
      if (event_record_.has(_SD_label_) &&
          event_record_.is_a<mctools::simulated_data>(_SD_label_)) {
        ptr_SD = &event_record_.grab<mctools::simulated_data>(_SD_label_);
        former_SD = true;
      } else {
        DT_LOG_TRACE(get_logging_priority(),
                     "Add the 'SD' bank in the event record...");
        ptr_SD = &event_record_.add<mctools::simulated_data>(_SD_label_);
      }
      mctools::simulated_data & SD = *ptr_SD;
      DT_THROW_IF(former_SD && ! _erase_former_SD_bank_,
                  std::logic_error,
                  "Module '" << get_name () << "' cannot remove former '"
                  << _SD_label_ <<  "' bank from simulated data !");

      DT_LOG_TRACE(get_logging_priority(),
                   "Clear the '" << _SD_label_ << "' bank from the event record...");
      SD.clear();

      DT_LOG_TRACE(get_logging_priority(),
                  "Pass the 'simulated_data' '" << _SD_label_
                   << "' object to the bank from the event record...");
      _simulation_manager_->grab_user_event_action().set_external_event_data(SD);

      {
        // if (_simulation_ctrl_ == 0) {
        //   DT_LOG_TRACE(get_logging_priority(),
        //                "Allocating the 'simulation_ctrl' object...");
        //   _simulation_ctrl_ = new simulation_ctrl (*_simulation_manager_);
        //   DT_LOG_TRACE(get_logging_priority(),
        //                "Install the 'simulation_ctrl' object in the G4 simulation manager...");
        //   _simulation_manager_->set_simulation_ctrl (*_simulation_ctrl_);
        //   DT_LOG_TRACE(get_logging_priority(),
        //                "New 'simulation_ctrl' object is allocated.");
        // }
        {
          DT_LOG_TRACE(get_logging_priority(),
                       "Acquire the event control lock...");
          boost::mutex::scoped_lock lock(*_simulation_ctrl_->event_mutex);

          if (_simulation_ctrl_->simulation_thread == 0) {
            DT_LOG_TRACE(get_logging_priority(),
                         "Starting the G4 simulation manager from its own thread...");
            _simulation_ctrl_->start();
            DT_LOG_TRACE(get_logging_priority(),
                         "G4 simulation manager thread started.");
            DT_LOG_TRACE(get_logging_priority(),
                         "Now wait for G4 to run an event...");
          }
          DT_LOG_TRACE(get_logging_priority(),
                       "Notify that event control is now available for the G4 simulation thread...");
          _simulation_ctrl_->event_availability_status = simulation_ctrl::AVAILABLE_FOR_G4;
          _simulation_ctrl_->event_available_condition->notify_one();
        }

        // Wait for the release of the event control by the G4 process :
        {
          DT_LOG_TRACE(get_logging_priority(),
                       "Wait for the release of the event control by the G4 simulation thread...");
          boost::mutex::scoped_lock lock(*_simulation_ctrl_->event_mutex);
          while (_simulation_ctrl_->event_availability_status == simulation_ctrl::AVAILABLE_FOR_G4) {
            _simulation_ctrl_->event_available_condition->wait(*_simulation_ctrl_->event_mutex);
          }
          DT_LOG_TRACE(get_logging_priority(),
                       "Ok ! The event control is released by the G4 simulation thread...");
        }

      }
      return 0;
    }

  }  // end of namespace g4

}  // end of namespace mctools

// OCD support for class 'mctools::g4::simulation_module' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::mctools::g4::simulation_module,ocd_)
{
  ocd_.set_class_name("mctools::g4::simulation_module");
  ocd_.set_class_library("mctools_g4");
  ocd_.set_class_description("A module to generate Monte-Carlo events through the Geant4 library");

  ocd_.set_configuration_hints("A ``mctools::g4::simulation_module`` object can be setup with the    \n"
                               "following syntax in a ``datatools::multi_properties`` configuration  \n"
                               "file, typically from a module manager object.                        \n"
                               "                                                                     \n"
                               "Example::                                                            \n"
                               "                                                                     \n"
                               "  #@description A module that generates raw data                     \n"
                               "  #@key_label   \"name\"                                             \n"
                               "  #@meta_label  \"type\"                                             \n"
                               "                                                                     \n"
                               "  [name=\"g4sim\" type=\"mctools::g4::simulation_module\"]           \n"
                               "  #@config A Geant4 simulation module                                \n"
                               "  foo : string = \"bar\"                                             \n"
                               "                                                                     \n"
                               );

  ocd_.set_validation_support(false);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::mctools::g4::simulation_module,"mctools::g4::simulation_module")

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
