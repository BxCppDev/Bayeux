// datatools/configuration/variant_repository.cc
/*
 * Copyright (C) 2014 Francois Mauger <mauger@lpccaen.in2p3.fr>
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

// Ourselves:
#include <datatools/configuration/variant_repository.h>

// This project (Bayeux/datatools):
#include <datatools/configuration/variant_registry.h>
#include <datatools/configuration/variant_registry_manager.h>

namespace datatools {

  namespace configuration {

    bool variant_repository::registry_entry::is_external() const
    {
      return _external_registry_ != 0;
    }

    bool variant_repository::registry_entry::is_embedded() const
    {
      return _embedded_registry_;
    }

    bool variant_repository::registry_entry::is_valid() const
    {
      return is_external() || is_embedded();
    }

    variant_repository::registry_entry::registry_entry()
    {
      _external_registry_ = 0;
      return;
    }

    void variant_repository::registry_entry::set_external_registry(variant_registry & reg_)
    {
      DT_THROW_IF(is_valid(), std::logic_error, "Entry already has a handle to a variant registry!");
      _external_registry_ = &reg_;
      return;
    }

    const variant_registry & variant_repository::registry_entry::get_registry() const
    {
      variant_registry * vr = 0;
      if (is_external()) {
        vr = _external_registry_;
      } else if (is_embedded()) {
        vr = _embedded_registry_.get();
      } else {
        DT_THROW_IF(true, std::logic_error, "Entry has no handle to a variant registry!");
      }
      return *vr;
    }

    variant_registry & variant_repository::registry_entry::grab_registry()
    {
      variant_registry * vr = 0;
      if (is_external()) {
        vr = _external_registry_;
      } else if (is_embedded()) {
        vr = _embedded_registry_.get();
      } else {
        DT_THROW_IF(true, std::logic_error, "Entry has no handle to a variant registry!");
      }
      return *vr;
    }

    void variant_repository::registry_entry::reset()
    {
      _external_registry_ = 0;
      if (_embedded_registry_) {
        _embedded_registry_.reset();
      }
      if (_embedded_manager_) {
        _embedded_manager_.reset();
      }
      return;
    }

    variant_repository::variant_repository()
    {
      return;
    }

    variant_repository::~variant_repository()
    {
      return;
    }

    bool variant_repository::has_registry(const std::string & registry_name_) const
    {
      registry_dict_type::const_iterator found = _registries_.find(registry_name_);
      return found != _registries_.end();
    }

    const variant_registry &
    variant_repository::get_registry(const std::string & registry_name_) const
    {
      registry_dict_type::const_iterator found = _registries_.find(registry_name_);
      DT_THROW_IF(found == _registries_.end(), std::logic_error,
                  "Repository does not have a registry named '" << registry_name_ << "' !");
      DT_THROW_IF(!found->second.is_valid(), std::logic_error,
                  "Registry named '" << registry_name_ << "' is not valid !");
      return found->second.get_registry();
    }

    variant_registry &
    variant_repository::grab_registry(const std::string & registry_name_)
    {
      registry_dict_type::iterator found = _registries_.find(registry_name_);
      DT_THROW_IF(found == _registries_.end(), std::logic_error,
                  "Repository does not have a registry named '" << registry_name_ << "' !");
      DT_THROW_IF(!found->second.is_valid(), std::logic_error,
                  "Registry named '" << registry_name_ << "' is not valid !");
      return found->second.grab_registry();
    }

    variant_repository::registry_entry &
    variant_repository::_add_entry(const std::string & registry_name_)
    {
      registry_dict_type::const_iterator found = _registries_.find(registry_name_);
      DT_THROW_IF(found != _registries_.end(), std::logic_error,
                  "Repository already has a registry named '" << registry_name_ << "' !");
      {
        registry_entry dummy_entry;
        _registries_[registry_name_] = dummy_entry;
      }
      registry_entry & e = _registries_.find(registry_name_)->second;
      return e;
    }

    const variant_repository::registry_dict_type &
    variant_repository::get_registries() const
    {
      return _registries_;
    }

    variant_repository::registry_dict_type &
    variant_repository::grab_registries()
    {
      return _registries_;
    }

    void variant_repository::reset()
    {
      _registries_.clear();
      return;
    }

    void variant_repository::unregistration(const std::string & registry_key_)
    {
      DT_THROW_IF(!has_registry(registry_key_), std::logic_error,
                  "Repository does not host a registry named '" << registry_key_ << "'!");
      registry_dict_type::iterator found = _registries_.find(registry_key_);
      _registries_.erase(found);
      return;
    }

    void variant_repository::registration_external(variant_registry & external_registry_,
                                                   const std::string & registry_key_)
    {
      std::string registry_name = registry_key_;
      if (registry_name.empty()) {
        if (external_registry_.has_name()) {
          registry_name = external_registry_.get_name();
        }
      }
      DT_THROW_IF(registry_name.empty(), std::logic_error,
                  "Registry name is empty!");
      DT_THROW_IF(has_registry(registry_name), std::logic_error,
                  "Repository already has a registry named '" << registry_name << "'!");
      registry_entry & re = _add_entry(registry_name);
      re.set_external_registry(external_registry_);

      return;
    }

    void variant_repository::registration_embedded(const std::string & registry_manager_config_,
                                                   const std::string & top_variant_name_,
                                                   const std::string & registry_key_,
                                                   const std::string & registry_display_name_,
                                                   const std::string & registry_terse_description_)
    {
      // std::cerr << "DEVEL: Registration of variant : '" << registry_key_ << "'" << std::endl;
      // std::cerr << "DEVEL: Manager configuration file is : '" << registry_manager_config_ << "'" << std::endl;

      // Create a registry factory/manager:
      variant_registry_manager * mgr_ptr = new variant_registry_manager;
      datatools::properties mgr_config;
      std::string  mgr_config_filename = registry_manager_config_;
      datatools::fetch_path_with_env(mgr_config_filename);
      datatools::properties::read_config(mgr_config_filename, mgr_config);
      if (!top_variant_name_.empty()) {
        // Force its top variant name if missing:
        mgr_ptr->set_top_variant_name(top_variant_name_);
      }
      mgr_ptr->initialize(mgr_config);

      std::string registry_name = registry_key_;
      std::string registry_display_name = registry_display_name_;
      std::string registry_terse_description = registry_terse_description_;

      if (registry_name.empty()) {
        registry_name = mgr_ptr->get_name();
      }
      DT_THROW_IF(registry_name.empty(), std::logic_error,
                  "Registry name is empty!");
      DT_THROW_IF(has_registry(registry_name), std::logic_error,
                  "Repository already has a registry named '" << registry_name << "'!");

      if (registry_display_name.empty()) {
        registry_display_name = mgr_ptr->get_display_name();
      }

      if (registry_terse_description.empty()) {
        registry_terse_description = mgr_ptr->get_terse_description();
      }

      registry_entry & re = _add_entry(registry_name);
      re._embedded_manager_.reset(mgr_ptr);
      re._embedded_registry_.reset(new variant_registry);
      variant_registry & vreg = *re._embedded_registry_;
      vreg.set_name(registry_name);
      vreg.set_display_name(registry_display_name);
      vreg.set_terse_description(registry_terse_description);
      vreg.initialize_from(*re._embedded_manager_);

      return;
    }

    bool variant_repository::is_accomplished() const
    {
      for (registry_dict_type::const_iterator i = _registries_.begin();
           i != _registries_.end();
           i++) {
        if (! i->second.get_registry().is_accomplished()) {
          return false;
        }
      }
      return true;
    }

    command::returned_info
    variant_repository::cmd_set_parameter(const std::string & registry_key_,
                                          const std::string & param_path_,
                                          const std::string & param_value_token_)
    {
      command::returned_info cri;
      try {
        cri.set_error_code(command::CEC_SCOPE_INVALID);
        DT_THROW_IF(!has_registry(registry_key_), std::logic_error,
                    "Variant repository has no registry named '" << registry_key_ << "' !");
        variant_registry & vreg = grab_registry(registry_key_);
        cri = vreg.cmd_set_parameter_value(param_path_, param_value_token_);
      }
      catch (std::exception & x) {
        std::string message = "Registry '" + registry_key_ + "' : " + x.what();
        cri.set_error_message(message);
      }
      return cri;
    }

    command::returned_info
    variant_repository::cmd_get_parameter(const std::string & registry_key_,
                                          const std::string & param_path_,
                                          std::string & param_value_token_) const
    {
      command::returned_info cri;
      try {
        cri.set_error_code(command::CEC_SCOPE_INVALID);
        DT_THROW_IF(!has_registry(registry_key_), std::logic_error,
                    "Variant repository has no registry named '" << registry_key_ << "' !");
        const variant_registry & vreg = get_registry(registry_key_);
        cri = vreg.cmd_get_parameter_value(param_path_, param_value_token_);
       }
      catch (std::exception & x) {
        std::string message = "Registry '" + registry_key_ + "' : " + x.what();
        cri.set_error_message(message);
      }
      return cri;
    }

    command::returned_info
    variant_repository::cmd_is_active_variant(const std::string & registry_key_,
                                              const std::string & variant_path_,
                                              bool & active_) const
    {
      active_ = false;
      command::returned_info cri;
      try {
        cri.set_error_code(command::CEC_SCOPE_INVALID);
        DT_THROW_IF(!has_registry(registry_key_), std::logic_error,
                    "Variant repository has no registry named '" << registry_key_ << "' !");
        const variant_registry & vreg = get_registry(registry_key_);
        cri = vreg.cmd_is_active_variant(variant_path_, active_);
      }
      catch (std::exception & x) {
        std::string message = "Registry '" + registry_key_ + "' : " + x.what();
        cri.set_error_message(message);
      }
      return cri;
    }

    void variant_repository::tree_dump(std::ostream & out_,
                                       const std::string & title_,
                                       const std::string & indent_,
                                       bool inherit_) const
    {
      this-> enriched_base::tree_dump(out_, title_, indent_, true);

      out_ << indent_ << i_tree_dumpable::tag << "Registries: " << std::endl;
      for (registry_dict_type::const_iterator i = _registries_.begin();
           i != _registries_.end();
           i++) {
        out_ << indent_ << i_tree_dumpable::skip_tag;
        registry_dict_type::const_iterator j = i;
        std::ostringstream indent_ss;
        indent_ss << indent_ << i_tree_dumpable::skip_tag;
        if (++j == _registries_.end()) {
          out_ << i_tree_dumpable::last_tag;
          indent_ss << i_tree_dumpable::last_skip_tag;
        } else {
          out_ << i_tree_dumpable::tag;
          indent_ss << i_tree_dumpable::skip_tag;
        }
        const std::string & regname = i->first;
        out_ << "Variant registry '" << regname << "' : ";
        const registry_entry & re = i->second;
        if (re.is_external()) {
          out_ << "<external registry>";
        } else if (re.is_embedded()) {
          out_ << "<embedded registry>";
        } else {
          out_ << "<invalid>";
        }
        // const variant_registry & reg = re.get_registry();

        out_ << std::endl;
      }
      out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_) << "End." << std::endl;

      return;
    }

  }  // end of namespace configuration

}  // end of namespace datatools
