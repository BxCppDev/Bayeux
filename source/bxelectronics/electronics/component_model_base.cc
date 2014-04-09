// component_model_base.cc
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
#include "electronics/component_model_base.h"

// Third Party
// - Bayeux/datatools
#include <datatools/properties.h>
#include <datatools/exception.h>

// This project
#include <electronics/component_utils.h>
#include <electronics/component_types.h>
#include <electronics/component_manager.h>
#include <electronics/logical_component.h>
#include <electronics/physical_component.h>

namespace electronics {

  DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(component_model_base,
                                                   "electronics::component_model_base/__system__");

  ELECTRONICS_COMPONENT_REGISTRATION_IMPLEMENT(component_model_base, "electronics::component_model_base");

  component_model_base::component_model_base()
  {
    set_logging_priority(datatools::logger::PRIO_FATAL);
    _initialized_ = false;
    _type_ = TYPE_UNDEFINED;
    _allow_embedded_components_ = true;
    return;
  }

  component_model_base::component_model_base(component_type type_)
  {
    set_logging_priority(datatools::logger::PRIO_FATAL);
    _initialized_ = false;
    _type_ = TYPE_UNDEFINED;
    _allow_embedded_components_ = true;
    set_type(type_);
    return;
  }

  component_model_base::~component_model_base()
  {
    _embedded_physical_components_.clear();
    if (_logical_) {
      _logical_.reset();
    }
    return;
  }

  bool component_model_base::is_allow_embedded_components() const
  {
    return _allow_embedded_components_;
  }

  void component_model_base::set_allow_embedded_components(bool a_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Component model '" << get_name() << "' is already initialized !");
    _allow_embedded_components_ = a_;
    return;
  }

  bool component_model_base::has_type() const
  {
    return _type_ != TYPE_UNDEFINED;
  }

  void component_model_base::set_type(component_type type_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Component model '" << get_name() << "' is already initialized !");
    _type_ = (int32_t) type_;
    return;
  }

  component_type component_model_base::get_type() const
  {
    return (component_type) _type_;
  }

  const component_model_base::digital_ports_dict_type &
  component_model_base::get_digital_ports() const
  {
    return _digital_ports_;
  }

  const component_model_base::digital_channels_dict_type
  component_model_base::get_digital_channels() const
  {
    return _digital_channels_;
  }

  void component_model_base::add_digital_port(const std::string & name_,
                                              const digital_port_desc & port_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Component model '" << get_name() << "' is already initialized !");
    DT_THROW_IF(name_.empty(),
                std::logic_error,
                "Component model '" << get_name() << "' cannot host an anonymous digital port !");
    DT_THROW_IF(! port_.is_initialized(),
                std::logic_error,
                "Component model '" << get_name() << "' cannot host an uninitialized digital port !");
    digital_ports_dict_type::const_iterator found_embedded
        = _digital_ports_.find(name_);
    DT_THROW_IF (found_embedded != _digital_ports_.end(),
                 std::logic_error,
                 "Component model '" << get_name()
                 << "' already has a digital port with name '"
                 << name_ << "' !");
    _digital_ports_[name_] = port_;
    return;
  }

  void component_model_base::add_digital_channel(const std::string & name_,
                                                 const digital_channel_desc & channel_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Component model '" << get_name() << "' is already initialized !");
    DT_THROW_IF(name_.empty(),
                std::logic_error,
                "Component model '" << get_name() << "' cannot host an anonymous digital channel !");
    DT_THROW_IF(! channel_.is_initialized(),
                std::logic_error,
                "Component model '" << get_name() << "' cannot host an uninitialized digital channel !");
    digital_channels_dict_type::const_iterator found_embedded
        = _digital_channels_.find(name_);
    DT_THROW_IF (found_embedded != _digital_channels_.end(),
                 std::logic_error,
                 "Component model '" << get_name()
                 << "' already has a digital channel with name '"
                 << name_ << "' !");
    _digital_channels_[name_] = channel_;
    return;
  }

  void component_model_base::add_register(const std::string & name_,
                                          const register_desc & reg_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Component model '" << get_name() << "' is already initialized !");
    DT_THROW_IF(name_.empty(),
                std::logic_error,
                "Component model '" << get_name() << "' cannot host an anonymous register !");
    DT_THROW_IF(! reg_.is_initialized(),
                std::logic_error,
                "Component model '" << get_name() << "' cannot host an uninitialized register !");
    registers_dict_type::const_iterator found_embedded = _registers_.find(name_);
    DT_THROW_IF(found_embedded != _registers_.end(),
                std::logic_error,
                "Component model '" << get_name()
                << "' already has a register with name '"
                << name_ << "' !");
    _registers_[name_] = reg_;
    return;
  }

  bool component_model_base::has_logical() const
  {
    return _logical_;
  }

  const logical_component & component_model_base::get_logical() const
  {
    DT_THROW_IF(! has_logical(), std::logic_error,
                "Component model '" << get_name() << "' has no logical component !");
    return *_logical_;
  }

  logical_component & component_model_base::_grab_logical()
  {
    DT_THROW_IF(! has_logical(), std::logic_error,
                "Component model '" << get_name() << "' has no logical component !");
    return *_logical_;
  }

  bool component_model_base::is_initialized() const
  {
    return _initialized_;
  }

  void component_model_base::initialize_simple()
  {
    datatools::properties dummy_config;
    component_model_pool_type dummy_pool;
    this->initialize(dummy_config, dummy_pool);
    return;
  }

  void component_model_base::initialize_standalone(const datatools::properties& config_)
  {
    component_model_pool_type dummy_pool;
    this->initialize(config_, dummy_pool);
    return;
  }

  void component_model_base::_at_initialize(const datatools::properties & config_,
                                             component_model_pool_type& component_models_)
  {
    this->_component_initialize(config_, component_models_);
    return;
  }

  void component_model_base::_at_reset()
  {
    this->_component_reset();
    return;
  }

  void component_model_base::initialize(const datatools::properties& config_,
                                        component_model_pool_type& component_models_)
  {
    DT_LOG_TRACE(get_logging_priority(), "Entering...");
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Component model '" << get_name() << "' is already initialized !");

    // Initialization:
    _at_initialize(config_, component_models_);
    DT_LOG_TRACE(get_logging_priority(), "Initialization done.");

    // Post initialization:
    _create_logical_();
    _logical_->set_name(logical_component::make_logical_component_name(this->get_name()));
    // Populate the logical:
    for (physical_dict_type::iterator i = _embedded_physical_components_.begin();
         i != _embedded_physical_components_.end();
         i++) {
      const std::string & label = i->first;
      const physical_component & physical = i->second;
      _logical_->add_daughter(label, physical);
    }
    _logical_->initialize();
    DT_LOG_TRACE(get_logging_priority(), "Post initialization done.");

    _set_initialized_(true);
    DT_LOG_TRACE(get_logging_priority(), "Exiting.");
    return;
  }

  void component_model_base::reset()
  {
    DT_THROW_IF(! is_initialized(),
                std::logic_error,
                "Component model '" << get_name() << "' is not initialized !");
    _set_initialized_(false);

    // Pre reset:
    _logical_->reset();

    // Reset:
    _at_reset();
    this->enriched_base::clear();
    return;
  }

  void component_model_base::_component_reset()
  {
    DT_LOG_TRACE(get_logging_priority(), "Entering...");
    _embedded_physical_components_.clear();
    _digital_channels_.clear();
    _digital_ports_.clear();
    _allow_embedded_components_ = true;
    _type_ = TYPE_UNDEFINED;
    DT_LOG_TRACE(get_logging_priority(),"Exiting.");
    return;
  }

  void component_model_base::_component_initialize(const datatools::properties& config_,
                                                   component_model_pool_type& component_models_)
  {
    DT_LOG_TRACE(get_logging_priority(), "Entering...");
    // Logging priority:
    datatools::logger::priority lp
      = datatools::logger::extract_logging_configuration(config_);
    set_logging_priority(lp);

    if (! has_name()) {
      if (config_.has_key("name")) {
        set_name(config_.fetch_string("name"));
      }
    }

    if (! has_display_name()) {
      if (config_.has_key("display_name")) {
        set_display_name(config_.fetch_string("display_name"));
      }
    }

    if (! has_terse_description()) {
      if (config_.has_key("terse_description")) {
        set_terse_description(config_.fetch_string("terse_description"));
      }
    }

    if (! has_type()) {
      if (config_.has_key("type")) {
        set_type(get_component_type(config_.fetch_string("type")));
      }
    }

    if (config_.has_key("allow_embedded_components")) {
      set_allow_embedded_components(config_.fetch_boolean("allow_embedded_components"));
    }

    if (! _digital_ports_.size()) {
      std::vector<std::string> ports_labels;
      if (config_.has_key("digital_ports")) {
        config_.fetch("digital_ports", ports_labels);
      }
      for (int i = 0; i < ports_labels.size(); i++) {
        std::ostringstream oss1;
        oss1 << "digital_ports." << ports_labels[i] << ".";
        digital_port_desc port;
        port.set_name(ports_labels[i]);
        datatools::properties port_cfg;
        config_.export_and_rename_starting_with(port_cfg, oss1.str(), "");
        port.initialize(port_cfg);
        add_digital_port(ports_labels[i], port);
      }
    }

    if (! _digital_channels_.size()) {
      std::vector<std::string> channels_labels;
      if (config_.has_key("digital_channels")) {
        config_.fetch("digital_channels", channels_labels);
      }
      for (int i = 0; i < channels_labels.size(); i++) {
        std::ostringstream oss1;
        oss1 << "digital_channels." << channels_labels[i] << ".";
        digital_channel_desc channel;
        channel.set_name(channels_labels[i]);
        datatools::properties channel_cfg;
        config_.export_and_rename_starting_with(channel_cfg, oss1.str(), "");
        channel.initialize(channel_cfg);
        add_digital_channel(channels_labels[i], channel);
      }
    }

    if (config_.has_key("embedded_components.labels")) {
      DT_THROW_IF(! is_allow_embedded_components(),
                  std::logic_error,
                  "Component model '" << get_name() << "' does not allow embedded components !");

      std::vector<std::string> labels;
      config_.fetch("embedded_components.labels", labels);
      for (std::vector<std::string>::const_iterator i = labels.begin ();
           i != labels.end ();
           i++) {
        const std::string component_label = *i;
        std::ostringstream component_model_oss;
        component_model_oss << "embedded_components."
                            << "model." << component_label;
        DT_THROW_IF (! config_.has_key(component_model_oss.str ()),
                     std::logic_error,
                     "Component model '" << get_name() << "' : Missing '" << component_model_oss.str () << "' property !");
        std::string component_model_name = config_.fetch_string(component_model_oss.str());
        component_model_pool_type::iterator found = component_models_.find(component_model_name);
        DT_THROW_IF (found == component_models_.end (),
                     std::logic_error,
                     "Component model '" << get_name() << "' : Cannot find electronic component model with name '"
                     << component_model_name << "' !");
        // Fetch the initialized embedded model
        component_model_handle_type & component_model_handle
          = found->second.grab_initialized_component_model_handle();
        this->add_embedded_component(component_label, component_model_handle.get());
      }
    }

    DT_LOG_TRACE(get_logging_priority(),"Exiting.");
    return;
  }

  // virtual
  bool component_model_base::_check_embedded_type(component_type type_) const
  {

    return ((int) type_ < (int) get_type());
  }

  physical_component & component_model_base::_add_embedded_physical(const std::string & component_label_,
                                                                    const logical_component & logical_)
  {
    {
      physical_dict_type::const_iterator found_embedded
        = _embedded_physical_components_.find(component_label_);
      DT_THROW_IF (found_embedded != _embedded_physical_components_.end(),
                   std::logic_error,
                   "Component model '" << get_name()
                   << "' already has an embedded physical component with label '"
                   << component_label_ << "' !");
    }
    DT_THROW_IF(_check_embedded_type(logical_.get_model().get_type()),
                std::logic_error,
                "Component model '" << get_name()
                << "' of type '" << get_component_type_label(get_type())
                << "' cannot accept an embedded component with label '"
                << component_label_ << "' of type '"
                << get_component_type_label(logical_.get_model().get_type())
                << "' !");
    {
      physical_component dummy_physical;
      _embedded_physical_components_[component_label_] = dummy_physical;
    }
    physical_component & physical = _embedded_physical_components_[component_label_];
    physical.set_name(physical_component::make_physical_component_name(get_name()+"."+component_label_));
    physical.set_logical(logical_);
    return physical;
  }

  void component_model_base::_remove_embedded_physical(const std::string & component_label_)
  {
    {
      physical_dict_type::iterator found_embedded
        = _embedded_physical_components_.find(component_label_);
      DT_THROW_IF (found_embedded == _embedded_physical_components_.end(),
                   std::logic_error,
                   "Component model '" << get_name()
                   << "' has no embedded physical component with label '"
                   << component_label_ << "' !");
      _embedded_physical_components_.erase(found_embedded);
    }
    _post_remove_embedded_physical(component_label_);
    return;
  }

  void component_model_base::_set_initialized_(bool init_)
  {
    _initialized_ = init_;
    return;
  }

  void component_model_base::_create_logical_()
  {
    if (! _logical_) {
      _logical_.reset(new logical_component);
      _logical_->set_model(*this);
    }
    return;
  }

  bool component_model_base::has_embedded_component(const std::string & component_label_) const
  {
    physical_dict_type::const_iterator found_embedded
      = _embedded_physical_components_.find(component_label_);
    return found_embedded != _embedded_physical_components_.end();
  }

  physical_component &
  component_model_base::add_embedded_component(const std::string & component_label_,
                                               const component_model_base & component_model_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Component model '" << get_name() << "' is already initialized !");
    DT_THROW_IF(! component_model_.is_initialized(),
                std::logic_error,
                "Component model '" << component_model_.get_name() << "' is not initialized !");
    physical_component & phys = _add_embedded_physical(component_label_,
                                                       component_model_.get_logical());
    return phys;
  }

  void component_model_base::remove_embedded_component(const std::string & component_label_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Component model '" << get_name() << "' is already initialized !");
    _remove_embedded_physical(component_label_);
    return;
  }

  const physical_component &
  component_model_base::get_embedded_component(const std::string & component_label_) const
  {
    physical_dict_type::const_iterator found_embedded
      = _embedded_physical_components_.find(component_label_);
    DT_THROW_IF (found_embedded == _embedded_physical_components_.end(),
                 std::logic_error,
                 "Component model '" << get_name()
                 << "' has no embedded physical component with label '"
                 << component_label_ << "' !");
    return found_embedded->second;
  }

  // virtual
  void component_model_base::_post_add_embedded_physical(const std::string & embedded_label_)
  {
    return;
  }

  // virtual
  void component_model_base::_post_remove_embedded_physical(const std::string & embedded_label_)
  {
    return;
  }

  void component_model_base::tree_dump(std::ostream& out_,
                                 const std::string& title_,
                                 const std::string& indent_,
                                 bool inherit_) const
  {
    this->enriched_base::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << i_tree_dumpable::tag
         << "Type : '" << get_component_type_label((component_type)_type_)
         << "'" << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Digital ports : " << _digital_ports_.size() << std::endl;

    for (digital_ports_dict_type::const_iterator i = _digital_ports_.begin();
         i != _digital_ports_.end();
         i++) {
      digital_ports_dict_type::const_iterator j = i;
      j++;
      out_ << indent_ << i_tree_dumpable::skip_tag;
      if (j == _digital_ports_.end()) {
        out_ << i_tree_dumpable::last_tag;
      } else {
        out_ << i_tree_dumpable::tag;
      }
      out_ << "'" << i->first << "' : "
           << i->second.get_io() << " " << i->second.get_channel_type()
           << " " << i->second.get_datatype();
      out_ << std::endl;
    }

    out_ << indent_ << i_tree_dumpable::tag
         << "Digital channels : " << _digital_channels_.size() << std::endl;
    for (digital_channels_dict_type::const_iterator i = _digital_channels_.begin();
         i != _digital_channels_.end();
         i++) {
      digital_channels_dict_type::const_iterator j = i;
      j++;
      out_ << indent_ << i_tree_dumpable::skip_tag;
      if (j == _digital_channels_.end()) {
        out_ << i_tree_dumpable::last_tag;
      } else {
        out_ << i_tree_dumpable::tag;
      }
      out_ << "'" << i->first << "' : "
           << i->second.get_channel_type() << " " << i->second.get_datatype();
      out_ << std::endl;
    }

    out_ << indent_ << i_tree_dumpable::tag
         << "Registers : " << _registers_.size() << std::endl;
    for (registers_dict_type::const_iterator i = _registers_.begin();
         i != _registers_.end();
         i++) {
      registers_dict_type::const_iterator j = i;
      j++;
      out_ << indent_ << i_tree_dumpable::skip_tag;
      if (j ==_registers_.end()) {
        out_ << i_tree_dumpable::last_tag;
      } else {
        out_ << i_tree_dumpable::tag;
      }
      out_ << "'" << i->first << "' : "
           << i->second.get_size();
      out_ << std::endl;
    }

    out_ << indent_ << i_tree_dumpable::tag
         << "Allow embedded components : " << _allow_embedded_components_
         << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Embedded components : ";
    if (_embedded_physical_components_.size() == 0) {
      out_ << "<none>";
    } else {
      out_ << _embedded_physical_components_.size();
    }
    out_ << std::endl;
    for (physical_dict_type::const_iterator i = _embedded_physical_components_.begin();
         i != _embedded_physical_components_.end();
         i++) {
      const std::string & component_label = i->first;
      const physical_component & physical = i->second;
      physical_dict_type::const_iterator j = i;
      j++;
      out_ << indent_ << i_tree_dumpable::skip_tag;
      if (j == _embedded_physical_components_.end()) {
        out_ << i_tree_dumpable::last_tag;
      } else {
        out_ << i_tree_dumpable::tag;
      }
      out_ << "'" << component_label << "' : as logical '";
      out_ << physical.get_logical().get_name() << "'";
      out_ << std::endl;
    }

    out_ << indent_ << i_tree_dumpable::tag
         << "Initialized : " << _initialized_ << std::endl;

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "Logical : ";
    if (! has_logical()) {
      out_ << "<none>";
      out_ << std::endl;
    } else {
      out_ << std::endl;
    }
    if (has_logical()) {
      std::ostringstream indent_oss;
      indent_oss << indent_ << i_tree_dumpable::inherit_skip_tag(inherit_);
      _logical_->tree_dump(out_, "", indent_oss.str());
    }

    return;
  }

} // namespace electronics
