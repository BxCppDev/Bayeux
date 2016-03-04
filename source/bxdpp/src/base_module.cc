/* base_module.cc
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
#include <dpp/base_module.h>

// Standard library
#include <stdexcept>
#include <string>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/ioutils.h>
#include <datatools/utils.h>
#include <datatools/service_manager.h>
#include <datatools/exception.h>

// This project:
#include <dpp/module_manager.h>

namespace dpp {

  DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(base_module, "dpp::base_module/__system__")

  bool base_module::is_initialized() const
  {
    return _initialized;
  }

  void base_module::_set_initialized(bool a_initialized)
  {
    _initialized = a_initialized;
    return;
  }

  bool base_module::has_name() const
  {
    return ! _name.empty();
  }

  void base_module::set_name(const std::string & a_new_value)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Module '" << _name << "' "
                << "is already initialized ! "
                << "Cannot change the name !");
    _set_name(a_new_value);
    return;
  }

  const std::string & base_module::get_name() const
  {
    return _name;
  }

  void base_module::_set_name(const std::string & a_new_value)
  {
    _name = a_new_value;
    return;
  }

  bool base_module::has_description() const
  {
    return ! _description.empty();
  }

  const std::string & base_module::get_description() const
  {
    return _description;
  }

  void base_module::set_description(const std::string & a_description)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Module '" << _name << "' "
                << "is already initialized ! "
                << "Cannot change the description !");
    _description = a_description;
    return;
  }

  bool base_module::has_last_error_message() const
  {
    return ! _last_error_message.empty();
  }

  void base_module::set_last_error_message(const std::string & errmsg_)
  {
    append_last_error_message(errmsg_);
    return;
  }

  void base_module::append_last_error_message(const std::string & errmsg_)
  {
    std::ostringstream msg(_last_error_message);
    if(! has_last_error_message()) {
      msg << ": ";
    }
    msg << errmsg_;
    _last_error_message = msg.str();
    return;
  }

  void base_module::reset_last_error_message()
  {
    _last_error_message.clear();
    return;
  }

  const std::string & base_module::get_last_error_message() const
  {
    return _last_error_message;
  }

  base_module::base_module(datatools::logger::priority p)
  {
    _initialized = false;
    _logging = datatools::logger::PRIO_FATAL;
    set_logging_priority(p);
    return;
  }

  base_module::~base_module()
  {
    DT_THROW_IF(_initialized,
                std::logic_error,
                "Module '" << _name << "' "
                << "still has its 'initialized' flag on ! "
                << "Possible bug !"
                );
    return;
  }

  void base_module::print(std::ostream & a_out) const
  {
    this->tree_dump(a_out, "Base module :");
    return;
  }

  void base_module::tree_dump(std::ostream & a_out ,
                              const std::string & a_title,
                              const std::string & a_indent,
                              bool a_inherit) const
  {
    std::string indent;
    if(! a_indent.empty()) {
      indent = a_indent;
    }
    if( ! a_title.empty() ) {
      a_out << indent << a_title << std::endl;
    }
    a_out << indent << datatools::i_tree_dumpable::tag
          << "Module name        : '" << _name << "'" << std::endl;
    a_out << indent << datatools::i_tree_dumpable::tag
          << "Module description : '" << _description << "'" << std::endl;
    a_out << indent << datatools::i_tree_dumpable::tag
          << "Module logging threshold : '"
          << datatools::logger::get_priority_label(_logging) << "'" << std::endl;
    a_out << indent << datatools::i_tree_dumpable::tag
          << "Module initialized : " << is_initialized() << std::endl;
    a_out << indent << datatools::i_tree_dumpable::inherit_tag(a_inherit)
          << "Last error message : '" << get_last_error_message() << "'" << std::endl;
    return;
  }

  void base_module::set_logging_priority(datatools::logger::priority a_priority)
  {
    _logging = a_priority;
    return;
  }

  datatools::logger::priority base_module::get_logging_priority() const
  {
    return _logging;
  }

  void base_module::_common_initialize(const datatools::properties & a_config)
  {
    DT_LOG_DEBUG(_logging, "Entering...");
    datatools::logger::priority p =
      datatools::logger::extract_logging_configuration(a_config,
                                                       datatools::logger::PRIO_UNDEFINED,
                                                       true);
    if(p != datatools::logger::PRIO_UNDEFINED) {
      set_logging_priority(p);
    }

    if(! has_name()) {
      if(a_config.has_key("name")) {
        set_name(a_config.fetch_string("name"));
      }
    }

    if(! has_description()) {
      if(a_config.has_key("description")) {
        set_description(a_config.fetch_string("description"));
      }
    }

    DT_LOG_DEBUG(_logging, "Exiting.");
    return;
  }

  void base_module::common_ocd(datatools::object_configuration_description & ocd_)
  {
    datatools::logger::declare_ocd_logging_configuration(ocd_, "fatal", "");

    {
      datatools::configuration_property_description & cpd = ocd_.add_property_info();
      cpd.set_name_pattern("description")
        .set_from("dpp::base_module")
        .set_terse_description("The description of the module")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_long_description("A description of the module.")
        .add_example("Example::                                                \n"
                     "                                                         \n"
                     "  description : string = \"Calibration of the raw data\" \n"
                     "                                                         \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd = ocd_.add_property_info();
      cpd.set_name_pattern("name")
        .set_from("dpp::base_module")
        .set_terse_description("The name of the module")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_long_description("A name given to the module.")
        .add_example("Example::                                                \n"
                     "                                                         \n"
                     "  name : string = \"Calib0\"                             \n"
                     "                                                         \n"
                     )
        ;
    }
    return;
  }

  void base_module::initialize_simple()
  {
    DT_LOG_DEBUG(_logging, "Entering...");
    datatools::properties dummy_config;
    datatools::service_manager dummy_service_manager;
    module_handle_dict_type dummy_module_dict;
    initialize(dummy_config, dummy_service_manager, dummy_module_dict);
    DT_LOG_DEBUG(_logging, "Exiting.");
    return;
  }

  void base_module::initialize_standalone(const datatools::properties & a_config)
  {
    DT_LOG_DEBUG(_logging, "Entering...");
    datatools::service_manager dummy_service_manager;
    module_handle_dict_type dummy_module_dict;
    initialize(a_config, dummy_service_manager, dummy_module_dict);
    DT_LOG_DEBUG(_logging, "Exiting.");
    return;
  }

  void base_module::initialize_with_service(const datatools::properties & a_config,
                                            datatools::service_manager & a_service_manager)
  {
    DT_LOG_DEBUG(_logging, "Entering...");
    module_handle_dict_type dummy_module_dict;
    initialize(a_config, a_service_manager, dummy_module_dict);
    DT_LOG_DEBUG(_logging, "Exiting.");
    return;
  }

  void base_module::initialize_without_service(const datatools::properties & a_config,
                                               module_handle_dict_type & a_module_dictionnary)
  {
    DT_LOG_DEBUG(_logging, "Entering...");
    datatools::service_manager dummy_service_manager;
    initialize(a_config, dummy_service_manager, a_module_dictionnary);
    DT_LOG_DEBUG(_logging, "Exiting.");
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
