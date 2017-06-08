// datatools/configuration/variant_record.cc
/*
 * Copyright (C) 2014-2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
#include <datatools/configuration/variant_record.h>

// Third party
// - Boost:
#include <boost/filesystem/path.hpp>

// This project:
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/configuration/parameter_model.h>
#include <datatools/configuration/variant_model.h>
#include <datatools/configuration/variant_registry.h>
#include <datatools/configuration/variant_repository.h>

namespace datatools {

  namespace configuration {

    void variant_record::_fix_dependers_on_this_variant_()
    {
      datatools::logger::priority logging = _logging_;
      // logging = datatools::logger::PRIO_TRACE;
      DT_LOG_TRACE_ENTERING(logging);

      DT_THROW_IF(!is_variant(), std::logic_error, "Record '" << _path_ << "' is not associated to a variant!");
      if (has_parent_registry()) {
        DT_LOG_DEBUG(logging, "Variant record '" << _path_ << " has a parent registry...");
        if (get_parent_registry().has_dependency_model()) {
          // Local dependency model:
          DT_LOG_DEBUG(logging, "Variant record '" << _path_ << " has a local dependency model...");
          const variant_dependency_model & ldepmod = get_parent_registry().get_dependency_model();
          variant_object_info voi;
          voi.make_local_variant(_path_);
          std::string local_variant_path = voi.get_full_path();
          DT_LOG_DEBUG(logging, "Local variant path : '" << local_variant_path << "'");
          if (ldepmod.has_dependee_record_by_path(local_variant_path)) {
            DT_LOG_DEBUG(logging, "Variant '" << local_variant_path << "' has local dependers");
            const variant_dependency_model::dependers_table_type & table = ldepmod.get_dependers_per_dependee();
            if (table.find(local_variant_path) != table.end()) {
              const std::set<std::string> & dependers = table.find(local_variant_path)->second;
              for (const std::string & depender_path : dependers) {
                DT_LOG_DEBUG(logging, "Depender of variant '" << local_variant_path << "' : '" << depender_path << "'");
                variant_object_info depender_voi;
                DT_THROW_IF(!depender_voi.parse_from_string(depender_path), std::logic_error,
                            "Could not parse depender local variant path '" << local_variant_path << "'!");
                if (depender_voi.is_parameter()) {
                  DT_LOG_DEBUG(logging, "Depender '" << depender_path << "' is a parameter...");
                  // FM: Re-consider this case:
                  // variant_registry & vreg = const_cast<variant_registry &>(get_parent_registry());
                  // variant_record & depender_record
                  //   = vreg.grab_variant_record(depender_voi.get_parameter_local_path());
                  // depender_record._fix_parameter_value();
                }
                if (depender_voi.is_parameter_value_group()) {
                  DT_LOG_DEBUG(logging, "Depender '" << depender_path << "' is a parameter group of values...");
                  variant_registry & vreg = const_cast<variant_registry &>(get_parent_registry());
                  variant_record & depender_record = vreg.grab_parameter_record(depender_voi.get_parameter_local_path());
                  // Make sure the value of the parameter to which the group is associated is still valid or needs to be
                  // fixed:
                  depender_record._fix_parameter_value_();
                }
              }
            }
          }
        }
        if (get_parent_registry().is_mounted()) {
          const std::string & registry_name = get_parent_registry().get_mounting_name();
          DT_LOG_DEBUG(logging, "Variant record '" << _path_ << " has a mounted parent registry using name '" << registry_name << "'...");
          if (has_parent_repository() && get_parent_repository().has_dependency_model()) {
            // Global dependency model:
            DT_LOG_DEBUG(logging, "Variant record '" << _path_ << " has a global dependency model...");
            const variant_dependency_model & gdepmod = get_parent_repository().get_dependency_model();
            variant_object_info voi;
            voi.make_variant(registry_name, _path_);
            std::string global_variant_path = voi.get_full_path();
            DT_LOG_DEBUG(logging, "Global variant path : '" << global_variant_path << "'");
            if (gdepmod.has_dependee_record_by_path(global_variant_path)) {
              DT_LOG_DEBUG(logging, "Variant '" << global_variant_path << "' has global dependers");
              const variant_dependency_model::dependers_table_type & table = gdepmod.get_dependers_per_dependee();
              if (table.find(global_variant_path) != table.end()) {
                const std::set<std::string> & dependers = table.find(global_variant_path)->second;
                for (const std::string & depender_path : dependers) {
                  DT_LOG_DEBUG(logging, "Depender of variant '" << global_variant_path << "' : '" << depender_path << "'");
                  variant_object_info depender_voi;
                  if (!depender_voi.parse_from_string(depender_path)) {
                    DT_LOG_FATAL(logging,  "Could not parse depender global variant path '" << global_variant_path << "'!");
                    DT_THROW(std::logic_error, "Could not parse depender global variant path '" << global_variant_path << "'!");
                  }
                  if (!depender_voi.is_valid()) {
                    DT_LOG_DEBUG(logging, " -> Invalid VOI : " << "continue...");
                    continue;
                  }
                  DT_LOG_DEBUG(logging, " -> VOI = " << depender_voi);
                  DT_LOG_DEBUG(logging, " -> Checking for a parameter...");
                  if (depender_voi.is_parameter()) {
                    DT_LOG_DEBUG(logging, "Depender '" << depender_path << "' is a parameter...");
                    // FM: reconsider this case...
                    // variant_repository & vrep = const_cast<variant_repository &>(get_parent_repository());
                    // variant_registry & vreg = vrep.grab_registry(depender_voi.get_registry_name());
                    // variant_record & depender_record = vreg.grab_variant_record(depender_voi.get_parameter_local_path());
                    //depender_record._fix_parameter_value_();
                  }
                  DT_LOG_DEBUG(logging, " -> Checking for a parameter value group...");
                  if (depender_voi.is_parameter_value_group()) {
                    DT_LOG_DEBUG(logging, "Depender '" << depender_path << "' is a parameter group of values...");
                    variant_repository & vrep = const_cast<variant_repository &>(get_parent_repository());
                    DT_LOG_DEBUG(logging, "Fetch depender registry '" << depender_voi.get_registry_name() << "'...");
                    variant_registry & vreg = vrep.grab_registry(depender_voi.get_registry_name());
                    DT_LOG_DEBUG(logging, "Fetch depender record '" << depender_voi.get_parameter_local_path() << "'...");
                    // vreg.tree_dump(std::cerr, "Depender's variant registry:", "[debug] ");
                    variant_record & depender_record = vreg.grab_parameter_record(depender_voi.get_parameter_local_path());
                    // Make sure the value of the parameter to which the group is associated is still valid or needs to be
                    // fixed:
                    DT_LOG_DEBUG(logging, "Fix the value of the associated parameter '" << depender_record.get_path() << "'...");
                    depender_record._fix_parameter_value_();
                  }
                }
              } // Not empty dependers table
            } else {
              DT_LOG_DEBUG(logging, "Variant '" << global_variant_path << "' has no global dependers");
            }
          }
        }
      }
      DT_LOG_TRACE_EXITING(logging);
      return;
    }

    bool variant_record::check_enabled_parameter() const
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      DT_THROW_IF(!is_parameter(), std::logic_error, "Variant record '" << _path_ << "' is not a parameter!");
      // const parameter_model & parmod = get_parameter_model();
      bool enabled = true;
      if (enabled && has_parent_registry()) {

        if (enabled && get_parent_registry().has_dependency_model()) {
          // Local dependency model:
          DT_LOG_DEBUG(_logging_, "Registry '" << get_parent_registry().get_name()
                       << "' has a local dependency model!");
          const variant_dependency_model & ldepmod = get_parent_registry().get_dependency_model();
          variant_object_info voi;
          voi.make_local_parameter(_path_);
          std::string param_local_path = voi.get_full_path();
          if (ldepmod.has_dependency(param_local_path)) {
            DT_LOG_DEBUG(_logging_, "Variant parameter '" << param_local_path << "' has a local dependency!");
            const variant_dependency & ldep = ldepmod.get_dependency(param_local_path);
            bool ldep_enabled = ldep();
            if (!ldep_enabled) {
              DT_LOG_DEBUG(_logging_, "Variant parameter '" << param_local_path
                           << "' is not enabled by its local dependency!");
              enabled = false;
            }
          } // has associated group with dependency
        } // has local dependency model

        if (enabled && get_parent_registry().is_mounted()) {
          // The variant record belongs to a registry mounted in a repository:
          const std::string & registry_name = get_parent_registry().get_mounting_name();
          if (has_parent_repository() && get_parent_repository().has_dependency_model()) {
            // Global dependency model:
            DT_LOG_DEBUG(_logging_, "Repository '" << get_parent_repository().get_name()
                         << "' has a global dependency model!");
            const variant_dependency_model & gdepmod = get_parent_repository().get_dependency_model();
            // Search global dependency associated to this parameter:
            variant_object_info voi;
            voi.make_parameter(registry_name, _path_);
            std::string param_global_path = voi.get_full_path();
            if (gdepmod.has_dependency(param_global_path)) {
              DT_LOG_DEBUG(_logging_, "Variant parameter '" << param_global_path << "' has a dependency!");
              const variant_dependency & gdep = gdepmod.get_dependency(param_global_path);
              bool gdep_enabled = gdep();
              if (!gdep_enabled) {
                DT_LOG_DEBUG(_logging_, "Variant parameter '" << param_global_path
                             << "' is not enabled by its global dependency!");
                enabled = false;
              }
            } // has associated group with dependency
          } // has global dependency model
        } // mounted registry

      } // has parent registry

      DT_LOG_TRACE_EXITING(_logging_);
      return enabled;
    }

    bool variant_record::check_enabled_group(const std::string & group_name_) const
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      DT_LOG_DEBUG(_logging_, "Checking enabled group '" << group_name_ << "'");
      const parameter_model & parmod = get_parameter_model();
      DT_THROW_IF(!parmod.has_group(group_name_),
                  std::domain_error,
                  "Parameter model '" << parmod.get_name() << "' has no group named '" << group_name_ << "'!");
      bool enabled = true;
      if (enabled && has_parent_registry()) {

        if (enabled && get_parent_registry().has_dependency_model()) {
          // Local dependency model:
          DT_LOG_DEBUG(_logging_, "Registry '" << get_parent_registry().get_name()
                       << "' has a local dependency model!");
          const variant_dependency_model & ldepmod = get_parent_registry().get_dependency_model();
          variant_object_info voi;
          voi.make_local_parameter_value_group(_path_, group_name_);
          std::string local_path = voi.get_full_path();
          if (ldepmod.has_dependency(local_path)) {
            DT_LOG_DEBUG(_logging_, "Group '" << local_path << "' has a local dependency!");
            const variant_dependency & ldep = ldepmod.get_dependency(local_path);
            bool ldep_enabled = ldep();
            if (!ldep_enabled) {
              DT_LOG_DEBUG(_logging_, "Variant object '" << local_path
                           << "' is not enabled by its local dependency!");
              enabled = false;
            }
          } // has associated group with dependency
        } // has local dependency model

        if (enabled && get_parent_registry().is_mounted()) {
          // The variant record belongs to a registry mounted in a repository:
          const std::string & registry_name = get_parent_registry().get_mounting_name();
          if (has_parent_repository() && get_parent_repository().has_dependency_model()) {
            // Global dependency model:
            DT_LOG_DEBUG(_logging_, "Repository '" << get_parent_repository().get_name()
                         << "' has a global dependency model!");
            const variant_dependency_model & gdepmod = get_parent_repository().get_dependency_model();
            // Search global dependency associated to this group:
            variant_object_info voi;
            voi.make_parameter_value_group(registry_name, _path_, group_name_);
            std::string global_path = voi.get_full_path();
            if (gdepmod.has_dependency(global_path)) {
              DT_LOG_DEBUG(_logging_, "Group '" << global_path << "' has a dependency!");
              const variant_dependency & gdep = gdepmod.get_dependency(global_path);
              bool gdep_enabled = gdep();
              if (!gdep_enabled) {
                DT_LOG_DEBUG(_logging_, "Variant object '" << global_path
                             << "' is not enabled by its global dependency!");
                enabled = false;
              }
            } // has associated group with dependency
            else {
              DT_LOG_DEBUG(_logging_, "Group '" << group_name_ << "' has no dependency!");
            }
          } // has global dependency model
        } // mounted registry

      } // has parent registry
      DT_LOG_DEBUG(_logging_, "Group '" << group_name_ << "' enabled = " << enabled);
      DT_LOG_TRACE_EXITING(_logging_);
      return enabled;
    }

    bool variant_record::check_enabled_value(const parameter_value_type & value_) const
    {
      datatools::logger::priority logging = _logging_;
      DT_LOG_TRACE_ENTERING(logging);
      bool enabled = true;
      const parameter_model & parmod = get_parameter_model();
      if (enabled && parmod.is_boolean() && ! parmod.is_boolean_valid(boost::get<bool>(value_))) {
        enabled = false;
      }
      if (enabled && parmod.is_integer() && ! parmod.is_integer_valid(boost::get<int>(value_))) {
        enabled = false;
      }
      if (enabled && parmod.is_real() && ! parmod.is_real_valid(boost::get<double>(value_))) {
        enabled = false;
      }
      if (enabled && parmod.is_string() && ! parmod.is_string_valid(boost::get<std::string>(value_))) {
        enabled = false;
      }
      DT_LOG_DEBUG(logging, "Record '" << get_path() << "' is preliminary enabled!");

      if (enabled && has_parent_registry()) {

        if (enabled && get_parent_registry().has_dependency_model()) {
          // Local dependency model:
          DT_LOG_DEBUG(logging, "Registry '" << get_parent_registry().get_name()
                       << "' has a local dependency model!");
          const variant_dependency_model & ldepmod = get_parent_registry().get_dependency_model();
          // Search group associated to the value:
          if (enabled) {
            bool has_group = false;
            std::string group_name;
            if (parmod.is_boolean() && parmod.is_enumeration() && parmod.enumerated_boolean_value_has_group(boost::get<bool>(value_))) {
              has_group = true;
              group_name = parmod.get_enumerated_boolean_value_group(boost::get<bool>(value_));
            }
            if (parmod.is_integer() && parmod.is_enumeration() && parmod.enumerated_integer_value_has_group(boost::get<int>(value_))) {
              has_group = true;
              group_name = parmod.get_enumerated_integer_value_group(boost::get<int>(value_));
            }
            if (parmod.is_real() && parmod.is_enumeration() && parmod.enumerated_real_value_has_group(boost::get<double>(value_))) {
              has_group = true;
              group_name = parmod.get_enumerated_real_value_group(boost::get<double>(value_));
            }
            if (parmod.is_string() && parmod.is_enumeration() && parmod.enumerated_string_value_has_group(boost::get<std::string>(value_))) {
              has_group = true;
              group_name = parmod.get_enumerated_string_value_group(boost::get<std::string>(value_));
            }
            // Search group-based dependency associated to the value:
            if (has_group) {
              // Fetch group associated to the value:
              DT_LOG_DEBUG(logging, "Value [" << value_ << "] for record '" << get_path() << "' has group '" <<  group_name << "'!");
              // Search local dependency associated to this group:
              variant_object_info voi;
              voi.make_local_parameter_value_group(_path_, group_name);
              std::string local_path = voi.get_full_path();
              DT_LOG_DEBUG(logging, " ==> local_path = '" << local_path << "'!");
              if (ldepmod.has_dependency(local_path)) {
                DT_LOG_DEBUG(logging, "Group '" << local_path << "' has a local dependency!");
                const variant_dependency & ldep = ldepmod.get_dependency(local_path);
                if (datatools::logger::is_trace(logging)) {
                  ldep.tree_dump(std::clog, "Variant dependency : ", "[trace]: ");
                }
                bool ldep_enabled = ldep();
                if (!ldep_enabled) {
                  DT_LOG_DEBUG(logging, "Variant group '" << local_path
                               << "' is not enabled by its local dependency!");
                  enabled = false;
                } else {
                  DT_LOG_DEBUG(logging, "Variant group '" << local_path
                               << "' is enabled by its local dependency!");
                  enabled = true;
                }
              } // has associated group with dependency
            } // has associated group
          }

          if (enabled && has_daughters()) {
            // Search variant dependency associated to the value:
            std::set<std::string> variants;
            if (parmod.is_boolean()) {
              parmod.find_variants_associated_to_boolean(boost::get<bool>(value_), variants);
            }
            if (parmod.is_integer()) {
              parmod.find_variants_associated_to_integer(boost::get<int>(value_), variants);
            }
            if (parmod.is_real()) {
              parmod.find_variants_associated_to_real(boost::get<double>(value_), variants);
            }
            if (parmod.is_string()) {
              parmod.find_variants_associated_to_string(boost::get<std::string>(value_), variants);
            }
            for (const std::string & variant_name : variants) {
              DT_LOG_DEBUG(logging, "Value [" << value_ << "] triggers variant '" << variant_name << "'!");
              const variant_record & daughter_variant = this->get_daughter(variant_name);
              // Search local dependency associated to this group:
              variant_object_info voi;
              voi.make_local_variant(daughter_variant.get_path());
              std::string local_path = voi.get_full_path();
              if (ldepmod.has_dependency(local_path)) {
                DT_LOG_DEBUG(logging, "Variant '" << local_path << "' has a local dependency!");
                const variant_dependency & ldep = ldepmod.get_dependency(local_path);
                bool ldep_valid = ldep();
                if (!ldep_valid) {
                  DT_LOG_DEBUG(logging, "Variant '" << local_path
                               << "' is not fulfilled by its local dependency!");
                  enabled = false;
                } else {
                  DT_LOG_DEBUG(logging, "Variant '" << local_path
                               << "' is fulfilled by its local dependency!");
                  enabled = true;
                }
              } // associated variant has dependency
            } // for variant_name
          } // has daughters
        } // has local dependency model

        if (enabled && get_parent_registry().is_mounted()) {
          // The variant record belongs to a registry mounted in a repository:
          const std::string & registry_name = get_parent_registry().get_mounting_name();
          if (has_parent_repository() && get_parent_repository().has_dependency_model()) {
            // Global dependency model:
            DT_LOG_DEBUG(logging, "Repository '" << get_parent_repository().get_name()
                         << "' has a global dependency model!");
            const variant_dependency_model & gdepmod = get_parent_repository().get_dependency_model();

            // Search group associated to the value:
            if (enabled) {
              bool has_group = false;
              std::string group_name;
              if (parmod.is_boolean() && parmod.is_enumeration() && parmod.enumerated_boolean_value_has_group(boost::get<bool>(value_))) {
                has_group = true;
                group_name = parmod.get_enumerated_boolean_value_group(boost::get<bool>(value_));
              }
              if (parmod.is_integer() && parmod.is_enumeration() && parmod.enumerated_integer_value_has_group(boost::get<int>(value_))) {
                has_group = true;
                group_name = parmod.get_enumerated_integer_value_group(boost::get<int>(value_));
              }
              if (parmod.is_real() && parmod.is_enumeration() && parmod.enumerated_real_value_has_group(boost::get<double>(value_))) {
                has_group = true;
                group_name = parmod.get_enumerated_real_value_group(boost::get<double>(value_));
              }
              if (parmod.is_string() && parmod.is_enumeration() && parmod.enumerated_string_value_has_group(boost::get<std::string>(value_))) {
                has_group = true;
                group_name = parmod.get_enumerated_string_value_group(boost::get<std::string>(value_));
              }
              // Search group-based dependency associated to the value:
              if (has_group) {
                // Fetch group associated to the value:
                DT_LOG_DEBUG(logging, "Value [" << value_ << "] has group '" <<  group_name << "'!");
                // Search global dependency associated to this group:
                variant_object_info voi;
                voi.make_parameter_value_group(registry_name, _path_, group_name);
                std::string global_path = voi.get_full_path();
                if (gdepmod.has_dependency(global_path)) {
                  DT_LOG_DEBUG(logging, "Group '" << global_path << "' has a dependency!");
                  const variant_dependency & gdep = gdepmod.get_dependency(global_path);
                  if (datatools::logger::is_debug(logging)) {
                    gdep.tree_dump(std::cerr, "", "[debug] ");
                  }
                  bool gdep_enabled = gdep();
                  if (!gdep_enabled) {
                    DT_LOG_DEBUG(logging, "Variant object '" << global_path
                                 << "' is not enabled by its global dependency!");
                    enabled = false;
                  } else {
                    DT_LOG_DEBUG(logging, "Variant object '" << global_path
                                 << "' is enabled by its global dependency!");
                    enabled = true;
                  }
                } // has associated group with dependency
              } // has associated group
            }

            // Search variant dependency associated to the value:
            if (enabled && has_daughters()) {
              std::set<std::string> variants;
              if (parmod.is_boolean()) {
                parmod.find_variants_associated_to_boolean(boost::get<bool>(value_), variants);
              }
              if (parmod.is_integer()) {
                parmod.find_variants_associated_to_integer(boost::get<int>(value_), variants);
              }
              if (parmod.is_real()) {
                parmod.find_variants_associated_to_real(boost::get<double>(value_), variants);
              }
              if (parmod.is_string()) {
                parmod.find_variants_associated_to_string(boost::get<std::string>(value_), variants);
              }
              for (const std::string & variant_name : variants) {
                DT_LOG_DEBUG(logging, "Value [" << value_ << "] triggers variant '" << variant_name << "'!");
                const variant_record & daughter_variant = this->get_daughter(variant_name);
                // Search global dependency associated to this group:
                variant_object_info voi;
                voi.make_variant(registry_name, daughter_variant.get_path());
                std::string global_path = voi.get_full_path();
                if (gdepmod.has_dependency(global_path)) {
                  DT_LOG_DEBUG(logging, "Variant '" << global_path << "' has a dependency!");
                  const variant_dependency & gdep = gdepmod.get_dependency(global_path);
                  bool gdep_valid = gdep();
                  if (!gdep_valid) {
                    DT_LOG_DEBUG(logging, "Variant '" << global_path
                                 << "' is not fulfilled by its global dependency!");
                    enabled = false;
                    break;
                  } else {
                    DT_LOG_DEBUG(logging, "Variant '" << global_path
                                 << "' is fulfilled by its global dependency!");
                    enabled = true;
                  }
                } // associated variant has dependency
              } // for variant_name
            } // has daughters
          } // has global dependency model
        } // is_mounted

      } // has_parent_registry
      DT_LOG_DEBUG(logging, "Value = [" << value_ << "] enabled = " << enabled);
      DT_LOG_TRACE_EXITING(logging);
      return enabled;
    }

    variant_record::variant_record()
    {
      _parameter_model_ = nullptr;
      _variant_model_ = nullptr;
      _parent_registry_ = nullptr;
      _parent_ = nullptr;
      _active_ = false;
      _with_update_ = false;
      _value_set_ = false;
      _boolean_value_ = false;
      _integer_value_ = 0;
      datatools::invalidate(_real_value_);
      _string_value_.clear();
      return;
    }

    variant_record::~variant_record()
    {
      return;
    }

    void variant_record::reset()
    {
      _logging_ = datatools::logger::PRIO_FATAL;
      _with_update_ = false;
      _path_.clear();
      _parent_registry_ = nullptr;
      _parameter_model_ = nullptr;
      _variant_model_ = nullptr;
      _parent_ = nullptr;
      _daughters_.clear();
      _ranked_daughters_.clear();
      _unranked_daughters_.clear();
      _active_ = false;
      _value_set_ = false;
      _boolean_value_ = false;
      _integer_value_ = 0;
      datatools::invalidate(_real_value_);
      _string_value_.clear();
      return;
    }

    bool variant_record::has_with_update() const
    {
      return _with_update_;
    }

    void variant_record::set_with_update(bool with_update_)
    {
      _with_update_ = with_update_;
      return;
    }

    datatools::logger::priority variant_record::get_logging() const
    {
      return _logging_;
    }

    void variant_record::set_logging(const datatools::logger::priority p_)
    {
      _logging_ = p_;
      return;
    }

    bool variant_record::is_valid() const
    {
      return has_path() && (has_parameter_model() || has_variant_model());
    }

    bool variant_record::has_path() const
    {
      return !_path_.empty();
    }

    void variant_record::set_path(const std::string & p_)
    {
      _path_ = p_;
      return;
    }

    const std::string & variant_record::get_path() const
    {
      return _path_;
    }

    bool variant_record::has_parent_registry() const
    {
      return _parent_registry_ != nullptr;
    }

    void variant_record::set_parent_registry(const variant_registry & reg_)
    {
      _parent_registry_ = &reg_;
      return;
    }

    const variant_registry & variant_record::get_parent_registry() const
    {
      return *_parent_registry_;
    }

    bool variant_record::has_parent_repository() const
    {
      if (!has_parent_registry()) {
        return false;
      }
      if (!_parent_registry_->has_parent_repository()) {
        return false;
      }
      return true;
    }

    const variant_repository & variant_record::get_parent_repository() const
    {
      DT_THROW_IF(!has_parent_repository(), std::logic_error, "No parent repository is set!");
      return _parent_registry_->get_parent_repository();
    }

    std::string variant_record::get_leaf_name() const
    {
      boost::filesystem::path pth(_path_);
      return pth.leaf().string();
    }

    bool variant_record::is_top_variant() const
    {
      return !has_parent() && is_variant();
    }

    bool variant_record::has_parent() const
    {
      return _parent_ != nullptr;
    }

    void variant_record::set_parent(variant_record & parent_, const std::string & daughter_name_, const int rank_)
    {
      DT_THROW_IF((is_parameter() && parent_.is_parameter())
                  || (is_variant() && parent_.is_variant()),
                  std::logic_error,
                  "Record '" << _path_ << "' : incompatible parent/daughter relationship!");
      _parent_ = &parent_;
      daughter_dict_type::const_iterator found = _parent_->get_daughters().find(daughter_name_);
      DT_THROW_IF(found != _parent_->get_daughters().end(),
                  std::logic_error,
                  "Record '" << _path_ << "' already has a daughter named '" << daughter_name_ << "' !");
      daughter_type d(daughter_name_, *this, rank_);
      _parent_->grab_daughters().insert(std::pair<std::string, daughter_type>(daughter_name_, d));
      parent_._compute_ranked_unranked_daughters_();
      return;
    }

    const variant_record & variant_record::get_parent() const
    {
      DT_THROW_IF(!has_parent(), std::logic_error,
                  "Record '" << _path_ << "' has no parent record!");
      return *_parent_;
    }

    variant_record & variant_record::grab_parent()
    {
      DT_THROW_IF(!has_parent(), std::logic_error,
                  "Record '" << _path_ << "' has no parent record!");
      return *_parent_;
    }

    bool variant_record::has_parameter_model() const
    {
      return _parameter_model_ != nullptr;
    }

    void variant_record::set_parameter_model(const parameter_model & pm_)
    {
      _variant_model_ = nullptr;
      _parameter_model_ = &pm_;
      return;
    }

    const parameter_model & variant_record::get_parameter_model() const
    {
      DT_THROW_IF(! has_parameter_model(), std::logic_error,
                  "No associated parameter model!");
      return *_parameter_model_;
    }

    bool variant_record::has_variant_model() const
    {
      return _variant_model_ != nullptr;
    }

    void variant_record::set_variant_model(const variant_model & vm_)
    {
      _parameter_model_ = nullptr;
      _variant_model_ = &vm_;
      return;
    }

    const variant_model & variant_record::get_variant_model() const
    {
      DT_THROW_IF(! has_variant_model(), std::logic_error,
                  "No associated variant model!");
      return *_variant_model_;
    }

    bool variant_record::is_variant() const
    {
      return has_variant_model();
    }

    bool variant_record::is_parameter() const
    {
      return has_parameter_model();
    }

    void variant_record::set_active(bool active_)
    {
      if (_active_ != active_) {
        _active_ = active_;
        if (!_active_) {
          DT_LOG_DEBUG(get_logging(), "Deactivation of variant record '" << get_path() << "'...");
        } else {
          DT_LOG_DEBUG(get_logging(), "Activation of variant record '" << get_path() << "'...");
        }
        if (has_with_update()) {
          DT_LOG_DEBUG(get_logging(), "Updating variant record '" << get_path() << "'...");
          _update_();
        }
      } else {
        DT_LOG_DEBUG(get_logging(), "Activation of variant record '" << get_path() << "' is already " << active_);
      }
      return;
    }

    void variant_record::_compute_ranked_unranked_daughters_()
    {
      _ranked_daughters_.clear();
      _unranked_daughters_.clear();
      std::map<int, std::string> ranked;
      for (const auto & dpair : _daughters_) {
        if (dpair.second.is_ranked()) {
          ranked[dpair.second.get_rank()] = dpair.first;
        } else {
          _unranked_daughters_.push_back(dpair.first);
        }
      }
      for (const auto & ranked_pair : ranked) {
        _ranked_daughters_.push_back(ranked_pair.second);
      }
      DT_LOG_DEBUG(get_logging(),
                   "Variant record : '" << get_path() << "'");
      DT_LOG_DEBUG(get_logging(),
                   "  Number of ranked daughters   = " << _ranked_daughters_.size());
      DT_LOG_DEBUG(get_logging(),
                   "  Number of unranked daughters = " << _unranked_daughters_.size());
      return;
    }

    void variant_record::build()
    {
      DT_LOG_DEBUG(get_logging(),
                   "Building variant record : '" << get_path() << "'...");
      _compute_ranked_unranked_daughters_();
      set_with_update(true);
      return;
    }

    // // XXX
    // void variant_record::build_recursive_list_of_ranked_records(std::vector<std::string> & ranked_) const
    // {
    //   if (is_variant()) {
    //     std::vector<std::string> ranked_param_records;
    //     build_list_of_ranked_parameter_records(ranked_param_records);
    //     for (std::size_t ipar = 0; ipar < ranked_param_records.size(); ipar++) {
    //       ranked_.push_back(ranked_param_records[ipar]);
    //       std::cerr << "DEVEL: Push ranked parameter '" << ranked_param_records[ipar] << "'" << std::endl;
    //       const variant_record & par_record = get_parent_registry().get_parameter_record(ranked_param_records[ipar]);
    //       par_record.build_recursive_list_of_ranked_records(ranked_);
    //     }
    //   } else {
    //     std::vector<std::string> ranked_var_records;
    //     for (const auto & var_daughter : get_daughters()) {
    //       std::string path = var_daughter.second.get_record().get_path();
    //       ranked_.push_back(path);
    //       std::cerr << "DEVEL: Push variant '" << path << "'" << std::endl;
    //       const variant_record & var_record = get_parent_registry().get_variant_record(path);
    //       var_record.build_recursive_list_of_ranked_records(ranked_);
    //     }
    //   }
    //   return;
    // }

    void variant_record::build_list_of_ranked_parameter_records(std::vector<std::string> & ranked_) const
    {
      ranked_.clear();
      if (!is_variant()) {
        return;
      }
      ranked_.reserve(_ranked_daughters_.size() + _unranked_daughters_.size());
      for (const auto & name : _ranked_daughters_) {
        ranked_.push_back(name);
      }
      for (const auto & name : _unranked_daughters_) {
        ranked_.push_back(name);
      }
      return;
    }

    bool variant_record::is_active() const
    {
      return _active_;
    }

    bool variant_record::value_is_set() const
    {
      DT_THROW_IF(! is_parameter(), std::logic_error,
                  "Record '" << _path_ << "' is not a parameter!");
      return _value_set_;
    }

    bool variant_record::default_value_is_set() const
    {
      DT_THROW_IF(! is_parameter(), std::logic_error,
                  "Record '" << _path_ << "' is not a parameter!");
      DT_THROW_IF(get_parameter_model().is_fixed(), std::logic_error,
                  "Parameter record '" << _path_ << "' is fixed!");
      if (value_is_set() && get_parameter_model().has_default_value()) {
        // DT_THROW_IF(! value_is_set(), std::logic_error,
        //             "Parameter record '" << _path_ << "' has no value set!");
        if (get_parameter_model().is_boolean()) {
          return _boolean_value_ == get_parameter_model().get_default_boolean();
        } else if (get_parameter_model().is_integer()) {
          return _integer_value_ == get_parameter_model().get_default_integer();
        } else if (get_parameter_model().is_real()) {
          return _real_value_ == get_parameter_model().get_default_real();
        } else if (get_parameter_model().is_string()) {
          return _string_value_ == get_parameter_model().get_default_string();
        }
      }
      return false;
    }

    bool variant_record::is_fixed_parameter() const
    {
      DT_THROW_IF(! is_parameter(), std::logic_error,
                  "Record '" << _path_ << "' is not a parameter!");
      return get_parameter_model().is_fixed();
    }

    command::returned_info variant_record::set_fixed_value()
    {
      command::returned_info cri;
      try {
        if (! is_parameter()) {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_CONTEXT);
          DT_THROW(std::logic_error,
                   "Record '" << _path_ << "' is not a parameter!");
        }
        if (get_parameter_model().is_variable()) {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_CONTEXT);
          DT_THROW(std::logic_error,
                   "Parameter record '" << _path_ << "' is not fixed!");
        }
        if (get_parameter_model().is_boolean()) {
          cri = set_boolean_value(get_parameter_model().get_fixed_boolean());
        } else if (get_parameter_model().is_integer()) {
          cri = set_integer_value(get_parameter_model().get_fixed_integer());
        } else if (get_parameter_model().is_real()) {
          cri = set_real_value(get_parameter_model().get_fixed_real());
        } else if (get_parameter_model().is_string()) {
          cri = set_string_value(get_parameter_model().get_fixed_string());
        } else {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_TYPE);
          DT_THROW( std::logic_error,
                    "Parameter record '" << _path_ << "' has no valid type!");
        }
      } catch (std::exception & x) {
        cri.set_error_message(x.what());
      }
      return cri;
    }

    command::returned_info
    variant_record::set_default_value()
    {
      command::returned_info cri;
      try {
        if (! is_parameter()) {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_CONTEXT);
          DT_THROW(std::logic_error,
                   "Record '" << _path_ << "' is not a parameter!");
        }
        if (get_parameter_model().is_fixed()) {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_CONTEXT);
          DT_THROW(std::logic_error,
                   "Parameter record '" << _path_ << "' is fixed!");
        }
        if (! get_parameter_model().has_default_value()) {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_CONTEXT);
          DT_THROW(std::logic_error,
                   "Parameter record '" << _path_ << "' has no default value!");
        }
        if (get_parameter_model().is_boolean()) {
          cri = set_boolean_value(get_parameter_model().get_default_boolean());
        } else if (get_parameter_model().is_integer()) {
          cri = set_integer_value(get_parameter_model().get_default_integer());
        } else if (get_parameter_model().is_real()) {
          cri = set_real_value(get_parameter_model().get_default_real());
        } else if (get_parameter_model().is_string()) {
          cri = set_string_value(get_parameter_model().get_default_string());
        } else {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_TYPE);
          DT_THROW( std::logic_error,
                    "Parameter record '" << _path_ << "' has no valid type!");
        }
      } catch (std::exception & x) {
        cri.set_error_message(x.what());
        unset_value();
      }
      return cri;
    }

    command::returned_info
    variant_record::set_boolean_value(bool value_)
    {
      command::returned_info cri;
      try {
        if (! is_parameter()) {
          cri.set_error_code(command::CEC_COMMAND_INVALID_CONTEXT);
          DT_THROW(std::logic_error,
                   "Record '" << _path_ << "' is not a parameter!");
        }
        if (! get_parameter_model().is_boolean()) {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_TYPE);
          DT_THROW(std::logic_error,
                   "Parameter record '" << _path_ << "' is not a boolean parameter!");
        }
        if (!is_boolean_valid(value_)) {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_VALUE);
          DT_THROW(std::logic_error,
                   "Parameter record '" << _path_ << "' does not accept boolean value '" << value_ << "' !");
        }
        _boolean_value_ = value_;
        _value_set_ = true;
        if (has_with_update()) _update_();
      } catch (std::exception & x) {
        cri.set_error_message(x.what());
        // DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Set boolean value failed: " << cri.get_error_message());
      }
      return cri;
    }

    command::returned_info
    variant_record::set_integer_value(int value_)
    {
      datatools::logger::priority logging = _logging_;
      // // XXX
      // if (get_leaf_name() == "accuracy" ) {
      //   logging = datatools::logger::PRIO_TRACE;
      //   _logging_ = datatools::logger::PRIO_TRACE;
      // }
      DT_LOG_TRACE_ENTERING(logging);
      command::returned_info cri;
      try {
        if (! is_parameter()) {
          cri.set_error_code(command::CEC_COMMAND_INVALID_CONTEXT);
          DT_THROW(std::logic_error,
                   "Record '" << _path_ << "' is not a parameter!");
        }
        // DT_THROW_IF(get_parameter_model().is_fixed(), std::logic_error,
        //           "Parameter record '" << _path_ << "' is fixed!");
        if (! get_parameter_model().is_integer()) {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_TYPE);
          DT_THROW(std::logic_error,
                      "Parameter record '" << _path_ << "' is not a integer parameter!");
        }
        if (!is_integer_valid(value_)) {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_VALUE);
          DT_THROW(std::logic_error,
                   "Parameter record '" << _path_ << "' does not accept integer value '" << value_ << "' !");
        }
        _integer_value_ = value_;
        _value_set_ = true;
        if (has_with_update()) _update_();
      } catch (std::exception & x) {
        cri.set_error_message(x.what());
        // DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Set integer value failed: " << cri.get_error_message());
      }
      DT_LOG_TRACE_EXITING(logging);
      return cri;
    }

    command::returned_info
    variant_record::set_real_value(double value_)
    {
      command::returned_info cri;
      datatools::logger::priority logging = _logging_;
      // // XXX
      // if (get_leaf_name() == "value") {
      //   logging = datatools::logger::PRIO_TRACE;
      //   _logging_ = datatools::logger::PRIO_TRACE;
      // }
      DT_LOG_TRACE_ENTERING(logging);
      DT_LOG_DEBUG(logging, "Setting real value = " << value_ << " ... ");
      try {
        if (! is_parameter()) {
          cri.set_error_code(command::CEC_COMMAND_INVALID_CONTEXT);
          DT_THROW(std::logic_error,
                   "Record '" << _path_ << "' is not a parameter!");
        }
        // DT_THROW_IF(get_parameter_model().is_fixed(), std::logic_error,
        //           "Parameter record '" << _path_ << "' is fixed!");
        if (! get_parameter_model().is_real()) {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_TYPE);
          DT_THROW(std::logic_error,
                   "Parameter record '" << _path_ << "' is not a real parameter!");
        }
        if (! is_real_valid(value_)) {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_VALUE);
          DT_THROW(std::logic_error,
                   "Parameter record '" << _path_ << "' does not accept real value '" << value_ << "' !");
        }
        _real_value_ = value_;
        _value_set_ = true;
        if (has_with_update()) _update_();
      } catch (std::exception & x) {
        cri.set_error_message(x.what());
        // DT_LOG_ERROR(logging, "Real value failed: " << cri.get_error_message());
      }
      DT_LOG_DEBUG(logging, "New real value = " << _real_value_);
      DT_LOG_TRACE_EXITING(logging);
      return cri;
    }

    bool variant_record::is_boolean_valid(const bool value_) const
    {
      parameter_value_type pvv = value_;
      bool valid = check_enabled_value(pvv);
      return valid;
    }

    bool variant_record::is_integer_valid(const int value_) const
    {
      parameter_value_type pvv = value_;
      bool valid = check_enabled_value(pvv);
      return valid;
    }

    bool variant_record::is_real_valid(const double value_) const
    {
      DT_LOG_TRACE_ENTERING(get_logging());
      parameter_value_type pvv = value_;
      bool valid = check_enabled_value(pvv);
      DT_LOG_DEBUG(get_logging(), "Value [" << value_ << "] valid = " << valid);
      DT_LOG_TRACE_EXITING(get_logging());
      return valid;
    }

    bool variant_record::is_string_valid(const std::string & value_) const
    {
      parameter_value_type pvv = value_;
      bool valid = check_enabled_value(pvv);
      if (! valid) {
        DT_LOG_DEBUG(get_logging(), "Value '" << value_ << "' is disabled!");
      }
      return valid;
      // if (! get_parameter_model().is_string_valid(value_)) {
      //   return false;
      // }
      // return true;
    }

    command::returned_info
    variant_record::set_string_value(const std::string & value_)
    {
      command::returned_info cri;
      try {
        if (! is_parameter()) {
          cri.set_error_code(command::CEC_COMMAND_INVALID_CONTEXT);
          DT_THROW(std::logic_error,
                   "Record '" << _path_ << "' is not a parameter!");
        }
        // DT_THROW_IF(get_parameter_model().is_fixed(), std::logic_error,
        //           "Parameter record '" << _path_ << "' is fixed!");
        if (! get_parameter_model().is_string()) {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_TYPE);
          DT_THROW(std::logic_error,
                   "Parameter record '" << _path_ << "' is not a string parameter!");
        }
        if (! is_string_valid(value_)) {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_VALUE);
          DT_THROW(std::logic_error,
                   "Parameter record '" << _path_ << "' does not accept string value '" << value_ << "' !");
        }
        _string_value_ = value_;
        _value_set_ = true;
        if (has_with_update()) _update_();
      } catch (std::exception & x) {
        cri.set_error_message(x.what());
        // DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Set string value failed: " << cri.get_error_message());
      }
      return cri;
    }

    command::returned_info
    variant_record::unset_value()
    {
      command::returned_info cri;
      try {
        if (! is_parameter()) {
          cri.set_error_code(command::CEC_COMMAND_INVALID_CONTEXT);
          DT_THROW(std::logic_error,
                   "Record '" << _path_ << "' is not a parameter!");
        }
        // DT_THROW_IF(get_parameter_model().is_fixed(), std::logic_error,
        //           "Parameter record '" << _path_ << "' is fixed!");
        _value_set_ = false;
        if (has_with_update()) _update_();
      } catch (std::exception & x) {
        cri.set_error_message(x.what());
      }
      return cri;
    }

    void variant_record::_fix_parameter_value_()
    {
      datatools::logger::priority logging = _logging_;
      // logging = datatools::logger::PRIO_TRACE;
      DT_LOG_TRACE_ENTERING(logging);
      DT_LOG_DEBUG(logging, "Fixing parameter value for record '" << _path_ << "'...");
      if (!is_active()) {
        DT_LOG_DEBUG(logging, "Parameter record '" << _path_ << "' is not active: pass...");
        return;
      }
      if (value_is_set()) {
        DT_LOG_DEBUG(logging, "Parameter record '" << _path_ << "' has its value set...");
        // We check if the current value is still valid (considering both the parameter model
        // and possible dependency models). If not, we try to set a default value if one is available
        // and valid in the current context, or unset the value as a fallback approach, making
        // the record unaccomplished.
        if (get_parameter_model().is_boolean()) {
          if (!is_boolean_valid(_boolean_value_)) {
            DT_LOG_DEBUG(logging, "Parameter record '" << _path_ << "' has an invalid value: " << _real_value_);
            unset_value();
            if (get_parameter_model().has_default_value()
                && is_boolean_valid(get_parameter_model().get_default_boolean())) {
              set_default_value();
              DT_LOG_DEBUG(logging, "Parameter record '" << _path_ << "' has now its default value set.");
            }
          }
        }
        if (get_parameter_model().is_integer()) {
          if (!is_integer_valid(_integer_value_)) {
            DT_LOG_DEBUG(logging, "Parameter record '" << _path_ << "' has an invalid value: " << _real_value_);
            unset_value();
            if (get_parameter_model().has_default_value()
                && is_integer_valid(get_parameter_model().get_default_integer())) {
              set_default_value();
             DT_LOG_DEBUG(logging, "Parameter record '" << _path_ << "' has now its default value set.");
            }
          }
        }
        if (get_parameter_model().is_real()) {
          if (!is_real_valid(_real_value_)) {
            DT_LOG_DEBUG(logging, "Parameter record '" << _path_ << "' has an invalid value: " << _real_value_);
            unset_value();
            if (get_parameter_model().has_default_value()
                && is_real_valid(get_parameter_model().get_default_real())) {
              set_default_value();
              DT_LOG_DEBUG(logging, "Parameter record '" << _path_ << "' has now its default value set.");
            }
          }
        }
        if (get_parameter_model().is_string()) {
          if (!is_string_valid(_string_value_)) {
            DT_LOG_DEBUG(logging, "Parameter record '" << _path_ << "' has an invalid string value...");
            unset_value();
            DT_LOG_DEBUG(logging, "Parameter record '" << _path_ << "' has now its value unset.");
            if (get_parameter_model().has_default_value()
                && is_string_valid(get_parameter_model().get_default_string())) {
              set_default_value();
              DT_LOG_DEBUG(logging, "Parameter record '" << _path_ << "' has now its default value set.");
            }
          }
        }
      }
      DT_LOG_TRACE_EXITING(logging);
      return;
    }

    void variant_record::_fix_()
    {
       if (is_parameter()) {
         _fix_parameter_value_();
       }
      return;
    }

    void variant_record::update()
    {
      // XXX
      // if (get_leaf_name() == "generator") {
      //   _logging_ = datatools::logger::PRIO_TRACE;
      // }
      if (_with_update_) {
        DT_LOG_TRACE(_logging_, "Record '" << get_path() << "' with update");
        _update_();
      } else {
        DT_LOG_TRACE(_logging_, "Record '" << get_path() << "' does not update");
      }
      return;
    }

    void variant_record::forced_update()
    {
      _update_();
      return;
    }

    void variant_record::_update_()
    {
      datatools::logger::priority logging = _logging_;
      // logging = datatools::logger::PRIO_TRACE;
      DT_LOG_TRACE_ENTERING(logging);
      if (is_parameter()) {
        DT_LOG_DEBUG(logging, "Updating parameter record '" << get_path() << "'...");
        _update_parameter_();
      }
      if (is_variant()) {
        DT_LOG_DEBUG(logging, "Updating variant record '" << get_path() << "'...");
        _update_variant_();
      }
      return;
    }

    void variant_record::_update_variant_()
    {
      datatools::logger::priority logging = _logging_;
      DT_LOG_DEBUG(logging, "Updating variant '" << get_path() << "'...");
      std::vector<std::string> ranked_params;
      build_list_of_ranked_parameter_records(ranked_params);
      for (std::size_t i = 0; i < ranked_params.size(); i++) {
        const std::string & daughter_name = ranked_params[i];
        // Activate the daughter parameters if the variant is active:
        variant_record & param_rec = grab_daughter(daughter_name);
        if (param_rec.is_active() != _active_) {
          DT_LOG_DEBUG(logging, "Set activation of parameter record '" << param_rec.get_path() << "' to " << _active_);
          // if (datatools::logger::is_debug(logging)) {
          //   // XXX
          //   param_rec.set_logging(datatools::logger::PRIO_DEBUG);
          // }
          param_rec.set_active(_active_);
          DT_LOG_DEBUG(logging, "Done.");
        }
      }
      // Check if a change in the variant activation has consequences
      // on the validity of some depender parameters:
      DT_LOG_DEBUG(logging, "Checking for depender parameters...");
      _fix_dependers_on_this_variant_();
      DT_LOG_DEBUG(logging, "Depender parameters has been fixed.");
      return;
    }

    void variant_record::_update_parameter_()
    {
      datatools::logger::priority logging = _logging_;
      // logging = datatools::logger::PRIO_TRACE;
      DT_LOG_TRACE_ENTERING(logging);
      // if (get_leaf_name() == "palette") {
      //   logging = datatools::logger::PRIO_TRACE;
      // }
      DT_LOG_DEBUG(logging, "Updating parameter '" << get_path() << "'...");
      // First we deactivate all variants by default...
      if (_daughters_.size()) {
        DT_LOG_DEBUG(logging, "Deactivate all " << _daughters_.size() << " daughter variants for parameter '" << get_path() << "'");
        for (daughter_dict_type::iterator i = _daughters_.begin();
             i != _daughters_.end();
             i++) {
          DT_LOG_DEBUG(logging, "Processing daughter variant '" << i->first << "'...");
          variant_record & vrec = i->second.grab_record();
          if (vrec.is_active()) {
            // if (datatools::logger::is_debug(logging)) {
            //   // XXX
            //   vrec.set_logging(datatools::logger::PRIO_DEBUG);
            // }
            vrec.set_active(false);
            DT_LOG_DEBUG(logging, "Deactivating variant '" << vrec.get_path() << "'...");
          } else {
            DT_LOG_DEBUG(logging, "Variant '" << vrec.get_path() << "' is already deactivated.");
          }
        }
      }
      // Then search if some daughter variants is activated:
      if (value_is_set()) {
        DT_LOG_DEBUG(logging, "Search if some daughter variants is activated for parameter '" << get_path() << "'");
        std::set<std::string> activated_variant_names;
        bool found_activable = false;

        if (get_parameter_model().is_boolean()) {
          found_activable = get_parameter_model().find_variants_associated_to_boolean(_boolean_value_,
                                                                                      activated_variant_names);
        } // boolean

        if (get_parameter_model().is_integer()) {
          found_activable = get_parameter_model().find_variants_associated_to_integer(_integer_value_,
                                                                                      activated_variant_names);
        } // integer

        if (get_parameter_model().is_real()) {
          found_activable = get_parameter_model().find_variants_associated_to_real(_real_value_,
                                                                                   activated_variant_names);
        } // real

        if (get_parameter_model().is_string()) {
          found_activable = get_parameter_model().find_variants_associated_to_string(_string_value_,
                                                                                     activated_variant_names);
        } // string

        DT_LOG_DEBUG(logging, "Number of activated variants = " << activated_variant_names.size());

        if (is_active() && found_activable && activated_variant_names.size() > 0) {
          for (const auto & activated_variant_name : activated_variant_names) {
            DT_LOG_DEBUG(logging, "activated_variant_name = '" << activated_variant_name << "'");
            // daughter_dict_type::iterator found = _daughters_.find(activated_variant_name);
            // DT_THROW_IF(found == _daughters_.end(),
            //             std::logic_error,
            //             "Parameter record '" << _path_ << "' has no activable variant named '"
            //             << activated_variant_name << "' ! This is a bug!");
            // variant_record & activated_variant_rec = found->second.grab_record();
            variant_record & activated_variant_rec = grab_daughter(activated_variant_name);
            if (!activated_variant_rec.is_active()) {
              DT_LOG_DEBUG(logging, "Activating variant '" << activated_variant_name << "'...");
              activated_variant_rec.set_active(true);
            }
          }
        }
      } // value_is_set
      DT_LOG_TRACE_EXITING(logging);
      return;
    }

    command::returned_info
    variant_record::get_boolean_value(bool & value_) const
    {
      command::returned_info cri;
      try {
        if (! is_parameter()) {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_CONTEXT);
          DT_THROW(std::logic_error,
                   "Record '" << _path_ << "' is not a parameter!");
        }
        if (! get_parameter_model().is_boolean()) {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_TYPE);
          DT_THROW(std::logic_error,
                   "Parameter record '" << _path_ << "' is not a boolean value!");
        }
        if (! value_is_set()) {
          cri.set_error_code(command::CEC_PARAMETER_UNSET_VALUE);
          DT_THROW(std::logic_error,
                   "Parameter record '" << _path_ << "' is not set!");
        }
        value_ = _boolean_value_;
      } catch (std::exception & x) {
        cri.set_error_message(x.what());
      }
      return cri;
    }

    command::returned_info
    variant_record::get_integer_value(int & value_) const
    {
      command::returned_info cri;
      try {
        if (! is_parameter()) {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_CONTEXT);
          DT_THROW(std::logic_error,
                   "Record '" << _path_ << "' is not a parameter!");
        }
        if (! get_parameter_model().is_integer()) {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_TYPE);
          DT_THROW(std::logic_error,
                   "Parameter record '" << _path_ << "' is not an integer value!");
        }
        if (! value_is_set()) {
          cri.set_error_code(command::CEC_PARAMETER_UNSET_VALUE);
          DT_THROW(std::logic_error,
                   "Parameter record '" << _path_ << "' is not set!");
        }
        value_ = _integer_value_;
      } catch (std::exception & x) {
        cri.set_error_message(x.what());
      }
      return cri;
    }

    command::returned_info
    variant_record::get_real_value(double & value_) const
    {
      command::returned_info cri;
      try {
        if (! is_parameter()) {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_CONTEXT);
          DT_THROW(std::logic_error,
                   "Record '" << _path_ << "' is not a parameter!");
        }
        if (! get_parameter_model().is_real()) {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_TYPE);
          DT_THROW(std::logic_error,
                   "Parameter record '" << _path_ << "' is not a real value!");
        }
        if (! value_is_set()) {
          cri.set_error_code(command::CEC_PARAMETER_UNSET_VALUE);
          DT_THROW(std::logic_error,
                   "Parameter record '" << _path_ << "' is not set!");
        }
        value_ = _real_value_;
      } catch (std::exception & x) {
        cri.set_error_message(x.what());
      }
      return cri;
    }

    command::returned_info
    variant_record::get_string_value(std::string & value_) const
    {
      command::returned_info cri;
      try {
        if (! is_parameter()) {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_CONTEXT);
          DT_THROW(std::logic_error,
                   "Record '" << _path_ << "' is not a parameter!");
        }
        if (! get_parameter_model().is_string()) {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_TYPE);
          DT_THROW(std::logic_error,
                   "Parameter record '" << _path_ << "' is not a string value!");
        }
        if (! value_is_set()) {
          cri.set_error_code(command::CEC_PARAMETER_UNSET_VALUE);
          DT_THROW(std::logic_error,
                   "Parameter record '" << _path_ << "' is not set!");
        }
        value_ = _string_value_;
      } catch (std::exception & x) {
        cri.set_error_message(x.what());
      }
      return cri;
    }

    command::returned_info
    variant_record::string_to_value(const std::string & format_)
    {
      DT_LOG_DEBUG(_logging_, "Format = '" << format_ << "'");
      command::returned_info cri;
      try {
        if (! is_parameter()) {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_CONTEXT);
          DT_THROW(std::logic_error,
                   "Record '" << _path_ << "' is not a parameter!");
        }
        if (get_parameter_model().is_fixed()) {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_CONTEXT);
          DT_THROW(std::logic_error,
                   "Parameter record '" << _path_ << "' is fixed!");
        }
        std::istringstream value_iss(format_);
        if (get_parameter_model().is_boolean()) {
          DT_LOG_DEBUG(_logging_, "Boolean...");
          bool value;
          if (!io::read_boolean(value_iss, value)) {
            cri.set_error_code(command::CEC_PARSING_FAILURE);
            // DT_LOG_ERROR(_logging_, "Parsing of '" << format_ << "' failed!");
            DT_THROW(std::logic_error,
                     "Invalid format '" << format_ << "' for boolean record '" << _path_ << "'!");
          }
          cri = set_boolean_value(value);
          if (cri.is_failure()) {
            // DT_LOG_ERROR(_logging_, "Could not set boolean value [" << value << "]:" << cri.get_error_message());
          }
        } else if (get_parameter_model().is_integer()) {
          DT_LOG_DEBUG(_logging_, "Integer...");
          int value;
          if (!io::read_integer(value_iss, value)) {
            cri.set_error_code(command::CEC_PARSING_FAILURE);
            // DT_LOG_ERROR(_logging_, "Parsing of '" << format_ << "' failed!");
            DT_THROW(std::logic_error,
                     "Invalid format '" << format_ << " for integer record '" << _path_ << "'!");
          }
          cri = set_integer_value(value);
          if (cri.is_failure()) {
            // DT_LOG_ERROR(_logging_, "Could not set integer value [" << value << "]:" << cri.get_error_message());
          }
        } else if (get_parameter_model().is_real()) {
          DT_LOG_DEBUG(_logging_, "Real...");
          double value;
          std::string unit_symbol;
          std::string unit_label;
          bool ok = datatools::units::parse_value_with_unit(format_, value,
                                                            unit_symbol, unit_label);
          if (!ok) {
            cri.set_error_code(command::CEC_PARSING_FAILURE);
            // DT_LOG_ERROR(_logging_, "Parsing of '" << format_ << "' failed!");
            DT_THROW(std::logic_error,
                     "Invalid format '" << format_ << "' for real record '" << _path_ << "'!");
          }
          DT_LOG_DEBUG(_logging_, "Real value = " << value);
          DT_LOG_DEBUG(_logging_, "Real unit symbol = '" << unit_symbol << "'");
          DT_LOG_DEBUG(_logging_, "Real unit label = '" << unit_label << "'");
          if (get_parameter_model().has_real_unit_label()) {
            if (unit_label != get_parameter_model().get_real_unit_label()) {
              cri.set_error_code(command::CEC_PARAMETER_INVALID_UNIT);
              DT_THROW(std::logic_error,
                       "Invalid unit for real record '" << _path_ << "'!");
            }
          }
          // io::read_real_number(value_iss, value, 15,
          //                      unit_label, unit_symbol);
          DT_LOG_DEBUG(_logging_, "Setting real value = " << value);
          cri = set_real_value(value);
          if (cri.is_failure()) {
            // DT_LOG_ERROR(_logging_, "Could not set real value [" << value << "]:" << cri.get_error_message());
          }
        } else if (get_parameter_model().is_string()) {
          DT_LOG_DEBUG(_logging_, "String...");
          std::string value;
          if (!io::read_quoted_string(value_iss, value)) {
            cri.set_error_code(command::CEC_PARSING_FAILURE);
            // DT_LOG_ERROR(_logging_, "Parsing of '" << format_ << "' failed!");
            DT_THROW(std::logic_error,
                     "Invalid format for string record '" << _path_ << "'!");
          }
          cri = set_string_value(value);
          if (cri.is_failure()) {
            // DT_LOG_ERROR(_logging_, "Could not set string value [" << value << "]:" << cri.get_error_message());
          }
        } else {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_TYPE);
          DT_THROW(std::logic_error,
                   "Invalid parameter type for record '" << _path_ << "'!");
        }
      } catch (std::exception & x) {
        // DT_LOG_ERROR(_logging_, "Set value failure: " << x.what());
        cri.set_error_message(x.what());
      }
      return cri;
    }

    command::returned_info variant_record::value_is_set(bool & set_) const
    {
      command::returned_info cri;
      try {
        cri.set_error_code(command::CEC_COMMAND_INVALID_CONTEXT);
        bool check = value_is_set();
        set_ = check;
        cri.set_error_code(command::CEC_SUCCESS);
      } catch (std::exception & x) {
        cri.set_error_message(x.what());
      }
      return cri;
    }

    command::returned_info
    variant_record::value_to_string(std::string & format_) const
    {
      command::returned_info cri;
      try {
        if (! is_parameter()) {
          cri.set_error_code(command::CEC_COMMAND_INVALID_CONTEXT);
          DT_THROW( std::logic_error,
                    "Record '" << _path_ << "' is not a parameter!");
        }
        if (! value_is_set()) {
          cri.set_error_code(command::CEC_PARAMETER_UNSET_VALUE);
          DT_THROW(std::logic_error,
                   "Parameter record '" << _path_ << "' has no value set!");
        }
        std::ostringstream value_oss;
        if (get_parameter_model().is_boolean()) {
          io::write_boolean(value_oss, _boolean_value_, true);
        }
        if (get_parameter_model().is_integer()) {
          io::write_integer(value_oss, _integer_value_);
        }
        if (get_parameter_model().is_real()) {
          std::string unit_label;
          std::string unit_symbol;
          if (get_parameter_model().has_real_preferred_unit()) {
            unit_symbol = get_parameter_model().get_real_preferred_unit();
          }
          if (get_parameter_model().has_real_unit_label()) {
            unit_label = get_parameter_model().get_real_unit_label();
          }
          io::write_real_number(value_oss, _real_value_, 15,
                                unit_symbol, unit_label);
        }
        if (get_parameter_model().is_string()) {
          io::write_quoted_string(value_oss, _string_value_);
        }
        format_ = value_oss.str();
      } catch (std::exception & x) {
        cri.set_error_message(x.what());
      }
      return cri;
    }

    variant_record::daughter_dict_type &
    variant_record::grab_daughters()
    {
      return _daughters_;
    }

    bool variant_record::has_daughters() const
    {
      return _daughters_.size() > 0;
    }

    const variant_record::daughter_dict_type &
    variant_record::get_daughters() const
    {
      return _daughters_;
    }

    const variant_record & variant_record::get_daughter(const std::string & name_) const
    {
       daughter_dict_type::const_iterator found = _daughters_.find(name_);
      DT_THROW_IF(found == _daughters_.end(), std::domain_error,
                  "No daughter record with name '" << name_ << "'!");
      return found->second.get_record();
    }

    variant_record & variant_record::grab_daughter(const std::string & name_)
    {
      daughter_dict_type::iterator found = _daughters_.find(name_);
      DT_THROW_IF(found == _daughters_.end(), std::domain_error,
                  "No daughter record with name '" << name_ << "'!");
      return found->second.grab_record();
    }

    void variant_record::tree_dump(std::ostream & out_,
                                   const std::string & title_,
                                   const std::string & indent_,
                                   bool inherit_) const
    {
      if (! title_.empty()) out_ << indent_ << title_ << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Path: '" << _path_ << "'" << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Parent: ";
      if (has_parent()) {
        out_ << "'" << _parent_->get_path() << "'";
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "With update: '" << std::boolalpha << _with_update_ << "'" << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Daughters : [" << _daughters_.size() << "]" << std::endl;
      std::vector<std::string> daughters;
      if (is_variant()) {
        build_list_of_ranked_parameter_records(daughters);
      } else {
        for (daughter_dict_type::const_iterator i = _daughters_.begin();
             i != _daughters_.end();
             i++) {
          daughters.push_back(i->first);
        }
      }
      for (std::size_t idaughter = 0; idaughter < daughters.size(); idaughter++) {
        const std::string & daughter_name = daughters[idaughter];
        out_ << indent_ << i_tree_dumpable::skip_tag;
        if ((idaughter + 1) == daughters.size()) {
          out_ << i_tree_dumpable::last_tag;
        } else {
          out_ << i_tree_dumpable::tag;
        }
        // const variant_record & daughter_rec = found->second.get_record();
        out_ << "Daughter: '"
             << daughter_name << "'";
        // with path '"
        //      << daughter_rec.get_path() << "'";
        out_ << std::endl;
      }

      out_ << indent_ << i_tree_dumpable::tag
           << "Ranked daughters   : [" << _ranked_daughters_.size() << "]" << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Unranked daughters : [" << _unranked_daughters_.size() << "]" << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Model: ";
      if (is_parameter()) {
        out_ << "'" << get_parameter_model().get_name() << "'";
        out_ << " (parameter)";
      } else if (is_variant())  {
        out_ << "'" << get_variant_model().get_name() << "'";
        out_ << " (variant)";
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Active: " << std::boolalpha << is_active() << std::endl;

      if (is_parameter()) {

        out_ << indent_ << i_tree_dumpable::tag
             << "Value set: " << std::boolalpha << value_is_set() << std::endl;

        out_ << indent_ << i_tree_dumpable::tag
             << "Value: ";
        if (value_is_set()) {
          if (get_parameter_model().is_boolean()) {

            bool value;
            get_boolean_value(value);
            io::write_boolean(out_, value);

          } else if (get_parameter_model().is_integer()) {

            int value;
            get_integer_value(value);
            io::write_integer(out_, value);

          } else if (get_parameter_model().is_real()) {

            double value;
            get_real_value(value);
            std::string unit_symbol;
            std::string unit_label;
            if (get_parameter_model().has_real_unit_label()) {
              unit_label = get_parameter_model().get_real_unit_label();
            }
            if (get_parameter_model().has_real_preferred_unit()) {
              unit_symbol = get_parameter_model().get_real_preferred_unit();
            }
            io::write_real_number(out_, value, 15, unit_symbol, unit_label);

          } else if (get_parameter_model().is_string()) {

            std::string value;
            get_string_value(value);
            io::write_quoted_string(out_,value);

          }
        } else {
          out_ << "<unset>";
        }
        if (is_fixed_parameter()) {
          out_ << " (fixed)";
        } else {
          if (default_value_is_set()) {
            out_ << " (default value)";
          }
        }
        out_ << std::endl;

      }
      out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
           << "Valid: " << is_valid() << std::endl;

      return;
    }

  } // end of namespace configuration

} // end of namespace datatools
