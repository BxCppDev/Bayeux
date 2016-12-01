//! \file mygsl/gaussian_function.cc
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
#include <mygsl/gaussian_function.h>

// Third party:
// - GSL:
#include <gsl/gsl_randist.h>
// - Bayeux/datatools:
#include <datatools/exception.h>

namespace mygsl {

  MYGSL_UNARY_FUNCTOR_REGISTRATION_IMPLEMENT(gaussian_function,
                                             "mygsl::gaussian_function")

  DATATOOLS_CLONEABLE_IMPLEMENTATION(gaussian_function)

  void gaussian_function::_set_defaults()
  {
    datatools::invalidate(_mu_);
    datatools::invalidate(_sigma_);
    datatools::invalidate(_amplitude_);
    datatools::invalidate(_factor_);
    return;
  }

  gaussian_function::gaussian_function()
  {
    _set_defaults();
    return;
  }

  gaussian_function::gaussian_function(double sigma_, double mu_)
  {
    _set_defaults();
    set_sigma(sigma_);
    set_mu(mu_);
    _init_();
    return;
  }

  gaussian_function::gaussian_function(double sigma_, double mu_, double amplitude_)
  {
    _set_defaults();
    set_sigma(sigma_);
    set_mu(mu_);
    set_amplitude(amplitude_);
    _init_();
    return;
  }

  gaussian_function::~gaussian_function()
  {
    return;
  }

  bool gaussian_function::is_initialized() const
  {
    return datatools::is_valid(_factor_);
  }

  void gaussian_function::initialize(const datatools::properties & config_,
                                     const unary_function_dict_type & functors_)
  {
    this->i_unary_function::_base_initialize(config_, functors_);

    // Parse configuration:
    if (!datatools::is_valid(_sigma_)) {
      if (config_.has_key("sigma")) {
        double sigma = config_.fetch_real("sigma");
        set_sigma(sigma);
      }
    }

    if (!datatools::is_valid(_mu_)) {
      if (config_.has_key("mu")) {
        double mu = config_.fetch_real("mu");
        set_mu(mu);
      }
    }

    if (!datatools::is_valid(_amplitude_)) {
      if (config_.has_key("amplitude")) {
        double a = config_.fetch_real("amplitude");
        set_amplitude(a);
      }
    }

    _init_();
    return;
  }

  void gaussian_function::_init_()
  {
    // Checks:
    DT_THROW_IF(!datatools::is_valid(_mu_),
                std::logic_error,
                "Invalid mu!");
    DT_THROW_IF(!datatools::is_valid(_sigma_),
                std::logic_error,
                "Invalid sigma!");
    _factor_ = 1.0;
    if (datatools::is_valid(_amplitude_)) {
      _factor_ = _amplitude_ / gsl_ran_gaussian_pdf(0.0, _sigma_);
    }
    return;
  }

  void gaussian_function::reset()
  {
    _set_defaults();
    this->i_unary_function::_base_reset();
    return;
  }

  void gaussian_function::set_sigma(double sigma_)
  {
    DT_THROW_IF(datatools::is_valid(sigma_) && sigma_ <= 0.0, std::logic_error,
                "Invalid negative or null sigma!");
    _sigma_ = sigma_;
    return;
  }

  double gaussian_function::get_sigma() const
  {
    return _sigma_;
  }

  void gaussian_function::set_mu(double mu_)
  {
     _mu_ = mu_;
    return;
  }

  double gaussian_function::get_mu() const
  {
    return _mu_;
  }

  void gaussian_function::set_amplitude(double amplitude_)
  {
    DT_THROW_IF(!datatools::is_valid(amplitude_), std::logic_error,
                "Invalid amplitude!");
    _amplitude_ = amplitude_;
    return;
  }

  bool gaussian_function::has_amplitude() const
  {
    return datatools::is_valid(_amplitude_);
  }

  double gaussian_function::get_amplitude() const
  {
    return _amplitude_;
  }

  void gaussian_function::reset_amplitude()
  {
    datatools::invalidate(_amplitude_);
    datatools::invalidate(_factor_);
    // _factor_ = 1.0;
    return;
  }

  double gaussian_function::_eval(double x_) const
  {
    return _factor_ * gsl_ran_gaussian_pdf(x_ - _mu_, _sigma_);
  }

  void gaussian_function::tree_dump(std::ostream & out_,
                                    const std::string & title_,
                                    const std::string & indent_,
                                    bool inherit_) const
  {
    this->i_unary_function::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << i_tree_dumpable::tag
         << "Sigma : " << _sigma_ << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Mu : " << _mu_ << std::endl;

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "Amplitude : " << _amplitude_ << std::endl;

    return;
  }

} // end of namespace mygsl
