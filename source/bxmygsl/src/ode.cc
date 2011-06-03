// mygsl::ode.cc
 
#include <mygsl/ode.h> 

#include <gsl/gsl_errno.h>

namespace mygsl {

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

  ode_driver::default_step_action ode_driver::__default_step_action;

  bool ode_driver::is_regular () const 
  {
    return __regular;
  }

  void ode_driver::regular ()
  {
    __regular=true; 
    return;
  }

  void ode_driver::not_regular ()
  {
    __regular=false; 
    return;
  }

  void ode_driver::set_default_step_action ()
  {
    __at_step_action = &ode_driver::__default_step_action;
    return;
  }

  void ode_driver::unset_step_action ()
  {
    __at_step_action = 0;
    return;
  }

  void ode_driver::set_step_action (at_step_action & asd_)
  {
    __at_step_action = &asd_;
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
    return __type;
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
    __type = type_;
    return;
  }

  size_t ode_driver::get_dimension () const
  {
    return __system.dimension;
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
    __ode_sys = 0;
    if (sys_.get_dimension () < 1) {
      throw std::runtime_error ("mygsl::ode_driver::set_dimension: Invalid value!");
    }
    __ode_sys = &sys_;

    __regular = regular_;

    set_type (type_);

    __epsabs    = epsabs_;
    __epsrel    = epsrel_;
    __step_type = 0;
    __step      = 0;
    __control   = 0;
    __evolve    = 0;

    __system.function  = ode_system::gsl_ode_function;
    __has_jacobian = false; 
    __system.jacobian  = 0;
    if (__ode_sys->has_jacobian ()) {
      __has_jacobian     = true; 
      __system.jacobian  = ode_system::gsl_ode_jacobian;
    }
    __system.dimension = __ode_sys->get_dimension ();
    __system.params    = __ode_sys;

    __at_step_action   = 0;

    __t = 0.0;
    __y = 0;
    __h = 0.0;

    init ();
    if (ode_driver::g_debug) {
      std::cerr << "DEBUG: ctor: " << " exiting." << std::endl;
    }
    return;
  }

  void ode_driver::reset ()
  {
    __epsabs    = 1.e-6;
    __epsrel    = 1.e-6;
    __ode_sys   = 0;
    __step_type = 0;
    if (__evolve) {
      gsl_odeiv_evolve_free (__evolve);
      __evolve=0;
    } 
    if (__control) {
      gsl_odeiv_control_free (__control);
      __control=0;
    } 
    if (__step) {
      gsl_odeiv_step_free (__step);
      __step=0;
    } 
    return;      
  }

  ode_driver::~ode_driver ()
  {
    reset ();
    return;
  }

  void ode_driver::__init_step ()
  {
    if (__type == "rk2") {
      __step_type = gsl_odeiv_step_rk2;
    }
    if (__type == "rk4") {
      __step_type = gsl_odeiv_step_rk4;
    }
    if (__type == "rkf45") {
      __step_type = gsl_odeiv_step_rkf45;
    }
    if (__type == "rkck") {
      __step_type = gsl_odeiv_step_rkck;
    }
    if (__type == "rk8pd") {
      __step_type = gsl_odeiv_step_rk8pd;
    }
    if (__type == "rk2imp") {
      __step_type = gsl_odeiv_step_rk2imp;
    }
    if (__type == "rk4imp") {
      __step_type = gsl_odeiv_step_rk4imp;
    }
    if (__type == "bsimp") {
      __step_type = gsl_odeiv_step_bsimp;
    }
    if (__type == "gear1") {
      __step_type = gsl_odeiv_step_gear1;
    }
    if (__type == "gear2") {
      __step_type = gsl_odeiv_step_gear2;
    }

    if (type_requires_jacobian (__type) &&  ! __has_jacobian) {
      std::ostringstream message;
      message << "ode_driver::__init_step: " 
	      << "ODE stepper '" << __type 
	      << "' requires Jacobian but ODE system does not provide one!";
      throw std::runtime_error (message.str ());
    }

    if (ode_driver::g_debug) {
      std::cerr << "DEBUG: __init_step: " 
		<< "__step_type=" << __step_type << std::endl;
    }
    return;
  }

  void ode_driver::init ()
  {
    if (ode_driver::g_debug) {
      std::cerr << "DEBUG: init: " << " entering..." << std::endl;
    }
    __init_step ();
    __step    = gsl_odeiv_step_alloc (__step_type,__system.dimension);
    __control = gsl_odeiv_control_y_new (__epsabs,__epsrel);
    __evolve  = gsl_odeiv_evolve_alloc (__system.dimension);
    if (ode_driver::g_debug) {
      std::cerr << "DEBUG: init: " << " exiting..." << std::endl;
    }
    return;
  }

  int ode_driver::__inner_run_not_regular ()
  {
    if (ode_driver::g_debug) {
      std::cerr << "DEBUG: __inner_run_not_regular: " 
		<< "entering..." << std::endl;
    }
    int ret = GSL_SUCCESS;
    while (__t < __t_end) {
      if (ode_driver::g_debug) {
	std::cerr << "DEBUG: __inner_run_not_regular: " 
		  << "__t_begin=" << __t_begin << std::endl;
	std::cerr << "DEBUG: __inner_run_not_regular: " 
		  << "__t=" << __t << std::endl;
	std::cerr << "DEBUG: __inner_run_not_regular: " 
		  << "__t_end=" << __t_end << std::endl;
	std::cerr << "DEBUG: __inner_run_not_regular: " 
		  << "__evolve=" << __evolve << std::endl;
	std::cerr << "DEBUG: __inner_run_not_regular: " 
		  << "__control=" << __control << std::endl;
	std::cerr << "DEBUG: __inner_run_not_regular: " 
		  << "__step=" << __step << std::endl;
	std::cerr << "DEBUG: __inner_run_not_regular: " 
		  << "__y=" << __y << std::endl;
      }

      int status = gsl_odeiv_evolve_apply (
					  __evolve,
					  __control,
					  __step,
					  &__system,
					  &__t,
					  __t_end,
					  &__h,
					  __y);

      if (ode_driver::g_debug) {
	std::cerr << "DEBUG: __inner_run_not_regular: " 
		  << "status=" << status << std::endl;
      }

      if (status != GSL_SUCCESS) {
	ret = status;
	break;
      }

      _at_step_hook ();

    }
    if (ode_driver::g_debug) {
      std::cerr << "DEBUG: __inner_run_not_regular: " 
		<< "exiting." << std::endl;
    }
    return ret;
  }

  int ode_driver::__inner_run_regular ()
  {
    int ret = GSL_SUCCESS;
    double h = __h;
    size_t n_steps = (size_t) ( (__t_end - __t_begin) / h);

    for (size_t istep=0; istep<n_steps; istep++) {
      double ti = istep*h;
      while (__t < ti) {
	int status = gsl_odeiv_evolve_apply (
					    __evolve,
					    __control,
					    __step,
					    &__system,
					    &__t,
					    ti,
					    &__h,
					    __y);
	  
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
    if (__at_step_action != 0) {
      (*__at_step_action) (__t,__y,__system.dimension);
    }
    return;
  }

  void ode_driver::__init_run ()
  {
    __y = new double[__system.dimension];
    return;
  }

  void ode_driver::__done_run ()
  {
    if (__y != 0) {
      delete[] __y;
      __y=0;
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
    __t_begin = t_begin_;
    __t_end   = t_end_;
    __h       = h_;

    __init_run ();

    if (ode_driver::g_debug) {
      std::cerr << "DEBUG: run: " << " __init_run done." << std::endl;
    }

    // setup working array:
    __ode_sys->to_double_star (__y,__system.dimension);

    if (ode_driver::g_debug) {
      std::cerr << "DEBUG: run: " << " to_double_star done." << std::endl;
    }
    __t       = __t_begin;
    if (__regular) ret = __inner_run_regular ();
    else ret = __inner_run_not_regular ();
    if (ode_driver::g_debug) {
      std::cerr << "DEBUG: run: " << " __inner_run_*_regular done." << std::endl;
    }

    // fetch system's data from working array:
    __ode_sys->from_double_star (__y,__system.dimension);
      
    if (ode_driver::g_debug) {
      std::cerr << "DEBUG: run: " << " from_double_star done." << std::endl;
    }
    __done_run ();
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
