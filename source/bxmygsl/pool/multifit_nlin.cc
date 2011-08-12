// mygsl::multifit_nlin.cc

#include <mygsl/multifit_nlin.h>

namespace mygsl {

  size_t multifit_nlin::DEFAULT_MAX_ITER = 1000;
  bool   multifit_nlin::DEFAULT_VERBOSE  = true;
  bool   multifit_nlin::g_debug          = false;

  /**********************************************************/

  void multifit_nlin_system::lock_params ()
  {
    __lock_params = true;
  }

  void multifit_nlin_system::unlock_params ()
  {
    __lock_params = false;
  }

  bool multifit_nlin_system::is_lock_params () const
  {
    return __lock_params == true;
  }

  multifit_nlin_system::multifit_nlin_system ()
  {
    __lock_params = false;
  }

  multifit_nlin_system::~multifit_nlin_system ()
  {
    __lock_params = false;
    __params.clear ();
    //std::clog << "multifit_nlin_system::~multifit_nlin_system: done." << std::endl;
  }

  const param_entry & multifit_nlin_system::get_free_param (int i_) const
  {
    if ((i_ < 0) || (i_ >= ))
      {
	throw runtime_error ("multifit_nlin_system::get_free_param: Invalid free parameter index!");
      }
    int j = 0;
    for (int i = 0; i < __params.size (); i++) 
      {
	if (! __params.at (i).is_free ())
	  {
	    j++;
	  }
	else
	  {
	    if (j == i) break;
	  }
      }
    return __params.at (j);
  }

  param_entry & multifit_nlin_system::get_free_param (int i_)
  {
    if ((i_ < 0) || (i_ >= ))
      {
	throw runtime_error ("multifit_nlin_system::get_free_param: Invalid free parameter index!");
      }
    int j = 0;
    for (int i = 0; i < __params.size (); i++) 
      {
	if (! __params.at (i).is_free ())
	  {
	    j++;
	  }
	else
	  {
	    if (j == i) break;
	  }
      }
    return __params.at (j);
  }

  void multifit_nlin_system::print_params (std::ostream & out_) const
  {
    for (int i = 0; i < __params.size (); i++) 
      {
      
	out_ << __params.at (i).name << ": " 
	     << __params.at (i).value << " ["
	     << __params.at (i).min << ";"
	     << __params.at (i).max << "] "
	     << __params.at (i).step << " "
	     << std::endl;
      }
  }

  void multifit_nlin_system::print_params2 (std::ostream & out_) const
  {
    for (int i = 0; i < __params.size (); i++) 
      {
	out_ << __params.at (i).value << ' ';
      }
  }
 
  void multifit_nlin_system::add_param (const param_entry & pe_)
  {
    if (is_lock_params ()) 
      {
	std::ostringstream message;
	message << "multifit_nlin_system::add_param: ";
	message << "system is locked!";
	throw std::runtime_error (message.str ());            
      }

    if (std::find_if (__params.begin (), 
		      __params.end (), 
		      param_has_name (pe_.name)) != __params.end ()) {
      std::ostringstream message;
      message << "multifit_nlin_system::add_param: ";
      message << "parameter '" << pe_.name << "' already exist!";
      throw std::runtime_error (message.str ());       
    }
    __params.push_back (pe_);
    __update_dimensions ();
  }

  const param_entry & multifit_nlin_system::get_param (int i_) const
  {
    return __params.at (i_);
  }

  void multifit_nlin_system::set_param_value (int i_, double val_)
  {
    __params.at (i_).set_value (val_);
  }

  void multifit_nlin_system::set_param_value_no_check (int i_, double val_)
  {
    __params.at (i_).set_value_no_check (val_);
  }

  bool multifit_nlin_system::is_param_free (int i_) const
  {
    return __params.at (i_).is_free ();
  }

  bool multifit_nlin_system::is_param_fixed (int i_) const
  {
    return __params.at (i_).is_fixed ();
  }

  double multifit_nlin_system::get_param_value (int i_) const
  {
    return get_param (i_).value;
  }

  double multifit_nlin_system::get_param_min (int i_) const
  {
    return get_param (i_).min;
  }

  double multifit_nlin_system::get_param_max (int i_) const
  {
    return get_param (i_).max;
  }

  double multifit_nlin_system::get_param_step (int i_) const
  {
    return get_param (i_).step;
  }

  const std::string & multifit_nlin_system::get_param_name (int i_) const
  {
    return get_param (i_).name;
  }

  /*
  int multifit_nlin_system::eval_f (const double * x_, 
				    double & f_)
  {
    from_double_star (x_, get_free_dimension ());
    return _eval_f (f_);
  }

  int multifit_nlin_system::eval_df (const double * x_, 
				double * gradient_)
  {
    from_double_star (x_, get_free_dimension ());
    return _eval_df (gradient_);
  }

  int multifit_nlin_system::eval_fdf (const double * x_, 
				 double & f_, 
				 double * gradient_)
  {
    int ret = eval_f (x_, f_);
    if (ret != 0) return 1;
    return eval_df (x_, gradient_);
  }
  */

  size_t multifit_nlin_system::get_dimension () const
  {
    return __params.size ();
  }
  
  size_t multifit_nlin_system::get_free_dimension () const
  {
    return __free_dimension;
  }
  
  size_t multifit_nlin_system::get_auto_dimension () const
  {
    return __auto_dimension;
  }

  int multifit_nlin_system::_prepare_values ()
  {
    std::ostringstream message;
    message << "multifit_nlin_system::_prepare_values: "
	    << "You should provide an inherited '_prepare_values' method in your 'multifit_nlin_system' class!";
    throw std::runtime_error (message.str ());
  }

  int multifit_nlin_system::_auto_values ()
  {
    if (get_auto_dimension () > 0) 
      { 
	std::ostringstream message;
	message << "multifit_nlin_system::_auto_values: "
		<< "There are AUTO paramaters! "
		<< "You should provide an inherited '_auto_values' method in your 'multifit_nlin_system' class!";
	throw std::runtime_error (message.str ());
      }
    return 0;
  }

  int multifit_nlin_system::auto_values ()
  {
    if (get_auto_dimension () > 0) 
      { 
	return _auto_values ();
      }
    return 0;
  }

  int multifit_nlin_system::prepare_values ()
  {
    int status = _prepare_values ();
    if (status != 0) return status ;
    return auto_values ();
  }
  
  void multifit_nlin_system::to_double_star (double * pars_, 
					     size_t dimension_) const
  {
    size_t fd = get_free_dimension ();
    if (dimension_ != fd) 
      {
	throw std::range_error (
				"multifit_nlin_system::to_double_star: Invalid dimension!");
      }
    int i_free = 0;
    for (int i_par = 0; i_par < get_dimension (); i_par++) 
      { 
	if (is_param_free (i_par)) 
	  {
	    pars_[i_free] = get_param_value (i_par);
	    i_free++;
	  }
      }
  }

  void multifit_nlin_system::from_double_star (const double * pars_, 
					       size_t dimension_)
  {
    if (dimension_ != get_free_dimension ()) 
      {
	throw std::range_error (
				"multifit_nlin_system::from_double_star: Invalid dimension!");
      }
    int i_free = 0;
    for (int i = 0; i < get_dimension (); i++) 
      { 
	if (is_param_free (i)) 
	  {
	    __params.at (i).set_value_no_check (pars_[i_free]);
	    i_free++;
	  }
      }
    prepare_values ();
  }

  void  multifit_nlin_system::dump (std::ostream & out_) const
  {
    std::string tag="|-- ";
    std::string last_tag="`-- ";
    out_ << "Multifit_Nlin system:" << std::endl;
    out_ << tag << "dimension: " << get_dimension () << std::endl;
    out_ << tag << "# free parameter: " << get_free_dimension () << std::endl;
    out_ << last_tag << "Parameters:" << std::endl;
    for (int i = 0; i < __params.size (); i++) 
      {
	std::string & atag = tag;
	if (i == (__params.size () - 1)) atag = last_tag;
	out_ << "   " << atag << "'" << get_param_name (i) << "': " << get_param_value (i)
	     << " is " << (is_param_fixed (i)?"fixed":is_param_free (i)?"free":"auto")
	     << " should be in [" << get_param_min (i) << ";" <<  get_param_max (i)
	     << "] with step " << get_param_step (i)
	     << std::endl;
      }
  }
 
  /**************************************************************************/

  int multifit_nlin::multifit_nlin_f (const gsl_vector * v_, 
				      void * params_,
				      gsl_vector * f_)
  {
    multifit_nlin_system * ms = static_cast<multifit_nlin_system*> (params_);
    ms->eval_f (v_->data, f_->data);
    return GSL_SUCCESS;
  }

  int multifit_nlin::multifit_nlin_df (const gsl_vector * v_, 
				       void * params_ , 
				       gsl_matrix * j_)
  {
    multifit_nlin_system * ms = static_cast<multifit_nlin_system*> (params_);
    ms->eval_df (v_->data, j_->data);
    return GSL_SUCCESS;
  }

  int multifit_nlin::multifit_nlin_fdf (const gsl_vector * v_, 
					void * params_,
					gsl_vector * f_, 
					gsl_matrix * j_)
  {
    multifit_nlin_system * ms = static_cast<multifit_nlin_system*> (params_);
    ms->eval_f (v_->data, v_->data);
    ms->eval_df (v_->data, j_->data);
    return GSL_SUCCESS;
  }

  /**********************************************************/

  multifit_nlin::default_step_action multifit_nlin::__default_step_action;

  void multifit_nlin::set_default_step_action ()
  {
    __at_step_action = &multifit_nlin::__default_step_action;
  }

  void multifit_nlin::unset_step_action ()
  {
    __at_step_action = 0;
  }

  void multifit_nlin::set_step_action (multifit_nlin::at_step_action & asd_)
  {
    __at_step_action = &asd_;
  }
  
  void multifit_nlin::default_step_action::action (int status_, 
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
    if (multifit_nlin::g_debug) std::clog << "Iteration: " << iter << " (" 
				     << (status==GSL_SUCCESS?"minimum found":"continue") 
				     << ')' << std::endl;
    std::cout << iter << ' ' << dim << ' ';
    for (int i=0; i<dim ; i++) {
      std::cout << x[i] << ' ';
    } 
    std::cout << f << ' ' << status << ' ';
    std::cout << std::endl;
  }

  /**********************************************************/
  void multifit_nlin::print_types (std::ostream & out_)
  {
    out_ << "Levenberg-Marquardt";
  }

  bool multifit_nlin::name_is_valid (const std::string & name_)
  {
    if (name_ == "Levenberg-Marquardt") return true;
    if (name_ == "lmsder") return true;
    return false;
  }

  std::string multifit_nlin::get_name () const
  {
    return std::string (gsl_multifit_fdfsolver_name (__s));
  }

  void multifit_nlin::__init_algorithm (const std::string & name_) 
  {
    if (! name_is_valid (name_)) {
      std::ostringstream message;
      message << "multifit_nlin::__init_algorithm: "
	      << "Invalid minimization algorithm '" 
	      << name_ << "'!";
      throw std::runtime_error (message.str ());
    }

    if (name_ == "Levenberg-Marquardt" || name_ == "lmsder") 
      {
	__T = gsl_multifit_fdfsolver_lmsder;
      }
  }

  void multifit_nlin::devel_dump_x () const
  {
    if (__x == 0) 
{
      std::clog << "DEVEL: __x==0" << std::endl;
      return;
    }
    std::clog << "DEVEL ================================= " << std::endl; 
    std::clog << "DEVEL: " 
	      << " __x.size  = " << __x->size
	      << std::endl;
    std::clog << "DEVEL: " 
	      << " __x.stride  = " << __x->stride
	      << std::endl;
    std::clog << "DEVEL: " 
	      << " __x.data  = " << __x->data
	      << std::endl;
    std::clog << "DEVEL: " 
	      << " __x.owner  = " << __x->owner
	      << std::endl;
    for (int i=0; i< __x->size; i++) 
{
      std::clog << "DEVEL: " 
		<< " __x.data["<< i<< "]  = " << __x->data[i]
		<< std::endl;
    }
    std::clog << "DEVEL: " 
	      << " __x.block.size  = " << __x->block->size
	      << std::endl;
    for (int i=0; i< __x->block->size; i++) 
{
      std::clog << "DEVEL: " 
		<< " __x.block["<< i<< "]  = " << __x->block->data[i]
		<< std::endl;
    }
    std::clog << "DEVEL ================================= " << std::endl; 
  }

  void multifit_nlin::init (const std::string & name_, 
			    multifit_nlin_system & ms_)
  {
    __sys = &ms_;
    if (! __sys->is_lock_params ()) 
{
      __sys->lock_params ();
    }

    __status = 0;
    __iter = 0;
    __n = __sys->get_number_of_data ();
    __p = __sys->get_free_dimension ();
    __dof = __n - __p;
    if (__dof < 1)
      {
	throw runtime_error ("multifit_nlin::init: DOF too small!");
      }
    __init_algorithm (name_);
    if (multifit_nlin::g_debug) std::clog << "DEBUG: multifit_nlin::init: allocating ' __x'..." << std::endl;
    __x  = gsl_vector_alloc (__p);
    gsl_vector_set_zero (__x);

    __covar  = gsl_matrix_alloc (__p, __p);

    // setup the multifit function:
    __fdf.f      = &multifit_nlin::multifit_nlin_f;
    __fdf.df     = &multifit_nlin::multifit_nlin_df;
    __fdf.fdf    = &multifit_nlin::multifit_nlin_fdf;
    __fdf.n      = __n;
    __fdf.p      = __p;
    __fdf.params = (void *) __sys;

    // setup the multifit solver:
    __s          = gsl_multifit_fdfsolver_alloc (__T, __n, __p);
    gsl_multifit_fdfsolver_set (__s, &__fdf, x); 


    if (multifit_nlin::g_debug) std::clog << "DEBUG: multifit_nlin::init: set initial values..." << std::endl;

    // load fit parameters' values from initial guess:
    for (int i = 0; i < __n; i++)
      {
	double val = __sys->get_param_value (i);
	gsl_vector_set (__x, i, val);
      }

    __sys->to_double_star (__x->data, __p);

    /*
      int i_free=0;
      std::clog << "DEVEL: multifit_nlin::init: gsl_vector_set (__x...)" << std::endl;
      for (int i_par=0; i_par< __sys->get_dimension (); i_par++) { 
      if (__sys->is_param_free (i_par)) {
      gsl_vector_set (__x, i_free, __sys->get_param_value (i_par));
      i_free++;
      }
      }
      std::clog << "DEVEL: multifit_nlin::init: gsl_vector_set (__x...) done." << std::endl;
    */

    if (multifit_nlin::g_debug) 
      {
	for (int i = 0; i < n; i++) 
	  {
	    double par = gsl_vector_get (__x, i);
	    std::clog << "DEBUG: multifit_nlin::init: par=" << par << std::endl;
	  }
      }

  }

  void multifit_nlin::reset ()
  {
    //std::clog << "multifit_nlin::reset: entering..." << std::endl;
    if (__s != 0) 
      {
	gsl_multifit_fsolver_free (__s);
	__s = 0;
      }
    if (__x != 0) 
      {
	gsl_vector_free (__x);
	__x = 0;
      }
    if (__covar != 0) 
      {
	gsl_matrix_free (__covar);
	__covar = 0;
      }
    __fdf = 0;

    __fdf.f      = 0;
    __fdf.df     = 0;
    __fdf.fdf    = 0;
    __fdf.n      = 0;
    __fdf.p      = 0;
    __fdf.params = 0;

    __at_step_action = 0;
    if (__sys != 0) 
      {
	__sys->unlock_params ();
	__sys = 0;
      }
    __iter = 0;
    //std::clog << "multifit_nlin::reset: done." << std::endl;
  }

  size_t multifit_nlin::get_iter () const
  {
    return __iter;
  }
  
  multifit_nlin::multifit_nlin ()
  {
    __verbose = DEFAULT_VERBOSE;
    __T = 0;
    __s = 0;

    __fdf.f      = 0;
    __fdf.df     = 0;
    __fdf.fdf    = 0;
    __fdf.n      = 0;
    __fdf.params = 0;

    __x          = 0;
    __covar      = 0;
    __max_iter   = DEFAULT_MAX_ITER;
    __at_step_action = 0;

    __status = 0;
    __iter   = 0;
	  
  }

multifit_nlin::~multifit_nlin ()
{
  reset ();
  }

/*
  void multifit_nlin::_at_step_hook (int status_, 
				     size_t iter_, 
				     double * x_, 
				     size_t   dim_, 
				     double f_)
  {
    if (multifit_nlin::g_debug) std::clog << "DEBUG: multifit_nlin::_at_step_hook: entering..." << std::endl;
    if (__at_step_action != 0) {
      if (multifit_nlin::g_debug) std::clog << "DEBUG: multifit_nlin::_at_step_hook: __at_step_action..." << std::endl;
      (*__at_step_action) (status_, iter_, x_, dim_, f_);
    }
    if (multifit_nlin::g_debug) std::clog << "DEBUG: multifit_nlin::_at_step_hook: exiting." << std::endl;
  }
  */

const string & multifit_nlin::get_error_message () const
{
  return __error_message;
}

void multifit_nlin::reset_error_message ()
{
  __error_message = "";  
}

size_t multifit_nlin::get_n () const
{
  return __n;
}

size_t multifit_nlin::get_p () const 
{
  return __p;
}

double multifit_nlin::get_dof () const 
{
  return __dof;
}

double multifit_nlin::get_chi () const 
{
  return __chi;
}


  int multifit_nlin::fit (double epsabs_, double epsrel_)
  {
    size_t iter   = 0;
    int    status = 0;
    size_t dim    = 0;
    double epsabs = epsabs_;
    double epsrel = epsrel_;

    double * x    = 0;
    double f;

    do {
      if (multifit_nlin::g_debug) std::clog << std::endl << "DEBUG: multifit_nlin::minimize: NEW ITERATTION" << std::endl;

      iter++;
      
      dim = __fdf.n;
      status = gsl_multifit_fdfsolver_iterate (__s);


      if (status != 0) 
	{
	  ostringstream message;
	  message << gsl_strerror (status);
	  __error_message = message.str ();
	  break;
	}
      status = gsl_multifit_test_delta (__s->dx, 
					__s->x, 
					epsabs,
					epsrel);
      //x = __s->x->data;	  
      //f = __s->f;
      //_at_step_hook (status, iter, x, dim, f);
      //__fval = f;
      
    } while ((status == GSL_CONTINUE) && (iter < __max_iter)); 

    __iter = iter;

    gsl_multifit_covar (__s->J, 0.0, __covar);

    __chi = gsl_blas_dnrm2 (__s->f);
    double c = GSL_MAX_DBL (1, __chi / sqrt (__dof));
    __status = status;
    if (__status == GSL_SUCCESS) 
      {
	if (multifit_nlin::g_debug) std::clog << "multifit_nlin::minimize: END" << std::endl;

	for (int i = 0; i < __p; i++)
	  {
	    double x = gsl_vector_get (__s->x, i);
	    double err = c * sqrt (gsl_matrix_get (__covar, i, i));
	    __sys->get_free_param (i).set_best_value (x,
						      err,
						      err);
	  }

      }
    else
      {
	ostringstream message;
	message << gsl_strerror (status);
	__error_message = message.str ();
      }
    return __status;     
  }

}

// end of mygsl::multifit_nlin.cc
