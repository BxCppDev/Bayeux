// mygsl::one_dimensional_root_finding.cc 

#include <mygsl/one_dimensional_root_finding.h>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cmath>

#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>

namespace mygsl {
  
  const string one_dimensional_root_solver::BISECTION_METHOD_LABEL  = "bisection";
  const string one_dimensional_root_solver::FALSEPOS_METHOD_LABEL   = "falsepos";
  const string one_dimensional_root_solver::BRENT_METHOD_LABEL      = "brent";
  const string one_dimensional_root_solver::NEWTON_METHOD_LABEL     = "newton";
  const string one_dimensional_root_solver::SECANT_METHOD_LABEL     = "secant";
  const string one_dimensional_root_solver::STEFFENSON_METHOD_LABEL = "steffenson";

  const size_t one_dimensional_root_solver::DEFAULT_MAX_ITER        = 1000;
  const double one_dimensional_root_solver::DEFAULT_EPSABS          = 1.e-3;


  /**********************************************************/

  one_dimensional_root_solver::default_step_action one_dimensional_root_solver::__default_step_action;

  void one_dimensional_root_solver::set_default_step_action ()
  {
    __at_step_action = &one_dimensional_root_solver::__default_step_action;
  }

  void one_dimensional_root_solver::unset_step_action ()
  {
    __at_step_action = 0;
  }

  void one_dimensional_root_solver::set_step_action (one_dimensional_root_solver::at_step_action & asd_)
  {
    __at_step_action = &asd_;
  }
  
  void one_dimensional_root_solver::default_step_action::action (int status_,
								 size_t iter_, 
								 double a_, 
								 double b_,
								 double c_)
  {
    bool local_debug = false;
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
  one_dimensional_root_solver::one_dimensional_root_solver (bool debug_)
  {
    __debug = debug_;
    __mode = MODE_NULL;
    __fsolver_type = 0;
    __fsolver = 0;
    __fdfsolver = 0;
    __eval_f   = 0;
    __eval_fdf = 0;
    __eval_numeric_fdf = 0;
    __status = 0;
    __iter = 0;
    __max_iter = DEFAULT_MAX_ITER;
    __epsabs = DEFAULT_EPSABS;
    __converged = false;
    __root.reset ();
    __at_step_action = 0;
  }
  
  // dtor:
  one_dimensional_root_solver::~one_dimensional_root_solver ()
  {
    reset ();
  }

  void one_dimensional_root_solver::_at_step_hook (int status_,
						   size_t iter_, 
						   double a_, 
						   double b_,
						   double c_)
  {
    if (__debug) std::clog << "DEBUG: one_dimensional_root_solver::_at_step_hook: entering..." << std::endl;
    if (__at_step_action != 0) 
      {
	if (__debug) std::clog << "DEBUG: one_dimensional_root_solver::_at_step_hook: __at_step_action..." << std::endl;
	(*__at_step_action) (status_, iter_, a_, b_, c_);
      }
    if (__debug) std::clog << "DEBUG: one_dimensional_root_solver::_at_step_hook: exiting." << std::endl;
  }

  void one_dimensional_root_solver::init (unary_eval & eval_f_, const string & method_) 
  {
    if  (__debug)
      {
	clog << "DEBUG: one_dimensional_root_solver::init(1): entering..." << endl;
      }
    if (is_initialized ())
      {
	reset ();
      }
    __eval_numeric_fdf = new unary_eval_promoted_with_numeric_derivative (eval_f_);
    init (*__eval_numeric_fdf, method_);

    if (__debug) 
      {
	clog << "one_dimensional_root_solver::init(1): exiting." << endl;
      }
  }

  void one_dimensional_root_solver::init (unary_eval_with_derivative & eval_fdf_, const string & method_)
  {
    if (is_initialized ())
      {
	reset ();
      }
    if (__debug) 
      {
	clog << "DEBUG: one_dimensional_root_solver::init(2): entering..." << endl;
      }

    if (method_ == BISECTION_METHOD_LABEL)
      {
	__mode = MODE_F;
	__fsolver_type = gsl_root_fsolver_bisection;
      }
    else if (method_ == FALSEPOS_METHOD_LABEL)
      {
	__mode = MODE_F;
	__fsolver_type = gsl_root_fsolver_falsepos;
      }
    else if (method_ == BRENT_METHOD_LABEL)
      {
	__mode = MODE_F;
	__fsolver_type = gsl_root_fsolver_brent;
      }
    else if (method_ == NEWTON_METHOD_LABEL)
      {
	__mode = MODE_FDF;
	__fdfsolver_type = gsl_root_fdfsolver_newton;
      }
    else if (method_ == SECANT_METHOD_LABEL)
      {
	__mode = MODE_FDF;
	__fdfsolver_type = gsl_root_fdfsolver_secant;
      }
    else if (method_ == STEFFENSON_METHOD_LABEL)
      {
	__mode = MODE_FDF;
	__fdfsolver_type = gsl_root_fdfsolver_steffenson;
      }
    else 
      {
	ostringstream message;
	message << "one_dimensional_root_solver::init(2): "
		<< "method '" << method_ << "' is not valid!";
	throw runtime_error (message.str ());
      }

    if (__debug) 
      {
	clog << "one_dimensional_root_solver::init(2): method is '"
	     << method_ << "'" << endl;
      }

    __eval_fdf = &eval_fdf_;

    // set functions:
    if (__mode == MODE_F)
      {
	__fsolver = gsl_root_fsolver_alloc (__fsolver_type);
	__function.function = g_function;
	__function.params   = __eval_fdf;
      }
    else
      {
	__fdfsolver = gsl_root_fdfsolver_alloc (__fdfsolver_type);
	__fdfunction.f   = g_function;
	__fdfunction.df  = g_dfunction;
	__fdfunction.fdf = g_fdfunction;
	__fdfunction.params     = __eval_fdf;
      }

    if (__debug) 
      {
	clog << "one_dimensional_root_solver::init(2): exiting." << endl;
      }
  }
  
  void one_dimensional_root_solver::reset ()
  {
    if (__debug) 
      {
	clog << "one_dimensional_root_solver::reset: entering..." << endl;
      }
    if (__fsolver != 0)
      {
	gsl_root_fsolver_free (__fsolver);
	__fsolver = 0;
	__fsolver_type = 0;
      }
    if (__fdfsolver != 0)
      {
	gsl_root_fdfsolver_free (__fdfsolver);
	__fdfsolver_type = 0;
	__fdfsolver = 0;
      }
    if (__eval_numeric_fdf != 0)
      {
	delete __eval_numeric_fdf;
	__eval_numeric_fdf = 0;
      }
    __status = 0;
    __iter = 0;
    __max_iter = DEFAULT_MAX_ITER;
    __epsabs = DEFAULT_EPSABS;
    __eval_f   = 0;
    __eval_fdf = 0;
    __converged = false;
    __root.reset ();
    if (__debug) 
      {
	clog << "one_dimensional_root_solver::reset: exiting." << endl;
      }

  }
  
  int one_dimensional_root_solver::solve (double epsabs_, 
					  double a_,
					  double b_)
  {
    if (__debug) 
      {
	clog << "one_dimensional_root_solver::solve: entering..." << endl;
      }
    int status = 0;
    int iter   = 0;
    __root.reset ();
    double r, x_lo, x_hi; // MODE_F
    double x, x0;         // MODE_FDF
    __epsabs = epsabs_;
    double epsrel = 0.0;
    if (__debug) 
      {
	clog << "one_dimensional_root_solver::solve: setting function and starting values..." << endl;
      }
    if ((a_ != a_) || (b_ != b_))
      {
	throw runtime_error ("one_dimensional_root_solver::solve: Invalid starting interval!");
      }
    if (a_ >= b_)
      {
	throw runtime_error ("one_dimensional_root_solver::solve: Invalid starting interval!");
      }
    double x_min = a_;
    double x_max = b_;
    //clog << "DEVEL: a = " << x_min << endl;
    //clog << "DEVEL: b = " << x_max << endl;

    if (__mode == MODE_F)
      {
	x_lo = x_min;
	x_hi = x_max;
	gsl_root_fsolver_set (__fsolver, 
			      &__function,
			      x_lo,
			      x_hi);
      }
    else
      {
	x = 0.5 * (x_min + x_max);
	gsl_root_fdfsolver_set (__fdfsolver, 
				&__fdfunction,
				x);
      }  

    double factor = 1.0;
    do
      {
	iter++;
	if (__mode == MODE_F)
	  {
	    status = gsl_root_fsolver_iterate (__fsolver);
	    r = gsl_root_fsolver_root (__fsolver);
	    if ((r < x_min) || (x > x_max))
	      {
		status = GSL_ERANGE;
	      }
	    else 
	      {
		x_lo = gsl_root_fsolver_x_lower (__fsolver);
		x_hi = gsl_root_fsolver_x_upper (__fsolver);
		status = gsl_root_test_interval (x_lo, x_hi, 0, factor * __epsabs);
	      }
	    _at_step_hook (status, iter, x_lo, x_hi, r);
	  }
	else
	  {
	    status = gsl_root_fdfsolver_iterate (__fdfsolver);
	    x0 = x;
	    x = gsl_root_fdfsolver_root (__fdfsolver);
	    if ((x < x_min) || (x > x_max))
	      {
		status = GSL_ERANGE;
	      }
	    else
	      {
		status = gsl_root_test_delta (x, x0, 0, factor * __epsabs);
	      }
	    _at_step_hook (status, iter, x_min, x_max, x);
	  }

	if ((status == GSL_SUCCESS) || (iter == __max_iter))
	  {
	    __converged = (status == GSL_SUCCESS);
	    if (__mode == MODE_F)
	      {
		__root.set_value (0.5 * (x_lo + x_hi));
		__root.set_error_low (abs (x_lo - x_hi));
		__root.set_error_high (abs (x_lo - x_hi));
	      }
	    else
	      {
		__root.set_value (0.5 * (x + x0));
		__root.set_error_low (0.5 * abs (x - x0));
		__root.set_error_high (0.5 * abs (x - x0));
	      }
	    __root.set_confidence_level (1.0);
	  }


      }
    while ((status == GSL_CONTINUE) && (iter < __max_iter));

    __iter = iter;
    __status = status;

    if (__debug) 
      {
	clog << "one_dimensional_root_solver::solve: exiting." << endl;
      }
    return __status;
  }
  
  double one_dimensional_root_solver::g_function (double x_, void * params_)
  {
    unary_eval_with_derivative * eval_fdf = static_cast<unary_eval_with_derivative *> (params_);
    return eval_fdf->eval_f (x_);
  }

  double one_dimensional_root_solver::g_dfunction (double x_, void * params_)
  {
    unary_eval_with_derivative * eval_fdf = static_cast<unary_eval_with_derivative *> (params_);
    return eval_fdf->eval_df (x_);
  }

  void one_dimensional_root_solver::g_fdfunction (double x_, void * params_, double * y_, double * dy_)
  {
    unary_eval_with_derivative * eval_fdf = static_cast<unary_eval_with_derivative *> (params_);
    *y_  = eval_fdf->eval_f (x_);
    *dy_ = eval_fdf->eval_df (x_);
  }

  // static:
  best_value one_dimensional_root_solver::solve (unary_eval & func_, 
					     double epsabs_, 
					     double a_, 
					     double b_,
					     const string & method_name_)
  {
    mygsl::one_dimensional_root_solver solver;
    solver.init (func_, method_name_);
    if (solver.solve (epsabs_, a_, b_) == 0)
      {
	return solver.get_root ();
      }
    best_value no_root;
    no_root.reset ();
    return no_root;
  }

  // static:
  best_value one_dimensional_root_solver::solve (unary_eval_with_derivative & dfunc_, 
					     double epsabs_, 
					     double a_, 
					     double b_, 
					     const string & method_name_)
  {
    mygsl::one_dimensional_root_solver solver;
    solver.init (dfunc_, method_name_);
    if (solver.solve (epsabs_, a_, b_) == 0)
      {
	return solver.get_root ();
      }
    best_value no_root;
    no_root.reset ();
    return no_root;
  }
  
}

// end of mygsl::one_dimensional_root_finding.cc
