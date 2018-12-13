// \file mygsl/kernel_smoother.cc

// Ourselves:
#include <mygsl/kernel_smoother.h>

// Third party:
// - GSL:
#include <gsl/gsl_math.h>

namespace mygsl {

  // virtual
  i_kernel_smoother::~i_kernel_smoother()
  {
    return;
  }

  gauss_kernel_smoother::gauss_kernel_smoother(const double b_)
  {
    DT_THROW_IF(b_ <= 0.0, std::domain_error,
                "Invalid 'b' value for the Gauss kernel!");
    _b_ = b_;
    return;
  }

  gauss_kernel_smoother::~gauss_kernel_smoother()
  {
    return;
  }

  double gauss_kernel_smoother::eval(double xs_, double xi_) const
  {
    return std::exp(-0.5 * gsl_pow_2((xs_ - xi_)/_b_));
  }
  
  nw_sampled_function_smoother::nw_sampled_function_smoother(const mygsl::tabulated_function & tf_,
                                                             i_kernel_smoother & ks_)
    : _tf_(tf_)
    , _ks_free_needed_(false)
    , _ks_(&ks_)
  {
    return;
  }
  
  nw_sampled_function_smoother::nw_sampled_function_smoother(const mygsl::tabulated_function & tf_,
                                                             const double b_)
    : _tf_(tf_)
  {
    _ks_ = new gauss_kernel_smoother(b_);
    _ks_free_needed_ = true;
    return;
  }

  nw_sampled_function_smoother::~nw_sampled_function_smoother()
  {
    if (_ks_free_needed_ and _ks_ != nullptr) {
      delete _ks_;
      _ks_ = nullptr;
    }
    return;
  }

  double nw_sampled_function_smoother::_eval(double x_) const
  {
    double x0 = x_;
    double sd = 0.0;
    double sn = 0.0;
    DT_THROW_IF(_tf_.size() < 1, std::logic_error,
                "Tabulated function has no points!");
    const mygsl::tabulated_function::points_map_type & pm = _tf_.points();
    for (mygsl::tabulated_function::points_map_type::const_iterator i = pm.begin();
         i != pm.end();
         i++) {
      double xi = i->first;
      double fi = i->second;
      double ki = _ks_->eval(x0, xi);
      sn += fi * ki;
      sd += ki;
    }
    return sn / sd;
  }

} // end of namespace mygsl
