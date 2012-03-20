// mygsl::ode.cc
 
#include <mygsl/ode.h> 

#include <gsl/gsl_errno.h>

namespace mygsl {

  using namespace std;

  bool ode_system::g_debug = false;
 
  int ode_system::gsl_ode_function (double         t_,
                                    const double * y_,
                                    double *       f_,
                                    void   *       params_)
  {
    if (ode_system::g_debug) {
      std::cerr << "DEBUG: gsl_ode_function: " 
                << " entering..." << std::endl;
    }
    if (ode_system::g_debug) {
      std::cerr << "DEBUG: ode_system::gsl_ode_function: test..." 
                << std::endl;
    }
    ode_system * sys = static_cast<ode_system *> (params_);
    if (sys == 0) {
      throw std::runtime_error ("mygsl::ode_system::gsl_ode_function: NULL!");
    }
    return sys->compute_derivatives (t_,y_,f_);
  }

  int ode_system::gsl_ode_jacobian (double         t_,
                                    const double * y_,
                                    double *       dfdy_,
                                    double *       dfdt_,
                                    void   *       params_)
  {
    if (ode_system::g_debug) {
      std::cerr << "DEBUG: gsl_ode_jacobian: " 
                << " entering..." << std::endl;
    }
    ode_system * sys = static_cast<ode_system *> (params_);
    if (sys == 0) {
      throw std::runtime_error ("mygsl::ode_system::gsl_ode_function: NULL!");
    }
    return sys->compute_jacobian (t_,y_,dfdy_,dfdt_);
  }

  int ode_system::compute_jacobian (double         t_   , 
                                    const double * y_   ,
                                    double  *      dfdy_,
                                    double  *      dfdt_ ) 
  {
    return GSL_EINVAL;
  }

  /*********************************************************************/

  const std::string ode_driver::DEFAULT_TYPE = "rk8pd";

  bool ode_driver::g_debug = false;

  ode_driver::default_step_action ode_driver::_default_step_action_;

  bool ode_driver::is_regular () const 
  {
    return _regular_;
  }

  void ode_driver::regular ()
  {
    _regular_=true; 
    return;
  }

  void ode_driver::not_regular ()
  {
    _regular_=false; 
    return;
  }

  void ode_driver::set_default_step_action ()
  {
    _at_step_action_ = &ode_driver::_default_step_action_;
    return;
  }

  void ode_driver::unset_step_action ()
  {
    _at_step_action_ = 0;
    return;
  }

  void ode_driver::set_step_action (at_step_action & asd_)
  {
    _at_step_action_ = &asd_;
    return;
  }

  void ode_driver::print_types (std::ostream & out_)
  {
    out_ << "rk2" << ' ';
    out_ << "rk4" << ' ';
    out_ << "rkf45" << ' ';
    out_ << "rkck" << ' ';
    out_ << "rk8pd" << ' ';
    out_ << "rk2imp" << ' ';
    out_ << "rk4imp" << ' ';
    out_ << "bsimp" << ' ';
    out_ << "gear1" << ' ';
    out_ << "gear2";
    return;
  }

  bool ode_driver::type_is_valid (const std::string & type_)
  {
    if (type_ == "rk2") return true;
    if (type_ == "rk4") return true;
    if (type_ == "rkf45") return true;
    if (type_ == "rkck") return true;
    if (type_ == "rk8pd") return true;
    if (type_ == "rk2imp") return true;
    if (type_ == "rk4imp") return true;
    if (type_ == "bsimp") return true;
    if (type_ == "gear1") return true;
    if (type_ == "gear2") return true; 
    return false;
  }

  bool ode_driver::type_requires_jacobian (const std::string & type_)
  {
    if (type_ == "bsimp") return true;
    return false;
  }

  const std::string & ode_driver::get_type () const 
  {
    return _type_;
  }
 
  void ode_driver::set_type (const std::string & type_) 
  {
    if (! type_is_valid (type_)) {
      throw std::runtime_error ("mygsl::ode_driver::set_type: Invalid type!");
    } 
    if (ode_driver::g_debug) {
      std::cerr << "DEBUG: ode_driver::set_type: type='" 
                << type_ << "'" << std::endl;
    } 
    _type_ = type_;
    return;
  }

  size_t ode_driver::get_dimension () const
  {
    return _system_.dimension;
  }

  ode_driver::ode_driver (ode_system & sys_, 
                          const std::string & type_,
                          double epsabs_, 
                          double epsrel_,
                          bool   regular_)
  {
    if (ode_driver::g_debug) {
      std::cerr << "DEBUG: ctor: " << " entering..." << std::endl;
    }
    _ode_sys_ = 0;
    if (sys_.get_dimension () < 1) {
      throw std::runtime_error ("mygsl::ode_driver::set_dimension: Invalid value!");
    }
    _ode_sys_ = &sys_;

    _regular_ = regular_;

    set_type (type_);

    _epsabs_    = epsabs_;
    _epsrel_    = epsrel_;
    _step_type_ = 0;
    _step_      = 0;
    _control_   = 0;
    _evolve_    = 0;

    _system_.function  = ode_system::gsl_ode_function;
    _has_jacobian_ = false; 
    _system_.jacobian  = 0;
    if (_ode_sys_->has_jacobian ()) {
      _has_jacobian_     = true; 
      _system_.jacobian  = ode_system::gsl_ode_jacobian;
    }
    _system_.dimension = _ode_sys_->get_dimension ();
    _system_.params    = _ode_sys_;

    _at_step_action_   = 0;

    _t_ = 0.0;
    _y_ = 0;
    _h_ = 0.0;

    init ();
    if (ode_driver::g_debug) {
      std::cerr << "DEBUG: ctor: " << " exiting." << std::endl;
    }
    return;
  }

  void ode_driver::reset ()
  {
    _epsabs_    = 1.e-6;
    _epsrel_    = 1.e-6;
    _ode_sys_   = 0;
    _step_type_ = 0;
    if (_evolve_) {
      gsl_odeiv_evolve_free (_evolve_);
      _evolve_=0;
    } 
    if (_control_) {
      gsl_odeiv_control_free (_control_);
      _control_=0;
    } 
    if (_step_) {
      gsl_odeiv_step_free (_step_);
      _step_=0;
    } 
    return;      
  }

  ode_driver::~ode_driver ()
  {
    reset ();
    return;
  }

  void ode_driver::_init_step_ ()
  {
    if (_type_ == "rk2") {
      _step_type_ = gsl_odeiv_step_rk2;
    }
    if (_type_ == "rk4") {
      _step_type_ = gsl_odeiv_step_rk4;
    }
    if (_type_ == "rkf45") {
      _step_type_ = gsl_odeiv_step_rkf45;
    }
    if (_type_ == "rkck") {
      _step_type_ = gsl_odeiv_step_rkck;
    }
    if (_type_ == "rk8pd") {
      _step_type_ = gsl_odeiv_step_rk8pd;
    }
    if (_type_ == "rk2imp") {
      _step_type_ = gsl_odeiv_step_rk2imp;
    }
    if (_type_ == "rk4imp") {
      _step_type_ = gsl_odeiv_step_rk4imp;
    }
    if (_type_ == "bsimp") {
      _step_type_ = gsl_odeiv_step_bsimp;
    }
    if (_type_ == "gear1") {
      _step_type_ = gsl_odeiv_step_gear1;
    }
    if (_type_ == "gear2") {
      _step_type_ = gsl_odeiv_step_gear2;
    }

    if (type_requires_jacobian (_type_) &&  ! _has_jacobian_) {
      std::ostringstream message;
      message << "ode_driver::_init_step_: " 
              << "ODE stepper '" << _type_ 
              << "' requires Jacobian but ODE system does not provide one!";
      throw std::runtime_error (message.str ());
    }

    if (ode_driver::g_debug) {
      std::cerr << "DEBUG: _init_step_: " 
                << "_step_type_=" << _step_type_ << std::endl;
    }
    return;
  }

  void ode_driver::init ()
  {
    if (ode_driver::g_debug) {
      std::cerr << "DEBUG: init: " << " entering..." << std::endl;
    }
    _init_step_ ();
    _step_    = gsl_odeiv_step_alloc (_step_type_,_system_.dimension);
    _control_ = gsl_odeiv_control_y_new (_epsabs_,_epsrel_);
    _evolve_  = gsl_odeiv_evolve_alloc (_system_.dimension);
    if (ode_driver::g_debug) {
      std::cerr << "DEBUG: init: " << " exiting..." << std::endl;
    }
    return;
  }

  int ode_driver::_inner_run_not_regular_ ()
  {
    if (ode_driver::g_debug) {
      std::cerr << "DEBUG: _inner_run_not_regular_: " 
                << "entering..." << std::endl;
    }
    int ret = GSL_SUCCESS;
    while (_t_ < _t_end_) {
      if (ode_driver::g_debug) {
        std::cerr << "DEBUG: _inner_run_not_regular_: " 
                  << "_t_begin_=" << _t_begin_ << std::endl;
        std::cerr << "DEBUG: _inner_run_not_regular_: " 
                  << "_t_=" << _t_ << std::endl;
        std::cerr << "DEBUG: _inner_run_not_regular_: " 
                  << "_t_end_=" << _t_end_ << std::endl;
        std::cerr << "DEBUG: _inner_run_not_regular_: " 
                  << "_evolve_=" << _evolve_ << std::endl;
        std::cerr << "DEBUG: _inner_run_not_regular_: " 
                  << "_control_=" << _control_ << std::endl;
        std::cerr << "DEBUG: _inner_run_not_regular_: " 
                  << "_step_=" << _step_ << std::endl;
        std::cerr << "DEBUG: _inner_run_not_regular_: " 
                  << "_y_=" << _y_ << std::endl;
      }

      int status = gsl_odeiv_evolve_apply (
                                           _evolve_,
                                           _control_,
                                           _step_,
                                           &_system_,
                                           &_t_,
                                           _t_end_,
                                           &_h_,
                                           _y_);

      if (ode_driver::g_debug) {
        std::cerr << "DEBUG: _inner_run_not_regular_: " 
                  << "status=" << status << std::endl;
      }

      if (status != GSL_SUCCESS) {
        ret = status;
        break;
      }

      _at_step_hook ();

    }
    if (ode_driver::g_debug) {
      std::cerr << "DEBUG: _inner_run_not_regular_: " 
                << "exiting." << std::endl;
    }
    return ret;
  }

  int ode_driver::_inner_run_regular_ ()
  {
    int ret = GSL_SUCCESS;
    double h = _h_;
    size_t n_steps = (size_t) ( (_t_end_ - _t_begin_) / h);

    for (size_t istep=0; istep<n_steps; istep++) {
      double ti = istep*h;
      while (_t_ < ti) {
        int status = gsl_odeiv_evolve_apply (
                                             _evolve_,
                                             _control_,
                                             _step_,
                                             &_system_,
                                             &_t_,
                                             ti,
                                             &_h_,
                                             _y_);
          
        /*
          if (status != GSL_SUCCESS) {
          ret = status;
          break;
          }
        */
          
          
      }

      _at_step_hook ();

    }
    return ret;
  }

  void ode_driver::_at_step_hook ()
  {
    if (_at_step_action_ != 0) {
      (*_at_step_action_) (_t_,_y_,_system_.dimension);
    }
    return;
  }

  void ode_driver::_init_run_ ()
  {
    _y_ = new double[_system_.dimension];
    return;
  }

  void ode_driver::_done_run_ ()
  {
    if (_y_ != 0) {
      delete[] _y_;
      _y_=0;
    }
    return;
  }

  int ode_driver::run (double t_begin_, 
                       double t_end_, 
                       double h_)
  {
    if (ode_driver::g_debug) {
      std::cerr << "DEBUG: run: " << " entering..." << std::endl;
    }
    int ret   = GSL_SUCCESS;
    _t_begin_ = t_begin_;
    _t_end_   = t_end_;
    _h_       = h_;

    _init_run_ ();

    if (ode_driver::g_debug) {
      std::cerr << "DEBUG: run: " << " _init_run_ done." << std::endl;
    }

    // setup working array:
    _ode_sys_->to_double_star (_y_,_system_.dimension);

    if (ode_driver::g_debug) {
      std::cerr << "DEBUG: run: " << " to_double_star done." << std::endl;
    }
    _t_       = _t_begin_;
    if (_regular_) ret = _inner_run_regular_ ();
    else ret = _inner_run_not_regular_ ();
    if (ode_driver::g_debug) {
      std::cerr << "DEBUG: run: " << " _inner_run_*_regular_ done." << std::endl;
    }

    // fetch system's data from working array:
    _ode_sys_->from_double_star (_y_,_system_.dimension);
      
    if (ode_driver::g_debug) {
      std::cerr << "DEBUG: run: " << " from_double_star done." << std::endl;
    }
    _done_run_ ();
    if (ode_driver::g_debug) {
      std::cerr << "DEBUG: run: " << " exiting." << std::endl;
    }
    return ret;
  }


  void ode_driver::default_step_action::action (double t_, 
                                                double * y_, 
                                                size_t dim_) 
  {
    double   t = t_;
    double * y = y_;
    std::cout << t << ' ';
    for (int i=0; i<dim_ ; i++) {
      std::cout << y[i] << ' ';
    } 
    std::cout << std::endl;
    return;
  }

}

// end of mygsl::ode.cc
