// component_base.cc
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
#include "electronics/component_base.h"

// Third Party
// - Bayeux/datatools
#include <datatools/properties.h>
#include <datatools/exception.h>

// This project
#include <electronics/component_utils.h>
#include <electronics/component_types.h>
#include <electronics/component_manager.h>

namespace electronics {

  DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(component_base,
                                                   "electronics::component_base/__system__");

  ELECTRONICS_COMPONENT_REGISTRATION_IMPLEMENT(component_base, "electronics::component_base");

  component_base::component_base()
  {
    _initialized_ = false;
    _type_ = TYPE_UNDEFINED;
    return;
  }

  component_base::component_base(component_type type_)
  {
    set_type(type_);
    return;
  }

  component_base::~component_base()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  bool component_base::allow_embedded_components() const
  {
    return true;
  }

  bool component_base::has_type() const
  {
    return _type_ != TYPE_UNDEFINED;
  }

  void component_base::set_type(component_type type_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Component '" << get_name() << "' is already initialized !");
    _type_ = (int32_t) type_;
    return;
  }

  component_type component_base::get_type() const
  {
    return (component_type) _type_;
  }

  void component_base::build_embedded_labels(std::vector<std::string>& labels_) const
  {
    labels_.clear();
    labels_.reserve(_embedded_components_.size());
    for (component_dict_type::const_iterator i = _embedded_components_.begin();
         i != _embedded_components_.end();
         i++) {
      labels_.push_back(i->first);
    }
    return;
  }

  bool component_base::has_embedded(const std::string & embedded_label_) const
  {
    return _embedded_components_.find(embedded_label_) != _embedded_components_.end();
  }

  void component_base::remove_embedded(const std::string & embedded_label_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Component '" << get_name() << "' is already locked !");
    DT_THROW_IF(embedded_label_.empty(),
                std::logic_error,
                "Component '" << get_name() << "' : Invalid empty embedded label !");
    DT_THROW_IF(!has_embedded(embedded_label_),
                std::logic_error,
                "Component '" << get_name() << "' does not have a embedded component with label '"
                << embedded_label_ << "' !");
    component_dict_type::iterator embedded = _embedded_components_.find(embedded_label_);
    _embedded_components_.erase(embedded);
    return;
  }

  void component_base::add_embedded(const std::string & embedded_label_,
                                    component_handle_type & embedded_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Component '" << get_name() << "' is already locked !");
    DT_THROW_IF(! this->allow_embedded_components(),
                std::logic_error,
                "Component '" << get_name() << "' does not allow embedded components !");
    DT_THROW_IF(embedded_label_.empty(),
                std::logic_error,
                "Component '" << get_name() << "' does not accept embedded with an empty label !");
    DT_THROW_IF(has_embedded(embedded_label_),
                std::logic_error,
                "Component '" << get_name() << "' already has a embedded component with label '"
                << embedded_label_ << "' !");
    DT_THROW_IF(! embedded_.has_data(),
                std::logic_error,
                "Component '" << get_name()
                << "' does not accept NULL embedded component with label '"
                << embedded_label_ << "' !");
    DT_THROW_IF(((int) embedded_.get().get_type() < (int) get_type()),
                std::logic_error,
                "Component '" << get_name()
                << "' of type '" << get_type()
                << "' cannot accept embedded component with label '"
                << embedded_label_ << "' of type '"
                << get_component_type_label(embedded_.get().get_type())
                << "' !");
    _embedded_components_[embedded_label_] = embedded_;
    return;
  }

  component_handle_type &
  component_base::grab_embedded(const std::string & embedded_label_)
  {
    component_dict_type::iterator found = _embedded_components_.find(embedded_label_);
    DT_THROW_IF(found == _embedded_components_.end(),
                std::logic_error,
                "Component '" << get_name() << "' has no embedded with label '"
                << embedded_label_ << "' !");
    return found->second;
  }

  const component_handle_type &
  component_base::get_embedded(const std::string & embedded_label_) const
  {
    component_dict_type::const_iterator found = _embedded_components_.find(embedded_label_);
    DT_THROW_IF(found == _embedded_components_.end(),
                std::logic_error,
                "Component '" << get_name() << "' has no embedded with label '"
                << embedded_label_ << "' !");
    return found->second;
  }

  void component_base::tree_dump(std::ostream& out_,
                                 const std::string& title_,
                                 const std::string& indent_,
                                 bool inherit_) const
  {
    this->advanced_object::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << i_tree_dumpable::tag
         << "Initialized : " << _initialized_ << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Type : '" << get_component_type_label((component_type)_type_)
         << "'" << std::endl;

    {
      out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
           << "Embedded components : ";
      if (_embedded_components_.size() == 0) {
        out_ << "<none>";
        out_ << std::endl;
      } else {
        out_ << std::endl;
      }
      for (component_dict_type::const_iterator i = _embedded_components_.begin();
           i != _embedded_components_.end();
           i++) {
        component_dict_type::const_iterator j = i;
        j++;
        out_ << indent_;
        out_ << i_tree_dumpable::inherit_skip_tag(inherit_);
        if (j == _embedded_components_.end()) {
          out_ <<  i_tree_dumpable::last_tag;
        } else {
          out_ <<  i_tree_dumpable::tag;
        }
        out_ << i->first << " : '" << i->second.get().get_name() << "'"
             << " (as type '" << get_component_type_label((component_type) i->second.get().get_type()) << "')";
        out_ << std::endl;
      }
    }

    return;
  }

  bool component_base::is_initialized() const
  {
    return _is_initialized();
  }

  void component_base::initialize_simple()
  {
    datatools::properties dummy_config;
    component_pool_type dummy_pool;
    this->initialize(dummy_config, dummy_pool);
    return;
  }

  void component_base::initialize_standalone(const datatools::properties& config_)
  {
    component_pool_type dummy_pool;
    this->initialize(config_, dummy_pool);
    return;
  }

  void component_base::initialize(const datatools::properties& config_,
                                  component_pool_type& components_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Component '" << get_name() << "' is already initialized !");

    this->_component_initialize(config_, components_);

    _set_initialized(true);
    return;
  }

  void component_base::reset()
  {
    DT_THROW_IF(! is_initialized(),
                std::logic_error,
                "Component '" << get_name() << "' is not initialized !");
    _set_initialized(false);
    this->_component_reset();
    return;
  }

  void component_base::_component_reset()
  {
    _embedded_components_.clear();
    _type_ = TYPE_UNDEFINED;
    return;
  }

  void component_base::_component_initialize(const datatools::properties& config_,
                                             component_pool_type& components_)
  {
    // Logging priority:
    datatools::logger::priority lp
      = datatools::logger::extract_logging_configuration(config_);
    set_logging_priority(lp);

    if (! has_name()) {
      if (config_.has_key("component.name")) {
        set_name(config_.fetch_string("component.name"));
      }
    }

    if (! has_display_name()) {
      if (config_.has_key("component.display_name")) {
        set_display_name(config_.fetch_string("component.display_name"));
      }
    }

    if (! has_terse_description()) {
      if (config_.has_key("component.terse_description")) {
        set_terse_description(config_.fetch_string("component.terse_description"));
      }
    }

    if (! has_type()) {
      if (config_.has_key("component.type")) {
        set_type(get_component_type(config_.fetch_string("component.type")));
      }
    }

    if (this->allow_embedded_components()) {
      if (config_.has_key("component.embedded.labels")) {
        std::vector<std::string> labels;
        config_.fetch("component.embedded.labels", labels);
        for (std::vector<std::string>::const_iterator i = labels.begin ();
             i != labels.end ();
             i++) {
          const std::string component_label = *i;
          std::ostringstream component_model_oss;
          component_model_oss << "component.embedded."
                              << component_label << ".model";
          DT_THROW_IF (! config_.has_key(component_model_oss.str ()),
                       std::logic_error,
                       "Missing '" << component_model_oss.str () << "' property !");
          std::string component_model_name = config_.fetch_string(component_model_oss.str());
          component_pool_type::iterator found = components_.find(component_model_name);
          DT_THROW_IF (found == components_.end (),
                       std::logic_error,
                       "Cannot find electronic component model with name '"
                       << component_model_name << "' !");
          component_handle_type & comp_handle = found->second.grab_initialized_component_handle();
          add_embedded(component_label, comp_handle);
        }
      }
    }

    return;
  }

  bool component_base::_is_initialized() const
  {
    return _initialized_;
  }

  void component_base::_set_initialized(bool init_)
  {
    _initialized_ = init_;
    return;
  }

} // namespace electronics
