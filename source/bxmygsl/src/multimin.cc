// mygsl::multimin.cc

#include <mygsl/multimin.h>

namespace mygsl {

  size_t multimin::DEFAULT_MAX_ITER = 300;

  bool   multimin::DEFAULT_VERBOSE  = true;

  /**********************************************************/

  double multimin_system::param_entry::get_value() const
  {
    return value;
  }

  void multimin_system::param_entry::set_value_no_check( double value_ )
  {
    value=value_;
  }

  void multimin_system::param_entry::set_value( double value_ )
  {
    if ( value_ < min ) {
      std::ostringstream message;
      message << "multimin_system::param_entry::set_value: ";
      message << "value < min for parameter '" << name << "'!";
      throw std::runtime_error(message.str());
    }
    if ( value_ > max ) {
      std::ostringstream message;
      message << "multimin_system::param_entry::set_value: ";
      message << "value > max for parameter '" << name << "'!";
      throw std::runtime_error(message.str());
    }
    value=value_;
  }

  multimin_system::multimin_system()
  {
  }

  multimin_system::~multimin_system()
  {
    __params.clear();
  }

  multimin_system::param_entry multimin_system::param_entry::make_param_entry(
    const std::string & name_  ,
    double value_ ,
    double min_   ,
    double max_   ,
    double step_  ,
    bool   fixed_ )
  {
    multimin_system::param_entry pe;
    pe.name = name_;
    pe.min  = min_;
    pe.max  = max_;
    pe.set_value(value_);
    if ( step_ > (max_-min_) ) {
      std::ostringstream message;
      message << "multimin_system::param_entry::make_param_entry: ";
      message << "step to large for parameter '" << name_ << "'!";
      throw std::runtime_error(message.str());
    }
    pe.step  = step_;
    pe.fixed = fixed_;
    return pe;
  }

  void multimin_system::print_params( std::ostream & out_ ) const
  {
    for ( int i=0; i<__params.size(); i++ ) {
      out_ << __params.at(i).name << ": " 
	   << __params.at(i).value << " ["
	   << __params.at(i).min << ";"
	   << __params.at(i).max << "] "
	   << __params.at(i).step << " "
	   << std::endl;
    }
  }
 
  void multimin_system::add_param( const param_entry & pe_ )
  {
    if ( find_if(__params.begin(),
		 __params.end(),
		 multimin_system::param_has_name(pe_.name) ) != __params.end() ) {
      std::ostringstream message;
      message << "multimin_system::add_param: ";
      message << "parameter '" << pe_.name << "' already exist!";
      throw std::runtime_error(message.str());       
    }
    __params.push_back(pe_);
  }

  const multimin_system::param_entry &  multimin_system::get_param( int i_ ) const
  {
    return __params.at(i_);
  }

  void multimin_system::set_param_value( int i_, double val_ )
  {
    __params.at(i_).set_value(val_);
  }

  double multimin_system::get_param_value( int i_ ) const
  {
    return get_param(i_).value;
  }

  double multimin_system::get_param_min( int i_ ) const
  {
    return get_param(i_).min;
  }

  double multimin_system::get_param_max( int i_ ) const
  {
    return get_param(i_).max;
  }

  double multimin_system::get_param_step( int i_ ) const
  {
    return get_param(i_).step;
  }

  const std::string & multimin_system::get_param_name( int i_ ) const
  {
    return get_param(i_).name;
  }

  int multimin_system::eval_fdf( const double * x_ , 
				 double & f_ , 
				 double * gradient_ )
  {
    int ret = eval_f(x_,f_);
    if ( ret != 0 ) return 1;
    return eval_df(x_,gradient_);
  }

  size_t multimin_system::get_dimension() const
  {
    return __params.size();
  }
  
  void multimin_system::to_double_star( double * pars_ , 
					size_t dimension_ ) const
  {
    if ( dimension_ != get_dimension() ) {
      throw std::range_error(
        "multimin_system::to_double_star: Invalid dimension!");
    }
    for ( int i=0; i<get_dimension(); i++ ) { 
      pars_[i] = get_param_value(i);
    }
  }

  void multimin_system::from_double_star( const double * pars_ , 
					  size_t dimension_ )
  {
    if ( dimension_ != get_dimension() ) {
      throw std::range_error(
        "multimin_system::from_double_star: Invalid dimension!");
    }
    for ( int i=0; i<get_dimension(); i++ ) { 
      //set_param_value(i,pars_[i]);
      __params.at(i).set_value_no_check(pars_[i]);
    }
  }
 
  /**********************************************************/
 
  double multimin::multimin_f( const gsl_vector * v_ , 
			       void * params_ )
  {
    multimin_system * ms = static_cast<multimin_system*>(params_);
    double f;
    ms->eval_f(v_->data,f);
    return f;
  }
  
  void multimin::multimin_df( const gsl_vector * v_ , 
			      void * params_ , 
			      gsl_vector * gradient_ )
  {
    multimin_system * ms = static_cast<multimin_system*>(params_);
    ms->eval_df(v_->data,gradient_->data);
  }

  void multimin::multimin_fdf( const gsl_vector * v_ , 
			       void * params_ ,
			       double * f_ , 
			       gsl_vector * gradient_ )
  {
    multimin_system * ms = static_cast<multimin_system*>(params_);
    ms->eval_fdf(v_->data,*f_,gradient_->data);
  }

  /**********************************************************/

  multimin::default_step_action multimin::__default_step_action;

  void multimin::set_default_step_action()
  {
    __at_step_action = &multimin::__default_step_action;
  }

  void multimin::unset_step_action()
  {
    __at_step_action = 0;
  }

  void multimin::set_step_action( multimin::at_step_action & asd_ )
  {
    __at_step_action = &asd_;
  }
  
  void multimin::default_step_action::action( int status_ ,
					      size_t iter_ , 
					      double * x_ , 
					      size_t   dim_ ,
					      double   f_ )
  {
    int    status = status_;
    size_t iter   = iter_;
    double * x    = x_;
    size_t dim    = dim_;
    double   f    = f_;
    std::clog << "Iteration: " << iter << " (" 
	      << (status==GSL_SUCCESS?"minimum found":"continue") 
	      << ')' << std::endl;
    std::cout << iter << ' ' << dim << ' ';
    for ( int i=0; i<dim ; i++ ) {
      std::cout << x[i] << ' ';
    } 
    std::cout << f << ' ' << status << ' ';
    std::cout << std::endl;
  }

  /**********************************************************/
  void multimin::print_types( std::ostream & out_ )
  {
    out_ << "conjugate_fr" << ' ';
    out_ << "conjugate_pr" << ' ';
    out_ << "vector_bfgs" << ' ';
    out_ << "steepest_descent" << ' ';
    out_ << "nmsimplex";
  }

  bool multimin::name_is_valid( const std::string & name_ )
  {
    if ( name_ == "conjugate_fr" ) return true;
    if ( name_ == "conjugate_pr" ) return true;
    if ( name_ == "vector_bfgs"  ) return true;
    if ( name_ == "steepest_descent" ) return true;
    if ( name_ == "nmsimplex"   ) return true;
    return false;
  }

  std::string multimin::get_name() const
  {
    if ( __mode == MODE_FDF && __fdfmin != 0  ) {
      return std::string(gsl_multimin_fdfminimizer_name(__fdfmin));
    }
    if ( __mode == MODE_F && __fmin != 0 ) {
      return std::string(gsl_multimin_fminimizer_name(__fmin));
    }
    return "";
  }

  void multimin::__init_algorithm( const std::string & name_ ) 
  {
    if ( ! name_is_valid(name_) ) {
      std::ostringstream message;
      message << "multimin::__init_algorithm: "
	      << "Invalid minimization algorithm '" 
	      << name_ << "'!";
      throw std::runtime_error(message.str());
    }

    if ( name_ == "conjugate_fr" ) {
      __algo_fdf = gsl_multimin_fdfminimizer_conjugate_fr;
      __set_mode(MODE_FDF);
    }
    
    if ( name_ == "conjugate_pr" ) {
      __algo_fdf = gsl_multimin_fdfminimizer_conjugate_pr;
      __set_mode(MODE_FDF);
    }
    
    if ( name_ == "vector_bfgs" ) {
      __algo_fdf = gsl_multimin_fdfminimizer_vector_bfgs;
      __set_mode(MODE_FDF);
    }
    
    if ( name_ == "steepest_descent" ) {
      __algo_fdf = gsl_multimin_fdfminimizer_steepest_descent;
      __set_mode(MODE_FDF);
    }
    
    if ( name_ == "nmsimplex" ) {
      __algo_f = gsl_multimin_fminimizer_nmsimplex;
      __set_mode(MODE_F);
    }
  }

  void multimin::__set_mode( int mode_ )
  {
    if ( mode_ != MODE_F && mode_ != MODE_FDF ) {
      throw std::runtime_error("multimin:__set_mode: Invalid mode!");
    }
    __mode = mode_;
  }

  void multimin::init( const std::string & name_ , 
		       multimin_system & ms_ )
  {
    __sys = &ms_;

    size_t n = __sys->get_dimension();

    __init_algorithm(name_);

    if ( __mode == MODE_FDF ) {
      __fdf.f      = &multimin::multimin_f;
      __fdf.df     = &multimin::multimin_df;
      __fdf.fdf    = &multimin::multimin_fdf;
      __fdf.n      = n;
      __fdf.params = (void *) __sys;
      __fdfmin     = gsl_multimin_fdfminimizer_alloc(__algo_fdf,__fdf.n);
    }

    if ( __mode == MODE_F ) {
      __f.f      = &multimin::multimin_f;
      __f.n      = n;
      __f.params = (void *) __sys;
      __fmin     = gsl_multimin_fminimizer_alloc(__algo_f,__f.n);
    }

    std::clog << "DEBUG: multimin::init: allocating ' __x'..." << std::endl;
    __x  = gsl_vector_alloc(n);
    std::clog << "DEBUG: multimin::init: set initial values..." << std::endl;
    __sys->to_double_star(__x->data,n);
    
    for ( int i=0; i<n; i++ ) {
      double par=gsl_vector_get(__x,i);
      std::clog << "DEBUG: multimin::init: par=" << par << std::endl;
    }
    

    if ( __mode == MODE_FDF ) {
      __fdf_step_size = 0.01;
      __fdf_tol       = 1.e-4;
      gsl_multimin_fdfminimizer_set(__fdfmin,&__fdf,__x,
				    __fdf_step_size,__fdf_tol); 
    }

    if ( __mode == MODE_F ) {
      __ss = gsl_vector_alloc(n);
      for ( int i=0; i<n ; i++ ) {
	gsl_vector_set(__ss,i,__sys->get_param_step(i));
      }
      gsl_multimin_fminimizer_set(__fmin,&__f,__x,__ss); 
    }

  }

  void multimin::reset()
  {
    if ( __fdfmin!=0 ) {
      gsl_multimin_fdfminimizer_free(__fdfmin);
      __fdfmin=0;
    }
    if ( __fmin!=0 ) {
      gsl_multimin_fminimizer_free(__fmin);
      __fmin=0;
    }
    if ( __x != 0 ) {
      gsl_vector_free(__x);
      __x=0;
    }
    if ( __ss != 0 ) {
      gsl_vector_free(__ss);
      __ss=0;
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
    __epsabs         = 1.0;
    __at_step_action = 0;

  }

  multimin::multimin()
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

    __x        = 0;
    __ss       = 0;
    __fdf_step_size = 0.01;
    __fdf_tol       = 1.e-4;
    __max_iter     = DEFAULT_MAX_ITER;
    __stopping     = STOPPING_SIZE;
    __epsabs         = 1.0;
    __at_step_action = 0;
	  
  }

  multimin::~multimin()
  {
    reset();
  }

  void multimin::_at_step_hook( int status_ ,
				size_t iter_ , 
				double * x_ , 
				size_t   dim_ , 
				double f_ )
  {
    std::clog << "DEBUG: multimin::_at_step_hook: entering..." << std::endl;
    if ( __at_step_action != 0 ) {
      std::clog << "DEBUG: multimin::_at_step_hook: __at_step_action..." << std::endl;
      (*__at_step_action)(status_,iter_,x_,dim_,f_);
    }
    std::clog << "DEBUG: multimin::_at_step_hook: exiting." << std::endl;
  }

  int multimin::minimize( double epsabs_ )
  {
    size_t iter   = 0;
    int    status = 0;
    size_t dim    = 0;
    double * x    = 0;
    __epsabs      = 1.e-2;
    double f;

    do {
      std::cerr << std::endl << "DEBUG: multimin::minimize: NEW ITERATTION" << std::endl;
      // setup working array:
      //___sys->to_double_star(__y,__system.dimension);

      iter++;

      if ( __mode ==  MODE_F ) {
	std::cerr << "DEBUG: multimin::minimize: MODE_F" << std::endl;
	//if ( __fmin != 0 ) {
	dim = __f.n;
	status=gsl_multimin_fminimizer_iterate(__fmin);
	if ( status != 0 ) {
	  break;
	}
	double size = gsl_multimin_fminimizer_size(__fmin);
	std::cerr << "DEBUG: multimin::minimize: MODE_F size=" << size 
		  << " epsabs=" << __epsabs << std::endl;
	status=gsl_multimin_test_size(size,__epsabs);
	x = __fmin->x->data;	  
	f = __fmin->fval;
      }

      if ( __mode ==  MODE_FDF ) {
	std::cerr << "DEBUG: multimin::minimize: MODE_FDF" << std::endl;
	//if ( __fdfmin != 0 ) {
	dim = __fdf.n;
	status=gsl_multimin_fdfminimizer_iterate(__fdfmin);
	if ( status != 0 ) {
	  break;
	}
	status=gsl_multimin_test_gradient(__fdfmin->gradient,__epsabs);
	x = __fdfmin->x->data;	  
	f = __fdfmin->f;
      }

      /*
      if ( __verbose ) {
	std::clog << "multimin::minimize:iteration: " 
		  << iter << std::endl;
	if ( status == GSL_SUCCESS ) {
	  std::clog << "multimin::minimize: minimum found at: " 
		    << std::endl;
	}
	else {
	  std::clog << "multimin::minimize: current step at: " 
		    << std::endl;
	}
	for ( size_t i=0; i<dim ; i++ ) {
	  //std::clog << "TEST 0 i=" << i << std::endl;
	  gsl_vector * tmp; 
	  if ( __mode ==  MODE_F ) {
	    tmp = __fmin->x;
	    //std::clog << "TEST 0a i=" << i << std::endl;
	  }
	  if ( __mode ==  MODE_FDF ) {
	    tmp = __fdfmin->x;
	  }
	  //std::clog << "Par[" << i << "] = "
	  //    << gsl_vector_get(tmp,i) << std::endl;
	}
      }
      */
      _at_step_hook(status,iter,x,dim,f);
      __fval=f;
      
    } while ( status == GSL_CONTINUE && iter < __max_iter); 

    if ( status == GSL_SUCCESS ) {
      std::clog << "multimin::minimize: END" << std::endl;
      __sys->from_double_star(__x->data,dim);
    }
   
    return status;     
  }

  /*
  void multimin::fetch_solution() const
  {
    __sys
  }
  */

}

// end of mygsl::multimin.cc
