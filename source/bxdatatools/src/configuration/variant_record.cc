// datatools/configuration/variant_record.cc
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
#include <datatools/configuration/variant_record.h>

// Third party
// - Boost:
#include <boost/filesystem/path.hpp>

// This project:
#include <datatools/configuration/parameter_model.h>
#include <datatools/configuration/variant_model.h>

namespace datatools {

  namespace configuration {

    // virtual
    bool variant_record::is_name_valid(const std::string & name_) const
    {
      // std::cerr << "DEVEL: variant_record::is_name_valid: name = '" << name_ << "'" << std::endl;
      return ::datatools::configuration::validate_model_name(name_);
    }

    variant_record::variant_record()
    {
      _parameter_model_ = 0;
      _variant_model_ = 0;
      _parent_ = 0;
      _active_ = false;
      _value_set_ = false;
      _boolean_value_ = false;
      _integer_value_ = 0;
      datatools::invalidate(_real_value_);
      _string_value_.clear();
      return;
    }

    void variant_record::reset()
    {
      _path_.clear();
      _parameter_model_ = 0;
      _variant_model_ = 0;
      _parent_ = 0;
      _daughters_.clear();
      _active_ = false;
      _value_set_ = false;
      _boolean_value_ = false;
      _integer_value_ = 0;
      datatools::invalidate(_real_value_);
      _string_value_.clear();
      this->enriched_base::clear();
      return;
    }

    variant_record::~variant_record()
    {
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

    std::string variant_record::get_leaf_name() const
    {
      boost::filesystem::path pth(_path_);
      return pth.leaf().string();
    }

    bool variant_record::has_parent() const
    {
      return _parent_ != 0;
    }

    void variant_record::set_parent(variant_record & parent_, const std::string & daughter_name_)
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
      _parent_->grab_daughters()[daughter_name_] = this;
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
      return _parameter_model_ != 0;
    }

    void variant_record::set_parameter_model(const parameter_model & pm_)
    {
      _variant_model_ = 0;
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
      return _variant_model_ != 0;
    }

    void variant_record::set_variant_model(const variant_model & vm_)
    {
      _parameter_model_ = 0;
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
        _update();
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
      if (get_parameter_model().has_default_value()) {
        DT_THROW_IF(! value_is_set(), std::logic_error,
                    "Parameter record '" << _path_ << "' has no value set!");
        if (get_parameter_model().is_boolean()) {
          return _boolean_value_ == get_parameter_model().get_default_boolean();
        } else if (get_parameter_model().is_integer()) {
          return _integer_value_ == get_parameter_model().get_default_integer();
        } else if (get_parameter_model().is_real()) {
          return _real_value_ == get_parameter_model().get_default_real();
        } else if (get_parameter_model().is_string()) {
          return _string_value_ == get_parameter_model().get_default_string();
        }
      } else {
        return false;
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
          set_boolean_value(get_parameter_model().get_default_boolean());
        } else if (get_parameter_model().is_integer()) {
          set_integer_value(get_parameter_model().get_default_integer());
        } else if (get_parameter_model().is_real()) {
          set_real_value(get_parameter_model().get_default_real());
        } else if (get_parameter_model().is_string()) {
          set_string_value(get_parameter_model().get_default_string());
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
        if (! get_parameter_model().is_boolean_valid(value_)) {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_VALUE);
          DT_THROW(std::logic_error,
                   "Parameter record '" << _path_ << "' does not accept boolean value '" << value_ << "' !");
        }
        _boolean_value_ = value_;
        _value_set_ = true;
        _update();
      } catch (std::exception & x) {
        cri.set_error_message(x.what());
      }
      return cri;
    }

    command::returned_info
    variant_record::set_integer_value(int value_)
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
        if (! get_parameter_model().is_integer()) {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_TYPE);
          DT_THROW(std::logic_error,
                      "Parameter record '" << _path_ << "' is not a integer parameter!");
        }
        if (! get_parameter_model().is_integer_valid(value_)) {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_VALUE);
          DT_THROW(std::logic_error,
                   "Parameter record '" << _path_ << "' does not accept integer value '" << value_ << "' !");
        }
        _integer_value_ = value_;
        _value_set_ = true;
        _update();
      } catch (std::exception & x) {
        cri.set_error_message(x.what());
      }
      return cri;
    }

    command::returned_info
    variant_record::set_real_value(double value_)
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
        if (! get_parameter_model().is_real()) {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_TYPE);
          DT_THROW(std::logic_error,
                   "Parameter record '" << _path_ << "' is not a real parameter!");
        }
        if (! get_parameter_model().is_real_valid(value_)) {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_VALUE);
          DT_THROW(std::logic_error,
                   "Parameter record '" << _path_ << "' does not accept real value '" << value_ << "' !");
        }
        _real_value_ = value_;
        _value_set_ = true;
        _update();
      } catch (std::exception & x) {
        cri.set_error_message(x.what());
      }
      return cri;
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
        if (! get_parameter_model().is_string_valid(value_)) {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_VALUE);
          DT_THROW(std::logic_error,
                   "Parameter record '" << _path_ << "' does not accept string value '" << value_ << "' !");
        }
        _string_value_ = value_;
        _value_set_ = true;
        _update();
      } catch (std::exception & x) {
        cri.set_error_message(x.what());
      }
      // std::cerr << "DEVEL: CRI == " << cri << std::endl;
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
        _update();
      } catch (std::exception & x) {
        cri.set_error_message(x.what());
      }
      return cri;
    }

    void variant_record::_update()
    {
      if (is_parameter()) {
        // Deactivate all variants...
        for (daughter_dict_type::iterator i = _daughters_.begin();
             i != _daughters_.end();
             i++) {
          variant_record * vrec = i->second;
          if (vrec->is_active()) {
            vrec->set_active(false);
          }
        }

        // Then search if some new variant is activated:
        if (value_is_set()) {
          std::string activated_variant_name;
          bool found_activable = false;

          if (get_parameter_model().is_boolean()) {
            found_activable = get_parameter_model().find_variant_associated_to_boolean(_boolean_value_,
                                                                                       activated_variant_name);
          } // boolean

          if (get_parameter_model().is_integer()) {
            found_activable = get_parameter_model().find_variant_associated_to_integer(_integer_value_,
                                                                                       activated_variant_name);
          } // integer

          if (get_parameter_model().is_real()) {
            found_activable = get_parameter_model().find_variant_associated_to_real(_real_value_,
                                                                                    activated_variant_name);
          } // real

          if (get_parameter_model().is_string()) {
            // std::cerr << "DEVEL: variant_record::_update: Parameter is string" << std::endl;
            found_activable = get_parameter_model().find_variant_associated_to_string(_string_value_,
                                                                                      activated_variant_name);
          } // string
          // std::cerr << "DEVEL: variant_record::_update: found_activable=" << found_activable << std::endl;

          if (is_active() && found_activable && !activated_variant_name.empty()) {
            // std::cerr << "DEVEL: variant_record::_update: activated_variant_name = '" << activated_variant_name << "'" << std::endl;
            daughter_dict_type::iterator found = _daughters_.find(activated_variant_name);
            DT_THROW_IF(found == _daughters_.end(),
                        std::logic_error,
                        "Parameter record '" << _path_ << "' has no activable variant named '"
                        << activated_variant_name << "' ! This is a bug!");
            variant_record * activated_variant_rec = found->second;
            if (!activated_variant_rec->is_active()) {
              activated_variant_rec->set_active(true);
            }
          }
        } // value_is_set
      } // is_parameter

      if (is_variant()) {
        for (daughter_dict_type::iterator i = grab_daughters().begin();
             i != grab_daughters().end();
             i++) {
          variant_record * param_rec = i->second;
          if (param_rec->is_active() != _active_) {
            param_rec->set_active(_active_);
          }
        }
      } // is_variant

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
          bool value;
          if (!io::read_boolean(value_iss, value)) {
            cri.set_error_code(command::CEC_PARSING_FAILURE);
            DT_THROW(std::logic_error,
                     "Invalid format '" << format_ << "' for boolean record '" << _path_ << "'!");
          }
          cri = set_boolean_value(value);
        } else if (get_parameter_model().is_integer()) {
          int value;
          if (!io::read_integer(value_iss, value)) {
            cri.set_error_code(command::CEC_PARSING_FAILURE);
            DT_THROW(std::logic_error,
                     "Invalid format '" << format_ << " for integer record '" << _path_ << "'!");
          }
          cri = set_integer_value(value);
        } else if (get_parameter_model().is_real()) {
          double value;
          std::string unit_symbol;
          std::string unit_label;
          bool ok = datatools::units::parse_value_with_unit(format_, value,
                                                            unit_symbol, unit_label);
          if (!ok) {
            cri.set_error_code(command::CEC_PARSING_FAILURE);
            DT_THROW(std::logic_error,
                     "Invalid format for real record '" << _path_ << "'!");
          }
          if (get_parameter_model().has_real_unit_label()) {
            if (unit_label != get_parameter_model().get_real_unit_label()) {
              cri.set_error_code(command::CEC_PARAMETER_INVALID_UNIT);
              DT_THROW(std::logic_error,
                       "Invalid unit for real record '" << _path_ << "'!");
            }
          }
          // io::read_real_number(value_iss, value, 15,
          //                      unit_label, unit_symbol);
          cri = set_real_value(value);
        } else if (get_parameter_model().is_string()) {
          std::string value;
          if (!io::read_quoted_string(value_iss, value)) {
            cri.set_error_code(command::CEC_PARSING_FAILURE);
            DT_THROW(std::logic_error,
                     "Invalid format for string record '" << _path_ << "'!");
          }
          cri = set_string_value(value);
        } else {
          cri.set_error_code(command::CEC_PARAMETER_INVALID_TYPE);
          DT_THROW(std::logic_error,
                   "Invalid parameter type for record '" << _path_ << "'!");
        }
      } catch (std::exception & x) {
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
      }
      catch (std::exception & x) {
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
      }
      catch (std::exception & x) {
        cri.set_error_message(x.what());
      }
      return cri;
    }

    variant_record::daughter_dict_type &
    variant_record::grab_daughters()
    {
      return _daughters_;
    }

    const variant_record::daughter_dict_type &
    variant_record::get_daughters() const
    {
      return _daughters_;
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
           << "Daughters : " << _daughters_.size() << "" << std::endl;
      for (daughter_dict_type::const_iterator i = _daughters_.begin();
             i != _daughters_.end();
           i++) {
        daughter_dict_type::const_iterator j = i;
        out_ << indent_ << i_tree_dumpable::skip_tag;
        if (++j == _daughters_.end()) {
          out_ << i_tree_dumpable::last_tag;
        } else {
          out_ << i_tree_dumpable::tag;
        }
        const std::string & daughter_name = i->first;
        const variant_record * daughter_rec = i->second;
        out_ << "Daughter: '"
             << daughter_name << "' with path '"
             << daughter_rec->get_path() << "'";
        out_ << std::endl;
      }

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
           << "Active: '" << is_active() << "'" << std::endl;

      if (is_parameter()) {

        out_ << indent_ << i_tree_dumpable::tag
             << "Value set: '" << value_is_set() << "'" << std::endl;

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

  }  // end of namespace configuration

}  // end of namespace datatools
