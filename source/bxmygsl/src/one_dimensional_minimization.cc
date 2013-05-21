// one_dimensional_minimization.cc

#include <mygsl/one_dimensional_minimization.h>

#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>

#include <datatools/exception.h>
#include <datatools/logger.h>

namespace mygsl {

  using namespace std;

  const string one_dimensional_minimization::GOLDENSECTION_METHOD_LABEL = "goldensection";
  const string one_dimensional_minimization::BRENT_METHOD_LABEL = "brent";

  const size_t one_dimensional_minimization::DEFAULT_MAX_ITER   = 1000;
  const double one_dimensional_minimization::DEFAULT_EPSABS     = 1.e-3;


  /**********************************************************/
  void one_dimensional_minimization::at_step_action::operator () (int status_,
                                                                  size_t iter_,
                                                                  double a_,
                                                                  double b_,
                                                                  double c_)
  {
    action (status_, iter_, a_, b_, c_);
    return;
  }

  /**********************************************************/

  one_dimensional_minimization::default_step_action one_dimensional_minimization::_default_step_action_;

  const best_value & one_dimensional_minimization::get_minimum_value () const
  {
    return _minimum_value_;
  }

  string one_dimensional_minimization::get_name () const
  {
    if (_fminimizer_ == 0) {
      return string ("");
    }
    return string (gsl_min_fminimizer_name (_fminimizer_));
  }

  void one_dimensional_minimization::set_debug (bool debug_)
  {
    _debug_ = debug_;
  }

  bool one_dimensional_minimization::is_debug () const
  {
    return _debug_;
  }

  size_t one_dimensional_minimization::get_iter () const
  {
    return _iter_;
  }

  size_t one_dimensional_minimization::get_max_iter () const
  {
    return _max_iter_;
  }

  double one_dimensional_minimization::get_epsabs () const
  {
    return _epsabs_;
  }

  bool one_dimensional_minimization::is_converged () const
  {
    return _converged_;
  }

  void one_dimensional_minimization::set_default_step_action ()
  {
    _at_step_action_ = &one_dimensional_minimization::_default_step_action_;
    return;
  }

  void one_dimensional_minimization::unset_step_action ()
  {
    _at_step_action_ = 0;
    return;
  }

  void one_dimensional_minimization::set_step_action (one_dimensional_minimization::at_step_action & asd_)
  {
    _at_step_action_ = &asd_;
    return;
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
    if (local_debug) {
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
  one_dimensional_minimization::one_dimensional_minimization (bool debug_)
  {
    _debug_ = debug_;
    _fminimizer_type_ = 0;
    _fminimizer_ = 0;
    _status_ = 0;
    _iter_ = 0;
    _max_iter_ = DEFAULT_MAX_ITER;
    _epsabs_ = DEFAULT_EPSABS;
    _functor_ = 0;
    _converged_ = false;
    _minimum_value_.reset ();
    _at_step_action_ = 0;
    return;
  }

  // dtor:
  one_dimensional_minimization::~one_dimensional_minimization ()
  {
    reset ();
    return;
  }

  void one_dimensional_minimization::_at_step_hook (int status_,
                                                    size_t iter_,
                                                    double a_,
                                                    double b_,
                                                    double c_)
  {
    if (_at_step_action_ != 0) {
      if (_debug_) std::clog << "DEBUG: one_dimensional_minimization::_at_step_hook: _at_step_action_..." << std::endl;
      (*_at_step_action_) (status_, iter_, a_, b_, c_);
    }
    return;
  }

  void one_dimensional_minimization::init (const i_unary_function & functor_,
                                           const string & method_)
  {
    initialize(functor_,method_);
  }

  void one_dimensional_minimization::initialize (const i_unary_function & functor_,
                                                 const string & method_)
  {
    _functor_ = &functor_;

    if (method_ == GOLDENSECTION_METHOD_LABEL) {
      _fminimizer_type_ = gsl_min_fminimizer_goldensection;
    } else if (method_ == BRENT_METHOD_LABEL) {
      _fminimizer_type_ = gsl_min_fminimizer_brent;
    } else {
      DT_THROW_IF(true,logic_error, "Method '" << method_ << "' is not valid !");
    }
    if (_debug_) {
      clog << "mygsl::one_dimensional_minimization::initialize: method is '"
           << method_ << "'" << endl;
    }

    _function_.function = g_function;
    _function_.params   = static_cast<void*>(const_cast<i_unary_function*>(_functor_)); //const_cast<void*>(_functor_);
    _fminimizer_ = gsl_min_fminimizer_alloc (_fminimizer_type_);

    return;
  }

  void one_dimensional_minimization::reset ()
  {
    if (_fminimizer_ != 0)  {
      gsl_min_fminimizer_free (_fminimizer_);
      _fminimizer_ = 0;
    }
    _fminimizer_type_ = 0;
    _status_ = 0;
    _iter_ = 0;
    _max_iter_ = DEFAULT_MAX_ITER;
    _epsabs_ = DEFAULT_EPSABS;
    _functor_ = 0;
    _converged_ = false;
    _minimum_value_.reset ();
    return;
  }

  int one_dimensional_minimization::minimize (double a_,
                                              double b_,
                                              double m_,
                                              double epsabs_)
  {
    int status = 0;
    int iter   = 0;
    _minimum_value_.reset ();
    double m = m_;
    double a = a_;
    double b = b_;
    _epsabs_ = epsabs_;
    double epsrel = 0.0;
    if (_debug_) {
      clog << "mygsl::one_dimensional_minimization::minimize: setting function and starting values..." << endl;
    }
    gsl_min_fminimizer_set (_fminimizer_,
                            &_function_,
                            m,
                            a,
                            b);
    if (_debug_) {
      clog << "mygsl::one_dimensional_minimization::minimize: starting iterations..." << endl;
    }
    do {
      iter++;
      status = gsl_min_fminimizer_iterate (_fminimizer_);
      m = gsl_min_fminimizer_x_minimum (_fminimizer_);
      a = gsl_min_fminimizer_x_lower (_fminimizer_);
      b = gsl_min_fminimizer_x_upper (_fminimizer_);

      status = gsl_min_test_interval (a, b, _epsabs_, epsrel);

      if ((status == GSL_SUCCESS) || (iter == _max_iter_)) {
        if (status == GSL_SUCCESS) {
          _converged_ = true;
          _minimum_value_.set_value (m);
          _minimum_value_.set_error_low (m - a);
          _minimum_value_.set_error_high (b - m);
          _minimum_value_.set_confidence_level (1.0);
        } else {
          _converged_ = false;
          _minimum_value_.set_value (m);
          _minimum_value_.set_error_low (m - a_);
          _minimum_value_.set_error_high (b_ - m);
          _minimum_value_.set_confidence_level (0.0);
        }
      }
      _at_step_hook (status, iter, a, b, m);
    }
    while ((status == GSL_CONTINUE) && (iter < _max_iter_));
    _iter_ = iter;
    _status_ = status;
    return _status_;
  }

  double one_dimensional_minimization::g_function (double x_,
                                                   void * params_)
  {
    const i_unary_function * func = static_cast<const i_unary_function *>(params_);
    double fx = func->eval (x_);
    return fx;
  }

  // static:
  best_value one_dimensional_minimization::minimize (const i_unary_function & func_,
                                                     double a_,
                                                     double b_,
                                                     double m_,
                                                     double epsabs_,
                                                     const string & method_name_)
  {
    mygsl::one_dimensional_minimization minimizer;
    minimizer.init (func_, method_name_);
    if (minimizer.minimize (a_, b_, m_, epsabs_) == 0) {
        return minimizer.get_minimum_value ();
      }
    best_value no_root;
    no_root.reset ();
    return no_root;
  }

}

// end of one_dimensional_minimization.cc
