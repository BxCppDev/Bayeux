/* service_tools.cc
 * 
 * Copyright (C) 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
// Ourselves
#include <datatools/services/service_tools.h>

// Standard Library
#include <stdexcept>
#include <sstream>

// Third Party

// Datatools
#include <datatools/services/base_service.h>

namespace datatools {
namespace service {

dependency_info_type::dependency_info_type() {
  level = STRICT_DEPENDENCY;
}


service_entry::service_entry() {
  service_status = 0;
  service_handle.reset();
}


bool service_entry::has_slave(const std::string& a_service_name) const {
  dependency_level_dict_type::const_iterator found = service_slaves.find(a_service_name);
  return (found != service_slaves.end()) && (found->second == STRICT_DEPENDENCY);
}


void service_entry::remove_slave (const std::string& a_service_name) {
  dependency_level_dict_type::iterator found = service_slaves.find(a_service_name);
  if (found != service_slaves.end()) {
    service_slaves.erase(found);
  }
}


bool service_entry::can_be_dropped() const {
  for (dependency_level_dict_type::const_iterator i = service_slaves.begin();
       i != service_slaves.end();
       ++i) {
    if (i->second == STRICT_DEPENDENCY) {
      return false;
    }
  }      
  return true;
}


void service_entry::tree_dump(std::ostream& a_out, 
                              const std::string& a_title,
                              const std::string& a_indent,
                              bool a_inherit) const {
  namespace du = datatools::utils;
  std::string indent;
  if (!a_indent.empty()) indent = a_indent;

  if (!a_title.empty()) a_out << indent << a_title << std::endl;

  a_out << indent << du::i_tree_dumpable::tag 
        << "Service name     : '" 
        << service_name 
        << "'" << std::endl;

  a_out << indent << du::i_tree_dumpable::tag 
        << "Service ID       : '" 
        << service_id 
        << "'" << std::endl;

  {
    a_out << indent << du::i_tree_dumpable::tag 
          << "Master services  : ";
    if (service_masters.size() == 0) {
      a_out << "<none>";
    }
    a_out << std::endl;
    for (service_dependency_dict_type::const_iterator i = 
         service_masters.begin();
         i != service_masters.end();
         ++i) {
      const std::string& master_service_name = i->first;
      const dependency_info_type& di = i->second;
      a_out << indent << du::i_tree_dumpable::skip_tag;

      std::ostringstream indent_oss;
      indent_oss << indent << du::i_tree_dumpable::skip_tag;
      service_dependency_dict_type::const_iterator j = i; 
      j++;
      if (j == service_masters.end()) {
        a_out << du::i_tree_dumpable::last_tag;
        indent_oss << du::i_tree_dumpable::last_skip_tag;
      } else {
        a_out << du::i_tree_dumpable::tag;
        indent_oss << du::i_tree_dumpable::skip_tag;
      }
      a_out << "Master '" << master_service_name << "' ";
      a_out << ": " 
            << ""
            << "id='"      << di.id << "' " 
            << "version='" << di.version << "' "
            << "meta='"    << di.meta << "' "
            << "level="    << di.level << ""
            << std::endl;
    }
  } 
  {
    a_out << indent << du::i_tree_dumpable::tag 
          << "Slave services   : ";
    if (service_slaves.size() == 0) {
      a_out << "<none>";
    }
    a_out << std::endl; 
    for (dependency_level_dict_type::const_iterator i = 
         service_slaves.begin();
         i != service_slaves.end();
         ++i) {
      const std::string& slave_service_name = i->first;
      int slave_level = i->second;
      a_out << indent << du::i_tree_dumpable::skip_tag;

      std::ostringstream indent_oss;
      indent_oss << indent << du::i_tree_dumpable::skip_tag;
      dependency_level_dict_type::const_iterator j = i; 
      j++;
      if (j == service_slaves.end()) {
        a_out << du::i_tree_dumpable::last_tag;
        indent_oss << du::i_tree_dumpable::last_skip_tag;
      } else  {
        a_out << du::i_tree_dumpable::tag;
        indent_oss << du::i_tree_dumpable::skip_tag;
      }
      a_out << "Slave '" << slave_service_name << "' ";
      a_out << ": " 
            << ""
            << "level="    << slave_level << ""
            << std::endl;
    }       
  }
  a_out << indent << du::i_tree_dumpable::tag 
        << "Can be dropped   : " 
        << this->can_be_dropped() << std::endl;

  a_out << indent << du::i_tree_dumpable::inherit_tag (a_inherit) 
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
      a_out << ' ' << '(' 
            << status_info.str() 
            << ')';
    }
  }
  a_out << std::endl;
}

}  // end of namespace service
}  // end of namespace datatools

