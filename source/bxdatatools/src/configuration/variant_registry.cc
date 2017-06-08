// datatools/configuration/variant_registry.cc
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
#include <datatools/configuration/variant_registry.h>

// Standard library:
#include <sstream>
#include <fstream>

// This project:
#include <datatools/exception.h>
#include <datatools/properties.h>
#include <datatools/configuration/variant_model.h>
#include <datatools/configuration/variant_physical.h>
#include <datatools/configuration/parameter_model.h>
#include <datatools/configuration/parameter_physical.h>
#include <datatools/configuration/variant_registry_manager.h>
#include <datatools/configuration/variant_record.h>
#include <datatools/configuration/variant_object_info.h>
#include <datatools/configuration/i_occurrence.h>
#include <datatools/ioutils.h>
#include <datatools/units.h>
#include <datatools/command_utils.h>

namespace datatools {

  namespace configuration {

    // static
    const std::string & variant_registry::default_top_variant_name()
    {
      static const std::string _n("/");
      return _n;
    }

    // virtual
    bool variant_registry::is_name_valid(const std::string & name_) const
    {
      return ::datatools::configuration::validate_instance_name(name_);
    }

    variant_registry::variant_registry()
    {
      _initialized_ = false;
      return;
    }

    variant_registry::~variant_registry()
    {
      if (is_initialized()) {
        reset();
      }
      return;
    }

    variant_registry::record_dict_type & variant_registry::grab_records()
    {
      return _records_;
    }

    const variant_registry::record_dict_type & variant_registry::get_records() const
    {
      return _records_;
    }

    bool variant_registry::has_dependency_model() const
    {
      return _dependency_model_.get() != nullptr;
    }

    const variant_dependency_model & variant_registry::get_dependency_model() const
    {
      DT_THROW_IF(_dependency_model_.get() == nullptr,
                  std::logic_error,
                  "Repository '" << get_name() << "' has no dependency model!");
      return *_dependency_model_.get();
    }

    void variant_registry::load_local_dependency_model(const datatools::properties & ldm_config_)
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Registry is locked!");
      DT_LOG_DEBUG(get_logging_priority(), "Loading local dependency model for registry '" << get_name() << "'...");
      _dependency_model_.reset(new variant_dependency_model(*this));
      _dependency_model_->initialize(ldm_config_);
      return;
    }

    bool variant_registry::is_initialized() const
    {
      return _initialized_;
    }

    void variant_registry::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Registry is not initialized!");
      if (has_dependency_model()) {
        _dependency_model_.reset();
      }
      _initialized_ = false;
      _top_variant_name_.clear();
      _records_.clear();
      this->enriched_base::reset();
      return;
    }

    void variant_registry::tree_dump(std::ostream & out_,
                                     const std::string & title_,
                                     const std::string & indent_,
                                     bool inherit_) const
    {
      this-> enriched_base::tree_dump(out_, title_, indent_, true);

      out_ << indent_ << i_tree_dumpable::tag
           << "Initialized : " << is_initialized() << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Top variant : '" << get_top_variant_name() << "'" << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Records     : " << std::endl;
      for (record_dict_type::const_iterator i = _records_.begin();
           i != _records_.end();
           i++) {
        out_ << indent_ << i_tree_dumpable::skip_tag;
        record_dict_type::const_iterator j = i;
        std::ostringstream indent_ss;
        indent_ss << indent_ << i_tree_dumpable::skip_tag;
        if (++j == _records_.end()) {
          out_ << i_tree_dumpable::last_tag;
          indent_ss << i_tree_dumpable::last_skip_tag;
        } else {
          out_ << i_tree_dumpable::tag;
          indent_ss << i_tree_dumpable::skip_tag;
        }
        const std::string & path = i->first;
        const variant_record & rec = i->second;
        out_ << "Path '" << path << "' : ";
        out_ << std::endl;
        rec.tree_dump(out_, "", indent_ss.str());
        // if (rec.value_is_set()) {
        //   out_ << "value";
        // } else {
        //   out_ << "<no value>";
        // }
        // out_ << std::endl;
      }

      out_ << indent_ << i_tree_dumpable::tag
           << "Dependency model : ";
      if (!has_dependency_model()) {
        out_ << "<none>";
      }
      out_ << std::endl;
      if (has_dependency_model()) {
        std::ostringstream indent2_oss;
        indent2_oss << indent_ << i_tree_dumpable::skip_tag;
        _dependency_model_->tree_dump(out_, "", indent2_oss.str());
      }

      std::vector<std::string> unset_params;
      list_of_unset_parameters(unset_params);
      out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
           << "Accomplished : "
           << (unset_params.size() == 0)
           << std::endl;
      if (unset_params.size()) {
        for (size_t i = 0; i < unset_params.size(); i++) {
          out_ << indent_ << i_tree_dumpable::inherit_skip_tag(inherit_);
          size_t j = i;
          if (++j == unset_params.size()) {
            out_ << i_tree_dumpable::last_tag;
          } else {
            out_ << i_tree_dumpable::tag;
          }
          out_ << "Unset: '" << unset_params[i] << "'"
               << std::endl;
        }
      }

      return;
    }

    const std::string & variant_registry::get_top_variant_name() const
    {
      return _top_variant_name_;
    }

    const variant_record & variant_registry::get_top_variant_record() const
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Registry is not initialized!");
      return get_variant_record("");
    }

    bool variant_registry::is_mounted() const
    {
      return has_parent_repository();
    }

    const std::string & variant_registry::get_mounting_name() const
    {
      return _mounting_name_;
    }

    bool variant_registry::has_parent_repository() const
    {
      return _parent_repository_ != nullptr;
    }

    void variant_registry::set_parent_repository(const variant_repository & rep_,
                                                 const std::string & mounting_name_)
    {
      DT_THROW_IF(mounting_name_.empty(), std::logic_error,
                  "Invalid empty registry mounting name!");
      _parent_repository_ = &rep_;
      _mounting_name_ = mounting_name_;
      return;
    }

    void variant_registry::reset_parent_repository()
    {
      _parent_repository_ = nullptr;
      _mounting_name_.clear();
      return;
    }

    const variant_repository & variant_registry::get_parent_repository() const
    {
      DT_THROW_IF(!has_parent_repository(), std::logic_error,
                  "Registry has no parent repository!");
      return *_parent_repository_;
    }

    bool variant_registry::has_top_variant_name() const
    {
      return ! _top_variant_name_.empty();
    }

    void variant_registry::initialize_from(const variant_registry_manager & mgr_,
                                           const std::string & top_variant_name_,
                                           const std::string & name_,
                                           const std::string & display_name_,
                                           const std::string & terse_description_)
    {
      DT_LOG_TRACE_ENTERING(get_logging_priority());
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Registry is already initialized!");

      DT_THROW_IF(!mgr_.is_initialized(),
                  std::logic_error,
                  "Variant manager is not initialized!");

      // Set the top variant name:
      std::string top_variant_name = top_variant_name_;
      if (top_variant_name.empty()) {
        DT_THROW_IF(mgr_.get_top_variant_name().empty(), std::logic_error,
                    "Variant registry manager has no top variant defined!");
        top_variant_name = mgr_.get_top_variant_name();
      }
      DT_THROW_IF(!model_item::has_item(mgr_.get_configuration_items(), top_variant_name, model_item::MODEL_VARIANT),
                  std::logic_error,
                  "Variant registry manager has no variant named '" << top_variant_name << "' !");
      DT_LOG_DEBUG(get_logging_priority(), "Add variant record '" << top_variant_name << "'...");
      const variant_model & top_variant_model =
        model_item::get_variant_model(mgr_.get_configuration_items(), top_variant_name);
      variant_record & top_variant_record = add_record(default_top_variant_name());
      top_variant_record.set_variant_model(top_variant_model);
      top_variant_record.set_active(true);
      DT_LOG_DEBUG(get_logging_priority(), "Building parameter records for variant record '" << top_variant_name << "'...");
      _build_parameter_records_from_variant_(top_variant_model, &top_variant_record);
      top_variant_record.build();
      _top_variant_name_ = top_variant_name;
      DT_LOG_DEBUG(get_logging_priority(),
                   "Variant record '" << top_variant_name << "' is set.");

      if (!name_.empty()) {
        set_name(name_);
      }

      if (!display_name_.empty()) {
        set_display_name(display_name_);
      }

      if (!terse_description_.empty()) {
        set_terse_description(terse_description_);
      }

      if (!has_name()) {
        if (mgr_.has_name()) {
          // Fetch the name of the registry manager:
          DT_LOG_TRACE(get_logging_priority(),
                       "Mgr name = '" << mgr_.get_name() << "'");
          set_name(mgr_.get_name());
        }
      }

      if (!has_display_name()) {
        if (mgr_.has_display_name()) {
          // Fetch the display name of the registry manager:
          DT_LOG_TRACE(get_logging_priority(),
                       "Mgr display name = '" << mgr_.get_display_name() << "'");
          set_display_name(mgr_.get_display_name());
        }
      }

      if (!has_terse_description()) {
        if (mgr_.has_terse_description()) {
          // Fetch the terse description of the registry manager:
          DT_LOG_TRACE(get_logging_priority(),
                       "Mgr terse description = '" << mgr_.get_terse_description() << "'");
          set_terse_description(mgr_.get_terse_description());
        }
      }

      _initialized_ = true;
      DT_LOG_TRACE_EXITING(get_logging_priority());
      return;
    }

    variant_record & variant_registry::add_record(const std::string & record_path_)
    {
      DT_THROW_IF(record_path_.empty(), std::logic_error,
                  "Empty record path is not allowed!");
      record_dict_type::const_iterator found = _records_.find(record_path_);
      DT_THROW_IF(found != _records_.end(), std::logic_error,
                  "Record with path '" << record_path_ << "' already exists!");
     {
        variant_record dummy;
        _records_[record_path_] = dummy;
      }
      variant_record & rec = _records_.find(record_path_)->second;
      rec.set_path(record_path_);
      rec.set_parent_registry(*this);
      return rec;
    }

    bool variant_registry::has_variant_record(const std::string & record_path_) const
    {
      record_dict_type::const_iterator found = _records_.find(record_path_);
      return found != _records_.end() && found->second.is_variant();
    }

    variant_record &
    variant_registry::grab_variant_record(const std::string & record_path_)
    {
      DT_THROW_IF(!has_variant_record(record_path_), std::logic_error,
                  "Variant record with path '" << record_path_ << "' does not exist!");
      record_dict_type::iterator found = _records_.find(record_path_);
      return found->second;
    }

    const variant_record &
    variant_registry::get_variant_record(const std::string & record_path_) const
    {
      DT_THROW_IF(!has_variant_record(record_path_), std::logic_error,
                  "Variant record with path '" << record_path_ << "' does not exist!");
      record_dict_type::const_iterator found = _records_.find(record_path_);
      return found->second;
    }

    bool variant_registry::parameter_exists(const std::string & variant_path_) const
    {
      if (!has_variant_record(variant_path_)) return false;
      const variant_record & variant_rec = get_variant_record(variant_path_);
      if (!variant_rec.is_parameter()) return false;
      return true;
    }

    bool variant_registry::variant_exists(const std::string & variant_path_) const
    {
      if (!has_variant_record(variant_path_)) return false;
      const variant_record & variant_rec = get_variant_record(variant_path_);
      if (!variant_rec.is_variant()) return false;
      return true;
    }

    bool variant_registry::is_active_variant(const std::string & variant_path_) const
    {
      const variant_record & variant_rec = get_variant_record(variant_path_);
      return variant_rec.is_active();
    }

    bool variant_registry::has_parameter_record(const std::string & record_path_) const
    {
      record_dict_type::const_iterator found = _records_.find(record_path_);
      return found != _records_.end() && found->second.is_parameter();
    }

    bool variant_registry::has_parameter_record_value_group(const std::string & param_value_group_path_) const
    {
      variant_object_info voi;
      uint32_t parsing_flag = 0;
      parsing_flag |= variant_object_info::PARSE_NO_GLOBAL;
      DT_THROW_IF(!voi.parse_from_string(param_value_group_path_, parsing_flag),
                  std::logic_error,
                  "Invalid variant object path format '" << param_value_group_path_ << "'!");
      DT_THROW_IF(!voi.is_local(),
                  std::logic_error,
                  "Variant path '" << param_value_group_path_ << "' is not local!");
      DT_THROW_IF(!voi.is_parameter_value_group(),
                  std::logic_error,
                  "Variant object path '" << param_value_group_path_ << "' is not a parameter value group!");
      record_dict_type::const_iterator found = _records_.find(voi.get_parameter_local_path());
      if (found == _records_.end()) {
        return false;
      }
      const std::string & group_name = voi.get_parameter_value_group_name();
      const variant_record & vrec = found->second;
      const parameter_model & parmod = vrec.get_parameter_model();
      if (!parmod.has_group(group_name)) {
        return false;
      }
      return true;
    }

    variant_record &
    variant_registry::grab_parameter_record(const std::string & record_path_)
    {
      DT_THROW_IF(!has_parameter_record(record_path_), std::logic_error,
                  "Parameter record with path '" << record_path_ << "' does not exist!");
      record_dict_type::iterator found = _records_.find(record_path_);
      return found->second;
    }

    const variant_record &
    variant_registry::get_parameter_record(const std::string & record_path_) const
    {
      DT_THROW_IF(!has_parameter_record(record_path_), std::logic_error,
                  "Parameter record with path '" << record_path_ << "' does not exist!");
      record_dict_type::const_iterator found = _records_.find(record_path_);
      return found->second;
    }

    void variant_registry::list_of_ranked_parameters(std::vector<std::string> & paths_) const
    {
       paths_.clear();
       const variant_record & top_vrec = _records_.find(default_top_variant_name())->second;
       if (datatools::logger::is_debug(get_logging_priority())) {
         top_vrec.tree_dump(std::cerr, "Top variant record: ", "[debug] ");
       }
       top_vrec.build_list_of_ranked_parameter_records(paths_);
       return;
    }

    void variant_registry::build_recursive_list_of_ranked_records(const variant_record & record_,
                                                                  std::vector<std::string> & ranked_) const
    {
      if (record_.is_variant()) {
        std::vector<std::string> ranked_param_records;
        record_.build_list_of_ranked_parameter_records(ranked_param_records);
        for (std::size_t ipar = 0; ipar < ranked_param_records.size(); ipar++) {
          std::ostringstream path_prefix_ss;
          if (record_.get_path() != default_top_variant_name()) {
            path_prefix_ss << record_.get_path() << '/';
          }
          path_prefix_ss << ranked_param_records[ipar];
          std::string path_prefix = path_prefix_ss.str();
          // std::cerr << "DEVEL: Ranked parameter record path = '" << path_prefix << "'" << std::endl;
          ranked_.push_back(path_prefix);
          const variant_record & par_record = record_.get_parent_registry().get_parameter_record(path_prefix);
          // std::cerr << "DEVEL: Push ranked parameter record '" << path_prefix << "'" << std::endl;
          build_recursive_list_of_ranked_records(par_record, ranked_);
        }
      } else {
        std::vector<std::string> ranked_var_records;
        for (const auto & var_daughter : record_.get_daughters()) {
          std::ostringstream path_prefix_ss;
          if (record_.get_path() != default_top_variant_name()) {
            path_prefix_ss << record_.get_path() << '/';
          }
          path_prefix_ss << var_daughter.first;
          std::string path_prefix = path_prefix_ss.str();
          // std::cerr << "DEVEL: Variant record path = '" << path_prefix << "'" << std::endl;
          // std::string path = var_daughter.second.get_record().get_path();
          ranked_.push_back(path_prefix);
          const variant_record & var_record = record_.get_parent_registry().get_variant_record(path_prefix);
          // std::cerr << "DEVEL: Push variant record '" << path_prefix << "'" << std::endl;
          build_recursive_list_of_ranked_records(var_record, ranked_);
        }
      }

      return;
    }

    void variant_registry::list_of_ranked_records(std::vector<std::string> & paths_,
                                                  uint32_t flags_) const
    {
      paths_.clear();
      bool with_parameters = true;
      bool with_variants = true;
      if (flags_ & LIST_NO_PARAMETERS) {
        with_parameters = false;
      }
      if (flags_ & LIST_NO_VARIANTS) {
        with_variants = false;
      }
      const variant_record & var_rec = _records_.find(default_top_variant_name())->second;
      std::vector<std::string> paths;
      build_recursive_list_of_ranked_records(var_rec, paths);
      for (const auto & path : paths) {
        // std::cerr << "********** DEVEL: path = '" << path << "'" << std::endl;
        const variant_record & rec = _records_.find(path)->second;
        if (rec.is_parameter() && !with_parameters) continue;
        if (rec.is_variant() && !with_variants) continue;
        paths_.push_back(rec.get_path());
      }
      return;
    }

    void variant_registry::list_of_parameters(std::vector<std::string> & paths_,
                                              uint32_t flags_) const
    {
      if (flags_ & LIST_CLEAR) {
        paths_.clear();
      }
      for (record_dict_type::const_iterator i = _records_.begin();
           i != _records_.end();
           i++) {
        const variant_record & rec = i->second;
        if (rec.is_parameter()) {
          bool add_it = true;
          if (flags_ & LIST_ACTIVE_ONLY) {
            if (!rec.is_active()) {
              add_it = false;
            }
          }
          if (flags_ & LIST_NO_SET) {
            if (rec.value_is_set()) {
              add_it = false;
            }
          }
          if (flags_ & LIST_NO_UNSET) {
            if (!rec.value_is_set()) {
              add_it = false;
            }
          }
          if (add_it) {
            paths_.push_back(rec.get_path());
          }
        }
      }
      return;
    }

    void variant_registry::list_of_unset_parameters(std::vector<std::string> & unset_paths_) const
    {
      for (record_dict_type::const_iterator i = _records_.begin();
           i != _records_.end();
           i++) {
        const variant_record & rec = i->second;
        if (rec.is_parameter() && rec.is_active() && !rec.value_is_set()) {
          unset_paths_.push_back(rec.get_path());
        }
      }
      return;
    }

    bool variant_registry::is_accomplished() const
    {
      std::vector<std::string> unset_paths;
      list_of_unset_parameters(unset_paths);
      return unset_paths.size() == 0;
    }

    void variant_registry::update()
    {
      DT_LOG_TRACE_ENTERING(get_logging_priority());
      std::vector<std::string> param_paths;
      list_of_ranked_parameters(param_paths);
      DT_LOG_DEBUG(get_logging_priority(), "Number of ranked parameters = " << param_paths.size());
      for (std::size_t iparam = 0; iparam < param_paths.size(); iparam++) {
        variant_record & prec = _records_.find(param_paths[iparam])->second;
        DT_LOG_DEBUG(get_logging_priority(), "Updating variant parameter record '" << param_paths[iparam] << "'...");
        // // XXX
        // if (param_paths[iparam] == "layout") {
        //   prec.set_logging(datatools::logger::PRIO_TRACE);
        // }
        prec.update();
        DT_LOG_DEBUG(get_logging_priority(), "Variant parameter record '" << param_paths[iparam] << "' has been updated.");
      }
      DT_LOG_TRACE_EXITING(get_logging_priority());
      return;
    }

    void variant_registry::_build_parameter_records_from_variant_(const variant_model & variant_model_,
                                                                  variant_record * parent_variant_record_)
    {
      DT_LOG_TRACE_ENTERING(get_logging_priority());
      DT_LOG_DEBUG(get_logging_priority(), "Building variant parameters from variant model '" << variant_model_.get_name() << "'...");
      std::vector<std::string> ranked_params;
      variant_model_.build_list_of_ranked_parameters(ranked_params);
      int current_rank = 0;
      for (std::size_t iparam = 0; iparam < ranked_params.size(); iparam++) {
        const std::string & param_name = ranked_params[iparam];
        DT_LOG_DEBUG(get_logging_priority(), "Building variant parameter named '" << param_name << "'...");
        const variant_model::parameter_record & prec = variant_model_.get_parameters().find(param_name)->second;
        const parameter_physical & pe = prec.physical;
        std::string param_path_prefix;
        if (parent_variant_record_ != nullptr) {
          if (parent_variant_record_->get_path() != default_top_variant_name()) {
            param_path_prefix = parent_variant_record_->get_path() + '/';
          }
        }
        const i_occurrence & occ = pe.get_occurrence();
        for (int j = 0; j < (int) occ.get_number_of_occurrences(); j++) {
          std::vector<uint32_t> indexes;
          occ.compute_index_path(indexes, j);
          std::ostringstream param_name_full_oss;
          param_name_full_oss << param_name;
          for (int k = 0; k < (int) indexes.size(); k++) {
            param_name_full_oss << '[' << indexes[k] << ']';
          }
          std::string param_name_full = param_name_full_oss.str();
          std::ostringstream param_path_oss;
          param_path_oss << param_path_prefix << param_name_full;
          std::string param_path = param_path_oss.str();
          DT_LOG_DEBUG(get_logging_priority(), "Adding param record '" << param_path << "'...");
          variant_record & param_rec = add_record(param_path);
          if (parent_variant_record_ != nullptr) {
            param_rec.set_parent(*parent_variant_record_, param_name_full, current_rank++);
          }
          param_rec.set_parameter_model(pe.get_model());
          // Set logging level:
          param_rec.set_logging(pe.get_model().get_logging_priority());
          if (parent_variant_record_ != nullptr && parent_variant_record_->is_active()) {
            param_rec.set_active(true);
            DT_LOG_DEBUG(get_logging_priority(), "param_rec.set_active(true).");
          } else {
            param_rec.set_active(true);
          }
          if (pe.get_model().is_variable()) {
            // Variable parameter:
            DT_LOG_DEBUG(get_logging_priority(), "Variable parameter '" << param_rec.get_path() << "'");
            if (pe.get_model().has_default_value()) {
              param_rec.set_default_value();
              DT_LOG_DEBUG(get_logging_priority(), "set_default_value() done");
            } else {
              param_rec.unset_value();
              DT_LOG_DEBUG(get_logging_priority(), "unset_value() done.");
            }
            if (datatools::logger::is_debug(get_logging_priority())) {
              param_rec.tree_dump(std::cerr, "Variable parameter record: ", "[debug] ");
            }
          } else {
            // Fixed parameter:
            DT_THROW_IF (!pe.get_model().has_fixed_value(), std::logic_error,
                         "Fixed parameter model '" << pe.get_model().get_name() << "' has no fixed value!");
            DT_LOG_DEBUG(get_logging_priority(), "fixed param_rec = '" << param_rec.get_path() << "'");
            param_rec.set_fixed_value();
            DT_LOG_DEBUG(get_logging_priority(), "param_rec.set_fixed_value() done.");
            if (datatools::logger::is_debug(get_logging_priority())) {
              param_rec.tree_dump(std::cerr, "Fixed parameter record: ", "[debug] ");
            }
          }
          _build_variant_records_from_parameter_(pe.get_model(), &param_rec);
          DT_LOG_DEBUG(get_logging_priority(), "===> Building parameter record '" << param_rec.get_path() << "'...");
          param_rec.build();
          DT_LOG_DEBUG(get_logging_priority(), "===> Variable parameter '" << param_rec.get_path() << "' is built.");
        }
      }
      DT_LOG_TRACE_EXITING(get_logging_priority());
      return;
    }

    void variant_registry::_build_variant_records_from_parameter_(const parameter_model & param_model_,
                                                                  variant_record * parent_param_record_)
    {
      DT_LOG_TRACE_ENTERING(get_logging_priority());
      DT_LOG_DEBUG(get_logging_priority(), "Building variant records from variant parameter '" << param_model_.get_name() << "'...");
      for (parameter_model::variant_dict_type::const_iterator
             i = param_model_.get_variants().begin();
           i != param_model_.get_variants().end();
           i++) {
        const std::string & variant_name = i->first;
        DT_LOG_DEBUG(get_logging_priority(), "Building variant record named '" << variant_name << "'...");
        const variant_physical & ve = i->second;
        std::string variant_path_prefix;
        if (parent_param_record_ != nullptr) {
          variant_path_prefix = parent_param_record_->get_path() + '/';
        }
        std::string variant_path = variant_path_prefix + variant_name;
        DT_LOG_DEBUG(get_logging_priority(), "Add variant record '" << variant_path << "' ");
        variant_record & variant_rec = add_record(variant_path);
        if (parent_param_record_ != nullptr) {
          variant_rec.set_parent(*parent_param_record_, variant_name);
        }
        variant_rec.set_variant_model(ve.get_model());
        DT_LOG_DEBUG(get_logging_priority(), "Default behaviour: force variant record '" << variant_path << "' to be active.");
        variant_rec.set_active(true);
        _build_parameter_records_from_variant_(ve.get_model(), &variant_rec);
        DT_LOG_DEBUG(get_logging_priority(), "=========> Building variant record '" << variant_path << "'...");
        variant_rec.build();
      }
      DT_LOG_TRACE_EXITING(get_logging_priority());
      return;
    }

  } // end of namespace configuration

} // end of namespace datatools
