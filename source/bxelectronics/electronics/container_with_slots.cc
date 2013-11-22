// container_with_slots.cc
//
// Copyright (c) 2013 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of snelectronics.
//
// snelectronics is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// snelectronics is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with snelectronics. If not, see <http://www.gnu.org/licenses/>.


// Ourselves
#include "snelectronics/container_with_slots.h"

namespace snemo {
  namespace electronics {

    container_with_slots::container_with_slots()
    {
      _initialized_ = false;
      _locked_ = false;

      return;
    }

    container_with_slots::~container_with_slots()
    {
      if (is_initialized()) {
        reset();
      }
      return;
    }

    bool container_with_slots::is_initialized() const
    {
      return _initialized_;
    }

    bool container_with_slots::is_locked() const
    {
      return _locked_;
    }

    void container_with_slots::lock()
    {
      _locked_ = true;
      return;
    }

    void container_with_slots::unlock()
    {
      _locked_ = false;
      return;
    }

    bool container_with_slots::has_component_label() const
    {
      return ! _component_label_.empty();
    }

    const std::string & container_with_slots::get_component_label() const
    {
      return _component_label_;
    }

    void container_with_slots::set_component_label(const std::string & label_)
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "Container of components is already initialized !");
      _component_label_ = label_;
      return;
    }

    uint32_t container_with_slots::get_number_of_slots() const
    {
      return _number_of_slots_;
    }

    void container_with_slots::set_number_of_slots(uint32_t nslots_)
    {
      DT_THROW_IF(is_initialized(),
                  std::logic_error,
                  "Container of components is already initialized !");
      _number_of_slots_ = nslots_;
      return;
    }

    bool container_with_slots::has_component(uint32_t slot_) const
    {
      return _components_.find(d_slot_) != _components_.end();
    }

    void container_with_slots::remove_component(uint32_t slot_)
    {
      DT_THROW_IF(is_locked(),
                  std::logic_error,
                  "Container of components is locked !");
      DT_THROW_IF(!has_component(slot_),
                  std::logic_error,
                  "Container of components does not have a component at slot '"
                  << slot_ << "' !");
      _components_.erase(_components_.find(slot_));
      {
        std::ostringstream mez_label;
        mez_label << _component_label_ << slot_;
        this->remove_daughter(mez_label.str());
      }
      return;
    }

    void container_with_slots::tree_dump(std::ostream& out_,
                                         const std::string& title_,
                                         const std::string& indent_,
                                         bool inherit_) const
    {

      return;
    }

  } // end of namespace electronics
} // end of namespace snemo

// end of container_with_slots.cc
