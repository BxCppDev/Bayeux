// datatools/base_service.cc
/*
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

// Ourselves:
#include <datatools/base_service.h>

// Standard Library:
#include <stdexcept>
#include <sstream>

// This project:
#include <datatools/service_tools.h>
#include <datatools/properties.h>
#include <datatools/ioutils.h>
#include <datatools/exception.h>
#include <datatools/object_configuration_description.h>

namespace datatools {

DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(
    base_service,
    "datatools::base_service/__system__");

// Constructor
base_service::base_service()
{
  set_logging_priority(datatools::logger::PRIO_FATAL);
}

// Constructor
base_service::base_service(const std::string& name,
                           const std::string& display_name,
                           const std::string& terse_description,
                           logger::priority lp)
  : enriched_base(name, display_name, terse_description, lp)
{
}

// Destructor :
base_service::~base_service() {}

void base_service::fetch_dependencies(
    service_dependency_dict_type& dependencies) const {
  dependencies.clear();
}

int base_service::initialize_simple() {
  datatools::properties dummy;
  return this->initialize_standalone(dummy);
}

int base_service::initialize_standalone(
    const datatools::properties& config) {
  service_dict_type dummy;
  return this->initialize(config, dummy);
}

void base_service::common_initialize(const datatools::properties& config) {

  this->enriched_base::initialize(config, false);

}

void base_service::common_ocd(datatools::object_configuration_description& ocd)
{
  enriched_base::init_ocd(ocd);
}

void base_service::tree_dump(std::ostream& a_out,
                             const std::string& a_title,
                             const std::string& a_indent,
                             bool a_inherit) const {

  this->enriched_base::tree_dump(a_out, a_title, a_indent, true);

  a_out << a_indent << i_tree_dumpable::inherit_tag(a_inherit)
      << "Service is initialized  : "
      << this->is_initialized() << std::endl;
}

}  // end of namespace datatools
