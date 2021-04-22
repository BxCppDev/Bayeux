/* dpp_driver.cc
 *
 * Copyright (C) 2011-2017 François Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

// Ourselves:
#include <dpp/dpp_driver.h>

// Third party:
// - Boost:
#include <boost/foreach.hpp>

// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/things.h>
#include <datatools/ioutils.h>
#include <datatools/properties.h>
#include <datatools/utils.h>

// This project:
#include <dpp/dpp_config.h>
#include <dpp/base_module.h>
#include <dpp/input_module.h>
#include <dpp/output_module.h>
#include <dpp/module_manager.h>

namespace dpp {

  dpp_driver_params::dpp_driver_params()
  {
    reset();
    return;
  }

  void dpp_driver_params::reset()
  {
    logging_label = "fatal";
    break_on_error_as_fatal = false;
    print_modulo = 10;
    module_manager_config_file.clear();
    module_names.clear();
    LL_config.clear();
    LL_dlls.clear();
    input_files.clear();
    output_files.clear();
    no_max_records = false;
    max_records = 0;
    max_records_per_output_file = 0;
    slice_start = -1;
    slice_stop  = -1;
    slice_width = -1;
    slice_store_out = false;
    save_stopped_data_records = false;
    preserve_existing_files = false;
    return;
  }

  void dpp_driver_params::dump(std::ostream & out_) const
  {
    tree_dump(out_, "dpp_driver_params::dump: ");
    return;
  }

  void dpp_driver_params::tree_dump(std::ostream & out_,
                                    const std::string & title_,
                                    const std::string & indent_,
                                    bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty()) {
      indent = indent_;
    }
    if (! title_.empty()) {
      out_ << indent << title_ << std::endl;
    }
    out_ << indent << datatools::i_tree_dumpable::tag << "logging_label  : '"
         << logging_label << "'" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "break_on_error_as_fatal  : "
         << std::boolalpha << break_on_error_as_fatal << "" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "print_modulo  : "
         << print_modulo << "" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "module_manager_config_file  : '"
         << module_manager_config_file << "'" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "module_names  : "
         << module_names.size() << "" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "LL_config  : '"
         << LL_config << "'" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "LL_dlls  : "
         << LL_dlls.size() << "" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "input_files  : "
         << input_files.size() << "" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "output_files  : "
         << output_files.size() << "" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "no_max_records  : "
         << std::boolalpha << no_max_records << "" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "max_records  : "
         << max_records << "" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "max_records_per_output_file  : "
         << max_records_per_output_file << "" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "slice_start  : "
         << slice_start << "" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "slice_stop  : "
         << slice_stop << "" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "slice_width  : "
         << slice_width << "" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "slice_store_out  : "
         << std::boolalpha << slice_store_out << "" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "save_stopped_data_records  : "
         << std::boolalpha << save_stopped_data_records << "" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_) << "preserve_existing_files  : "
         << std::boolalpha << preserve_existing_files << "" << std::endl;
    return;
  }

  /* ------------------------------------------------------------ */

  dpp_driver::dpp_driver()
  {
    _initialized_ = false;
    _logging_ = datatools::logger::PRIO_WARNING;
    _use_slice_ = false;
    return;
  }

  dpp_driver::~dpp_driver()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  bool dpp_driver::is_initialized() const
  {
    return _initialized_;
  }

  void dpp_driver::setup(const dpp_driver_params & params_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Driver is already initialized !");
    _params_ = params_;
    return;
  }

  void dpp_driver::initialize()
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Driver is already initialized !");

    _logging_ = datatools::logger::get_priority(_params_.logging_label);

    if (_params_.module_manager_config_file.empty()) {
      DT_LOG_WARNING(_logging_, "No module manager configuration file !");
    }

    if (_params_.module_names.empty()) {
      DT_LOG_WARNING(_logging_, "No processing module name !");
    }

    if (_params_.input_files.size() > 0) {
      DT_LOG_NOTICE(_logging_, "Input data sources : " << _params_.input_files.size());
      for (std::vector<std::string>::const_iterator i = _params_.input_files.begin();
           i != _params_.input_files.end();
           i++) {
        DT_LOG_NOTICE(_logging_," - source : '" << *i << "'");
      }
    } else {
      DT_LOG_NOTICE(_logging_, "No input data source !");
      if (_params_.max_records <= 0) {
        // This is only an error if we decide to enforce the use of
        // a limit on the maximum number of data records :
        DT_THROW_IF(!_params_.no_max_records,
                    std::logic_error,
                    "Missing maximum number of data records !");
      }
    }

    if (_params_.output_files.size() > 0) {
      DT_LOG_NOTICE(_logging_, "Output data sinks : " << _params_.output_files.size());
      for (std::vector<std::string>::const_iterator i = _params_.output_files.begin();
           i != _params_.output_files.end();
           i++) {
        DT_LOG_NOTICE(_logging_, " - sink : '" << *i << "'");
      }
    } else {
      DT_LOG_NOTICE(_logging_, "No output data sink !");
    }

    _lib_loader_.reset(new datatools::library_loader(_params_.LL_config));
    datatools::library_loader & LL = *_lib_loader_.get();
    BOOST_FOREACH(const std::string & dll_name, _params_.LL_dlls) {
      DT_LOG_INFORMATION(_logging_, "Loading DLL '" << dll_name << "'...");
      DT_LOG_INFORMATION(datatools::logger::PRIO_ALWAYS, "Loading DLL '" << dll_name << "'...");
      DT_THROW_IF(LL.load(dll_name) != EXIT_SUCCESS,
                  std::runtime_error,
                  "Loading DLL '" << dll_name  << "' fails !");
    }
    // _lib_loader_->print(std::cerr);
    // Load properties from the configuration file:
    // DT_THROW_IF(_params_.module_names.empty() && _params_.module_manager_config_file.empty(),
    //             std::logic_error,
    //             "Missing module manager configuration file !");

    BOOST_FOREACH(const std::string & module_name, _params_.module_names) {
      DT_LOG_INFORMATION(_logging_, "Using data processing module '" << module_name << "'.");
    }

    if (! _params_.module_manager_config_file.empty()) {
      _module_mgr_.reset(new dpp::module_manager);
      std::string MM_config_file = _params_.module_manager_config_file;
      datatools::fetch_path_with_env(MM_config_file);
      DT_LOG_NOTICE(_logging_, "Manager config. file : '" << MM_config_file << "'");

      datatools::properties MM_config;
      datatools::read_config(MM_config_file, MM_config);
      _module_mgr_->set_logging_priority(_logging_);
      _module_mgr_->initialize(MM_config);
      DT_LOG_NOTICE(_logging_, "Module manager (initialized) : ");
      if (_logging_ >= datatools::logger::PRIO_NOTICE) {
        _module_mgr_->tree_dump(std::clog, "", "[notice]: ");
      }
    }

    // The sequence of active modules (may be empty):
    DT_THROW_IF(_params_.module_names.size() && ! _module_mgr_, std::logic_error,
                "No module manager is available to queue "
                << _params_.module_names.size() << " requested modules !");
    for (size_t i = 0; i < _params_.module_names.size(); i++) {
      const std::string & module_name = _params_.module_names[i];
      DT_THROW_IF(! _module_mgr_->has(module_name),
                  std::logic_error,
                  "Manager has no module named '" << module_name << "' !");
      DT_LOG_NOTICE(_logging_, "Found module named '" << module_name << "'");
      dpp::base_module & the_module =_module_mgr_->grab(module_name);
      _modules_.push_back(&the_module);
      DT_LOG_NOTICE(_logging_, "Added module : ");
      if (_logging_ >= datatools::logger::PRIO_NOTICE) {
        the_module.tree_dump(std::clog, "", "[notice]: ");
      }
    }

    // Setup the data output sink :
    if (_params_.output_files.size() > 0) {
      _sink_.reset(new dpp::output_module(_logging_));
      // _sink_->set_logging_priority(datatools::logger::PRIO_DEBUG);
      datatools::properties sink_config;
      DT_LOG_DEBUG(_logging_, "Preserve existing files = "
                   << std::boolalpha << _params_.preserve_existing_files);
      if (_params_.preserve_existing_files) {
        sink_config.store_flag("preserve_existing_files");
      }
      sink_config.store("name", "data_output_sink");
      sink_config.store("files.mode", "list");
      sink_config.store("files.list.filenames", _params_.output_files);
      if (_params_.max_records_per_output_file > 0) {
        sink_config.store("max_record_per_file",
                          _params_.max_records_per_output_file);
      }
      if (_module_mgr_ && _module_mgr_->has_service_manager()) {
        _sink_->initialize_with_service(sink_config,
                                        _module_mgr_->grab_service_manager());
      } else {
        _sink_->initialize_standalone(sink_config);
      }
      if (datatools::logger::is_debug(_logging_)) {
        _sink_->tree_dump(std::cerr, "Sink: ", "[debug] ");
      }
    }

    // Setup the data input source :
    if (_params_.input_files.size() > 0) {
      _source_.reset(new dpp::input_module(_logging_));
      datatools::properties source_config;
      source_config.store("name", "data_input_source");
      source_config.store("files.mode", "list");
      source_config.store("files.list.filenames", _params_.input_files);
      if (_module_mgr_ && _module_mgr_->has_service_manager()) {
        _source_->initialize_with_service(source_config,
                                          _module_mgr_->grab_service_manager());
      } else {
        _source_->initialize_standalone(source_config);
      }
    }

    // Checks:
    _use_slice_ = false;
    DT_THROW_IF(_params_.slice_start < -1, std::domain_error,
                "Invalid slice start!");
    DT_THROW_IF(_params_.slice_stop < -1, std::domain_error,
                "Invalid slice stop!");

    if (_params_.slice_start < 0 && _params_.slice_stop >= 0) {
      _params_.slice_start = 0;
      if (_params_.slice_width > 0) {
        _params_.slice_start = std::max(0, _params_.slice_stop -_params_.slice_width + 1) ;
      }
    }

    if (_params_.slice_start >= 0 && _params_.slice_stop >= 0) {
      DT_THROW_IF(_params_.slice_stop < _params_.slice_start, std::range_error,
                  "Invalid slice start/stop!");
    }

    // Fix slice:
    if (_params_.slice_width > 0 && _params_.slice_start < 0) {
      _params_.slice_start = 0;
    }

    DT_LOG_DEBUG(_logging_, "Slice_start     = " << _params_.slice_start);
    DT_LOG_DEBUG(_logging_, "Slice_stop      = " << _params_.slice_stop);
    DT_LOG_DEBUG(_logging_, "Slice_width     = " << _params_.slice_width);
    DT_LOG_DEBUG(_logging_, "Slice_store_out = " << _params_.slice_store_out);

    if (_params_.slice_start >= 0 || _params_.slice_stop >= 0 || _params_.slice_width >= 0) {
      _use_slice_ = true;
    }
    DT_LOG_DEBUG(_logging_, "Using slice     = " << _use_slice_);

    _initialized_ = true;
    return;
  }

  void dpp_driver::reset()
  {
    DT_THROW_IF(! is_initialized(),
                std::logic_error,
                "Driver is not initialized !");
    _initialized_ = false;

    if (_sink_) {
      _sink_.reset();
    }

    if (_source_) {
      _source_.reset();
    }

    if (_modules_.size()) {
      _modules_.clear();
    }

    if (_module_mgr_) {
      _module_mgr_.reset();
    }
    _lib_loader_.reset();
    _params_.reset();
    _use_slice_ = false;
    _logging_ = datatools::logger::PRIO_WARNING;
    return;
  }

  void dpp_driver::run()
  {
    int error_code = EXIT_SUCCESS;
    datatools::logger::priority logging = _logging_;
    DT_THROW_IF(! is_initialized(), std::logic_error, "Driver is not initialized !");

    // Loop on the data records from the data source file :
    DT_LOG_DEBUG(logging, "Entering data record loop...");

    // Instantiate the working data record object :
    datatools::things DR;

    // Loop on the data records :
    int record_counter = 0;
    int processed_counter = 0;
    int stored_counter = 0;
    while (true) {
      bool do_break_record_loop = false;
      DT_LOG_DEBUG(logging, "Clear the working data record object...");
      DR.clear();

      // Manage the source if any (fill the data record from it) :
      if (_source_) {
        if (_source_->is_terminated()) {
          break;
        }
        int input_status = _source_->process(DR);
        if (input_status & dpp::base_module::PROCESS_FATAL) {
          DT_LOG_ERROR(logging, "Source of data records had a fatal error ! Break !");
          break;
        } else if (input_status & dpp::base_module::PROCESS_STOP) {
          DT_LOG_NOTICE(logging, "Source of data records has sent a stop !");
          break;
        }
      } // end of if (source)

      if ((_params_.print_modulo > 0) && (record_counter % _params_.print_modulo == 0)) {
        DT_LOG_NOTICE(logging, "Data record #" << record_counter);
      }

      bool in_slice = true;
      if (in_slice && (_params_.slice_start >= 0)) {
        if (record_counter < _params_.slice_start) {
          in_slice = false;
        }
      }
      if (in_slice && (_params_.slice_stop >= 0) && (_params_.slice_stop >= _params_.slice_start)) {
        if (record_counter > _params_.slice_stop) {
          in_slice = false;
        }
      }
      if (in_slice && (_params_.slice_start >= 0) && (_params_.slice_width >= 0)) {
        if (record_counter > (_params_.slice_start + _params_.slice_width - 1)) {
          in_slice = false;
        }
      }
      DT_LOG_DEBUG(_logging_, "in_slice = " << in_slice);

      bool process_it = true;
      if (_use_slice_ && !in_slice) {
        process_it = false;
      }

      int processing_status = dpp::base_module::PROCESS_OK;
      if (process_it) {
        processed_counter++;
        // Process the data record using the choosen processing module :
        DT_LOG_DEBUG(logging, "Processing the data record...");
        try {
          BOOST_FOREACH(dpp::base_module * active_module_ptr, _modules_) {
            dpp::base_module & the_active_module = *active_module_ptr;
            DT_LOG_DEBUG(logging, "Module name '" << the_active_module.get_name() << "'");
            processing_status = the_active_module.process(DR);
            DT_LOG_DEBUG(logging, "Processing status : " << processing_status);
            if (processing_status & dpp::base_module::PROCESS_FATAL) {
              // A fatal error has been met, we break the processing loop :
              DT_LOG_FATAL(logging, "Processing of data record #" << record_counter << " met a fatal error. Break !");
              do_break_record_loop = true;
              error_code = EXIT_FAILURE;
            } else if (processing_status & dpp::base_module::PROCESS_ERROR) {
              // A non-fatal error has been met, we warn and
              // skip to the next data record :
              DT_LOG_ERROR(logging, "Processing of data record #" << record_counter << " failed.");
              if (_params_.break_on_error_as_fatal) {
                // Force termination even if error is not fatal:
                do_break_record_loop = true;
                error_code = EXIT_FAILURE;
                DT_LOG_FATAL(logging, "Error promoted as fatal; forcing termination... Break !");
              }
            } else if (processing_status & dpp::base_module::PROCESS_STOP) {
              DT_LOG_WARNING(logging, "Processing of data record #" << record_counter << " stopped at some stage.");
              break;
            }
            if (do_break_record_loop) {
              break;
            }
          } // BOOST_FOREACH
        } catch (std::exception & x) {
          DT_LOG_ERROR(logging, "Caught exception " << x.what());
          throw x;
        }
      } // process_it

      DT_LOG_DEBUG(logging, "End of processing...");

      // Manage the sink if any :
      if (_sink_ && ! _sink_->is_terminated()) {
        bool save_it = true;
        if (save_it && (processing_status & dpp::base_module::PROCESS_STOP)) {
          save_it = _params_.save_stopped_data_records;
        }
        if (save_it && _use_slice_) {
          if (! in_slice && ! _params_.slice_store_out) {
            save_it = false;
          }
        }
        if (save_it) {
          DT_LOG_DEBUG(logging, "Save the data record...");
          // Save the processed data record in the sink :
          try {
            // DR.tree_dump(std::cerr, "T : ", "*** ");
            // _lib_loader_->print(std::cerr);
            int output_status = _sink_->process(DR);
            DT_LOG_DEBUG(logging, "Data record has been saved.");
            if (output_status != dpp::base_module::PROCESS_OK) {
              DT_LOG_ERROR(logging, "Error while storing data record #" << record_counter << " !");
            }
          } catch (std::exception & x) {
            DT_LOG_ERROR(logging, "Error while storing data record #" << record_counter << ": " << x.what());
            break;
          }
          stored_counter++;
        }
      }

      record_counter++;
      if ((_params_.max_records > 0) && (record_counter == _params_.max_records)) {
        DT_LOG_DEBUG(logging, "Max number of data record reached !");
        do_break_record_loop = true;
      }
      // break check :
      if (do_break_record_loop) {
        DT_LOG_DEBUG(logging, "Break the loop !");
        break;
      }
    } // End of data record loop

    DT_LOG_DEBUG(logging, "Exit the data record loop.");
    DT_LOG_NOTICE(logging, "Number of records           : " << record_counter);
    DT_LOG_NOTICE(logging, "Number of processed records : " << processed_counter);
    if (_sink_) {
      DT_LOG_NOTICE(logging, "Number of saved records     : " << stored_counter);
    }

    if (error_code != EXIT_SUCCESS) {
      DT_LOG_ERROR(logging, "Error code : " << error_code);
    }
    return;
  }

} // namespace dpp
