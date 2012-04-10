/* cut_factory.cc
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

#include <cuts/cut_factory.h>

#include <stdexcept>
#include <sstream>

namespace cuts {

  using namespace std;
      
  bool cut_factory::g_devel = false;
 
  bool cut_factory::is_locked () const
  {
    return _locked_;
  }
  
  void cut_factory::set_locked (bool a_locked)
  {
    _locked_ = a_locked;
    return;
  }

  bool cut_factory::is_debug () const
  {
    return _debug_;
  }
  
  void cut_factory::set_debug (bool a_debug)
  {
    _debug_ = a_debug;
    return;
  }

  bool cut_factory::is_verbose () const
  {
    return _verbose_;
  }
  
  void cut_factory::set_verbose (bool a_verbose)
  {
    _verbose_ = a_verbose;
    return;
  }

  // ctor:
  cut_factory::cut_factory (unsigned int a_flag)
    : _factory_register_ ("cuts::i_cut/cut_factory", 
                          a_flag & verbose ? i_cut::factory_register_type::verbose : 0)
  {
    _debug_ = a_flag & debug;
    _verbose_ = a_flag & verbose;
    _locked_ = false;
    bool preload = true;
    if (a_flag & no_preload)
      {
        preload = false;
      }
    if (is_debug ())
      {
        clog << "DEBUG: cuts::cut_factory::ctor: Initial cuts' registered ID are : " << endl;
        tree_dump (clog, "", "DEBUG: ");
      }
    if (preload)
      {
        if (_verbose_) 
          {
            clog << "NOTICE: cuts::cut_factory::ctor: " 
                 << "Preloading the global cuts factory dictionary..." << endl;
          }
        _factory_register_.import (DATATOOLS_FACTORY_GET_SYSTEM_REGISTER (cuts::i_cut));
        tree_dump (clog, "Factory after preload:", "NOTICE: ");
      }
    return;
  }
  
  cut_handle_type 
  cut_factory::create_cut (const string & a_cut_id, 
                           const datatools::utils::properties & a_cut_configuration,
                           datatools::service::service_manager & a_service_manager,
                           cut_handle_dict_type & a_cut_dict)
  {
    using namespace std;
    bool devel = g_devel;
    if (devel) 
      {
        cerr << "DEVEL: cut_factory::create_cut: " 
             << "Cut ID == '" << a_cut_id << "' " << endl;
      }

    cut_handle_type new_cut_handle (0);
    // search for the cut's label in the factory dictionary:
    if (! _factory_register_.has (a_cut_id))
      {
        std::cerr << "No registered class with ID '" 
                  << a_cut_id << "' !" << std::endl;
        
      }
    else
      {
        cuts::i_cut::factory_register_type::factory_type & the_factory 
          = _factory_register_.get (a_cut_id);
        i_cut * new_cut_ptr = the_factory ();
        new_cut_ptr->initialize (a_cut_configuration,
                                 a_service_manager, 
                                 a_cut_dict);
        new_cut_handle.reset (new_cut_ptr);
      }

    return (new_cut_handle);
  }
  
  // dtor:
  cut_factory::~cut_factory ()
  {
    //_creators.clear ();
    return;
  }

  void cut_factory::tree_dump (ostream & a_out, 
                               const string & a_title,
                               const string & a_indent,
                               bool a_inherit) const
  {
    string indent;
    if (! a_indent.empty ())
      {
        indent = a_indent;
      }
    if ( ! a_title.empty () ) 
      {
        a_out << indent << a_title << endl;
      }  
       
    namespace du = datatools::utils;

    a_out << indent << du::i_tree_dumpable::tag 
          << "Debug         : " << is_debug () << endl;

    a_out << indent << du::i_tree_dumpable::tag 
          << "Verbose       : " << is_verbose () << endl;

    a_out << indent << du::i_tree_dumpable::tag 
          << "List of registered cuts : " << endl;
    {
      std::ostringstream indent_oss;
      indent_oss << indent << du::i_tree_dumpable::skip_tag; 
      _factory_register_.print (a_out, indent_oss.str ());
    }

    a_out << indent << du::i_tree_dumpable::inherit_tag (a_inherit) 
          << "Locked        : " << is_locked () << endl;

    return;
  }

}  // end of namespace cuts

// end of cut_factory.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
