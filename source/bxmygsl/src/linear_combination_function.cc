//! \file mygsl/linear_combination_function.cc
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
#include <mygsl/linear_combination_function.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/properties.h>

namespace mygsl {

  MYGSL_UNARY_FUNCTOR_REGISTRATION_IMPLEMENT(linear_combination_function,
                                             "mygsl::linear_combination_function");

  void linear_combination_function::_set_defaults()
  {
    _explicit_domain_of_definition_ = false;
    datatools::invalidate(_non_zero_domain_min_);
    datatools::invalidate(_non_zero_domain_max_);
    return;
  }

  linear_combination_function::linear_combination_function()
  {
    _set_defaults();
    return;
  }

  linear_combination_function::linear_combination_function(double w1_,
                                                           const i_unary_function & f1_,
                                                           double w2_,
                                                           const i_unary_function & f2_)
  {
    _set_defaults();
    add(w1_, f1_);
    add(w2_, f2_);
    return;
  }

  linear_combination_function::~linear_combination_function()
  {
    _terms_.clear();
    return;
  }

  bool linear_combination_function::is_initialized() const
  {
    return _terms_.size() > 0;
  }

  void linear_combination_function::initialize(const datatools::properties & config_,
                                               unary_function_dict_type & functors_)
  {
    // Parse configuration:
    if (_terms_.size() == 0) {
      /*
      if (config_.has_key("first_functor")) {
        const std::string & functor_name = config_.fetch_string("first_functor");
        unary_function_dict_type::const_iterator found = functors_.find(functor_name);
        DT_THROW_IF(found == functors_.end(), std::logic_error,
                    "No functor with name '" << functor_name << "'!");
        const i_unary_function & func = found->second.get();
        set_f(func);
      }
    }

    if (_g_ == 0) {
      if (config_.has_key("second_functor")) {
        const std::string & functor_name = config_.fetch_string("second_functor");
        unary_function_dict_type::const_iterator found = functors_.find(functor_name);
        DT_THROW_IF(found == functors_.end(), std::logic_error,
                    "No functor with name '" << functor_name << "'!");
        const i_unary_function & func = found->second.get();
        set_g(func);
      }
    }
      */
    }

    // Checking...
    DT_THROW_IF(_terms_.size() == 0, std::logic_error, "Missing terms !");
    return;
  }

  void linear_combination_function::reset()
  {
    _terms_.clear();
    _set_defaults();
    return;
  }

  int linear_combination_function::add(double w_, const i_unary_function & f_)
  {

    DT_THROW_IF(!datatools::is_valid(w_), std::logic_error, "Invalid functor weight !");
    DT_THROW_IF(!f_.is_initialized(), std::logic_error,
                "Functor is not initialized!");
    int idx = _terms_.size();
    term_type tt;
    tt.weight = w_;
    tt.functor = &f_;
    _terms_.push_back(tt);
    _recompute();
    return idx;
  }

  void linear_combination_function::change_weight(int term_index_, double weight_)
  {
    DT_THROW_IF(term_index_ < 0 || term_index_ >= _terms_.size(),
                std::range_error, "Invalid functor term index !");
    DT_THROW_IF(!datatools::is_valid(weight_), std::logic_error, "Invalid functor weight !");
    _terms_[term_index_].weight = weight_;
    return;
  }

  double linear_combination_function::_eval(double x_) const
  {
    double tmp = 0.0;
    for (int i = 0; i < (int) _terms_.size(); i++) {
      tmp += _terms_[i].weight * _terms_[i].functor->eval(x_);
    }
    return tmp;
  }

  void linear_combination_function::_recompute()
  {
    _set_defaults();
    for (int i = 0; i < (int) _terms_.size(); i++) {
      if (_terms_[i].functor->has_explicit_domain_of_definition()) {
       _explicit_domain_of_definition_ = true;
       break;
      }
    }

    for (int i = 0; i < (int) _terms_.size(); i++) {
      if (_terms_[i].functor->has_non_zero_domain_min()) {
        if (!datatools::is_valid(_non_zero_domain_min_)
            || _terms_[i].functor->get_non_zero_domain_min() > _non_zero_domain_min_) {
          _non_zero_domain_min_ = _terms_[i].functor->get_non_zero_domain_min();
        }
      } else {
        datatools::invalidate(_non_zero_domain_min_);
        break;
      }
    }

    for (int i = 0; i < (int) _terms_.size(); i++) {
      if (_terms_[i].functor->has_non_zero_domain_max()) {
        if (!datatools::is_valid(_non_zero_domain_max_)
            || _terms_[i].functor->get_non_zero_domain_max() < _non_zero_domain_max_) {
          _non_zero_domain_max_ = _terms_[i].functor->get_non_zero_domain_max();
        }
      } else {
        datatools::invalidate(_non_zero_domain_max_);
        break;
      }
    }

    return;
  }

  bool linear_combination_function::has_explicit_domain_of_definition() const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error,
                "Linear combination function is not initialized!");
    return _explicit_domain_of_definition_;
  }

  bool linear_combination_function::is_in_domain_of_definition(double x_) const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error,
                "Linear combination function is not initialized!");
    bool in_dod = true;
    for (int i = 0; i < (int) _terms_.size(); i++) {
      if (_terms_[i].functor->has_explicit_domain_of_definition()) {
        if (!_terms_[i].functor->is_in_domain_of_definition(x_)) {
          in_dod = false;
          break;
        }
      }
    }
    return in_dod;
  }

  double linear_combination_function::get_non_zero_domain_min() const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error,
                "Linear combination function is not initialized!");
    return _non_zero_domain_min_;
  }

  double linear_combination_function::get_non_zero_domain_max() const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error,
                "Linear combination function is not initialized!");
    return _non_zero_domain_max_;
  }

  void linear_combination_function::tree_dump(std::ostream & out_,
                                              const std::string & title_,
                                              const std::string & indent_,
                                              bool inherit_) const
  {
    this->i_unary_function::tree_dump(out_, title_, indent_, true);

    for (int i = 0; i < (int) _terms_.size(); i++) {
      out_ << indent_ << i_tree_dumpable::tag
           << "Functor @[" << _terms_[i].functor << "] with weight = "
           << _terms_[i].weight
           << std::endl;
    }

    out_ << indent_ << i_tree_dumpable::tag
         << "Explicit domain of definition : " << _explicit_domain_of_definition_ << std::endl;
    out_ << indent_ << i_tree_dumpable::tag
         << "Non-zero domain min : " << _non_zero_domain_min_ << std::endl;

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "Non-zero domain max : " << _non_zero_domain_max_ << std::endl;

    return;
  }

} // end of namespace mygsl
