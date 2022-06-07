/* output_module.cc
 *
 * Copyright (C) 2013-2022 Francois Mauger <mauger@lpccaen.in2p3.fr>
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

// Standard library:
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/service_manager.h>
#include <datatools/exception.h>

// This project:
#include <dpp/output_module.h>
#include <dpp/io_common.h>
#include <dpp/simple_data_sink.h>
#include <dpp/simple_brio_data_sink.h>
#include <dpp/context_service.h>

namespace dpp {

  // Registration instantiation macro :
  DPP_MODULE_REGISTRATION_IMPLEMENT(output_module, "dpp::output_module")

  // Implementation of the interface :

  void output_module::set_limits(int max_record_total_,
                                 int max_record_per_file_,
                                 int max_files_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Output module '" << get_name() << "' is already initialized !");
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
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Output module '" << get_name() << "' is already initialized !");
    io_common & ioc = _grab_common();
    datatools::smart_filename::make_single(ioc.grab_filenames(), filepath_);
    return;
  }

  void output_module::set_list_of_output_files(const std::vector<std::string> & filepaths_,
                                               bool allow_duplicate_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Output module '" << get_name() << "' is already initialized !");
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
    DT_THROW_IF(is_initialized(), std::logic_error,
                "Output module '" << get_name() << "' is already initialized !");
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

  void output_module::set_preserve_existing_output(bool a_preserve_existing_output)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Output module '" << get_name() << "' is already initialized !");
    _preserve_existing_output_ = a_preserve_existing_output;
    return;
  }

  bool output_module::is_terminated() const
  {
    DT_THROW_IF(! is_initialized(),
                std::logic_error,
                "Output module '" << get_name() << "' is not initialized ! ");
    return get_common().is_terminated();
  }

  void output_module::_set_defaults()
  {
    _preserve_existing_output_ = false;
    _sink_   = nullptr;
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
      _common_->set_io(io_common::IO_OUTPUT);
    }
    return *_common_.get();
  }

  bool output_module::has_metadata_store() const
  {
    return (_common_.get() != nullptr);
  }

  const datatools::multi_properties & output_module::get_metadata_store() const
  {
    DT_THROW_IF(!has_metadata_store(),
                std::logic_error,
                "No embedded metadata store exists!");
    output_module * mutable_this = const_cast<output_module*>(this);
    io_common & ioc = mutable_this->_grab_common();
    return ioc.get_metadata_store();
  }

  datatools::multi_properties & output_module::grab_metadata_store()
  {
    io_common & ioc = _grab_common();
    return ioc.grab_metadata_store();
  }

  void output_module::clear_metadata_store()
  {
    if (_common_) {
      _common_->clear_metadata_store();
    }
    return;
  }

  void output_module::set_context_label(const std::string & label_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Output module '" << get_name() << "' is already initialized ! ");
    io_common & ioc = _grab_common();
    ioc.set_context_label(label_);
    return;
  }

  void output_module::export_context_metadata(const std::string & metadata_label_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Output module '" << get_name() << "' is already initialized ! ");
    io_common & ioc = _grab_common();
    ioc.add_context_metadata(metadata_label_);
    return;
  }

  void output_module::set_export_context_all(bool val_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Output module '" << get_name() << "' is already initialized ! ");
    io_common & ioc = _grab_common();
    ioc.set_context_all(val_);
    return;
  }

  output_module::output_module(datatools::logger::priority logging_priority_)
    : base_module(logging_priority_)
  {
    _set_defaults();
    return;
  }

  output_module::~output_module()
  {
    if (is_initialized()) output_module::reset();
    return;
  }

  void output_module::initialize(const datatools::properties & a_config,
                                 datatools::service_manager & a_service_manager,
                                 dpp::module_handle_dict_type & /*a_module_dict*/)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Output module '" << get_name() << "' is already initialized ! ");

    base_module::_common_initialize(a_config);

    /**************************************************************
     *   fetch setup parameters from the configuration container  *
     **************************************************************/
    if (a_config.has_flag("preserve_existing_files")) {
      set_preserve_existing_output(true);
    }

    if (! _common_) {
      _grab_common();
    }
    _common_.get()->initialize(a_config, a_service_manager);

    this->_open_sink_();

    /*************************************
     *  end of the initialization step   *
     *************************************/

    // Tag the module as initialized :
    _set_initialized(true);
    return;
  }

  void output_module::reset()
  {
    DT_THROW_IF(! is_initialized(),
                std::logic_error,
                "Output module '" << get_name() << "' is not initialized !");

    // Tag the module as un-initialized :
    _set_initialized(false);

    /****************************
     *  revert to some defaults *
     ****************************/

    if (_sink_ != nullptr) {
      if (_sink_->is_open()) {
        _sink_->close();
      }
      delete _sink_;
      _sink_ = nullptr;
    }
    _grab_common().clear_metadata_store();
    _common_.get()->reset();
    _common_.reset();
    _set_defaults();

    /****************************
     *  end of the reset step   *
     ****************************/

    return;
  }

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

  void output_module::_store_metadata_()
  {
    // First consider the external context service (if any) and its global metadata sections:
    if (get_common().has_context_service()) {
      const context_service & ctx_service = get_common().get_context_service();
      const datatools::multi_properties & ctx_store = ctx_service.get_store();
      const std::vector<std::string> & ctx_md_labels = get_common().get_context_metadata();
      int counter = 0;
      for (datatools::multi_properties::entries_ordered_col_type::const_iterator i
             = ctx_store.ordered_entries().begin();
           i != ctx_store.ordered_entries().end();
           i++) {
        const datatools::multi_properties::entry * p_entry = *i;
        const std::string & ctx_section_key = p_entry->get_key();
        const std::string & ctx_section_meta = p_entry->get_meta();
        bool store_it = false;
        if (get_common().is_context_all()) {
          // Store all context sections:
          store_it = true;
        } else {
          if (std::find(ctx_md_labels.begin(),
                        ctx_md_labels.end(),
                        ctx_section_key) != ctx_md_labels.end()) {
            // Store only context sections that are registered in the output module:
            store_it = true;
          }
        }
        if (store_it) {
          // Get a local copy:
          datatools::properties ctx_props = ctx_store.get_section(ctx_section_key);
          ctx_props.store_string(io_common::context_key(), ctx_section_key);
          ctx_props.store_string(io_common::context_meta(), ctx_section_meta);
          ctx_props.store_integer(io_common::context_rank(), counter);
          _sink_->store_metadata(ctx_props);
          counter++;
        }
      }
      /*
        for (unsigned int i(0); i < ctx_md_labels.size(); i++) {
        const std::string & md_label = ctx_md_labels[i];
        if (! ctx_store.has_section(md_label)) {
        DT_LOG_WARNING(get_logging_priority(),
        "Output module requested context metadata with label '"
        << md_label << "' but context service '"
        << get_common().get_context_label() << "' does not provide it !");
        continue;
        }
        // Get a local copy:
        datatools::properties ctx_props = ctx_store.get_section(md_label);
        ctx_props.store_string(io_common::context_key(), md_label);
        ctx_props.store_integer(io_common::context_rank(), i);
        _sink_->store_metadata(ctx_props);
        }
      */
    }

    // Then consider metadata to be stored from the embedded store to the output stream:
    datatools::multi_properties & MDS = grab_metadata_store();
    for (size_t i(0); i < MDS.size(); i++) {
      const std::string & key = MDS.ordered_key(i);
      const std::string & meta = MDS.get(key).get_meta();
      // Get a working local copy:
      datatools::properties props = MDS.grab_section(key);
      props.store_string(io_common::metadata_key(), key);
      props.store_string(io_common::metadata_meta(), meta);
      props.store_integer(io_common::metadata_rank(), i);
      _sink_->store_metadata(props);
    }
    return;
  }

  base_module::process_status output_module::_open_sink_()
  {
    if (_sink_ == nullptr) {
      _grab_common().set_file_index(get_common().get_file_index()+1);
      if (get_common().get_file_index() >= (int) get_common().get_filenames().size()) {
        _grab_common().set_terminated(true);
        return PROCESS_FATAL;
      }
      std::string sink_label = get_common().get_filenames()[get_common().get_file_index()];
      _sink_ = io_common::allocate_writer(sink_label, get_logging_priority());
      if (! _sink_) {
        DT_LOG_ERROR(get_logging_priority(),
                     "Cannot allocate any data writer for file '"
                     << sink_label << "' !");
        return PROCESS_FATAL;
      }
      if (_preserve_existing_output_) {
        _sink_->set_preserve_existing_sink(true);
      }
      if (! _sink_->is_open()) _sink_->open();
      _grab_common().set_file_record_counter(0);
      // Process metadata:
      _store_metadata_();
    }
    return PROCESS_OK;
  }

  base_module::process_status output_module::_store(const datatools::things & a_event_record)
  {
    process_status store_status = PROCESS_OK;
    if (_sink_ == nullptr) {
      // attempt to open a sink of event records :
      store_status = _open_sink_();
      if (store_status != PROCESS_OK) {
        return store_status;
      }
    }
    /*
      if (_sink_ == nullptr) {
      _grab_common().set_file_index(get_common().get_file_index()+1);
      if (get_common().get_file_index() >= (int)get_common().get_filenames().size()) {
      store_status = PROCESS_FATAL;
      return store_status;
      }
      std::string sink_label = get_common().get_filenames()[get_common().get_file_index()];
      _sink_ = io_common::allocate_writer(sink_label, get_logging_priority());
      if (! _sink_) {
      DT_LOG_ERROR(get_logging_priority(),
      "Cannot allocate any data writer for file '"
      << sink_label << "' !");
      store_status = PROCESS_FATAL;
      return store_status;
      }
      if (_preserve_existing_output_) {
      _sink_->set_preserve_existing_sink(true);
      }
      if (! _sink_->is_open()) _sink_->open();
      _grab_common().set_file_record_counter(0);
      // Process metadata:
      _store_metadata_();
      }
    */
    // force storage of the current data record :
    bool store_it = true;
    // store action :
    if (store_it) {
      if (_sink_ != nullptr) {
        bool ok = _sink_->store_next_record(a_event_record);
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
      if (get_common().get_record_counter() >= get_common().get_max_record_total()) {
        stop_output = true;
        stop_file   = true;
        DT_LOG_NOTICE(_logging, "Output module '" << get_name() << "' has reached the maximum number "
                      << "of records stored in the output data source (" << get_common().get_max_record_total() << ") !");
      }
    }
    if (get_common().get_max_record_per_file() > 0) {
      if (get_common().get_file_record_counter() >= get_common().get_max_record_per_file()) {
        stop_file = true;
        DT_LOG_NOTICE(_logging, "Output module '" << get_name() << "' has reached the maximum number "
                      << "of records to be stored in the current output file (" << get_common().get_max_record_per_file() << ") !");
      }
    }
    if (stop_file) {
      if (_sink_ != nullptr) {
        _sink_->reset();
        delete _sink_;
        _sink_ = nullptr;
      }
      _grab_common().set_file_record_counter(0);
      if (get_common().get_max_files() > 0) {
        if ((get_common().get_file_index() + 1) >= get_common().get_max_files()) {
          stop_output = true;
          DT_LOG_NOTICE(_logging, "Output module '" << get_name() << "' has reached the requested maximum number "
                        << "of output files (" << get_common().get_max_files() << ") !");
        }
      }
      if ((get_common().get_file_index() + 1) >= (int)get_common().get_filenames().size()) {
        stop_output = true;
        DT_LOG_NOTICE(_logging, "Output module '" << get_name() << "' has filled the last requested "
                      << "output file (total is " << get_common().get_filenames().size() << " files) !");
      }
    }
    if (stop_output) {
      _grab_common().set_terminated(true);
    }
    return store_status;
  }


  void output_module::tree_dump(std::ostream & a_out ,
                                const std::string & a_title,
                                const std::string & a_indent,
                                bool a_inherit) const
  {
    this->base_module::tree_dump(a_out, a_title, a_indent, is_initialized());
    if (! is_initialized()) {
      return;
    }
    std::string indent;
    if (! a_indent.empty()) {
      indent = a_indent;
    }

    a_out << indent << datatools::i_tree_dumpable::tag
          << "Preserve existing output : " << std::boolalpha << _preserve_existing_output_ << std::endl;

    if (_common_) {
      a_out << indent << datatools::i_tree_dumpable::tag
            << "Common   : " << std::endl;
      std::ostringstream indent2;
      indent2 << indent << datatools::i_tree_dumpable::skip_tag;
      get_common().tree_dump(a_out, "", indent2.str());
    }

    a_out << indent << datatools::i_tree_dumpable::tag
          << "Has metadata store : " << std::boolalpha << has_metadata_store() << std::endl;

    a_out << indent << datatools::i_tree_dumpable::inherit_tag(a_inherit)
          << "Sink : " << _sink_  << std::endl;

    return;
  }

} // namespace dpp

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
