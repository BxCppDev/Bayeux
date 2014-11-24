// run_action.cc

// Standard library:
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <map>

// Third party:
// - Boost:
#include <boost/filesystem.hpp>
#include <boost/thread/mutex.hpp>
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/properties.ipp>
// - Bayeux/mygsl:
#include <mygsl/seed_manager.h>
// - Bayeux/geomtools:
#include <geomtools/geomtools_config.h>
// - Bayeux/dpp:
#include <dpp/dpp_config.h>
// - Bayeux/genbb_help:
#include <genbb_help/genbb_help_config.h>
// - Bayeux/genvtx:
#include <genvtx/genvtx_config.h>
// - Geant4:
#include <globals.hh>
#include <G4Run.hh>
#include <G4RunManager.hh>
#include <G4UImanager.hh>
#include <G4VVisManager.hh>
#include <G4ios.hh>

// This project:
#include <mctools/simulated_data.h>
#include <mctools/mctools_config.h>
#include <mctools/utils.h>
#include <mctools/g4/run_action.h>
#include <mctools/g4/manager.h>
#include <mctools/g4/simulation_ctrl.h>

namespace mctools {

  namespace g4 {

    bool run_action::is_initialized () const
    {
      return _initialized_;
    }

    bool run_action::using_run_header_footer () const
    {
      return _use_run_header_footer_;
    }

    void run_action::set_use_run_header_footer (bool a_new_value)
    {
      DT_THROW_IF (is_initialized (), std::logic_error, "Object is locked !");
      _use_run_header_footer_ = a_new_value;
      return;
    }

    manager & run_action::grab_manager ()
    {
      return *_manager_;
    }

    const manager & run_action::get_manager () const
    {
      return *_manager_;
    }

    bool run_action::save_data () const
    {
      return _save_data_;
    }

    bool run_action::has_number_of_events_modulo () const
    {
      return _number_of_events_modulo_ > NUMBER_OF_EVENTS_MODULO_NONE;
    }

    void run_action::set_number_of_events_modulo (int modulo_)
    {
      if (modulo_ <= NUMBER_OF_EVENTS_MODULO_NONE) {
        _number_of_events_modulo_ = NUMBER_OF_EVENTS_MODULO_NONE;
      } else {
        _number_of_events_modulo_ = modulo_;
      }
      return;
    }

    int run_action::get_number_of_events_modulo () const
    {
      return _number_of_events_modulo_;
    }

    int32_t run_action::get_number_of_saved_events () const
    {
      return _number_of_saved_events_;
    }

    int32_t run_action::get_number_of_processed_events () const
    {
      return _number_of_processed_events_;
    }

    void run_action::increment_number_of_processed_events ()
    {
      _number_of_processed_events_++;
      return;
    }

    void run_action::increment_number_of_saved_events ()
    {
      _number_of_saved_events_++;
      return;
    }

    void run_action::reset_number_of_saved_events ()
    {
      _number_of_saved_events_ = 0;
      return;
    }

    void run_action::reset_number_of_processed_events ()
    {
      _number_of_processed_events_ = 0;
      return;
    }

    void run_action::set_output_file (const std::string & a_filename)
    {
      DT_THROW_IF (is_initialized (), std::logic_error,
                   "Cannot change output  file name ! Object is locked !");
      _output_file_ = a_filename;
      _save_data_ = true;
      return;
    }

    const brio::writer & run_action::get_brio_writer () const
    {
      return _brio_writer_;
    }

    brio::writer & run_action::grab_brio_writer ()
    {
      return _brio_writer_;
    }

    datatools::data_writer & run_action::grab_writer ()
    {
      return _writer_;
    }

    const datatools::data_writer & run_action::get_writer () const
    {
      return _writer_;
    }

    void run_action::_set_default()
    {
      _use_run_header_footer_                 = false;
      _number_of_processed_events_            = 0;
      _number_of_events_modulo_               = NUMBER_OF_EVENTS_MODULO_NONE;
      _save_data_                             = false;
      _output_file_preserve_                  = true;
      _output_file_dir_                       = ".";
      _output_file_prefix_                    = "mctools_g4_";
      _output_file_format_                    = "ascii";
      _output_file_compression_               = "gzip";
      _output_file_                           = "";
      _manager_                               = 0;
      _brio_general_info_store_label_         = io_utils::GENERAL_INFO_STORE;
      _brio_plain_simulated_data_store_label_ = io_utils::PLAIN_SIMULATED_DATA_STORE;
      return;
    }

    run_action::run_action(manager & a_mgr)
    {
      _initialized_ = false;
      _set_default();
      _manager_ = &a_mgr;
      return;
    }

    run_action::~run_action ()
    {
      return;
    }

    void run_action::initialize (const datatools::properties & a_config)
    {
      // Fetch parameters:
      loggable_support::_initialize_logging_support(a_config);

      if (a_config.has_flag ("file.using_run_header_footer")) {
        set_use_run_header_footer (true);
      }

      if (! has_number_of_events_modulo ()) {
        if (a_config.has_key ("number_of_events_modulo")) {
          const int number_of_events_modulo = a_config.fetch_integer ("number_of_events_modulo");
          set_number_of_events_modulo (number_of_events_modulo);
        }
      }

      if (a_config.has_key ("file.save")) {
        _save_data_ = a_config.fetch_boolean ("file.save");
      }

      if (a_config.has_flag ("file.no_preserve")) {
        _output_file_preserve_ = false;
      }

      if (_save_data_) {
        if (_output_file_.empty ()) {
          if (a_config.has_key ("file.name")) {
            const std::string output_file = a_config.fetch_string ("file.name");
            set_output_file(output_file);
          }
        }
      }

      // Inhibit the data file saving in pipeine mode :
      if (grab_manager().has_simulation_ctrl()) {
        DT_LOG_NOTICE(_logprio(), "Inhibit plain data saving in thread-driven mode !");
        _save_data_ = false;
      }

      bool search_file_directives = true;
      if (! _save_data_) {
        search_file_directives = false;
      }
      if (! _output_file_.empty ()) {
        search_file_directives = false;
      }

      if (search_file_directives) {
        if (a_config.has_key ("file.directory")) {
          _output_file_dir_ = a_config.fetch_string ("file.directory");
        }

        bool using_brio = false;
        if (a_config.has_key ("file.format")) {
          std::string format = a_config.fetch_string ("file.format");
          bool ok = false;
          if (format == "brio" || format == "trio") {
            using_brio = true;
            ok = true;
          } else if (format == "xml") ok = true;
          else if (format == "binary") ok = true;
          else if (format == "ascii") ok = true;
          if (ok) {
            _output_file_format_ = format;
          } else {
            DT_THROW_IF(true, std::logic_error, "Invalid format '" << format << "' !");
          }
        }

        if (! using_brio && a_config.has_key ("file.compression")) {
          std::string compression = a_config.fetch_string ("file.compression");
          bool ok = false;
          if (compression == "none") ok = true;
          else if (compression == "gzip") ok = true;
          else if (compression == "bzip2") ok = true;
          if (ok) {
            _output_file_compression_ = compression;
          } else {
            DT_THROW_IF(true, std::logic_error, "Invalid compression mode '" << compression << "' !");
          }
        }

        if (a_config.has_key ("file.prefix")) {
          _output_file_prefix_ = a_config.fetch_string ("file.prefix");
        }
      } // (search_file_directives)

      _initialized_ = true;
      return;
    }

    void run_action::dump (ostream & a_out) const
    {
      a_out << "run_action::dump:" << std::endl;
      a_out << "|-- Save data           : "  << _save_data_ << std::endl;
      a_out << "|-- Output file dir     : '"  << _output_file_dir_ << "'" << std::endl;
      a_out << "|-- Output file prefix  : '"  << _output_file_prefix_ << "'" << std::endl;
      a_out << "|-- Output file format  : '"  << _output_file_format_ << "'" << std::endl;
      a_out << "|-- Output compression  : '"  << _output_file_compression_ << "'" << std::endl;
      a_out << "`-- Output file         : '"  << _output_file_ << "'" << std::endl;
      return;
    }

    void run_action::_build_run_header ()
    {
      _run_header_.set_description ("mctools::g4 run header");

      const mygsl::seed_manager & the_seed_manager = _manager_->get_seed_manager ();
      std::vector<std::string> seed_labels;
      the_seed_manager.get_labels (seed_labels);
      for (int i = 0; i < (int) seed_labels.size (); i++) {
        int32_t seed = the_seed_manager.get_seed (seed_labels[i]);
        std::ostringstream seed_key;
        seed_key << "seed." << seed_labels[i];
        _run_header_.store (seed_key.str (), seed);
      }

      return;
    }

    void run_action::_build_run_footer ()
    {
      _run_footer_.set_description ("mctools::g4 run footer");

      _run_footer_.store ("number_of_processed_events", _number_of_processed_events_);
      _run_footer_.store ("number_of_saved_events", _number_of_saved_events_);

      DT_LOG_NOTICE(_logprio(), "Number of processed events = " << _number_of_processed_events_);
      if (_save_data_) {
        DT_LOG_NOTICE(_logprio(), "Number of saved events = " << _number_of_saved_events_);
      }
      return;
    }

    void run_action::BeginOfRunAction (const G4Run* a_run)
    {
      std::cout << std::flush;
      std::cerr << std::flush;
      std::clog << std::flush;
      DT_LOG_NOTICE(_logprio(),"# Run " << a_run->GetRunID () << " is starting...");
      _number_of_processed_events_ = 0;
      _number_of_saved_events_ = 0;

      /*************************************************************/
      // External threaded run control :
      if (get_manager ().has_simulation_ctrl ()) {
        DT_LOG_TRACE(_logprio(),"Check point.");
        simulation_ctrl & SimCtrl
          = grab_manager ().grab_simulation_ctrl ();
        {
          DT_LOG_TRACE(_logprio(),"Acquire the lock...");
          boost::mutex::scoped_lock lock (*SimCtrl.event_mutex);
          DT_LOG_TRACE(_logprio(),
                       "Wait for the event control to be available again "
                       << "for G4 run start and G4 event loop start ...");
          while (SimCtrl.event_availability_status == simulation_ctrl::NOT_AVAILABLE_FOR_G4) {
            DT_LOG_TRACE(_logprio(), "Not yet...");
            SimCtrl.event_available_condition->wait (*SimCtrl.event_mutex);
          }
          if (SimCtrl.event_availability_status == simulation_ctrl::ABORT) {
            DT_LOG_WARNING(_logprio(), "Detect a 'Abort' request from the external process !");
            G4RunManager::GetRunManager ()->AbortRun ();
          }
          DT_LOG_TRACE(_logprio(), "Starting the G4 run...");
        }
      }

      /*************************************************************/

      // open the data writer:
      if (_save_data_) {
        bool using_brio = false;
        bool output_file_directives = false;
        if (_output_file_.empty ()) {
          std::ostringstream output_file_oss;
          if (! _output_file_dir_.empty ()) {
            output_file_oss << _output_file_dir_;
            if (_output_file_dir_[_output_file_dir_.size () - 1] != '/') {
              output_file_oss << '/';
            }
          }

          // Add the file prefix:
          output_file_oss << _output_file_prefix_;

          // Run number suffix:
          output_file_oss << a_run->GetRunID ();

          // Boost serialization format extension:
          if (_output_file_format_ == "ascii") {
            output_file_oss << '.' << datatools::io_factory::format::text_extension ();
          } else if (_output_file_format_ == "binary") {
            output_file_oss << '.' << datatools::io_factory::format::binary_extension ();
          } else if (_output_file_format_ == "xml") {
            output_file_oss << '.' << datatools::io_factory::format::xml_extension ();
          } else if (_output_file_format_ == "brio") {
            using_brio = true;
            output_file_oss << brio::store_info::constants::brio_file_extension ();
          } else if (_output_file_format_ == "trio") {
            using_brio = true;
            output_file_oss << brio::store_info::constants::trio_file_extension ();
          }

          // compression extension:
          if (! using_brio) {
            if (_output_file_compression_ == "gzip") {
              output_file_oss << '.' << datatools::io_factory::format::gzip_extension ();
            } else if (_output_file_compression_ == "bzip2") {
              output_file_oss << '.' << datatools::io_factory::format::bzip2_extension ();
            }
          }
          _output_file_ = output_file_oss.str ();
          output_file_directives = true;
        }
        std::string output_file_name = _output_file_;
        datatools::fetch_path_with_env (output_file_name);
        if (! output_file_directives) {
          using_brio = false;
          const std::string extension = boost::filesystem::extension (output_file_name);
          if (extension == brio::store_info::constants::brio_file_extension ()) {
            using_brio = true;
          } else if (extension == brio::store_info::constants::trio_file_extension ()) {
            using_brio = true;
          }
        }

        if (using_run_header_footer ()) {
          _run_header_.clear ();
          _build_run_header ();
        }

        if (using_brio) {
          DT_LOG_NOTICE(_logprio(), "Opening brio output data file '" << output_file_name << "'...");
          DT_LOG_DEBUG(_logprio(), "Brio writer is opened: '" << _brio_writer_.is_opened() << "'...");
          if (_brio_writer_.is_locked()) _brio_writer_.unlock();
          _brio_writer_.set_existing_file_protected (_output_file_preserve_);
          _brio_writer_.open (output_file_name);
          DT_LOG_NOTICE(_logprio(), "Output data file '" << output_file_name << "' is open.");
          if (using_run_header_footer ()) {
            DT_LOG_DEBUG(_logprio(), "Brio writer has store '" << _brio_general_info_store_label_
                          << "' : " << _brio_writer_.has_store(_brio_general_info_store_label_));

            _brio_writer_.add_store (_brio_general_info_store_label_,
                                     datatools::properties::SERIAL_TAG);
            _brio_writer_.select_store (_brio_general_info_store_label_);
            _brio_writer_.store (_run_header_);
          }
          const std::string store = _brio_plain_simulated_data_store_label_;
          DT_LOG_DEBUG(_logprio(), "Brio writer has store '" << store
                        << "' : " << _brio_writer_.has_store(store));
          _brio_writer_.add_store (store, mctools::simulated_data::SERIAL_TAG);
          _brio_writer_.lock ();
          _brio_writer_.select_store (store);
        } else {
          DT_LOG_NOTICE(_logprio(), "Opening datatools serialization output data file '"
                        << output_file_name << "'...");
          _writer_.init (output_file_name,
                         datatools::using_multiple_archives);
          if (using_run_header_footer ()) {
            _writer_.store (_run_header_);
          }
        }
      }

      if (G4VVisManager::GetConcreteInstance ()) {
        G4UImanager * UI = G4UImanager::GetUIpointer ();
        UI->ApplyCommand ("/vis/scene/notifyHandlers");
      }

      if (get_manager ().using_time_stat ()) {
        grab_manager ().grab_CT_map ()["RA"].start ();
      }
      DT_LOG_NOTICE(_logprio(),"Run #" << a_run->GetRunID () << " is started.");
      return;
    }

    void run_action::EndOfRunAction (const G4Run * a_run)
    {
      DT_LOG_NOTICE(_logprio(),"Run #" << a_run->GetRunID () << " is stopping...");

      if (get_manager ().using_time_stat ()) {
        grab_manager ().grab_CT_map ()["RA"].stop ();
      }

      if (using_run_header_footer ()) {
        _run_footer_.clear ();
        _build_run_footer ();
      }

      // close the data writer:
      if (_save_data_) {
        DT_LOG_DEBUG(_logprio(),"Closing the data writer for run #"
                     << a_run->GetRunID () << "...");
        if (_brio_writer_.is_opened ()) {
          if (_use_run_header_footer_) {
            _brio_writer_.select_store (_brio_general_info_store_label_);
            _brio_writer_.store (_run_footer_);
          }
          if (_brio_writer_.is_opened()) _brio_writer_.close ();
          _brio_writer_.unlock ();
        } else {
          if (_use_run_header_footer_) {
            _writer_.store (_run_footer_);
          }
          _writer_.reset ();
        }
      }

      if (G4VVisManager::GetConcreteInstance ()) {
        G4UImanager::GetUIpointer ()->ApplyCommand ("/vis/viewer/update");
      }

      DT_LOG_NOTICE(_logprio(),"Run #" << a_run->GetRunID () << " is stopped.");
      return;
    }

  } // end of namespace g4

} // end of namespacemctools

/** Opening macro for implementation
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(mctools::g4::run_action,ocd_)
{
  // The class name :
  ocd_.set_class_name ("mctools::g4::run_action");

  // The class terse description :
  ocd_.set_class_description ("The Geant4 simulation mandatory run action");

  // The library the class belongs to :
  ocd_.set_class_library ("mctools_g4");

  // The class detailed documentation :
  ocd_.set_class_documentation ("This is Geant4 simulation engine embedded run action.      \n"
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

  {
    // Description of the 'number_of_events_modulo' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("number_of_events_modulo")
      .set_terse_description("The period to print the number of processed event")
      .set_traits(datatools::TYPE_INTEGER)
      .set_mandatory(false)
      .set_long_description("Default value: ``0`` (no print)                         \n"
                            "                                                        \n"
                            "Example::                                               \n"
                            "                                                        \n"
                            "  number_of_events_modulo : integer = 100               \n"
                            "                                                        \n"
                            )
      ;
  }

  {
    // Description of the 'file.save' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("file.save")
      .set_terse_description("Flag to inhibit the saving of simulated data in some output file")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("Default value: ``0`` (don't save data)                        \n"
                            "                                                              \n"
                            "Example::                                                     \n"
                            "                                                              \n"
                            "  file.save : boolean = 1                                     \n"
                            "                                                              \n"
                            "This flag may be activated when the Geant4 simulation         \n"
                            "engine is used from the ``g4_production`` executable.         \n"
                            "This flag must be deactivated when the Geant4 simulation      \n"
                            "engine is used from the ``mctools::g4::simulation_module``    \n"
                            "class through a data processing pipeline because in this case \n"
                            "the pipeline generally implements its own I/O mechanisms.     \n"
                            )
      ;
  }

  {
    // Description of the 'file.no_preserve' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("file.no_preserve")
      .set_terse_description("Flag to allow the overwriting of some already existing output file")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_triggered_by_flag("file.save")
      .set_long_description("Default value: ``0`` (preserve existing data file)            \n"
                            "                                                              \n"
                            "Example::                                                     \n"
                            "                                                              \n"
                            "  file.no_preserve : boolean = 0                              \n"
                            "                                                              \n"
                            "When activated, this flag may implies the deletion of some    \n"
                            "important data file. use it at your own risk !                \n"
                            )
      ;
  }

  {
    // Description of the 'file.name' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("file.name")
      .set_terse_description("The name of the file where to save the simulated data")
      .set_traits(datatools::TYPE_STRING)
      .set_path(true)
      .set_complex_triggering_conditions(true)
      //.set_triggered_by("file.save")
      .set_long_description("This property is not taken into account if the *output file*  \n"
                            "attribute has already been set by a previous  call to the     \n"
                            "``mctools::g4::run_action::set_output_file(..;)`` method.     \n"
                            "                                                              \n"
                            "Example::                                                     \n"
                            "                                                              \n"
                            "  file.name : string as path = \"g4_production.out.xml\"      \n"
                            "                                                              \n"
                            "One must use a special extension corresponding to a           \n"
                            "data format supported by Bayeux through the ``datatools`` and \n"
                            "``brio`` libraries : \n"
                            "                                                              \n"
                            "  * ``.xml`` or ``.xml.gz`` or ``.xml.bz2`` : Boost XML archive\n"
                            "  * ``.txt`` or ``.txt.gz`` or ``.txt.bz2`` : Boost ASCII archive\n"
                            "  * ``.data`` or ``.data.gz`` or ``.data.bz2``: Boost binary archive\n"
                            "  * ``.brio`` or ``.trio`` : resp. Brio binary and Brio ASCII \n"
                            "                                                              \n"
                            )
      ;
  }


  {
    // Description of the 'file.directory' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("file.directory")
      .set_terse_description("The directory where the output data file is stored")
      .set_traits(datatools::TYPE_STRING)
      .set_path(true)
      .set_complex_triggering_conditions(true)
      .set_long_description("This property is not taken into account if the *output file*  \n"
                            "attribute has already been set by a previous  call to the     \n"
                            "``mctools::g4::run_action::set_output_file(..;)`` method or   \n"
                            "by the ``file.directory`` property.                           \n"
                            "                                                              \n"
                            "Example::                                                     \n"
                            "                                                              \n"
                            "  file.directory : string as path = \"/tmp/data\"             \n"
                            "                                                              \n"
                            )
      ;
  }

  {
    // Description of the 'file.format' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("file.prefix")
      .set_terse_description("The prefix of the output data file name")
      .set_traits(datatools::TYPE_STRING)
      .set_complex_triggering_conditions(true)
      .set_long_description("This property is not taken into account if the *output file*  \n"
                            "attribute has already been set by a previous  call to the     \n"
                            "``mctools::g4::run_action::set_output_file(..;)`` method or   \n"
                            "by the ``file.directory`` property.                           \n"
                            "                                                              \n"
                            "Example::                                                     \n"
                            "                                                              \n"
                            "  file.prefix : string = \"g4_production_\"                   \n"
                            "                                                              \n"
                            "Note that the Geant4 current run number will be appended to   \n"
                            "the name of the output data file.                             \n"
                            )
      ;
  }

  {
    // Description of the 'file.format' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("file.format")
      .set_terse_description("The format of the output data file")
      .set_traits(datatools::TYPE_STRING)
      .set_complex_triggering_conditions(true)
      .set_long_description("This property is not taken into account if the *output file*  \n"
                            "attribute has already been set by a previous  call to the     \n"
                            "``mctools::g4::run_action::set_output_file(..;)`` method or   \n"
                            "by the ``file.directory`` property.                           \n"
                            "                                                              \n"
                            "Allowed values:                                               \n"
                            "                                                              \n"
                            " ``\"xml\" : Use the Boost portable XML archive format        \n"
                            " ``\"ascii\" : Use the Boost portable text archive format     \n"
                            " ``\"binary\" : Use the Boost portable binary archive format  \n"
                            " ``\"brio\" : Use the portable Brio format (binary)           \n"
                            " ``\"trio\" : Use the portable Brio format (text)             \n"
                            "                                                              \n"
                            "Example::                                                     \n"
                            "                                                              \n"
                            "  file.format : string = \"xml\"                              \n"
                            "                                                              \n"
                            )
      ;
  }

  {
    // Description of the 'file.format' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("file.compression")
      .set_terse_description("The compression of the output data file")
      .set_traits(datatools::TYPE_STRING)
      .set_complex_triggering_conditions(true)
      .set_long_description("This property is not taken into account if the *output file*  \n"
                            "attribute has already been set by a previous  call to the     \n"
                            "``mctools::g4::run_action::set_output_file(..;)`` method or   \n"
                            "by the ``file.directory`` property.                           \n"
                            "                                                              \n"
                            "This property is not taken into account for Brio format which \n"
                            "is already natively compressed.                               \n"
                            "                                                              \n"
                            "Allowed values:                                               \n"
                            "                                                              \n"
                            " ``\"none\" : No compression of the output data file          \n"
                            " ``\"gzip\" : Use GZIP compression                            \n"
                            " ``\"bzip2\" : Use BZIP2 compression                          \n"
                            "                                                              \n"
                            "Example::                                                     \n"
                            "                                                              \n"
                            "  file.compression : string = \"gzip\"                        \n"
                            "                                                              \n"
                            )
      ;
  }

  {
    // Description of the 'file.using_run_header_footer' configuration property :
    datatools::configuration_property_description & cpd
      = ocd_.add_property_info();
    cpd.set_name_pattern("file.using_run_header_footer")
      .set_terse_description("Flag to activate the saving of special informations in run header and footer data structure")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_triggered_by_flag("file.save")
      .set_long_description("Default value: ``0``                                    \n"
                            "                                                        \n"
                            "Example::                                               \n"
                            "                                                        \n"
                            "  file.using_run_header_footer : boolean = 0            \n"
                            "                                                        \n"
                            )
      ;
  }


  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Typical configuration is::                                             \n"
                               "                                                                       \n"
                               " #@description Run action logging priority                             \n"
                               " logging.priority : string = \"warning\"                               \n"
                               "                                                                       \n"
                               " #@description Print period for event number                           \n"
                               " number_of_events_modulo : integer = 100                               \n"
                               "                                                                       \n"
                               " #@description Inhibit output data file                                \n"
                               " file.save : boolean = 1                                               \n"
                               "                                                                       \n"
                               " #@description Print period for event number                           \n"
                               " file.no_preserve : boolean = 0                                        \n"
                               "                                                                       \n"
                               " #@description The name of the output data file                        \n"
                               " #file.name : string as path = \"/tmp/data/g4_sim_data.xml\"           \n"
                               "                                                                       \n"
                               " #@description Output file directory                                   \n"
                               " file.directory : string as path = \"/tmp/data\"                       \n"
                               "                                                                       \n"
                               " #@description Output file prefix                                      \n"
                               " file.prefix : string = \"g4_sim_data_\"                               \n"
                               "                                                                       \n"
                               " #@description Output file format                                      \n"
                               " file.format : string = \"binary\"                                     \n"
                               "                                                                       \n"
                               " #@description Output file compression                                 \n"
                               " file.compression : string = \"gzip\"                                  \n"
                               "                                                                       \n"
                               " #@description Save run header/footer information                      \n"
                               " file.using_run_header_footer : boolean = 0                            \n"
                               "                                                                       \n"
                               );

  ocd_.set_validation_support(true);

  // Lock the description:
  ocd_.lock();

  // ... and we are done.
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation

// Registration macro for class 'mctools::g4::manager' :
DOCD_CLASS_SYSTEM_REGISTRATION(mctools::g4::run_action,"mctools::g4::run_action")
