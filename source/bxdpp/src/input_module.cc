/* input_module.cc
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
#include <dpp/input_module.h>
#include <dpp/io_common.h>
#include <dpp/simple_data_source.h>
#include <dpp/simple_brio_data_source.h>
#include <dpp/context_service.h>

namespace dpp {

  // Registration instantiation macro :
  DPP_MODULE_REGISTRATION_IMPLEMENT(input_module, "dpp::input_module")

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
    if (stop_ > 0) {
      datatools::smart_filename::make_incremental(ioc.grab_filenames(),
                                                  path_,
                                                  prefix_,
                                                  extension_,
                                                  stop_,
                                                  start_,
                                                  increment_
                                                  );
    } else {
      datatools::smart_filename::make_unranged_incremental(ioc.grab_filenames(),
                                                           path_,
                                                           prefix_,
                                                           extension_,
                                                           start_,
                                                           increment_
                                                           );
    }
    return;
  }

  bool input_module::has_source() const
  {
    return _source_ != nullptr;
  }

  const i_data_source & input_module::get_source() const
  {
    DT_THROW_IF(_source_ == nullptr,
                std::logic_error,
                "Input Module '" << get_name () << "' has no source !");
    return *_source_;
  }

  void input_module::set_context_label(const std::string & label_)
  {
    DT_THROW_IF(is_initialized (),
                std::logic_error,
                "Input Module '" << get_name () << "' is already initialized !");
    io_common & ioc = _grab_common();
    ioc.set_context_label(label_);
    return;
  }

  void input_module::import_context_metadata(const std::string & metadata_label_)
  {
    DT_THROW_IF(is_initialized (),
                std::logic_error,
                "Input module '" << get_name () << "' is already initialized ! ");
    io_common & ioc = _grab_common();
    ioc.add_context_metadata(metadata_label_);
    return;
  }

  void input_module::set_import_context_all(bool val_)
  {
    DT_THROW_IF(is_initialized (),
                std::logic_error,
                "Input module '" << get_name () << "' is already initialized ! ");
    io_common & ioc = _grab_common();
    ioc.set_context_all(val_);
  }

  bool input_module::is_terminated() const
  {
    DT_THROW_IF(! is_initialized (),
                std::logic_error,
                "Input module '" << get_name () << "' is not initialized !");
    /*
    if (! get_common().is_terminated()) {
      if (! _source_) {
        // need a mutable this:
        base_module::process_status status = _open_source();
      }
    }
    */
    return get_common().is_terminated();
  }

  void input_module::set_clear_record(bool cr_)
  {
    DT_THROW_IF(is_initialized (),
                std::logic_error,
                "Input module '" << get_name () << "' is already initialized !");
    _clear_record_ = cr_;
    return;
  }

  bool input_module::is_clear_record() const
  {
    return _clear_record_;
  }

  void input_module::_set_defaults()
  {
    _clear_record_ = false;
    _source_ = nullptr;
    _metadata_updated_ = false;
    //_metadata_preload_ = true;
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
      _common_->set_io(io_common::IO_INPUT);
    }
    return *_common_.get();
  }

  // Constructor :
  input_module::input_module(datatools::logger::priority logging_priority_)
    : base_module(logging_priority_)
  {
    _set_defaults();
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
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Input module '" << get_name () << "' is already initialized ! ");

    if (a_config.has_key("clear_record")) {
      set_clear_record(a_config.fetch_boolean("clear_record"));
    }

    // DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "common initialize...");
    _common_initialize(a_config);
    // DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "common initialize done.");

    /**************************************************************
     *   fetch setup parameters from the configuration container  *
     **************************************************************/
    if (! _common_) {
      // DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "I/O common does not exist. Create it!");
      _grab_common();
    } else {
      // DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "I/O common exists");
    }

    // DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "Initializing I/O common...");
    _common_.get()->initialize(a_config, a_service_manager);

    /*
    if (a_config.has_key("metadata.preload")) {
      _metadata_preload_ = a_config.fetch_boolean("metadata.preload");
    }
    */

    /*
    if (_metadata_preload_) {
      base_module::process_status status = _open_source();
    }
    */
    // DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "About to open the source");
    this->_open_source();
    // DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "Source open");

    /*************************************
     *  end of the initialization step   *
     *************************************/

    // Tag the module as initialized :
    _set_initialized(true);
    return;
  }

  // Reset :
  void input_module::reset()
  {
    DT_THROW_IF(! is_initialized (),
                std::logic_error,
                "Input module '" << get_name () << "' is not initialized !");

    // Tag the module as un-initialized :
    _set_initialized(false);

    /****************************
     *  revert to some defaults *
     ****************************/

    if (_source_ != nullptr) {
      if (_source_->is_open()) {
        _source_->close();
      }
      delete _source_;
      _source_ = nullptr;
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

  // Processing :
  base_module::process_status
  input_module::process(::datatools::things & a_data_record)
  {
    DT_THROW_IF(! is_initialized(),
                std::logic_error,
                "Input module '" << get_name() << "' is not initialized !");
    _metadata_updated_ = false;
    if (is_clear_record()) {
      a_data_record.clear();
    }
    return _load(a_data_record);
  }

  base_module::process_status
  input_module::_open_source()
  {
    while (_source_ == nullptr) {
      _grab_common().set_file_record_counter(0);
      _grab_common().set_file_index(get_common().get_file_index()+1);
      DT_LOG_TRACE(_logging, "file_index = " << get_common().get_file_index());
      if (get_common().get_filenames().is_ranged()) {
        if (get_common().get_file_index() >= (int)get_common().get_filenames().size()) {
          _grab_common().set_terminated(true);
          DT_LOG_TRACE (get_logging_priority(), "No more available input data file !");
          return PROCESS_STOP;
        }
      }
      std::string source_label = get_common().get_filenames()[get_common().get_file_index()];
      DT_LOG_TRACE(_logging, "Attempt to allocate the reader...");
      _source_ = io_common::allocate_reader(source_label, get_logging_priority());
      DT_LOG_TRACE(_logging, "Attempt was done.");
      if (! _source_) {
        DT_LOG_TRACE(_logging, "Cannot open file with index = " << get_common().get_file_index());
        _grab_common().set_terminated(true);
        DT_LOG_ERROR (get_logging_priority(),
                      "Cannot allocate any data reader for file '"
                      << source_label << "' !");
        return PROCESS_FATAL;
      }
      DT_LOG_TRACE(_logging, "Source is allocated...");
      if (! _source_->is_open()) {
        DT_LOG_TRACE(_logging, "Try to open the source...");
        _source_->open();
      }
      DT_LOG_TRACE(_logging, "Source is open.");
      DT_LOG_DEBUG(_logging, "Load metadata");
      _load_metadata_();
      DT_LOG_DEBUG(_logging, "Done");
      // Check if we have some records in it :
      if (! _source_->has_next_record()) {
        DT_LOG_TRACE(_logging, "NO NEXT RECORD.");
        _source_->reset();
        delete _source_;
        _source_ = nullptr;
      }
    }
    return PROCESS_OK;
  }

  base_module::process_status
  input_module::_load(datatools::things & a_data_record)
  {
    DT_LOG_TRACE(_logging, "Entering...");
    process_status load_status = PROCESS_OK;
    if (get_common().get_filenames().is_ranged()) {
      DT_LOG_TRACE(_logging, "filenames.size() = " << get_common().get_filenames().size());
    }
    if (!_source_) {
      // attempt to open a source file of event records :
      DT_LOG_TRACE(_logging, "Trying to open the source...");
      load_status = _open_source();
      if (load_status != PROCESS_OK) {
        DT_LOG_TRACE(_logging, "Source is NOT open.");
        return load_status;
      }
      DT_LOG_TRACE(_logging, "Source is open.");
    }

    // Force loading of the current event record :
    bool load_it = true;
    // Load action :
    if (load_it) {
      // Make sure the input data record is empty :
      if ((a_data_record.size() > 0)) {
        DT_LOG_TRACE(_logging, "Data record is not empty [size=" << a_data_record.size()<< "]!");
        load_status = PROCESS_ERROR;
        return load_status;
      }
      a_data_record.clear();
      DT_THROW_IF(_source_ == nullptr,
                  std::logic_error,
                  "Input module '" << get_name()
                  << "' : No available data source ! This is a bug !");
      DT_THROW_IF(! _source_->has_next_record (),
                  std::logic_error,
                  "Input module '" << get_name()
                  << "' : No more available event record ! This is a bug !");
      _source_->load_next_record(a_data_record);
      _grab_common().set_file_record_counter(get_common().get_file_record_counter()+1);
      _grab_common().set_record_counter(get_common().get_record_counter()+1);
    }

    _check_limits();
    return load_status;
  }

  void input_module::_check_limits()
  {
    bool stop_file  = false;
    bool stop_input = false;
    if (get_common().get_max_record_total() > 0) {
      if (get_common().get_record_counter() >= get_common().get_max_record_total()) {
        stop_input = true;
        stop_file  = true;
        DT_LOG_NOTICE(_logging,
                      "Input module '" << get_name () << "' has reached the maximum number "
                      << "of data records from the input data source ("
                      << get_common().get_max_record_total() << ") !");
      }
    }
    if (get_common().get_max_record_per_file() > 0) {
      if (get_common().get_file_record_counter() >= get_common().get_max_record_per_file()) {
        stop_file = true;
        std::ostringstream message;
        DT_LOG_NOTICE(_logging,
                      "Input module '" << get_name () << "' has reached the maximum number "
                      << "of records from the current input file ("
                      << get_common().get_max_record_per_file() << ") !");
      }
    }
    // check the end of the input file :
    if ((_source_ != nullptr) && (! _source_->has_next_record ())) {
      stop_file = true;
    }
    // manage the end of the input file :
    if (stop_file) {
      if (_source_ != nullptr) {
        _source_->reset ();
        delete _source_;
        _source_ = nullptr;
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
                        "Input module '" << get_name()
                        << "' has reached the requested maximum number of input file(s) ("
                        << effective_max_files << ") !");
        }
      }
      if (get_common().get_filenames().is_ranged()) {
        if ((get_common().get_file_index() + 1) >= (int)get_common().get_filenames().size()) {
          stop_input = true;
          std::ostringstream message;
          DT_LOG_NOTICE(_logging,
                        "Input module '" << get_name()
                        << "' has loaded the last available input file (total is "
                        << get_common().get_filenames().size ()
                        << " file" << (get_common().get_filenames().size()>1?"":"s") << ")!");
        }
      }
    }
    if (stop_input) {
      _grab_common().set_terminated(true);
    }
    return;
  }

  bool input_module::has_metadata_store() const
  {
    return (_common_.get() != nullptr);
  }

  const datatools::multi_properties & input_module::get_metadata_store() const
  {
    DT_THROW_IF(!has_metadata_store(),
                std::logic_error,
                "No embedded metadata store exists!");
    return _common_->get_metadata_store();
  }

  void input_module::clear_metadata_store()
  {
    if (has_metadata_store()) {
      _common_->clear_metadata_store();
    }
    return;
  }

  bool input_module::metadata_was_updated() const
  {
    return _metadata_updated_;
  }

  void input_module::_load_metadata_()
  {
    // std::cerr << "DEVEL: dpp::input_module::_load_metadata_: Entering..." << std::endl;
    _grab_common().clear_metadata_store();
    int64_t nmetadata = _source_->get_number_of_metadata();
    // std::cerr << "DEVEL: dpp::input_module::_load_metadata_: #metadata = "
    //           << nmetadata << std::endl;
    datatools::multi_properties * ctx_store = nullptr;
    const std::vector<std::string> & ctx_md_labels = get_common().get_context_metadata();
    if (get_common().has_context_service()) {
      // We define a handle to store embedded in the context service:
      context_service & ctx_service = _grab_common().grab_context_service();
      ctx_store = &ctx_service.grab_store();
    }
    datatools::multi_properties & MDS = _grab_common().grab_metadata_store();
    // MDS.tree_dump(std::cerr, "dpp::input_module:_load_metadata_: Input module metadata (INIT): ", "[devel] ");
    for (int64_t i(0); i < nmetadata; i++) {
      datatools::properties p;
      _source_->load_metadata(p, i);
      // p.tree_dump(std::cerr, "dpp::input_module:_load_metadata_: Input module metadata entry: ", "[devel] ");
      if (p.has_key(io_common::metadata_key())) {
        // Metadata to be embedded in the input module itself:
        std::string key = p.fetch_string(io_common::metadata_key());
        std::string meta;
        if (p.has_key(io_common::metadata_meta())) {
          meta = p.fetch_string(io_common::metadata_meta());
        }
        if (p.has_key(io_common::metadata_key())) p.clean(io_common::metadata_key());
        if (p.has_key(io_common::metadata_meta())) p.clean(io_common::metadata_meta());
        if (p.has_key(io_common::metadata_rank())) p.clean(io_common::metadata_rank());
        // if (p.has_key(io_common::metadata_topic())) p.clean(io_common::metadata_topic());
        MDS.add(key, meta, p);
      } else if (p.has_key(io_common::context_key())) {
        std::string ctx_section_key = p.fetch_string(io_common::context_key());
        std::string ctx_section_meta;
        if (p.has_key(io_common::context_meta())) {
          ctx_section_meta = p.fetch_string(io_common::context_meta());
        }
        // Metadata to be embedded in the context service store, if any:
        if (ctx_store) {
          bool load_it = false;
          if (get_common().is_context_all()) {
            // Load all context sections:
            load_it = true;
          } else {
            if (std::find(ctx_md_labels.begin(),
                          ctx_md_labels.end(),
                          ctx_section_key) != ctx_md_labels.end()) {
              // Load only context sections that are registered in the input module:
              load_it = true;
            }
          }
          if (load_it) {
            p.fetch_integer(io_common::context_rank());
            if (p.has_key(io_common::context_key())) p.clean(io_common::context_key());
            if (p.has_key(io_common::context_meta())) p.clean(io_common::context_meta());
            if (p.has_key(io_common::context_rank())) p.clean(io_common::context_rank());
            //if (p.has_key(io_common::context_topic())) p.clean(io_common::context_topic());
            if (ctx_store->has_section(ctx_section_key)) {
              // Update the existing store with new contents:
              ctx_store->remove(ctx_section_key);
            }
            ctx_store->add(ctx_section_key, ctx_section_meta, p);
          } else {
            DT_LOG_WARNING(get_logging_priority(),
                           "Input module '" << get_name()
                           << "' : Ignoring loaded context metadata with label '" << ctx_section_key << "' !");
          }
        }
      }
    }
    // MDS.tree_dump(std::cerr, "dpp::input_module:_load_metadata_: Input module metadata: ", "[devel] ");
    _metadata_updated_ = true;
    // std::cerr << "DEVEL: dpp::input_module::_load_metadata_: Exiting." << std::endl;
    return;
  }

  /*
  void input_module::set_preload_metadata(bool value_)
  {
    _metadata_preload_ = value_;
    return;
  }
  */

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

    /*
    a_out << indent << datatools::i_tree_dumpable::tag
          << "Preload metadata : " << _metadata_preload_ << std::endl;
    */

    a_out << indent << datatools::i_tree_dumpable::tag
          << "Updated metadata : " << _metadata_updated_ << std::endl;

    a_out << indent << datatools::i_tree_dumpable::inherit_tag (a_inherit)
          << "Source : " << _source_ << std::endl;

    return;
  }

} // namespace dpp
