// -*- mode: c++ ; -*-
/* plain_simulated_data_input_module.cc
 */

#include <mctools/plain_simulated_data_input_module.h>

#include <datatools/io_factory.h>
#include <datatools/exception.h>

#include <brio/reader.h>

namespace mctools {

  // Registration instantiation macro :
  DPP_MODULE_REGISTRATION_IMPLEMENT(plain_simulated_data_input_module, "mctools::plain_simulated_data_input_module");

  void plain_simulated_data_input_module::set_max_record_total (int a_max_record_total)
  {
    DT_THROW_IF (is_initialized (), std::logic_error,
                 "Module '" << get_name () << "' is already initialized !");
    DT_THROW_IF (a_max_record_total < 0, std::logic_error,
                 "Invalid 'max_record_total' value (" << a_max_record_total << ") !");
    _max_record_total_ = a_max_record_total;
    return;
  }

  void plain_simulated_data_input_module::set_max_record_per_file (int a_max_record_per_file)
  {
    DT_THROW_IF (is_initialized (), std::logic_error,
                 "Module '" << get_name () << "' is already initialized !");
    DT_THROW_IF (a_max_record_per_file < 0, std::logic_error,
                 "Invalid 'max_record_per_file' value (" << a_max_record_per_file << ") !");
    _max_record_per_file_ = a_max_record_per_file;
    return;
  }

  void plain_simulated_data_input_module::set_filenames (const datatools::properties & a_setup)
  {
    DT_THROW_IF (is_initialized (), std::logic_error,
                 "Module '" << get_name () << "' is already initialized !");
    if (_filenames_.is_valid ()) {
      _filenames_.reset ();
    }
    _filenames_.initialize (a_setup);
    DT_THROW_IF (! _filenames_.is_valid (), std::logic_error,
                 "Module '" << get_name () << "' : invalid list of filenames !");
    return;
  }

  bool plain_simulated_data_input_module::is_terminated () const
  {
    return _terminated_;
  }


  void plain_simulated_data_input_module::_set_defaults ()
  {
    _max_record_per_file_ = 0;
    _max_record_total_ = 0;
    _max_files_ = -1;
    _terminated_ = false;
    _file_record_counter_ = 0;
    _record_counter_ = 0;
    _file_index_ = -1;
    _bio_source_.reset(0);
    _brio_source_.reset(0);
    return;
  }

  plain_simulated_data_input_module::plain_simulated_data_input_module(datatools::logger::priority logging_priority)
    : dpp::base_module(logging_priority)
  {
    _set_defaults ();
    return;
  }


  // Destructor :
  plain_simulated_data_input_module::~plain_simulated_data_input_module ()
  {
    // Make sure all internal resources are terminated before destruction :
    if (is_initialized ()) reset ();
  }

  // Initialization :
  void plain_simulated_data_input_module::initialize (const datatools::properties  & a_config,
                                                      datatools::service_manager   & a_service_manager,
                                                      dpp::module_handle_dict_type & a_module_dict)
  {
    DT_THROW_IF (is_initialized (), std::logic_error,
                 "Module '" << get_name () << "' is already initialized !");
    /**************************************************************
     *   fetch setup parameters from the configuration container  *
     **************************************************************/

    _common_initialize(a_config);


    if (! _filenames_.is_valid ()) {
      datatools::properties input_config;
      a_config.export_and_rename_starting_with (input_config, "input.", "");
      set_filenames (input_config);
      if (is_debug ()) {
        _filenames_.dump (std::cerr);
      }
    }

    if (a_config.has_key ("input.max_files")) {
      int the_max_files = a_config.fetch_integer ("input.max_files");
      DT_THROW_IF (the_max_files < 1, std::logic_error,
                   "Module '" << get_name () << "' : invalid max number of files !");
      set_max_files (the_max_files);
    }

    if (a_config.has_key ("input.max_record_total"))
      {
        int the_max_record_total = a_config.fetch_integer ("input.max_record_total");
        DT_THROW_IF (the_max_record_total < 1,  std::logic_error,
                     "Module '" << get_name () << "' : invalid max total number of event record !");
        set_max_record_total (the_max_record_total);
      }

    if (a_config.has_key ("input.max_record_per_file"))
      {
        int the_max_record_per_file = a_config.fetch_integer ("input.max_record_per_file");
        DT_THROW_IF (the_max_record_per_file < 1,  std::logic_error,
                     "Module '" << get_name () << "' : invalid max number of event record per file !");
        set_max_record_per_file (the_max_record_per_file);
      }


    /*************************************
     *  end of the initialization step   *
     *************************************/

    // Tag the module as initialized :
    _set_initialized (true);
    return;
  }

  // Reset :
  void plain_simulated_data_input_module::reset ()
  {
    DT_THROW_IF (! is_initialized (), std::logic_error,
                 "Module '" << get_name () << "' is not initialized !");

    /****************************
     *  revert to some defaults *
     ****************************/
    if (_bio_source_.get() != 0)
      {
        _bio_source_.get()->reset ();
        _bio_source_.reset(0);
      }
    if (_brio_source_.get() != 0)
      {
        _brio_source_.get()->reset ();
        _brio_source_.reset(0);
      }

    _filenames_.reset ();
    _set_defaults ();

    /****************************
     *  end of the reset step   *
     ****************************/

    // Tag the module as un-initialized :
    _set_initialized (false);
    return;
  }


  // Processing :
  int plain_simulated_data_input_module::process (datatools::things & a_event_record)
  {
    DT_THROW_IF (! is_initialized (), std::logic_error,
                 "Module '" << get_name () << "' is not initialized !");
    return _load (a_event_record);
  }


  int plain_simulated_data_input_module::_load (datatools::things & a_event_record)
  {
    // attempt to open a source of event records :
    //while ((_bio_source_ == 0) && (_brio_source_ == 0))
    int load_status = OK;
    while (_bio_source_.get() == 0 && _brio_source_.get() == 0) {
      _file_record_counter_ = 0;
      _file_index_++;
      if (_file_index_ >= _filenames_.size ()) {
        DT_LOG_FATAL(get_logging_priority(),
                     "No more available input data file for input module '" << get_name() << "' !");
        load_status = FATAL;
        return load_status;
      }
      std::string source_label = _filenames_[_file_index_];
      int mode_guess = 0;
      bool brio_format = false;
      namespace ds = datatools;
      if (brio::store_info::guess_mode_from_filename (source_label, mode_guess)
          == brio::store_info::SUCCESS) {
        brio_format = true;
      } else if (ds::io_factory::guess_mode_from_filename (source_label, mode_guess)
                 == ds::io_factory::SUCCESS) {
        brio_format = false;
      } else {
        DT_LOG_FATAL(get_logging_priority(),
                     "Cannot guess mode for input data file '" << source_label << "' for input module '" << get_name() << "' !");
        load_status = FATAL;
        return load_status;
      }
      uint32_t input_flags = mctools::i_data_source::blank;
      if (brio_format) {
        _brio_source_.reset(new brio::reader);
        _brio_source_.get()->open(source_label);
      } else {
        _bio_source_.reset(new datatools::data_reader);
        _bio_source_.get()->init_multi(source_label);
      }
      //if (! _source_->is_open ()) _source_->open ();
      // check if we have some records in it :
      if (! _source_->has_next_record ()) {
        _source_->reset ();
        delete _source_;
        _source_ = 0;
        _brio_source_ = 0;
        _bio_source_ = 0;
      }
    }

    // force loading of the current event record :
    bool load_it = true;

    // load action :
    if (load_it) {
      // Check if
      if ((a_event_record.size () > 0)) {
        load_status = ERROR;
        return load_status;
      }
      a_event_record.clear ();
      if (_source_ != 0) {
        DT_THROW_IF (! _source_->has_next_record (), std::logic_error,
                     "No more available event record ! This is a bug !");
        /*
          std::cerr << datatools::io::error << message.str () << std::endl;
          load_status = FATAL;
          return load_status;
        */
        _source_->load_next_record (a_event_record);
        _file_record_counter_++;
        _record_counter_++;
      } else {
        DT_THROW_IF(true, std:runtime_error,
                    "No available data source ! This is a bug !");
        /*
          load_status = FATAL;
          return load_status;
        */
      }
    }

    bool stop_file   = false;
    bool stop_input = false;

    if (_max_record_total_ > 0) {
      if (_record_counter_ >= _max_record_total_) {
        stop_input = true;
        stop_file   = true;
        DT_LOG_WARNING(get_logging_priority(),
                       "Module '" << get_name () << "' has reached the maximum number of records from the input data source (" << _max_record_total_ << ") !");
      }
    }

    if (_max_record_per_file_ > 0) {
      if (_file_record_counter_ >= _max_record_per_file_) {
        stop_file = true;
        std::ostringstream message;
        DT_LOG_WARNING(get_logging_priority(),
                       "Module '" << get_name () << "' has reached the maximum number of records from the current input file (" << _max_record_per_file_ << ") !");
      }
    }

    // check the end of the input file :
    if ((_source_ != 0) && (! _source_->has_next_record ())) {
      stop_file = true;
    }

    // manage the end of the input file :
    if (stop_file) {
      if (_source_ != 0) {
        _source_->reset ();
        delete _source_;
        _source_ = 0;
        _brio_source_ = 0;
        _bio_source_ = 0;
      }
      _file_record_counter_ = 0;
      int effective_max_files = _max_files_;
      if (_max_files_ < 0) {
        if (_filenames_.is_ranged()) {
          effective_max_files = _filenames_.size ();
        }
      }
      if (effective_max_files > 0) {
        DT_THROW_IF ((_file_index_ + 1) >= effective_max_files,
                     std::logic_error,
                     "Module '" << get_name ()
                     << "' has reached the requested maximum number of input file(s) ("
                     << effective_max_files << ") !");
      }
      if ((_file_index_ + 1) >= _filenames_.size ()) {
        DT_LOG_WARNING(get_logging_priority(),
                       "Module '" << get_name ()
                       << "' has loaded the last available input file (total is "
                       << _filenames_.size () << " file" << (_filenames_.size ()>1?"":"s") << ") !");
        stop_input = true;
      }
    }

    if (stop_input) {
      _terminated_ = true;
    }

    return load_status;
  }

} // end of namespace mctools
// OCD support for class 'mctools::plain_simulated_data_input_module' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::mctools::plain_simulated_data_input_module,ocd_)
{
  ocd_.set_class_name ("mctools::plain_simulated_data_input_module");
  ocd_.set_class_library ("mctools");
  ocd_.set_class_description ("A module to load Monte-Carlo plain events from a set data files");



  ocd_.set_configuration_hints ("A ``mctools::plain_simulated_data_input_module`` object can be setup with the    \n"
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
                                "  #@config A MC events input module                                  \n"
                                "  foo : string = \"bar\"                                             \n"
                                "  to be done                                                         \n"
                                "                                                                     \n"
                                );

  ocd_.set_validation_support(false);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::mctools::plain_simulated_data_input_module,"mctools::plain_simulated_data_input_module")

// end of plain_simulated_data_input_module.cc
