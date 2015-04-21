// parameter_store.cc
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
#include <mygsl/parameter_store.h>

// Third party:
// - Bayeux/datatools :
#include <datatools/exception.h>
#include <datatools/logger.h>

namespace mygsl {

  parameter_store::parameter_record::parameter_record()
  {
    _parent_ = 0;
    _type_ = datatools::TYPE_NONE;
    _index_ = INVALID_INDEX;
    _value_set_ = false;
    return;
  }

  parameter_store::parameter_record::parameter_record(parameter_store & parent_)
  {
    set_parent(parent_);
    _type_ = datatools::TYPE_NONE;
    _index_ = INVALID_INDEX;
    _value_set_ = false;
    return;
  }

  void parameter_store::parameter_record::reset()
  {
    _name_.clear();
    _type_ = datatools::TYPE_NONE;
    _description_.clear();
    _index_ = INVALID_INDEX;
    _value_set_ = false;
    _parent_ = 0;
    return;
  }

  void parameter_store::parameter_record::set_parent(parameter_store & parent_)
  {
    _parent_ = &parent_;
    return;
  }

  void parameter_store::parameter_record::set_index(int index_)
  {
    _index_ = index_;
    return;
  }

  int parameter_store::parameter_record::get_index() const
  {
    return _index_;
  }

  void parameter_store::parameter_record::set_name(const std::string & name_)
  {
    _name_ = name_;
    return;
  }

  const std::string & parameter_store::parameter_record::get_name() const
  {
    return _name_;
  }

  void parameter_store::parameter_record::set_type(datatools::basic_type t_)
  {
    _type_ = t_;
    return;
  }

  datatools::basic_type parameter_store::parameter_record::get_type() const
  {
    return _type_;
  }

  bool parameter_store::parameter_record::is_boolean() const
  {
    return _type_ == datatools::TYPE_BOOLEAN;
  }

  bool parameter_store::parameter_record::is_integer() const
  {
    return _type_ == datatools::TYPE_INTEGER;
  }

  bool parameter_store::parameter_record::is_real() const
  {
    return _type_ == datatools::TYPE_REAL;
  }

  bool parameter_store::parameter_record::is_string() const
  {
    return _type_ == datatools::TYPE_STRING;
  }

  void parameter_store::parameter_record::set_description(const std::string & description_)
  {
    _description_ = description_;
    return;
  }

  const std::string & parameter_store::parameter_record::get_description() const
  {
    return _description_;
  }

  bool parameter_store::parameter_record::is_value_set() const
  {
    return _value_set_;
  }

  void parameter_store::parameter_record::set_boolean_value(bool val_)
  {
    _value_set_ = true;
    _value_ = val_;
    _parent_->_update_parameters();
    return;
  }

  void parameter_store::parameter_record::set_integer_value(int val_)
  {
    _value_set_ = true;
    _value_ = val_;
    _parent_->_update_parameters();
    return;
  }

  void parameter_store::parameter_record::set_real_value(double val_)
  {
    _value_set_ = true;
    _value_ = val_;
    _parent_->_update_parameters();
    return;
  }

  void parameter_store::parameter_record::set_string_value(const std::string & val_)
  {
    _value_set_ = true;
    _value_ = val_;
    _parent_->_update_parameters();
    return;
  }

  void parameter_store::parameter_record::unset_value()
  {
    _value_set_ = false;
    _value_ = std::numeric_limits<double>::quiet_NaN();
    _parent_->_update_parameters();
    return;
  }

  bool parameter_store::parameter_record::to_boolean() const
  {
    DT_THROW_IF(!_value_set_, std::logic_error, "Parameter's boolean value is not set!");
    return boost::get<bool>(_value_);
  }

  int parameter_store::parameter_record::to_integer() const
  {
    DT_THROW_IF(!_value_set_, std::logic_error, "Parameter's integer value is not set!");
    // std::cerr << "DEVEL: " << "parameter_store::parameter_record::to_real: "
    //        << std::endl;
    return boost::get<int>(_value_);
  }

  double parameter_store::parameter_record::to_real() const
  {
    DT_THROW_IF(!_value_set_, std::logic_error, "Parameter's real value is not set!");
    return boost::get<double>(_value_);
  }

  const std::string & parameter_store::parameter_record::to_string() const
  {
    DT_THROW_IF(!_value_set_, std::logic_error, "Parameter's string value is not set!");
    return boost::get<std::string>(_value_);
  }

  parameter_store::parameter_store()
  {
    return;
  }

  parameter_store::~parameter_store()
  {
    return;
  }

  void parameter_store::reset()
  {
    _indexed_parameters_.clear();
    _parameters_.clear();
    return;
  }

  int parameter_store::add(const std::string & name_,
                           datatools::basic_type type_,
                           const std::string & description_)
  {
    parameter_dict_type::iterator found = _parameters_.find(name_);
    DT_THROW_IF(found != _parameters_.end(),
                std::logic_error,
                "Parameter with name '" << name_ << "' already exists!");
    {
      parameter_record dummy;
      _parameters_[name_] = dummy;
    }
    int param_next_index = _indexed_parameters_.size();
    parameter_record & pr = _parameters_.find(name_)->second;
    pr.set_parent(*this);
    pr.set_name(name_);
    pr.set_type(type_);
    pr.set_description(description_);
    pr.set_index(param_next_index);
    pr.unset_value();
    _indexed_parameters_.push_back(&pr);
    return param_next_index;
  }

  int parameter_store::get_parameter_index(const std::string & parameter_name_) const
  {
    parameter_dict_type::const_iterator found
      = _parameters_.find(parameter_name_);
    DT_THROW_IF(found == _parameters_.end(),
                std::logic_error,
                "Invalid parameter name '" << parameter_name_ << "'!");
    return found->second.get_index();
  }

  void parameter_store::remove(const std::string & name_)
  {
    parameter_dict_type::iterator found = _parameters_.find(name_);
    DT_THROW_IF(found == _parameters_.end(),
                std::logic_error, "Parameter with name '" << name_ << "' does not exist!");
    int index = found->second._index_;
    _indexed_parameters_[index] = 0;
    _parameters_.erase(found);
    return;
  }

  bool parameter_store::has(const std::string & name_) const
  {
    return _parameters_.find(name_) != _parameters_.end();
  }

  const parameter_store::parameter_record & parameter_store::get(const std::string & name_) const
  {
    return const_cast<parameter_store*>(this)->grab(name_);
  }

  bool parameter_store::index_is_valid(int index_) const
  {
    if (index_ < 0 && index_ >= (int)_indexed_parameters_.size()) return false;
    if (_indexed_parameters_[index_] == 0) return false;
    return true;
  }

  const parameter_store::parameter_record & parameter_store::get(int index_, bool do_check_) const
  {
    return const_cast<parameter_store*>(this)->grab(index_, do_check_);
  }

  parameter_store::parameter_record & parameter_store::grab(int index_, bool do_check_)
  {
    if (do_check_) {
      DT_THROW_IF(index_ < 0 && index_ >= (int)_indexed_parameters_.size(), std::range_error,
                  "Invalid parameter storage index [" << index_ << "]!");
    }
    parameter_record * pr = _indexed_parameters_[index_];
    if (do_check_) {
      DT_THROW_IF(pr == 0, std::logic_error, "Invalid parameter at indx [" << index_ << "]!");
    }
    return *pr;
  }

  parameter_store::parameter_record & parameter_store::grab(const std::string & name_)
  {
    parameter_dict_type::iterator found = _parameters_.find(name_);
    DT_THROW_IF(found == _parameters_.end(),
                std::logic_error, "Parameter with name '" << name_ << "' does not exist!");
    return found->second;
  }

  void parameter_store::tree_dump(std::ostream & out_,
                                  const std::string & title_,
                                  const std::string & indent_,
                                  bool inherit_) const
  {
    if (! title_.empty()) out_ << indent_ << title_ << std::endl;

    if (_parameters_.size() == 0) {
      out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_);
    } else {
      out_ << indent_ << i_tree_dumpable::tag;
    }
    out_ << "Number of parameters: ";
    if (_parameters_.size() == 0) {
      out_<< "<none>";
    } else {
      out_ << '[' << _parameters_.size() << ']';
    }
    out_<< std::endl;
    for (parameter_dict_type::const_iterator i = _parameters_.begin();
         i != _parameters_.end();
         i++) {
      const std::string & param_name = i->first;
      const parameter_record & pr = i->second;
      out_ << indent_;
      parameter_dict_type::const_iterator j = i;
      j++;
      std::ostringstream indent_oss;
      indent_oss << indent_;
      if (j == _parameters_.end()) {
        out_ << i_tree_dumpable::inherit_tag(inherit_);
        indent_oss << i_tree_dumpable::inherit_skip_tag(inherit_);
      } else {
        out_ << i_tree_dumpable::tag;
        indent_oss << i_tree_dumpable::skip_tag;
      }
      out_ << "Parameter '" << param_name << "' : ";
      out_ << "type='" << datatools::get_label_from_type(pr.get_type())
           << "' ";
      out_ << "index=[" << pr.get_index() << "] ";
      out_ << "value=";
      if (pr.is_value_set()) {
        if (pr.is_string()) {
          out_ << '"';
        }
        out_ << pr._value_;
        if (pr.is_string()) {
          out_ << '"';
        }
      } else {
        out_ << "<not set>";
      }
      out_ << std::endl;
    }
    return;
  }

  bool parameter_store::is_registered_subscriber(const i_subscriber & s_) const
  {
    for (std::list<i_subscriber*>::const_iterator i = _subscribers_.begin();
         i != _subscribers_.end();
         i++) {
      if (*i == &s_) {
        return true;
      }
    }
    return false;
  }

  void parameter_store::register_subscriber(i_subscriber & s_)
  {
    DT_THROW_IF(is_registered_subscriber(s_), std::logic_error,
                "Subscriber at [" << &s_ << "] is already registered in the store!");
    _subscribers_.push_back(&s_);
    return;
  }

  void parameter_store::unregister_subscriber(i_subscriber & s_)
  {
    DT_THROW_IF(!is_registered_subscriber(s_), std::logic_error,
                "Subscriber at [" << &s_ << "] is not registered in the store!");
    std::list<i_subscriber*>::iterator found = _subscribers_.end();
    for (std::list<i_subscriber*>::iterator i = _subscribers_.begin();
         i != _subscribers_.end();
         i++) {
      if (*i == &s_) {
        found = i;
        break;
      }
    }
    if (found != _subscribers_.end()) {
      _subscribers_.erase(found);
    }
    return;
  }

  void parameter_store::_update_parameters()
  {
    for (std::list<i_subscriber*>::iterator i = _subscribers_.begin();
         i != _subscribers_.end();
         i++) {
      // DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "Send update signal to subscriber #" << j++);
      (*i)->update_parameters();
    }
    return;
  }

} // end of namespace mygsl
