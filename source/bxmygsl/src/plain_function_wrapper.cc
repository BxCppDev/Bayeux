// plain_function_wrapper.cc
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
#include <mygsl/plain_function_wrapper.h>

// Standard library:
#include <cmath>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/exception.h>

namespace mygsl {

  MYGSL_UNARY_FUNCTOR_REGISTRATION_IMPLEMENT(plain_function_wrapper,
                                             "mygsl::plain_function_wrapper")

  void plain_function_wrapper::set_plain_function_by_name(const std::string & plain_func_name_)
  {
    // std::cerr << "DEVEL: plain_function_wrapper::set_plain_function_by_name: "
    //           << "set plain function with name='" << plain_func_name_ << "'"
    //           << std::endl;
    DT_THROW_IF(!plain_function_is_known(plain_func_name_), std::logic_error,
                "Unknown plain function with name '" << plain_func_name_ << "'");
    set_plain_function(get_simple_function(plain_func_name_));
    // std::cerr << "DEVEL: plain_function_wrapper::set_plain_function_by_name: "
    //           << "plain function address=[" << (void*) _plain_function_ << "]"
    //           << std::endl;
    return;
  }

  void plain_function_wrapper::set_plain_function(plain_function_type func_)
  {
    _plain_function_ = func_;
    return;
  }

  bool plain_function_wrapper::has_plain_function() const
  {
    return _plain_function_ != 0;
  }

  plain_function_wrapper::plain_function_wrapper()
  {
    _plain_function_ = 0;
    return;
  }

  plain_function_wrapper::plain_function_wrapper(const std::string & plain_func_name_)
  {
    _plain_function_ = 0;
    set_plain_function_by_name(plain_func_name_);
    return;
  }

  plain_function_wrapper::plain_function_wrapper(const plain_function_type & func_)
  {
    set_plain_function(func_);
    return;
  }

  plain_function_wrapper::~plain_function_wrapper()
  {
    _plain_function_ = 0;
    return;
  }

  double plain_function_wrapper::_eval(double x_) const
  {
    DT_THROW_IF(!has_plain_function(), std::logic_error,
                "Missing plain function!");
    return (*_plain_function_)(x_);
  }

  bool plain_function_wrapper::is_initialized() const
  {
    return _plain_function_ != 0;
  }

  void plain_function_wrapper::reset()
  {
    _plain_function_ = 0;
    return;
  }

  void plain_function_wrapper::initialize(const datatools::properties & config_,
                                          unary_function_dict_type & /* functors_ */)
  {
    std::string plain_function_name;
    if (!has_plain_function()) {
      if (config_.has_key("function")) {
        plain_function_name = config_.fetch_string("function");
      }
    }

    if (! plain_function_name.empty()) {
      set_plain_function_by_name(plain_function_name);
    }

    DT_THROW_IF(!has_plain_function(), std::logic_error, "Plain function is not defined!");
    return;
  }

} // end of namespace mygsl
