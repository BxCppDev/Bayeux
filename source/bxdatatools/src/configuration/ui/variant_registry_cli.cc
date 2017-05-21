// datatools/configuration/ui/variant_registry_cli.cc
/*
 * Copyright (C) 2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
#include <datatools/configuration/ui/variant_registry_cli.h>

// This project:
#include <datatools/exception.h>
#include <datatools/configuration/variant_registry.h>
#include <datatools/configuration/variant_record.h>
#include <datatools/configuration/parameter_model.h>
#include <datatools/detail/command_macros.h>

namespace datatools {

  namespace configuration {

    namespace ui {

      variant_registry_cli::variant_registry_cli(variant_registry & registry_)
        : _logging_(datatools::logger::PRIO_FATAL)
        , _registry_(registry_)
      {
        return;
      }

      datatools::logger::priority
      variant_registry_cli::get_logging() const
      {
        return _logging_;
      }

      void variant_registry_cli::set_logging(const datatools::logger::priority p_)
      {
        _logging_ = p_;
        return;
      }

      datatools::command::returned_info
      variant_registry_cli::cmd_has_variant(const std::string & variant_path_,
                                            bool & existing_) const
      {
        datatools::command::returned_info cri;
        cri.set_error_code(command::CEC_SUCCESS);
        existing_ = _registry_.has_variant_record(variant_path_);
        return cri;
      }

      datatools::command::returned_info
      variant_registry_cli::cmd_has_parameter(const std::string & parameter_path_,
                                              bool & existing_) const
      {
        datatools::command::returned_info cri;
        cri.set_error_code(command::CEC_SUCCESS);
        existing_ = _registry_.has_parameter_record(parameter_path_);
        return cri;
      }

      datatools::command::returned_info
      variant_registry_cli::cmd_is_active_variant(const std::string & variant_path_,
                                                  bool & active_) const
      {
        datatools::command::returned_info cri;
        try {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_KEY);
          DT_THROW_IF(!_registry_.has_variant_record(variant_path_), std::logic_error,
                      "Variant record with path '" << variant_path_
                      << "' does not exist in registry '" << _registry_.get_name() << "'!!");
          const variant_record & variant_rec = _registry_.get_variant_record(variant_path_);
          cri.set_error_code(command::CEC_PARAMETER_INVALID_CONTEXT);
          active_ = variant_rec.is_active();
          cri.set_error_code(command::CEC_SUCCESS);
        } catch (std::exception & x) {
          cri.set_error_message(x.what());
        }
        return cri;
      }

      datatools::command::returned_info
      variant_registry_cli::cmd_get_parameter_value(const std::string & param_path_,
                                                    std::string & value_token_) const
      {
        datatools::command::returned_info cri;
        try {
          if (! _registry_.has_parameter_record(param_path_)) {
            cri.set_error_code(command::CEC_PARAMETER_INVALID_KEY);
            DT_THROW(std::logic_error,
                     "Parameter record with path '" << param_path_ << "' does not exist "
                     << "in registry '" << _registry_.get_name() << "'!");
          }
          const variant_record & param_rec = _registry_.get_parameter_record(param_path_);
          if (!param_rec.is_active()) {
            cri.set_error_code(command::CEC_PARAMETER_INVALID_CONTEXT);
            DT_THROW(std::logic_error,
                     "Parameter record '" << param_path_ << "' is not active "
                     << "in registry '" << _registry_.get_name() << "'!");
          }
          if (param_rec.get_parameter_model().is_boolean()) {
            std::ostringstream os;
            bool value = false;
            cri = param_rec.get_boolean_value(value);
            if (cri.is_failure()) {
              DT_THROW(std::logic_error,
                       "Boolean parameter '" << param_path_ << "' has no available value"
                       << "in registry '" << _registry_.get_name() << "'!");
            }
            io::write_boolean(os, value);
            value_token_ = os.str();
          } else if (param_rec.get_parameter_model().is_integer()) {
            std::ostringstream os;
            int value = 0;
            cri = param_rec.get_integer_value(value);
            if (cri.is_failure()) {
              DT_THROW(std::logic_error,
                       "Integer parameter '" << param_path_ << "' has no available value"
                       << "in registry '" << _registry_.get_name() << "'!");
            }
            io::write_integer(os, value);
            value_token_ = os.str();
          } else if (param_rec.get_parameter_model().is_real()) {
            std::ostringstream os;
            double value;
            cri = param_rec.get_real_value(value);
            if (cri.is_failure()) {
              DT_THROW(std::logic_error,
                       "Real parameter '" << param_path_ << "' has no available value"
                       << " in registry '" << _registry_.get_name() << "'!");
            }
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
            cri.set_error_code(command::CEC_PARAMETER_INVALID_TYPE);
            DT_THROW(std::logic_error,
                     "Parameter '" << param_path_ << "' has no known type"
                     << "in registry '" << _registry_.get_name() << "'!");
          }
        } catch (std::exception & x) {
          DT_LOG_ERROR(_logging_, x.what());
          cri.set_error_message(x.what());
        }
        return cri;
      }

      datatools::command::returned_info
      variant_registry_cli::cmd_is_parameter_value_enabled(const std::string & param_path_,
                                                           const std::string & value_token_,
                                                           bool & enabled_)
      {
        datatools::command::returned_info cri;
        try {
          enabled_ = false;
          if (!_registry_.has_parameter_record(param_path_)) {
            cri.set_error_code(command::CEC_PARAMETER_INVALID_KEY);
            DT_THROW(std::logic_error,
                     "Parameter record with path '" << param_path_ << "' does not exist!");
          }
          variant_record & param_rec = _registry_.grab_parameter_record(param_path_);
          if (!param_rec.is_active()) {
            cri.set_error_code(command::CEC_PARAMETER_INVALID_CONTEXT);
            DT_THROW(std::logic_error,
                     "Parameter record '" << param_path_ << "' is not active!");
          }
          const parameter_model & parmod = param_rec.get_parameter_model();
          if (parmod.is_fixed()) {
            cri.set_error_code(command::CEC_PARAMETER_INVALID_CONTEXT);
            DT_THROW(std::logic_error,
                     "Parameter record '" << param_path_ << "' has a fixed value!");
          }

          if (parmod.is_boolean()) {
            // Boolean value:
            std::istringstream inss(value_token_);
            bool value;
            bool parsed = io::read_boolean(inss, value);
            if (!parsed) {
              cri.set_error_code(command::CEC_PARSING_FAILURE);
              DT_THROW(std::logic_error,
                       "Boolean value for parameter '" << param_path_ << "' cannot be parsed from '" << value_token_ << "'!");
            }
            enabled_ = param_rec.is_boolean_valid(value);
          }

        } catch (std::exception & x) {
          cri.set_error_message(x.what());
        }
        return cri;
      }

      datatools::command::returned_info
      variant_registry_cli::cmd_set_parameter_value(const std::string & param_path_,
                                                    const std::string & value_token_)
      {
        datatools::command::returned_info cri;
        try {
          if (!_registry_.has_parameter_record(param_path_)) {
            cri.set_error_code(command::CEC_PARAMETER_INVALID_KEY);
            DT_THROW(std::logic_error,
                     "Parameter record with path '" << param_path_ << "' does not exist!");
          }
          variant_record & param_rec = _registry_.grab_parameter_record(param_path_);
          if (!param_rec.is_active()) {
            cri.set_error_code(command::CEC_PARAMETER_INVALID_CONTEXT);
            DT_THROW(std::logic_error,
                     "Parameter record '" << param_path_ << "' is not active!");
          }
          const parameter_model & parmod = param_rec.get_parameter_model();
          if (parmod.is_fixed()) {
            cri.set_error_code(command::CEC_PARAMETER_INVALID_CONTEXT);
            DT_THROW(std::logic_error,
                     "Parameter record '" << param_path_ << "' has a fixed value!");
          }
          if (parmod.is_boolean()) {
            std::istringstream inss(value_token_);
            bool value;
            bool parsed = io::read_boolean(inss, value);
            if (!parsed) {
              cri.set_error_code(command::CEC_PARSING_FAILURE);
              DT_THROW(std::logic_error,
                       "Boolean value for parameter '" << param_path_ << "' cannot be parsed from '" << value_token_ << "'!");
            }
            cri = param_rec.set_boolean_value(value);
            if (cri.is_failure()) {
              cri.set_error_code(command::CEC_PARAMETER_INVALID_VALUE);
              DT_THROW(std::logic_error,
                       "Boolean value for parameter '" << param_path_ << "' cannot accept value='" << value << "'!");
            }
          } else if (parmod.is_integer()) {
            std::istringstream inss(value_token_);
            int value;
            bool parsed = io::read_integer(inss, value);
            if (!parsed) {
              cri.set_error_code(command::CEC_PARSING_FAILURE);
              DT_THROW(std::logic_error,
                       "Integer value for parameter '" << param_path_ << "' cannot be parsed from '" << value_token_ << "'!");
            }
            cri = param_rec.set_integer_value(value);
            if (cri.is_failure()) {
              cri.set_error_code(command::CEC_PARAMETER_INVALID_VALUE);
              DT_THROW(std::logic_error,
                       "Integer value for parameter '" << param_path_ << "' cannot accept value='" << value << "'!");
            }
          } else if (parmod.is_real()) {
            std::istringstream inss(value_token_);
            double value;
            std::string unit_label;
            bool parsed = units::find_value_with_unit(value_token_, value, unit_label);
            if (!parsed) {
              cri.set_error_code(command::CEC_PARSING_FAILURE);
              DT_THROW(std::logic_error,
                       "Real value for parameter '" << param_path_ << "' cannot be parsed from '" << value_token_ << "'!");
            }
            cri = param_rec.set_real_value(value);
            if (cri.is_failure()) {
              cri.set_error_code(command::CEC_PARAMETER_INVALID_VALUE);
              DT_THROW(std::logic_error,
                       "Real value for parameter '" << param_path_ << "' cannot accept value='" << value << "'!");
            }
          } else if (parmod.is_string()) {
            std::string value = value_token_;
            cri = param_rec.set_string_value(value);
            if (cri.is_failure()) {
              cri.set_error_code(command::CEC_PARAMETER_INVALID_VALUE);
              DT_THROW(std::logic_error,
                       "String value for parameter '" << param_path_ << "' cannot accept value='" << value << "'!");
            }
          } else {
            cri.set_error_code(command::CEC_PARAMETER_INVALID_TYPE);
            DT_THROW(std::logic_error,
                     "Parameter '" << param_path_ << "' has no known type!");
          }
        } catch (std::exception & x) {
          cri.set_error_message(x.what());
        }
        return cri;
      }

    }  // end of namespace ui

  }  // end of namespace configuration

}  // end of namespace datatools
