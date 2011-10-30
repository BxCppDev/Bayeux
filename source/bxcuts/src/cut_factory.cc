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

#include <stdexcept>
#include <sstream>

#include <cuts/cut_factory.h>
#include <cuts/i_cut.h>

namespace cuts {
      
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

  void cut_factory::do_register (const cut_creator_type & a_cut_creator,
                                 const string & a_cut_id)
  {
    if (is_locked ())
      {
        ostringstream message;
        message << "cuts::cut_factory::do_register: " 
                << "Cut factory is locked !";
        throw logic_error (message.str ());
      }
    bool abort_on_error = true;
    string cut_id = a_cut_id;
    if (a_cut_id.empty ())
      {
        ostringstream message;
        message << "cuts::cut_factory::do_register: " 
                << "Missing cut ID !";
        if (abort_on_error)
          {
            throw logic_error (message.str ());
          }
        else 
          {
            message << ' ' << "Missing cut ID, nothing to do...";
            clog << "WARNING: cuts::cut_factory::do_register: " 
                 << message.str () << endl;
          }
      }
    cut_creator_dict_type::const_iterator found = _creators.find (a_cut_id);
    if (found != _creators.end ())
      {
        ostringstream message;
        message << "cuts::cut_factory::do_register: " 
                << "Cut ID '" << a_cut_id << "' is already used "
                << "within the cut_factory dictionary!";
        if (abort_on_error)
          {
            throw logic_error (message.str ());
          }
        else 
          {
            message << ' ' << "Overwriting current value...";
            clog << "WARNING: cuts::cut_factory::do_register: " 
                 << message.str () << endl;
          }
      }
    _creators[cut_id] = a_cut_creator;
    return;
  }

  // ctor:
  cut_factory::cut_factory (unsigned int a_flag)
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
        clog << "NOTICE: cuts::cut_factory::ctor: " 
             << "Preloading the global cuts dictionary..." << endl;
        // preload local cut creator dictionary with the 
        // global cut creator dictionary :
        _creators = cut_tools::get_cut_creator_db ().get_dict ();
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

    // search for the cut's label in the creators dictionary:
    cut_creator_dict_type::iterator found = _creators.find (a_cut_id);
    if (found != _creators.end ())
      {
        cut_creator_type a_creator = found->second;
        i_cut * new_cut_ptr = a_creator (a_cut_configuration,
                                         a_service_manager,
                                         a_cut_dict);
        cut_handle_type new_cut_handle (new_cut_ptr);
        return new_cut_handle;
      }
    else
      {
        if (devel) 
          {
            cerr << "DEVEL: cut_factory::create_cut: "
                 << "Null cut !" << endl;
          }
      }

    cut_handle_type empty_cut_handle;
    return (empty_cut_handle);
  }
  
  // dtor:
  cut_factory::~cut_factory ()
  {
    _creators.clear ();
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
          << "Creators      : ";
    size_t sz = _creators.size ();
    a_out << sz << " element" << (sz < 2? "": "s") << endl;
    ostringstream indent_ss;
    int count = 0;
    for (cut_creator_dict_type::const_iterator it = _creators.begin ();
         it != _creators.end ();
         it++) 
      {
        count++;
        a_out << indent;
        a_out << du::i_tree_dumpable::skip_tag;
        if (count == sz) 
          {
            a_out << du::i_tree_dumpable::last_tag; 
          }
        else 
          {
            a_out << du::i_tree_dumpable::tag;
          }
        a_out << "Cut ID='" << it->first 
              << "' : address=" << hex << (void *) it->second 
              << dec << endl;
      }
        
    a_out << indent << du::i_tree_dumpable::inherit_tag (a_inherit) 
          << "Locked        : " << is_locked () << endl;

    return;
  }

  const cut_creator_dict_type & cut_factory::get_creators () const
  {
    return _creators;
  }

  cut_creator_dict_type & cut_factory::get_creators ()
  {
    return _creators;
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
