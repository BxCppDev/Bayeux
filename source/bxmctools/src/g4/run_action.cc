// -*- mode: c++ ; -*-
/* run_action.cc
 */

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <map>

#include <boost/filesystem.hpp>
#include <boost/thread/mutex.hpp>

#include <datatools/utils.h>
#include <mygsl/seed_manager.h>

// Packages' versions :
#include <geomtools/geomtools_config.h>
#include <dpp/dpp_config.h>
#include <genbb_help/genbb_help_config.h>
#include <genvtx/genvtx_config.h>
#include <mctools/simulated_data.h>

#include <mctools/mctools_config.h>
#include <mctools/g4/run_action.h>
#include <mctools/g4/manager.h>
#include <mctools/g4/simulation_ctrl.h>

#include <globals.hh>
#include <G4Run.hh>
#include <G4RunManager.hh>
#include <G4UImanager.hh>
#include <G4VVisManager.hh>
#include <G4ios.hh>

#include <datatools/properties.ipp>

namespace mctools {

  namespace g4 {

    using namespace std;

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
      DT_THROW_IF (is_initialized (), logic_error, "Object is locked !");
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

    bool run_action::has_number_events_modulo () const
    {
      return _number_events_modulo_ > NUMBER_EVENTS_MODULO_NONE;
    }

    void run_action::set_number_events_modulo (int modulo_)
    {
      if (modulo_ <= NUMBER_EVENTS_MODULO_NONE) {
        _number_events_modulo_ = NUMBER_EVENTS_MODULO_NONE;
      } else {
        _number_events_modulo_ = modulo_;
      }
      return;
    }

    int run_action::get_number_events_modulo () const
    {
      return _number_events_modulo_;
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

    void run_action::set_output_file (const string & a_filename)
    {
      DT_THROW_IF (is_initialized (), logic_error,
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

    // ctor:
    run_action::run_action (manager & a_mgr)
    {
      _initialized_ = false;
      _use_run_header_footer_ = false;
      _number_of_processed_events_ = 0;
      _number_events_modulo_ = NUMBER_EVENTS_MODULO_NONE;
      _save_data_ = true;
      _output_file_preserve_ = true;
      _output_file_dir_ = ".";
      _output_file_prefix_ = "mc_g4_";
      _output_file_format_ = "ascii";
      _output_file_compression_ = "gzip";
      _output_file_ = "";
      _manager_ = &a_mgr;
      _brio_general_info_store_label_ = "GI";
      _brio_plain_simulated_data_store_label_ = "PSD";
      return;
    }

    // dtor:
    run_action::~run_action ()
    {
      return;
    }

    void run_action::initialize (const datatools::properties & a_config)
    {
      // fetch parameters...
      loggable_support::_initialize_logging_support(a_config);

      if (a_config.has_flag ("using_run_header_footer")) {
        set_use_run_header_footer (true);
      }

      if (! has_number_events_modulo ()) {
        if (a_config.has_key ("number_events_modulo")) {
          int number_events_modulo = a_config.fetch_integer ("number_events_modulo");
          set_number_events_modulo (number_events_modulo);
        }
      }

      if (a_config.has_flag ("file.no_save")) {
        _save_data_ = false;
      }

      if (a_config.has_flag ("file.no_preserve")) {
        _output_file_preserve_ = false;
      }

      if (_save_data_) {
        if (_output_file_.empty ()) {
          if (a_config.has_key ("file.name")) {
            _output_file_ = a_config.fetch_string ("file.name");
          }
        }
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
          string format = a_config.fetch_string ("file.format");
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
            DT_THROW_IF(true, logic_error, "Invalid format '" << format << "' !");
          }
        }

        if (! using_brio && a_config.has_key ("file.compression")) {
          string compression = a_config.fetch_string ("file.compression");
          bool ok = false;
          if (compression == "none") ok = true;
          else if (compression == "gzip") ok = true;
          else if (compression == "bzip2") ok = true;
          if (ok) {
            _output_file_compression_ = compression;
          } else {
            DT_THROW_IF(true, logic_error, "Invalid compression mode '" << compression << "' !");
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
      a_out << "run_action::dump:" << endl;
      a_out << "|-- Save data           : "  << _save_data_ << endl;
      a_out << "|-- Output file dir     : '"  << _output_file_dir_ << "'" << endl;
      a_out << "|-- Output file prefix  : '"  << _output_file_prefix_ << "'" << endl;
      a_out << "|-- Output file format  : '"  << _output_file_format_ << "'" << endl;
      a_out << "|-- Output compression  : '"  << _output_file_compression_ << "'" << endl;
      a_out << "`-- Output file         : '"  << _output_file_ << "'" << endl;
      return;
    }

    void run_action::_build_run_header ()
    {
      _run_header_.set_description ("mctools::g4_production run header");

      // _run_header_.store ("sngeometry_version",  SNGEOMETRY_LIB_VERSION);
      // _run_header_.store ("sncore_version",      SNCORE_LIB_VERSION);
      // _run_header_.store ("sngenbb_version",     SNGENBB_LIB_VERSION);
      // _run_header_.store ("sngenvertex_version", SNGENVERTEX_LIB_VERSION);
      // _run_header_.store ("sng4_version",        SNG4_LIB_VERSION);
      // _run_header_.store ("geometry_setup_label",
      //                     _manager_->get_geom_manager ().get_setup_label ());
      // _run_header_.store ("geometry_setup_version",
      //                     _manager_->get_geom_manager ().get_setup_version ());
      // _run_header_.store ("vertex_generator_name",
      //                     _manager_->get_vertex_generator ().get_generator_name ());
      // _run_header_.store ("event_generator_name",
      //                     _manager_->get_event_generator ().get_generator_name ());

      const mygsl::seed_manager & the_seed_manager = _manager_->get_seed_manager ();
      vector<string> seed_labels;
      the_seed_manager.get_labels (seed_labels);
      for (int i = 0; i < (int) seed_labels.size (); i++) {
        int32_t seed = the_seed_manager.get_seed (seed_labels[i]);
        ostringstream seed_key;
        seed_key << "seed." << seed_labels[i];
        _run_header_.store (seed_key.str (), seed);
      }

      return;
    }

    void run_action::_build_run_footer ()
    {
      _run_footer_.set_description ("mctools::g4_production run footer");

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
      cout << flush;
      cerr << flush;
      clog << flush;
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
          ostringstream output_file_oss;
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
            output_file_oss << '.' << datatools::io_factory::TXT_EXT;
          } else if (_output_file_format_ == "binary") {
            output_file_oss << '.' << datatools::io_factory::BIN_EXT;
          } else if (_output_file_format_ == "xml") {
            output_file_oss << '.' << datatools::io_factory::XML_EXT;
          } else if (_output_file_format_ == "brio") {
            using_brio = true;
            output_file_oss << brio::store_info::BRIO_FILE_EXTENSION;
          } else if (_output_file_format_ == "trio") {
            using_brio = true;
            output_file_oss << brio::store_info::TRIO_FILE_EXTENSION;
          }

          // compression extension:
          if (! using_brio) {
            if (_output_file_compression_ == "gzip") {
              output_file_oss << '.' << datatools::io_factory::GZ_EXT;
            } else if (_output_file_compression_ == "bzip2") {
              output_file_oss << '.' << datatools::io_factory::BZIP2_EXT;
            }
          }
          _output_file_ = output_file_oss.str ();
          output_file_directives = true;
        }
        string output_file_name = _output_file_;
        datatools::fetch_path_with_env (output_file_name);
        if (! output_file_directives) {
          using_brio = false;
          string extension = boost::filesystem::extension (output_file_name);
          if (extension == brio::store_info::BRIO_FILE_EXTENSION) {
            using_brio = true;
          } else if (extension == brio::store_info::TRIO_FILE_EXTENSION) {
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
          const std::string & store = _brio_plain_simulated_data_store_label_;
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

  // end of run_action.cc
