// rack_model_base.cc
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
#include "electronics/rack_model_base.h"

// Third Party

// This project
#include <electronics/crate_model_base.h>
#include <electronics/component_types.h>
#include <electronics/physical_component.h>

namespace electronics {

  ELECTRONICS_COMPONENT_REGISTRATION_IMPLEMENT(rack_model_base, "electronics::rack_model_base");

  rack_model_base::rack_model_base() : component_model_base(TYPE_RACK)
  {
    set_allow_embedded_components(true);
    _max_number_of_crates_ = 0;
    return;
  }

  rack_model_base::~rack_model_base()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  void rack_model_base::set_max_number_of_crates(uint32_t nos_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Rack model '" << get_name() << "' is already initialized !");
    DT_THROW_IF(_crate_labels_.size() > 0,
                std::logic_error,
                "Rack model '" << get_name() << "' already has plugged crates !");
    _max_number_of_crates_ = nos_;
    return;
  }

  uint32_t rack_model_base::get_max_number_of_crates()
  {
    return _max_number_of_crates_;
  }

  bool rack_model_base::has_format() const
  {
    return ! _format_.empty();
  }

  const std::string & rack_model_base::get_format() const
  {
    return _format_;
  }

  void rack_model_base::set_format(const std::string & format_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Rack model '" << get_name() << "' is already initialized !");
    _format_ = format_;
    return;
  }

  bool rack_model_base::has_crate(uint32_t crate_slot_) const
  {
    return _crate_labels_.find(crate_slot_) != _crate_labels_.end();
  }

  void rack_model_base::remove_crate(uint32_t crate_slot_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Rack model '" << get_name() << "' is locked !");
    DT_THROW_IF(!has_crate(crate_slot_),
                std::logic_error,
                "Rack model '" << get_name() << "' does not have a crate at slot '" << crate_slot_ << "' !");
    {
      std::string crate_label = _crate_labels_[crate_slot_];
      // Explicit unregistration of the crate label at its slot:
      _crate_labels_.erase(_crate_labels_.find(crate_slot_));
      this->_remove_embedded_physical(crate_label);
    }
    return;
  }

  physical_component &
  rack_model_base::add_crate(uint32_t crate_slot_,
                             const component_model_base & crate_model_,
                             const std::string & crate_label_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Rack model '" << get_name() << "' is already initialized !");
    indexed_labels_dict_type::iterator found = _crate_labels_.find(crate_slot_);
    DT_THROW_IF(_max_number_of_crates_ == 0,
                std::logic_error,
                "Rack model '" << get_name() << "' does not accept crates !");
    DT_THROW_IF(has_crate(crate_slot_),
                std::logic_error,
                "Rack model '" << get_name() << "' already has a crate at slot '" << crate_slot_ << "' !");
    const crate_model_base * crate = dynamic_cast<const crate_model_base *>(&crate_model_);
    DT_THROW_IF(crate == 0 || crate_model_.get_type() != TYPE_CRATE,
                std::logic_error,
                "Rack model '" << get_name() << "' : Attempt to embed a non crate component '"
                <<  crate_model_.get_name()
                << "' at slot '" << crate_slot_ << "' !");

    if (has_format() && crate->has_format()) {
      DT_THROW_IF(_format_ != crate->get_format(),
                  std::logic_error,
                  "Rack model '" << get_name() << "' :  Attempt to embed a crate with incompatible format ('"
                  << crate->get_format()
                  << "') at slot '" << crate_slot_ << "' !");
    }

    // Finally, insert the crate as a embedded component:
    // Compute the crate label:
    std::string crate_label = crate_label_;
    if (crate_label.empty()) {
      if (_crate_labels_.find(crate_slot_) != _crate_labels_.end()) {
        crate_label = _crate_labels_[crate_slot_];
        } else {
        // Automated crate label:
        std::ostringstream mod_label;
        mod_label << "crate_" << crate_slot_;
        crate_label = mod_label.str();
      }
    }
    // Explicit registration of the crate label at its slot:
    _crate_labels_[crate_slot_] = crate_label;
    return this->_add_embedded_physical(crate_label, crate_model_.get_logical());
  }

  const physical_component &
  rack_model_base::get_crate(uint32_t crate_slot_)
  {
    indexed_labels_dict_type::iterator found = _crate_labels_.find(crate_slot_);
    DT_THROW_IF(found == _crate_labels_.end(),
                std::logic_error,
                "Rack model '" << get_name() << "' has no embedded crate at slot '" << crate_slot_ << "' !");
    return get_embedded_component(found->second);
  }

  void rack_model_base::_at_initialize(const datatools::properties& config_,
                                        component_model_pool_type& components_)
  {
    this->_rack_initialize(config_, components_);
    return;
  }

  void rack_model_base::_at_reset()
  {
    this->_rack_reset();
    return;
  }

  void rack_model_base::_rack_reset()
  {
    _crate_labels_.clear();
    _max_number_of_crates_ = 0;
    _format_.clear();
    this->_component_reset();
    return;
  }

  void rack_model_base::_rack_initialize(const datatools::properties & config_,
                                         component_model_pool_type& component_models_)
  {
    this->_component_initialize(config_, component_models_);

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
        uint32_t mnoc = n;
        set_max_number_of_crates(mnoc);
      }
    }

    if (_max_number_of_crates_ > 0) {
      // Loop on crate slots:
      for (int islot = 0; islot < _max_number_of_crates_; islot++) {
        DT_LOG_TRACE(get_logging_priority(), "Search for crate slot " << islot << " : ");
        // Attempt to find a crate model at the visited slot:
        std::ostringstream crate_slot_model_key;
        crate_slot_model_key << "rack.crates.slot_" << islot << ".model";
        DT_LOG_TRACE(get_logging_priority(),
                     "  Prop key: '" << crate_slot_model_key.str() << "'");
        config_.tree_dump(std::cerr, "Config: ", ">>>");
        if (config_.has_key(crate_slot_model_key.str())) {
          std::string crate_model = config_.fetch_string(crate_slot_model_key.str());
          component_model_pool_type::iterator found = component_models_.find(crate_model);
          DT_THROW_IF(found == component_models_.end(),
                      std::logic_error,
                      "Cannot find a component model named '" << crate_model << "' !");
          component_model_handle_type & crate_model_handle
            = found->second.grab_initialized_component_model_handle();
          std::ostringstream crate_slot_label_key;
          crate_slot_label_key << "rack.crates.slot_" << islot << ".label";
          DT_LOG_TRACE(get_logging_priority(),
                       "  Prop key: '" << crate_slot_label_key.str() << "'");
          std::string crate_label;
          // Attempt to find a crate label:
          if (config_.has_key(crate_slot_label_key.str())) {
            crate_label = config_.fetch_string(crate_slot_label_key.str());
          }
          add_crate(islot, crate_model_handle.get(), crate_label);
        }
      }
    }

    return;
  }

  /// Post remove
  void rack_model_base::_post_remove_embedded_physical(const std::string & embedded_label_)
  {
    // Explicitely unregistration of a crate:
    indexed_labels_dict_type::iterator found
      = std::find_if(_crate_labels_.begin(),
                     _crate_labels_.end(),
                     std::bind2nd(map_data_compare<indexed_labels_dict_type>(),
                                  embedded_label_));
    if (found != _crate_labels_.end()) {
      // If the removed embedded component is one of the registered crates, it is unregistered:
      DT_LOG_TRACE(get_logging_priority(),
                   "Unregistering crate at slot='" << found->first << "'");
      _crate_labels_.erase(found);
    }
    return;
  }

  void rack_model_base::tree_dump(std::ostream& out_,
                                  const std::string& title_,
                                  const std::string& indent_,
                                  bool inherit_) const
  {
    this->component_model_base::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << i_tree_dumpable::tag
         << "Format : '" << _format_ << "'" << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Max. number of crates : '" << _max_number_of_crates_ << "'" << std::endl;

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "Crates : ";
    if (_max_number_of_crates_ == 0) {
      out_ << "<none>";
    }
    out_ << std::endl;
    out_ << indent_
         << i_tree_dumpable::inherit_skip_tag(inherit_)
         << i_tree_dumpable::tag
         << "Number of slots : "
         << _max_number_of_crates_ << std::endl;

    out_ << indent_
         << i_tree_dumpable::inherit_skip_tag(inherit_)
         << i_tree_dumpable::tag
         << "Format : '" << _format_ << "'" << std::endl;

    out_ << indent_
         << i_tree_dumpable::inherit_skip_tag(inherit_)
         << i_tree_dumpable::last_tag
         << "Crates : ";
    if (_crate_labels_.size() > 0) {
      out_ << _crate_labels_.size();
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;
    for (int islot = 0; islot < _max_number_of_crates_; islot++) {
      out_ << indent_ << i_tree_dumpable::inherit_skip_tag(inherit_);
      out_ << i_tree_dumpable::last_skip_tag;
      if (islot == _max_number_of_crates_ - 1) {
        out_ << i_tree_dumpable::last_tag;
      } else {
        out_ << i_tree_dumpable::tag;
      }
      out_ << "Slot #" << islot << " : ";
      indexed_labels_dict_type::const_iterator found_at_slot = _crate_labels_.find(islot);
      if (found_at_slot != _crate_labels_.end()) {
        out_ << "Model '" << get_embedded_component(found_at_slot->second).get_logical().get_model().get_name() << "'";
      } else {
        out_ << "<empty>";
      }
      out_ << std::endl;
    }
    return;
  }

} // end of namespace electronics
