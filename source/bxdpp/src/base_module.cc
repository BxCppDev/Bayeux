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

#include <stdexcept>
#include <string>
#include <sstream>

#include <dpp/base_module.h>
#include <dpp/module_manager.h>

#if DPP_DATATOOLS_LEGACY == 1
#include <datatools/utils/properties.h>
#include <datatools/utils/ioutils.h>
#include <datatools/utils/utils.h>
#include <datatools/services/service_manager.h>
#else
#include <datatools/properties.h>
#include <datatools/ioutils.h>
#include <datatools/utils.h>
#include <datatools/service_manager.h>
#endif

namespace dpp {
 
  DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION (base_module, "dpp::base_module/__system__");

  bool 
  base_module::is_debug () const
  {
    return _debug_level > dpp::NO_DEBUG;
  }
    
  void 
  base_module::set_debug (bool a_debug)
  {
    if (a_debug)
      {
        _debug_level = dpp::DEBUG_LVL1;
      }
    else
      {
        _debug_level = dpp::NO_DEBUG;
      }
    return;
  }

  bool base_module::is_initialized () const
  {
    return _initialized;
  }

  void base_module::_set_initialized (bool a_initialized)
  {
    _initialized = a_initialized;
    return;
  }

  void base_module::_lock_guard (const std::string & where_, const std::string & what_)
  {
    if (!is_initialized ()) return;
    std::ostringstream message;
    if (! where_.empty ())
      {
        message << where_ << " : ";
      }
    else
      {
        message << "dpp::base_module::_lock_guard" << " : ";
      }
    if (! what_.empty ())
      {
        message << what_;
      }
    else
      {
        message << "Operation prohibited ! Module '" << get_name () 
                << "' is already initialized !";
      }
    throw std::logic_error (message.str ());
    return;
  }
      
  int
  base_module::get_debug_level () const
  {
    return _debug_level;
  }
  
  void
  base_module::set_debug_level (int a_new_value)
  {
    if (a_new_value < dpp::NO_DEBUG) 
      {
        _debug_level = dpp::NO_DEBUG;
      }
    else 
      {
        _debug_level = a_new_value;
      }
    return;
  }
   
  void
  base_module::set_name (const std::string & a_new_value)
  {
    if (is_initialized ())
      {
        std::ostringstream message;
        message << "dpp::base_module::set_name: "
                << "Module '" << _name << "' " 
                << "is already initialized ! "
                << "Cannot change the name !";
        throw std::logic_error (message.str ());
      }
    _set_name (a_new_value);
    return;
  }
 
  const std::string &
  base_module::get_name () const
  {
    return _name;
  }
  
  void
  base_module::_set_name (const std::string & a_new_value)
  {
    _name = a_new_value;
    return;
  }
 
  bool base_module::has_description () const
  {
    return ! _description.empty ();
  }
   
  const std::string & base_module::get_description () const
  {
    return _description;
  }
    
  void base_module::set_description (const std::string & a_description)
  {
    _description = a_description;
    return;
  }

  const std::string & base_module::get_version () const
  {
    return _version;
  }
   
  void base_module::set_version (const std::string & a_version)
  {
    _version = a_version;
    return;
  }

  bool base_module::has_last_error_message () const
  {
    return ! _last_error_message_.empty ();       
  }

  void base_module::set_last_error_message (const std::string & errmsg_)
  {
    append_last_error_message (errmsg_);
    return;
  }

  void base_module::append_last_error_message (const std::string & errmsg_)
  {
    std::ostringstream msg (_last_error_message_);
    if (! _last_error_message_.empty ())
      {
        msg << ": ";
      }
    msg << errmsg_; 
    _last_error_message_ = msg.str ();
    return;
  }

  void base_module::reset_last_error_message ()
  {
    _last_error_message_.clear ();
    return;
  }
      
  const std::string & base_module::get_last_error_message () const
  {
    return _last_error_message_;
  }
 
  // ctor:
  base_module::base_module (const std::string & a_process_name, 
                            const std::string & a_process_description, 
                            const std::string & a_process_version, 
                            int a_debug_level)
    : _name (a_process_name),
      _description (a_process_description),
      _version (a_process_version)
  {
    _initialized = false;
    _debug_level = a_debug_level;
    return;
  }
  
  // dtor:
  base_module::~base_module ()
  {
    if (_initialized)
      {
        std::ostringstream message;
        message << "dpp::base_module::~base_module: "
                << "Module '" << _name << "' " 
                << "still has its 'initialized' flag on ! "
                << "Possible bug !";
        throw std::logic_error (message.str ());
      }
    return;
  }

  void base_module::print (std::ostream & a_out) const
  {
    this->tree_dump (a_out, "Base module :");
    return;
  }

  void base_module::tree_dump (std::ostream & a_out , 
                               const std::string & a_title,
                               const std::string & a_indent,
                               bool a_inherit) const
  {
    std::string indent;
    if (! a_indent.empty ())
      {
        indent = a_indent;
      }
    if ( ! a_title.empty () ) 
      {
        a_out << indent << a_title << std::endl;
      }  
    a_out << indent << DPP_DU::i_tree_dumpable::tag 
          << "Module name        : '" << _name << "'" << std::endl;
    a_out << indent << DPP_DU::i_tree_dumpable::tag 
          << "Module description : '" << _description << "'" << std::endl;
    a_out << indent << DPP_DU::i_tree_dumpable::tag 
          << "Module version     : '" << _version << "'" << std::endl;
    a_out << indent << DPP_DU::i_tree_dumpable::tag 
          << "Module debug level : " << _debug_level << std::endl;
    a_out << indent << DPP_DU::i_tree_dumpable::inherit_tag (a_inherit)
          << "Module initialized : " << is_initialized () << std::endl;
        
    return;
  }
  
  void base_module::initialize_simple ()
  {
    DPP_DU::properties dummy_config;
    DPP_DS::service_manager dummy_service_manager;
    module_handle_dict_type dummy_module_dict;
    initialize (dummy_config, dummy_service_manager, dummy_module_dict);
    return;
  }
   
  void base_module::initialize_standalone (const DPP_DU::properties & a_config)
  {
    DPP_DS::service_manager dummy_service_manager;
    module_handle_dict_type dummy_module_dict;
    initialize (a_config, dummy_service_manager, dummy_module_dict);
    return;
  }
   
  void base_module::initialize_with_service (const DPP_DU::properties & a_config,
                                             DPP_DS::service_manager & a_service_manager)
  {
    module_handle_dict_type dummy_module_dict;
    initialize (a_config, a_service_manager, dummy_module_dict);
    return;
  }
   
  void base_module::initialize_without_service (const DPP_DU::properties & a_config,
                                                module_handle_dict_type & a_module_dictionnary)
  {
    DPP_DS::service_manager dummy_service_manager;
    initialize (a_config, dummy_service_manager, a_module_dictionnary);
    return;
  }

}  // end of namespace dpp

// end of base_module.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
