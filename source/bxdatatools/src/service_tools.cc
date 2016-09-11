/* service_tools.cc
 *
 * Copyright (C) 2011-2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
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

// Ourselves:
#include <datatools/service_tools.h>

// Standard Library:
#include <stdexcept>
#include <sstream>

// This project:
#include <datatools/base_service.h>
#include <datatools/exception.h>
#include <datatools/service_manager.h>

namespace datatools {

  dependency_info_type::dependency_info_type() {
    level = DEPENDENCY_STRICT;
  }

  void dependency_info_type::reset() {
    id.clear();
    version.clear();
    meta.clear();
    level = DEPENDENCY_STRICT;
  }

  uint32_t service_entry::get_service_status() const
  {
    return service_status;
  }

  void service_entry::update_service_status(uint32_t bits)
  {
    service_status |= bits;
    if (has_service_manager()) {
      if (bits & STATUS_CREATED) {
        manager->at_service_creation(service_name, service_id);
      }
      if (bits & STATUS_INITIALIZED) {
        manager->at_service_initialization(service_name);
      }
    }
    return;
  }

  void service_entry::reset_service_status(uint32_t bits)
  {
    service_status ^= bits;
    if (has_service_manager()) {
      if (bits & STATUS_INITIALIZED) {
        manager->at_service_reset(service_name);
      }
      if (bits & STATUS_CREATED) {
        manager->at_service_drop(service_name);
      }
    }
    return;
  }

  const datatools::properties & service_entry::get_service_config() const
  {
    return service_config;
  }

  datatools::properties & service_entry::grab_service_config()
  {
    DT_THROW_IF(is_initialized(),
                 std::logic_error,
                 "Cannot modify the configuration of service named '" << get_service_name() << "' !");
    return service_config;
  }

  void service_entry::set_service_config(const datatools::properties & sc_)
  {
    DT_THROW_IF(is_initialized(),
                 std::logic_error,
                 "Cannot modify the configuration of the initialized service named '" << get_service_name() << "' !");
    service_config = sc_;
    return;
  }

  const std::string & service_entry::get_service_id() const
  {
    return service_id;
  }

  void service_entry::set_service_id(const std::string & sid_)
  {
    DT_THROW_IF(sid_.empty(), std::logic_error,"Empty service ID is not allowed !");
    service_id = sid_;
  }

  const std::string & service_entry::get_service_name() const
  {
    return service_name;
  }

  void service_entry::set_service_name(const std::string & sn_)
  {
    DT_THROW_IF(sn_.empty(),std::logic_error, "Empty service name is not allowed !");
    service_name = sn_;
    return;
  }

  const service_handle_type & service_entry::get_service_handle() const
  {
    return service_handle;
  }

  service_handle_type & service_entry::grab_service_handle()
  {
    return service_handle;
  }

  bool service_entry::has_service_manager() const
  {
    return manager != nullptr;
  }

  void service_entry::set_service_manager(service_manager & smgr)
  {
    manager = &smgr;
  }

  const service_manager & service_entry::get_service_manager() const
  {
    DT_THROW_IF(!has_service_manager(), std::logic_error, "Missing service manager!");
    return *manager;
  }

  service_manager & service_entry::grab_service_manager()
  {
    DT_THROW_IF(!has_service_manager(), std::logic_error, "Missing service manager!");
    return *manager;
  }

  service_entry::service_entry() {
    service_status = STATUS_BLANK;
    manager = nullptr;
  }

  service_entry::service_entry(const std::string & name_, service_manager & mgr_) {
    service_status = STATUS_BLANK;
    manager = &mgr_;
    set_service_name(name_);
  }

  service_entry::~service_entry() {
    // if (manager != nullptr) {
    //   manager->destroy_service(*this);
    // }
  }

  bool service_entry::has_master(const std::string& name) const {
    service_dependency_dict_type::const_iterator found = service_masters.find(name);
    return (found != service_masters.end());
  }

  bool service_entry::has_slave(const std::string& name) const {
    dependency_level_dict_type::const_iterator found = service_slaves.find(name);
    return (found != service_slaves.end()) && (found->second == DEPENDENCY_STRICT);
  }

  void service_entry::remove_slave(const std::string& name) {
    dependency_level_dict_type::iterator found = service_slaves.find(name);
    if (found != service_slaves.end()) {
      service_slaves.erase(found);
    }
  }

  bool service_entry::can_be_dropped() const {
    for (dependency_level_dict_type::const_iterator i = service_slaves.begin();
         i != service_slaves.end();
         ++i) {
      if (i->second == DEPENDENCY_STRICT) {
        return false;
      }
    }
    return true;
  }

  bool service_entry::is_created() const
  {
    return service_status & STATUS_CREATED;
  }

  bool service_entry::is_initialized() const
  {
    if (!is_created()) {
      return false;
    }
    if (service_status & STATUS_INITIALIZED) {
      if (!service_handle.get().is_initialized()) {
        // update the status:
        service_entry * mutable_this = const_cast<service_entry*>(this);
        mutable_this->reset_service_status(service_entry::STATUS_INITIALIZED);
        return false;
      }
      return true;
    }
    if (is_created()) {
      if (service_handle.get().is_initialized()) {
        // update the status:
        service_entry * mutable_this = const_cast<service_entry*>(this);
        mutable_this->update_service_status(service_entry::STATUS_INITIALIZED);
        return true;
      }
    }
    return false;
  }

  void service_entry::tree_dump(std::ostream& out,
                                const std::string& title,
                                const std::string& a_indent,
                                bool a_inherit) const {
    std::string indent;
    if (!a_indent.empty()) indent = a_indent;

    if (!title.empty()) out << indent << title << std::endl;

    out << indent << i_tree_dumpable::tag
        << "Service name     : '"
        << service_name
        << "'" << std::endl;

    out << indent << i_tree_dumpable::tag
        << "Service ID       : '"
        << service_id
        << "'" << std::endl;

    out << indent << i_tree_dumpable::tag
        << "Service configuration : ";
    if (service_config.size()) {
      out << '[' << service_config.size() << " propertie(s)]";
    } else {
      out << "<none>";
    }
    out << std::endl;

    out << indent << i_tree_dumpable::tag
        << "Service manager  : ";
    if (has_service_manager()) {
      out << "[@" << manager << "]";
    } else {
      out << "<none>";
    }
    out << "'" << std::endl;

    {
      out << indent << i_tree_dumpable::tag
          << "Master services  : ";
      if (service_masters.size() == 0) {
        out << "<none>";
      }
      out << std::endl;
      for (service_dependency_dict_type::const_iterator i =
             service_masters.begin();
           i != service_masters.end();
           ++i) {
        const std::string& master_service_name = i->first;
        const dependency_info_type& di = i->second;
        out << indent << i_tree_dumpable::skip_tag;

        std::ostringstream indent_oss;
        indent_oss << indent << i_tree_dumpable::skip_tag;
        service_dependency_dict_type::const_iterator j = i;
        j++;
        if (j == service_masters.end()) {
          out << i_tree_dumpable::last_tag;
          indent_oss << i_tree_dumpable::last_skip_tag;
        } else {
          out << i_tree_dumpable::tag;
          indent_oss << i_tree_dumpable::skip_tag;
        }
        out << "Master '" << master_service_name << "' ";
        out << ": "
            << ""
            << "id='"      << di.id << "' "
            << "version='" << di.version << "' "
            << "meta='"    << di.meta << "' "
            << "level="    << di.level << ""
            << std::endl;
      }
    }
    {
      out << indent << i_tree_dumpable::tag
          << "Slave services   : ";
      if (service_slaves.size() == 0) {
        out << "<none>";
      }
      out << std::endl;
      for (dependency_level_dict_type::const_iterator i =
             service_slaves.begin();
           i != service_slaves.end();
           ++i) {
        const std::string& slave_service_name = i->first;
        int slave_level = i->second;
        out << indent << i_tree_dumpable::skip_tag;

        std::ostringstream indent_oss;
        indent_oss << indent << i_tree_dumpable::skip_tag;
        dependency_level_dict_type::const_iterator j = i;
        j++;
        if (j == service_slaves.end()) {
          out << i_tree_dumpable::last_tag;
          indent_oss << i_tree_dumpable::last_skip_tag;
        } else  {
          out << i_tree_dumpable::tag;
          indent_oss << i_tree_dumpable::skip_tag;
        }
        out << "Slave '" << slave_service_name << "' ";
        out << ": "
            << ""
            << "level="    << slave_level << ""
            << std::endl;
      }
    }

    out << indent << i_tree_dumpable::tag
        << "Service status   : "
        << service_status;
    {
      size_t count = 0;
      std::ostringstream status_info;
      if (service_status & service_entry::STATUS_CREATED) {
        if (count) status_info << ',';
        status_info << "created";
        count++;
      }
      if (service_status & service_entry::STATUS_INITIALIZED) {
        if (count) status_info << ',';
        status_info << "initialized";
        count++;
      }
      if (count) {
        out << ' ' << '('
            << status_info.str()
            << ')';
      }
    }
    out << std::endl;

    out << indent << i_tree_dumpable::inherit_tag(a_inherit)
        << "Can be dropped   : "
        << this->can_be_dropped() << std::endl;
 }

  bool find_service_name_with_id(const service_dict_type & services_,
                                 const std::string & service_id_,
                                 std::string & service_name_)
  {
    service_name_.clear();
    for (::datatools::service_dict_type::const_iterator iserv = services_.begin();
         iserv != services_.end();
         iserv++) {
      const std::string & service_name = iserv->first;
      const ::datatools::service_entry & sentry = iserv->second;
      if (sentry.get_service_id() == service_id_) {
        service_name_ = service_name;
        break;
      }
    }
    return !service_name_.empty();
  }

  bool find_service_names_with_id(const service_dict_type & services_,
                                  const std::string & service_id_,
                                  std::vector<std::string> & service_names_)
  {
    std::size_t sz = service_names_.size();
    for (::datatools::service_dict_type::const_iterator iserv = services_.begin();
         iserv != services_.end();
         iserv++) {
      const std::string & service_name = iserv->first;
      const ::datatools::service_entry & sentry = iserv->second;
      if (sentry.get_service_id() == service_id_) {
        service_names_.push_back(service_name);
      }
    }
    return service_names_.size() > sz;
  }

  base_service & grab_service(service_dict_type & services_,
                              const std::string & service_name_)
  {
    ::datatools::service_dict_type::iterator found = services_.find(service_name_);
    DT_THROW_IF(found == services_.end(), std::logic_error,
                "Cannot find service named '" << service_name_ << "'!");
    ::datatools::service_entry & sentry = found->second;
    service_manager & smgr = sentry.grab_service_manager();
    return smgr.grab_service(service_name_);
  }

  const base_service & get_service(const service_dict_type & services_,
                                   const std::string & service_name_)
  {
    service_dict_type & mutable_services = const_cast<service_dict_type &>(services_);
    return const_cast<base_service&>(grab_service(mutable_services,service_name_));
  }

}  // end of namespace datatools
