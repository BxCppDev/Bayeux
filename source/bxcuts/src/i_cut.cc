/* i_cut.cc
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

#include <stdexcept>
#include <sstream>

#include <cuts/i_cut.h>

#include <datatools/utils/properties.h>
#include <datatools/utils/ioutils.h>
#include <datatools/services/service_manager.h>

namespace cuts {

  using namespace std;

  //DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION (i_cut, "cuts::i_cut/__system__");

  bool 
  i_cut::is_debug () const
  {
    return _debug_level_ > 0;
  }
    
  void 
  i_cut::set_debug (bool a_debug)
  {
    if (a_debug)
      {
        _debug_level_ = 1;
      }
    else
      {
        _debug_level_ = 0;
      }
    return;
  }

  bool i_cut::is_initialized () const
  {
    return _initialized_;
  }

  void i_cut::_set_initialized (bool a_initialized)
  {
    _initialized_ = a_initialized;
    return;
  }

  int
  i_cut::get_debug_level () const
  {
    return _debug_level_;
  }
  
  void
  i_cut::set_debug_level (int a_new_value)
  {
    if (a_new_value < 0) 
      {
        _debug_level_ = 0;
      }
    else 
      {
        _debug_level_ = a_new_value;
      }
    return;
  }
  
  const string &
  i_cut::get_name () const
  {
    return _name;
  }
  
  void
  i_cut::_set_name (const string & a_new_value)
  {
    _name = a_new_value;
    return;
  }
    
  const string & i_cut::get_description () const
  {
    return _description;
  }
    
  void i_cut::set_description (const string & a_description)
  {
    _description = a_description;
    return;
  }

  const string & i_cut::get_version () const
  {
    return _version;
  }
   
  void i_cut::set_version (const string & a_version)
  {
    _version = a_version;
    return;
  }

  // ctor:
  i_cut::i_cut (const string & a_cut_name, 
                const string & a_cut_description, 
                const string & a_cut_version, 
                int a_debug_level)
    : _name (a_cut_name),
      _description (a_cut_description),
      _version (a_cut_version)
  {
    _initialized_ = false;
    _debug_level_ = a_debug_level;
    _user_data_ = 0;
    return;
  }
  
  // dtor:
  i_cut::~i_cut ()
  {
    _user_data_ = 0;
    if (_initialized_)
      {
        ostringstream message;
        message << "cuts::i_cut::~i_cut: "
                << "Cut '" << _name << "' " 
                << "still has its 'initialized' flag on ! "
                << "Possible bug !";
        throw logic_error (message.str ());
      }
    return;
  }

  void i_cut::print (ostream & a_out) const
  {
    this->tree_dump (a_out, "Base cut :");
    return;
  }

  void i_cut::tree_dump (ostream & a_out , 
                         const string & a_title,
                         const string & a_indent,
                         bool a_inherit) const
  {
    namespace du = datatools::utils;
    string indent;
    if (! a_indent.empty ())
      {
        indent = a_indent;
      }
    if ( ! a_title.empty () ) 
      {
        a_out << indent << a_title << endl;
      }  
    a_out << indent << du::i_tree_dumpable::tag 
          << "Cut name        : '" << _name << "'" << endl;
    a_out << indent << du::i_tree_dumpable::tag 
          << "Cut description : '" << _description << "'" << endl;
    a_out << indent << du::i_tree_dumpable::tag 
          << "Cut version     : '" << _version << "'" << endl;
    a_out << indent << du::i_tree_dumpable::tag 
          << "Cut debug level : " << _debug_level_ << endl;
    a_out << indent << du::i_tree_dumpable::inherit_tag (a_inherit)
          << "Cut initialized : " << is_initialized () << endl;      
    return;
  }
   
  void i_cut::initialize_standalone (const datatools::utils::properties & a_config)
  {
    datatools::service::service_manager dummy;
    cut_handle_dict_type dummy2;
    initialize (a_config, dummy, dummy2);
    return;
  }
   
  void i_cut::initialize_with_service (const datatools::utils::properties & a_config,
                                       datatools::service::service_manager & a_service_manager)
  {
    cut_handle_dict_type dummy;
    initialize (a_config, a_service_manager, dummy);
    return;
  }
  
  bool i_cut::has_user_data () const
  {
    return _user_data_ != 0;
  }

  void * i_cut::_get_user_data () const
  {
    return _user_data_;
  }

  void i_cut::set_user_data (void * a_user_data)
  {
    _user_data_ = a_user_data;
    return;
  }

  void i_cut::unset_user_data ()
  {
    _user_data_ = 0;
    return;
  }

  void i_cut::_prepare_cut ()
  {
    return;
  }

  void i_cut::_finish_cut ()
  {
    return;
  }

  int i_cut::process ()
  {
    _prepare_cut ();
    int status = _accept ();
    _finish_cut ();
    return status;
  }

  int i_cut::operator() ()
  {
    return this->process ();
  }

  void i_cut::reset () 
  {
    _user_data_ = 0;
    return;
  }

}  // end of namespace cuts

// end of i_cut.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
