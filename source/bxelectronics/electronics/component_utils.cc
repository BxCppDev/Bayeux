/* component_utils.cc
 *
 * Copyright (C) 2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
// Ourselves
#include <electronics/component_utils.h>

// Standard Library
#include <stdexcept>
#include <string>
#include <sstream>

// Third Party
// - Bayeux/datatools
#include <datatools/exception.h>

// This project
#include <electronics/component_base.h>
#include <electronics/component_manager.h>


namespace electronics {

  std::string make_indexed_name(const std::string & prefix_,
                                uint32_t index_)
  {
    std::ostringstream oss;
    oss << prefix_ << index_;
    return oss.str();
  }

  uint32_t component_entry::get_component_status() const
  {
    return _component_status_;
  }

  void component_entry::update_component_status(uint32_t bits)
  {
    _component_status_ |= bits;
    return;
  }

  void component_entry::reset_component_status(uint32_t bits)
  {
    _component_status_ ^= bits;
    return;
  }

  const datatools::properties & component_entry::get_component_config () const
  {
    return _component_config_;
  }

  datatools::properties & component_entry::grab_component_config ()
  {
    DT_THROW_IF (_component_status_ != STATUS_BLANK,
                 std::logic_error,
                 "Cannot modify the configuration of component named '" << get_component_name () << "' !");
    return _component_config_;
  }

  void component_entry::set_component_config (const datatools::properties & sc_)
  {
    DT_THROW_IF (_component_status_ != STATUS_BLANK,
                 std::logic_error,
                 "Cannot modify the configuration of component named '" << get_component_name () << "' !");
    _component_config_ = sc_;
    return;
  }

  const std::string & component_entry::get_component_id () const
  {
    return _component_id_;
  }

  void component_entry::set_component_id (const std::string & sid_)
  {
    DT_THROW_IF (sid_.empty(), std::logic_error,"Empty component ID is not allowed !");
    _component_id_ = sid_;
  }

  const std::string & component_entry::get_component_name () const
  {
    return _component_name_;
  }

  void component_entry::set_component_name (const std::string & sn_)
  {
    DT_THROW_IF (sn_.empty(),std::logic_error, "Empty component name is not allowed !");
    _component_name_ = sn_;
    return;
  }

  const component_handle_type & component_entry::get_component_handle() const
  {
    return _component_handle_;
  }

  component_handle_type & component_entry::grab_component_handle()
  {
    return _component_handle_;
  }

  // Return a mutable handle to the initialized component
  component_handle_type & component_entry::grab_initialized_component_handle ()
  {
    if (! _component_handle_) {
      DT_THROW_IF(_component_manager_ == 0,std::logic_error,
                  "No component manager is available to create the component '"
                  << this->_component_name_ << "' ! ");
      _component_manager_->create_component(*this);
    }
    _component_manager_->initialize_component(*this);
    return _component_handle_;
  }

  // Check if there is the component manager
  bool component_entry::has_component_manager() const
  {
    return _component_manager_ != 0;
  }

  // Install a handle to the component manager
  void component_entry::set_component_manager(component_manager & cmgr_)
  {
    _component_manager_ = &cmgr_;
    return;
  }

  // Reset the handle to the component manager
  void component_entry::reset_component_manager()
  {
    _component_manager_ = 0;
    return;
  }

  component_entry::component_entry() {
    _component_status_ = STATUS_BLANK;
    _component_manager_ = 0;
    return;
  }

  component_entry::~component_entry() {
    _component_handle_.reset();
    _component_status_ = STATUS_BLANK;
  }

  bool component_entry::is_created() const
  {
    return _component_status_ & STATUS_CREATED;
  }

  bool component_entry::is_initialized() const
  {
    return _component_status_ & STATUS_INITIALIZED;
  }

  void component_entry::tree_dump(std::ostream& out_,
                                  const std::string& title_,
                                  const std::string& indent_,
                                  bool inherit_) const {
    std::string indent;
    if (!indent_.empty()) indent = indent_;

    if (!title_.empty()) out_ << indent << title_ << std::endl;

    out_ << indent << i_tree_dumpable::tag
         << "Component name     : '"
         << _component_name_
         << "'" << std::endl;

    out_ << indent << i_tree_dumpable::tag
         << "Component ID       : '"
         << _component_id_
         << "'" << std::endl;

    out_ << indent << i_tree_dumpable::inherit_tag(inherit_)
         << "Component status   : "
         << _component_status_;
    {
      size_t count = 0;
      std::ostringstream status_info;
      if (_component_status_ & component_entry::STATUS_CREATED) {
        if (count) status_info << ',';
        status_info << "created";
        count++;
      }
      if (_component_status_ & component_entry::STATUS_INITIALIZED) {
        if (count) status_info << ',';
        status_info << "initialized";
        count++;
      }
      if (count) {
        out_ << ' ' << '('
             << status_info.str()
             << ')';
      }
    }
    out_ << std::endl;
    return;
  }

} // end of namespace electronics
