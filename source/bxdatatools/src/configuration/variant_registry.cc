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
      // std::cerr << "DEVEL: variant_registry::is_name_valid: name = '" << name_ << "'" << std::endl;
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

    bool variant_registry::is_initialized() const
    {
      return _initialized_;
    }

    void variant_registry::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Registry is not initialized!");
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

    void variant_registry::initialize_from(const variant_registry_manager & mgr_,
                                           const std::string & top_variant_name_,
                                           const std::string & name_,
                                           const std::string & display_name_,
                                           const std::string & terse_description_)
    {
      bool devel = false;
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Registry is already initialized!");

      DT_THROW_IF(!mgr_.is_initialized(),
                  std::logic_error,
                  "Variant manager is not initialized!");

      std::string top_variant_name = top_variant_name_;
      if (top_variant_name.empty()) {
        DT_THROW_IF(mgr_.get_top_variant_name().empty(), std::logic_error,
                    "Variant registry manager has no top variant defined!");
        top_variant_name = mgr_.get_top_variant_name();
      }
      DT_THROW_IF(!item::has_item(mgr_.get_configuration_items(), top_variant_name, item::MODEL_VARIANT),
                  std::logic_error,
                  "Variant registry manager has no variant named '" << top_variant_name << "' !");
      const variant_model & top_variant_model =
        item::get_variant_model(mgr_.get_configuration_items(), top_variant_name);
      variant_record & top_variant_record = add_record(default_top_variant_name());
      top_variant_record.set_variant_model(top_variant_model);
      top_variant_record.set_active(true);
      _build_parameter_records_from_variant(top_variant_model, &top_variant_record);
      _top_variant_name_ = top_variant_name;

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
          if (devel) std::cerr << "DEVEL: variant_registry::initialize_from: Mgr name = '" << mgr_.get_name() << "'" << std::endl;
          set_name(mgr_.get_name());
        }
      }

      if (!has_display_name()) {
        if (mgr_.has_display_name()) {
          // Fetch the display name of the registry manager:
          if (devel) std::cerr << "DEVEL: variant_registry::initialize_from: Mgr display name = '" << mgr_.get_display_name() << "'" << std::endl;
          set_display_name(mgr_.get_display_name());
        }
      }

      if (!has_terse_description()) {
        if (mgr_.has_terse_description()) {
          // Fetch the terse description of the registry manager:
          if (devel) std::cerr << "DEVEL: variant_registry::initialize_from: Mgr terse description = '" << mgr_.get_terse_description() << "'" << std::endl;
          set_terse_description(mgr_.get_terse_description());
        }
      }

      _initialized_ = true;

      return;
    }

    variant_record & variant_registry::add_record(const std::string & record_path_)
    {
      // DT_THROW_IF(record_path_.empty(), std::logic_error,
      //             "Empty record path is not allowed!");
      record_dict_type::const_iterator found = _records_.find(record_path_);
      DT_THROW_IF(found != _records_.end(), std::logic_error,
                  "Record with path '" << record_path_ << "' already exists!");
     {
        variant_record dummy;
        _records_[record_path_] = dummy;
      }
      variant_record & rec = _records_.find(record_path_)->second;
      rec.set_path(record_path_);
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

    bool variant_registry::has_parameter_record(const std::string & record_path_) const
    {
      record_dict_type::const_iterator found = _records_.find(record_path_);
      return found != _records_.end() && found->second.is_parameter();
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

    command::returned_info
    variant_registry::cmd_has_variant(const std::string & variant_path_,
                                      bool & existing_) const
    {
      command::returned_info cri;
      cri.set_error_code(command::CEC_SUCCESS);
      existing_ = has_variant_record(variant_path_);
      return cri;
    }

    command::returned_info
    variant_registry::cmd_is_active_variant(const std::string & variant_path_,
                                            bool & active_) const
    {
      command::returned_info cri;
      try {
        cri.set_error_code(command::CEC_PARAMETER_INVALID_KEY);
        // tree_dump(std::cerr, "Registry: ", "DEVEL: ***** ");
        DT_THROW_IF(!has_variant_record(variant_path_), std::logic_error,
                    "Variant record with path '" << variant_path_ << "' does not exist!");
        const variant_record & variant_rec = get_variant_record(variant_path_);
        cri.set_error_code(command::CEC_PARAMETER_INVALID_CONTEXT);
        active_ = variant_rec.is_active();
        // std::cerr << "DEVEL: *************** ACTIVE = " << active_ << std::endl;
        cri.set_error_code(command::CEC_SUCCESS);
      }
      catch (std::exception & x) {
        cri.set_error_message(x.what());
      }
      return cri;
    }

    command::returned_info
    variant_registry::cmd_get_parameter_value(const std::string & param_path_,
                                              std::string & value_token_) const
    {
      command::returned_info cri;
      try {
        cri.set_error_code(command::CEC_PARAMETER_INVALID_KEY);
        DT_THROW_IF(!has_parameter_record(param_path_), std::logic_error,
                    "Parameter record with path '" << param_path_ << "' does not exist!");
        const variant_record & param_rec = get_parameter_record(param_path_);
        cri.set_error_code(command::CEC_PARAMETER_INVALID_CONTEXT);
        DT_THROW_IF (!param_rec.is_active(), std::logic_error,
                    "Parameter record '" << param_path_ << "' is not active!");

        cri.set_error_code(command::CEC_PARAMETER_INVALID_TYPE);
        if (param_rec.get_parameter_model().is_boolean()) {

          std::ostringstream os;
          bool value = false;
          cri = param_rec.get_boolean_value(value);
          DT_THROW_IF (cri.is_failure(), std::logic_error,
                       "Boolean parameter '" << param_path_ << "' has no available value!");
          io::write_boolean(os, value);
          value_token_ = os.str();

        } else if (param_rec.get_parameter_model().is_integer()) {

          std::ostringstream os;
          int value = 0;
          cri = param_rec.get_integer_value(value);
          DT_THROW_IF (cri.is_failure(), std::logic_error,
                       "Integer parameter '" << param_path_ << "' has no available value!");
          io::write_integer(os, value);
          value_token_ = os.str();

        } else if (param_rec.get_parameter_model().is_real()) {

          std::ostringstream os;
          double value;
          cri = param_rec.get_real_value(value);
          DT_THROW_IF (cri.is_failure(), std::logic_error,
                       "Real parameter '" << param_path_ << "' has no available value!");
          io::write_real_number(os,
                                value,
                                15,
                                param_rec.get_parameter_model().get_real_preferred_unit(),
                                param_rec.get_parameter_model().get_real_unit_label()
                                );
          value_token_ = os.str();

          // value_token_ = "__REAL_VALUE__";
          // cri.set_error_message("Not implemented yet!");

        } else if (param_rec.get_parameter_model().is_string()) {

          std::ostringstream os;
          std::string value;
          cri = param_rec.get_string_value(value);
          io::write_quoted_string(os, value);
          value_token_ = os.str();
          // value_token_ = "__STRING_VALUE__";
          // cri.set_error_message("Not implemented yet!");

        } else {
          DT_THROW_IF (true, std::logic_error,
                       "Parameter '" << param_path_ << "' has no known type!");
        }
      }
      catch (std::exception & x) {
        cri.set_error_message(x.what());
      }
      return cri;
    }

    command::returned_info
    variant_registry::cmd_set_parameter_value(const std::string & param_path_,
                                              const std::string & value_token_)
    {
      command::returned_info cri;
      try {
        cri.set_error_code(command::CEC_PARAMETER_INVALID_KEY);
        DT_THROW_IF(!has_parameter_record(param_path_), std::logic_error,
                    "Parameter record with path '" << param_path_ << "' does not exist!");
        variant_record & param_rec = grab_parameter_record(param_path_);
        cri.set_error_code(command::CEC_PARAMETER_INVALID_CONTEXT);
        DT_THROW_IF (!param_rec.is_active(), std::logic_error,
                    "Parameter record '" << param_path_ << "' is not active!");
        DT_THROW_IF (param_rec.get_parameter_model().is_fixed(), std::logic_error,
                     "Parameter record '" << param_path_ << "' has a fixed value!");
        cri.set_error_code(command::CEC_PARAMETER_INVALID_TYPE);
        if (param_rec.get_parameter_model().is_boolean()) {
          std::istringstream inss(value_token_);
          bool value;
          bool parsed = io::read_boolean(inss, value);
          DT_THROW_IF (!parsed, std::logic_error,
                       "Boolean parameter '" << param_path_ << "' cannot be parsed from '" << value_token_ << "'!");
          // cri.set_error_code(command::CEC_PARAMETER_INVALID_VALUE);
          cri = param_rec.set_boolean_value(value);
          DT_THROW_IF (cri.is_failure(), std::logic_error,
                       "Boolean parameter '" << param_path_ << "' cannot accept value='" << value << "'!");
          //cri.set_error_code(command::CEC_SUCCESS);
        } else if (param_rec.get_parameter_model().is_integer()) {
          std::istringstream inss(value_token_);
          int value;
          bool parsed = io::read_integer(inss, value);
          DT_THROW_IF (!parsed, std::logic_error,
                       "Integer parameter '" << param_path_ << "' cannot be parsed from '" << value_token_ << "'!");
          // cri.set_error_code(command::CEC_PARAMETER_INVALID_VALUE);
          cri = param_rec.set_integer_value(value);
          DT_THROW_IF (cri.is_failure(), std::logic_error,
                       "Integer parameter '" << param_path_ << "' cannot accept value='" << value << "'!");
          //cri.set_error_code(command::CEC_SUCCESS);
        } else if (param_rec.get_parameter_model().is_real()) {
          std::istringstream inss(value_token_);
          double value;
          std::string unit_label;
          bool parsed = units::find_value_with_unit(value_token_, value, unit_label);
          DT_THROW_IF (!parsed, std::logic_error,
                       "Real parameter '" << param_path_ << "' cannot be parsed from '" << value_token_ << "'!");
          cri = param_rec.set_real_value(value);
          DT_THROW_IF (cri.is_failure(), std::logic_error,
                       "Real parameter '" << param_path_ << "' cannot accept value='" << value_token_ << "'!");
          //cri.set_error_code(command::CEC_SUCCESS);
        } else if (param_rec.get_parameter_model().is_string()) {
          std::string value = value_token_;
          cri = param_rec.set_string_value(value);
          DT_THROW_IF (cri.is_failure(), std::logic_error,
                       "String parameter '" << param_path_ << "' cannot accept value='" << value << "'!");
          //cri.set_error_code(command::CEC_SUCCESS);
        } else {
          DT_THROW_IF (true, std::logic_error,
                       "Parameter '" << param_path_ << "' has no known type!");
        }
      }
      catch (std::exception & x) {
        cri.set_error_message(x.what());
      }
      // std::cerr << "DEVEL: CRI == " << cri << std::endl;
      return cri;
    }

    void variant_registry::_build_parameter_records_from_variant(const variant_model & variant_model_,
                                                                 variant_record * parent_variant_record_)
    {
      bool devel = false;
      for (variant_model::parameter_dict_type::const_iterator i
             = variant_model_.get_parameters().begin();
           i != variant_model_.get_parameters().end();
           i++) {
        const std::string & param_name = i->first;
        const parameter_physical & pe = i->second;
        std::string param_path_prefix;
        if (parent_variant_record_ != 0) {
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
          if (devel) std::cerr << "DEVEL: add param record '" << param_path << "' " << std::endl;
          variant_record & param_rec = add_record(param_path);
          param_rec.set_name(pe.get_name());
          param_rec.set_terse_description(pe.get_terse_description());
          if (parent_variant_record_ != 0) {
            param_rec.set_parent(*parent_variant_record_, param_name_full);
          }
          param_rec.set_parameter_model(pe.get_model());
          if (pe.get_model().is_variable()) {
            if (devel) std::cerr << "DEVEL: " << "_build_parameter_records_from_variant: "
                                 << "variable param_rec = " << param_rec.get_path()
                                 << std::endl;
            // Variable parameter:
            if (pe.get_model().has_default_value()) {
              param_rec.set_default_value();
              if (devel) std::cerr << "DEVEL: " << "_build_parameter_records_from_variant: "
                                   << "set_default_value() done."
                                   << std::endl;
            } else {
              param_rec.unset_value();
              if (devel) std::cerr << "DEVEL: " << "_build_parameter_records_from_variant: "
                                   << "unset_value() done."
                                   << std::endl;
            }
            if (devel) param_rec.tree_dump(std::cerr, "Variable parameter record: ", "DEVEL: ");
          } else {
            // Fixed parameter:
            DT_THROW_IF (!pe.get_model().has_fixed_value(), std::logic_error,
                         "Fixed parameter model '" << pe.get_model().get_name() << "' has no fixed value!");
            if (devel) std::cerr << "DEVEL: " << "_build_parameter_records_from_variant: "
                                 << "fixed param_rec = " << param_rec.get_path()
                                 << std::endl;
            param_rec.set_fixed_value();
            if (devel) std::cerr << "DEVEL: " << "_build_parameter_records_from_variant: "
                                 << "param_rec.set_fixed_value() done."
                                 << std::endl;
            if (devel)  param_rec.tree_dump(std::cerr, "Fixed parameter record: ", "DEVEL: ");
          }
          //
          _build_variant_records_from_parameter(pe.get_model(), &param_rec);
          if (parent_variant_record_ && parent_variant_record_->is_active()) { // == 0) {
            param_rec.set_active(true);
          } else {
            // XXX param_rec.set_active(true);
          }
        }
      }
      return;
    }

    void variant_registry::_build_variant_records_from_parameter(const parameter_model & param_model_,
                                                                 variant_record * parent_param_record_)
    {
      bool devel = false;
       for (parameter_model::variant_dict_type::const_iterator
             i = param_model_.get_variants().begin();
           i != param_model_.get_variants().end();
           i++) {
        const std::string & variant_name = i->first;
        const variant_physical & ve = i->second;
        std::string variant_path_prefix;
        if (parent_param_record_ != 0) {
          variant_path_prefix = parent_param_record_->get_path() + '/';
        }
        std::string variant_path = variant_path_prefix + variant_name;
        if (devel) std::cerr << "DEVEL: add variant record '" << variant_path << "' " << std::endl;
        variant_record & variant_rec = add_record(variant_path);
        variant_rec.set_name(ve.get_name());
        variant_rec.set_terse_description(ve.get_terse_description());
        if (parent_param_record_ != 0) {
          variant_rec.set_parent(*parent_param_record_, variant_name);
        }
        variant_rec.set_variant_model(ve.get_model());
        _build_parameter_records_from_variant(ve.get_model(), &variant_rec);
      }

      return;
    }

  }  // end of namespace configuration

}  // end of namespace datatools
