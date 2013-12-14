/* output_module.cc
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

#include <dpp/output_module.h>
#include <dpp/io_common.h>
#include <dpp/simple_data_sink.h>
#include <dpp/simple_brio_data_sink.h>

namespace dpp {

  // Registration instantiation macro :
  DPP_MODULE_REGISTRATION_IMPLEMENT(output_module, "dpp::output_module");

  /*** Implementation of the interface ***/

  void output_module::set_limits(int max_record_total_,
                                 int max_record_per_file_,
                                 int max_files_)
  {
    DT_THROW_IF (is_initialized (), std::logic_error,
                 "Output module '" << get_name () << "' is already initialized !");
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

  void output_module::set_single_output_file(const std::string & filepath_)
  {
    DT_THROW_IF (is_initialized (), std::logic_error,
                 "Output module '" << get_name () << "' is already initialized !");
    io_common & ioc = _grab_common();
    datatools::smart_filename::make_single(ioc.grab_filenames(), filepath_);
    return;
  }

  void output_module::set_list_of_output_files(const std::vector<std::string> & filepaths_,
                                               bool allow_duplicate_)
  {
    DT_THROW_IF (is_initialized (), std::logic_error,
                 "Output module '" << get_name () << "' is already initialized !");
    io_common & ioc = _grab_common();
    datatools::smart_filename & filenames = ioc.grab_filenames();
    datatools::smart_filename::make_list(filenames, allow_duplicate_);
    for (size_t i = 0; i < filepaths_.size(); i++) {
      filenames.add_to_list(filepaths_[i]);
    }
    return;
  }

  void output_module::set_incremental_output_files(const std::string & path_,
                                                 const std::string & prefix_,
                                                 const std::string & extension_,
                                                 unsigned int stop_,
                                                 unsigned int start_,
                                                 int increment_)
  {
    DT_THROW_IF (is_initialized (), std::logic_error,
                 "Output module '" << get_name () << "' is already initialized !");
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

  void output_module::set_preserve_existing_output (bool a_preserve_existing_output)
  {
    DT_THROW_IF(is_initialized (),
                std::logic_error,
                "Output module '" << get_name () << "' is already initialized !");
    _preserve_existing_output_ = a_preserve_existing_output;
    return;
  }

  bool output_module::is_terminated () const
  {
    DT_THROW_IF(! is_initialized (),
                std::logic_error,
                "Output module '" << get_name () << "' is not initialized ! ");
    return get_common().is_terminated();
  }

  void output_module::_set_defaults ()
  {
    _preserve_existing_output_ = false;
    _sink_   = 0;
    return;
  }

  const io_common & output_module::get_common() const
  {
    DT_THROW_IF(! _common_, std::logic_error,
                "No internal output common data is defined !");
    return *_common_.get();
  }

  io_common & output_module::_grab_common()
  {
    if (! _common_) {
      _common_.reset(new io_common(_logging, get_name()));
    }
    return *_common_.get();
  }

  // Constructor :
  output_module::output_module(datatools::logger::priority logging_priority_)
    : base_module(logging_priority_)
  {
    _set_defaults ();
    return;
  }

  output_module::~output_module()
  {
    if (is_initialized()) output_module::reset();
    return;
  }

  // Initialization :
  void output_module::initialize(const datatools::properties & a_config,
                                 datatools::service_manager & a_service_manager,
                                 dpp::module_handle_dict_type & /*a_module_dict*/)
  {
    DT_THROW_IF(is_initialized (),
                std::logic_error,
                "Output module '" << get_name () << "' is already initialized ! ");

    _common_initialize(a_config);

    /**************************************************************
     *   fetch setup parameters from the configuration container  *
     **************************************************************/
    if (! _common_) {
      _grab_common();
    }
    _common_.get()->initialize(a_config, a_service_manager);

    if (a_config.has_flag ("preserve_existing_files")) {
      set_preserve_existing_output (true);
    }

    /*************************************
     *  end of the initialization step   *
     *************************************/

    // Tag the module as initialized :
    _set_initialized (true);
    return;
  }

  // Reset :
  void output_module::reset()
  {
    DT_THROW_IF(! is_initialized (),
                std::logic_error,
                "Output module '" << get_name () << "' is not initialized !");

    // Tag the module as un-initialized :
    _set_initialized (false);

    /****************************
     *  revert to some defaults *
     ****************************/

    if (_sink_ != 0) {
      if (_sink_->is_open ()) {
        _sink_->close ();
      }
      delete _sink_;
      _sink_ = 0;
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
  output_module::process(::datatools::things & a_data_record)
  {
    DT_THROW_IF(! is_initialized(),
                std::logic_error,
                "Output module '" << get_name() << "' is not initialized !");
    if (! is_terminated()) {
      return _store(a_data_record);
    }
    return PROCESS_OK;
  }

  base_module::process_status
  output_module::_store (const datatools::things & a_event_record)
  {
    process_status store_status = PROCESS_OK;
    if (_sink_ == 0) {
      _grab_common().set_file_index(get_common().get_file_index()+1);
      if (get_common().get_file_index() >= (int)get_common().get_filenames().size ()) {
        store_status = PROCESS_FATAL;
        return store_status;
      }
      std::string sink_label = get_common().get_filenames()[get_common().get_file_index()];
      _sink_ = io_common::allocate_writer(sink_label, get_logging_priority ());
      if (! _sink_) {
        DT_LOG_ERROR (get_logging_priority (),
                      "Cannot allocate any data writer for file '"
                      << sink_label << "' !");
        store_status = PROCESS_FATAL;
        return store_status;
      }
      if (_preserve_existing_output_) {
        _sink_->set_preserve_existing_sink (true);
      }
      if (! _sink_->is_open ()) _sink_->open ();
      _grab_common().set_file_record_counter(0);
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
        _grab_common().set_file_record_counter(get_common().get_file_record_counter()+1);
        _grab_common().set_record_counter(get_common().get_record_counter()+1);
      } else {
        DT_LOG_ERROR(_logging, "No available data sink ! This is a bug !");
        store_status = PROCESS_FATAL;
        return store_status;
      }
    }

    bool stop_file   = false;
    bool stop_output = false;
    if (get_common().get_max_record_total() > 0) {
      if (get_common().get_file_record_counter() >= get_common().get_max_record_total()) {
        stop_output = true;
        stop_file   = true;
        DT_LOG_NOTICE(_logging, "Output module '" << get_name () << "' has reached the maximum number "
                      << "of records stored in the output data source (" << get_common().get_max_record_total() << ") !");
      }
    }
    if (get_common().get_max_record_per_file() > 0) {
      if (get_common().get_file_record_counter() >= get_common().get_max_record_per_file()) {
        stop_file = true;
        DT_LOG_NOTICE(_logging, "Output module '" << get_name () << "' has reached the maximum number "
                      << "of records to be stored in the current output file (" << get_common().get_max_record_per_file() << ") !");
      }
    }
    if (stop_file) {
      if (_sink_ != 0) {
        _sink_->reset ();
        delete _sink_;
        _sink_ = 0;
      }
      _grab_common().set_file_record_counter(0);
      if (get_common().get_max_files() > 0) {
        if ((get_common().get_file_index() + 1) >= get_common().get_max_files()) {
          stop_output = true;
          DT_LOG_NOTICE(_logging, "Output module '" << get_name () << "' has reached the requested maximum number "
                        << "of output files (" << get_common().get_max_files() << ") !");
        }
      }
      if ((get_common().get_file_index() + 1) >= (int)get_common().get_filenames().size ()) {
        stop_output = true;
        DT_LOG_NOTICE(_logging, "Output module '" << get_name () << "' has filled the last requested "
                      << "output file (total is " << get_common().get_filenames().size () << " files) !");
      }
    }
    if (stop_output) {
      _grab_common().set_terminated(true);
    }
    return store_status;
  }

  void output_module::tree_dump (std::ostream & a_out ,
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

    a_out << indent << datatools::i_tree_dumpable::tag
          << "Preserve existing output : " << _preserve_existing_output_ << std::endl;

    a_out << indent << datatools::i_tree_dumpable::inherit_tag (a_inherit)
          << "Sink : " << _sink_  << std::endl;

    return;
  }

} // namespace dpp

// end of output_module.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
