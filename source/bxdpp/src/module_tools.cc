/* module_tools.cc 
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
#include <iostream>
#include <sstream>

#include <dpp/module_tools.h>
#include <dpp/base_module.h>
#include <dpp/module_manager.h>

namespace dpp {

  const datatools::properties & module_entry_type::get_module_config () const
  {
    return _module_config_;
  }

  datatools::properties & module_entry_type::grab_module_config ()
  {
    return _module_config_;
  }

  void module_entry_type::set_module_config (const datatools::properties & config_)
  {
    _module_config_ = config_;
    return;
  }

  const std::string & module_entry_type::get_module_name () const
  {
    return _module_name_;
  }

  const std::string & module_entry_type::get_module_id () const
  {
    return _module_id_;
  }

  void module_entry_type::set_ptr (base_module * ptr_)
  {
    _module_handle_.reset (ptr_);
    return;
  }

  void module_entry_type::set_module_name (const std::string & n_)
  {
    _module_name_ = n_;
    return;
  }

  void module_entry_type::set_module_id (const std::string & id_)
  {
    _module_id_ = id_;
    return;
  }
 
  void module_entry_type::set_blank ()
  {
    _module_status_ = 0;
    return;
  }
      
  void module_entry_type::set_created ()
  {
    _module_status_ |= module_entry_type::STATUS_CREATED;
    return;
  }
       
  void module_entry_type::set_initialized ()
  {
    _module_status_ |= module_entry_type::STATUS_INITIALIZED;
    return;
  }
      
  void module_entry_type::set_uninitialized ()
  {
    _module_status_ ^= module_entry_type::STATUS_INITIALIZED;
    return;
  }
 
  module_entry_type::module_entry_type ()
  { 
    _manager_ = 0;
    _module_status_ = 0;
    return;
  }

  module_entry_type::~module_entry_type ()
  {
    _module_status_ = 0;
    _module_handle_.reset (0);
    _manager_ = 0;
    return;
  }
        
  bool module_entry_type::has_manager () const
  {
    return _manager_ != 0;
  }
      
  void module_entry_type::set_manager (module_manager & mgr_)
  {
    _manager_ = &mgr_;
    return;
  }
      
  bool module_entry_type::is_created () const
  {
    return _module_status_ & STATUS_CREATED;
  }

  bool module_entry_type::is_initialized () const
  {
    return _module_status_ & STATUS_INITIALIZED;
  }
       
  bool module_entry_type::has_module () const
  {
    return _module_handle_.has_data ();
  }

  const base_module & module_entry_type::get_module () const
  {
    return _module_handle_.get ();
  }

  base_module & module_entry_type::grab_module ()
  {
    return _module_handle_.grab ();
  }

  const module_handle_type & module_entry_type::get_module_handle () const
  {
    return _module_handle_;
  }
 
  module_handle_type & module_entry_type::grab_module_handle ()
  {
    return _module_handle_;
  }
     
  module_handle_type & module_entry_type::grab_initialized_module_handle ()
  {
    if (! _module_handle_.has_data ())
      {
        if (_manager_ == 0)
          {
            std::ostringstream message;
            message << "dpp::module_entry_type::grab_initialized_module_handle: "
                    << "No manager is available to create the module '" << this->_module_name_
                    << "' ! ";
            throw std::logic_error (message.str ());
                
          }
        _manager_->create_module (*this);
      }
    if (_module_handle_.has_data ())
      {
        if (_manager_ == 0)
          {
            std::ostringstream message;
            message << "dpp::module_entry_type::grab_initialized_module_handle: "
                    << "No manager is available to initialize the module '" << this->_module_name_
                    << "' ! ";
            throw std::logic_error (message.str ());
                
          }
        _manager_->initialize_module (*this);
      }
    return _module_handle_;
  }

  void module_entry_type::tree_dump (std::ostream & out_ , 
                                     const std::string & title_,
                                     const std::string & indent_,
                                     bool inherit_) const
  {
    namespace du = datatools;
    std::string indent;
    if (! indent_.empty ())
      {
        indent = indent_;
      }
    if ( ! title_.empty () ) 
      {
        out_ << indent << title_ << std::endl;
      }  

    out_ << indent << du::i_tree_dumpable::tag 
         << "Module name     : '" << _module_name_ << "'" << std::endl;

    out_ << indent << du::i_tree_dumpable::tag 
         << "Module ID       : '" << _module_id_ << "'" << std::endl;

    out_ << indent << du::i_tree_dumpable::tag 
         << "Module status   : " << _module_status_;
    {
      size_t count = 0;
      std::ostringstream status_info;
      if (is_created ())
        {
          if (count) status_info << ',';
          status_info << "created";
          count++;
        }
      if (is_initialized ())
        {
          if (count) status_info << ',';
          status_info << "initialized";
          count++;
        }
      if (count)
        {
          out_ << ' ' << '(' << status_info.str () << ')';
        }
    }
    out_ << std::endl;

    out_ << indent << du::i_tree_dumpable::tag 
         << "Module handle   : ";
    if (_module_handle_.has_data ())
      {
        const base_module & bm = _module_handle_.get ();
        out_ << "'" << bm.get_name () << "'";
      }
    else
      {
        out_ << "<null>";
      }
    out_ << std::endl;
        
    if (_module_handle_.has_data ())
      {
        out_ << indent << du::i_tree_dumpable::tag 
             << "Module description : ";
        const base_module & bm = _module_handle_.get ();
        if (bm.has_description ())
          {
            out_ << "'" << bm.get_description () << "'";
          }
        else
          {
            out_ << "<none>";
          }
        out_ << std::endl;
      }
       
    out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_) 
         << "Manager   : " << has_manager () << std::endl;
       
    return;
  }

}  // end of namespace dpp

// end of module_tools.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
