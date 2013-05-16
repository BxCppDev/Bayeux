/* io_module.cc
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#include <stdexcept>
#include <sstream>

#include <datatools/properties.h>
#include <datatools/service_manager.h>
#include <datatools/ioutils.h>
#include <datatools/io_factory.h>
#include <datatools/exception.h>

#include <brio/utils.h>

#include <dpp/io_module.h>
#include <dpp/simple_data_sink.h>
#include <dpp/simple_data_source.h>
#include <dpp/simple_brio_data_sink.h>
#include <dpp/simple_brio_data_source.h>
#include <dpp/context_service.h>

namespace dpp {

  // Registration instantiation macro :
  DPP_MODULE_REGISTRATION_IMPLEMENT(io_module, "dpp::io_module");

  /*** Implementation of the interface ***/

  void io_module::set_context_service (dpp::context_service & a_context_service, const std::string & a_ctx_label)
  {
    DT_THROW_IF(is_initialized (),
                std::logic_error,
                "I/O module '" << get_name () << "' is already initialized !");
    _Ctx_label_ = a_ctx_label;
    _Ctx_service_ = &a_context_service;
    return;
  }

  void io_module::set_preserve_existing_output (bool a_preserve_existing_output)
  {
    DT_THROW_IF(is_initialized (),
                std::logic_error,
                "I/O module '" << get_name () << "' is already initialized !");
    _preserve_existing_output_ = a_preserve_existing_output;
    return;
  }

  void io_module::set_rw_mode (rw_mode_type a_rw_mode)
  {
    DT_THROW_IF(is_initialized (),
                std::logic_error,
                "I/O module '" << get_name () << "' is already initialized !");
    DT_THROW_IF(a_rw_mode != RW_MODE_INPUT && a_rw_mode != RW_MODE_OUTPUT,
                std::domain_error,
                "Invalid R/W mode' !");
    _rw_mode_ = a_rw_mode;
    return;
  }

  void io_module::set_max_files (int a_max_files)
  {
    DT_THROW_IF(is_initialized (),
                std::logic_error,
                "I/O module '" << get_name () << "' is already initialized !");
    DT_THROW_IF(a_max_files < 0,
                std::domain_error,
                "Invalid 'max_files' !");
    _max_files_ = a_max_files;
    return;
  }

  void io_module::set_max_record_total (int a_max_record_total)
  {
    DT_THROW_IF(is_initialized (),
                std::logic_error,
                "I/O module '" << get_name () << "' is already initialized !");
    DT_THROW_IF(a_max_record_total < 0,
                std::domain_error,
                "Invalid 'max_record_total' !");
    _max_record_total_ = a_max_record_total;
    return;
  }

  void io_module::set_max_record_per_file (int a_max_record_per_file)
  {
    DT_THROW_IF(is_initialized (),
                std::logic_error,
                "I/O module '" << get_name () << "' is already initialized !");
    DT_THROW_IF(a_max_record_per_file < 0,
                std::domain_error,
                "Invalid 'max_record_per_file' !");
    _max_record_per_file_ = a_max_record_per_file;
    return;
  }

  void io_module::set_filenames (const datatools::properties & a_setup)
  {
    DT_THROW_IF(is_initialized (),
                std::logic_error,
                "I/O module '" << get_name () << "' is already initialized !");
    if (_filenames_.is_valid ()) {
      _filenames_.reset ();
    }
    _filenames_.initialize (a_setup);
    DT_THROW_IF(! _filenames_.is_valid (),
                std::logic_error,
                "Invalid list of filenames for I/O module '" << get_name () << "' !");
    return;
  }

  bool io_module::is_terminated () const
  {
    return _terminated_;
  }

  bool io_module::is_input () const
  {
    return _rw_mode_ == RW_MODE_INPUT;
  }

  bool io_module::is_output () const
  {
    return _rw_mode_ == RW_MODE_OUTPUT;
  }

  void io_module::_set_defaults ()
  {
    _rw_mode_ = RW_MODE_INVALID;
    _max_record_per_file_ = 0;
    _max_record_total_ = 0;
    _max_files_ = -1;
    _preserve_existing_output_ = false;
    _Ctx_label_ = "";
    _Ctx_service_ = 0;
    _terminated_ = false;
    _file_record_counter_ = 0;
    _record_counter_ = 0;
    _file_index_ = -1;
    _source_ = 0;
    _sink_   = 0;
    _bio_source_ = 0;
    _bio_sink_   = 0;
    _brio_source_ = 0;
    _brio_sink_   = 0;
    return;
  }

  // Constructor :
  DPP_MODULE_CONSTRUCTOR_IMPLEMENT_HEAD(io_module,logging_priority_)
  {
    _set_defaults ();
    return;
  }


  DPP_MODULE_DEFAULT_DESTRUCTOR_IMPLEMENT(io_module)

  // Initialization :
  DPP_MODULE_INITIALIZE_IMPLEMENT_HEAD(io_module,
                                       a_config,
                                       a_service_manager,
                                       a_module_dict)
  {
    DT_THROW_IF(is_initialized (),
                std::logic_error,
                "I/O module '" << get_name () << "' is already initialized ! ");

    _common_initialize(a_config);

    /**************************************************************
     *   fetch setup parameters from the configuration container  *
     **************************************************************/

    if (_rw_mode_ == RW_MODE_INVALID) {
      if (a_config.has_key ("mode")) {
        std::string mode = a_config.fetch_string ("mode");
        if (mode == "output"){
          _rw_mode_ = RW_MODE_OUTPUT;
        } else if (mode == "input") {
          _rw_mode_ = RW_MODE_INPUT;
        }
        DT_THROW_IF(_rw_mode_ == RW_MODE_INVALID,
                    std::logic_error,
                    "I/O module '" << get_name () << "'  : invalid mode '" << mode << "'! ");
      }
    }

    if (_rw_mode_ == RW_MODE_OUTPUT) {
      if (! _filenames_.is_valid ()) {
        datatools::properties output_config;
        a_config.export_and_rename_starting_with (output_config, "output.", "");
        set_filenames (output_config);
        if (is_debug ()) {
          DT_LOG_DEBUG(_logging, "I/O module '" << get_name () << "'  : filenames: ");
          _filenames_.dump (std::cerr);
        }
      }
      if (a_config.has_flag ("output.preserve_existing")) {
        set_preserve_existing_output (true);
      }

      if (a_config.has_key ("output.max_files")) {
        _max_files_ = a_config.fetch_integer ("output.max_files");
        DT_THROW_IF(_max_files_ < 1,
                    std::logic_error,
                    "Module '" << get_name () << "' : invalid max number of files !");
      }

      if (a_config.has_key ("output.max_record_total")) {
        _max_record_total_ = a_config.fetch_integer ("output.max_record_total");
        DT_THROW_IF(_max_record_total_ < 1,
                    std::logic_error,
                    "Module '" << get_name () << "' : invalid max total number of data records !");
      }

      if (a_config.has_key ("output.max_record_per_file")) {
        _max_record_per_file_ = a_config.fetch_integer ("output.max_record_per_file");
        DT_THROW_IF(_max_record_per_file_ < 1,
                    std::logic_error,
                    "I/O module '" << get_name () << "' : invalid max number of data records per file !");
      }
    }

    if (_rw_mode_ == RW_MODE_INPUT) {
      if (! _filenames_.is_valid ()) {
        datatools::properties input_config;
        a_config.export_and_rename_starting_with (input_config, "input.", "");
        set_filenames (input_config);
        if (is_debug ()) {
          DT_LOG_DEBUG(_logging, "I/O module '" << get_name () << "'  : filenames: ");
          _filenames_.dump (std::cerr);
        }
      }
      size_t nb_output_files = _filenames_.size ();

      if (a_config.has_key ("input.max_files")) {
        int the_max_files = a_config.fetch_integer ("input.max_files");
        DT_THROW_IF(the_max_files < 1,
                    std::logic_error,
                    "I/O module '" << get_name () << "' : invalid max number of files !");
        set_max_files (the_max_files);
      }

      if (a_config.has_key ("input.max_record_total")) {
        int the_max_record_total = a_config.fetch_integer ("input.max_record_total");
        DT_THROW_IF(the_max_record_total < 1,
                    std::logic_error,
                    "I/O module '" << get_name () << "' : invalid max total number of data records !");
        set_max_record_total (the_max_record_total);
      }

      if (a_config.has_key ("input.max_record_per_file"))  {
        int the_max_record_per_file = a_config.fetch_integer ("input.max_record_per_file");
        DT_THROW_IF(the_max_record_per_file < 1,
                    std::logic_error,
                    "I/O module '" << get_name () << "' : invalid max number of data records per file !");
        set_max_record_per_file (the_max_record_per_file);
      }
    }

    if (_Ctx_service_ == 0) {
      if (a_config.has_key ("Ctx_label")) {
        _Ctx_label_ = a_config.fetch_string ("Ctx_label");
      }
      if (! _Ctx_label_.empty ()) {

        if (a_service_manager.has (_Ctx_label_)
            && a_service_manager.is_a<dpp::context_service> (_Ctx_label_)) {
          dpp::context_service & Ctx
            = a_service_manager.get<dpp::context_service> (_Ctx_label_);
          _Ctx_service_ = &Ctx;
        } else {
          DT_LOG_ERROR(_logging, "Service manager has no 'context' service labelled with '" << _Ctx_label_ << "' !");
        }
      }
    }

    if (_Ctx_service_ == 0) {
      DT_LOG_WARNING(_logging, "Module '" << get_name ()
                     << "' has no access to any 'context' service !");
    }

    /*************************************
     *  end of the initialization step   *
     *************************************/

    // Tag the module as initialized :
    _set_initialized (true);
    return;
  }

  // Reset :
  DPP_MODULE_RESET_IMPLEMENT_HEAD(io_module)
  {
    DT_THROW_IF(! is_initialized (),
                std::logic_error,
                "I/O module '" << get_name () << "' is not initialized !");

    /****************************
     *  revert to some defaults *
     ****************************/

    if (is_input ()) {
      if (_source_ != 0) {
        if (_source_->is_open ()) {
          _source_->close ();
        }
        delete _source_;
        _source_ = 0;
        _bio_source_ = 0;
        _brio_source_ = 0;
      }
    }

    if (is_output ()) {
      if (_sink_ != 0) {
        if (_sink_->is_open ()) {
          _sink_->close ();
        }
        delete _sink_;
        _sink_ = 0;
        _bio_sink_ = 0;
        _brio_sink_ = 0;
      }
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
  DPP_MODULE_PROCESS_IMPLEMENT_HEAD(io_module, a_data_record)
  {
    DT_THROW_IF(! is_initialized(),
                std::logic_error,
                "I/O module '" << get_name() << "' is not initialized !");
    if (_rw_mode_ == RW_MODE_INPUT) {
      return _load(a_data_record);
    }
    if (_rw_mode_ == RW_MODE_OUTPUT) {
      if (! _terminated_) {
        return _store(a_data_record);
      } else {
        return PROCESS_OK;
      }
    }
    return PROCESS_ERROR;
  }

  int io_module::_load (datatools::things & a_event_record)
  {
    DT_LOG_TRACE(_logging, "Entering...");
    DT_LOG_TRACE(_logging, "_filenames_.size () = " << _filenames_.size ());
    // attempt to open a source of event records :
    //while ((_bio_source_ == 0) && (_brio_source_ == 0))
    int load_status = PROCESS_OK;
    while (_source_ == 0) {
      _file_record_counter_ = 0;
      _file_index_++;
      DT_LOG_TRACE(_logging, "_file_index_ = " << _file_index_);
      if (_file_index_ >= _filenames_.size ()) {
        std::ostringstream message;
        message << "dpp::io_module::_load: "
                << "No more available input data file !";
        std::cerr << datatools::io::error << message.str () << std::endl;
        load_status = PROCESS_FATAL;
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
        std::ostringstream message;
        message << "dpp::io_module::_load: "
                << "Cannot guess mode for input data file '" << source_label << "'!";
        std::cerr << datatools::io::error << message.str () << std::endl;
        load_status = PROCESS_FATAL;
        return load_status;
      }
      uint32_t input_flags = dpp::i_data_source::blank;
      if (brio_format) {
        _brio_source_ = new dpp::simple_brio_data_source (source_label, input_flags);
        _source_ = _brio_source_;
      } else {
        _bio_source_ = new dpp::simple_data_source (source_label, input_flags);
        _source_ = _bio_source_;
      }
      if (! _source_->is_open ()) _source_->open ();
      DT_LOG_TRACE(_logging, "Source is open.");
      // check if we have some records in it :
      if (! _source_->has_next_record ()) {
        DT_LOG_TRACE(_logging, "NO NEXT RECORD.");
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
        load_status = PROCESS_ERROR;
        return load_status;
      }
      a_event_record.clear ();
      DT_THROW_IF(_source_ == 0,
                  std::logic_error,
                  "No available data source ! This is a bug !");
      DT_THROW_IF(! _source_->has_next_record (),
                  std::logic_error,
                  "No more available event record ! This is a bug !");
      _source_->load_next_record (a_event_record);
      _file_record_counter_++;
      _record_counter_++;
    }
    bool stop_file   = false;
    bool stop_input = false;
    if (_max_record_total_ > 0) {
      if (_record_counter_ >= _max_record_total_) {
        stop_input = true;
        stop_file   = true;
        DT_LOG_NOTICE(_logging,
                      "Module '" << get_name () << "' has reached the maximum number "
                      << "of data records from the input data source (" << _max_record_total_ << ") !");
      }
    }
    if (_max_record_per_file_ > 0) {
      if (_file_record_counter_ >= _max_record_per_file_) {
        stop_file = true;
        std::ostringstream message;
        DT_LOG_NOTICE(_logging,
                      "Module '" << get_name () << "' has reached the maximum number "
                      << "of records from the current input file (" << _max_record_per_file_ << ") !");
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
        if ((_file_index_ + 1) >= effective_max_files) {
          stop_input = true;
          DT_LOG_NOTICE(_logging,
                        "Module '" << get_name () << "' has reached the requested maximum number of input file(s) ("
                        << effective_max_files << ") !");
        }
      }
      if ((_file_index_ + 1) >= _filenames_.size ()) {
        stop_input = true;
        std::ostringstream message;
        DT_LOG_NOTICE(_logging,
                      "Module '" << get_name () << "' has loaded the last available input file (total is "
                      << _filenames_.size () << " file" << (_filenames_.size ()>1?"":"s") << ")!");
      }
    }
    if (stop_input) {
      _terminated_ = true;
    }
    return load_status;
  }

  int io_module::_store (const datatools::things & a_event_record)
  {
    int store_status = PROCESS_OK;
    if (_sink_ == 0) {
      _file_index_++;
      if (_file_index_ >= _filenames_.size ()) {
        store_status = PROCESS_FATAL;
        return store_status;
      }
      std::string sink_label = _filenames_[_file_index_];
      int mode_guess;
      bool brio_format = false;
      namespace ds = datatools;
      if (brio::store_info::guess_mode_from_filename (sink_label, mode_guess)
          == brio::store_info::SUCCESS) {
        brio_format = true;
      } else if (ds::io_factory::guess_mode_from_filename (sink_label, mode_guess)
                 == ds::io_factory::SUCCESS) {
        brio_format = false;
      } else {
        DT_LOG_FATAL(_logging, "Cannot guess mode for output data file '" << sink_label << "'!");
        store_status = PROCESS_FATAL;
        return store_status;
      }
      uint32_t output_flags = dpp::i_data_sink::blank;
      if (_preserve_existing_output_) {
        output_flags |= dpp::i_data_sink::preserve_existing_sink;
      }
      if (brio_format) {
        _brio_sink_ = new dpp::simple_brio_data_sink (sink_label, output_flags);
        _sink_ = _brio_sink_;
      } else {
        _bio_sink_ = new dpp::simple_data_sink (sink_label, output_flags);
        _sink_ = _bio_sink_;
      }
      if (! _sink_->is_open ()) _sink_->open ();
      _file_record_counter_ = 0;
    }
    // force storage of the current data record :
    bool store_it = true;
    // store action :
    if (store_it) {
      if (_sink_ != 0) {
        bool ok = _sink_->store_next_record (a_event_record);
        if (! ok) {
          DT_LOG_ERROR(_logging, "Cannot store the data record ! This is a bug !");
          store_status = PROCESS_FATAL;
          return store_status;
        }
        _file_record_counter_++;
        _record_counter_++;
      } else {
        DT_LOG_ERROR(_logging, "No available data sink ! This is a bug !");
        store_status = PROCESS_FATAL;
        return store_status;
      }
    }

    bool stop_file   = false;
    bool stop_output = false;
    if (_max_record_total_ > 0) {
      if (_record_counter_ >= _max_record_total_) {
        stop_output = true;
        stop_file   = true;
        DT_LOG_NOTICE(_logging, "Module '" << get_name () << "' has reached the maximum number "
                      << "of records stored in the output data source (" << _max_record_total_ << ") !");
      }
    }
    if (_max_record_per_file_ > 0) {
      if (_file_record_counter_ >= _max_record_per_file_) {
        stop_file = true;
        DT_LOG_NOTICE(_logging, "Module '" << get_name () << "' has reached the maximum number "
                      << "of records to be stored in the current output file (" << _max_record_per_file_ << ") !");
      }
    }
    if (stop_file) {
      if (_sink_ != 0) {
        _sink_->reset ();
        delete _sink_;
        _sink_ = 0;
        _brio_sink_ = 0;
        _bio_sink_ = 0;
      }
      _file_record_counter_ = 0;
      if (_max_files_ > 0) {
        if ((_file_index_ + 1) >= _max_files_) {
          stop_output = true;
          DT_LOG_NOTICE(_logging, "Module '" << get_name () << "' has reached the requested maximum number "
                        << "of output files (" << _max_files_ << ") !");
        }
      }
      if ((_file_index_ + 1) >= _filenames_.size ()) {
        stop_output = true;
        DT_LOG_NOTICE(_logging, "Module '" << get_name () << "' has filled the last requested "
                      << "output file (total is " << _filenames_.size () << " files) !");
      }
    }
    if (stop_output) {
      _terminated_ = true;
    }
    return store_status;
  }

  void io_module::tree_dump (std::ostream & a_out ,
                             const std::string & a_title,
                             const std::string & a_indent,
                             bool a_inherit) const
  {
    this->base_module::tree_dump (a_out, a_title, a_indent, is_initialized ());
    if (! is_initialized ()) {
      return;
    }
    std::string indent;
    if (! a_indent.empty ())
      {
        indent = a_indent;
      }
    a_out << indent << datatools::i_tree_dumpable::tag
          << "R/W mode              : " << _rw_mode_ << " ";
    if (is_input ()) a_out << "(input)";
    else if (is_output ()) a_out << "(output)";
    a_out << std::endl;

    if (is_output ()) {
      a_out << indent << datatools::i_tree_dumpable::tag
            << "Preserve existing output : " << _preserve_existing_output_ << std::endl;
    }

    a_out << indent << datatools::i_tree_dumpable::tag
          << "Max record per file   : " << _max_record_per_file_ << std::endl;

    a_out << indent << datatools::i_tree_dumpable::tag
          << "Max record total      : " << _max_record_total_ << std::endl;

    a_out << indent << datatools::i_tree_dumpable::tag
          << "Max files             : " << _max_files_ << std::endl;

    a_out << indent << datatools::i_tree_dumpable::tag
          << "Terminated            : " << _terminated_ << std::endl;

    a_out << indent << datatools::i_tree_dumpable::tag
          << "Current file record counter   : " << _file_record_counter_ << std::endl;

    a_out << indent << datatools::i_tree_dumpable::tag
          << "Current record counter        : " << _record_counter_ << std::endl;

    a_out << indent << datatools::i_tree_dumpable::tag
          << "Current file index            : " << _file_index_ << std::endl;

    if (is_input ()) {
      a_out << indent << datatools::i_tree_dumpable::tag
            << "Source : " << _source_
            << " (bio @ " << _bio_source_
            << ", brio @ " << _brio_source_ << ')' << std::endl;
    }
    if (is_output ()) {
      a_out << indent << datatools::i_tree_dumpable::tag
            << "Sink : " << _sink_ << " (bio @ " << _bio_sink_ << ", brio @ " << _brio_sink_ << ')' << std::endl;
    }

    a_out << indent << datatools::i_tree_dumpable::tag
          << "Context service label : '" << _Ctx_label_ << "'" << std::endl;

    a_out << indent << datatools::i_tree_dumpable::inherit_tag (a_inherit)
          << "Context service @ " << _Ctx_service_ << std::endl;

    return;
  }

  void io_module::print (ostream & a_out) const
  {
    this->tree_dump (a_out, "I/O module :");
    return;
  }

} // namespace dpp

// end of io_module.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
