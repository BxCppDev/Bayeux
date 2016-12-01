//! \file mygsl/i_unary_function_with_parameters.cc
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of Bayeux.
//
// Bayeux is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux. If not, see <http://www.gnu.org/licenses/>.

// Ourselves:
#include <mygsl/i_unary_function_with_parameters.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/properties.h>

namespace mygsl {

  i_unary_function_with_parameters::i_unary_function_with_parameters()
  {
    _params_ = 0;
    return;
  }

  i_unary_function_with_parameters::i_unary_function_with_parameters(const parameter_store & store_)
  {
    set_parameter_store(store_);
    return;
  }

  i_unary_function_with_parameters::~i_unary_function_with_parameters()
  {
    reset_parameter_store();
    return;
  }

  bool i_unary_function_with_parameters::is_initialized() const
  {
    return this->i_unary_function::is_initialized() && has_parameter_store();
  }

  void i_unary_function_with_parameters::initialize(const datatools::properties & config_,
                                                    const unary_function_dict_type & functors_)
  {
    this->i_unary_function::initialize(config_, functors_);

    if (_requested_parameters_.empty()) {
      std::vector<std::string> params;
      if (config_.has_key("parameters")) {
        config_.fetch("parameters", params);
        for (int i = 0; i < (int) params.size(); i++) {
          datatools::basic_type param_type = datatools::TYPE_REAL;
          {
            std::ostringstream param_type_key;
            param_type_key << "parameters." << params[i] << ".type";
            if (config_.has_key(param_type_key.str())) {
              const std::string & param_type_label = config_.fetch_string(param_type_key.str());
              param_type = static_cast<datatools::basic_type>(datatools::get_type_from_label(param_type_label));
              DT_THROW_IF(param_type == datatools::TYPE_NONE, std::logic_error,
                          "Invalid parameter type '" << param_type_label << "' for parameter '" << params[i] << "'!");

            }
          }
          register_requested_parameter(params[i], param_type);
        }
      }
    }

    // Checks:
    DT_THROW_IF(!has_parameter_store(), std::logic_error,
                "No parameter store is available!");

    for (std::map<std::string, datatools::basic_type>::const_iterator i
           = _requested_parameters_.begin();
         i != _requested_parameters_.end();
         i++) {
      const std::string & param_name = i->first;
      datatools::basic_type param_type = i->second;
      DT_THROW_IF(!_params_->has(param_name), std::logic_error,
                  "Store does not have a parameter with name '" << param_name << "'!");
      const parameter_store::parameter_record & pr = _params_->get(param_name);
      DT_THROW_IF(pr.get_type() != param_type, std::logic_error,
                  "Parameter '" << param_name << "' is not of type '" << datatools::get_label_from_type(param_type) << "'!");
    }

    return;
  }

  void i_unary_function_with_parameters::reset()
  {
    reset_parameter_store();
    _requested_parameters_.clear();
    this->i_unary_function::reset();
    return;
  }

  bool i_unary_function_with_parameters::has_parameter_store() const
  {
    return _params_ != 0;
  }

  void i_unary_function_with_parameters::set_parameter_store(const parameter_store & store_)
  {
    _params_ = &store_;
    const_cast<parameter_store*>(_params_)->register_subscriber(*this);
    return;
  }

  void i_unary_function_with_parameters::reset_parameter_store()
  {
    if (_params_) {
      const_cast<parameter_store*>(_params_)->unregister_subscriber(*this);
    }
    _params_ = 0;
    return;
  }

  void i_unary_function_with_parameters::register_requested_parameter(const std::string & parameter_name_,
                                                                      datatools::basic_type parameter_type_)
  {
    DT_THROW_IF(_requested_parameters_.find(parameter_name_) != _requested_parameters_.end(), std::logic_error,
                "Parameter '" << parameter_name_ << "' is already requested!");
    DT_THROW_IF(parameter_type_ == datatools::TYPE_NONE, std::logic_error,
                "Invalid parameter type!");
    _requested_parameters_[parameter_name_] = parameter_type_;
    return;
  }

  int i_unary_function_with_parameters::get_parameter_index(const std::string & parameter_name_) const
  {
    int param_index = parameter_store::INVALID_INDEX;
    if (has_parameter_store() && _params_->has(parameter_name_)) {
      param_index = _params_->get_parameter_index(parameter_name_);
    }
    return param_index;
  }

  bool i_unary_function_with_parameters::is_parameter_set(int param_index_) const
  {
    DT_THROW_IF(!has_parameter_store(), std::logic_error, "Missing parameter store!");
    return _params_->get(param_index_).is_value_set();
  }

  void i_unary_function_with_parameters::fetch_parameter(int param_index_, bool & value_) const
  {
    DT_THROW_IF(!has_parameter_store(), std::logic_error, "Missing parameter store!");
    value_ = _params_->get(param_index_).to_boolean();
    return;
  }

  void i_unary_function_with_parameters::fetch_parameter(int param_index_, int & value_) const
  {
    DT_THROW_IF(!has_parameter_store(), std::logic_error, "Missing parameter store!");
    value_ = _params_->get(param_index_).to_integer();
    return;
  }

  void i_unary_function_with_parameters::fetch_parameter(int param_index_, double & value_) const
  {
    DT_THROW_IF(!has_parameter_store(), std::logic_error, "Missing parameter store!");
    value_ = _params_->get(param_index_).to_real();
    return;
  }

  void i_unary_function_with_parameters::fetch_parameter(int param_index_, std::string & value_) const
  {
    DT_THROW_IF(!has_parameter_store(), std::logic_error, "Missing parameter store!");
    value_ = _params_->get(param_index_).to_string();
    return;
  }

  void i_unary_function_with_parameters::update_parameters()
  {
    _at_parameters_change();
    return;
  }

  double i_unary_function_with_parameters::_eval(double x_) const
  {
    DT_THROW_IF(!has_parameter_store(), std::logic_error,
                "No parameter store is available!");
    return _eval_from_parameters(x_);
  }

  void i_unary_function_with_parameters::tree_dump(std::ostream & out_,
                                                   const std::string & title_,
                                                   const std::string & indent_,
                                                   bool inherit_) const
  {
    this->i_unary_function::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Requested parameters: ";
    if (_requested_parameters_.size() == 0) {
      out_ << "<none>";
    } else {
      out_ << '[' << _requested_parameters_.size() << ']';
    }
    out_ << std::endl;
    for (std::map<std::string, datatools::basic_type>::const_iterator i =
           _requested_parameters_.begin();
         i != _requested_parameters_.end();
         i++) {
      out_ << indent_ <<  datatools::i_tree_dumpable::skip_tag;
      std::map<std::string, datatools::basic_type>::const_iterator j = i;
      j++;
      if (j == _requested_parameters_.end()) {
        out_ << datatools::i_tree_dumpable::last_tag;
      } else {
        out_ << datatools::i_tree_dumpable::tag;
      }
      out_ << "Parameter '" << i->first << " of type '" << datatools::get_label_from_type(i->second) << "'" << std::endl;
    }

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Parameter store: ";
    if (! has_parameter_store()) {
      out_ << "<none>";
    } else {
      out_ << _params_;
    }
    out_ << std::endl;

    return;
  }

} // end of namespace mygsl
