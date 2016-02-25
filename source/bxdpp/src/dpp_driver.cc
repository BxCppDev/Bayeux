/* dpp_driver.cc
 *
 * Copyright (C) 2011-2016 François Mauger <mauger@lpccaen.in2p3.fr>
 *
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
#include <datatools/library_loader.h>

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
    help = false;
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
    out_ << indent << datatools::i_tree_dumpable::tag << "help  : "
         << help << "" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "logging_label  : '"
         << logging_label << "'" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "break_on_error_as_fatal  : "
         << break_on_error_as_fatal << "" << std::endl;
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
         << no_max_records << "" << std::endl;
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
         << slice_store_out << "" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag << "save_stopped_data_records  : "
         << save_stopped_data_records << "" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_) << "preserve_existing_files  : "
         << preserve_existing_files << "" << std::endl;
    return;
  }

  /*
  // static
  void dpp_driver_params::build_opts(boost::program_options::options_description & opts_,
  dpp_driver_params & params_)
  {
  namespace po = boost::program_options;
  opts_.add_options ()
  ("help,h", po::value<bool> (&params_.help)
  ->zero_tokens()
  ->default_value(false),
  "produce help message.")
  ("logging-priority,P",
  po::value<std::string>(&params_.logging_label)->default_value ("warning"),
  "set the logging priority.")
  ("load-dll,l",
  po::value<std::vector<std::string> > (&params_.LL_dlls),
  "set a DLL to be loaded.")
  ("dlls-config,L",
  po::value<std::string> (&params_.LL_config),
  "set the DLL loader configuration file.")
  ("modulo,%",
  po::value<int> (&params_.print_modulo)->default_value (10),
  "set the modulo print period for data record.")
  ("max-records,M",
  po::value<int> (&params_.max_records)->default_value (0),
  "set the maximum number of data records to be processed.")
  ("no-max-records,X",
  po::value<bool>(&params_.no_max_records)->zero_tokens()->default_value (false),
  "Do not limit the maximum number of data records to be processed.")
  ("module,m",
  po::value<std::vector<std::string> > (&params_.module_names),
  "add a module in the pipeline (optional).")
  ("module-manager-config,c",
  po::value<std::string> (&params_.module_manager_config_file),
  "set the module manager configuration file.")
  ("input-file,i",
  po::value<std::vector<std::string> > (&params_.input_files),
  "set an input file (optional).")
  ("output-file,o",
  po::value<std::vector<std::string> > (&params_.output_files),
  "set the output file (optional).")
  ("preserve-existing-files,x",
  po::value<bool>(&params_.preserve_existing_files)->zero_tokens()->default_value (false),
  "preserve existing files (recommended).")
  ("max-records-per-output-file,O",
  po::value<int> (&params_.max_records_per_output_file)->default_value (0),
  "set the maximum number of data records per output file.")
  // ("save-stopped-records,s",
  //  po::value<bool>(&save_stopped_data_records)->zero_tokens()->default_value (false),
  //  "Blablabla.")
  ;
  return;
  }
  */

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

    datatools::library_loader LL(_params_.LL_config);
    BOOST_FOREACH(const std::string & dll_name, _params_.LL_dlls) {
      DT_LOG_INFORMATION(_logging_, "Loading DLL '" << dll_name << "'...");
      DT_THROW_IF(LL.load(dll_name) != EXIT_SUCCESS,
                  std::runtime_error,
                  "Loading DLL '" << dll_name  << "' fails !");
    }

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
      datatools::properties::read_config(MM_config_file, MM_config);
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
      datatools::properties sink_config;
      if (_params_.preserve_existing_files) sink_config.store_flag("preserve_existing_files");
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
    // std::cerr << "DEVEL: dpp::dpp_driver::initialize: slice_start = " << _params_.slice_start << std::endl;
    // std::cerr << "DEVEL: dpp::dpp_driver::initialize: slice_stop  = " << _params_.slice_stop  << std::endl;
    // std::cerr << "DEVEL: dpp::dpp_driver::initialize: slice_width = " << _params_.slice_width << std::endl;
    // std::cerr << "DEVEL: dpp::dpp_driver::initialize: slice_store_out = " << _params_.slice_store_out << std::endl;

    if (_params_.slice_start >= 0 || _params_.slice_stop >= 0 || _params_.slice_width >= 0) {
      _use_slice_ = true;
    }
    // std::cerr << "DEVEL: dpp::dpp_driver::initialize: using slice = " << _use_slice_ << std::endl;

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
    _params_.reset();
    _use_slice_ = false;
    _logging_ = datatools::logger::PRIO_WARNING;
    return;
  }

  void dpp_driver::run()
  {
    int error_code = EXIT_SUCCESS;
    DT_THROW_IF(! is_initialized(), std::logic_error, "Driver is not initialized !");

    // Loop on the data records from the data source file :
    DT_LOG_DEBUG(_logging_, "Entering data record loop...");

    // Instantiate the working data record object :
    datatools::things DR;

    // Loop on the data records :
    int record_counter = 0;
    int processed_counter = 0;
    int stored_counter = 0;
    while (true) {
      bool do_break_record_loop = false;
      DT_LOG_DEBUG(_logging_, "Clear the working data record object...");
      DR.clear();

      // Manage the source if any (fill the data record from it) :
      if (_source_) {
        if (_source_->is_terminated()) {
          break;
        }
        int input_status = _source_->process(DR);
        if (input_status & dpp::base_module::PROCESS_FATAL) {
          DT_LOG_ERROR(_logging_, "Source of data records had a fatal error ! Break !");
          break;
        } else if (input_status & dpp::base_module::PROCESS_STOP) {
          DT_LOG_NOTICE(_logging_, "Source of data records has sent a stop !");
          break;
        }
      } // end of if (source)

      if ((_params_.print_modulo > 0) && (record_counter % _params_.print_modulo == 0)) {
        DT_LOG_NOTICE(_logging_, "Data record #" << record_counter);
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
      // std::cerr << "DEVEL: dpp::dpp_driver::run: in_slice = " << in_slice << std::endl;

      bool process_it = true;
      if (_use_slice_ && !in_slice) {
        process_it = false;
      }

      int processing_status = dpp::base_module::PROCESS_OK;
      if (process_it) {
        processed_counter++;
        // Process the data record using the choosen processing module :
        DT_LOG_DEBUG(_logging_, "Processing the data record...");
        try {
          BOOST_FOREACH(dpp::base_module * active_module_ptr, _modules_) {
            dpp::base_module & the_active_module = *active_module_ptr;
            processing_status = the_active_module.process(DR);
            DT_LOG_DEBUG(_logging_, "Processing status : " << processing_status);
            if (processing_status & dpp::base_module::PROCESS_FATAL) {
              // A fatal error has been met, we break the processing loop :
              DT_LOG_FATAL(_logging_, "Processing of data record #" << record_counter << " met a fatal error. Break !");
              do_break_record_loop = true;
              error_code = EXIT_FAILURE;
            } else if (processing_status & dpp::base_module::PROCESS_ERROR) {
              // A non-fatal error has been met, we warn and
              // skip to the next data record :
              DT_LOG_ERROR(_logging_, "Processing of data record #" << record_counter << " failed.");
              if (_params_.break_on_error_as_fatal) {
                // Force termination even if error is not fatal:
                do_break_record_loop = true;
                error_code = EXIT_FAILURE;
                DT_LOG_FATAL(_logging_, "Error promoted as fatal; forcing termination... Break !");
              }
            } else if (processing_status & dpp::base_module::PROCESS_STOP) {
              DT_LOG_WARNING(_logging_, "Processing of data record #" << record_counter << " stopped at some stage.");
              break;
            }
            if (do_break_record_loop) {
              break;
            }
          } // BOOST_FOREACH
        }
        catch (std::exception & x) {
          throw x;
        }
      } // process_it

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
          // Save the processed data record in the sink :
          int output_status = _sink_->process(DR);
          if (output_status != dpp::base_module::PROCESS_OK) {
            DT_LOG_ERROR(_logging_, "Error while storing data record #" << record_counter << " !");
          }
          stored_counter++;
        }
      }

      record_counter++;
      if ((_params_.max_records > 0) && (record_counter == _params_.max_records)) {
        DT_LOG_DEBUG(_logging_, "Max number of data record reached !");
        do_break_record_loop = true;
      }
      // break check :
      if (do_break_record_loop) {
        DT_LOG_DEBUG(_logging_, "Break the loop !");
        break;
      }
    } // End of data record loop

    DT_LOG_DEBUG(_logging_, "Exit the data record loop.");
    DT_LOG_NOTICE(_logging_, "Number of records           : " << record_counter);
    DT_LOG_NOTICE(_logging_, "Number of processed records : " << processed_counter);
    if (_sink_) {
      DT_LOG_NOTICE(_logging_, "Number of saved records     : " << stored_counter);
    }

    if (error_code != EXIT_SUCCESS) {
      DT_LOG_ERROR(_logging_, "Error code : " << error_code);
    }
    return;
  }

} // namespace dpp
