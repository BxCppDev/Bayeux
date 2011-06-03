// mygsl::unary_eval_with_derivative.cc

#include <mygsl/unary_eval_with_derivative.h>

#include <sstream>
#include <stdexcept>
#include <functional>
#include <limits>

#include <mygsl/numerical_differentiation.h>

namespace mygsl {
  
  unary_eval_with_derivative::unary_eval_with_derivative ()
    : unary_eval ()
  {
    return;
  }
  
  unary_eval_with_derivative::unary_eval_with_derivative (const interval & domain_)
    : unary_eval (domain_)
  {
    return;
  }
  
  double unary_eval_with_derivative::eval (double x_) const
  {
    return eval_f (x_);
  }

  double unary_eval_with_derivative::eval_df (double x_) const
  {
    return eval_df_numeric (x_);
  }

  void unary_eval_with_derivative::eval_fdf (double x_, double & f_, double & df_) const
  {
    f_ = this->eval_f (x_);
    df_ = this->eval_df (x_);
    return;
  }

  double unary_eval_with_derivative::eval_df_numeric (double x_) const
  {
    double h = get_domain().get_eps ();
    double df = numeric_limits<double>::quiet_NaN ();
    double df_abserr = 0.0;
    if (! is_valid (x_))
      {
	ostringstream message;
	message << "unary_eval_with_derivative::eval: "
		<< "value '" << x_ << "' is out of the evaluation domain!";
	throw runtime_error (message.str ());
      }
    if (get_domain ().is_in_safe (x_))
      {
	df = derivative_central (*this, x_, h, df_abserr);
      }
    else if (get_domain ().is_in_min_unsafe (x_))
      {
	df = derivative_forward (*this, x_, h, df_abserr);
      }
    else if (get_domain ().is_in_max_unsafe (x_))
      {
	df = derivative_backward (*this, x_, h, df_abserr);
      }
    return df;
  }

  /************************************************/

  unary_eval_promoted_with_numeric_derivative::unary_eval_promoted_with_numeric_derivative (const unary_eval & eval_)
    : unary_eval_with_derivative ()
  {
    __eval = &eval_;
  } 

  bool unary_eval_promoted_with_numeric_derivative::is_valid (double x_) const
  {
    //clog << "DEVEL: unary_eval_promoted_with_numeric_derivative::is_valid: " << endl;
    return __eval->is_valid (x_);
  }

  double unary_eval_promoted_with_numeric_derivative::eval_f (double x_) const 
  {
    //clog << "DEVEL: unary_eval_promoted_with_numeric_derivative::eval_f: " << endl;
    return (*__eval) (x_);
  }

}

// end of mygsl::unary_eval_with_derivative.cc
