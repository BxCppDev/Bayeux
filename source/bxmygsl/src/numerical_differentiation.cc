// mygsl::numerical_differentiation.cc

#include <mygsl/numerical_differentiation.h>

namespace mygsl {

  double derivative_central (const unary_eval & eval_, double x_, double h_, double & abserr_ )
  {
    double result, abserr;
    gsl_function func;
    func.function = unary_eval::g_function;
    func.params   = const_cast<unary_eval *>(&eval_);
    if (gsl_deriv_central (&func, x_, h_, &result, &abserr) != 0)
      {
	throw runtime_error ("derivative_central: cannot derivate function!");
      }
    abserr_ = abserr;
    return result;
  }

  double derivative_central (const unary_eval & eval_, double x_, double h_)
  {
    double abserr;
    derivative_central (eval_, x_, h_, abserr);
  }


  double derivative_forward (const unary_eval & eval_, double x_, double h_, double & abserr_ )
  {
    double result, abserr;
    gsl_function func;
    func.function = unary_eval::g_function;
    func.params   = const_cast<unary_eval *>(&eval_);
    if (gsl_deriv_forward (&func, x_, h_, &result, &abserr) != 0)
      {
	throw runtime_error ("derivative_forward: cannot derivate function!");
      }
    abserr_ = abserr;
    return result;
  }

  double derivative_forward (const unary_eval & eval_, double x_, double h_)
  {
    double abserr;
    derivative_forward (eval_, x_, h_, abserr);
  }

  double derivative_backward (const unary_eval & eval_, double x_, double h_, double & abserr_ )
  {
    double result, abserr;
    gsl_function func;
    func.function = unary_eval::g_function;
    func.params   = const_cast<unary_eval *>(&eval_);
    if (gsl_deriv_forward (&func, x_, h_, &result, &abserr) != 0)
      {
	throw runtime_error ("derivative_backward: cannot derivate function!");
      }
    abserr_ = abserr;
    return result;
  }

  double derivative_backward (const unary_eval & eval_, double x_, double h_)
  {
    double abserr;
    derivative_backward (eval_, x_, h_, abserr);
  }

  double derivative (const unary_eval & eval_, 
		     double x_, 
		     const interval & domain_, 
		     double & abserr_)
  {
    double h = domain_.get_eps ();
    if (domain_.is_in_safe (x_))
      {
	return derivative_central (eval_, x_, h, abserr_);
      }
    else if (domain_.is_in_min_unsafe (x_))
      {
	return derivative_forward (eval_, x_, h, abserr_);
      }
    else if (domain_.is_in_max_unsafe (x_))
      {
	return derivative_backward (eval_, x_, h, abserr_);
      }
    else
      {
	throw runtime_error ("derivative_backward: cannot derivate function!");
      }
    return 0.0;
  }

  double derivative (const unary_eval & eval_, 
		     double x_, 
		     const interval & domain_)
  {
    double abserr;
    derivative (eval_, x_, domain_, abserr);
  }

}

// end of mygsl::numerical_differentiation.cc
