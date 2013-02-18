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

#if DPP_DATATOOLS_LEGACY == 1
#include <datatools/utils/properties.h>
#include <datatools/utils/ioutils.h>
#include <datatools/utils/things.h>
#else
#include <datatools/properties.h>
#include <datatools/ioutils.h>
#include <datatools/things.h>
#endif

namespace dpp {
 
  // Registration instantiation macro :
  DPP_MODULE_REGISTRATION_IMPLEMENT(dummy_module, "dpp::dummy_module");

  // The string used to flag the not-initialized label of the module :
  const std::string dummy_module::UNINITIALIZED_LABEL = "";

  // The default GP label of the bank :
  const std::string dummy_module::DEFAULT_GP_LABEL = "GP";

  // The default flag name :
  const std::string dummy_module::DEFAULT_FLAG_NAME = "dpp::dummy_module::flag_name";

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
  dummy_module::dummy_module (int a_debug_level)        
    : base_module ("dpp::dummy_module",         
                   "An data record processor dummy module",        
                   "0.1",         
                   a_debug_level) 
  {
    _flag_name_    = UNINITIALIZED_LABEL;
    _GP_label_ = "GP"; // default value
    return;
  }
      
  // Destructor :
  dummy_module::~dummy_module ()
  {         
    // Make sure all internal resources are terminated 
    // before destruction : 
    if (is_initialized ()) reset ();
    return; 
  }

  // Initialization :
  void dummy_module::initialize (const DPP_DU::properties & a_config,
                                 DPP_DS::service_manager & a_service_manager,
                                 module_handle_dict_type & a_module_dict)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "dummy_module::initialize: "
                << "Module '" << get_name () << "' is already initialized ! ";
        throw std::logic_error (message.str ());
      }

    // Note: is_debug and set_debug are inherited from the base class.
    if (! is_debug ())
      {
        if (a_config.has_flag ("debug"))
          {
            set_debug (true); 
          }
      }
  
    if (_flag_name_ == UNINITIALIZED_LABEL)
      {
        // If the label is no setup, pickup from the configuration list:
        if (a_config.has_key ("flag_name"))
          {
            std::string flag_name = a_config.fetch_string ("flag_name");
            set_flag_name (flag_name);
          }
      }

    if (a_config.has_key ("GP_label"))
      {
        std::string gp_label = a_config.fetch_string ("GP_label");
        set_GP_label (gp_label);
      }
 
    // Default :
    if (_flag_name_ == UNINITIALIZED_LABEL)
      {
        _flag_name_ = DEFAULT_FLAG_NAME;
      }

    // Flag the initialization status :
    if (_GP_label_ != UNINITIALIZED_LABEL)
      {
        _set_initialized (true);
      }
    return;
  }
  
  // Reset :
  void dummy_module::reset ()
  {
    if (! is_initialized ())
      {
        std::ostringstream message;
        message << "dpp::dummy_module::reset: "
                << "Module '" << get_name () << "' is not initialized !";
        throw std::logic_error (message.str ());
      }
    _set_initialized (false);

    _flag_name_ = "";
    _GP_label_ = DEFAULT_GP_LABEL;

    return;
  }

  // Processing :
  int dummy_module::process (DPP_DU::things & the_data_record) 
  {
    if (! is_initialized ())
      {
        std::ostringstream message;
        message << "dpp::dummy_module::process: "
                << "Module '" << get_name () << "' is not initialized !";
        throw std::logic_error (message.str ());
      }

    if (the_data_record.has (_GP_label_) 
        && the_data_record.is_a<DPP_DU::properties> (_GP_label_))
      {
        // Find the GP bank :
        DPP_DU::properties & the_gp_bank 
          = the_data_record.grab<DPP_DU::properties> (_GP_label_);
        if (is_debug ())
          {
            std::clog << DPP_DU::io::debug
                 << "dpp::dummy_module::process: "
                 << "Found the '" << _GP_label_ << "' bank !"
                 << std::endl;
          }
        // Add a string property in it with value '_label_' :
        if (is_debug ())
          {
            std::clog << DPP_DU::io::debug
                 << "dpp::dummy_module::process: "
                 << "Adding the '" << _flag_name_ << "' flag property..."
                 << std::endl;
          }
        std::string key = _flag_name_;
        int count = 0;
        the_gp_bank.update_flag (key);
      }
    else
      {
        std::clog << DPP_DU::io::error
             << "dpp::dummy_module::process: "
             << "Could not find any GP bank !"
             << std::endl;
        // Cannot find the event header :
        return ERROR;
      }
    return SUCCESS;
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
