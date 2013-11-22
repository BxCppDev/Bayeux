// rack_base.cc
//
//
// Copyright (c) 2013 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of electronics.
//
// electronics is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// electronics is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with electronics.  If not, see <http://www.gnu.org/licenses/>.

// Ourselves
#include "electronics/rack_base.h"

// Third Party

// This project
#include <electronics/crate_base.h>
#include <electronics/component_types.h>

namespace electronics {

  ELECTRONICS_COMPONENT_REGISTRATION_IMPLEMENT(rack_base, "electronics::rack_base");

  rack_base::rack_base()
  {
    set_type(TYPE_RACK);
    _max_number_of_crates_ = 0;
    return;
  }

  rack_base::~rack_base()
  {
    return;
  }

  void rack_base::set_max_number_of_crates(uint32_t nos_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Rack '" << get_name() << "' is already initialized !");
    _max_number_of_crates_ = nos_;
    return;
  }

  uint32_t rack_base::get_max_number_of_crates()
  {
    return _max_number_of_crates_;
  }

  bool rack_base::has_format() const
  {
    return ! _format_.empty();
  }

  const std::string & rack_base::get_format() const
  {
    return _format_;
  }

  void rack_base::set_format(const std::string & format_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Rack '" << get_name() << "' is already initialized !");
    _format_ = format_;
    return;
  }

  bool rack_base::has_crate(uint32_t slot_id_) const
  {
    return _crates_.find(slot_id_) != _crates_.end();
  }

  void rack_base::remove_crate(uint32_t slot_id_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Rack '" << get_name() << "' is locked !");
    DT_THROW_IF(!has_crate(slot_id_),
                std::logic_error,
                "Rack '" << get_name() << "' does not have a crate at slot '" << slot_id_ << "' !");
    _crates_.erase(_crates_.find(slot_id_));
    {
      std::ostringstream mod_label;
      mod_label << "crate_" << slot_id_;
      this->remove_embedded(mod_label.str());
    }
    return;
  }

  void rack_base::add_crate(uint32_t slot_id_,
                            component_handle_type & crate_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Rack '" << get_name() << "' is locked !");
    indexed_component_dict_type::iterator found = _crates_.find(slot_id_);
    DT_THROW_IF(_max_number_of_crates_ == 0,
                std::logic_error,
                "Rack '" << get_name() << "' does not accept crates !");
    DT_THROW_IF(has_crate(slot_id_),
                std::logic_error,
                "Rack '" << get_name() << "' already has a crate at slot '" << slot_id_ << "' !");
    DT_THROW_IF(! crate_.has_data(),
                std::logic_error,
                "Rack '" << get_name() << "' cannot embed a NULL crate at slot '" << slot_id_ << "' !");
    const crate_base * crate = dynamic_cast<const crate_base *>(&crate_.get());
    DT_THROW_IF(crate == 0,
                std::logic_error,
                "Rack '" << get_name() << "' : Attempt to embed a non crate component '"
                <<  crate_.get().get_name()
                << "' at slot '" << slot_id_ << "' !");

    if (has_format() && crate->has_format()) {
      DT_THROW_IF(_format_ != crate->get_format(),
                  std::logic_error,
                  "Rack '" << get_name() << "' :  Attempt to embed a crate with incompatible format ('"
                  << crate->get_format()
                  << "') at slot '" << slot_id_ << "' !");
    }
    // Finally, insert the crate:
    {
      std::ostringstream mod_label;
      mod_label << "crate_" << slot_id_;
      // ... as a embedded component:
      this->add_embedded(mod_label.str(), crate_);
    }
    // ... as an explicit crate at its slot:
    _crates_[slot_id_] = crate_;
    return;
  }

  component_handle_type & rack_base::grab_crate(uint32_t slot_id_)
  {
    indexed_component_dict_type::iterator found = _crates_.find(slot_id_);
    DT_THROW_IF(found == _crates_.end(),
                std::logic_error,
                "Rack '" << get_name() << "' has no embedded crate at slot '" << slot_id_ << "' !");
    return found->second;
  }

  const component_handle_type & rack_base::get_crate(uint32_t slot_id_) const
  {
    indexed_component_dict_type::const_iterator found = _crates_.find(slot_id_);
    DT_THROW_IF(found == _crates_.end(),
                std::logic_error,
                "Rack '" << get_name() << "' has no embedded crate at slot '"
                << slot_id_ << "' !");
    return found->second;
  }


  void rack_base::_rack_reset()
  {
    _crates_.clear();
    _max_number_of_crates_ = 0;
    _format_.clear();
    this->_component_reset();
    return;
  }

  void rack_base::_rack_initialize(const datatools::properties & config_,
                                   component_pool_type& components_)
  {
    this->_component_initialize(config_, components_);

    if (! has_format()) {
      if (config_.has_key("rack.format")) {
        std::string cf = config_.fetch_string("rack.format");
        set_format(cf);
      }
    }

    if (_max_number_of_crates_ == 0) {
      if (config_.has_key("rack.max_number_of_crates")) {
        int32_t n = config_.fetch_integer("rack.max_number_of_crates");
        DT_THROW_IF ((n < 1 || n > 64), std::domain_error,
                     "Invalid number of crates '" << n << "' !");
        uint32_t mnos = n;
        set_max_number_of_crates(mnos);
      }
    }

    if (_max_number_of_crates_ > 0) {
      for (int islot = 0; islot < _max_number_of_crates_; islot++) {
        std::ostringstream mb_slot_key;
        mb_slot_key << "rack.crates.slot_" << islot;
        if (config_.has_key(mb_slot_key.str())) {
          std::string crate_name = config_.fetch_string(mb_slot_key.str());
          component_handle_type & crate_handle
            = components_.find(crate_name)->second.grab_component_handle();
          add_crate(islot, crate_handle);
        }
      }
    }

    return;
  }

  void rack_base::initialize(const datatools::properties& config_,
                             component_pool_type& components_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Rack '" << get_name() << "' is already initialized !");

    this->_rack_initialize(config_, components_);

    _set_initialized(true);
    return;
  }

  void rack_base::reset()
  {
    DT_THROW_IF(! is_initialized(),
                std::logic_error,
                "Rack '" << get_name() << "' is not initialized !");
    _set_initialized(false);
    this->_rack_reset();
    return;
  }

  void rack_base::tree_dump(std::ostream& out_,
                            const std::string& title_,
                            const std::string& indent_,
                            bool inherit_) const
  {
    this->component_base::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << i_tree_dumpable::tag
         << "Format : '" << _format_ << "'" << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Max. number of crates : '" << _max_number_of_crates_ << "'" << std::endl;

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "Crates : " << _crates_.size() << std::endl;
    for (int islot = 0; islot < _max_number_of_crates_; islot++) {
      out_ << indent_ << i_tree_dumpable::inherit_skip_tag(inherit_);
      if (islot == _max_number_of_crates_ - 1) {
        out_ << i_tree_dumpable::last_tag;
      } else {
        out_ << i_tree_dumpable::tag;
      }
      out_ << "Slot #" << islot << " : ";
      if (has_crate(islot)) {
        out_ << "'" << get_crate(islot).get().get_name() << "'";
      } else {
        out_ << "<empty>";
      }
      out_ << std::endl;
    }

    return;
  }

} // end of namespace electronics
