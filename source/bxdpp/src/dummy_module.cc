/* dummy_module.cc
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

#include <stdexcept>
#include <sstream>

#include <dpp/dummy_module.h>

#include <datatools/properties.h>
#include <datatools/ioutils.h>
#include <datatools/things.h>

namespace dpp {

  // Registration instantiation macro :
  DPP_MODULE_REGISTRATION_IMPLEMENT(dummy_module, "dpp::dummy_module");

  // The default GP label of the bank :
  const std::string & dummy_module::default_gp_label()
  {
    static std::string label = "GP";
    return label;
  }

  // The default flag name :
  const std::string & dummy_module::default_flag_name()
  {
    static std::string label = "dpp::dummy_module::flag_name";
    return label;
  }

  // Setter for the EH label of the module :
  void dummy_module::set_GP_label (const std::string & a_gp_label)
  {
    _GP_label_ = a_gp_label;
    return;
  }

  // Getter for the label of the module :
  const std::string & dummy_module::get_GP_label () const
  {
    return _GP_label_;
  }

  // Setter for the label of the module :
  void dummy_module::set_flag_name (const std::string & a_flag_name)
  {
    _flag_name_ = a_flag_name;
    return;
  }

  // Getter for the label of the module :
  const std::string & dummy_module::get_flag_name () const
  {
    return _flag_name_;
  }

  /*** Implementation of the interface ***/

  // Constructor :
  DPP_MODULE_CONSTRUCTOR_IMPLEMENT_HEAD(dummy_module,logging_priority_)
  {
    _flag_name_ = "";
    _GP_label_  = "";
    return;
  }

  // Destructor :
  DPP_MODULE_DEFAULT_DESTRUCTOR_IMPLEMENT(dummy_module)

  // Initialization :
  DPP_MODULE_INITIALIZE_IMPLEMENT_HEAD(dummy_module,
                                       a_config,
                                       /*a_service_manager*/,
                                       /*a_module_dict*/)
  {
    DT_THROW_IF(is_initialized (),
                std::logic_error,
                "Module '" << get_name () << "' is already initialized ! ");

    _common_initialize(a_config);

    if (_flag_name_.empty()) {
      // If the label is no setup, pickup from the configuration list:
      if (a_config.has_key ("flag_name")) {
        std::string flag_name = a_config.fetch_string ("flag_name");
        set_flag_name (flag_name);
      }
    }

    if (a_config.has_key("GP_label")) {
      std::string gp_label = a_config.fetch_string ("GP_label");
      set_GP_label(gp_label);
    }

    if (_GP_label_.empty()) {
      set_GP_label(default_gp_label());
    }

    // Default :
    if (_flag_name_.empty()) {
      _flag_name_ = default_flag_name();
    }

    // Flag the initialization status :
    if (! _GP_label_.empty()) {
      _set_initialized (true);
    }
    return;
  }

  // Reset :
  DPP_MODULE_RESET_IMPLEMENT_HEAD(dummy_module)
  {
    DT_THROW_IF(! is_initialized (),
                std::logic_error,
                "Module '" << get_name () << "' is not initialized !");
    _set_initialized (false);
    _flag_name_.clear();
    _GP_label_.clear();
    return;
  }

  // Processing :
  DPP_MODULE_PROCESS_IMPLEMENT_HEAD(dummy_module,the_data_record)
  {
    DT_THROW_IF(! is_initialized (),
                std::logic_error,
                "Module '" << get_name () << "' is not initialized !");

    // If no GP bank is present, add one :
    if (! the_data_record.has (_GP_label_)) {
      datatools::properties & the_gp_bank = the_data_record.add<datatools::properties> (_GP_label_);
      the_gp_bank.set_description("Bank of general properties");
    }

    // Grab the GB bank and add a flag with given name :
    if (the_data_record.has (_GP_label_)
        && the_data_record.is_a<datatools::properties> (_GP_label_)) {
      // Find the GP bank :
      datatools::properties & the_gp_bank
        = the_data_record.grab<datatools::properties> (_GP_label_);
      DT_LOG_DEBUG(_logging, "Found the '" << _GP_label_ << "' bank !");
      DT_LOG_DEBUG(_logging, "Adding the '" << _flag_name_ << "' flag property...");
      std::string key = _flag_name_;
      the_gp_bank.update_flag (key);
    } else {
      DT_LOG_CRITICAL(_logging, "Could not find any GP bank !");
      return PROCESS_ERROR; // Cannot find the event header

    }
    return PROCESS_SUCCESS;
  }

}  // end of namespace dpp

// end of dummy_module.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
