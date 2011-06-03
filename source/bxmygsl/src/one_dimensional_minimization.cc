// mygsl::one_dimensional_minimization.cc

#include <mygsl/one_dimensional_minimization.h>

#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>

namespace mygsl {
  
  const string one_dimensional_minimization::GOLDENSECTION_METHOD_LABEL = "goldensection";
  const string one_dimensional_minimization::BRENT_METHOD_LABEL = "brent";

  const size_t one_dimensional_minimization::DEFAULT_MAX_ITER   = 1000;
  const double one_dimensional_minimization::DEFAULT_EPSABS     = 1.e-3;
  

  /**********************************************************/

  one_dimensional_minimization::default_step_action one_dimensional_minimization::__default_step_action;

  void one_dimensional_minimization::set_default_step_action ()
  {
    __at_step_action = &one_dimensional_minimization::__default_step_action;
  }

  void one_dimensional_minimization::unset_step_action ()
  {
    __at_step_action = 0;
  }

  void one_dimensional_minimization::set_step_action (one_dimensional_minimization::at_step_action & asd_)
  {
    __at_step_action = &asd_;
  }
  
  void one_dimensional_minimization::default_step_action::action (int status_,
								 size_t iter_, 
								 double a_, 
								 double b_,
								 double c_)
  {
    bool   local_debug = false;
    int    status = status_;
    size_t iter   = iter_;
    double a = a_;
    double b = b_;
    double c = c_;
    if (local_debug) 
      {
	std::cerr << "DEBUG: Iteration: " << iter << " (" 
		  << ((status == GSL_SUCCESS)? "minimum found": "continue") 
		  << ')' << std::endl;
      }
    std::cout << iter << ' ';
    std::cout.precision (15);
    std::cout << a << ' ';
    std::cout << b << ' ';
    std::cout << c << ' ' << status << ' ';
    std::cout << std::endl;
  }

  /**********************************************************/

  // ctor:
  one_dimensional_minimization::one_dimensional_minimization (bool debug_)
  {
    __debug = debug_;
    __fminimizer_type = 0;
    __fminimizer = 0;
    __status = 0;
    __iter = 0;
    __max_iter = DEFAULT_MAX_ITER;
    __epsabs = DEFAULT_EPSABS;
    __eval = 0;
    __converged = false;
    __minimum_value.reset ();
    __at_step_action = 0;
  }

  // dtor:
  one_dimensional_minimization::~one_dimensional_minimization ()
  {
    reset ();
  }

  void one_dimensional_minimization::_at_step_hook (int status_,
						   size_t iter_, 
						   double a_, 
						   double b_,
						   double c_)
  {
    if (__debug) std::clog << "DEBUG: one_dimensional_minimization::_at_step_hook: entering..." << std::endl;
    if (__at_step_action != 0) 
      {
	if (__debug) std::clog << "DEBUG: one_dimensional_minimization::_at_step_hook: __at_step_action..." << std::endl;
	(*__at_step_action) (status_, iter_, a_, b_, c_);
      }
    if (__debug) std::clog << "DEBUG: one_dimensional_minimization::_at_step_hook: exiting." << std::endl;
  }

  void one_dimensional_minimization::init (unary_eval & eval_,
					   const string & method_)
  {
    if (__debug) 
      {
	clog << "one_dimensional_minimization::init: entering..." << endl;
      }
    __eval = &eval_;

    if (method_ == GOLDENSECTION_METHOD_LABEL)
      {
	__fminimizer_type = gsl_min_fminimizer_goldensection;
      }
    else if (method_ == BRENT_METHOD_LABEL)
      {
	__fminimizer_type = gsl_min_fminimizer_brent;
      }
    else 
      {
	ostringstream message;
	message << "one_dimensional_minimization::init: "
		<< "method '" << method_ << "' is not valid!";
	throw runtime_error (message.str ());
      }
    if (__debug) 
      {
	clog << "one_dimensional_minimization::init: method is '"
	     << method_ << "'" << endl;
      }

    __function.function = g_function;
    __function.params   = __eval;
    __fminimizer = gsl_min_fminimizer_alloc (__fminimizer_type);

    if (__debug) 
      {
	clog << "one_dimensional_minimization::init: exiting." << endl;
      }
  }

  void one_dimensional_minimization::reset ()
  {
    if (__debug) 
      {
	clog << "one_dimensional_minimization::reset: entering..." << endl;
      }
    if (__fminimizer != 0) 
      {
	gsl_min_fminimizer_free (__fminimizer);
	__fminimizer = 0;
      }
    __fminimizer_type = 0;
    __status = 0;
    __iter = 0;
    __max_iter = DEFAULT_MAX_ITER;
    __epsabs = DEFAULT_EPSABS;
    __eval = 0;
    __converged = false;
    __minimum_value.reset ();
    if (__debug) 
      {
	clog << "one_dimensional_minimization::reset: exiting." << endl;
      }
  }

  int one_dimensional_minimization::minimize (double a_,
					      double b_,
					      double m_,
					      double epsabs_)
  {
    if (__debug) 
      {
	clog << "one_dimensional_minimization::minimize: entering..." << endl;
      }
    int status = 0;
    int iter   = 0;
    __minimum_value.reset ();
    double m = m_;
    double a = a_;
    double b = b_;
    __epsabs = epsabs_;
    double epsrel = 0.0;
    if (__debug) 
      {
	clog << "one_dimensional_minimization::minimize: setting function and starting values..." << endl;
      }
    gsl_min_fminimizer_set (__fminimizer, 
			    &__function,
			    m,
			    a,
			    b);
    if (__debug) 
      {
	clog << "one_dimensional_minimization::minimize: starting iterations..." << endl;
      }
    do
      {
	iter++;
	status = gsl_min_fminimizer_iterate (__fminimizer);
	m = gsl_min_fminimizer_x_minimum (__fminimizer);
	a = gsl_min_fminimizer_x_lower (__fminimizer);
	b = gsl_min_fminimizer_x_upper (__fminimizer);

	status = gsl_min_test_interval (a, b, __epsabs, epsrel);

	if ((status == GSL_SUCCESS) || (iter == __max_iter))
	  {
	    if (status == GSL_SUCCESS)
	      {
		__converged = true;
		__minimum_value.set_value (m);
		__minimum_value.set_error_low (m - a);
		__minimum_value.set_error_high (b - m);
		__minimum_value.set_confidence_level (1.0);
	      }
	    else
	      {
		__converged = false;
		__minimum_value.set_value (m);
		__minimum_value.set_error_low (m - a_);
		__minimum_value.set_error_high (b_ - m);
		__minimum_value.set_confidence_level (0.0);
	      }
	  }
	_at_step_hook (status, iter, a, b, m);
      }
    while ((status == GSL_CONTINUE) && (iter < __max_iter));
    __iter = iter;
    __status = status;
    if (__debug) 
      {
	clog << "one_dimensional_minimization::minimize: exiting." << endl;
      }
    return __status;
  }

  double one_dimensional_minimization::g_function (double x_, 
						   void * params_)
  {
    const unary_eval * func = static_cast<const unary_eval *> (params_);
    double fx = func->evaluate (x_);
    return fx;
  }

  // static:
  best_value one_dimensional_minimization::minimize (unary_eval & func_, 
						     double a_, 
						     double b_,
						     double m_,
						     double epsabs_, 
						     const string & method_name_)
  {
    mygsl::one_dimensional_minimization minimizer;
    minimizer.init (func_, method_name_);
    if (minimizer.minimize (a_, b_, m_, epsabs_) == 0)
      {
	return minimizer.get_minimum_value ();
      }
    best_value no_root;
    no_root.reset ();
    return no_root;
  }


}

// end of mygsl::one_dimensional_minimization.cc
