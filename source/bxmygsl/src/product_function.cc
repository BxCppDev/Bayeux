//! \file mygsl/product_function.cc
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
#include <mygsl/product_function.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/properties.h>

namespace mygsl {

  MYGSL_UNARY_FUNCTOR_REGISTRATION_IMPLEMENT(product_function,
                                             "mygsl::product_function")

  product_function::product_function()
    : _f_()
    , _g_()
  {
    return;
  }

  product_function::product_function(const i_unary_function & f_,
                                     const i_unary_function & g_)
    : _f_(f_)
    , _g_(g_)
  {
    return;
  }

  product_function::~product_function()
  {
    return;
  }

  bool product_function::is_initialized() const
  {
    return ! _f_.is_null() && ! _g_.is_null();
  }

  void product_function::initialize(const datatools::properties & config_,
                                    const unary_function_dict_type & functors_)
  {
    // Parse configuration:
    if (_f_.is_null()) {
      if (config_.has_key("first_functor")) {
        const std::string & functor_name = config_.fetch_string("first_functor");
        unary_function_dict_type::const_iterator found = functors_.find(functor_name);
        DT_THROW_IF(found == functors_.end(), std::logic_error,
                    "No functor with name '" << functor_name << "'!");
        _f_.reset(found->second.to_const());
      }
    }

    if (_g_.is_null()) {
      if (config_.has_key("second_functor")) {
        const std::string & functor_name = config_.fetch_string("second_functor");
        unary_function_dict_type::const_iterator found = functors_.find(functor_name);
        DT_THROW_IF(found == functors_.end(), std::logic_error,
                    "No functor with name '" << functor_name << "'!");
        _g_.reset(found->second.to_const());
      }
    }

    _at_init();
    return;
  }

  void product_function::_at_init()
  {
    // Checking...
    DT_THROW_IF(_f_.is_null(), std::logic_error,
                "Missing first functor !");
    DT_THROW_IF(_g_.is_null(), std::logic_error,
                "Missing second functor !");
    return;
  }

  void product_function::reset()
  {
    _f_.reset();
    _g_.reset();
    return;
  }

  bool product_function::has_f() const
  {
    return !_f_.is_null();
  }

  bool product_function::has_g() const
  {
    return! _g_.is_null();
  }

  void product_function::set_f(const i_unary_function & f_)
  {
    _f_.reset(f_);
    return;
  }

  void product_function::set_g(const i_unary_function & g_)
  {
    _g_.reset(g_);
    return;
  }

  double product_function::_eval(double x_) const
  {
    return _f_.func().eval(x_) * _g_.func().eval(x_);
  }

  bool product_function::has_explicit_domain_of_definition() const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error,
                "Product function is not initialized!");
    return _f_.func().has_explicit_domain_of_definition()
      || _g_.func().has_explicit_domain_of_definition();
  }

  bool product_function::is_in_domain_of_definition(double x_) const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error,
                "Product function is not initialized!");
    if (_f_.func().has_explicit_domain_of_definition()) {
      if (!_f_.func().is_in_domain_of_definition(x_)) {
        return false;
      }
    }
    if (_g_.func().has_explicit_domain_of_definition()) {
      if (!_g_.func().is_in_domain_of_definition(x_)) {
        return false;
      }
    }
    return true;
  }

  double product_function::get_non_zero_domain_min() const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error,
                "Product function is not initialized!");
    double nzdmin = -std::numeric_limits<double>::infinity();
    if (_f_.func().has_non_zero_domain_min()) {
      if (_f_.func().get_non_zero_domain_min() > nzdmin) {
        nzdmin = _f_.func().get_non_zero_domain_min();
      }
    }
    if (_g_.func().has_non_zero_domain_min()) {
      if (_g_.func().get_non_zero_domain_min() > nzdmin) {
        nzdmin = _g_.func().get_non_zero_domain_min();
      }
    }
    return nzdmin;
  }

  double product_function::get_non_zero_domain_max() const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error,
                "Product function is not initialized!");
    double nzdmax = +std::numeric_limits<double>::infinity();
    if (_f_.func().has_non_zero_domain_max()) {
      if (_f_.func().get_non_zero_domain_max() < nzdmax) {
        nzdmax = _f_.func().get_non_zero_domain_max();
      }
    }
    if (_g_.func().has_non_zero_domain_max()) {
      if (_g_.func().get_non_zero_domain_max() < nzdmax) {
        nzdmax = _g_.func().get_non_zero_domain_max();
      }
    }
    return nzdmax;
  }

  void product_function::tree_dump(std::ostream & out_,
                                   const std::string & title_,
                                   const std::string & indent_,
                                   bool inherit_) const
  {
    this->i_unary_function::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << i_tree_dumpable::tag
         << "First functor  : ";
    if (has_f()) {
      out_ << "@[" << &_f_.func() << "]";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "Second functor : ";
    if (has_g()) {
      out_ << "@[" << &_g_.func() << "]";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    return;
  }

} // end of namespace mygsl
