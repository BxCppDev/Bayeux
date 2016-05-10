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
#include <datatools/kernel.h>
#include <datatools/exception.h>
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
      return _embedded_registry_.get() != 0;
    }

    bool variant_repository::registry_entry::is_valid() const
    {
      return is_external() || is_embedded();
    }

    variant_repository::registry_entry::registry_entry()
    {
      _rank_ = -1;
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
      _rank_ = -1;
      return;
    }

    void variant_repository::registry_entry::set_rank(int rank_)
    {
      _rank_ = rank_;
      return;
    }

    int variant_repository::registry_entry::get_rank() const
    {
      return _rank_;
    }

    bool variant_repository::registry_entry::has_rank() const
    {
      return _rank_ >= 0;
    }

    bool variant_repository::registry_entry::has_dependencies() const
    {
      return _dependencies_.size() > 0;
    }

    bool variant_repository::registry_entry::depends_on(const std::string & variant_path_) const
    {
      std::list<std::string>::const_iterator found = std::find(_dependencies_.begin(),
                                                               _dependencies_.end(),
                                                               variant_path_);
      if (found != _dependencies_.end()) {
        return true;
      }
      return false;
    }

    void variant_repository::registry_entry::add_dependency(const std::string & variant_path_)
    {
      DT_THROW_IF(depends_on(variant_path_), std::logic_error,
                  "Registry already depends on variant '" << variant_path_ << "'!");
      _dependencies_.push_back(variant_path_);
      return;
    }

    void variant_repository::registry_entry::remove_dependency(const std::string & variant_path_)
    {
      std::list<std::string>::iterator found = std::find(_dependencies_.begin(),
                                                         _dependencies_.end(),
                                                         variant_path_);
      DT_THROW_IF(found == _dependencies_.end(), std::logic_error,
                  "Registry does not depend on variant '" << variant_path_ << "'!");
      _dependencies_.erase(found);
      return;
    }

    const std::list<std::string> & variant_repository::registry_entry::get_dependencies() const
    {
      return _dependencies_;
    }

    // virtual
    bool variant_repository::is_name_valid(const std::string & name_) const
    {
      // std::cerr << "DEVEL: variant_repository::is_name_valid: name = '" << name_ << "'" << std::endl;
      return ::datatools::configuration::validate_instance_name(name_);
    }

    variant_repository::variant_repository()
    {
      _initialized_ = false;
      _locked_ = false;
      return;
    }

    variant_repository::~variant_repository()
    {
      if (is_initialized()) {
        reset();
      }
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
    variant_repository::_add_entry(const std::string & registry_name_, int rank_)
    {
      DT_THROW_IF(!validate_model_name(registry_name_), std::logic_error,
                  "Invalid registry name '" << registry_name_ << "'!");
      registry_dict_type::const_iterator found = _registries_.find(registry_name_);
      DT_THROW_IF(found != _registries_.end(), std::logic_error,
                  "Repository already has a registry named '" << registry_name_ << "' !");
      {
        registry_entry dummy_entry;
        _registries_[registry_name_] = dummy_entry;
      }
      registry_entry & e = _registries_.find(registry_name_)->second;
      if (rank_ >= 0) {
        DT_THROW_IF(_ranked_.find(rank_) != _ranked_.end(),
                    std::logic_error,
                    "Rank [" << rank_ << "] is always used by registry '"
                    << _ranked_.find(rank_)->second << "'!");
        e.set_rank(rank_);
        _ranked_[rank_] = registry_name_;
      } else {
        _unranked_.push_back(registry_name_);
      }
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

    bool variant_repository::is_ranked(const std::string & registry_name_) const
    {
      for (ranked_dict_type::const_iterator i = _ranked_.begin();
           i != _ranked_.end();
           i++) {
        if (i->second == registry_name_) {
          return true;
        }
      }
      return false;
    }

    int variant_repository::get_rank(const std::string & registry_name_) const
    {
       for (ranked_dict_type::const_iterator i = _ranked_.begin();
           i != _ranked_.end();
           i++) {
         if (i->second == registry_name_) {
           return i->first;
         }
       }
       return -1;
    }

    void variant_repository::build_ordered_registry_keys(std::vector<std::string> & keys_) const
    {
      keys_.clear();
      for (ranked_dict_type::const_iterator i = _ranked_.begin();
           i != _ranked_.end();
           i++) {
        keys_.push_back(i->second);
      }
      for (std::vector<std::string>::const_iterator i = _unranked_.begin();
           i != _unranked_.end();
           i++) {
        keys_.push_back(*i);
      }
      return;
    }

    bool variant_repository::has_organization() const
    {
      return ! _organization_.empty();
    }

    const std::string & variant_repository::get_organization() const
    {
      return _organization_;
    }

    void variant_repository::set_organization(const std::string & o_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Repository is locked!");
      DT_THROW_IF(o_.find("/") != std::string::npos, std::logic_error,
                  "Organization name cannot contain the '/' character!");
      _organization_ = o_;
      return;
    }

    bool variant_repository::has_application() const
    {
      return ! _application_.empty();
    }

    const std::string & variant_repository::get_application() const
    {
      return _application_;
    }

    void variant_repository::set_application(const std::string & a_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Repository is locked!");
      DT_THROW_IF(a_.find("/") != std::string::npos, std::logic_error,
                  "Application name cannot contains the '/' character!");
      _application_ = a_;
      return;
    }

    bool variant_repository::is_initialized() const
    {
      return _initialized_;
    }

    void variant_repository::initialize_simple()
    {
      datatools::properties dummy;
      initialize(dummy);
    }

    void variant_repository::_legacy_load_registries(const datatools::properties & config_)
    {
       std::vector<std::string> registry_config_filenames;
       if (config_.has_key("registries.config")) {
          config_.fetch("registries.config", registry_config_filenames);
       }

      // Configure:
      for (size_t i = 0; i < registry_config_filenames.size(); i++) {
        int rank = -1;
        std::string variant_config_registration = registry_config_filenames[i];
        DT_LOG_TRACE(get_logging_priority(),
                     "Variants' configuration : '" << variant_config_registration << "'");
        // Format is :
        //   "path1/subdir/file.conf"
        //   "${SOME_ENV_PATH}/path1/subdir/file.conf"
        //   "@some-label:path1/subdir/file.conf"
        std::string variant_name;
        std::string variant_mgr_config_file;
        /*
        // or:
        //   "foo@path1/subdir/file.conf"
        // This syntax is buggy because it prevents for
        // using the following syntax :
        //  "@foo-label:resource-path-to-some-file"
        size_t apos = variant_config_registration.find('@');
        if (apos != std::string::npos) {
          variant_name = variant_config_registration.substr(0, apos);
          variant_mgr_config_file = variant_config_registration.substr(apos + 1);
        } else {
          variant_mgr_config_file = variant_config_registration;
        }
        */
        variant_mgr_config_file = variant_config_registration;
        datatools::fetch_path_with_env(variant_mgr_config_file);

        // std::string reg_filename = registry_config_filenames[i];
        // datatools::fetch_path_with_env(reg_filename);
        DT_LOG_TRACE(get_logging_priority(),
                     "Variant '" << variant_name << "' configuration file '"
                     << variant_mgr_config_file << "' registration...");
        registration_embedded(variant_mgr_config_file, "", variant_name, "", "", rank);
      }

      return;
    }

    void variant_repository::load_registries(const datatools::properties & config_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Repository is locked!");

      if (config_.has_flag("legacy_load")) {
        // Deprecated:
        DT_LOG_WARNING(datatools::logger::PRIO_WARNING,
                       "Legacy mode for variant registry loading is deprecated!");
       _legacy_load_registries(config_);
      }

      // The set of variant registries to be loaded in the repository:
      std::set<std::string> registry_names;
      if (config_.has_key("registries")) {
        config_.fetch("registries", registry_names, false);
      }

      // Temporary building records:
      std::map<std::string, std::string> registry_configs;
      std::map<std::string, std::string> registry_top_variant_names;
      std::map<std::string, std::string> registry_display_names;
      std::map<std::string, std::string> registry_descriptions;
      std::map<std::string, int> registry_ranks;
      std::map<std::string, std::set<std::string> > registry_dependencies;

      // Parsing:
      {
        std::map<int, std::string> parsed_ranks; // ensure ranks are unique

        // Scan configuration info for all registries:
        for (std::set<std::string>::const_iterator ireg = registry_names.begin();
             ireg != registry_names.end();
             ireg++) {
          const std::string & reg_name = *ireg;

          {
            // Parse variant registry config:
            std::ostringstream reg_config_oss;
            reg_config_oss << "registries." << reg_name << ".config";
            std::string reg_config;
            if (config_.has_key(reg_config_oss.str())) {
              reg_config = config_.fetch_string(reg_config_oss.str());
              DT_THROW_IF(reg_config.empty(), std::logic_error, "Empty configuration filename for registry '" << reg_name << "'!");
            }
            if (!reg_config.empty()) {
              registry_configs[reg_name] = reg_config;
            }
          }

          {
            // Parse variant registry ranks:
            std::ostringstream reg_rank_oss;
            reg_rank_oss << "registries." << reg_name << ".rank";
            if (config_.has_key(reg_rank_oss.str())) {
              int rank = config_.fetch_integer(reg_rank_oss.str());
              DT_THROW_IF(rank < 0,
                          std::range_error,
                          "Rank [" << rank << "] is not valid for registry '" << reg_name << "'!");
              DT_THROW_IF(parsed_ranks.count(rank) != 0,
                          std::range_error,
                          "Rank [" << rank << "] for registry '"
                          << reg_name << "' is already used by registry '"
                          << parsed_ranks.find(rank)->second << "'!");
              parsed_ranks[rank] = reg_name;
              registry_ranks[reg_name] = rank;
            }
          }

          {
            // Parse variant registry display name:
            std::ostringstream reg_display_name_oss;
            reg_display_name_oss << "registries." << reg_name << ".display_name";
            std::string reg_display_name;
            if (config_.has_key(reg_display_name_oss.str())) {
              reg_display_name = config_.fetch_string(reg_display_name_oss.str());
              DT_THROW_IF(reg_display_name.empty(), std::logic_error, "Empty display name for registry '" << reg_name << "'!");
            }
            if (!reg_display_name.empty()) {
              registry_display_names[reg_name] = reg_display_name;
            }
          }

          {
            // Parse variant registry description:
            std::ostringstream reg_description_oss;
            reg_description_oss << "registries." << reg_name << ".description";
            std::string reg_description;
            if (config_.has_key(reg_description_oss.str())) {
              reg_description = config_.fetch_string(reg_description_oss.str());
              DT_THROW_IF(reg_description.empty(), std::logic_error, "Empty description for registry '" << reg_name << "'!");
            }
            if (!reg_description.empty()) {
              registry_descriptions[reg_name] = reg_description;
            }
          }

          {
            // Parse variant registry top variant name:
            std::ostringstream reg_top_variant_oss;
            reg_top_variant_oss << "registries." << reg_name << ".top_variant";
            std::string reg_top_variant_name;
            if (config_.has_key(reg_top_variant_oss.str())) {
              reg_top_variant_name = config_.fetch_string(reg_top_variant_oss.str());
              DT_THROW_IF(reg_top_variant_name.empty(), std::logic_error, "Empty top variant name for registry '" << reg_name << "'!");
            }
            if (!reg_top_variant_name.empty()) {
              registry_top_variant_names[reg_name] = reg_top_variant_name;
            }
          }

          {
            // Parse variant registry dependencies:
            std::ostringstream reg_dependencies_oss;
            reg_dependencies_oss << "registries." << reg_name << ".dependencies";
            if (config_.has_key(reg_dependencies_oss.str())) {
              std::set<std::string> reg_dependencies;
              config_.fetch(reg_dependencies_oss.str(), reg_dependencies, false);
              if (reg_dependencies.size()) {
                registry_dependencies[reg_name] = reg_dependencies;
              }
            }
          }

        }
      }

      // Registration and configuration of all embedded registries:
      for (std::set<std::string>::const_iterator ireg = registry_names.begin();
           ireg != registry_names.end();
           ireg++) {
        const std::string & the_registry_name = *ireg;
        std::string the_registry_top_variant_name;
        std::string the_registry_display_name;
        std::string the_registry_terse_description;
        std::string the_registry_variant_mgr_config_file;
        int the_registry_rank = -1;

        // Extract specific informations from temporary records:
        the_registry_variant_mgr_config_file = registry_configs.find(the_registry_name)->second;
        datatools::fetch_path_with_env(the_registry_variant_mgr_config_file);
        if (registry_top_variant_names.count(the_registry_name)) {
          the_registry_top_variant_name = registry_top_variant_names.find(the_registry_name)->second;
        }
        if (registry_display_names.count(the_registry_name)) {
          the_registry_display_name = registry_display_names.find(the_registry_name)->second;
        }
        if (registry_descriptions.count(the_registry_name)) {
          the_registry_terse_description = registry_descriptions.find(the_registry_name)->second;
        }
        if (registry_ranks.count(the_registry_name)) {
          the_registry_rank = registry_ranks.find(the_registry_name)->second;
        }
        DT_LOG_DEBUG(get_logging_priority(),
                     "Register variant registry '" << the_registry_name << "' from configuration file '"
                     << the_registry_variant_mgr_config_file << "' registration...");
        this->registration_embedded(the_registry_variant_mgr_config_file,
                                    the_registry_top_variant_name,
                                    the_registry_name,
                                    the_registry_display_name,
                                    the_registry_terse_description,
                                    the_registry_rank);
      }

      // Specify dependencies:
      for (std::map<std::string, std::set<std::string> >::const_iterator ireg = registry_dependencies.begin();
           ireg != registry_dependencies.end();
           ireg++) {
        const std::string & the_registry_name = ireg->first;
        const std::set<std::string> & the_registry_dependencies = ireg->second;
        for (std::set<std::string>::const_iterator idep = the_registry_dependencies.begin();
             idep != the_registry_dependencies.end();
             idep++) {
          const std::string & variant_path = *idep;
          DT_LOG_DEBUG(get_logging_priority(),
                       "Adding dependency '" << variant_path << "' to variant registry '" << the_registry_name << "'...");
          this->add_registry_dependency(the_registry_name, variant_path);
        }
      }

      return;
    }

    void variant_repository::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Repository '" << get_name() << "' is already initialized!");

      bool requested_lock = false;

      this->enriched_base::initialize(config_, false);

      // Parse configuration parameters:
      if (!has_organization()) {
        if (config_.has_key("organization")) {
          const std::string & org = config_.fetch_string("organization");
          set_organization(org);
        }
      }

      if (!has_application()) {
        if (config_.has_key("application")) {
          const std::string & app = config_.fetch_string("application");
          set_application(app);
        }
      }

      if (config_.has_flag("lock")) {
        requested_lock = true;
      }

      load_registries(config_);

      if (requested_lock) {
        lock();
      }

      _initialized_ = true;
      return;
    }

    void variant_repository::reset()
    {
      DT_THROW_IF(! is_initialized(), std::logic_error, "Repository is not initialized!");
      _initialized_ = false;
      if (is_locked()) {
        unlock();
      }
      clear_registries();
      _application_.clear();
      _organization_.clear();
      return;
    }

    void variant_repository::clear_registries()
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Repository is locked!");
      _registries_.clear();
      return;
    }

    void variant_repository::external_registries_unregistration()
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Repository is locked!");
      //_registries_.clear();
      std::vector<std::string> external_registry_keys;
      for (registry_dict_type::const_iterator i = _registries_.begin();
           i != _registries_.end();
           i++) {
        if (i->second.is_external()) {
          external_registry_keys.push_back(i->first);
        }
      }
      for (std::vector<std::string>::const_iterator j = external_registry_keys.begin();
           j != external_registry_keys.end();
           j++) {
        _registries_.erase(*j);
      }
      return;
    }

    void variant_repository::unregistration(const std::string & registry_key_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Repository is locked!");
      DT_THROW_IF(!has_registry(registry_key_), std::logic_error,
                  "Repository does not host a registry named '" << registry_key_ << "'!");
      registry_dict_type::iterator found = _registries_.find(registry_key_);
      _registries_.erase(found);
      return;
    }

    void variant_repository::registration_external(variant_registry & external_registry_,
                                                   const std::string & registry_key_,
                                                   int rank_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Repository is locked!");
      std::string registry_name = registry_key_;
      if (registry_name.empty()) {
        if (external_registry_.has_name()) {
          registry_name = external_registry_.get_name();
        }
      }
      DT_THROW_IF(!validate_instance_name(registry_name), std::logic_error,
                  "Invalid registry name!");
      DT_THROW_IF(has_registry(registry_name), std::logic_error,
                  "Repository already has a registry named '" << registry_name << "'!");
      registry_entry & re = _add_entry(registry_name, rank_);
      re.set_external_registry(external_registry_);
      return;
    }

    void variant_repository::registration_embedded(const std::string & registry_manager_config_,
                                                   const std::string & top_variant_name_,
                                                   const std::string & registry_key_,
                                                   const std::string & registry_display_name_,
                                                   const std::string & registry_terse_description_,
                                                   int rank_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Repository is locked!");
      // std::cerr << "DEVEL: Registration of variant : '" << registry_key_ << "'" << std::endl;
      // std::cerr << "DEVEL: Manager configuration file is : '" << registry_manager_config_ << "'" << std::endl;

      // Create a registry factory/manager:
      variant_registry_manager * mgr_ptr = new variant_registry_manager;
      datatools::properties mgr_config;
      std::string mgr_config_filename = registry_manager_config_;
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
      DT_THROW_IF(!validate_instance_name(registry_name), std::logic_error,
                  "Invalid registry name!");
      DT_THROW_IF(has_registry(registry_name), std::logic_error,
                  "Repository already has a registry named '" << registry_name << "'!");

      if (registry_display_name.empty()) {
        registry_display_name = mgr_ptr->get_display_name();
      }

      if (registry_terse_description.empty()) {
        registry_terse_description = mgr_ptr->get_terse_description();
      }

      registry_entry & re = _add_entry(registry_name, rank_);
      re._embedded_manager_.reset(mgr_ptr);
      re._embedded_registry_.reset(new variant_registry);
      variant_registry & vreg = *re._embedded_registry_;
      vreg.set_name(registry_name);
      vreg.set_display_name(registry_display_name);
      vreg.set_terse_description(registry_terse_description);
      vreg.initialize_from(*re._embedded_manager_);

      return;
    }

    void variant_repository::add_registry_dependency(const std::string & registry_name_,
                                                     const std::string & dependee_variant_path_)
    {
      registry_dict_type::iterator found = _registries_.find(registry_name_);
      DT_THROW_IF(found == _registries_.end(),
                  std::logic_error,
                  "Registry '" << registry_name_ << "' does not exist!");
      registry_entry & re = found->second;
      DT_THROW_IF(re.depends_on(dependee_variant_path_),
                  std::logic_error,
                  "Registry '" << registry_name_ << "' already depends on variant '"
                  << dependee_variant_path_ << "'!");
      int found_colon = dependee_variant_path_.find(':');
      DT_THROW_IF(found_colon == dependee_variant_path_.npos,
                  std::logic_error,
                  "Bad variant path format!");
      std::string dependee_registry = dependee_variant_path_.substr(0, found_colon);
      // std::cerr << "DEVEL: dependee registry = '" << dependee_registry << "'" << std::endl;
      std::string local_variant_path = dependee_variant_path_.substr(found_colon + 1);
      // std::cerr << "DEVEL: dependee registry local variant path = '" << local_variant_path << "'" << std::endl;
      if (!get_registry(dependee_registry).has_variant_record(local_variant_path)) {
        DT_THROW(std::logic_error,
                 "Dependee variant path '" << dependee_variant_path_ << "' does not exist!");
      }
      re.add_dependency(dependee_variant_path_);
      return;
    }

    void variant_repository::remove_registry_dependency(const std::string & registry_name_,
                                                        const std::string & variant_path_)
    {
      registry_dict_type::iterator found = _registries_.find(registry_name_);
      DT_THROW_IF(found == _registries_.end(),
                  std::logic_error,
                  "Registry '" << registry_name_ << "' does not exist!");
      registry_entry & re = found->second;
      DT_THROW_IF(re.depends_on(variant_path_),
                  std::logic_error,
                  "Registry '" << registry_name_ << "' does not depends on variant '"
                  << variant_path_ << "'!");
      re.remove_dependency(variant_path_);
      return;
    }

    bool variant_repository::has_registry_dependency(const std::string & registry_name_,
                                                     const std::string & variant_path_) const
    {
      registry_dict_type::const_iterator found = _registries_.find(registry_name_);
      DT_THROW_IF(found == _registries_.end(),
                  std::logic_error,
                  "Registry '" << registry_name_ << "' does not exist!");
      const registry_entry & re = found->second;
      if (re.depends_on(variant_path_)) {
        return true;
      }
      return false;
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

    bool variant_repository::is_locked() const
    {
      return _locked_;
    }

    void variant_repository::lock()
    {
      _locked_ = true;
      return;
    }

    void variant_repository::unlock()
    {
      _locked_ = false;
      return;
    }

    command::returned_info
    variant_repository::cmd_set_parameter(const std::string & registry_key_,
                                          const std::string & param_path_,
                                          const std::string & param_value_token_)
    {
      command::returned_info cri;
      try {
        cri.set_error_code(command::CEC_SCOPE_INVALID);
        bool registry_active = is_active_registry(registry_key_);
        if (! registry_active) {
          cri.set_error_code(command::CEC_COMMAND_INVALID_CONTEXT);
          std::string message = "Registry '" + registry_key_ + "' is not active";
          cri.set_error_message(message);
        } else {
          variant_registry & vreg = grab_registry(registry_key_);
          cri = vreg.cmd_set_parameter_value(param_path_, param_value_token_);
        }
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
      } catch (std::exception & x) {
        std::string message = "Registry '" + registry_key_ + "' : " + x.what();
        cri.set_error_message(message);
      }
      return cri;
    }

    bool variant_repository::is_active_registry(const std::string & registry_key_) const
    {
      registry_dict_type::const_iterator found = _registries_.find(registry_key_);
      DT_THROW_IF(found == _registries_.end(), std::logic_error,
                  "Variant repository has no registry named '" << registry_key_ << "' !");
      const registry_entry & re = found->second;
      for (std::list<std::string>::const_iterator iter = re._dependencies_.begin();
           iter !=  re._dependencies_.end();
           iter++) {
        const std::string & variant_path = *iter;
        int found_colon = variant_path.find(':');
        std::string dependee_registry = variant_path.substr(0, found_colon);
        // std::cerr << "DEVEL: dependee registry = '" << dependee_registry << "'" << std::endl;
        std::string local_variant_path = variant_path.substr(found_colon + 1);
        // std::cerr << "DEVEL: dependee registry local variant path = '" << local_variant_path << "'" << std::endl;
        bool variant_active = false;
        command::returned_info cri2 = cmd_is_active_variant(dependee_registry,
                                                            local_variant_path,
                                                            variant_active);
        if (!variant_active) {
          return false;
        }
      }
      return true;
    }

    command::returned_info
    variant_repository::cmd_is_active_registry(const std::string & registry_key_,
                                               bool & active_) const
    {
      active_ = false;
      command::returned_info cri;
      try {
        cri.set_error_code(command::CEC_SCOPE_INVALID);
        bool active_registry = is_active_registry(registry_key_);
        active_ = active_registry;
        cri.set_success();
      } catch (std::exception & x) {
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

      out_ << indent_ << i_tree_dumpable::tag << "Organization: ";
      if (has_organization()) {
        out_ << "'" << get_organization() << "'";
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;

      out_ << indent_ << i_tree_dumpable::tag << "Application: ";
      if (has_application()) {
        out_ << "'" << get_application() << "'";
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;

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

      out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_) << "Locked: ";
      if (is_locked()) {
        out_ << "yes";
      } else {
        out_ << "no";
      }
      out_ << std::endl;

      return;
    }

    void variant_repository::system_export(uint32_t /* flags_ */)
    {
      DT_THROW_IF(! datatools::kernel::is_instantiated(), std::logic_error,
                  "Kernel is not instantiated!");
      datatools::kernel & krnl = kernel::instance();
      krnl.import_configuration_repository(*this);
      return;
    }

    void variant_repository::system_discard() const
    {
      DT_THROW_IF(! datatools::kernel::is_instantiated(), std::logic_error,
                  "Kernel is not instantiated!");
      datatools::kernel & krnl = kernel::instance();
      for (registry_dict_type::const_iterator i = _registries_.begin();
           i != _registries_.end();
           i++) {
        const std::string & registry_name = i->first;
        if (krnl.has_external_configuration_registry(registry_name)) {
          krnl.clear_configuration_registry(registry_name);
        }
      }
      return;
    }

  }  // end of namespace configuration

}  // end of namespace datatools
