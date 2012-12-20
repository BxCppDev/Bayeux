/* service_tools.cc
 * 
 * Copyright (C) 2011-2012 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
#include <datatools/service_tools.h>

// Standard Library
#include <stdexcept>
#include <sstream>

// Third Party

// Datatools
#include <datatools/base_service.h>

namespace datatools {

dependency_info_type::dependency_info_type() {
  level = DEPENDENCY_STRICT;
}


service_entry::service_entry() {
  service_status = 0;
  service_handle.reset();
}

  /*
  const datatools::properties & get_service_config () const;

  void set_service_config (const datatools::properties &);

  const std::string & get_service_id () const;

  void set_service_id (const std::string &);

  const std::string & get_service_name () const;

  */
  void set_service_name (const std::string &);

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
      << "Can be dropped   : " 
      << this->can_be_dropped() << std::endl;

  out << indent << i_tree_dumpable::inherit_tag (a_inherit) 
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
}

}  // end of namespace datatools

