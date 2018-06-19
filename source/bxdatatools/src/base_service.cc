// datatools/base_service.cc
/*
 * Copyright (C) 2011-2017 Francois Mauger <mauger@lpccaen.in2p3.fr>
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

// This project:
#include <datatools/service_tools.h>
#include <datatools/properties.h>
#include "datatools/enriched_base.h"
#include "datatools/factory_macros.h"
#include "datatools/i_tree_dump.h"
#include "datatools/logger.h"

namespace datatools {

  DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(base_service,
                                                   "datatools::base_service/__system__")

  base_service::base_service()
  {
    return;
  }

  base_service::base_service(const std::string & name_,
                             const std::string & display_name_,
                             const std::string & terse_description_,
                             logger::priority lp_)
    : enriched_base(name_, display_name_, terse_description_, lp_)
  {
    return;
  }

  base_service::~base_service()
  {
    return;
  }

  void base_service::fetch_dependencies(service_dependency_dict_type & dependencies_) const
  {
    dependencies_.clear();
    return;
  }

  int base_service::initialize_simple()
  {
    datatools::properties dummy;
    return this->initialize_standalone(dummy);
  }

  int base_service::initialize_with_services(service_dict_type& service_dict_)
  {
    datatools::properties dummy;
    return this->initialize(dummy, service_dict_);
  }

  int base_service::initialize_standalone(const datatools::properties & config_)
  {
    service_dict_type dummy;
    return this->initialize(config_, dummy);
  }

  void base_service::common_initialize(const datatools::properties & config_)
  {
    _common_initialize(config_);
    return;
  }

  void base_service::_common_initialize(const datatools::properties & config_)
  {
    this->enriched_base::initialize(config_, false);
    return;
  }
  
  void base_service::print_tree(std::ostream & out_,
                                const boost::property_tree::ptree & options_) const
  {
    i_tree_dumpable::base_print_options popts;
    popts.configure_from(options_);

    this->enriched_base::print_tree(out_,
                                    base_print_options::force_inheritance(options_));
                                    
    out_ << popts.indent << i_tree_dumpable::inherit_tag(popts.inherit)
          << "Service is initialized  : "
          << std::boolalpha << this->is_initialized() << std::endl;
   
    return;
  }

  void base_service::tree_dump(std::ostream & out_,
                               const std::string & title_,
                               const std::string & indent_,
                               bool inherit_) const
  {
    this->enriched_base::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
          << "Service is initialized  : "
          << std::boolalpha << this->is_initialized() << std::endl;

    return;
  }

  void base_service::common_ocd(datatools::object_configuration_description & ocd_)
  {
    enriched_base::init_ocd(ocd_);
    return;
  }

}  // end of namespace datatools
