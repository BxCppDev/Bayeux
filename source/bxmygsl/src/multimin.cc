// mygsl::multimin.cc

#include <mygsl/multimin.h>

namespace mygsl {

  size_t multimin::DEFAULT_MAX_ITER = 1000;
  bool   multimin::DEFAULT_VERBOSE  = true;
  bool   multimin::g_debug          = false;

  /**********************************************************/

  double multimin_system::param_entry::get_dist_to_min() const
  {
    double dist=0.0;
    if ( has_min() ) {
      dist=std::abs(get_value()-min);
    }
    return dist;
  }

  double multimin_system::param_entry::get_dist_to_max() const
  {
    double dist=0.0;
    if ( has_max() ) {
      dist=std::abs(get_value()-max);
    }
    return dist;
  }

  double multimin_system::param_entry::get_dist_to_limit() const
  {
    double dist=0.0;

    double dmin=-1.0;
    double dmax=-1.0;

    if ( has_min() ) dmin=get_dist_to_min();
    if ( has_max() ) dmax=get_dist_to_max();

    if ( dmin >= 0. ) dist=dmin;
    if ( dmax >= 0. && dmax < dmin ) dist=dmax;

    return dist;
  }

  bool multimin_system::param_entry::is_in_range() const
  {
    bool ok=true;
    if ( has_limit() ) {
      if ( ok && has_min() && get_value()<min ) ok=false;
      if ( ok && has_max() && get_value()>max ) ok=false;
    }
    return ok;
  }

  double multimin_system::param_entry::get_value() const
  {
    return value;
  }

  double multimin_system::param_entry::get_step() const
  {
    return step;
  }

  double multimin_system::param_entry::get_min() const
  {
    return min;
  }

  double multimin_system::param_entry::get_max() const
  {
    return max;
  }

  void multimin_system::param_entry::set_value_no_check( double value_ )
  {
    value=value_;
  }

  void multimin_system::param_entry::set_value( double value_ )
  {
    double old_val=value;
    value=value_;
    if ( ! is_in_range() ) {
      value=old_val;
      std::ostringstream message;
      message << "multimin_system::param_entry::set_value: ";
      message << "value < min for parameter '" << name << "'!";
      throw std::runtime_error(message.str());
    }
  }

  void multimin_system::lock_params()
  {
    __lock_params=true;
  }

  void multimin_system::unlock_params()
  {
    __lock_params=false;
  }

  bool multimin_system::is_lock_params() const
  {
    return __lock_params==true;
  }

  multimin_system::multimin_system()
  {
    __lock_params=false;
  }

  multimin_system::~multimin_system()
  {
    __lock_params=false;
    __params.clear();
    //std::cerr << "multimin_system::~multimin_system: done." << std::endl;
  }

  multimin_system::param_entry 
  multimin_system::param_entry::make_param_entry_auto(
    const std::string & name_ )
  {
    multimin_system::param_entry pe;
    pe.name  = name_;
    pe.type  = param_entry::TYPE_AUTO;
    pe.limit = param_entry::LIMIT_NO;
    //pe.set_value_no_check(value_);
    return pe;
  }

  multimin_system::param_entry 
  multimin_system::param_entry::make_param_entry_fixed(
    const std::string & name_ ,
    double value_ )
  {
    multimin_system::param_entry pe;
    pe.name  = name_;
    pe.type  = param_entry::TYPE_FIXED;
    pe.limit = param_entry::LIMIT_NO;
    pe.set_value_no_check(value_);
    return pe;
  }

  multimin_system::param_entry 
  multimin_system::param_entry::make_param_entry_ranged(
    const std::string & name_  ,
    double value_ ,
    double min_   ,
    double max_   ,
    double step_  ,
    bool   fixed_ )
  {
    multimin_system::param_entry pe;
    pe.name = name_;
    pe.type  = TYPE_FREE;
    if ( fixed_ ) pe.type  = param_entry::TYPE_FIXED;
    pe.limit = param_entry::LIMIT_RANGE;
    pe.min  = min_;
    pe.max  = max_;
    pe.set_value(value_);
    if ( step_ > (max_-min_) ) {
      std::ostringstream message;
      message << "multimin_system::param_entry::make_param_entry_ranged: ";
      message << "step to large for parameter '" << name_ << "'!";
      throw std::runtime_error(message.str());
    }
    pe.step  = step_;
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

  void multimin_system::print_params2( std::ostream & out_ ) const
  {
    for ( int i=0; i<__params.size(); i++ ) {
      out_ << __params.at(i).value << ' ';
    }
  }
 
  void multimin_system::add_param( const param_entry & pe_ )
  {
    if ( is_lock_params() ) {
      std::ostringstream message;
      message << "multimin_system::add_param: ";
      message << "system is locked!";
      throw std::runtime_error(message.str());            
    }

    if (std::find_if (__params.begin(),
		 __params.end(),
		 multimin_system::param_has_name(pe_.name) ) != __params.end() ) {
      std::ostringstream message;
      message << "multimin_system::add_param: ";
      message << "parameter '" << pe_.name << "' already exist!";
      throw std::runtime_error(message.str());       
    }
    __params.push_back(pe_);
    __update_dimensions();
  }

  const multimin_system::param_entry & 
  multimin_system::get_param( int i_ ) const
  {
    return __params.at(i_);
  }

  void multimin_system::set_param_value( int i_, double val_ )
  {
    __params.at(i_).set_value(val_);
  }

  void multimin_system::set_param_value_no_check( int i_, double val_ )
  {
    __params.at(i_).set_value_no_check(val_);
  }

  bool multimin_system::is_param_free( int i_ ) const
  {
    return __params.at(i_).is_free();
  }

  bool multimin_system::is_param_fixed( int i_ ) const
  {
    return __params.at(i_).is_fixed();
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

  int multimin_system::eval_f( const double * x_ , 
			       double & f_ )
  {
    from_double_star(x_,get_free_dimension());
    return _eval_f(f_);
  }

  int multimin_system::eval_df( const double * x_ , 
				double * gradient_ )
  {
    from_double_star(x_,get_free_dimension());
    return _eval_df(gradient_);
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
  
  void multimin_system::__update_free_dimension()
  {
    param_is_free pif;
    __free_dimension=std::count_if(__params.begin(),__params.end(),pif);
  }
  
  void multimin_system::__update_auto_dimension()
  {
    param_is_auto pia;
    __auto_dimension=std::count_if(__params.begin(),__params.end(),pia);
  }

  void multimin_system::__update_dimensions()
  {
    __update_free_dimension();
    __update_auto_dimension();
  }
  
  size_t multimin_system::get_free_dimension() const
  {
    return __free_dimension;
  }
  
  size_t multimin_system::get_auto_dimension() const
  {
    return __auto_dimension;
  }

  int multimin_system::_prepare_values()
  {
    std::ostringstream message;
    message << "multimin_system::_prepare_values: "
	    << "You should provide an inherited '_prepare_values' method in your 'multimin_system' class!";
    throw std::runtime_error(message.str());
  }

  int multimin_system::_auto_values()
  {
    if ( get_auto_dimension() > 0 ) { 
      std::ostringstream message;
      message << "multimin_system::_auto_values: "
	      << "There are AUTO paramaters! "
	      << "You should provide an inherited '_auto_values' method in your 'multimin_system' class!";
      throw std::runtime_error(message.str());
    }
    return 0;
  }

  int multimin_system::auto_values()
  {
    if ( get_auto_dimension() > 0 ) { 
      return _auto_values();
    }
    return 0;
  }

  int multimin_system::prepare_values()
  {
    int status = _prepare_values();
    if ( status != 0 ) return status ;
    return auto_values();
  }
  
  void multimin_system::to_double_star( double * pars_ , 
					size_t dimension_ ) const
  {
    size_t fd=get_free_dimension();
    if ( dimension_ != fd ) {
      throw std::range_error(
        "multimin_system::to_double_star: Invalid dimension!");
    }
    int i_free=0;
    for ( int i_par=0; i_par<get_dimension(); i_par++ ) { 
      if ( is_param_free(i_par) ) {
	pars_[i_free] = get_param_value(i_par);
	i_free++;
      }
    }
  }

  void multimin_system::from_double_star( const double * pars_ , 
					  size_t dimension_ )
  {
    if ( dimension_ != get_free_dimension() ) {
      throw std::range_error(
        "multimin_system::from_double_star: Invalid dimension!");
    }
    int i_free=0;
    for ( int i=0; i<get_dimension(); i++ ) { 
      if ( is_param_free(i) ) {
	__params.at(i).set_value_no_check(pars_[i_free]);
	i_free++;
      }
    }
    prepare_values();
  }

  void  multimin_system::dump( std::ostream & out_ ) const
  {
    std::string tag="|-- ";
    std::string last_tag="`-- ";
    out_ << "Multimin system:" << std::endl;
    out_ << tag << "dimension: " << get_dimension() << std::endl;
    out_ << tag << "# free parameter: " << get_free_dimension() << std::endl;
    out_ << last_tag << "Parameters:" << std::endl;
    for ( int i=0; i<__params.size(); i++ ) {
      std::string & atag=tag;
      if ( i==__params.size()-1) atag=last_tag;
      out_ << "   " << atag << "'" << get_param_name(i) << "': " << get_param_value(i)
	   << " is " << (is_param_fixed(i)?"fixed":is_param_free(i)?"free":"auto")
	   << " should be in [" << get_param_min(i) << ";" <<  get_param_max(i)
	   << "] with step " << get_param_step(i)
	   << std::endl;
    }
  }
 
  /**************************************************************************/
 
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
    if ( multimin::g_debug ) std::cerr << "Iteration: " << iter << " (" 
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

  void multimin::devel_dump_x() const
  {
    if ( __x == 0 ) {
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
    for ( int i=0; i< __x->size; i++ ) {
      std::cerr << "DEVEL: " 
		<< " __x.data["<< i<< "]  = " << __x->data[i]
		<< std::endl;
    }
    std::cerr << "DEVEL: " 
	      << " __x.block.size  = " << __x->block->size
	      << std::endl;
    for ( int i=0; i< __x->block->size; i++ ) {
      std::cerr << "DEVEL: " 
		  << " __x.block["<< i<< "]  = " << __x->block->data[i]
		<< std::endl;
    }
    std::cerr << "DEVEL ================================= " << std::endl; 
  }

  void multimin::init( const std::string & name_ , 
		       multimin_system & ms_ )
  {
    __sys = &ms_;
    if ( ! __sys->is_lock_params() ) {
      __sys->lock_params();
    }

    size_t n = __sys->get_free_dimension();
    //std::cerr << "DEVEL: multimin::init: n= " << n << std::endl;
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

    if ( multimin::g_debug ) std::clog << "DEBUG: multimin::init: allocating ' __x'..." << std::endl;
    __x  = gsl_vector_alloc(n);
    gsl_vector_set_zero(__x);
    //std::cerr << "DEVEL: multimin::init: gsl_vector_set_zero(__x)" << std::endl;

    /*
    bool ldebug=true;
    if ( ldebug ) { devel_dump_x(); }
    */

    if ( multimin::g_debug ) std::clog << "DEBUG: multimin::init: set initial values..." << std::endl;

    __sys->to_double_star(__x->data,n);
    /*
    int i_free=0;
    std::cerr << "DEVEL: multimin::init: gsl_vector_set(__x...)" << std::endl;
    for ( int i_par=0; i_par< __sys->get_dimension(); i_par++ ) { 
      if ( __sys->is_param_free(i_par) ) {
	gsl_vector_set(__x,i_free,__sys->get_param_value(i_par));
	i_free++;
      }
    }
    std::cerr << "DEVEL: multimin::init: gsl_vector_set(__x...) done." << std::endl;
    */

    if ( multimin::g_debug ) {
      for ( int i=0; i<n; i++ ) {
	double par=gsl_vector_get(__x,i);
	std::clog << "DEBUG: multimin::init: par=" << par << std::endl;
      }
    }

    if ( __mode == MODE_FDF ) {
      __fdf_step_size = 0.01;
      __fdf_tol       = 1.e-4;
      gsl_multimin_fdfminimizer_set(__fdfmin,&__fdf,__x,
				    __fdf_step_size,__fdf_tol); 
    }

    if ( __mode == MODE_F ) {
      __ss = gsl_vector_alloc(n);
      gsl_vector_set_zero(__ss);
      int i_free=0;
      for ( int i=0; i< __sys->get_dimension(); i++ ) {
	if ( __sys->is_param_free(i) ) {
	  gsl_vector_set(__ss,i_free,__sys->get_param_step(i));
	  i_free++;
	}
      }
      gsl_multimin_fminimizer_set(__fmin,&__f,__x,__ss); 
    }

  }

  void multimin::reset()
  {
    //std::cerr << "multimin::reset: entering..." << std::endl;
    if ( __fdfmin!=0 ) {
      gsl_multimin_fdfminimizer_free(__fdfmin);
      __fdfmin=0;
      //std::cerr << "multimin::reset: free __fdfmin done" << std::endl;
    }
    if ( __fmin!=0 ) {
      gsl_multimin_fminimizer_free(__fmin);
      __fmin=0;
      //std::cerr << "multimin::reset: free __fmin done" << std::endl;
    }
    if ( __x != 0 ) {
      //std::cerr << "multimin::reset: free __x..." << std::endl; 
      //devel_dump_x();
      gsl_vector_free(__x);
      //std::cerr << "multimin::reset: free __x done" << std::endl;
      __x=0;
    }
    if ( __ss != 0 ) {
      gsl_vector_free(__ss);
      __ss=0;
      //std::cerr << "multimin::reset: free __ss done" << std::endl;
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
    if ( __sys != 0 ) {
      __sys->unlock_params();
      __sys=0;
    }
    __n_iter = 0;
    __fval   = 0.0;
    //std::cerr << "multimin::reset: done." << std::endl;
  }

  size_t multimin::get_n_iter() const
  {
    return __n_iter;
  }
  
  double multimin::get_fval() const
  {
    return __fval;
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

    __n_iter = 0;
	  
  }

  multimin::~multimin()
  {
    //std::cerr << "multimin::~multimin: entering..." << std::endl;
    reset();
    //std::cerr << "multimin::~multimin: done." << std::endl;
  }

  void multimin::_at_step_hook( int status_ ,
				size_t iter_ , 
				double * x_ , 
				size_t   dim_ , 
				double f_ )
  {
    if ( multimin::g_debug ) std::clog << "DEBUG: multimin::_at_step_hook: entering..." << std::endl;
    if ( __at_step_action != 0 ) {
      if ( multimin::g_debug ) std::clog << "DEBUG: multimin::_at_step_hook: __at_step_action..." << std::endl;
      (*__at_step_action)(status_,iter_,x_,dim_,f_);
    }
    if ( multimin::g_debug ) std::clog << "DEBUG: multimin::_at_step_hook: exiting." << std::endl;
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
      if ( multimin::g_debug ) std::cerr << std::endl << "DEBUG: multimin::minimize: NEW ITERATTION" << std::endl;

      iter++;

      if ( __mode ==  MODE_F ) {
	if ( multimin::g_debug ) std::cerr << "DEBUG: multimin::minimize: MODE_F" << std::endl;
	//if ( __fmin != 0 ) {
	dim = __f.n;
	status=gsl_multimin_fminimizer_iterate(__fmin);
	if ( status != 0 ) {
	  break;
	}
	double size = gsl_multimin_fminimizer_size(__fmin);
	if ( multimin::g_debug ) std::cerr << "DEBUG: multimin::minimize: MODE_F size=" << size 
		  << " epsabs=" << __epsabs << std::endl;
	status=gsl_multimin_test_size(size,__epsabs);
	x = __fmin->x->data;	  
	f = __fmin->fval;
      }

      if ( __mode ==  MODE_FDF ) {
	if ( multimin::g_debug ) std::cerr << "DEBUG: multimin::minimize: MODE_FDF" << std::endl;
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

    __n_iter=iter;
    if ( status == GSL_SUCCESS ) {
      if ( multimin::g_debug ) std::clog << "multimin::minimize: END" << std::endl;
      __sys->from_double_star(x,dim);
    }
   
    return status;     
  }

}

// end of mygsl::multimin.cc
