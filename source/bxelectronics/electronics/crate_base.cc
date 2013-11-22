// crate_base.cc
//
// Copyright (c) 2013 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of Bayeux/electronics.
//
// Bayeux/electronics is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux/electronics is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux/electronics.  If not, see <http://www.gnu.org/licenses/>.

// Ourselves
#include "electronics/crate_base.h"

// Third Party

// This project
#include <electronics/board_base.h>
#include <electronics/component_types.h>

namespace electronics {

  ELECTRONICS_COMPONENT_REGISTRATION_IMPLEMENT(crate_base, "electronics::crate_base");

  crate_base::crate_base()
  {
    set_type(TYPE_CRATE);
    _max_number_of_modules_ = 0;
    return;
  }

  crate_base::~crate_base()
  {
    return;
  }

  void crate_base::set_max_number_of_modules(uint32_t nos_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Crate '" << get_name() << "' is already initialized !");
    _max_number_of_modules_ = nos_;
    return;
  }

  uint32_t crate_base::get_max_number_of_modules()
  {
    return _max_number_of_modules_;
  }

  bool crate_base::has_format() const
  {
    return ! _format_.empty();
  }

  const std::string & crate_base::get_format() const
  {
    return _format_;
  }

  void crate_base::set_format(const std::string & format_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Crate '" << get_name() << "' is already initialized !");
    _format_ = format_;
    return;
  }

  bool crate_base::has_module(uint32_t slot_id_) const
  {
    return _modules_.find(slot_id_) != _modules_.end();
  }

  void crate_base::remove_module(uint32_t slot_id_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Crate '" << get_name() << "' is locked !");
    DT_THROW_IF(!has_module(slot_id_),
                std::logic_error,
                "Crate '" << get_name() << "' does not have a module at slot '" << slot_id_ << "' !");
    _modules_.erase(_modules_.find(slot_id_));
    {
      std::ostringstream mod_label;
      mod_label << "module_" << slot_id_;
      this->remove_embedded(mod_label.str());
    }
    return;
  }

  void crate_base::add_module(uint32_t slot_id_,
                              component_handle_type & module_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Crate '" << get_name() << "' is locked !");
    indexed_component_dict_type::iterator found = _modules_.find(slot_id_);
    DT_THROW_IF(_max_number_of_modules_ == 0,
                std::logic_error,
                "Crate '" << get_name() << "' does not accept modules !");
    DT_THROW_IF(has_module(slot_id_),
                std::logic_error,
                "Crate '" << get_name() << "' already has a module at slot '" << slot_id_ << "' !");
    DT_THROW_IF(! module_.has_data(),
                std::logic_error,
                "Crate '" << get_name() << "' cannot embed a NULL module at slot '" << slot_id_ << "' !");
    const board_base * module = dynamic_cast<const board_base *>(&module_.get());
    DT_THROW_IF(module == 0,
                std::logic_error,
                "Crate '" << get_name() << "' : Attempt to embed a non module component '"
                <<  module_.get().get_name()
                << "' at slot '" << slot_id_ << "' !");

    if (has_format() && module->has_format()) {
      DT_THROW_IF(_format_ != module->get_format(),
                  std::logic_error,
                  "Crate '" << get_name() << "' :  Attempt to embed a module with incompatible format ('"
                  << module->get_format()
                  << "') at slot '" << slot_id_ << "' !");
    }
    // Finally, insert the module:
    {
      std::ostringstream mod_label;
      mod_label << "module_" << slot_id_;
      // ... as a embedded component:
      this->add_embedded(mod_label.str(), module_);
    }
    // ... as an explicit module at its slot:
    _modules_[slot_id_] = module_;
    return;
  }

  component_handle_type & crate_base::grab_module(uint32_t slot_id_)
  {
    indexed_component_dict_type::iterator found = _modules_.find(slot_id_);
    DT_THROW_IF(found == _modules_.end(),
                std::logic_error,
                "Crate '" << get_name() << "' has no embedded module at slot '" << slot_id_ << "' !");
    return found->second;
  }

  const component_handle_type & crate_base::get_module(uint32_t slot_id_) const
  {
    indexed_component_dict_type::const_iterator found = _modules_.find(slot_id_);
    DT_THROW_IF(found == _modules_.end(),
                std::logic_error,
                "Crate '" << get_name() << "' has no embedded module at slot '"
                << slot_id_ << "' !");
    return found->second;
  }


  void crate_base::_crate_reset()
  {
    _modules_.clear();
    _max_number_of_modules_ = 0;
    _format_.clear();
    this->_component_reset();
    return;
  }

  void crate_base::_crate_initialize(const datatools::properties & config_,
                                     component_pool_type& components_)
  {
    this->_component_initialize(config_, components_);

    if (! has_format()) {
      if (config_.has_key("crate.format")) {
        std::string cf = config_.fetch_string("crate.format");
        set_format(cf);
      }
    }

    if (_max_number_of_modules_ == 0) {
      if (config_.has_key("crate.max_number_of_modules")) {
        int32_t n = config_.fetch_integer("crate.max_number_of_modules");
        DT_THROW_IF ((n < 1 || n > 64), std::domain_error,
                     "Invalid number of modules '" << n << "' !");
        uint32_t mnos = n;
        set_max_number_of_modules(mnos);
      }
    }

    if (_max_number_of_modules_ > 0) {
      for (int islot = 0; islot < _max_number_of_modules_; islot++) {
        std::ostringstream mb_slot_key;
        mb_slot_key << "crate.modules.slot_" << islot;
        if (config_.has_key(mb_slot_key.str())) {
          std::string module_name = config_.fetch_string(mb_slot_key.str());
          component_handle_type & module_handle
            = components_.find(module_name)->second.grab_component_handle();
          add_module(islot, module_handle);
        }
      }
    }

    return;
  }

  void crate_base::initialize(const datatools::properties& config_,
                              component_pool_type& components_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Crate '" << get_name() << "' is already initialized !");

    this->_crate_initialize(config_, components_);

    _set_initialized(true);
    return;
  }

  void crate_base::reset()
  {
    DT_THROW_IF(! is_initialized(),
                std::logic_error,
                "Crate '" << get_name() << "' is not initialized !");
    _set_initialized(false);
    this->_crate_reset();
    return;
  }

  void crate_base::tree_dump(std::ostream& out_,
                             const std::string& title_,
                             const std::string& indent_,
                             bool inherit_) const
  {
    this->component_base::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << i_tree_dumpable::tag
         << "Format : '" << _format_ << "'" << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Max. number of modules : '" << _max_number_of_modules_ << "'" << std::endl;

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "Modules : " << _modules_.size() << std::endl;
    for (int islot = 0; islot < _max_number_of_modules_; islot++) {
      out_ << indent_ << i_tree_dumpable::inherit_skip_tag(inherit_);
      if (islot == _max_number_of_modules_ - 1) {
        out_ << i_tree_dumpable::last_tag;
      } else {
        out_ << i_tree_dumpable::tag;
      }
      out_ << "Slot #" << islot << " : ";
      if (has_module(islot)) {
        out_ << "'" << get_module(islot).get().get_name() << "'";
      } else {
        out_ << "<empty>";
      }
      out_ << std::endl;
    }

    return;
  }

} // end of namespace electronics
