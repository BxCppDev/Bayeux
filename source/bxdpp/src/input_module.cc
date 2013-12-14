/* input_module.cc
 *
 * Copyright (C) 2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
#include <datatools/exception.h>

#include <dpp/input_module.h>
#include <dpp/io_common.h>
#include <dpp/simple_data_source.h>
#include <dpp/simple_brio_data_source.h>

namespace dpp {

  // Registration instantiation macro :
  DPP_MODULE_REGISTRATION_IMPLEMENT(input_module, "dpp::input_module");

  /*** Implementation of the interface ***/

  void input_module::set_limits(int max_record_total_,
                                int max_record_per_file_,
                                int max_files_)
  {
    DT_THROW_IF (is_initialized (), std::logic_error,
                 "Input module '" << get_name () << "' is already initialized !");
    io_common & ioc = _grab_common();
    if (max_record_total_ > 0) {
      ioc.set_max_record_total(max_record_total_);
    }
    if (max_record_per_file_ > 0) {
      ioc.set_max_record_per_file(max_record_per_file_);
    }
    if (max_files_ > 0) {
      ioc.set_max_files(max_files_);
    }
    return;
  }

  void input_module::set_single_input_file(const std::string & filepath_)
  {
    DT_THROW_IF (is_initialized (), std::logic_error,
                 "Input module '" << get_name () << "' is already initialized !");
    io_common & ioc = _grab_common();
    datatools::smart_filename::make_single(ioc.grab_filenames(), filepath_);
    return;
  }

  void input_module::set_list_of_input_files(const std::vector<std::string> & filepaths_,
                                             bool allow_duplicate_)
  {
    DT_THROW_IF (is_initialized (), std::logic_error,
                 "Input module '" << get_name () << "' is already initialized !");
    io_common & ioc = _grab_common();
    datatools::smart_filename & filenames = ioc.grab_filenames();
    datatools::smart_filename::make_list(filenames, allow_duplicate_);
    for (size_t i = 0; i < filepaths_.size(); i++) {
      filenames.add_to_list(filepaths_[i]);
    }
    return;
  }

  void input_module::set_incremental_input_files(const std::string & path_,
                                                 const std::string & prefix_,
                                                 const std::string & extension_,
                                                 unsigned int stop_,
                                                 unsigned int start_,
                                                 int increment_)
  {
    DT_THROW_IF (is_initialized (), std::logic_error,
                 "Input module '" << get_name () << "' is already initialized !");
    io_common & ioc = _grab_common();
    datatools::smart_filename::make_incremental(ioc.grab_filenames(),
                                                path_,
                                                prefix_,
                                                extension_,
                                                stop_,
                                                start_,
                                                increment_
                                                );
    return;
  }

  bool input_module::is_terminated () const
  {
    DT_THROW_IF(! is_initialized (),
                std::logic_error,
                "Input module '" << get_name () << "' is not initialized !");
    return get_common().is_terminated();
  }

  void input_module::_set_defaults ()
  {
    _source_ = 0;
    return;
  }

  const io_common & input_module::get_common() const
  {
    DT_THROW_IF(! _common_, std::logic_error,
                "No internal input common data is defined !");
    return *_common_.get();
  }

  io_common & input_module::_grab_common()
  {
    if (! _common_) {
      _common_.reset(new io_common(_logging, get_name()));
    }
    return *_common_.get();
  }

  // Constructor :
  input_module::input_module(datatools::logger::priority logging_priority_)
    : base_module(logging_priority_)
  {
    _set_defaults ();
    return;
  }

  input_module::~input_module()
  {
    if (is_initialized()) input_module::reset();
    return;
  }

  // Initialization :
  void input_module::initialize(const datatools::properties & a_config,
                                datatools::service_manager & a_service_manager,
                                dpp::module_handle_dict_type & /*a_module_dict*/)
  {
    DT_THROW_IF(is_initialized (),
                std::logic_error,
                "Input module '" << get_name () << "' is already initialized ! ");

    _common_initialize(a_config);

    /**************************************************************
     *   fetch setup parameters from the configuration container  *
     **************************************************************/
    if (! _common_) {
      _grab_common();
    }
    _common_.get()->initialize(a_config, a_service_manager);

    /*************************************
     *  end of the initialization step   *
     *************************************/

    // Tag the module as initialized :
    _set_initialized (true);
    return;
  }

  // Reset :
  void input_module::reset()
  {
    DT_THROW_IF(! is_initialized (),
                std::logic_error,
                "Input module '" << get_name () << "' is not initialized !");

    // Tag the module as un-initialized :
    _set_initialized (false);

    /****************************
     *  revert to some defaults *
     ****************************/

    if (_source_ != 0) {
      if (_source_->is_open ()) {
        _source_->close ();
      }
      delete _source_;
      _source_ = 0;
    }

    _common_.get()->reset();
    _common_.reset(0);
    _set_defaults ();

    /****************************
     *  end of the reset step   *
     ****************************/

    return;
  }

  // Processing :
  base_module::process_status
  input_module::process(::datatools::things & a_data_record)
  {
    DT_THROW_IF(! is_initialized(),
                std::logic_error,
                "Input module '" << get_name() << "' is not initialized !");
    return _load(a_data_record);
  }

  base_module::process_status
  input_module::_load (datatools::things & a_data_record)
  {
    DT_LOG_TRACE(_logging, "Entering...");
    DT_LOG_TRACE(_logging, "filenames.size () = " << get_common().get_filenames().size ());
    // attempt to open a source of event records :
    process_status load_status = PROCESS_OK;
    while (_source_ == 0) {
      _grab_common().set_file_record_counter(0);
      _grab_common().set_file_index(get_common().get_file_index()+1);
      DT_LOG_TRACE(_logging, "file_index = " << get_common().get_file_index());
      if (get_common().get_file_index() >= (int)get_common().get_filenames().size ()) {
        DT_LOG_ERROR (get_logging_priority (), "No more available input data file !");
        load_status = PROCESS_FATAL;
        return load_status;
      }
      std::string source_label = get_common().get_filenames()[get_common().get_file_index()];
      _source_ = io_common::allocate_reader(source_label, get_logging_priority ());
      if (! _source_) {
        DT_LOG_ERROR (get_logging_priority (),
                      "Cannot allocate any data reader for file '"
                      << source_label << "' !");
        load_status = PROCESS_FATAL;
        return load_status;
      }

      if (! _source_->is_open ()) _source_->open ();
      DT_LOG_TRACE(_logging, "Source is open.");
      // check if we have some records in it :
      if (! _source_->has_next_record ()) {
        DT_LOG_TRACE(_logging, "NO NEXT RECORD.");
        _source_->reset ();
        delete _source_;
        _source_ = 0;
      }
    }
    // force loading of the current event record :
    bool load_it = true;
    // load action :
    if (load_it) {
      // Check if
      if ((a_data_record.size () > 0)) {
        load_status = PROCESS_ERROR;
        return load_status;
      }
      a_data_record.clear ();
      DT_THROW_IF(_source_ == 0,
                  std::logic_error,
                  "No available data source ! This is a bug !");
      DT_THROW_IF(! _source_->has_next_record (),
                  std::logic_error,
                  "No more available event record ! This is a bug !");
      _source_->load_next_record (a_data_record);
      _grab_common().set_file_record_counter(get_common().get_file_record_counter()+1);
      _grab_common().set_record_counter(get_common().get_record_counter()+1);
    }
    bool stop_file   = false;
    bool stop_input = false;
    if (get_common().get_max_record_total() > 0) {
      if (get_common().get_record_counter() >= get_common().get_max_record_total()) {
        stop_input = true;
        stop_file   = true;
        DT_LOG_NOTICE(_logging,
                      "Input module '" << get_name () << "' has reached the maximum number "
                      << "of data records from the input data source (" << get_common().get_max_record_total() << ") !");
      }
    }
    if (get_common().get_max_record_per_file() > 0) {
      if (get_common().get_file_record_counter() >= get_common().get_max_record_per_file()) {
        stop_file = true;
        std::ostringstream message;
        DT_LOG_NOTICE(_logging,
                      "Input module '" << get_name () << "' has reached the maximum number "
                      << "of records from the current input file (" << get_common().get_max_record_per_file() << ") !");
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
      }
      _grab_common().set_file_record_counter(0);
      int effective_max_files = get_common().get_max_files();
      if (get_common().get_max_files() < 0) {
        if (get_common().get_filenames().is_ranged()) {
          effective_max_files = get_common().get_filenames().size ();
        }
      }
      if (effective_max_files > 0) {
        if ((get_common().get_file_index() + 1) >= effective_max_files) {
          stop_input = true;
          DT_LOG_NOTICE(_logging,
                        "Input module '" << get_name () << "' has reached the requested maximum number of input file(s) ("
                        << effective_max_files << ") !");
        }
      }
      if ((get_common().get_file_index() + 1) >= (int)get_common().get_filenames().size ()) {
        stop_input = true;
        std::ostringstream message;
        DT_LOG_NOTICE(_logging,
                      "Input module '" << get_name () << "' has loaded the last available input file (total is "
                      << get_common().get_filenames().size ()
                      << " file" << (get_common().get_filenames().size ()>1?"":"s") << ")!");
      }
    }
    if (stop_input) {
      _grab_common().set_terminated(true);
    }
    return load_status;
  }

  void input_module::tree_dump (std::ostream & a_out ,
                                const std::string & a_title,
                                const std::string & a_indent,
                                bool a_inherit) const
  {
    this->base_module::tree_dump (a_out, a_title, a_indent, is_initialized ());
    if (! is_initialized ()) {
      return;
    }

    std::string indent;
    if (! a_indent.empty ()) {
      indent = a_indent;
    }

    if (_common_) {
      a_out << indent << datatools::i_tree_dumpable::tag
            << "Common   : " << std::endl;
      std::ostringstream indent2;
      indent2 << indent << datatools::i_tree_dumpable::skip_tag;
      get_common().tree_dump(a_out, "", indent2.str());
    }

    a_out << indent << datatools::i_tree_dumpable::inherit_tag (a_inherit)
          << "Source : " << _source_ << std::endl;

    return;
  }

} // namespace dpp

// end of input_module.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
