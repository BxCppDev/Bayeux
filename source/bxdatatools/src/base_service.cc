/* base_service.cc
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
#include <datatools/base_service.h>

// Standard Library
#include <stdexcept>
#include <sstream>

// Third Party

// Datatools
#include <datatools/service_tools.h>
#include <datatools/properties.h>
#include <datatools/ioutils.h>

namespace datatools {

  //bool base_service::g_debug = false;

DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(
    base_service,
    "datatools::base_service/__system__");


// ctor:
base_service::base_service(const std::string& name,
                           const std::string& description,
                           const std::string& version)
    : name_(name),
      description_(description),
      version_(version) {}

// Destructor :
base_service::~base_service() {}


const std::string& base_service::get_name() const {
  return name_;
}


void base_service::set_name(const std::string& name) {
  name_ = name;
}


bool base_service::has_description() const {
  return !description_.empty();
}


const std::string& base_service::get_description() const {
  return description_;
}


void base_service::set_description(const std::string& description) {
  description_ = description;
}


bool base_service::has_version() const {
  return !version_.empty();
}


const std::string& base_service::get_version() const {
  return version_;
}


void base_service::set_version(const std::string& version) {
  version_ = version;
}


void base_service::fetch_dependencies(
    service_dependency_dict_type& dependencies) const {
  dependencies.clear();
}


int base_service::initialize_standalone(
    const datatools::properties& config) {
  service_dict_type dummy;
  return this->initialize(config, dummy);
}


void base_service::tree_dump(std::ostream& out,
                             const std::string& title,
                             const std::string& a_indent,
                             bool a_inherit) const {
  std::string indent;
  if (!a_indent.empty()) indent = a_indent;

  if (!title.empty()) out << indent << title << std::endl;

  out << indent << i_tree_dumpable::tag
      << "Service name        : '"
      << name_ << "'" << std::endl;

  out << indent << i_tree_dumpable::tag
      << "Service description : '"
      << description_ << "'" << std::endl;

  out << indent << i_tree_dumpable::tag
      << "Service version     : '"
      << version_ << "'" << std::endl;

  out << indent << i_tree_dumpable::inherit_tag (a_inherit)
      << "Service initialized : "
      << this->is_initialized() << std::endl;
}

}  // end of namespace datatools

