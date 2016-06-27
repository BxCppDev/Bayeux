// datatools/configuration/utils.cc
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
#include <datatools/configuration/utils.h>

// Standard library:
#include <sstream>

// Third party
// - Boost:
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

// This project:
#include <datatools/exception.h>
#include <datatools/multi_properties.h>
#include <datatools/configuration/parameter_model.h>
#include <datatools/configuration/variant_model.h>
#include <datatools/configuration/variant_repository.h>
#include <datatools/detail/command_macros.h>

namespace datatools {

  namespace configuration {

    bool validate_item_name(const std::string & candidate_)
    {
      if (candidate_.find("/") != std::string::npos ) {
        return false;
      }
      return true;
    }

    bool validate_instance_name(const std::string & candidate_)
    {
      return ::datatools::name_validation(candidate_, ::datatools::NV_INSTANCE);
    }

    bool validate_model_name(const std::string & candidate_)
    {
      return ::datatools::name_validation(candidate_, ::datatools::NV_MODEL);
    }

    // static
    const std::string & item::variant_suffix()
    {
      static const std::string s(".VM");
      return s;
    }

    // static
    const std::string & item::parameter_suffix()
    {
      static const std::string s(".PM");
      return s;
    }

    // static
    std::string item::label_from_model_type(model_type model_)
    {
      switch (model_) {
      case MODEL_PARAMETER :
        return "parameter";
      case MODEL_VARIANT :
        return "variant";
      default:
        return "";
      }
    }

    // static
    item::model_type item::model_type_from_label(const std::string & label_)
    {
      if (label_ == "parameter") {
        return MODEL_PARAMETER;
      } else if (label_ == "variant") {
        return MODEL_VARIANT;
      }
      return MODEL_UNDEFINED;
    }

    item::item()
    {
      _model_ = MODEL_UNDEFINED;
      return;
    }

    item::~item()
    {
      if (is_instantiated()) {
        reset();
      }
      return;
    }

    void item::reset()
    {
      DT_THROW_IF(!is_instantiated(),
                  std::logic_error,
                  "Item is not instantiated !");
      destroy();
      _model_ = MODEL_UNDEFINED;
      _config_.clear();
      _name_.clear();
      return;
    }

    const std::string & item::get_name() const
    {
      return _name_;
    }

    void item::set_name(const std::string & name_)
    {
      _name_ = name_;
      return;
    }

    bool item::is_parameter() const
    {
      return _model_ == MODEL_PARAMETER;
    }

    bool item::is_variant() const
    {
      return _model_ == MODEL_VARIANT;
    }

    bool item::has_model() const
    {
      return _model_ != MODEL_UNDEFINED;
    }

    void item::set_model(model_type model_)
    {
      _model_ = model_;
      return;
    }

    item::model_type item::get_model() const
    {
      return _model_;
    }

    const properties & item::get_config() const
    {
      return _config_;
    }

    properties & item::grab_config()
    {
      return _config_;
    }

    const parameter_model & item::get_parameter_model() const
    {
      DT_THROW_IF(!is_parameter() || !is_instantiated(),
                  std::logic_error,
                  "Item is not an instantiated parameter !");
      return _parameter_handle_.get();
    }

    parameter_model & item::grab_parameter_model()
    {
      DT_THROW_IF(!is_parameter() || !is_instantiated(),
                  std::logic_error,
                  "Item is not an instantiated parameter !");
      return _parameter_handle_.grab();
    }

    const variant_model & item::get_variant_model() const
    {
      DT_THROW_IF(!is_variant() || !is_instantiated(),
                  std::logic_error,
                  "Item is not an instantiated variant !");
      return _variant_handle_.get();
    }

    variant_model & item::grab_variant_model()
    {
      DT_THROW_IF(!is_variant() || !is_instantiated(),
                  std::logic_error,
                  "Item is not an instantiated variant !");
      return _variant_handle_.grab();
    }

    void item::set_parameter_handle(const pm_handle_type & hparam_)
    {
      DT_THROW_IF(!is_parameter(),
                  std::logic_error,
                  "Item is not a parameter !");
      _parameter_handle_ = hparam_;
      return;
    }

    void item::set_variant_handle(const vm_handle_type & hvar_)
    {
      DT_THROW_IF(!is_variant(),
                  std::logic_error,
                  "Item is not a variant !");
      _variant_handle_ = hvar_;
      return;
    }

    bool item::is_instantiated() const
    {
      if (is_parameter()) return _parameter_handle_.has_data();
      if (is_variant()) return _variant_handle_.has_data();
      return false;
    }

    void item::instantiate(item_dict_type & items_)
    {
      DT_THROW_IF(is_instantiated(),
                  std::logic_error,
                  "Item is already instantiated !");
      if (is_variant()) {
        variant_model * var = new variant_model;
        var->set_name(_name_);
        var->initialize(_config_, items_);
      } else if (is_parameter()) {
        parameter_model * param = new parameter_model;
        param->set_name(_name_);
        param->initialize(_config_, items_);
      }
      return;
    }

    void item::destroy()
    {
      DT_THROW_IF(!is_instantiated(),
                  std::logic_error,
                  "Item is not instantiated !");
      if (is_parameter()) _parameter_handle_.reset();
      if (is_variant()) _variant_handle_.reset();
      return;
    }

    const pm_handle_type & item::get_parameter_handle() const
    {
      DT_THROW_IF(!is_parameter() || !is_instantiated(),
                  std::logic_error,
                  "Item is not an instantiated parameter !");
      return _parameter_handle_;
    }

    pm_handle_type & item::grab_parameter_handle()
    {
      DT_THROW_IF(!is_parameter() || !is_instantiated(),
                  std::logic_error,
                  "Item is not an instantiated parameter !");
      return _parameter_handle_;
    }

    const vm_handle_type & item::get_variant_handle() const
    {
      DT_THROW_IF(!is_variant() || !is_instantiated(),
                  std::logic_error,
                  "Item is not an instantiated variant !");
      return _variant_handle_;
    }

    vm_handle_type & item::grab_variant_handle()
    {
      DT_THROW_IF(!is_variant() || !is_instantiated(),
                  std::logic_error,
                  "Item is not an instantiated variant !");
      return _variant_handle_;
    }

    parameter_model & item::make_parameter(const std::string & name_)
    {
      if (is_instantiated()) {
        destroy();
      }
      if (!name_.empty()) set_name(name_);
      set_model(MODEL_PARAMETER);
      parameter_model * par = new parameter_model;
      par->set_name(name_);
      _parameter_handle_.reset(par);
      return *par;
    }

    variant_model & item::make_variant(const std::string & name_)
    {
      if (is_instantiated()) {
        destroy();
      }
      if (!name_.empty()) set_name(name_);
      set_model(MODEL_VARIANT);
      variant_model * var = new variant_model;
      var->set_name(name_);
      _variant_handle_.reset(var);
      return *var;
    }

    void item::tree_dump(std::ostream& out_,
                         const std::string& title_,
                         const std::string& indent_,
                         bool inherit_) const
    {
      std::string indent;
      if (! indent_.empty()) indent = indent_;
      if (! title_.empty()) out_ << indent << title_ << std::endl;

      out_ << indent << i_tree_dumpable::tag
           << "Name : ";
      if (! _name_.empty()) {
        out_ << "'" << _name_ << "'";
      } else {
        out_ << "<none>";
      }
      out_ << std::endl;

      out_ << indent << i_tree_dumpable::tag
           << "Configuration : ";
      if (get_config().empty()) {
        out_ << "<none>";
      }
      out_ << std::endl;
      if (!get_config().empty()) {
        std::ostringstream indent2;
        indent2 << indent << i_tree_dumpable::skip_tag;
        get_config().tree_dump(out_, "", indent2.str());
      }

      out_ << indent_ << i_tree_dumpable::tag
           << "Model : '"
           << this->label_from_model_type(_model_) << "'" << std::endl;


      if (this->is_instantiated()) {
        out_ << indent_ << i_tree_dumpable::tag
             << "Instantiated : "
             << this->is_instantiated() << std::endl;
        if (this->is_variant()) {
          out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
               << "Variant model: "
               << _variant_handle_.get().get_name() << std::endl;
        } else if (this->is_parameter()) {
          out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
               << "Parameter model: "
               << _parameter_handle_.get().get_name() << std::endl;
        }
      } else {
        out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
             << "Instantiated : "
             << this->is_instantiated() << std::endl;
      }

      return;
    }

    // static
    parameter_model & item::add_parameter_item(item_dict_type & dict_, const std::string & name_)
    {
      std::string name = name_;
      DT_THROW_IF(boost::ends_with(name, variant_suffix()),
                  std::logic_error,
                  "Parameter item key '" << name << "' ends with an invalid suffix!");
      // if (! boost::ends_with(name, parameter_suffix())) {
      //   name = name_ + parameter_suffix();
      // }
      DT_THROW_IF(dict_.find(name) != dict_.end(), std::logic_error,
                  "Item '" << name << "' already exist in the dictionary!");
      {
        item dummy;
        dict_[name] = dummy;
      }
      item & i = dict_.find(name)->second;
      return i.make_parameter(name);
    }

    // static
    variant_model & item::add_variant_item(item_dict_type & dict_, const std::string & name_)
    {
      std::string name = name_;
      DT_THROW_IF(boost::ends_with(name, parameter_suffix()),
                  std::logic_error,
                  "Variant item key '" << name << "' ends with an invalid suffix!");
      // if (! boost::ends_with(name, variant_suffix())) {
      //   name = name_ + variant_suffix();
      // }
      DT_THROW_IF(dict_.find(name) != dict_.end(), std::logic_error,
                  "Item '" << name << "' already exist in the dictionary!");
      {
        item dummy;
        dict_[name] = dummy;
      }
      item & i = dict_.find(name)->second;
      return i.make_variant(name);
    }

    // static
    bool item::has_item(const item_dict_type & dict_,
                        const std::string & name_,
                        item::model_type model_)
    {
      item_dict_type::const_iterator found = dict_.find(name_);
      if (found == dict_.end()) {
        return false;
      }
      if (model_ == MODEL_PARAMETER) {
        return found->second.is_parameter();
      } else if (model_ == MODEL_VARIANT) {
        return found->second.is_variant();
      }
      return true;
    }

    // static
    const parameter_model & item::get_parameter_model(const item_dict_type & dict_,
                                                const std::string & name_)
    {
      item_dict_type::const_iterator found = dict_.find(name_);
      DT_THROW_IF(found == dict_.end(),
                  std::logic_error,
                  "Dictionary has no item named '" << name_ << "' !");
      DT_THROW_IF(!found->second.is_parameter(),
                  std::logic_error,
                  "Dictionary item named '" << name_ << "' is not a parameter model!");
      return found->second.get_parameter_model();
    }

    // static
    const variant_model & item::get_variant_model(const item_dict_type & dict_,
                                            const std::string & name_)
    {
      item_dict_type::const_iterator found = dict_.find(name_);
      DT_THROW_IF(found == dict_.end(),
                  std::logic_error,
                  "Dictionary has no item named '" << name_ << "' !");
      DT_THROW_IF(!found->second.is_variant(),
                  std::logic_error,
                  "Dictionary item named '" << name_ << "' is not a variant model!");
      return found->second.get_variant_model();
    }

    // static
    void item::remove_item(item_dict_type & dict_, const std::string & name_)
    {
      item_dict_type::iterator found = dict_.find(name_);
      DT_THROW_IF(found == dict_.end(), std::logic_error,
                  "Item '" << name_ << "' does not exist in the dictionary!");
      dict_.erase(found);
      return;
    }

    // static
    void item::print_items(const item_dict_type & dict_,
                           std::ostream & out_,
                           const std::string & title_,
                           const std::string & indent_,
                           bool inherit_)
    {
      std::string indent;
      if (! indent_.empty()) indent = indent_;
      if (! title_.empty()) out_ << indent << title_ << std::endl;

      /*
      if (dict_.size() == 0) out_ << "<none>";
      out_ << std::endl;
      */
      for (item_dict_type::const_iterator i = dict_.begin();
           i != dict_.end();
           i++) {
        item_dict_type::const_iterator j = i;
        j++;
        out_ << indent << i_tree_dumpable::inherit_skip_tag(inherit_);
        std::ostringstream indent2_ss;
        indent2_ss << indent << i_tree_dumpable::inherit_skip_tag(inherit_);
        if (j == dict_.end()) {
          out_  << i_tree_dumpable::last_tag;
          indent2_ss << i_tree_dumpable::last_skip_tag;
        } else {
          out_  << i_tree_dumpable::tag;
          indent2_ss << i_tree_dumpable::skip_tag;
        }
        out_ << "Item '" << i->first << "' ";
        const item & it = i->second;
        if (it.is_parameter()) {
          out_ << "[parameter model]";
        } else if (it.is_variant()) {
          out_ << "[variant model]";
        } else {
          out_ << "[undefined model]";
        }
        out_ << std::endl;
        if (it.is_instantiated()) {
          if (it.is_parameter()) {
            const parameter_model & par = it.get_parameter_model();
            par.tree_dump(out_, "", indent2_ss.str());
          }
          if (it.is_variant()) {
            const variant_model & var = it.get_variant_model();
            var.tree_dump(out_, "", indent2_ss.str());
          }
        }
      }
      return;
    }

    variant_parameter_set_type::variant_parameter_set_type()
    {
      return;
    }

    void variant_parameter_set_type::reset()
    {
      registry_key.clear();
      param_key.clear();
      param_value_str.clear();
      return;
    }

    command::returned_info
    variant_parameter_set_type::parse(const std::string & variant_set_)
    {
      command::returned_info cri;
      size_t apos = variant_set_.find(':');
      if (apos == variant_set_.npos) {
        DT_COMMAND_RETURNED_ERROR(cri,
                                  command::CEC_COMMAND_INVALID_SYNTAX,
                                  "Invalid syntax in variant set directive ('" << variant_set_ << "' !");
      }
      std::string variant_registry_key = variant_set_.substr(0, apos);
      std::string variant_param_set    = variant_set_.substr(apos + 1);
      apos = variant_param_set.find('=');
      std::string variant_param_key;
      std::string variant_param_value_str;
      if (apos == variant_param_set.npos) {
        // Assume a boolean parameter:
        variant_param_key       = variant_param_set;
        variant_param_value_str = "true";
      } else {
        variant_param_key       = variant_param_set.substr(0, apos);
        variant_param_value_str = variant_param_set.substr(apos + 1);
      }
      registry_key    = variant_registry_key;
      param_key       = variant_param_key;
      param_value_str = variant_param_value_str;
      return cri;
    }

    variant_parameter_set_comparator::variant_parameter_set_comparator() : _repository_(0)
    {
      return;
    }

    variant_parameter_set_comparator::variant_parameter_set_comparator(variant_repository & rep_)
      : _repository_(&rep_)
    {
      return;
    }

    bool variant_parameter_set_comparator::operator()(const std::string & vs1_, const std::string & vs2_) const
    {
      variant_parameter_set_type vps1;
      variant_parameter_set_type vps2;

      command::returned_info cri = vps1.parse(vs1_);
      DT_THROW_IF(cri.is_failure(), std::logic_error,
                  cri.get_error_message());
      cri = vps2.parse(vs2_);
      DT_THROW_IF(cri.is_failure(), std::logic_error,
                  cri.get_error_message());

      // If registries are ranked, apply the rank as a priority:
      if (_repository_ != 0) {
        if (_repository_->is_ranked(vps1.registry_key) && ! _repository_->is_ranked(vps2.registry_key)) {
          return true;
        }
        if (! _repository_->is_ranked(vps1.registry_key) && _repository_->is_ranked(vps2.registry_key)) {
          return false;
        }
        if (_repository_->is_ranked(vps1.registry_key) && _repository_->is_ranked(vps2.registry_key)) {
          if (_repository_->get_rank(vps1.registry_key) < _repository_->get_rank(vps2.registry_key)) {
            return true;
          }
          if (_repository_->get_rank(vps1.registry_key) > _repository_->get_rank(vps2.registry_key)) {
            return false;
          }
        }
      }

      // Otherwise, use lexicographical compare, taking into account '/' separators:
      if (vps1.registry_key < vps2.registry_key) {
        return true;
      } else if (vps1.registry_key > vps2.registry_key) {
        return false;
      } else {
        std::vector<std::string> path_tokens_1;
        std::vector<std::string> path_tokens_2;
        boost::split(path_tokens_1, vps1.param_key, boost::algorithm::is_any_of("/"));
        boost::split(path_tokens_2, vps2.param_key, boost::algorithm::is_any_of("/"));
        // Comparison rules:
        //
        //  foo/bar  -> foo + bar (2)
        //  bar/foo  -> bar + foo (1)
        //
        //
        //  foo/bar  -> foo + bar (1)
        //  foo/foo  -> foo + foo (2)
        //
        size_t i = 0;
        while (1) {
          if (i >= path_tokens_1.size() || i >= path_tokens_2.size()) {
            break;
          }
          const std::string & tok_1 = path_tokens_1[i];
          const std::string & tok_2 = path_tokens_2[i];
          if (tok_1 < tok_2) {
            return true;
          } else if (tok_1 > tok_2) {
            return false;
          } else {
            if (path_tokens_1.size() < path_tokens_2.size()) {
              return true;
            } else if (path_tokens_1.size() > path_tokens_2.size()) {
              return false;
            } else {
              i++;
            }
          }
        }
      }
      return false;
    }

  }  // end of namespace configuration

}  // end of namespace datatools
