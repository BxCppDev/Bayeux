// mygsl::multimin.cc

#include <mygsl/multimin.h>

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <string>
#include <cmath>

#include <datatools/exception.h>

namespace mygsl {

  using namespace std;

  const size_t multimin::DEFAULT_MAX_ITER = 1000;
  const bool   multimin::DEFAULT_VERBOSE  = true;

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
      DT_THROW_IF(true,std::range_error,"Value < min for parameter '" << name << "' !");
    }
  }

  void multimin_system::lock_params()
  {
    _lock_params=true;
    return;
  }

  void multimin_system::unlock_params()
  {
    _lock_params=false;
    return;
  }

  bool multimin_system::is_lock_params() const
  {
    return _lock_params==true;
  }

  multimin_system::multimin_system()
  {
    _lock_params=false;
    return;
  }

  multimin_system::~multimin_system()
  {
    _lock_params=false;
    _params.clear();
    //std::cerr << "multimin_system::~multimin_system: done." << std::endl;
    return;
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
    DT_THROW_IF ( step_ > (max_-min_),
                  std::logic_error,
                  "Step to large for parameter '" << name_ << "' !");
    pe.step  = step_;
    return pe;
  }

  void multimin_system::print_params( std::ostream & out_ ) const
  {
    for (size_t i=0; i<_params.size(); i++ ) {

      out_ << _params.at(i).name << ": "
           << _params.at(i).value << " ["
           << _params.at(i).min << ";"
           << _params.at(i).max << "] "
           << _params.at(i).step << " "
           << std::endl;
    }
    return;
  }

  void multimin_system::print_params2( std::ostream & out_ ) const
  {
    for (size_t i=0; i<_params.size(); i++ ) {
      out_ << _params.at(i).value << ' ';
    }
    return;
  }

  void multimin_system::add_param( const param_entry & pe_ )
  {
    DT_THROW_IF ( is_lock_params(), std::logic_error,
                  "System is locked !");
    DT_THROW_IF ((std::find_if (_params.begin(),
                                _params.end(),
                                multimin_system::param_has_name(pe_.name) ) != _params.end() ),
                 std::logic_error,
                 "Parameter '" << pe_.name << "' already exist !");
    _params.push_back(pe_);
    _update_dimensions_();
    return;
  }

  const multimin_system::param_entry &
  multimin_system::get_param( int i_ ) const
  {
    return _params.at(i_);
  }

  void multimin_system::set_param_value( int i_, double val_ )
  {
    _params.at(i_).set_value(val_);
    return;
  }

  void multimin_system::set_param_value_no_check( int i_, double val_ )
  {
    _params.at(i_).set_value_no_check(val_);
    return;
  }

  bool multimin_system::is_param_free( int i_ ) const
  {
    return _params.at(i_).is_free();
  }

  bool multimin_system::is_param_fixed( int i_ ) const
  {
    return _params.at(i_).is_fixed();
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
    return _params.size();
  }

  void multimin_system::_update_free_dimension_()
  {
    param_is_free pif;
    _free_dimension_=std::count_if(_params.begin(),_params.end(),pif);
    return;
  }

  void multimin_system::_update_auto_dimension_()
  {
    param_is_auto pia;
    _auto_dimension_=std::count_if(_params.begin(),_params.end(),pia);
    return;
  }

  void multimin_system::_update_dimensions_()
  {
    _update_free_dimension_();
    _update_auto_dimension_();
    return;
  }

  size_t multimin_system::get_free_dimension() const
  {
    return _free_dimension_;
  }

  size_t multimin_system::get_auto_dimension() const
  {
    return _auto_dimension_;
  }

  int multimin_system::_prepare_values()
  {
    DT_THROW_IF(true,
                std::runtime_error,
                "You should provide an inherited '_prepare_values' method in your 'multimin_system' class !");
  }

  int multimin_system::_auto_values()
  {
    DT_THROW_IF ( get_auto_dimension() > 0 ,
                  std::runtime_error,
                  "There are AUTO paramaters! "
                  << "You should provide an inherited '_auto_values' method in your 'multimin_system' class !");
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
    DT_THROW_IF ( dimension_ != fd, std::range_error, "Invalid dimension !");
    int i_free=0;
    for (size_t i_par=0; i_par<get_dimension(); i_par++ ) {
      if ( is_param_free(i_par) ) {
        pars_[i_free] = get_param_value(i_par);
        i_free++;
      }
    }
    return;
  }

  void multimin_system::from_double_star( const double * pars_ ,
                                          size_t dimension_ )
  {
    DT_THROW_IF ( dimension_ != get_free_dimension(), std::range_error,
                  "Invalid dimension !");
    int i_free=0;
    for (size_t i=0; i<get_dimension(); i++ ) {
      if ( is_param_free(i) ) {
        _params.at(i).set_value_no_check(pars_[i_free]);
        i_free++;
      }
    }
    prepare_values();
    return;
  }

  void  multimin_system::dump( std::ostream & out_ ) const
  {
    std::string tag="|-- ";
    std::string last_tag="`-- ";
    out_ << "Multimin system:" << std::endl;
    out_ << tag << "dimension: " << get_dimension() << std::endl;
    out_ << tag << "# free parameter: " << get_free_dimension() << std::endl;
    out_ << last_tag << "Parameters:" << std::endl;
    for (size_t i=0; i<_params.size(); i++ ) {
      std::string & atag=tag;
      if ( i==_params.size()-1) atag=last_tag;
      out_ << "   " << atag << "'" << get_param_name(i) << "': " << get_param_value(i)
           << " is " << (is_param_fixed(i)?"fixed":is_param_free(i)?"free":"auto")
           << " should be in [" << get_param_min(i) << ";" <<  get_param_max(i)
           << "] with step " << get_param_step(i)
           << std::endl;
    }
    return;
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
    return;
  }

  void multimin::multimin_fdf( const gsl_vector * v_ ,
                               void * params_ ,
                               double * f_ ,
                               gsl_vector * gradient_ )
  {
    multimin_system * ms = static_cast<multimin_system*>(params_);
    ms->eval_fdf(v_->data,*f_,gradient_->data);
    return;
  }

  /**********************************************************/

  multimin::default_step_action multimin::_default_step_action_;

  void multimin::set_default_step_action()
  {
    _at_step_action_ = &multimin::_default_step_action_;
    return;
  }

  void multimin::unset_step_action()
  {
    _at_step_action_ = 0;
    return;
  }

  void multimin::set_step_action( multimin::at_step_action & asd_ )
  {
    _at_step_action_ = &asd_;
    return;
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
    // if ( multimin::g_debug ) std::cerr << "Iteration: " << iter << " ("
    //           << (status==GSL_SUCCESS?"minimum found":"continue")
    //           << ')' << std::endl;
    std::cout << iter << ' ' << dim << ' ';
    for (size_t i=0; i<dim ; i++ ) {
      std::cout << x[i] << ' ';
    }
    std::cout << f << ' ' << status << ' ';
    std::cout << std::endl;
    return;
  }

  /**********************************************************/
  void multimin::print_types( std::ostream & out_ )
  {
    out_ << "conjugate_fr" << ' ';
    out_ << "conjugate_pr" << ' ';
    out_ << "vector_bfgs" << ' ';
    out_ << "steepest_descent" << ' ';
    out_ << "nmsimplex";
    return;
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
    if ( _mode_ == MODE_FDF && _fdfmin_ != 0  ) {
      return std::string(gsl_multimin_fdfminimizer_name(_fdfmin_));
    }
    if ( _mode_ == MODE_F && _fmin_ != 0 ) {
      return std::string(gsl_multimin_fminimizer_name(_fmin_));
    }
    return "";
  }

  void multimin::_init_algorithm_( const std::string & name_ )
  {
    DT_THROW_IF ( ! name_is_valid(name_), std::logic_error,
                  "Invalid minimization algorithm '" << name_ << "' !");

    if ( name_ == "conjugate_fr" ) {
      _algo_fdf_ = gsl_multimin_fdfminimizer_conjugate_fr;
      _set_mode_(MODE_FDF);
    }

    if ( name_ == "conjugate_pr" ) {
      _algo_fdf_ = gsl_multimin_fdfminimizer_conjugate_pr;
      _set_mode_(MODE_FDF);
    }

    if ( name_ == "vector_bfgs" ) {
      _algo_fdf_ = gsl_multimin_fdfminimizer_vector_bfgs;
      _set_mode_(MODE_FDF);
    }

    if ( name_ == "steepest_descent" ) {
      _algo_fdf_ = gsl_multimin_fdfminimizer_steepest_descent;
      _set_mode_(MODE_FDF);
    }

    if ( name_ == "nmsimplex" ) {
      _algo_f_ = gsl_multimin_fminimizer_nmsimplex;
      _set_mode_(MODE_F);
    }
    return;
  }

  void multimin::_set_mode_( int mode_ )
  {
    DT_THROW_IF ( mode_ != MODE_F && mode_ != MODE_FDF ,
                  std::logic_error, "Invalid mode !");
    _mode_ = mode_;
    return;
  }

  void multimin::devel_dump_x() const
  {
    if ( _x_ == 0 ) {
      std::cerr << "DEVEL: _x_==0" << std::endl;
      return;
    }
    std::cerr << "DEVEL ================================= " << std::endl;
    std::cerr << "DEVEL: "
              << " _x_.size  = " << _x_->size
              << std::endl;
    std::cerr << "DEVEL: "
              << " _x_.stride  = " << _x_->stride
              << std::endl;
    std::cerr << "DEVEL: "
              << " _x_.data  = " << _x_->data
              << std::endl;
    std::cerr << "DEVEL: "
              << " _x_.owner  = " << _x_->owner
              << std::endl;
    for (size_t i=0; i< _x_->size; i++ ) {
      std::cerr << "DEVEL: "
                << " _x_.data["<< i<< "]  = " << _x_->data[i]
                << std::endl;
    }
    std::cerr << "DEVEL: "
              << " _x_.block.size  = " << _x_->block->size
              << std::endl;
    for (size_t i=0; i< _x_->block->size; i++ ) {
      std::cerr << "DEVEL: "
                  << " _x_.block["<< i<< "]  = " << _x_->block->data[i]
                << std::endl;
    }
    std::cerr << "DEVEL ================================= " << std::endl;
    return;
  }

  void multimin::init( const std::string & name_ ,
                       multimin_system & ms_ )
  {
    _sys_ = &ms_;
    if ( ! _sys_->is_lock_params() ) {
      _sys_->lock_params();
    }

    size_t n = _sys_->get_free_dimension();
    //std::cerr << "DEVEL: multimin::init: n= " << n << std::endl;
    _init_algorithm_(name_);

    if ( _mode_ == MODE_FDF ) {
      _fdf_.f      = &multimin::multimin_f;
      _fdf_.df     = &multimin::multimin_df;
      _fdf_.fdf    = &multimin::multimin_fdf;
      _fdf_.n      = n;
      _fdf_.params = (void *) _sys_;
      _fdfmin_     = gsl_multimin_fdfminimizer_alloc(_algo_fdf_,_fdf_.n);
    }

    if ( _mode_ == MODE_F ) {
      _f_.f      = &multimin::multimin_f;
      _f_.n      = n;
      _f_.params = (void *) _sys_;
      _fmin_     = gsl_multimin_fminimizer_alloc(_algo_f_,_f_.n);
    }

    // if ( multimin::g_debug ) std::clog << "DEBUG: multimin::init: allocating ' _x_'..." << std::endl;
    _x_  = gsl_vector_alloc(n);
    gsl_vector_set_zero(_x_);
    //std::cerr << "DEVEL: multimin::init: gsl_vector_set_zero(_x_)" << std::endl;

    /*
    bool ldebug=true;
    if ( ldebug ) { devel_dump_x(); }
    */

    // if ( multimin::g_debug ) std::clog << "DEBUG: multimin::init: set initial values..." << std::endl;

    _sys_->to_double_star(_x_->data,n);
    /*
    int i_free=0;
    std::cerr << "DEVEL: multimin::init: gsl_vector_set(_x_...)" << std::endl;
    for ( int i_par=0; i_par< _sys_->get_dimension(); i_par++ ) {
      if ( _sys_->is_param_free(i_par) ) {
        gsl_vector_set(_x_,i_free,_sys_->get_param_value(i_par));
        i_free++;
      }
    }
    std::cerr << "DEVEL: multimin::init: gsl_vector_set(_x_...) done." << std::endl;
    */

    // if ( multimin::g_debug ) {
    //   for ( int i=0; i<n; i++ ) {
    //     double par=gsl_vector_get(_x_,i);
    //     std::clog << "DEBUG: multimin::init: par=" << par << std::endl;
    //   }
    // }

    if ( _mode_ == MODE_FDF ) {
      _fdf_step_size_ = 0.01;
      _fdf_tol_       = 1.e-4;
      gsl_multimin_fdfminimizer_set(_fdfmin_,&_fdf_,_x_,
                                    _fdf_step_size_,_fdf_tol_);
    }

    if ( _mode_ == MODE_F ) {
      _ss_ = gsl_vector_alloc(n);
      gsl_vector_set_zero(_ss_);
      int i_free=0;
      for (size_t i=0; i< _sys_->get_dimension(); i++ ) {
        if ( _sys_->is_param_free(i) ) {
          gsl_vector_set(_ss_,i_free,_sys_->get_param_step(i));
          i_free++;
        }
      }
      gsl_multimin_fminimizer_set(_fmin_,&_f_,_x_,_ss_);
    }

    return;
  }

  void multimin::reset()
  {
    //std::cerr << "multimin::reset: entering..." << std::endl;
    if ( _fdfmin_!=0 ) {
      gsl_multimin_fdfminimizer_free(_fdfmin_);
      _fdfmin_=0;
      //std::cerr << "multimin::reset: free _fdfmin_ done" << std::endl;
    }
    if ( _fmin_!=0 ) {
      gsl_multimin_fminimizer_free(_fmin_);
      _fmin_=0;
      //std::cerr << "multimin::reset: free _fmin_ done" << std::endl;
    }
    if ( _x_ != 0 ) {
      //std::cerr << "multimin::reset: free _x_..." << std::endl;
      //devel_dump_x();
      gsl_vector_free(_x_);
      //std::cerr << "multimin::reset: free _x_ done" << std::endl;
      _x_=0;
    }
    if ( _ss_ != 0 ) {
      gsl_vector_free(_ss_);
      _ss_=0;
      //std::cerr << "multimin::reset: free _ss_ done" << std::endl;
    }
    _algo_fdf_ = 0;
    _algo_f_   = 0;
    _fdf_step_size_ = 0.01;
    _fdf_tol_       = 1.e-4;

    _f_.f      = 0;
    _f_.n      = 0;
    _f_.params = 0;

    _fdf_.f      = 0;
    _fdf_.df     = 0;
    _fdf_.fdf    = 0;
    _fdf_.n      = 0;
    _fdf_.params = 0;

    _mode_           = MODE_F;
    _stopping_       = STOPPING_SIZE;
    _epsabs_         = 1.0;
    _at_step_action_ = 0;
    if ( _sys_ != 0 ) {
      _sys_->unlock_params();
      _sys_=0;
    }
    _n_iter_ = 0;
    _fval_   = 0.0;
    //std::cerr << "multimin::reset: done." << std::endl;
    return;
  }

  size_t multimin::get_n_iter() const
  {
    return _n_iter_;
  }

  double multimin::get_fval() const
  {
    return _fval_;
  }

  multimin::multimin()
  {
    _algo_fdf_ = 0;
    _algo_f_   = 0;
    _verbose_  = DEFAULT_VERBOSE;
    _mode_     = MODE_F;
    _fdfmin_   = 0;
    _fmin_     = 0;

    _f_.f      = 0;
    _f_.n      = 0;
    _f_.params = 0;
    _fdf_.f      = 0;
    _fdf_.df     = 0;
    _fdf_.fdf    = 0;
    _fdf_.n      = 0;
    _fdf_.params = 0;

    _x_        = 0;
    _ss_       = 0;
    _fdf_step_size_ = 0.01;
    _fdf_tol_       = 1.e-4;
    _max_iter_     = DEFAULT_MAX_ITER;
    _stopping_     = STOPPING_SIZE;
    _epsabs_         = 1.0;
    _at_step_action_ = 0;

    _n_iter_ = 0;

    return;
  }

  multimin::~multimin()
  {
    //std::cerr << "multimin::~multimin: entering..." << std::endl;
    reset();
    //std::cerr << "multimin::~multimin: done." << std::endl;
    return;
  }

  void multimin::_at_step_hook( int status_ ,
                                size_t iter_ ,
                                double * x_ ,
                                size_t   dim_ ,
                                double f_ )
  {
    // if ( multimin::g_debug ) std::clog << "DEBUG: multimin::_at_step_hook: entering..." << std::endl;
    if ( _at_step_action_ != 0 ) {
      // if ( multimin::g_debug ) std::clog << "DEBUG: multimin::_at_step_hook: _at_step_action_..." << std::endl;
      (*_at_step_action_)(status_,iter_,x_,dim_,f_);
    }
    // if ( multimin::g_debug ) std::clog << "DEBUG: multimin::_at_step_hook: exiting." << std::endl;
  }

  int multimin::minimize( double /*epsabs_*/ )
  {
    size_t iter   = 0;
    int    status = 0;
    size_t dim    = 0;
    double * x    = 0;
    _epsabs_      = 1.e-2;
    double f = 0.0;

    do {
      // if ( multimin::g_debug ) std::cerr << std::endl << "DEBUG: multimin::minimize: NEW ITERATTION" << std::endl;

      iter++;

      if ( _mode_ ==  MODE_F ) {
        // if ( multimin::g_debug ) std::cerr << "DEBUG: multimin::minimize: MODE_F" << std::endl;
        //if ( _fmin_ != 0 ) {
        dim = _f_.n;
        status=gsl_multimin_fminimizer_iterate(_fmin_);
        if ( status != 0 ) {
          break;
        }
        double size = gsl_multimin_fminimizer_size(_fmin_);
        // if ( multimin::g_debug ) std::cerr << "DEBUG: multimin::minimize: MODE_F size=" << size
                  // << " epsabs=" << _epsabs_ << std::endl;
        status=gsl_multimin_test_size(size,_epsabs_);
        x = _fmin_->x->data;
        f = _fmin_->fval;
      }

      if ( _mode_ ==  MODE_FDF ) {
        // if ( multimin::g_debug ) std::cerr << "DEBUG: multimin::minimize: MODE_FDF" << std::endl;
        //if ( _fdfmin_ != 0 ) {
        dim = _fdf_.n;
        status=gsl_multimin_fdfminimizer_iterate(_fdfmin_);
        if ( status != 0 ) {
          break;
        }
        status=gsl_multimin_test_gradient(_fdfmin_->gradient,_epsabs_);
        x = _fdfmin_->x->data;
        f = _fdfmin_->f;
      }

      /*
      if ( _verbose_ ) {
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
          if ( _mode_ ==  MODE_F ) {
            tmp = _fmin_->x;
            //std::clog << "TEST 0a i=" << i << std::endl;
          }
          if ( _mode_ ==  MODE_FDF ) {
            tmp = _fdfmin_->x;
          }
          //std::clog << "Par[" << i << "] = "
          //    << gsl_vector_get(tmp,i) << std::endl;
        }
      }
      */
      _at_step_hook(status,iter,x,dim,f);
      _fval_=f;

    } while ( status == GSL_CONTINUE && iter < _max_iter_);

    _n_iter_=iter;
    if ( status == GSL_SUCCESS ) {
      // if ( multimin::g_debug ) std::clog << "multimin::minimize: END" << std::endl;
      _sys_->from_double_star(x,dim);
    }

    return status;
  }

}

// end of mygsl::multimin.cc
