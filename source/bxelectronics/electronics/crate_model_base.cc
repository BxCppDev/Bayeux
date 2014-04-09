// crate_model_base.cc
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
#include "electronics/crate_model_base.h"

// Third Party

// This project
#include <electronics/component_types.h>
#include <electronics/board_model_base.h>
#include <electronics/physical_component.h>

namespace electronics {

  ELECTRONICS_COMPONENT_REGISTRATION_IMPLEMENT(crate_model_base, "electronics::crate_model_base");

  crate_model_base::crate_model_base()
  {
    set_type(TYPE_CRATE);
    set_allow_embedded_components(true);
    _max_number_of_modules_ = 0;
    return;
  }

  crate_model_base::~crate_model_base()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  void crate_model_base::set_max_number_of_modules(uint32_t nos_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Crate model '" << get_name() << "' is already initialized !");
    DT_THROW_IF(_module_labels_.size() > 0,
                std::logic_error,
                "Crate model '" << get_name() << "' already has plugged modules !");
    _max_number_of_modules_ = nos_;
    return;
  }

  uint32_t crate_model_base::get_max_number_of_modules()
  {
    return _max_number_of_modules_;
  }

  bool crate_model_base::has_format() const
  {
    return ! _format_.empty();
  }

  const std::string & crate_model_base::get_format() const
  {
    return _format_;
  }

  void crate_model_base::set_format(const std::string & format_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Crate model '" << get_name() << "' is already initialized !");
    _format_ = format_;
    return;
  }

  bool crate_model_base::has_module(uint32_t module_slot_) const
  {
    return _module_labels_.find(module_slot_) != _module_labels_.end();
  }

  void crate_model_base::remove_module(uint32_t module_slot_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Crate model '" << get_name() << "' is initialized !");
    DT_THROW_IF(!has_module(module_slot_),
                std::logic_error,
                "Crate model '" << get_name() << "' does not have a module at slot '" << module_slot_ << "' !");
    {
      std::string module_label = _module_labels_[module_slot_];
      // Explicit unregistration of the module label at its slot:
      _module_labels_.erase(_module_labels_.find(module_slot_));
      this->_remove_embedded_physical(module_label);
    }
    return;
  }

  physical_component &
  crate_model_base::add_module(uint32_t module_slot_,
                               const component_model_base & module_model_,
                               const std::string & module_label_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Crate model '" << get_name() << "' is already initialized !");
    indexed_labels_dict_type::iterator found = _module_labels_.find(module_slot_);
    DT_THROW_IF(_max_number_of_modules_ == 0,
                std::logic_error,
                "Crate model '" << get_name() << "' does not accept modules !");
    DT_THROW_IF(module_slot_ >= _max_number_of_modules_,
                std::logic_error,
                "Crate model '" << get_name() << "' : Invalid module slot '" << module_slot_ << "' !");
    const board_model_base * board = dynamic_cast<const board_model_base *>(&module_model_);
    DT_THROW_IF(board == 0, //|| module_model_.get_type() != TYPE_MODULE_BOARD,
                std::logic_error,
                "Crate model '" << get_name() << "' : Attempt to embed a non module component '"
                << module_model_.get_name()
                << "' at slot '" << module_slot_ << "' !");
    uint32_t module_width = 1;
    if (board != 0) {
      module_width =  board->get_slot_width();
    }
    for (int islot = module_slot_;
         islot < module_slot_ + module_width;
         islot++) {
        DT_THROW_IF(has_module(islot),
                    std::logic_error,
                    "Crate model '" << get_name() << "' already has a module at slot '" << islot << "' !");
    }
    if (has_format() && board->has_format()) {
      DT_THROW_IF(_format_ != board->get_format(),
                  std::logic_error,
                  "Crate model '" << get_name() << "' :  Attempt to embed a module board with incompatible format ('"
                  << board->get_format()
                  << "') at slot '" << module_slot_ << "' !");
    }
    // Finally, insert the module as a embedded component:
    // Compute the module label:
    std::string module_label = module_label_;
    if (module_label.empty()) {
      if (_module_labels_.find(module_slot_) != _module_labels_.end()) {
        module_label = _module_labels_[module_slot_];
      } else {
        // Automated module label:
        std::ostringstream mod_label;
        mod_label << "module_" << module_slot_;
        module_label = mod_label.str();
      }
    }
    // Explicit registration of the module label at its slot:
    _module_labels_[module_slot_] = module_label;
    return this->_add_embedded_physical(module_label, module_model_.get_logical());
  }

  const physical_component &
  crate_model_base::get_module(uint32_t module_slot_)
  {
    indexed_labels_dict_type::iterator found = _module_labels_.find(module_slot_);
    DT_THROW_IF(found == _module_labels_.end(),
                std::logic_error,
                "Crate model '" << get_name() << "' has no embedded module at slot '" << module_slot_ << "' !");
    return get_embedded_component(found->second);
  }

  void crate_model_base::_at_initialize(const datatools::properties& config_,
                                        component_model_pool_type& components_)
  {
    this->_crate_initialize(config_, components_);
    return;
  }

  void crate_model_base::_at_reset()
  {
    this->_crate_reset();
    return;
  }

  void crate_model_base::_crate_reset()
  {
    _module_labels_.clear();
    _max_number_of_modules_ = 0;
    _format_.clear();
    this->_component_reset();
    return;
  }

  void crate_model_base::_crate_initialize(const datatools::properties & config_,
                                           component_model_pool_type& component_models_)
  {
    this->_component_initialize(config_, component_models_);

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
        uint32_t mnom = n;
        set_max_number_of_modules(mnom);
      }
    }

    if (_max_number_of_modules_ > 0) {
      for (int islot = 0; islot < _max_number_of_modules_; islot++) {
       // Attempt to find a module model at the visited slot:
        std::ostringstream module_slot_model_key;
        module_slot_model_key << "crate.modules.slot_" << islot << ".model" ;
        if (config_.has_key(module_slot_model_key.str())) {
          std::string module_model = config_.fetch_string(module_slot_model_key.str());
          component_model_pool_type::iterator found = component_models_.find(module_model);
          DT_THROW_IF(found == component_models_.end(),
                      std::logic_error,
                      "Cannot find a component model named '" << module_model << "' !");
          component_model_handle_type & module_model_handle
            = found->second.grab_initialized_component_model_handle();
          std::ostringstream module_slot_label_key;
          module_slot_label_key << "crate.modules.slot_" << islot << ".label";
          std::string module_label;
          // Attempt to find a module label:
          if (config_.has_key(module_slot_label_key.str())) {
            module_label = config_.fetch_string(module_slot_label_key.str());
          }
          add_module(islot, module_model_handle.get(), module_label);
        }
      }
    }

    return;
  }

  /// Post remove
  void crate_model_base::_post_remove_embedded_physical(const std::string & embedded_label_)
  {
    // Explicitely unregistration of a module:
    indexed_labels_dict_type::iterator found
      = std::find_if(_module_labels_.begin(),
                     _module_labels_.end(),
                     std::bind2nd(map_data_compare<indexed_labels_dict_type>(),
                                  embedded_label_));
    if (found != _module_labels_.end()) {
      // If the removed embedded component is one of the registered modules, it is unregistered:
      DT_LOG_TRACE(get_logging_priority(),
                   "Unregistering module at slot='" << found->first << "'");
      _module_labels_.erase(found);
    }
    return;
  }

  void crate_model_base::tree_dump(std::ostream& out_,
                             const std::string& title_,
                             const std::string& indent_,
                             bool inherit_) const
  {
    this->component_model_base::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << i_tree_dumpable::tag
         << "Format : '" << _format_ << "'" << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Max. number of modules : '" << _max_number_of_modules_ << "'" << std::endl;

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "Modules : ";
    if (_max_number_of_modules_ == 0) {
      out_ << "<none>";
    }
    out_ << std::endl;
    out_ << indent_
         << i_tree_dumpable::inherit_skip_tag(inherit_)
         << i_tree_dumpable::tag
         << "Number of slots : "
         << _max_number_of_modules_ << std::endl;

    out_ << indent_
         << i_tree_dumpable::inherit_skip_tag(inherit_)
         << i_tree_dumpable::tag
         << "Format : '" << _format_ << "'" << std::endl;

    out_ << indent_
         << i_tree_dumpable::inherit_skip_tag(inherit_)
         << i_tree_dumpable::last_tag
         << "Modules : ";
    if (_module_labels_.size() > 0) {
      out_ << _module_labels_.size();
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;
    int count_width = 0;
    for (int islot = 0; islot < _max_number_of_modules_; islot++) {
      out_ << indent_ << i_tree_dumpable::inherit_skip_tag(inherit_);
      out_ << i_tree_dumpable::last_skip_tag;
      if (islot == _max_number_of_modules_ - 1) {
        out_ << i_tree_dumpable::last_tag;
      } else {
        out_ << i_tree_dumpable::tag;
      }
      out_ << "Slot #" << islot << " : ";
      indexed_labels_dict_type::const_iterator found_at_slot = _module_labels_.find(islot);
      if (found_at_slot != _module_labels_.end()) {
        const board_model_base & board = dynamic_cast<const board_model_base &>(get_embedded_component(found_at_slot->second).get_logical().get_model());
        out_ << "Model '" << board.get_name() << "'";
        count_width = board.get_slot_width();
        count_width--;
      } else {
        if (count_width > 0) {
          out_ << "<>";
          count_width--;
        } else {
          out_ << "<empty>";
          count_width = 0;
        }
      }
      out_ << std::endl;
    }

    return;
  }

} // end of namespace electronics
