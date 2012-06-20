/* base_service.cc
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
#include <datatools/services/base_service.h>

// Standard Library
#include <stdexcept>
#include <sstream>

// Third Party

// Datatools
#include <datatools/services/service_tools.h>
#include <datatools/utils/properties.h>
#include <datatools/utils/ioutils.h>

namespace datatools {
namespace service {


bool base_service::g_debug = false;

DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(
    base_service,
    "datatools::service::base_service/__system__");


const std::string& base_service::get_name() const {
  return _name;
}


void base_service::_set_name(const std::string& a_new_value) {
  _name = a_new_value;
}


bool base_service::has_description() const {
  return !_description.empty();
}


const std::string& base_service::get_description() const {
  return _description;
}


void base_service::set_description(const std::string& a_description) {
  _description = a_description;
}


bool base_service::has_version () const {
  return !_version.empty();
}


const std::string& base_service::get_version() const {
  return _version;
}


void base_service::set_version(const std::string& a_version) {
  _version = a_version;
}


void base_service::fetch_dependencies(
    service_dependency_dict_type& a_dependency_list) const {
  a_dependency_list.clear();
}

// ctor:
base_service::base_service(const std::string& a_service_name,
                           const std::string& a_service_description,
                           const std::string& a_service_version)
    : _name(a_service_name),
      _description(a_service_description),
      _version(a_service_version) {}

// Destructor :
base_service::~base_service() {}


int base_service::initialize_standalone(
    const datatools::utils::properties & a_config) {
  service_dict_type dummy;
  return this->initialize(a_config, dummy);
}


void base_service::tree_dump(std::ostream& a_out,
                             const std::string & a_title,
                             const std::string & a_indent,
                             bool a_inherit) const {
  namespace du = datatools::utils;
  std::string indent;
  if (!a_indent.empty()) indent = a_indent;

  if (!a_title.empty()) a_out << indent << a_title << std::endl;

  a_out << indent << du::i_tree_dumpable::tag
        << "Service name        : '" 
        << _name << "'" << std::endl;

  a_out << indent << du::i_tree_dumpable::tag
        << "Service description : '" 
        << _description << "'" << std::endl;

  a_out << indent << du::i_tree_dumpable::tag
        << "Service version     : '" 
        << _version << "'" << std::endl;

  a_out << indent << du::i_tree_dumpable::inherit_tag (a_inherit)
        << "Service initialized : " 
        << this->is_initialized() << std::endl;
}

}  // end of namespace service
}  // end of namespace datatools

// end of base_service.cc
/*
 ** Local Variables: --
 ** mode: c++ --
 ** c-file-style: "gnu" --
 ** tab-width: 2 --
 ** End: --
 */
