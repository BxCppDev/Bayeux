// -*- mode: c++ ; -*- 
/* plain_simulated_data_input_module.cc
 */

#include <mctools/plain_simulated_data_input_module.h>

#include <datatools/io_factory.h>
#include <brio/reader.h>

namespace mctools {

  // Registration instantiation macro :
  DPP_MODULE_REGISTRATION_IMPLEMENT(plain_simulated_data_input_module, "mctools::plain_simulated_data_input_module");

  void plain_simulated_data_input_module::set_max_record_total (int a_max_record_total)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "mctools::plain_simulated_data_input_module::set_max_record_total: "
                << "Module '" << get_name () << "' is already initialized !";
        throw std::logic_error (message.str ());
      }
    if (a_max_record_total < 0)
      {
        std::ostringstream message;
        message << "mctools::plain_simulated_data_input_module::set_max_record_total: "
                << "Invalid 'max_record_total' !";
        throw std::logic_error (message.str ());
      }
    _max_record_total_ = a_max_record_total;
    return;
  }

  void plain_simulated_data_input_module::set_max_record_per_file (int a_max_record_per_file)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "mctools::plain_simulated_data_input_module::set_max_record_per_file: "
                << "Module '" << get_name () << "' is already initialized !";
        throw std::logic_error (message.str ());
      }
    if (a_max_record_per_file < 0)
      {
        std::ostringstream message;
        message << "mctools::plain_simulated_data_input_module::set_max_record_per_file: "
                << "Invalid 'max_record_per_file' !";
        throw std::logic_error (message.str ());
      }
    _max_record_per_file_ = a_max_record_per_file;
    return;
  }

  void plain_simulated_data_input_module::set_filenames (const datatools::properties & a_setup)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "mctools::plain_simulated_data_input_module::set_filenames: "
                << "Module '" << get_name () << "' is already initialized !";
        throw std::logic_error (message.str ());
      }
    if (_filenames_.is_valid ())
      {
        _filenames_.reset ();
      }
    _filenames_.initialize (a_setup);
    if (! _filenames_.is_valid ())
      {
        std::ostringstream message;
        message << "mctools::plain_simulated_data_input_module::set_filenames: "
                << "Module '" << get_name () << "' : invalid list of filenames !";
        throw std::logic_error (message.str ());
      }
    //_filenames_.dump(std::cerr);
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

  plain_simulated_data_input_module::plain_simulated_data_input_module(bool debug_)
    : dpp::base_module ("dpp::plain_simulated_data_input_module",
                        "An plain simulated data record input module",
                        "0.1",
                        debug_ ? 1 : 0)

  {
    _set_defaults ();
    return;
  }


  // Destructor :
  plain_simulated_data_input_module::~plain_simulated_data_input_module ()
  {
    if (is_initialized ()) reset ();
  }

  // Initialization :
  void plain_simulated_data_input_module::initialize (const datatools::properties               & a_config,
                                                      datatools::service_manager              & a_service_manager,
                                                      dpp::module_handle_dict_type & a_module_dict)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "mctools::plain_simulated_data_input_module::initialize: "
                << "Module '" << get_name () << "' is already initialized ! ";
        throw std::logic_error (message.str ());
      }

    /**************************************************************
     *   fetch setup parameters from the configuration container  *
     **************************************************************/

    if (! is_debug ())
      {
        if (a_config.has_flag ("debug"))
          {
            set_debug (true);
          }
      }

    if (! _filenames_.is_valid ())
      {
        datatools::properties input_config;
        a_config.export_and_rename_starting_with (input_config, "input.", "");
        set_filenames (input_config);
        if (is_debug ())
          {
            _filenames_.dump (std::cerr);
          }
      }

    if (a_config.has_key ("input.max_files"))
      {
        int the_max_files = a_config.fetch_integer ("input.max_files");
        if (the_max_files < 1)
          {
            std::ostringstream message;
            message << "io_module::initialize: "
                    << "Module '" << get_name () << "' : invalid max number of files !";
                throw std::logic_error (message.str ());
          }
        set_max_files (the_max_files);
      }
    
    if (a_config.has_key ("input.max_record_total"))
      {
        int the_max_record_total = a_config.fetch_integer ("input.max_record_total");
        if (the_max_record_total < 1)
          {
            std::ostringstream message;
            message << "io_module::initialize: "
                    << "Module '" << get_name () << "' : invalid max total number of event record !";
            throw std::logic_error (message.str ());
          }
        set_max_record_total (the_max_record_total);
      }
    
    if (a_config.has_key ("input.max_record_per_file"))
      {
        int the_max_record_per_file = a_config.fetch_integer ("input.max_record_per_file");
        if (the_max_record_per_file < 1)
          {
            std::ostringstream message;
            message << "io_module::initialize: "
                    << "Module '" << get_name () << "' : invalid max number of event record per file !";
            throw std::logic_error (message.str ());
          }
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
    if (! is_initialized ())
      {
        std::ostringstream message;
        message << "mctools::plain_simulated_data_input_module::initialize: "
                << "Module '" << get_name () << "' is not initialized !";
        throw std::logic_error (message.str ());
      }

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
    if (! is_initialized ())
      {
        std::ostringstream message;
        message << "mctools::plain_simulated_data_input_module::process: "
                << "Module '" << get_name () << "' is not initialized !";
        throw std::logic_error (message.str ());
      }
    return _load (a_event_record);

    //return ERROR;
  }


  int plain_simulated_data_input_module::_load (datatools::things & a_event_record)
  {
    // attempt to open a source of event records :
    //while ((_bio_source_ == 0) && (_brio_source_ == 0))
    int load_status = OK;
    while (_bio_source_.get() == 0 && _brio_source_.get() == 0)
      {
        _file_record_counter_ = 0;
        _file_index_++;
        if (_file_index_ >= _filenames_.size ())
          {
            std::ostringstream message;
            message << "mctools::plain_simulated_data_input_module::_load: "
                    << "No more available input data file !";
            //throw std::logic_error (message.str ());
            std::cerr << datatools::io::error << message.str () << std::endl;
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
        }
        else if (ds::io_factory::guess_mode_from_filename (source_label, mode_guess)
                 == ds::io_factory::SUCCESS) {
          brio_format = false;
        }
        else {
          std::ostringstream message;
          message << "mctools::plain_simulated_data_input_module::_load: "
                  << "Cannot guess mode for input data file '" << source_label << "'!";
          //throw std::logic_error (message.str ());
          std::cerr << datatools::io::error << message.str () << std::endl;
          load_status = FATAL;
          return load_status;
        }
        uint32_t input_flags = mctools::i_data_source::blank;
        if (brio_format) {
          _brio_source_.reset(new brio::reader);
          _brio_source_.get()->open(source_label);
        }
        else {
          _bio_source_.reset(new datatools::data_reader);
          _bio_source_.get()->init_multi(source_label);
        }
    if ()
    //if (! _source_->is_open ()) _source_->open ();
    // check if we have some records in it :
        if (! _source_->has_next_record ())
          {
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
    if (load_it)
      {
        // Check if
        if ((a_event_record.size () > 0))
          {
            load_status = ERROR;
            return load_status;
          }
        a_event_record.clear ();
        if (_source_ != 0)
          {
            if (! _source_->has_next_record ())
              {
                std::ostringstream message;
                message << "mctools::plain_simulated_data_input_module::_load: "
                        << "No more available event record ! This is a bug !";
                throw std::logic_error (message.str ());
                /*
                  std::cerr << datatools::io::error << message.str () << std::endl;
                  load_status = FATAL;
                  return load_status;
                */
              }
            _source_->load_next_record (a_event_record);
            _file_record_counter_++;
            _record_counter_++;
          }
        else
          {
            std::ostringstream message;
            message << "mctools::plain_simulated_data_input_module::_load: "
                    << "No available data source ! This is a bug !";
            throw std::logic_error (message.str ());
            /*
              load_status = FATAL;
              return load_status;
            */
          }
      }

    bool stop_file   = false;
    bool stop_input = false;

    if (_max_record_total_ > 0)
      {
        if (_record_counter_ >= _max_record_total_)
          {
            stop_input = true;
            stop_file   = true;
            std::ostringstream message;
            message << "mctools::plain_simulated_data_input_module::_load: "
                    << "Module '" << get_name () << "' has reached the maximum number of records from the input data source (" << _max_record_total_ << ") !";
            std::clog << datatools::io::notice << message.str () << std::endl;
            //load_status = ERROR;
          }
      }

    if (_max_record_per_file_ > 0)
      {
        if (_file_record_counter_ >= _max_record_per_file_)
          {
            stop_file = true;
            std::ostringstream message;
            message << "mctools::plain_simulated_data_input_module::_load: "
                    << "Module '" << get_name () << "' has reached the maximum number of records from the current input file (" << _max_record_per_file_ << ") !";
            std::clog << datatools::io::notice << message.str () << std::endl;
          }
      }

    // check the end of the input file :
    if ((_source_ != 0) && (! _source_->has_next_record ()))
      {
        stop_file = true;
      }

    // manage the end of the input file :
    if (stop_file)
      {
        if (_source_ != 0)
          {
            _source_->reset ();
            delete _source_;
            _source_ = 0;
            _brio_source_ = 0;
            _bio_source_ = 0;
          }
        _file_record_counter_ = 0;
        int effective_max_files = _max_files_;
        if (_max_files_ < 0)
          {
            if (_filenames_.is_ranged())
              {
                effective_max_files = _filenames_.size ();
              }
          }
        if (effective_max_files > 0)
          {
            if ((_file_index_ + 1) >= effective_max_files)
              {
                stop_input = true;
                std::ostringstream message;
                message << "mctools::plain_simulated_data_input_module::_load: "
                        << "Module '" << get_name () 
                        << "' has reached the requested maximum number of input file(s) ("
                        << effective_max_files << ") !";
                std::clog << datatools::io::notice << message.str () << std::endl;
              }
          }
        if ((_file_index_ + 1) >= _filenames_.size ())
          {
            std::ostringstream message;
            message << "mctools::plain_simulated_data_input_module::_load: "
                    << "Module '" << get_name () 
                    << "' has loaded the last available input file (total is "
                    << _filenames_.size () << " file" << (_filenames_.size ()>1?"":"s") << ")!";
            std::clog << datatools::io::notice << message.str () << std::endl;
            stop_input = true;
          }
      }

    if (stop_input) {
        _terminated_ = true;
      }

    return load_status;
  }

} // end of namespace mctools

// end of plain_simulated_data_input_module.cc
