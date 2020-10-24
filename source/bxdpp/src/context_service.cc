/* context_service.cc
 *
 * Copyright (C) 2011-2015 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
#include <sstream>
#include <stdexcept>

// Third party:
// - Boost:
#include <boost/filesystem.hpp>
// - Bayeux/datatools:
#include <datatools/multi_properties.h>
#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/exception.h>
#include <datatools/logger.h>

// This project:
#include <dpp/dpp_config.h>

// Ourselves:
#include <dpp/context_service.h>

namespace dpp {

  /// Auto-registration of this service class in a central service Db
  DATATOOLS_SERVICE_REGISTRATION_IMPLEMENT(context_service, "dpp::context_service")

  const datatools::multi_properties & context_service::get_store() const
  {
    DT_THROW_IF (! is_initialized(),
                 std::logic_error,
                 "Service '" << get_name() << "' is not initialized !");
    return *_store_;
  }

  datatools::multi_properties & context_service::grab_store()
  {
    DT_THROW_IF (! is_initialized(),
                 std::logic_error,
                 "Service '" << get_name() << "' is not initialized !");
    return *_store_;
  }

  const datatools::multi_properties & context_service::operator()() const
  {
    return get_store();
  }

  datatools::multi_properties & context_service::operator()()
  {
    return grab_store();
  }

  bool context_service::is_initialized() const
  {
    return _initialized_;
  }

  int context_service::initialize(const datatools::properties & config_,
                                  datatools::service_dict_type & /*service_dict_*/)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Service '" << get_name() << "' is already initialized !");

    this->datatools::base_service::_common_initialize(config_);

    if (_load_filename_.empty()) {
      if (config_.has_key("load.file")) {
        _load_filename_ = config_.fetch_string("load.file");
      }
    }

    if (! _load_filename_.empty()) {
      if (_backup_filename_.empty()) {
        if (config_.has_key("backup.file")) {
          _backup_filename_ = config_.fetch_string("backup.file");
        }
      }
    }

    if (_store_filename_.empty()) {
      if (config_.has_key("store.file")) {
        _store_filename_ = config_.fetch_string("store.file");
      }
    }

    _store_ = new datatools::multi_properties;

    if (! _load_filename_.empty()) {
      std::string dummy = _load_filename_;
      datatools::fetch_path_with_env(dummy);
      if (boost::filesystem::exists(dummy)) {
        DT_LOG_NOTICE(get_logging_priority(),
                      "Service '" << get_name()
                      << "' is loading from file '"
                      << _load_filename_ << "' !");
        datatools::read_config(dummy, *_store_);
        if (! _backup_filename_.empty()) {
          std::string dummy2 = _backup_filename_;
          datatools::fetch_path_with_env(dummy2);
          DT_LOG_NOTICE(get_logging_priority(),
                        "Service '" << get_name() << "' is backuping in file '"
                        << dummy2 << "' !");
          datatools::write_config(dummy2, *_store_);
        }
      } else {
        DT_LOG_WARNING(datatools::logger::PRIO_WARNING,
                       "Service '" << get_name()
                       << "' cannot load from file '"
                       << dummy << "' !");
      }
    }
    _initialized_ = true;
    return EXIT_SUCCESS;
  }

  int context_service::reset()
  {
    DT_THROW_IF(! is_initialized(),
                std::logic_error,
                "Service '" << get_name() << "' is not initialized !");
    _initialized_ = false;

    if (_store_ != 0) {
      if (! _store_filename_.empty()) {
        DT_LOG_NOTICE(get_logging_priority(),
                      "Service '" << get_name()
                      << "' is storing in file '"
                      << _store_filename_ << "' !");
        std::string dummy = _store_filename_;
        datatools::fetch_path_with_env(dummy);
        datatools::write_config(dummy, *_store_);
      }
      delete _store_;
      _store_ = 0;
    }

    _load_filename_.clear();
    _store_filename_.clear();
    _backup_filename_.clear();
    return EXIT_SUCCESS;
  }

  context_service::context_service()
  {
    _initialized_ = false;
    _store_ = 0;
    return;
  }

  context_service::~context_service()
  {
    if (this->context_service::is_initialized()) {
      this->context_service::reset();
    }
    return;
  }

  void context_service::tree_dump(std::ostream & out_ ,
                                  const std::string & title_,
                                  const std::string & indent_,
                                  bool inherit_) const
  {
    namespace du = datatools;
    this->datatools::base_service::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << du::i_tree_dumpable::tag
          << "Load filename   : '" << _load_filename_ << "' " << std::endl;

    out_ << indent_ << du::i_tree_dumpable::tag
          << "Backup filename : '" << _backup_filename_ << "' " << std::endl;

    out_ << indent_ << du::i_tree_dumpable::tag
          << "Store filename  : '" << _store_filename_ << "' " << std::endl;

    if (_store_ != 0) {
      out_ << indent_ << du::i_tree_dumpable::inherit_tag(inherit_)
            << "Store : [" << _store_ << "] " << std::endl;
      std::ostringstream indent_ss;
      indent_ss << indent_ << du::i_tree_dumpable::inherit_skip_tag(inherit_);
      _store_->tree_dump(out_, "", indent_ss.str());
    } else {
      out_ << indent_ << du::i_tree_dumpable::inherit_tag(inherit_)
            << "Store : <none>" << std::endl;
    }
    return;
  }

}  // end of namespace dpp
