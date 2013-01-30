/* cut_tools.cc 
 * 
 * Copyright (C) 2012 Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#include <cuts/cut_tools.h>
#include <cuts/i_cut.h>
#include <cuts/cut_manager.h>

namespace cuts {

  const datatools::properties & cut_entry_type::get_cut_config () const
  {
    return _cut_config_;
  }

  datatools::properties & cut_entry_type::grab_cut_config ()
  {
    return _cut_config_;
  }

  void cut_entry_type::set_cut_config (const datatools::properties & config_)
  {
    _cut_config_ = config_;
    return;
  }

  const std::string & cut_entry_type::get_cut_name () const
  {
    return _cut_name_;
  }

  const std::string & cut_entry_type::get_cut_id () const
  {
    return _cut_id_;
  }

  void cut_entry_type::set_ptr (i_cut * ptr_)
  {
    _cut_handle_.reset (ptr_);
    return;
  }

  void cut_entry_type::set_cut_name (const std::string & n_)
  {
    _cut_name_ = n_;
    return;
  }

  void cut_entry_type::set_cut_id (const std::string & id_)
  {
    _cut_id_ = id_;
    return;
  }
 
  void cut_entry_type::set_blank ()
  {
    _cut_status_ = 0;
    return;
  }
      
  void cut_entry_type::set_created ()
  {
    _cut_status_ |= cut_entry_type::STATUS_CREATED;
    return;
  }
       
  void cut_entry_type::set_initialized ()
  {
    _cut_status_ |= cut_entry_type::STATUS_INITIALIZED;
    return;
  }
      
  void cut_entry_type::set_uninitialized ()
  {
    _cut_status_ ^= cut_entry_type::STATUS_INITIALIZED;
    return;
  }
 
  cut_entry_type::cut_entry_type ()
  { 
    _manager_ = 0;
    _cut_status_ = 0;
    return;
  }
        
  bool cut_entry_type::has_manager () const
  {
    return _manager_ != 0;
  }
      
  void cut_entry_type::set_manager (cut_manager & mgr_)
  {
    _manager_ = &mgr_;
    return;
  }
      
  bool cut_entry_type::is_created () const
  {
    return _cut_status_ & STATUS_CREATED;
  }

  bool cut_entry_type::is_initialized () const
  {
    return _cut_status_ & STATUS_INITIALIZED;
  }
       
  bool cut_entry_type::has_cut () const
  {
    return _cut_handle_.has_data ();
  }

  const i_cut & cut_entry_type::get_cut () const
  {
    return _cut_handle_.get ();
  }

  i_cut & cut_entry_type::grab_cut ()
  {
    return _cut_handle_.grab ();
  }

  const cut_handle_type & cut_entry_type::get_cut_handle () const
  {
    return _cut_handle_;
  }
 
  cut_handle_type & cut_entry_type::grab_cut_handle ()
  {
    return _cut_handle_;
  }
     
  cut_handle_type & cut_entry_type::grab_initialized_cut_handle ()
  {
    if (! _cut_handle_.has_data ())
      {
        if (_manager_ == 0)
          {
            std::ostringstream message;
            message << "snemo::core::processing::cut_entry_type::grab_initialized_cut_handle: "
                    << "No manager is available to create the cut '" << this->_cut_name_
                    << "' ! ";
            throw std::logic_error (message.str ());
                
          }
        _manager_->create_cut (*this);
      }
    if (_cut_handle_.has_data ())
      {
        if (_manager_ == 0)
          {
            std::ostringstream message;
            message << "snemo::core::processing::cut_entry_type::grab_initialized_cut_handle: "
                    << "No manager is available to initialize the cut '" << this->_cut_name_
                    << "' ! ";
            throw std::logic_error (message.str ());
                
          }
        _manager_->initialize_cut (*this);
      }
    return _cut_handle_;
  }

  void cut_entry_type::tree_dump (std::ostream & out_ , 
                                     const std::string & title_,
                                     const std::string & indent_,
                                     bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty ())
      {
        indent = indent_;
      }
    if ( ! title_.empty () ) 
      {
        out_ << indent << title_ << std::endl;
      }  

    out_ << indent << datatools::i_tree_dumpable::tag 
         << "Cut name     : '" << _cut_name_ << "'" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag 
         << "Cut ID       : '" << _cut_id_ << "'" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag 
         << "Cut status   : " << _cut_status_;
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

    out_ << indent << datatools::i_tree_dumpable::tag 
         << "Cut handle   : ";
    if (_cut_handle_.has_data ())
      {
        const i_cut & bm = _cut_handle_.get ();
        out_ << "'" << bm.get_name () << "'";
      }
    else
      {
        out_ << "<null>";
      }
    out_ << std::endl;
        
    if (_cut_handle_.has_data ())
      {
        out_ << indent << datatools::i_tree_dumpable::tag 
             << "Cut description : ";
        const i_cut & bm = _cut_handle_.get ();
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
       
    out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_) 
         << "Manager   : " << has_manager () << std::endl;
       
    return;
  }

}  // end of namespace cuts

// end of cuts_tools.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
