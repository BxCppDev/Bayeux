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
    return *store_;
  }

  datatools::multi_properties & context_service::grab_store()
  {
    DT_THROW_IF (! is_initialized(),
                 std::logic_error,
                 "Service '" << get_name() << "' is not initialized !");
    return *store_;
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
    return initialized_;
  }

  int context_service::initialize(const datatools::properties & a_config,
                                  datatools::service_dict_type & /*a_service_dict*/)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Service '" << get_name() << "' is already initialized !");

    this->datatools::base_service::_common_initialize(a_config);

    if (load_filename_.empty()) {
      if (a_config.has_key("load.file")) {
        load_filename_ = a_config.fetch_string("load.file");
      }
    }

    if (! load_filename_.empty()) {
      if (backup_filename_.empty()) {
        if (a_config.has_key("backup.file")) {
          backup_filename_ = a_config.fetch_string("backup.file");
        }
      }
    }

    if (store_filename_.empty()) {
      if (a_config.has_key("store.file")) {
        store_filename_ = a_config.fetch_string("store.file");
      }
    }

    store_ = new datatools::multi_properties;
    if (get_logging_priority() >= datatools::logger::PRIO_DEBUG) {
      store_->set_debug(true);
    }

    if (! load_filename_.empty()) {
      std::string dummy = load_filename_;
      datatools::fetch_path_with_env(dummy);
      if (boost::filesystem::exists(dummy)) {
        DT_LOG_NOTICE(get_logging_priority(),
                      "Service '" << get_name()
                      << "' is loading from file '"
                      << load_filename_ << "' !");
        store_->read(dummy);
        if (! backup_filename_.empty()) {
          std::string dummy2 = backup_filename_;
          datatools::fetch_path_with_env(dummy2);
          DT_LOG_NOTICE(get_logging_priority(),
                        "Service '" << get_name() << "' is backuping in file '"
                        << dummy2 << "' !");
          store_->write(dummy2);
        }
      } else {
        DT_LOG_WARNING(datatools::logger::PRIO_WARNING,
                       "Service '" << get_name()
                       << "' cannot load from file '"
                       << dummy << "' !");
      }
    }
    initialized_ = true;
    return EXIT_SUCCESS;
  }

  int context_service::reset()
  {
    DT_THROW_IF(! is_initialized(),
                std::logic_error,
                "Service '" << get_name() << "' is not initialized !");
    initialized_ = false;

    if (store_ != 0) {
      if (! store_filename_.empty()) {
        DT_LOG_NOTICE(get_logging_priority(),
                      "Service '" << get_name()
                      << "' is storing in file '"
                      << store_filename_ << "' !");
        std::string dummy = store_filename_;
        datatools::fetch_path_with_env(dummy);
        store_->write(dummy);
      }
      delete store_;
      store_ = 0;
    }

    load_filename_.clear();
    store_filename_.clear();
    backup_filename_.clear();
    return EXIT_SUCCESS;
  }

  context_service::context_service()
  {
    initialized_ = false;
    store_ = 0;
    return;
  }

  context_service::~context_service()
  {
    if (this->context_service::is_initialized()) {
      this->context_service::reset();
    }
    return;
  }

  void context_service::tree_dump(std::ostream & a_out ,
                                  const std::string & a_title,
                                  const std::string & a_indent,
                                  bool a_inherit) const
  {
    namespace du = datatools;
    this->datatools::base_service::tree_dump(a_out, a_title, a_indent, true);

    a_out << a_indent << du::i_tree_dumpable::tag
          << "Load filename   : '" << load_filename_ << "' " << std::endl;

    a_out << a_indent << du::i_tree_dumpable::tag
          << "Backup filename : '" << backup_filename_ << "' " << std::endl;

    a_out << a_indent << du::i_tree_dumpable::tag
          << "Store filename  : '" << store_filename_ << "' " << std::endl;

    if (store_ != 0) {
      a_out << a_indent << du::i_tree_dumpable::inherit_tag(a_inherit)
            << "Store : [" << store_ << "] " << std::endl;
      std::ostringstream indent_ss;
      indent_ss << a_indent << du::i_tree_dumpable::inherit_skip_tag(a_inherit);
      store_->tree_dump(a_out, "", indent_ss.str());
    } else {
      a_out << a_indent << du::i_tree_dumpable::inherit_tag(a_inherit)
            << "Store : <none>" << std::endl;
    }
    return;
  }

}  // end of namespace dpp

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
