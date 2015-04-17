// function_with_domain.cc
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
#include <mygsl/function_with_domain.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/exception.h>

namespace mygsl {

  MYGSL_UNARY_FUNCTOR_REGISTRATION_IMPLEMENT(function_with_domain,
                                             "mygsl::function_with_domain");

  bool function_with_domain::has_domain_of_definition() const
  {
    return _domain_of_definition_.is_valid();
  }

  void function_with_domain::set_domain_of_definition(const interval & domain_)
  {
    _domain_of_definition_ = domain_;
    return;
  }

  const interval & function_with_domain::get_domain_of_definition() const
  {
    return _domain_of_definition_;
  }

  bool function_with_domain::has_functor() const
  {
    return _functor_ != 0;
  }

  void function_with_domain::set_functor(const i_unary_function & functor_)
  {
    DT_THROW_IF(!functor_.is_initialized(), std::logic_error,
                "Functor is not initialized!");
    _functor_ = &functor_;
    return;
  }

  function_with_domain::function_with_domain()
  {
    _functor_ = 0;
    _domain_of_definition_ = interval::make_no_limit();
    return;
  }

  function_with_domain::function_with_domain(const i_unary_function & functor_,
                                             const interval & domain_)
  {
    set_functor(functor_);
    set_domain_of_definition(domain_);
    return;
  }

  function_with_domain::function_with_domain(const i_unary_function & functor_)
  {
    set_functor(functor_);
    _domain_of_definition_ = interval::make_no_limit ();
    return;
  }

  function_with_domain::~function_with_domain()
  {
    return;
  }

  bool function_with_domain::has_explicit_domain_of_definition() const
  {
    return ! _domain_of_definition_.is_no_limit();
  }

  bool function_with_domain::is_in_domain_of_definition(double x_) const
  {
    return _domain_of_definition_.is_in (x_);
  }

  double function_with_domain::_eval(double x_) const
  {
    return _functor_->eval(x_);
  }

  bool function_with_domain::is_initialized() const
  {
    return has_functor() && has_domain_of_definition();
  }

  void function_with_domain::initialize(const datatools::properties & config_,
                                        unary_function_dict_type & functors_)
  {
    if (!has_functor()) {
      if (config_.has_key("functor")) {
        const std::string & functor_name = config_.fetch_string("functor");
        unary_function_dict_type::const_iterator found = functors_.find(functor_name);
        DT_THROW_IF(found == functors_.end(), std::logic_error,
                    "No functor with name '" << functor_name << "'!");
        const i_unary_function & func = found->second.get();
        set_functor(func);
      }
    }

    DT_THROW_IF(!has_functor(), std::logic_error, "Functor is not defined!");
    return;
  }

  void function_with_domain::reset()
  {
    DT_THROW_IF(!is_initialized(), std::logic_error,
                "Functor is not initialized!");
    _functor_ = 0;
    _domain_of_definition_.reset();
    return;
  }

} // end of namespace mygsl
