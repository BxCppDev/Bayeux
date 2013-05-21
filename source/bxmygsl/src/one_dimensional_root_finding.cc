// one_dimensional_root_finding.cc

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

#include <datatools/exception.h>


namespace mygsl {

  using namespace std;

  const string one_dimensional_root_solver::BISECTION_METHOD_LABEL  = "bisection";
  const string one_dimensional_root_solver::FALSEPOS_METHOD_LABEL   = "falsepos";
  const string one_dimensional_root_solver::BRENT_METHOD_LABEL      = "brent";
  const string one_dimensional_root_solver::NEWTON_METHOD_LABEL     = "newton";
  const string one_dimensional_root_solver::SECANT_METHOD_LABEL     = "secant";
  const string one_dimensional_root_solver::STEFFENSON_METHOD_LABEL = "steffenson";

  const size_t one_dimensional_root_solver::DEFAULT_MAX_ITER        = 1000;
  const double one_dimensional_root_solver::DEFAULT_EPSABS          = 1.e-3;

  /**********************************************************/

  void one_dimensional_root_solver::at_step_action::operator () (int status_,
                                                                 size_t iter_,
                                                                 double a_,
                                                                 double b_,
                                                                 double c_)
  {
    action (status_, iter_, a_, b_, c_);
  }

  /**********************************************************/

  one_dimensional_root_solver::default_step_action one_dimensional_root_solver::_default_step_action_;

  bool one_dimensional_root_solver::is_fsolver () const
  {
    return _fsolver_ != 0;
  }

  bool one_dimensional_root_solver::is_fdfsolver () const
  {
    return _fdfsolver_ != 0;
  }

  bool one_dimensional_root_solver::is_initialized () const
  {
    return is_fsolver () || is_fdfsolver ();
  }

  const best_value & one_dimensional_root_solver::get_root () const
  {
    return _root_;
  }

  string one_dimensional_root_solver::get_name () const
  {
    if (_fsolver_ != 0)
      {
        return string (gsl_root_fsolver_name (_fsolver_));
      }
    if (_fdfsolver_ != 0)
      {
        return string (gsl_root_fdfsolver_name (_fdfsolver_));
      }
    return string ("");
  }

  void one_dimensional_root_solver::set_debug (bool debug_)
  {
    _debug_ = debug_;
    return;
  }

  bool one_dimensional_root_solver::is_debug () const
  {
    return _debug_;
  }

  size_t one_dimensional_root_solver::get_iter () const
  {
    return _iter_;
  }

  size_t one_dimensional_root_solver::get_max_iter () const
  {
    return _max_iter_;
  }

  double one_dimensional_root_solver::get_epsabs () const
  {
    return _epsabs_;
  }

  bool one_dimensional_root_solver::is_converged () const
  {
    return _converged_;
  }

  void one_dimensional_root_solver::set_default_step_action ()
  {
    _at_step_action_ = &one_dimensional_root_solver::_default_step_action_;
    return;
  }

  void one_dimensional_root_solver::unset_step_action ()
  {
    _at_step_action_ = 0;
    return;
  }

  void one_dimensional_root_solver::set_step_action (one_dimensional_root_solver::at_step_action & asd_)
  {
    _at_step_action_ = &asd_;
    return;
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
    return;
  }

  /**********************************************************/

  // ctor:
  one_dimensional_root_solver::one_dimensional_root_solver (bool debug_)
  {
    _debug_ = debug_;
    _mode_ = MODE_NULL;
    _fsolver_type_ = 0;
    _fsolver_ = 0;
    _fdfsolver_ = 0;
    _functor_f_   = 0;
    _functor_fdf_ = 0;
    _functor_numeric_fdf_ = 0;
    _status_ = 0;
    _iter_ = 0;
    _max_iter_ = DEFAULT_MAX_ITER;
    _epsabs_ = DEFAULT_EPSABS;
    _converged_ = false;
    _root_.reset ();
    _at_step_action_ = 0;
    return;
  }

  // dtor:
  one_dimensional_root_solver::~one_dimensional_root_solver ()
  {
    reset ();
    return;
  }

  void one_dimensional_root_solver::_at_step_hook (int status_,
                                                   size_t iter_,
                                                   double a_,
                                                   double b_,
                                                   double c_)
  {
    //if (_debug_) std::clog << "DEBUG: one_dimensional_root_solver::_at_step_hook: entering..." << std::endl;
    if (_at_step_action_ != 0)
      {
        //if (_debug_) std::clog << "DEBUG: one_dimensional_root_solver::_at_step_hook: _at_step_action_..." << std::endl;
        (*_at_step_action_) (status_, iter_, a_, b_, c_);
      }
    //if (_debug_) std::clog << "DEBUG: one_dimensional_root_solver::_at_step_hook: exiting." << std::endl;
    return;
  }

  void one_dimensional_root_solver::init (const i_unary_function & functor_f_, const string & method_)
  {
    if (_debug_) {
      //clog << "DEBUG: one_dimensional_root_solver::init(1): entering..." << endl;
    }
    if (is_initialized ()) {
      reset ();
    }
    _functor_numeric_fdf_ = new unary_function_promoted_with_numeric_derivative (functor_f_);
    init (*_functor_numeric_fdf_, method_);

    if (_debug_) {
      //clog << "one_dimensional_root_solver::init(1): exiting." << endl;
    }
    return;
  }

  void one_dimensional_root_solver::init (const i_unary_function_with_derivative & functor_fdf_, const string & method_)
  {
    if (is_initialized ()) {
      reset ();
    }
    if (_debug_) {
      //clog << "DEBUG: one_dimensional_root_solver::init(2): entering..." << endl;
    }

    if (method_ == BISECTION_METHOD_LABEL) {
      _mode_ = MODE_F;
      _fsolver_type_ = gsl_root_fsolver_bisection;
    } else if (method_ == FALSEPOS_METHOD_LABEL) {
      _mode_ = MODE_F;
      _fsolver_type_ = gsl_root_fsolver_falsepos;
    } else if (method_ == BRENT_METHOD_LABEL) {
      _mode_ = MODE_F;
      _fsolver_type_ = gsl_root_fsolver_brent;
    } else if (method_ == NEWTON_METHOD_LABEL) {
      _mode_ = MODE_FDF;
      _fdfsolver_type_ = gsl_root_fdfsolver_newton;
    } else if (method_ == SECANT_METHOD_LABEL) {
      _mode_ = MODE_FDF;
      _fdfsolver_type_ = gsl_root_fdfsolver_secant;
    } else if (method_ == STEFFENSON_METHOD_LABEL) {
      _mode_ = MODE_FDF;
      _fdfsolver_type_ = gsl_root_fdfsolver_steffenson;
    } else {
      DT_THROW_IF(true,
                  logic_error,
                  "Method '" << method_ << "' is not valid !");
    }

    if (_debug_) {
      //clog << "one_dimensional_root_solver::init(2): method is '"
      //           << method_ << "'" << endl;
    }

    _functor_fdf_ = &functor_fdf_;

    // set functions:
    if (_mode_ == MODE_F) {
      _fsolver_ = gsl_root_fsolver_alloc (_fsolver_type_);
      _function_.function = g_function;
      _function_.params   = static_cast<void*>(const_cast<i_unary_function_with_derivative*>(_functor_fdf_));
    } else {
      _fdfsolver_ = gsl_root_fdfsolver_alloc (_fdfsolver_type_);
      _fdfunction_.f   = g_function;
      _fdfunction_.df  = g_dfunction;
      _fdfunction_.fdf = g_fdfunction;
      _fdfunction_.params = static_cast<void*>(const_cast<i_unary_function_with_derivative*>(_functor_fdf_));
    }

    if (_debug_) {
      //clog << "one_dimensional_root_solver::init(2): exiting." << endl;
    }
    return;
  }

  void one_dimensional_root_solver::reset ()
  {
    if (_debug_) {
      //clog << "one_dimensional_root_solver::reset: entering..." << endl;
    }
    if (_fsolver_ != 0) {
      gsl_root_fsolver_free (_fsolver_);
      _fsolver_ = 0;
      _fsolver_type_ = 0;
    }
    if (_fdfsolver_ != 0) {
      gsl_root_fdfsolver_free (_fdfsolver_);
      _fdfsolver_type_ = 0;
      _fdfsolver_ = 0;
    }
    if (_functor_numeric_fdf_ != 0) {
      delete _functor_numeric_fdf_;
      _functor_numeric_fdf_ = 0;
    }
    _status_ = 0;
    _iter_ = 0;
    _max_iter_ = DEFAULT_MAX_ITER;
    _epsabs_ = DEFAULT_EPSABS;
    _functor_f_   = 0;
    _functor_fdf_ = 0;
    _converged_ = false;
    _root_.reset ();
    if (_debug_) {
      //clog << "one_dimensional_root_solver::reset: exiting." << endl;
    }
    return;

  }

  int one_dimensional_root_solver::solve (double epsabs_,
                                          double a_,
                                          double b_)
  {
    if (_debug_) {
      //clog << "one_dimensional_root_solver::solve: entering..." << endl;
    }
    int status = 0;
    int iter   = 0;
    _root_.reset ();
    double r, x_lo, x_hi; // MODE_F
    double x, x0;         // MODE_FDF
    _epsabs_ = epsabs_;
    double epsrel = 0.0;
    if (_debug_) {
      //clog << "one_dimensional_root_solver::solve: setting function and starting values..." << endl;
    }
    DT_THROW_IF ((a_ != a_) || (b_ != b_), runtime_error, "Invalid starting interval!");
    DT_THROW_IF (a_ >= b_, runtime_error, "Invalid starting interval!");
    double x_min = a_;
    double x_max = b_;
    //clog << "DEVEL: a = " << x_min << endl;
    //clog << "DEVEL: b = " << x_max << endl;

    if (_mode_ == MODE_F) {
      x_lo = x_min;
      x_hi = x_max;
      gsl_root_fsolver_set (_fsolver_,
                            &_function_,
                            x_lo,
                            x_hi);
    } else {
      x = 0.5 * (x_min + x_max);
      gsl_root_fdfsolver_set (_fdfsolver_,
                              &_fdfunction_,
                              x);
    }

    double factor = 1.0;
    do {
      iter++;
      if (_mode_ == MODE_F) {
        status = gsl_root_fsolver_iterate (_fsolver_);
        r = gsl_root_fsolver_root (_fsolver_);
        if ((r < x_min) || (x > x_max)) {
          status = GSL_ERANGE;
        } else {
          x_lo = gsl_root_fsolver_x_lower (_fsolver_);
          x_hi = gsl_root_fsolver_x_upper (_fsolver_);
          status = gsl_root_test_interval (x_lo, x_hi, 0, factor * _epsabs_);
        }
        _at_step_hook (status, iter, x_lo, x_hi, r);
      } else {
        status = gsl_root_fdfsolver_iterate (_fdfsolver_);
        x0 = x;
        x = gsl_root_fdfsolver_root (_fdfsolver_);
        if ((x < x_min) || (x > x_max)) {
          status = GSL_ERANGE;
        } else {
          status = gsl_root_test_delta (x, x0, 0, factor * _epsabs_);
        }
        _at_step_hook (status, iter, x_min, x_max, x);
      }

      if ((status == GSL_SUCCESS) || (iter == _max_iter_)) {
        _converged_ = (status == GSL_SUCCESS);
        if (_mode_ == MODE_F) {
          _root_.set_value (0.5 * (x_lo + x_hi));
          _root_.set_error_low (abs (x_lo - x_hi));
          _root_.set_error_high (abs (x_lo - x_hi));
        } else {
          _root_.set_value (0.5 * (x + x0));
          _root_.set_error_low (0.5 * abs (x - x0));
          _root_.set_error_high (0.5 * abs (x - x0));
        }
        _root_.set_confidence_level (1.0);
      }


    }
    while ((status == GSL_CONTINUE) && (iter < _max_iter_));

    _iter_ = iter;
    _status_ = status;

    if (_debug_) {
      //clog << "one_dimensional_root_solver::solve: exiting." << endl;
    }
    return _status_;
  }

  double one_dimensional_root_solver::g_function (double x_, void * params_)
  {
    const i_unary_function_with_derivative * functor_fdf = static_cast<const i_unary_function_with_derivative *>(params_);
    return functor_fdf->eval_f(x_);
  }

  double one_dimensional_root_solver::g_dfunction (double x_, void * params_)
  {
    const i_unary_function_with_derivative * functor_fdf = static_cast<const i_unary_function_with_derivative *>(params_);
    return functor_fdf->eval_df(x_);
  }

  void one_dimensional_root_solver::g_fdfunction (double x_, void * params_, double * y_, double * dy_)
  {
    const i_unary_function_with_derivative * functor_fdf = static_cast<const i_unary_function_with_derivative *>(params_);
    *y_  = functor_fdf->eval_f(x_);
    *dy_ = functor_fdf->eval_df(x_);
    return;
  }

  // static:
  best_value one_dimensional_root_solver::solve (const i_unary_function & func_,
                                                 double epsabs_,
                                                 double a_,
                                                 double b_,
                                                 const string & method_name_)
  {
    mygsl::one_dimensional_root_solver solver;
    solver.init (func_, method_name_);
    if (solver.solve (epsabs_, a_, b_) == 0) {
      return solver.get_root ();
    }
    best_value no_root;
    no_root.reset ();
    return no_root;
  }

  // static:
  best_value one_dimensional_root_solver::solve (const i_unary_function_with_derivative & dfunc_,
                                                 double epsabs_,
                                                 double a_,
                                                 double b_,
                                                 const string & method_name_)
  {
    mygsl::one_dimensional_root_solver solver;
    solver.init (dfunc_, method_name_);
    if (solver.solve (epsabs_, a_, b_) == 0) {
      return solver.get_root ();
    }
    best_value no_root;
    no_root.reset ();
    return no_root;
  }

} // namespace mygsl

// end of one_dimensional_root_finding.cc
