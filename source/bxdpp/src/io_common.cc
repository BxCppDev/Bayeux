/* io_common.cc
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

// Standard library:
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/multi_properties.h>
#include <datatools/service_manager.h>
#include <datatools/exception.h>
#include <datatools/io_factory.h>
// - Bayeux/brio:
#include <brio/utils.h>

// This project:
#include <dpp/io_common.h>
#include <dpp/context_service.h>
#include <dpp/simple_data_source.h>
#include <dpp/simple_brio_data_source.h>
#include <dpp/simple_data_sink.h>
#include <dpp/simple_brio_data_sink.h>

namespace dpp {

  /*** Implementation of the interface ***/

  // static
  const std::string & io_common::metadata_key()
  {
    static std::string s("__dpp.io.metadata.key");
    return s;
  }

  // static
  const std::string & io_common::metadata_meta()
  {
    static std::string s("__dpp.io.metadata.meta");
    return s;
  }

  // static
  const std::string & io_common::metadata_rank()
  {
    static std::string s("__dpp.io.metadata.rank");
    return s;
  }

  // static
  const std::string & io_common::metadata_topic()
  {
    static std::string s("__dpp.io.metadata.topic");
    return s;
  }

  // static
  const std::string & io_common::context_key()
  {
    static std::string s("__dpp.io.context.key");
    return s;
  }

  // static
  const std::string & io_common::context_meta()
  {
    static std::string s("__dpp.io.context.meta");
    return s;
  }

  // static
  const std::string & io_common::context_topic()
  {
    static std::string s("__dpp.io.context.topic");
    return s;
  }

  // static
  const std::string & io_common::context_rank()
  {
    static std::string s("__dpp.io.context.rank");
    return s;
  }

  // static
  io_common::format_type io_common::guess_format_from_filename(const std::string & filename_)
  {
    int mode_guess = 0;
    if (brio::store_info::guess_mode_from_filename(filename_, mode_guess)
        == brio::store_info::SUCCESS) {
      return FORMAT_BRIO;
    } else if (datatools::io_factory::guess_mode_from_filename(filename_, mode_guess)
               == datatools::io_factory::SUCCESS) {
      return FORMAT_BIO;
    }
    return FORMAT_INVALID;
  }

  // static
  i_data_source * io_common::allocate_reader(const std::string & filename_,
                                             datatools::logger::priority p_)
  {
    i_data_source * reader = 0;
    io_common::format_type format = io_common::guess_format_from_filename(filename_);
    if (format == io_common::FORMAT_INVALID) {
      DT_LOG_ERROR(p_, /* datatools::logger::PRIO_ERROR, */
                   "Cannot guess mode for input data file '"
                   << filename_ << "' !");
    } else {
      try {
        if (format == io_common::FORMAT_BRIO) {
          reader = new dpp::simple_brio_data_source(filename_, p_);
        } else {
          reader = new dpp::simple_data_source(filename_, p_);
        }
      }
      catch (std::exception & error) {
        DT_LOG_ERROR(p_, /* datatools::logger::PRIO_ERROR,*/
                     "Cannot open data source for input data file '"
                     << filename_ << "' : " << error.what());
        reader = 0;
      }
    }
    return reader;
  }

  // static
  i_data_sink * io_common::allocate_writer(const std::string & filename_,
                                           datatools::logger::priority p_)
  {
    i_data_sink * writer = 0;
    io_common::format_type format = io_common::guess_format_from_filename(filename_);
    if (format == io_common::FORMAT_INVALID) {
      DT_LOG_ERROR(p_, /* datatools::logger::PRIO_ERROR, */
                   "Cannot guess mode for output data file '"
                   << filename_ << "' !");
    } else {
      try {
        if (format == io_common::FORMAT_BRIO) {
          writer = new dpp::simple_brio_data_sink(filename_, p_);
        } else {
          writer = new dpp::simple_data_sink(filename_, p_);
        }
      }
      catch (std::exception & error) {
        DT_LOG_ERROR(p_, /* datatools::logger::PRIO_ERROR, */
                     "Cannot open data sink for output data file '"
                     << filename_ << "' : " << error.what());
        writer = 0;
      }
    }
    return writer;
  }

  void io_common::set_module_name(const std::string & modname_)
  {
    _module_name_= modname_;
    return;
  }

  const std::string & io_common::get_module_name() const
  {
    return _module_name_;
  }

  io_common::format_type io_common::get_format() const
  {
    return _format_;
  }

  void io_common::set_format(io_common::format_type f_)
  {
    _format_ = f_;
    return;
  }

  io_common::io_type io_common::get_io() const
  {
    return _io_;
  }

  void io_common::set_io(io_common::io_type f_)
  {
    _io_ = f_;
    return;
  }

  void io_common::set_context_label(const std::string & a_ctx_label)
  {
    _Ctx_label_ = a_ctx_label;
    return;
  }

  const std::string & io_common::get_context_label() const
  {
    return _Ctx_label_;
  }

  bool io_common::has_context_service() const
  {
    return _Ctx_service_ != 0;
  }

  void io_common::set_context_service(dpp::context_service & a_context_service, const std::string & a_ctx_label)
  {
    set_context_label(a_ctx_label);
    _Ctx_service_ = &a_context_service;
    return;
  }

  const dpp::context_service & io_common::get_context_service() const
  {
    DT_THROW_IF(! has_context_service(),
                std::logic_error,
                "I/O module '" << _module_name_ << "' has no context service !");
    return *_Ctx_service_;
  }

  dpp::context_service & io_common::grab_context_service()
  {
    DT_THROW_IF(! has_context_service(),
                std::logic_error,
                "I/O module '" << _module_name_ << "' has no context service !");
    return *_Ctx_service_;
  }

  void io_common::add_context_metadata(const std::string & a_label)
  {
    DT_THROW_IF(std::find(_Ctx_metadata_.begin(), _Ctx_metadata_.end(), a_label) != _Ctx_metadata_.end(),
                std::logic_error,
                "Context metadata with label '" << a_label<< "' already exists !");
    _Ctx_metadata_.push_back(a_label);
    return;
  }

  bool io_common::is_context_all() const
  {
    return _Ctx_metadata_all_;
  }

  void io_common::set_context_all(bool val_)
  {
    _Ctx_metadata_all_ = val_;
    return;
  }

  void io_common::clear_context_metadata()
  {
    _Ctx_metadata_.clear();
    return;
  }

  const std::vector<std::string> & io_common::get_context_metadata() const
  {
    return _Ctx_metadata_;
  }

  void io_common::set_max_files(int a_max_files)
  {
    DT_THROW_IF(a_max_files < 0,
                std::domain_error,
                "Invalid 'max_files' for I/O module '" << get_module_name() << "' !");
    _max_files_ = a_max_files;
    return;
  }

  int io_common::get_max_files() const
  {
    return _max_files_;
  }

  void io_common::set_max_record_total(int a_max_record_total)
  {
    DT_THROW_IF(a_max_record_total < 0,
                std::domain_error,
                "Invalid 'max_record_total' for I/O module '" << get_module_name() << "' !");
    _max_record_total_ = a_max_record_total;
    return;
  }

  int io_common::get_max_record_total() const
  {
    return _max_record_total_;
  }

  void io_common::set_max_record_per_file(int a_max_record_per_file)
  {
    DT_THROW_IF(a_max_record_per_file < 0,
                std::domain_error,
                "Invalid 'max_record_per_file' for I/O module '" << get_module_name() << "' !");
    _max_record_per_file_ = a_max_record_per_file;
    return;
  }

  int io_common::get_max_record_per_file() const
  {
    return _max_record_per_file_;
  }

  void io_common::init_filenames(const datatools::properties & a_setup)
  {
    if (_filenames_.is_valid()) {
      _filenames_.reset();
    }
    datatools::properties files_setup;
    a_setup.export_and_rename_starting_with(files_setup, "files." , "");
    _filenames_.initialize(files_setup);
    DT_THROW_IF(! _filenames_.is_valid(),
                std::logic_error,
                "Invalid list of filenames for I/O module '" << get_module_name() << "' !");
    return;
  }

  const datatools::smart_filename & io_common::get_filenames() const
  {
    return _filenames_;
  }

  datatools::smart_filename & io_common::grab_filenames()
  {
    return _filenames_;
  }

  bool io_common::is_terminated() const
  {
    return _terminated_;
  }

  void io_common::set_terminated(bool t_)
  {
    _terminated_ = t_;
    return;
  }

  int io_common::get_file_record_counter() const
  {
    return _file_record_counter_;
  }

  int io_common::get_record_counter() const
  {
    return _record_counter_;
  }

  int io_common::get_file_index() const
  {
    return _file_index_;
  }

  void io_common::set_file_record_counter(int val_)
  {
    _file_record_counter_ = val_;
    return;
  }

  void io_common::set_record_counter(int val_)
  {
    _record_counter_ = val_;
    return;
  }

  void io_common::set_file_index(int val_)
  {
    _file_index_ = val_;
    return;
  }

  void io_common::_set_defaults()
  {
    _logging_ = nullptr;
    _module_name_.clear();
    _io_ = IO_INVALID;           //!< I/O flag
    _format_ = FORMAT_INVALID;   //!< Format flag
    _max_record_per_file_ = 0;
    _max_record_total_ = 0;
    _max_files_ = -1;
    _Ctx_label_ = "";
    _Ctx_service_ = 0;
    _terminated_ = false;
    _file_record_counter_ = 0;
    _record_counter_ = 0;
    _file_index_ = -1;
    _metadata_store_.set_key_label("name");
    _metadata_store_.set_meta_label("");
    _metadata_store_.set_description("Bayeux/dpp library's I/O module metadata store");
    _Ctx_metadata_all_ = false;
    return;
  }

  // Constructor :
  io_common::io_common(datatools::logger::priority & logging_,
                       const std::string & module_name_)
  {
    _set_defaults();
    _logging_ = &logging_;
    set_module_name(module_name_);
    return;
  }

  io_common::~io_common()
  {
    reset();
    return;
  }

  // Initialization :
  void io_common::initialize(const datatools::properties & a_config,
                             datatools::service_manager & a_service_manager)
  {
    // DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "Entering...");

    // a_config.tree_dump(std::cerr, "I/O common initialize: ", "[DEVEL] ");
    /**************************************************************
     *   fetch setup parameters from the configuration container  *
     **************************************************************/
    if (! _filenames_.is_valid()) {
      init_filenames(a_config);
    }
    // DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "Filenames init...");

    if (_max_files_ < 0) {
      if (a_config.has_key("max_files")) {
        _max_files_ = a_config.fetch_integer("max_files");
        DT_THROW_IF(_max_files_ < 1,
                    std::logic_error,
                    "I/O Module '" << get_module_name() << "' : invalid max number of files !");
      }
    }

    if (_max_record_total_ == 0) {
      if (a_config.has_key("max_record_total")) {
        _max_record_total_ = a_config.fetch_integer("max_record_total");
        DT_THROW_IF(_max_record_total_ < 1,
                    std::logic_error,
                    "I/O Module '" << get_module_name() << "' : invalid max total number of data records !");
      }
    }

    if (_max_record_per_file_ == 0) {
      if (a_config.has_key("max_record_per_file")) {
        _max_record_per_file_ = a_config.fetch_integer("max_record_per_file");
        DT_THROW_IF(_max_record_per_file_ < 1,
                    std::logic_error,
                    "I/O module '" << get_module_name() << "' : invalid max number of data records per file !");
      }
    }

    if (_Ctx_service_ == nullptr) {
      if (a_config.has_key("Ctx_label")) {
        _Ctx_label_ = a_config.fetch_string("Ctx_label");
      }
      if (! _Ctx_label_.empty()) {
        if (a_service_manager.has(_Ctx_label_)
            && a_service_manager.is_a<dpp::context_service>(_Ctx_label_)) {
          dpp::context_service & Ctx
            = a_service_manager.grab<dpp::context_service>(_Ctx_label_);
          _Ctx_service_ = &Ctx;
        } else {
          DT_LOG_ERROR((*_logging_), "Service manager has no 'context' service labelled with '"
                       << _Ctx_label_ << "' for I/O module '" << get_module_name() << "' !");
        }
      }
    }

    if (_Ctx_service_ == nullptr) {
      DT_LOG_WARNING((*_logging_), "Module '" << get_module_name()
                     << "' has no access to any 'context' service !");
    }

    /*************************************
     *  end of the initialization step   *
     *************************************/
    // DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "Exiting.");
    return;
  }

  // Reset :
  void io_common::reset()
  {
    /****************************
     *  revert to some defaults *
     ****************************/

    _metadata_store_.clear();
    _filenames_.reset();
    _set_defaults();

    /****************************
     *  end of the reset step   *
     ****************************/

    return;
  }

  const datatools::multi_properties & io_common::get_metadata_store() const
  {
    return _metadata_store_;
  }

  datatools::multi_properties & io_common::grab_metadata_store()
  {
    // 2022-06-06 FM: remove this test
    // DT_THROW_IF(_io_ == IO_INPUT,
    //             std::logic_error,
    //             "Cannot obtain write access on the metadata store in 'input' mode !");
    return _metadata_store_;
  }

  void io_common::clear_metadata_store()
  {
    _metadata_store_.clear();
    return;
  }

  void io_common::tree_dump(std::ostream & a_out ,
                            const std::string & a_title,
                            const std::string & a_indent,
                            bool a_inherit) const
  {
    std::string indent;
    if (! a_indent.empty()) {
      indent = a_indent;
    }
    if (! a_title.empty()) {
      a_out << indent << a_title << std::endl;
    }

    a_out << indent << datatools::i_tree_dumpable::tag
          << "Metadata store : " << _metadata_store_.size() << std::endl;

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

    a_out << indent << datatools::i_tree_dumpable::tag
          << "Context service label : '" << _Ctx_label_ << "'" << std::endl;

    a_out << indent << datatools::i_tree_dumpable::inherit_tag(a_inherit)
          << "Context service @ " << _Ctx_service_ << std::endl;

    return;
  }

} // namespace dpp
