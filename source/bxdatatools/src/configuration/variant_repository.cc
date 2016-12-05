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

// Standard library:
#include <iomanip>

// Third party:
// - Boost:
#include <boost/algorithm/string.hpp>

// This project (Bayeux/datatools):
#include <datatools/kernel.h>
#include <datatools/exception.h>
#include <datatools/ioutils.h>
#include <datatools/configuration/variant_registry.h>
#include <datatools/configuration/variant_registry_manager.h>
#include <datatools/configuration/variant_record.h>
#include <datatools/configuration/parameter_model.h>
#include <datatools/configuration/variant_model.h>
#include <datatools/configuration/io.h>

namespace datatools {

  namespace configuration {

    void variant_repository::registry_entry::set_name(const std::string & name_)
    {
      _name_ = name_;
      return;
    }

    const std::string & variant_repository::registry_entry::get_name() const
    {
      return _name_;
    }

    bool variant_repository::registry_entry::is_external() const
    {
      return _external_registry_ != nullptr;
    }

    bool variant_repository::registry_entry::is_embedded() const
    {
      return _embedded_registry_.get() != nullptr;
    }

    bool variant_repository::registry_entry::is_valid() const
    {
      return !_name_.empty() && (is_external() || is_embedded());
    }

    variant_repository::registry_entry::registry_entry(variant_repository & vrep_)
      : _parent_repository_(&vrep_)
    {
      _rank_ = -1;
      _external_registry_ = nullptr;
      // _external_backup_repository_ = nullptr;
      return;
    }

    variant_repository::registry_entry::~registry_entry()
    {
      if (is_embedded()) {
        _embedded_registry_.reset();
      }
      if (is_external()) {
        reset_external_registry();
        if (_embedded_manager_.get() != nullptr) {
          _embedded_manager_.reset();
        }
      }
      return;
    }

    const variant_repository & variant_repository::registry_entry::get_parent_repository() const
    {
      return *_parent_repository_;
    }

    void variant_repository::registry_entry::set_external_registry(variant_registry & reg_)
    {
      DT_THROW_IF(is_valid(), std::logic_error, "Entry already has a handle to a variant registry!");
      _external_registry_ = &reg_;
      DT_THROW_IF(reg_.is_mounted(), std::logic_error,
                  "Cannot mount a registry already mounted in another repository!");
      // if (_external_registry_->has_parent_repository()) {
      //   _external_backup_repository_ = &_external_registry_->get_parent_repository();
      // }
      _external_registry_->set_parent_repository(*_parent_repository_, _name_);
      return;
    }

    void variant_repository::registry_entry::reset_external_registry()
    {
      if (is_external()) {
        // if (_external_backup_repository_ != nullptr) {
        //   _external_registry_->set_parent_repository(*_external_backup_repository_);
        //   _external_backup_repository_ = nullptr;
        // } else {
        _external_registry_->reset_parent_repository();
        // }
      }
      return;
    }

    const variant_registry & variant_repository::registry_entry::get_registry() const
    {
      variant_registry * vr = nullptr;
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
      variant_registry * vr = nullptr;
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
      _external_registry_ = nullptr;
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

    // virtual
    bool variant_repository::is_name_valid(const std::string & name_) const
    {
      return ::datatools::configuration::validate_instance_name(name_);
    }

    variant_repository::variant_repository()
    {
      _initialized_ = false;
      _locked_ = false;
      _reporting_ = nullptr;
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

    unsigned int variant_repository::get_number_of_registries() const
    {
      return _registries_.size();
    }

    void variant_repository::list_of_registry_keys(std::set<std::string> & keys_) const
    {
      keys_.clear();
      for (registry_dict_type::const_iterator i = _registries_.begin();
           i != _registries_.end();
           i++) {
        keys_.insert(i->first);
      }
      return;
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

    bool variant_repository::rank_is_used(int rank_) const
    {
      if (rank_ < 0) return false;
      return _ranked_.find(rank_) != _ranked_.end();
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
        registry_entry dummy_entry(*this);
        dummy_entry.set_name(registry_name_);
        _registries_.insert(std::pair<std::string, registry_entry>(registry_name_, dummy_entry));
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

    bool variant_repository::has_reporting() const
    {
      return _reporting_ != nullptr;
    }

    void variant_repository::set_reporting(variant_reporting & rpt_)
    {
      DT_THROW_IF(has_reporting(), std::logic_error, "Repository already has an attached reporting object!");
      _reporting_ = &rpt_;
      return;
    }

    void variant_repository::reset_reporting()
    {
      DT_THROW_IF(!has_reporting(), std::logic_error, "Repository has no attached reporting object!");
      _reporting_ = nullptr;
      return;
    }

    variant_reporting & variant_repository::grab_reporting()
    {
      DT_THROW_IF(!has_reporting(), std::logic_error, "Repository has no attached reporting object!");
      return *_reporting_;
    }

    bool variant_repository::has_dependency_model() const
    {
      return _dependency_model_.get() != nullptr;
    }

    const variant_dependency_model & variant_repository::get_dependency_model() const
    {
      DT_THROW_IF(_dependency_model_.get() == nullptr,
                  std::logic_error,
                  "Repository '" << get_name() << "' has no dependency model!");
      return *_dependency_model_.get();
    }

    variant_repository::importer::importer(const datatools::properties & config_, uint32_t flags_)
      : _config_(config_)
    {
      if (flags_ & IMPORT_DEBUG) {
        _logging_ = datatools::logger::PRIO_DEBUG;
      }
      if (flags_ & IMPORT_NOCLEAR) {
        _noclear_ = true;
      }
      if (flags_ & IMPORT_DONT_IGNORE_UNKNOWN_REGISTRY) {
        _dont_ignore_unknown_registries_ = true;
      }
      if (flags_ & IMPORT_DONT_IGNORE_UNKNOWN_ORGAPP) {
        _dont_ignore_unknown_orgapp_ = true;
      }
      return;
    }

    void variant_repository::importer::operator()(variant_repository & vrep_)
    {
      if (vrep_.is_locked()) {
        if (!_noclear_) {
          DT_LOG_DEBUG(_logging_, "Unlock the working variant repository");
          vrep_.unlock();
        }
      }

      if (_config_.has_key("organization")) {
        std::string imported_org = _config_.fetch_string("organization");
        boost::trim(imported_org);
        if (!imported_org.empty()) {
          if (vrep_.has_organization()) {
            DT_THROW_IF(_dont_ignore_unknown_orgapp_ && imported_org != vrep_.get_organization(),
                        std::logic_error,
                        "Non matching organization (imported='" << imported_org << "' vs requested='"
                        << vrep_.get_organization() << "'!");
          } else {
            vrep_.set_organization(imported_org);
          }
        }
      }

      if (_config_.has_key("application")) {
        std::string imported_app = _config_.fetch_string("application");
        boost::trim(imported_app);
        if (!imported_app.empty()) {
          if (vrep_.has_application()) {
            DT_THROW_IF(_dont_ignore_unknown_orgapp_ && imported_app != vrep_.get_application(),
                        std::logic_error,
                        "Non matching application (imported='" << imported_app << "' vs requested='"
                        << vrep_.get_application() << "'!");
          } else {
            vrep_.set_application(imported_app);
          }
        }
      }

      if (_config_.has_key("settings")) {
        _config_.fetch("settings", _settings_);
      }
      while (_settings_.size()) {
        std::string setting_item = _settings_.front();
        DT_LOG_DEBUG(_logging_, "Fetching the first setting rule '" << setting_item << "'...");
        // unsigned int equal_pos = setting_repr.find('=');
        // DT_THROW_IF(equal_pos < 1 && equal_pos >= setting_repr.length(),
        //          std::logic_error,
        //          "Invalid format for variant parameter directive '" << setting_repr << "'!");
        // std::string value_repr = setting_repr.substr(equal_pos + 1);
        // std::string record_path = setting_repr.substr(0, equal_pos);
        unsigned int colon_pos = setting_item.find(':');
        DT_THROW_IF(colon_pos < 1 && colon_pos >= setting_item.length(),
                    std::logic_error,
                    "Invalid format for variant parameter setting '" << setting_item << "'!");
        std::string registry_name = setting_item.substr(0, colon_pos);
        boost::trim(registry_name);
        if (registry_name != _current_registry_name_) {
          DT_LOG_WARNING(_logging_,
                         "Switching to registry '" << registry_name << "'...");
          if (!vrep_.has_registry(registry_name)) {
            if (_dont_ignore_unknown_registries_) {
              DT_THROW(std::logic_error,
                       "Variant repository has no known registry named '" << registry_name << "'!");
            }
            DT_LOG_WARNING(_logging_,
                           "Variant repository has no known registry named '" << registry_name << "'!");
            _current_registry_name_.clear();
          } else {
            DT_LOG_WARNING(_logging_,
                           "Processing registry '" << registry_name << "'...");
            _current_registry_name_ = registry_name;
            variant_registry & current_registry = vrep_.grab_registry(_current_registry_name_);
            _process_registry(current_registry);
          }
        }
      }
      return;
    }

    void variant_repository::importer::_process_registry(variant_registry & vreg_)
    {
      std::vector<variant_record *> top_records;
      for (variant_registry::record_dict_type::iterator i = vreg_.grab_records().begin();
           i != vreg_.grab_records().end();
           i++) {
        variant_record & rec = i->second;
        if (!rec.has_parent()) {
          top_records.push_back(&rec);
        }
      }
      for (std::size_t i = 0; i < top_records.size(); i++) {
        DT_LOG_DEBUG(_logging_,
                     "Processing top record '" << top_records[i]->get_path()
                     << "' in registry '" << _current_registry_name_ << "'...");
        _process_record(*top_records[i]);
      }
      return;
    }

    void variant_repository::importer::_process_record(variant_record & vrec_)
    {
      if (vrec_.is_active()) {

        if (vrec_.is_parameter() && vrec_.get_parameter_model().is_variable()) {
          DT_LOG_DEBUG(_logging_,
                       "Processing parameter '" << vrec_.get_path()
                       << "' in registry '" << _current_registry_name_ << "'...");
          std::string setting_repr = _settings_.front();
          unsigned int equal_pos = setting_repr.find('=');
          DT_THROW_IF(equal_pos < 1 && equal_pos >= setting_repr.length(),
                      std::logic_error,
                      "Invalid format for variant parameter setting directive '" << setting_repr << "'!");
          std::string record_full_path = setting_repr.substr(0, equal_pos);
          unsigned int colon_pos = record_full_path.find(':');
          std::string registry_name = record_full_path.substr(0, colon_pos);
          if (registry_name != _current_registry_name_) {
            DT_LOG_DEBUG(_logging_,
                         "Leaving registry '" << _current_registry_name_ << "'...");
            _current_registry_name_.clear();
            return;
          }
          _settings_.erase(_settings_.begin());
          std::string record_path = record_full_path.substr(colon_pos + 1);
          std::string value_str = setting_repr.substr(equal_pos + 1);
          if (vrec_.get_parameter_model().is_string()) {
            datatools::add_quotes(value_str, '"');
          }
          command::returned_info cri;
          if (value_str == ascii_io::unset_label()) {
            cri = vrec_.unset_value();
          } else {
            cri = vrec_.string_to_value(value_str);
            DT_THROW_IF(! cri.is_success(),
                        std::logic_error,
                        "Failed to convert variant parameter '" << vrec_.get_name()
                        << "' from '" << value_str << "' : "
                        << cri.get_error_message());
          }
          if (cri.is_success()) {
           // Traverse the daughter variants:
           for (variant_record::daughter_dict_type::iterator i = vrec_.grab_daughters().begin();
                 i != vrec_.grab_daughters().end();
                 i++) {
              variant_record * rec = i->second;
              _process_record(*rec);
            }
          }
        }

        if (vrec_.is_variant()) {
          DT_LOG_DEBUG(_logging_,
                       "Processing variant '" << vrec_.get_path()
                       << "' in registry '" << _current_registry_name_ << "'...");
          // Traverse the daughter parameters:
          for (variant_record::daughter_dict_type::iterator i = vrec_.grab_daughters().begin();
               i != vrec_.grab_daughters().end();
               i++) {
            DT_LOG_DEBUG(_logging_,
                         "Processing daughter '" << i->first << "' in variant '" << vrec_.get_path()
                         << "' in registry '" << _current_registry_name_ << "'...");
            variant_record * rec = i->second;
            _process_record(*rec);
          }
        }
      }
      return;
    }

    variant_repository::exporter::exporter(datatools::properties & config_, uint32_t flags_)
      : _config_(config_)
    {
      if (flags_ & EXPORT_DEBUG) {
        _logging_ = datatools::logger::PRIO_DEBUG;
      }
      if (flags_ & EXPORT_NOCLEAR) {
        _noclear_ = true;
      }
      return;
    }

    void variant_repository::exporter::operator()(const variant_repository & vrep_)
    {
      if (!_noclear_) {
        _config_.clear();
      }
      std::vector<std::string> params_settings;
      std::vector<std::string> registry_keys;
      vrep_.build_ordered_registry_keys(registry_keys);
      for (unsigned int ireg = 0; ireg < registry_keys.size(); ireg++) {
        const std::string & reg_name = registry_keys[ireg];
        if (!vrep_.is_active_registry(reg_name)) {
          continue;
        }
        _current_registry_name_ = reg_name;
        _process_registry(vrep_.get_registry(reg_name));
      }
      if (vrep_.has_organization()) {
        _config_.store("organization", vrep_.get_organization());
      }
      if (vrep_.has_application()) {
        _config_.store("application", vrep_.get_application());
      }
      _config_.store("settings", _settings_);
      return;
    }

    void variant_repository::exporter::_process_record(const variant_record & vrec_)
    {
      if (vrec_.is_active()) {
        if (vrec_.is_parameter() && vrec_.get_parameter_model().is_variable()) {
          DT_LOG_DEBUG(_logging_,
                       "Processing parameter '" << vrec_.get_path()
                       << "' in registry '" << _current_registry_name_ << "'...");
          // Parameter setting:
          std::ostringstream setting_oss;
          setting_oss << _current_registry_name_ << ':';
          setting_oss << vrec_.get_path() << '=';
          std::string value_str;
          command::returned_info cri = vrec_.value_to_string(value_str);
          DT_LOG_DEBUG(_logging_,
                       "Variant parameter '" << vrec_.get_path() << "' value is '"
                       << value_str << "'"
                       << " in registry '" << _current_registry_name_ << "'...");
          if (vrec_.get_parameter_model().is_string()) {
            datatools::remove_quotes(value_str, '"');
          }
          if (cri.is_success()) {
            // Publish the value:
            setting_oss << value_str;
            _settings_.push_back(setting_oss.str());
            for (variant_record::daughter_dict_type::const_iterator i = vrec_.get_daughters().begin();
                 i != vrec_.get_daughters().end();
                 i++) {
              const variant_record * rec = i->second;
              _process_record(*rec);
            }
          } else {
            // Publish the unset value:
            setting_oss << ascii_io::unset_label();
            _settings_.push_back(setting_oss.str());
          }
        }
        if (vrec_.is_variant()) {
          // Variant:
          DT_LOG_DEBUG(_logging_,
                       "Processing variant '" << vrec_.get_path()
                       << "' in registry '" << _current_registry_name_ << "'...");
          // Traverse the daughter parameters:
          for (variant_record::daughter_dict_type::const_iterator i = vrec_.get_daughters().begin();
               i != vrec_.get_daughters().end();
               i++) {
            DT_LOG_DEBUG(_logging_,
                         "Processing daughter '" << i->first << "' in variant '" << vrec_.get_path()
                         << "' in registry '" << _current_registry_name_ << "'...");
            const variant_record * rec = i->second;
            _process_record(*rec);
          }
        }
      }
      return;
    }

    void variant_repository::exporter::_process_registry(const variant_registry & vreg_)
    {
      std::vector<const variant_record *> top_records;
      for (variant_registry::record_dict_type::const_iterator i = vreg_.get_records().begin();
           i != vreg_.get_records().end();
           i++) {
        const variant_record & rec = i->second;
        if (!rec.has_parent()) {
          top_records.push_back(&rec);
        }
      }
      for (size_t i = 0; i < top_records.size(); i++) {
        DT_LOG_DEBUG(_logging_,
                     "Processing top record '" << top_records[i]->get_path()
                     << "' in registry '" << _current_registry_name_ << "'...");
        _process_record(*top_records[i]);
      }
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

    void variant_repository::load_global_dependency_model(const datatools::properties & gdm_config_)
    {
      DT_THROW_IF(is_locked(), std::logic_error, "Repository is locked!");
      _dependency_model_.reset(new variant_dependency_model(*this));
      _dependency_model_->initialize(gdm_config_);
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

          // {
          //   // Parse variant registry dependencies:
          //   std::ostringstream reg_dependencies_oss;
          //   reg_dependencies_oss << "registries." << reg_name << ".dependencies";
          //   if (config_.has_key(reg_dependencies_oss.str())) {
          //     std::set<std::string> reg_dependencies;
          //     config_.fetch(reg_dependencies_oss.str(), reg_dependencies, false);
          //     if (reg_dependencies.size()) {
          //       registry_dependencies[reg_name] = reg_dependencies;
          //     }
          //   }
          // }

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
                     "Registration of variant registry '" << the_registry_name << "' from configuration file '"
                     << the_registry_variant_mgr_config_file << "'...");
        this->registration_embedded(the_registry_variant_mgr_config_file,
                                    the_registry_top_variant_name,
                                    the_registry_name,
                                    the_registry_display_name,
                                    the_registry_terse_description,
                                    the_registry_rank);
      }

      // // New dependency model:
      // {
      //   std::set<unsigned int> slots;
      //   if (config_.has_key("dependees.slots")) {
      //     config_.fetch_positive("dependees.slots", slots);
      //   } else if (config_.has_key("dependees.number_of_slots")) {
      //     unsigned int noslots = config_.fetch_positive_integer("dependees.number_of_slots");
      //     for (unsigned int islot = 0; islot < noslots; islot++) {
      //       slots.insert(islot);
      //     }
      //   }
      //   std::map<unsigned int, std::string> dependee_slots;
      //   for (auto islot : slots) {
      //     std::ostringstream slot_variant_key_oss;
      //     slot_variant_key_oss << "dependees.slot_" << islot << ".variant";
      //     std::string slot_variant_key = slot_variant_key_oss.str();
      //     DT_THROW_IF(!config_.has_key(slot_variant_key), std::logic_error,
      //                 "Missing '" << slot_variant_key << "' property!");
      //     std::string dependee_variant = config_.fetch_string(slot_variant_key);
      //     dependee_slots[islot] = dependee_variant;
      //   }
      // }

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

      // Load a global dependency model:
      {
        datatools::properties gdm_config;
        if (config_.has_key("load_global_dependency_model")) {
          // Load global dependency model from a file:
          std::string dependencies_def_file = config_.fetch_string("load_global_dependency_model");
          datatools::fetch_path_with_env(dependencies_def_file);
          datatools::properties gdm_config;
          gdm_config.read_configuration(dependencies_def_file);
        } else {
          config_.export_and_rename_starting_with(gdm_config, "gdm.", "");
        }
        if (gdm_config.size()) {
          load_global_dependency_model(gdm_config);
        }
      }

      if (requested_lock) {
        lock();
      }

      bool has_warnings = false;
      if (!has_name()) {
        has_warnings = true;
        DT_LOG_WARNING(get_logging_priority(), "Repository has no name!");
      }

      if (!has_organization()) {
        has_warnings = true;
        DT_LOG_WARNING(get_logging_priority(), "Repository '" << get_name() << "' has no organization set!");
      }

      if (!has_application()) {
        has_warnings = true;
        DT_LOG_WARNING(get_logging_priority(), "Repository '" << get_name() << "' has no application set!");
      }

      if (!get_number_of_registries()) {
        has_warnings = true;
        DT_LOG_WARNING(get_logging_priority(), "Repository '" << get_name() << "' has no variant registry !");
      }

      if (!is_locked()) {
        has_warnings = true;
        DT_LOG_WARNING(get_logging_priority(), "Repository '" << get_name() << "' is not locked!");
      }

      if (has_warnings) {
        DT_LOG_WARNING(get_logging_priority(), "You configuration setup (file/multi_properties) for variant repository '" << get_name()
                       << "' seems not to be valid!");
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
      if (_dependency_model_.get() == nullptr) {
        _dependency_model_.reset();
      }
      if (has_reporting()) {
        reset_reporting();
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
      DT_THROW_IF(external_registry_.has_parent_repository(), std::logic_error,
                  "Registry named '" << registry_name << "' already has a parent repository!");
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
      std::shared_ptr<variant_registry_manager> mgr_ptr(new variant_registry_manager);
      // variant_registry_manager * mgr_ptr = new variant_registry_manager;
      datatools::properties mgr_config;
      std::string mgr_config_filename = registry_manager_config_;
      datatools::fetch_path_with_env(mgr_config_filename);
      uint32_t options = 0;
      // options = datatools::properties::config::RESOLVE_PATH;
      options = datatools::properties::config::FORBID_VARIANTS;
      datatools::properties::read_config(mgr_config_filename, mgr_config, options);
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
      re._embedded_manager_ = mgr_ptr;
      re._embedded_registry_.reset(new variant_registry);
      variant_registry & vreg = *re._embedded_registry_;
      vreg.set_logging_priority(re._embedded_manager_->get_logging_priority());
      vreg.set_name(registry_name);
      vreg.set_display_name(registry_display_name);
      vreg.set_terse_description(registry_terse_description);
      vreg.set_parent_repository(*this, registry_name);
      vreg.initialize_from(*re._embedded_manager_);

      // Load local dependency model:
      {
        DT_LOG_DEBUG(get_logging_priority(), "Attempt to load a local dependency model for registry '" << registry_name << "'...");
        datatools::properties ldm_config;
        if (mgr_config.has_key("load_local_dependency_model")) {
          // Load local dependency model from a file:
          std::string dependencies_def_file = mgr_config.fetch_string("load_local_dependency_model");
          datatools::fetch_path_with_env(dependencies_def_file);
          datatools::properties ldm_config;
          ldm_config.read_configuration(dependencies_def_file);
        } else {
          mgr_config.export_and_rename_starting_with(ldm_config, "ldm.", "");
        }
        if (ldm_config.size()) {
          vreg.load_local_dependency_model(ldm_config);
        }
      }

      return;
    }

    // static
    bool variant_repository::parse_variant_path(const std::string & variant_full_path_,
                                                std::string & registry_name_,
                                                std::string & variant_path_,
                                                bool & reversed_)
    {
      registry_name_.clear();
      variant_path_.clear();
      reversed_ = false;
      size_t found_colon = variant_full_path_.find(':');
      if (found_colon == variant_full_path_.npos) {
        return false;
      }
      bool reversed = false;
      if (variant_full_path_[0] == '!') {
        reversed = true;
      }
      std::string registry_name = variant_full_path_.substr((reversed?1:0), found_colon);
      if (registry_name.empty()) {
        return false;
      }
      std::string variant_path = variant_full_path_.substr(found_colon + 1);
      if (variant_path.empty()) {
        return false;
      }
      registry_name_ = registry_name;
      variant_path_  = variant_path;
      reversed_      = reversed;
      return true;
    }

    /*

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
      std::string dependee_registry;
      std::string local_variant_path;
      bool        reversed;
      bool parsed = parse_variant_path(dependee_variant_path_,
                                       dependee_registry,
                                       local_variant_path,
                                       reversed);
      DT_THROW_IF(!parsed, std::logic_error, "Bad variant path format!");
      DT_THROW_IF(!has_registry(dependee_registry), std::logic_error, "No variant registry '" << dependee_registry << "'!");
      DT_THROW_IF(!get_registry(dependee_registry).has_variant_record(local_variant_path),
                  std::logic_error,
                  "Dependee variant path '" << dependee_variant_path_ << "' does not exist!");
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
      DT_THROW_IF(!re.depends_on(variant_path_),
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
    */

    bool variant_repository::is_accomplished() const
    {
      for (registry_dict_type::const_iterator i = _registries_.begin();
           i != _registries_.end();
           i++) {
        if (!is_active_registry(i->first)) {
          continue;
        }
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

    bool variant_repository::has_variant(const std::string & registry_key_,
                                         const std::string & variant_path_) const
    {
      if (!has_registry(registry_key_)) return false;
      const variant_registry & vreg = get_registry(registry_key_);
      return vreg.has_variant_record(variant_path_);
    }

    bool variant_repository::has_variant_parameter(const std::string & registry_key_,
                                                   const std::string & variant_parameter_path_) const
    {
      if (!has_registry(registry_key_)) return false;
      const variant_registry & vreg = get_registry(registry_key_);
      return vreg.has_parameter_record(variant_parameter_path_);
    }

    /*
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
      } catch (std::exception & x) {
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
        if (! has_registry(registry_key_)) {
          // The repository has no registry named 'registry_key_' :
          cri.set_error_code(command::CEC_SCOPE_INVALID);
          cri.set_error_message("Variant repository has no registry named '" + registry_key_ + "' !");
        } else {
          const variant_registry & vreg = get_registry(registry_key_);
          cri = vreg.cmd_get_parameter_value(param_path_, param_value_token_);
        }
      } catch (std::exception & x) {
        std::string message = "Registry '" + registry_key_ + "' : " + x.what();
        cri.set_error_code(command::CEC_FAILURE);
        cri.set_error_message(message);
      }
      return cri;
    }

    command::returned_info
    variant_repository::cmd_has_parameter(const std::string & registry_key_,
                                          const std::string & param_path_,
                                          bool & present_) const
    {
      command::returned_info cri;
      present_ = false;
      try {
        if (! has_registry(registry_key_)) {
          // The repository has no registry named 'registry_key_' :
          cri.set_error_code(command::CEC_SCOPE_INVALID);
          cri.set_error_message("Variant repository has no registry named '" + registry_key_ + "' !");
        } else {
          const variant_registry & vreg = get_registry(registry_key_);
          present_ = vreg.has_parameter_record(param_path_);
        }
      } catch (std::exception & x) {
        std::string message = "Registry '" + registry_key_ + "' : " + x.what();
        cri.set_error_code(command::CEC_FAILURE);
        cri.set_error_message(message);
        present_ = false;
      }
      return cri;
    }
    */

    bool variant_repository::is_active_variant(const std::string & registry_key_,
                                               const std::string & variant_path_) const
    {
      DT_THROW_IF(!has_registry(registry_key_), std::logic_error,
                  "Variant repository has no registry named '" << registry_key_ << "' !");
      const variant_registry & vreg = get_registry(registry_key_);
      bool active = vreg.is_active_variant(variant_path_);
      return active;
    }

    bool variant_repository::is_active_registry(const std::string & registry_key_) const
    {
      registry_dict_type::const_iterator found = _registries_.find(registry_key_);
      DT_THROW_IF(found == _registries_.end(), std::logic_error,
                  "Variant repository has no registry named '" << registry_key_ << "' !");
      bool active = true;
      if (has_dependency_model()) {
        if (get_dependency_model().has_dependency(registry_key_)) {
          const variant_dependency & dep = get_dependency_model().get_dependency(registry_key_);
          active = false;
          if (dep()) {
            active = true;
          }
        }
      }
      /*
      const registry_entry & re = found->second;
      bool active = true;
      if (re.has_dependency_model()) {
        if (re.get_dependency_model().has_dependency(":")) {
          const variant_dependency & dep = re.get_dependency_model().get_dependency(":");
          active = false;
          if (dep()) {
            active = true;
          }
        }
      }
      */

        /*
      for (std::list<std::string>::const_iterator iter = re._dependencies_.begin();
           iter !=  re._dependencies_.end();
           iter++) {
        const std::string & variant_path = *iter;
        std::string dependee_registry;
        std::string local_variant_path;
        bool        reversed;
        bool parsed = parse_variant_path(variant_path,
                                         dependee_registry,
                                         local_variant_path,
                                         reversed);
        bool variant_active = false;
        command::returned_info cri = cmd_is_active_variant(dependee_registry,
                                                           local_variant_path,
                                                           variant_active);
        DT_THROW_IF(cri.is_failure(), std::logic_error, cri.get_error_message());
        if (!variant_active) {
          active = false;
        }
        if (reversed) {
          active = !active;
        }
        if (!active) {
          break;
        }
      }

      }
        */
      return active;
    }

    /*
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
    */

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

      out_ << indent_ << i_tree_dumpable::tag << "Accomplished: "
           << (is_accomplished() ? "<yes>" : "<no>")
           << std::endl;

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
        out_ << "Variant registry '" << regname << "' ";
        const registry_entry & re = i->second;
        if (re.is_valid()) {
          if (re.is_external()) {
            out_ << "(external registry";
          } else if (re.is_embedded()) {
            out_ << "(embedded registry";
          }
          if (re.has_rank()) {
            out_ << ",rank=" << re.get_rank();
          }
          out_ << ')';
        } else {
          out_ << "(invalid)";
        }
        const variant_registry & reg = re.get_registry();
        if (reg.is_accomplished()) {
          out_ << " : " << "<accomplished>";
        } else {
          out_ << " : " << "<unaccomplished>";
        }
        out_ << std::endl;
      }

      {
        out_ << indent_ << i_tree_dumpable::tag << "Dependency model: ";
        if (!has_dependency_model()) {
          out_ << "<none>";
        }
        out_ << std::endl;
        if (has_dependency_model()) {
          std::ostringstream indent2_oss;
          indent2_oss << indent_ << i_tree_dumpable::skip_tag;
          _dependency_model_->tree_dump(out_, "", indent2_oss.str());
        }
      }

      {
        out_ << indent_ << i_tree_dumpable::tag << "Current profile: "
             << std::endl;
        uint32_t io_flags =
          datatools::configuration::ascii_io::IO_DESCRIPTION
          | datatools::configuration::ascii_io::IO_TITLE;
        datatools::configuration::ascii_io aw(io_flags);
        std::ostringstream prof_out;
        aw.store_repository(prof_out, *this);
        std::ostringstream indent2;
        indent2 << indent_ << i_tree_dumpable::skip_tag << "+-- ";
        datatools::print_multi_lines(out_, prof_out.str(), indent2.str());
      }

      out_ << indent_ << i_tree_dumpable::tag << "Reporting: "
           << (has_reporting() ? "<yes>" : "<no>")
           << std::endl;

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
      if (has_reporting()) {
        // Attach the variant usage reporting object to the kernel variant repository:
        configuration::variant_repository & k_var_rap = krnl.grab_variant_repository();
        DT_THROW_IF(k_var_rap.has_reporting(), std::logic_error, "Kernel variant repository already has an attached variant reporting!");
        k_var_rap.set_reporting(*_reporting_);
      }
      return;
    }

    void variant_repository::system_discard() const
    {
      DT_THROW_IF(! datatools::kernel::is_instantiated(), std::logic_error,
                  "Kernel is not instantiated!");
      datatools::kernel & krnl = kernel::instance();
      if (has_reporting()) {
        configuration::variant_repository & k_var_rap = krnl.grab_variant_repository();
        // DT_THROW_IF(!k_var_rap.has_reporting(), std::logic_error, "Kernel variant repository has no attached variant reporting!");
        if (k_var_rap.has_reporting()) {
          // Detach the variant usage reporting object from the kernel variant repository:
          k_var_rap.reset_reporting();
        }
      }

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

    void variant_repository::print_rst(std::ostream & out_, uint32_t flags_) const
    {
      bool with_title = true;
      if (flags_ & PRINT_RST_NO_TITLE) {
        with_title = false;
      }

      if (with_title) {
        std::ostringstream titleoss;
        titleoss << "Variant repository ``" << get_name() << "``" << std::ends;
        // std::cerr << "DEVEL: len = " << titleoss.str().length() << std::endl;
        out_ << std::setw(titleoss.str().length()) << std::setfill('=') << "" << std::endl;
        out_ << titleoss.str() << std::endl;
        out_ << std::setw(titleoss.str().length()) << std::setfill('=') << "" << std::endl;
        out_ << std::endl;
      }

      if (has_terse_description()) {
        datatools::print_multi_lines(out_, get_terse_description(), "");
        out_ << std::endl;
      }

      out_ << "General informations" << std::endl;
      out_ << "====================" << std::endl;
      out_ << std::endl;

      {
        std::size_t count = 0;

        if (has_display_name()) {
          out_ << "* Display name : ``" << get_display_name() << "``" << std::endl;
          count++;
        }

        if (has_organization()) {
          out_ << "* Organization : ``" << get_organization() << "``" << std::endl;
          count++;
        }

        if (has_application()) {
          out_ << "* Application : ``" << get_application() << "``" << std::endl;
          count++;
        }

        // out_ << "* Initialized: ``" << is_initialized() << "``" << std::endl;
        // count++;

        // out_ << "* Locked: ``" << is_locked() << "``" << std::endl;
        // count++;

        if (count) {
          out_ << std::endl;
        }
      }

      out_ << "Registries" << std::endl;
      out_ << "==========" << std::endl;
      out_ << std::endl;

      {
        std::vector<std::string> vreg_keys;
        build_ordered_registry_keys(vreg_keys);

        out_ << "There";
        if (vreg_keys.size() == 1) {
          out_ << " is only one variant registry";
        } else if (vreg_keys.size() > 1) {
          out_ << " are " << vreg_keys.size() << " variant registries";
        } else {
          out_ << " is no variant registry";
        }
        out_ << " in this repository.";
        out_ << std::endl;
        out_ << std::endl;

        for (std::size_t ivreg = 0;
             ivreg < vreg_keys.size();
             ivreg++) {
          const std::string & vreg_key = vreg_keys[ivreg];
          const variant_registry & vreg = get_registry(vreg_key);
          std::ostringstream hdross;
          hdross << "``\"" << vreg_key  << "\"`` (variant model: ``\"" << vreg.get_top_variant_name() << "\"``)" << std::ends;
          out_ << hdross.str() << std::endl;
          out_ << std::setw(hdross.str().length()) << std::setfill('-') << "" << std::endl;
          out_ << std::endl;

          if (has_terse_description()) {
            datatools::print_multi_lines(out_, vreg.get_terse_description(), "");
            out_ << std::endl;
          }

          {
            std::size_t count = 0;

            if (has_display_name()) {
              out_ << "* Display name: ``" << '"' << vreg.get_display_name() << '"' << "``" << std::endl;
              count++;
            }

            // Rank

            // // XXX
            // out_ << "*Depends on: "; //``" << '"' << vreg.get_top_variant_name() << '"' << "``" << std::endl;
            // out_ << std::endl;
            // count++;

            out_ << "* Top variant name: ``" << '"' << vreg.get_top_variant_name() << '"' << "``" << std::endl;
            count++;

            // out_ << "* Initialized: ``" << vreg.is_initialized() << "``" << std::endl;
            // count++;

            std::vector<std::string> param_paths;
            uint32_t list_flags = 0;
            vreg.list_of_parameters(param_paths, list_flags);

            out_ << "* Parameters: ``" << param_paths.size() << "``" << std::endl;
            count++;

            for (std::size_t iparam = 0; iparam < param_paths.size(); iparam++) {
              const std::string & varParamName = param_paths[iparam];
              if (iparam == 0) {
                out_ << std::endl;
              }
              const variant_record & varParRec = vreg.get_parameter_record(varParamName);
              const parameter_model & varParModel = varParRec.get_parameter_model();

              out_ << "  * ``" << '"' << varParamName << '"'
                   << "`` (parameter model: ``"
                   << '"' << varParModel.get_name() << '"'
                   << "``) :"
                   << std::endl;
              out_ << std::endl;

              const std::string & varParLeafName = varParRec.get_leaf_name();
              const variant_record & varParentRec = varParRec.get_parent();
              const variant_model & varParentModel = varParentRec.get_variant_model();
              const std::string & param_desc = varParentModel.get_parameter_description(varParLeafName);
              if (!param_desc.empty()) {
                out_ << "    * Description: ``";
                out_ << param_desc;
                out_ << "``" << std::endl;
              }
              // std::string value_format;
              // command::returned_info cri = varParRec.value_to_string(value_format);
              out_ << "    * Full path: ``" << '"' << vreg_key << ':' << varParamName << '"' << "``" << std::endl;
              uint32_t varParModelRstOpt = 0;
              varParModelRstOpt |= parameter_model::PRINT_RST_NO_TITLE;
              varParModelRstOpt |= parameter_model::PRINT_RST_NO_DESC;
              varParModel.print_rst(out_, "    ", varParModelRstOpt);
              out_ << std::endl;
            }
            if (count) {
              out_ << std::endl;
            }
          }

          out_ << std::endl;

          if (vreg_keys.size()) {
            out_ << std::endl;
          }
        }
      }
      return;
    }

  }  // end of namespace configuration

}  // end of namespace datatools
