// mygsl::multidimensional_minimization.cc

#include <mygsl/multidimensional_minimization.h>

namespace mygsl {

  const size_t multidimensional_minimization::DEFAULT_MAX_ITER    = 1000;
  const size_t multidimensional_minimization::DEFAULT_MODULO_ITER = 100;
  const bool   multidimensional_minimization::DEFAULT_VERBOSE  = true;
  const double multidimensional_minimization::DEFAULT_EPSABS   = 1.e-3;
  bool         multidimensional_minimization::g_debug          = false;

  const double multidimensional_minimization_system::DEFAULT_OUT_OF_LIMIT_SLOPE = 1.e10;

  /************************************************************************/

  void multidimensional_minimization_system::set_numeric_eval_df (bool use_)
  {
    __numeric_eval_df = use_;
  }

  bool multidimensional_minimization_system::is_numeric_eval_df () const
  {
    return __numeric_eval_df;
  }

  multidimensional_minimization_system::multidimensional_minimization_system (double slope_,
									      bool use_numeric_eval_)
  {
    __out_of_limit_slope = slope_;
    if (slope_ <= 0)
      {
	__out_of_limit_slope = DEFAULT_OUT_OF_LIMIT_SLOPE;
      }
    __numeric_eval_df = use_numeric_eval_;
  }

  multidimensional_minimization_system::~multidimensional_minimization_system ()
  {
  }

  double multidimensional_minimization_system::func_eval_f_MR  (double x_, void * params_)
  {
    bool local_devel = multidimensional_minimization::g_debug;
    //if (local_devel) clog << "DEVEL: func_eval_f: entering... " << endl;
    func_eval_f_param * fefp = static_cast<func_eval_f_param *>(params_);
    int free_param_index = fefp->free_param_index;
    multidimensional_minimization_system * sys = fefp->sys;

    // backup former 'x' value:
    double save_x = sys->get_free_param (free_param_index).get_value ();

    // enforce 'x' value:
    sys->get_free_param (free_param_index).set_value_no_check (x_);
    sys->init_params_values ();

    double result = numeric_limits<double>::quiet_NaN ();
    if (sys->__eval_f_MR (result) != 0)
      {
	throw runtime_error ("multidimensional_minimization_system::func_eval_f: Cannot evaluate function!");
      }

    // restore former 'x' value:
    sys->get_free_param (free_param_index).set_value_no_check (save_x);
    sys->init_params_values ();
    //if (local_devel) clog << "DEVEL: func_eval_f: exiting." << endl;
    return result;
  }

  int multidimensional_minimization_system::__eval_f_MR (double & f_)
  {
    bool local_debug = multidimensional_minimization::g_debug;
    int code = _eval_f (f_);
    if (code != 0)
      {
	return code;
      }

    // search for 'out of limits' free parameters: 
    double slope = __out_of_limit_slope;
    bool dump = false;
    for (int i = 0; i < get_number_of_free_params (); i++) 
      {
	double dist = get_free_param (i).get_dist_to_limit ();
	double step = get_free_param (i).get_step ();
	// add arbitrary huge value:
	double df = slope * dist / step;
	if (local_debug)
	  {
	    if (df != 0.0)
	      {
		clog << "DEVEL: multidimensional_minimization_system::__eval_f_MR: out of limits == " << get_free_param (i).get_name () << " : Delta(f) == " << df << endl;
		dump = true; //print_status (clog);
	      }
	  }
	f_ += df;
      }

    // search for 'out of limits' auto parameters: 
    for (int i = 0; i < get_number_of_auto_params (); i++) 
      {
	double dist = get_auto_param (i).get_dist_to_limit ();
	double step = get_auto_param (i).get_step ();
	// add arbitrary huge value:
	double df = slope * dist / step;
	if (local_debug)
	  {
	    if (df != 0.0)
	      {
		clog << "DEVEL: multidimensional_minimization_system::__eval_f_MR: out of limits == " << get_auto_param (i).get_name () << " : Delta(f) == " << df << endl;
		dump = true; //print_status (clog);
	      }
	  }
	f_ += df;
      }
    if (dump) print_status (clog);
    return code;
  }

  int multidimensional_minimization_system::__eval_df_MR (double * gradient_)
  {
    int code = _eval_df (gradient_);
    if (code != 0)
      {
	return code;
      }

    // search for 'out of limits' free parameters: 
    double slope = __out_of_limit_slope;
    for (int i = 0; i < get_number_of_free_params (); i++) 
      {
	double sign = get_free_param (i).get_sign_limit ();
	// add arbitrary derivative value:
	gradient_[i] += sign * __out_of_limit_slope;
      }

    // search for 'out of limits' auto parameters: 
    for (int i = 0; i < get_number_of_auto_params (); i++) 
      {
	double sign = get_auto_param (i).get_sign_limit ();
	// add arbitrary derivative value:
	gradient_[i] += sign * __out_of_limit_slope;
      }

    return code;
  }

  int multidimensional_minimization_system::__numerical_eval_df_MR (double * gradient_)
  {
    // clog << "DEVEL: MMS::__numerical_eval_df_MR" << endl;
    bool local_devel = multidimensional_minimization::g_debug;
    if (local_devel) clog << "DEVEL: __numerical_eval_df: entering..." << endl;  
    for (int i = 0; i < get_number_of_free_params (); i++) 
      {
	double save_value = get_free_param (i).get_value ();
	double x = save_value;
	double h = get_free_param (i).get_step ();
	double df = 0.0;
	double df_err = 0.0;
	int res_deriv = GSL_SUCCESS;
	gsl_function func;
	func_eval_f_param fefp (i, *this);
	func.params = &fefp;
	func.function = multidimensional_minimization_system::func_eval_f_MR;
	if (get_free_param (i).is_in_safe_range ())
	  {
	    if (local_devel) clog << "DEVEL: __numerical_eval_df: gsl_deriv_central... " << endl;
	    res_deriv = gsl_deriv_central (&func, x, h, &df, &df_err);
	    // clog << "DEVEL: MMS::__numerical_eval_df_MR: gsl_deriv_central" << endl;
	  }
	else if (get_free_param (i).is_in_range_but_close_to_min ())
	  {
	    if (local_devel) clog << "DEVEL: __numerical_eval_df: gsl_deriv_forward... " << endl;
	    res_deriv = gsl_deriv_forward (&func, x, h, &df, &df_err);
	    // clog << "DEVEL: MMS::__numerical_eval_df_MR: gsl_deriv_forward" << endl;
	  }
	else if (get_free_param (i).is_in_range_but_close_to_max ())
	  {
	    if (local_devel) clog << "DEVEL: __numerical_eval_df: gsl_deriv_backward... " << endl;
	    res_deriv = gsl_deriv_backward (&func, x, h, &df, &df_err);
	    // clog << "DEVEL: MMS::__numerical_eval_df_MR: gsl_deriv_backward" << endl;
	  }
	else 
	  {
	    df = 0.0;
	  }
	gradient_[i] = df;
      }
    if (local_devel) clog << "DEVEL: __numerical_eval_df: exiting." << endl;  
    return 0;
  }

  int multidimensional_minimization_system::_eval_df (double * gradient_)
  {
    cerr << "ERROR: The 'multidimensional_minimization_system::_eval_df' method " << endl;
    cerr << "       must be overloaded for your own class!" << endl;
    cerr << "       If not, you should consider to activate the " << endl;
    cerr << "       'use_numeric_eval_df(true)' method in order to perform" << endl;
    cerr << "       automatically the numerical estimation of the " << endl;
    cerr << "       derivatives!" << endl;
    return 1;
  }
 
  double multidimensional_minimization_system::get_out_of_limit_slope () const
  {
    return __out_of_limit_slope;
  }

  int multidimensional_minimization_system::eval_f (const double * x_, 
						    double & f_)
  {
    // clog << "DEVEL: MMS::eval_f" << endl;
    from_double_star (x_, get_number_of_free_params ());
    int code = __eval_f_MR (f_);
    return code;
  }

  int multidimensional_minimization_system::eval_df (const double * x_, 
						     double * gradient_)
  {
    // clog << "DEVEL: MMS::eval_df" << endl;
    from_double_star (x_, get_number_of_free_params ());
    int code = 0;
    if (! __numeric_eval_df)
      {
	code = __eval_df_MR (gradient_);
      }
    else
      {
	code = multidimensional_minimization_system::__numerical_eval_df_MR (gradient_);
      }
    return code;
  }

  int multidimensional_minimization_system::eval_fdf (const double * x_, 
						      double & f_, 
						      double * gradient_)
  {
    // clog << "DEVEL: MMS::eval_fdf" << endl;
    int code = eval_f (x_, f_);
    if (code != 0) 
      {
	return code;
      }
    return eval_df (x_, gradient_);
  }


  int multidimensional_minimization_system::init_params_values ()
  {
    //lock_params ();
    multiparameter_system::init ();
    return 0;
  }
  
  void multidimensional_minimization_system::to_double_star (double * pars_, 
							     size_t dimension_) const
  {
    size_t fd = get_number_of_free_params ();
    if (dimension_ != fd) 
      {
	throw std::range_error ("multidimensional_minimization_system::to_double_star: Invalid dimension!");
      }
    int i_free = 0;
    for (int i = 0; i < fd; i++) 
      { 
	pars_[i] = get_free_param (i).get_value ();
      }
  }

  void multidimensional_minimization_system::from_double_star (const double * pars_, 
							       size_t dimension_)
  {
    if (dimension_ != get_number_of_free_params ()) 
      {
	throw std::range_error ("multidimensional_minimization_system::from_double_star: Invalid dimension!");
      }
    for (int i = 0; i < get_number_of_free_params (); i++) 
      { 
	get_free_param (i).set_value_no_check (pars_[i]);
      }
    init_params_values ();
  }

  /*
  void multidimensional_minimization_system::search_likelihood_sigma ()
  {
    for (int param_index = param_begin; 
	 param_index < param_end; 
	 param_index++)
      {
	const mygsl::param_entry & param = get_free_param (param_index);
	double h  = param.get_step ();
	mygsl::multidimensional_minimization_system::func_eval_f_param func;
	func.free_param_index = param_index;
	func.sys              = const_cast<mygsl::multidimensional_minimization_system*>(this);
	double sigma_low;
	double sigma_high;
	double p = param.get_value ();
	double fmin = mygsl::multidimensional_minimization_system::func_eval_f_MR (p, &func);
	double h = param.get_step ();
	double dir = +1;
	while (true)
	  { 
	    p += (dir * h);
	    double f 
	      = mygsl::multidimensional_minimization_system::func_eval_f_MR (p, &func);
	    if (f > (fmin + 0.5))
	      {
		dir = -1;
		h /= 20;
	      }
	  }
	while (
      }
  }
  */

  void multidimensional_minimization_system::plot_f (const string & prefix_, int mode_) const
  {
    string prefix = prefix_;
    string ext = ".data";
    if (prefix.empty ())
      {
	prefix = "mms_param_";
      }
    int param_begin = 0;
    int param_end = get_number_of_free_params ();
    for (int param_index = param_begin; 
	 param_index < param_end; 
	 param_index++)
      {
	const mygsl::param_entry & param = get_free_param (param_index);
	clog << "NOTICE: multidimensional_minimization_system::plot_f: " 
	     << "Plot function to be minimized for parameter '" 
	     << param.get_name () 
	     << "' in the current parameters' region: " << endl;
	ostringstream fname; 
	fname << prefix  
	      << param.get_name () << ext;
	ofstream f_param (fname.str ().c_str ());
	int count = 0;
	double h  = param.get_step ();
	double h2 = h / 50.;
	double dt = h;
	double min = param.get_min ();
	double max = param.get_max ();
	for (double p = (min - 5.5 * h);
	     p < (max + 5.5 * h);
	     p += dt)
	  {
	    if (p < (min - 1.5 * h))
	      {
		dt = h;
	      } 
	    else if (p < (min + h))
	      {
		dt = h2;
	      } 
	    else if (p < (max - 1.5 * h )) 
	      {
		dt = h;
	      }
	    else if (p < (max + h )) 
	      {
		dt = h2;
	      }
	    else
	      {
		dt = h;
	      }
	    mygsl::multidimensional_minimization_system::func_eval_f_param 
	      func (param_index, const_cast<mygsl::multidimensional_minimization_system&>(*this));
	    //func.free_param_index = param_index;
	    //func.sys              = const_cast<mygsl::multidimensional_minimization_system*>(this);
	    double f 
	      = mygsl::multidimensional_minimization_system::func_eval_f_MR (p, &func);
	    f_param.precision (14);
	    f_param << p << ' ' << f << endl;
	    //if ((count % 100) == 0) clog << "  Count #" << count << endl;
	    count++;
	  }
	f_param.close ();
	clog << "NOTICE: multidimensional_minimization_system::plot_f: " 
	     << "done." << endl;
      }
    return;
  }
 
  /**************************************************************************/
 
  double multidimensional_minimization::f (const gsl_vector * v_, 
					   void * params_)
  {
    multidimensional_minimization_system * mms 
      = static_cast<multidimensional_minimization_system*> (params_);
    double f;
    mms->eval_f (v_->data, f);
    return f;
  }
  
  void multidimensional_minimization::df (const gsl_vector * v_, 
					  void * params_, 
					  gsl_vector * gradient_)
  {
    multidimensional_minimization_system * mms 
      = static_cast<multidimensional_minimization_system*> (params_);
    mms->eval_df (v_->data, gradient_->data);
  }

  void multidimensional_minimization::fdf (const gsl_vector * v_, 
					   void * params_, 
					   double * f_, 
					   gsl_vector * gradient_)
  {
    multidimensional_minimization_system * mms 
      = static_cast<multidimensional_minimization_system*> (params_);
    mms->eval_fdf (v_->data, *f_, gradient_->data);
  }

  /****************************************************************************/

  multidimensional_minimization::default_step_action multidimensional_minimization::__default_step_action;

  void multidimensional_minimization::set_default_step_action ()
  {
    __at_step_action = &multidimensional_minimization::__default_step_action;
  }

  void multidimensional_minimization::unset_step_action ()
  {
    __at_step_action = 0;
  }

  void multidimensional_minimization::set_step_action (multidimensional_minimization::at_step_action & asd_)
  {
    __at_step_action = &asd_;
  }
  
  void multidimensional_minimization::default_step_action::action (int status_, 
								   size_t iter_,
								   double * x_, 
								   size_t   dim_,
								   double   f_)
  {
    int    status = status_;
    size_t iter   = iter_;
    double * x    = x_;
    size_t dim    = dim_;
    double   f    = f_;
    if (g_debug) 
      {
	std::cerr << "DEBUG: Iteration: " << iter << " (" 
		  << ((status == GSL_SUCCESS)? "minimum found": "continue") 
		  << ')' << std::endl;
      }
    std::cout << iter << ' ' << dim << ' ';
    for (int i = 0; i < dim ; i++) 
      {
	std::cout.precision (15);
	std::cout << x[i] << ' ';
      } 
    std::cout << f << ' ' << status << ' ';
    std::cout << std::endl;
  }

  /**********************************************************/

  void multidimensional_minimization::print_types (std::ostream & out_)
  {
    out_ << "conjugate_fr" << ' ';
    out_ << "conjugate_pr" << ' ';
    out_ << "vector_bfgs" << ' ';
    out_ << "steepest_descent" << ' ';
    out_ << "nmsimplex";
  }

  bool multidimensional_minimization::name_is_valid (const std::string & name_)
  {
    if (name_ == "conjugate_fr") return true;
    if (name_ == "conjugate_pr") return true;
    if (name_ == "vector_bfgs" ) return true;
    if (name_ == "steepest_descent") return true;
    if (name_ == "nmsimplex"  ) return true;
    return false;
  }

  std::string multidimensional_minimization::get_name () const
  {
    if (__mode == MODE_FDF && __fdfmin != 0 ) 
      {
	return std::string (gsl_multimin_fdfminimizer_name (__fdfmin));
      }
    if (__mode == MODE_F && __fmin != 0) 
      {
	return std::string (gsl_multimin_fminimizer_name (__fmin));
      }
    return "";
  }

  void multidimensional_minimization::__init_algorithm (const std::string & name_) 
  {
    if (! name_is_valid (name_)) 
      {
	std::ostringstream message;
	message << "multidimensional_minimization::__init_algorithm: "
		<< "Invalid minimization algorithm '" 
		<< name_ << "'!";
	throw std::runtime_error (message.str ());
      }
    
    if (name_ == "conjugate_fr") 
      {
	__algo_fdf = gsl_multimin_fdfminimizer_conjugate_fr;
	__set_mode (MODE_FDF);
      }
    
    if (name_ == "conjugate_pr") 
      {
	__algo_fdf = gsl_multimin_fdfminimizer_conjugate_pr;
	__set_mode (MODE_FDF);
      }
    
    if (name_ == "vector_bfgs") 
      {
	__algo_fdf = gsl_multimin_fdfminimizer_vector_bfgs;
	__set_mode (MODE_FDF);
      }
    
    if (name_ == "steepest_descent") 
      {
	__algo_fdf = gsl_multimin_fdfminimizer_steepest_descent;
	__set_mode (MODE_FDF);
      }
    
    if (name_ == "nmsimplex") 
      {
	__algo_f = gsl_multimin_fminimizer_nmsimplex;
	__set_mode (MODE_F);
      }
  }

  void multidimensional_minimization::__set_mode (int mode_)
  {
    if (mode_ != MODE_F && mode_ != MODE_FDF) 
      {
	throw std::runtime_error ("multidimensional_minimization:__set_mode: Invalid mode!");
      }
    __mode = mode_;
  }

  void multidimensional_minimization::devel_dump_x () const
  {
    if (__x == 0) 
      {
	std::cerr << "DEVEL: __x==0" << std::endl;
	return;
      }
    std::cerr << "DEVEL ================================= " << std::endl; 
    std::cerr << "DEVEL: " 
	      << " __x.size  = " << __x->size
	      << std::endl;
    std::cerr << "DEVEL: " 
	      << " __x.stride  = " << __x->stride
	      << std::endl;
    std::cerr << "DEVEL: " 
	      << " __x.data  = " << __x->data
	      << std::endl;
    std::cerr << "DEVEL: " 
	      << " __x.owner  = " << __x->owner
	      << std::endl;
    for (int i = 0; i < __x->size; i++) 
      {
	std::cerr << "DEVEL: " 
		  << " __x.data[" << i << "]  = " << __x->data[i]
		  << std::endl;
      }
    std::cerr << "DEVEL: " 
	      << " __x.block.size  = " << __x->block->size
	      << std::endl;
    for (int i = 0; i < __x->block->size; i++) 
      {
	std::cerr << "DEVEL: " 
		  << " __x.block[" << i << "]  = " << __x->block->data[i]
		  << std::endl;
      }
    std::cerr << "DEVEL ================================= " << std::endl; 
  }

  void multidimensional_minimization::init (const std::string & name_, 
					    multidimensional_minimization_system & ms_)
  {
    __sys = &ms_;
    if (! __sys->is_lock_params ()) 
      {
	__sys->lock_params ();
      }
    size_t n = __sys->get_number_of_free_params ();
    __init_algorithm (name_);

    if (__mode == MODE_FDF) 
      {
	__fdf.f      = &multidimensional_minimization::f;
	__fdf.df     = &multidimensional_minimization::df;
	__fdf.fdf    = &multidimensional_minimization::fdf;
	__fdf.n      = n;
	__fdf.params = (void *) __sys;
	__fdfmin     = gsl_multimin_fdfminimizer_alloc (__algo_fdf, __fdf.n);
      }
    
    if (__mode == MODE_F) 
      {
	__f.f      = &multidimensional_minimization::f;
	__f.n      = n;
	__f.params = (void *) __sys;
	__fmin     = gsl_multimin_fminimizer_alloc (__algo_f, __f.n);
      }
    
    __x = gsl_vector_alloc (n);
    gsl_vector_set_zero (__x);
    
    __sys->to_double_star (__x->data, n);

    if (g_debug) 
      {
	for (int i = 0; i < n; i++) 
	  {
	    double par = gsl_vector_get (__x, i);
	    std::clog << "DEBUG: multidimensional_minimization::init: par=" << par << std::endl;
	  }
      }
  
    if (__mode == MODE_FDF) 
      {
	__fdf_step_size = 0.01;
	__fdf_tol       = 1.e-4;
	gsl_multimin_fdfminimizer_set (__fdfmin, 
				       &__fdf, 
				       __x, 
				       __fdf_step_size, 
				       __fdf_tol); 
      }

    if (__mode == MODE_F) 
      {
	__ss = gsl_vector_alloc (n);
	gsl_vector_set_zero (__ss);
	for (int i = 0; i < __sys->get_number_of_free_params (); i++) 
	  {
	    gsl_vector_set (__ss, i, __sys->get_param (i).get_step ());
	  }
	gsl_multimin_fminimizer_set (__fmin, &__f, __x, __ss); 
      }
  }

  void multidimensional_minimization::reset ()
  {
    if (__fdfmin != 0) 
      {
	gsl_multimin_fdfminimizer_free (__fdfmin);
	__fdfmin = 0;
      }
    if (__fmin != 0) 
      {
	gsl_multimin_fminimizer_free (__fmin);
	__fmin = 0;
      }
    if (__x != 0) 
      {
	gsl_vector_free (__x);
	__x = 0;
      }
    if (__ss != 0) 
      {
	gsl_vector_free (__ss);
	__ss = 0;
      }
    __algo_fdf = 0;
    __algo_f   = 0;
    __fdf_step_size = 0.01;
    __fdf_tol       = 1.e-4;

    __f.f      = 0;
    __f.n      = 0;
    __f.params = 0;

    __fdf.f      = 0;
    __fdf.df     = 0;
    __fdf.fdf    = 0;
    __fdf.n      = 0;
    __fdf.params = 0;

    __mode           = MODE_F;
    __stopping       = STOPPING_SIZE;
    __epsabs         = DEFAULT_EPSABS;
    __at_step_action = 0;
    if (__sys != 0) 
      {
	__sys->unlock_params ();
	__sys = 0;
      }
    __n_iter = 0;
    __fval   = 0.0;
  }

  void multidimensional_minimization::set_max_iter (size_t max_)
  {
    __max_iter = max_;
  }

  void multidimensional_minimization::set_modulo_iter (size_t modulo_)
  {
    __modulo_iter = modulo_;
  }

  size_t multidimensional_minimization::get_max_iter () const
  {
    return __max_iter;
  }

  size_t multidimensional_minimization::get_modulo_iter () const
  {
    return __modulo_iter;
  }

  size_t multidimensional_minimization::get_n_iter () const
  {
    return __n_iter;
  }
  
  double multidimensional_minimization::get_fval () const
  {
    return __fval;
  }
  
  // ctor:
  multidimensional_minimization::multidimensional_minimization ()
  {
    __algo_fdf = 0;
    __algo_f   = 0;
    __verbose  = DEFAULT_VERBOSE;
    __mode     = MODE_F;
    __fdfmin   = 0;
    __fmin     = 0;

    __f.f      = 0;
    __f.n      = 0;
    __f.params = 0;
    __fdf.f      = 0;
    __fdf.df     = 0;
    __fdf.fdf    = 0;
    __fdf.n      = 0;
    __fdf.params = 0;

    __x              = 0;
    __ss             = 0;
    __fdf_step_size  = 0.01;
    __fdf_tol        = 1.e-4;
    __max_iter       = DEFAULT_MAX_ITER;
    __stopping       = STOPPING_SIZE;
    __epsabs         = 1.0;
    __at_step_action = 0;

    __n_iter         = 0;
    __modulo_iter    = DEFAULT_MODULO_ITER;	  
  }

  // dtor:
  multidimensional_minimization::~multidimensional_minimization ()
  {
    reset ();
  }

  void multidimensional_minimization::_at_step_hook (int status_, 
						     size_t iter_, 
						     double * x_, 
						     size_t dim_, 
						     double f_)
  {
    if (g_debug) std::clog << "DEBUG: multidimensional_minimization::_at_step_hook: entering..." << std::endl;
    if (__at_step_action != 0) 
      {
	if (g_debug) std::clog << "DEBUG: multidimensional_minimization::_at_step_hook: __at_step_action..." << std::endl;
	(*__at_step_action) (status_, iter_, x_, dim_, f_);
      }
    if (g_debug) std::clog << "DEBUG: multidimensional_minimization::_at_step_hook: exiting." << std::endl;
  }

  int multidimensional_minimization::minimize (double epsabs_)
  {
    // clog << ">>> TEST 100" << endl;
    size_t iter   = 0;
    int    status = 0;
    size_t dim    = 0;
    double * x    = 0;
    __epsabs      = epsabs_;
    if (epsabs_ < 0.0)
      { 
	__epsabs = DEFAULT_EPSABS;
      }
    double f;

    do {
      if (g_debug) std::cerr << std::endl << "DEBUG: multidimensional_minimization::minimize: NEW ITERATTION" << std::endl;

      iter++;

      if (__mode == MODE_F)  
	{
	  if (g_debug) std::cerr << "DEBUG: multidimensional_minimization::minimize: MODE_F" << std::endl;
	  dim = __f.n;
	  status = gsl_multimin_fminimizer_iterate (__fmin);
	  if (status != 0) 
	    {
	      break;
	    }
	  double size = gsl_multimin_fminimizer_size (__fmin);
	  if (g_debug) 
	    {
	      std::cerr << "DEBUG: multidimensional_minimization::minimize: " 
			<< "MODE_F size=" << size 
			<< " epsabs=" << __epsabs << std::endl;
	    }
	  status = gsl_multimin_test_size (size, __epsabs);
	  x = __fmin->x->data;	  
	  f = __fmin->fval;
	}

      if (__mode == MODE_FDF) 
	{
	  if (g_debug) std::cerr << "DEBUG: multidimensional_minimization::minimize: MODE_FDF" << std::endl;
	  dim = __fdf.n;
	  status = gsl_multimin_fdfminimizer_iterate (__fdfmin);
	  if (status != 0) 
	    {
	      break;
	    }
	  status = gsl_multimin_test_gradient (__fdfmin->gradient, __epsabs);
	  x = __fdfmin->x->data;	  
	  f = __fdfmin->f;
	}
      _at_step_hook (status, iter, x, dim, f);
      __fval = f;
      if ((iter % __modulo_iter) == 0) 
	{
	  clog << "mygsl::multidimensional_minimization::minimize: Iteration #" << iter << " Fval == " << __fval << endl;
	  __sys->print_status (clog);
	  clog << endl;
	}
      
    } while ((status == GSL_CONTINUE) && (iter < __max_iter)); 

    __n_iter = iter;
    if (status == GSL_SUCCESS) 
      {
	if (g_debug) std::clog << "multidimensional_minimization::minimize: END" << std::endl;
	__sys->from_double_star (x, dim);
      }
   
    return status;     
  }

}

// end of mygsl::multidimensional_minimization.cc
