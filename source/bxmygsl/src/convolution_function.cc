//! \file mygsl/convolution_function.cc
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>
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
#include <mygsl/convolution_function.h>

// Standard library:
#include <limits>     // std::numeric_limits<double>
#include <cmath>      // std::exp(), M_PI, std::isinf()...
#include <stdexcept>  // std::runtime_error, std::logic_error
#include <iostream>   // std::cout, std::cerr, std::clog, std::endl
#include <sstream>    // std::ostringstream

// Third party:
// - GSL:
#include <gsl/gsl_integration.h> /** gsl_integration_workspace_alloc,
                                  *  gsl_integration_workspace_free,
                                  *  gsl_integration_qag
                                  */
#include <gsl/gsl_errno.h> // gsl_strerror

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/properties.h>

namespace mygsl {

  MYGSL_UNARY_FUNCTOR_REGISTRATION_IMPLEMENT(convolution_function,
                                             "mygsl::convolution_function")

  /// \brief PIMPL-ized GSL workspace wrapper
  class convolution_function::gsl_pimpl
  {
  public:

    /// Constructor
    gsl_pimpl(size_t = 1000);

    /// Destructor
    ~gsl_pimpl();

    /// Return the maximum size of the embedded GSL workspace
    size_t get_limit() const;

    /// Return a pointer to the embedded GSL workspace
    gsl_integration_workspace * get_workspace();

  private:

    std::size_t _limit_; ///< Maximum size of the GSL workspace
    gsl_integration_workspace * _workspace_ = nullptr; ///< GSL integration workspace handle

  };

  size_t convolution_function::gsl_pimpl::get_limit() const
  {
    return _limit_;
  }

  gsl_integration_workspace * convolution_function::gsl_pimpl::get_workspace()
  {
    return _workspace_;
  }

  convolution_function::gsl_pimpl::gsl_pimpl(std::size_t limit_)
  {
    _limit_ = limit_;
    _workspace_ = nullptr;
    if (_workspace_ == nullptr) {
      // std::cerr << "DEBUG: "
      //           << "convolution_function::convolution_function: "
      //           << "Initializing the GSM integration workspace..."
      //           << '\n';
      _workspace_ = gsl_integration_workspace_alloc(_limit_);
    }
    return;
  }

  convolution_function::gsl_pimpl::~gsl_pimpl()
  {
    if (_workspace_ != nullptr) {
      // std::cerr << "DEBUG: "
      //           << "convolution_function::convolution_function: "
      //           << "Terminating the GSM integration workspace..."
      //           << '\n';
      gsl_integration_workspace_free(_workspace_);
      _workspace_ = nullptr;
    }
    _limit_ = 0;
    return;
  }

  convolution_function::convolution_function()
  {
    _f_ = nullptr;
    _g_ = nullptr;
    _limit_ = 0;
    return;
  }

  convolution_function::convolution_function(const i_unary_function & f_,
                                             const i_unary_function & g_,
                                             size_t limit_)
  {
    _f_ = nullptr;
    _g_ = nullptr;
    _limit_ = 0;
    set_f(f_);
    set_g(g_);
    set_limit(limit_);
    return;
  }

  convolution_function::~convolution_function()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  bool convolution_function::is_initialized() const
  {
    return _f_ != nullptr && _g_ != nullptr && _limit_ > 0 && _gsl_work_ != nullptr;
  }

  void convolution_function::initialize(const datatools::properties & config_,
                                    unary_function_dict_type & functors_)
  {
    // Parse configuration:
    if (_f_ == nullptr) {
      if (config_.has_key("first_functor")) {
        const std::string & functor_name = config_.fetch_string("first_functor");
        unary_function_dict_type::const_iterator found = functors_.find(functor_name);
        DT_THROW_IF(found == functors_.end(), std::logic_error,
                    "No functor with name '" << functor_name << "'!");
        const i_unary_function & func = found->second.get();
        set_f(func);
      }
    }

    if (_g_ == nullptr) {
      if (config_.has_key("second_functor")) {
        const std::string & functor_name = config_.fetch_string("second_functor");
        unary_function_dict_type::const_iterator found = functors_.find(functor_name);
        DT_THROW_IF(found == functors_.end(), std::logic_error,
                    "No functor with name '" << functor_name << "'!");
        const i_unary_function & func = found->second.get();
        set_g(func);
      }
    }

    if (_limit_ == 0) {
      if (config_.has_key("limit")) {
        unsigned int limit = config_.fetch_positive_integer("limit");
        DT_THROW_IF(limit < 10, std::range_error,
                    "Limit to small [" << limit << "]!");
        set_limit(limit);
      }
    }

    // Checking...
    DT_THROW_IF(_f_ == nullptr, std::logic_error,
                "Missing first functor !");
    DT_THROW_IF(_g_ == nullptr, std::logic_error,
                "Missing second functor !");
    if (_limit_ == 0) {
      set_limit(1000);
    }
    _at_init();
    return;
  }

  void convolution_function::reset()
  {
    _at_reset();
    _limit_ = 0;
    _f_ = nullptr;
    _g_ = nullptr;
    return;
  }

  void convolution_function::_at_init()
  {
    _gsl_work_ = new convolution_function::gsl_pimpl(_limit_);
    return;
  }

  void convolution_function::_at_reset()
  {
    if (_gsl_work_) {
      delete _gsl_work_;
    }
    return;
  }

  bool convolution_function::has_f() const
  {
    return _f_ != nullptr;
  }

  bool convolution_function::has_g() const
  {
    return _g_ != nullptr;
  }

  void convolution_function::set_f(const i_unary_function & f_)
  {
    DT_THROW_IF(!f_.is_initialized(), std::logic_error,
                "First functor is not initialized!");
    _f_ = &f_;
    return;
  }

  void convolution_function::set_g(const i_unary_function & g_)
  {
    DT_THROW_IF(!g_.is_initialized(), std::logic_error,
                "Second functor is not initialized!");
    _g_ = &g_;
    return;
  }

  const i_unary_function & convolution_function::get_f() const
  {
    DT_THROW_IF(!has_f(), std::logic_error,
                "Convolution function is not initialized!");
    return *_f_;
  }

  const i_unary_function & convolution_function::get_g() const
  {
    DT_THROW_IF(!has_g(), std::logic_error,
                "Convolution function is not initialized!");
    return *_g_;
  }

  void convolution_function::set_limit(std::size_t limit_)
  {
    _limit_ = limit_;
    return;
  }

  std::size_t convolution_function::get_limit() const
  {
    return _limit_;
  }

  void convolution_function::tree_dump(std::ostream & out_,
                                       const std::string & title_,
                                       const std::string & indent_,
                                       bool inherit_) const
  {
    this->i_unary_function::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << i_tree_dumpable::tag
         << "First functor  : ";
    if (has_f()) {
      out_ << "@[" << _f_ << "]";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ << i_tree_dumpable::tag
         << "Second functor : ";
    if (has_g()) {
      out_ << "@[" << _g_ << "]";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
         << "Limit : ";
    out_ << _limit_ << std::endl;

    return;
  }

  double convolution_function::_eval(double t_) const
  {
    convolution_term ct(*this, t_);

    gsl_function F;
    F.function = &convolution_function::convolution_func;
    F.params   = &ct;

    double result = std::numeric_limits<double>::quiet_NaN();
    double abserr;
    double epsabs = 0.0;
    double epsrel = 1.0e-5;

    if (ct.has_non_zero_domain_min() && ct.has_non_zero_domain_max()) {
      double tmin = ct.get_non_zero_domain_min();
      double tmax = ct.get_non_zero_domain_max();
      gsl_error_handler_t * gsl_eh_save = gsl_set_error_handler_off();
      int code = gsl_integration_qag(&F,
                                     tmin, tmax,
                                     epsabs, epsrel,
                                     _gsl_work_->get_limit(),
                                     GSL_INTEG_GAUSS61,
                                     _gsl_work_->get_workspace(),
                                     &result, &abserr);
      gsl_set_error_handler(gsl_eh_save);
      DT_THROW_IF(code != 0, std::runtime_error,
                  "GSL QAG integration error : " << gsl_strerror(code) << " !");
    } else {
      DT_THROW(std::logic_error, "Unsupported integration mode !");
    }
    return result;
  }

  void convolution_function::convolution_term::print(std::ostream & out_,
                                                  const std::string & title_) const
  {
    if (! title_.empty()) {
      out_ << title_ << " : \n";
    }
    out_ << "|-- Convolution function : " << _cs_ << std::endl;
    out_ << "|-- Time : " << _t_    << std::endl;
    out_ << "|-- Tmin : " << _tmin_ << std::endl;
    out_ << "`-- Tmax : " << _tmax_ << std::endl;
    return;
  }

  convolution_function::convolution_term::convolution_term(const convolution_function & cs_,
                                                           double t_)
  {
    _cs_   = &cs_;
    _t_    = t_;
    double tmin1 = -std::numeric_limits<double>::infinity();
    double tmax1 = +std::numeric_limits<double>::infinity();
    double tmin2 = -std::numeric_limits<double>::infinity();
    double tmax2 = +std::numeric_limits<double>::infinity();
    if (_cs_->get_f().has_non_zero_domain_min()) {
      tmin1 = _cs_->get_f().get_non_zero_domain_min();
    }
    if (_cs_->get_f().has_non_zero_domain_max()) {
      tmax1 = _cs_->get_f().get_non_zero_domain_max();
    }
    if (_cs_->get_g().has_non_zero_domain_min()) {
      tmax2 = _t_ - _cs_->get_g().get_non_zero_domain_min();
    }
    if (_cs_->get_g().has_non_zero_domain_max()) {
      tmin2 = _t_ - _cs_->get_g().get_non_zero_domain_max();
    }
    _tmin_ = std::max(tmin1,tmin2);
    _tmax_ = std::max(tmax1,tmax2);
    return;
  }

  /// The minimum bound of the non-zero domain
  double convolution_function::convolution_term::get_non_zero_domain_min() const
  {
    return _tmin_;
  }

  /// The minimum bound of the non-zero domain
  double convolution_function::convolution_term::get_non_zero_domain_max() const
  {
    return _tmax_;
  }

  /// Function interface
  double convolution_function::convolution_term::_eval(double t_) const
  {
    double res = 0.0;
    if (! std::isinf(_tmin_) && t_ < _tmin_) {
    } else if (! std::isinf(_tmax_) && t_ >= _tmax_) {
    } else {
      res = _cs_->get_f().eval(t_) * _cs_->get_g().eval(_t_ - t_);
    }
    return res;
  }

  double convolution_function::convolution_func(double tp_, void * params_)
  {
    const convolution_term * ct = static_cast<const convolution_term *>(params_);
    double res = ct->eval(tp_);
    return res;
  }

} // end of namespace mygsl
