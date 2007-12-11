// -*- mode: c++; -*- 
/* mygsl::tabfunc.cc
 */

#include <mygsl/tabfunc.h>

namespace mygsl {

  const std::string tabulated_function::DEFAULT_INTERP_NAME="linear";

  bool tabulated_function::g_debug = false;

  const std::string & tabulated_function::interpolator_name() const
  {
    return __interpolator_name;
  }

  const tabulated_function::points_map_t & tabulated_function::points() const 
  {
    return __points;
  }

  const std::string & tabulated_function::default_interpolator_name()
  {
    return DEFAULT_INTERP_NAME;
  }

  bool tabulated_function::interpolator_name_is_valid( const std::string & name_ )
  {
    if ( name_ == "default" ) return true;
    if ( name_ == "linear" ) return true;
    if ( name_ == "polynomial" ) return true;
    if ( name_ == "cspline" ) return true;
    if ( name_ == "cspline_periodic" ) return true;
    if ( name_ == "akima" ) return true;
    if ( name_ == "akima_periodic" ) return true;
    return false;
  }

  void tabulated_function::scale( double s_ ) 
  {
    for ( points_map_t::iterator i = __points.begin();
	  i !=__points.end();
	  i++ ) {
      i->second*=s_;
    }
  }

  size_t tabulated_function::size() const 
  {
    return __points.size();
  }

  bool tabulated_function::is_table_locked() const
  {
    return __table_locked;
  }

  void tabulated_function::lock_table( const std::string & interp_name_ )
  {
    if ( is_table_locked() ) relock_table(interp_name_);
    if ( ! interp_name_.empty() ) {
      if ( g_debug ) {
	std::cerr << "DEBUG: tabulated_function::lock_table: interpolator name='" 
		  << interp_name_ << "'" << std::endl;
      }
      __interpolator_name = interp_name_;
    }
    size_t npoints = __points.size();
    if ( npoints == 0 ) {
      return;
    }
    const gsl_interp_type * git = 0;
    if ( ! interpolator_name_is_valid(__interpolator_name) ) {
      std::ostringstream message;
      message << "tabulated_function::lock_table: "
	      << "Interpolator '"
	      << __interpolator_name << "' is not supported!";
      throw std::runtime_error(message.str());
    }
    else {
      if ( g_debug ) {
	std::cerr << "DEBUG: tabulated_function::lock_table: name='" 
		  << __interpolator_name << "'" << std::endl;
      }
      if ( __interpolator_name == "linear" ) 
	git = gsl_interp_linear;
      if ( __interpolator_name == "polynomial" ) 
	git = gsl_interp_polynomial;
      if ( __interpolator_name == "cspline" ) 
	git = gsl_interp_cspline;
      if ( __interpolator_name == "cspline_periodic" ) 
	git = gsl_interp_cspline_periodic;
      if ( __interpolator_name == "akima" ) 
	git = gsl_interp_akima;
      if ( __interpolator_name == "akima_periodic" ) 
	git = gsl_interp_akima_periodic;
    }
    __gs             = gsl_spline_alloc(git,npoints);
    size_t min_size  = gsl_spline_min_size(__gs);
    std::string name = gsl_spline_name(__gs);
    if ( g_debug ) {
      std::cerr << "DEBUG: tabulated_function::lock_table: #points=" 
		<< npoints << "" << std::endl;
      std::cerr << "DEBUG: tabulated_function::lock_table: min_size=" 
		<< min_size << "" << std::endl;
      std::cerr << "DEBUG: tabulated_function::lock_table: name='" 
		<< name << "'" << std::endl;
    }

    if ( npoints < (int) min_size ) {
      std::ostringstream message;
      message << "tabulated_function::lock_table: "
	      << "Not enough data points for '"
	      << name << "' interpolator!";
      if ( __gs != 0 ) gsl_spline_free(__gs);
      __gs=0;
      throw std::runtime_error(message.str());
    }
      
    __giacc = gsl_interp_accel_alloc();
      
    double * x_tmp = new double[npoints];
    double * y_tmp = new double[npoints];
      
    int bin=0;
    for ( points_map_t::const_iterator i=__points.begin();
	  i!=__points.end();
	  i++ ) {
      double x,y;
      x = i->first;
      y = i->second;
      if ( __x_max < __x_min ) {
	__x_min=x;
	__x_max=x;
      }
      else {
	if ( x > __x_max ) __x_max=x;
	if ( x < __x_min ) __x_min=x;
      }
      x_tmp[bin] = i->first;
      y_tmp[bin] = i->second;
      bin++;
    } 
      
    gsl_spline_init(__gs,x_tmp,y_tmp,npoints);
      
    delete[] x_tmp;
    delete[] y_tmp;
      
    __table_locked=true;
  }

  void tabulated_function::unlock_table()
  {
    if ( !is_table_locked() ) return;

    if ( __gs != 0 ) {
      gsl_interp_accel_free(__giacc);
      gsl_spline_free(__gs);
      __gs    = 0;
      __giacc = 0;
      __x_min=0.0;
      __x_max=-1.0;
    }
    __table_locked=false;
  }

  void tabulated_function::relock_table( const std::string & interp_name_ )
  {
    std::cerr << "WARNING: tabulated_function::relock_table:"
	      << "relock forced!" << std::endl;
    unlock_table();
    lock_table(interp_name_);
  }

  void tabulated_function::add_point( double x_ , double y_ , bool lock_after_ )
  {
    if ( g_debug ) {
      std::cerr << "DEBUG: tabulated_function::add_point: " 
		<< '(' << x_ << ',' << y_ << ')' << std::endl;
    }
    if ( is_table_locked() ) unlock_table();
    __points[x_]=y_;
    if ( lock_after_ ) {
      if ( g_debug ) {
	std::cerr << "DEBUG: tabulated_function::add_point: lock!"
		  << std::endl;
      }
      lock_table(__interpolator_name);
    }
  }

  tabulated_function::tabulated_function( const std::string & interp_name_ )
  {
    __table_locked = false;
    __gs     = 0;
    __giacc  = 0;
    __x_min  = 0.0;
    __x_max  = -1.0;
    __interpolator_name = interp_name_;
  }

  tabulated_function::tabulated_function( const tabulated_function & tab_func_ )
  {
    __table_locked = false;
    __gs     = 0;
    __giacc  = 0;
    __x_min  = 0.0;
    __x_max  = -1.0;
    __interpolator_name = tab_func_.__interpolator_name;
    __points = tab_func_.__points;
    if ( tab_func_.is_table_locked() ) {
      lock_table();
    }
  }

  tabulated_function & tabulated_function::operator=( const tabulated_function & tab_func_ )
  {
    if ( &(*this) == &tab_func_ ) return *this;
    reset();
    __interpolator_name = tab_func_.__interpolator_name;
    __points = tab_func_.__points;
    if ( tab_func_.is_table_locked() ) {
      lock_table();
    }    
    return *this;
  }

  double tabulated_function::x_min() const
  {
    return __x_min;
  }

  double tabulated_function::x_max() const
  {
    return __x_max;
  }

  void tabulated_function::reset()
  {
    unlock_table();
    __points.clear();
    __interpolator_name = DEFAULT_INTERP_NAME;
  }

  tabulated_function::~tabulated_function()
  {
    reset();
  }

  bool tabulated_function::in_range( double x_ ) const
  {
    /*
      std::cerr << "DEBUG: tabulated_function::in_range: [x_min,x_max]=[" 
      << __x_min << ',' << __x_max << ']' << std::endl;
    */
    //if ( x_ < __x_min ) return false;
    //if ( x_ > __x_max ) return false;
    return true;
  }

  double tabulated_function::eval( double x_ ) const 
  {
    if ( !is_table_locked() ) {
      throw std::runtime_error(
			       "tabulated_function::eval: Object not locked!");
    }
    if ( ! in_range(x_) ) {
      throw std::out_of_range(
			      "tabulated_function::eval: Out of range value!");
    }
    double y = gsl_spline_eval(__gs,x_,__giacc);
    return y;
  }

  void tabulated_function::tabfunc_load( std::istream & in_ , void * context_ )
  {
    std::string interpolator_name;
    in_ >> interpolator_name >> std::ws;
    if ( ! interpolator_name_is_valid(interpolator_name) ) {
      std::ostringstream message;
      message << "tabulated_function::tabfunc_load: "
	      << "Interpolator '"
	      << __interpolator_name << "' is not supported!";
      throw std::out_of_range(message.str());
    }
    size_t n=0;
    in_ >> n >> std::ws;
    for ( int i=0; i<n ; i++ ) {
      double x,y;
      in_ >> x >> std::ws >> y >> std::ws;
      if ( ! in_ ) {
	throw std::runtime_error(
	  "tabulated_function::tabfunc_load: invalid format for (x,y) data point");
      }
      add_point(x,y,false);
    }
    lock_table(interpolator_name);
  }

  void tabulated_function::tabfunc_store( std::ostream & out_ , void * context_ ) const
  {
    out_ << __interpolator_name << ' ';
    out_ << __points.size() << std::endl;
    for ( points_map_t::const_iterator i=__points.begin();
	  i!=__points.end();
	  i++ ) {
      out_ << i->first << ' ' << i->second << ' ';
    } 
  }

  void tabulated_function::print_points( std::ostream & out_ , 
					 const std::string & header_comment_ ,
					 const std::string & footer_comment_  
					 ) const
  {
    if ( ! header_comment_.empty() ) {
      out_ << "# " << header_comment_ << std::endl;
    }
    for ( points_map_t::const_iterator i=__points.begin();
	  i!=__points.end();
	  i++ ) {
      out_ << i->first << ' ' << i->second << std::endl;
    } 
    if ( ! footer_comment_.empty() ) {
      out_ << "# " << footer_comment_ << std::endl;
    }
  }


} // end of namespace mygsl

// end of mygsl::tabfunc.cc
