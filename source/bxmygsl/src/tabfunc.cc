// -*- mode: c++; -*-
/* mygsl::tabfunc.cc
 */

#include <mygsl/tabfunc.h>

#include <sstream>
#include <stdexcept>
#include <limits>

namespace mygsl {

  using namespace std;

  const std::string tabulated_function::LINEAR_INTERP_NAME = "linear";
  const std::string tabulated_function::POLYNOMIAL_INTERP_NAME = "polynomial";
  const std::string tabulated_function::CSPLINE_INTERP_NAME = "cspline";
  const std::string tabulated_function::CSPLINE_PERIODIC_INTERP_NAME = "cspline_periodic";
  const std::string tabulated_function::AKIMA_INTERP_NAME = "akima";
  const std::string tabulated_function::AKIMA_PERIODIC_INTERP_NAME = "akima_periodic";
  const std::string tabulated_function::DEFAULT_INTERP_NAME = tabulated_function::LINEAR_INTERP_NAME;

  bool tabulated_function::g_debug = false;
   
  bool tabulated_function::is_verbose () const
  {
    return _verbose_;
  }

  void tabulated_function::set_verbose (bool v_)
  {
    _verbose_ = v_;
    return;
  }

  bool tabulated_function::is_valid (double x_) const
  {
    return ((x_ >= _x_min_) && (x_ <= _x_max_));
  }

  const std::string & tabulated_function::interpolator_name () const
  {
    return _interpolator_name_;
  }

  const tabulated_function::points_map_t & tabulated_function::points () const 
  {
    return _points_;
  }

  const std::string & tabulated_function::default_interpolator_name ()
  {
    return DEFAULT_INTERP_NAME;
  }

  bool tabulated_function::interpolator_name_is_valid (const std::string & name_)
  {
    if (name_ == LINEAR_INTERP_NAME) return true;
    if (name_ == POLYNOMIAL_INTERP_NAME) return true;
    if (name_ == CSPLINE_INTERP_NAME) return true;
    if (name_ == CSPLINE_PERIODIC_INTERP_NAME) return true;
    if (name_ == AKIMA_INTERP_NAME) return true;
    if (name_ == AKIMA_PERIODIC_INTERP_NAME) return true;
    return false;
  }

  void tabulated_function::scale (double s_) 
  {
    for (points_map_t::iterator i = _points_.begin ();
         i != _points_.end ();
         i++) 
      {
        i->second *= s_;
      }
    return;
  }

  size_t tabulated_function::size () const 
  {
    return _points_.size ();
  }

  bool tabulated_function::is_table_locked () const
  {
    return _table_locked_;
  }

  void tabulated_function::lock_table (const std::string & interp_name_)
  {
    if (is_table_locked ()) 
      {
        unlock_table ();
      }
    if (! interp_name_.empty ()) 
      {
        if (g_debug) 
          {
            std::cerr << "DEBUG: tabulated_function::lock_table: interpolator name='" 
                      << interp_name_ << "'" << std::endl;
          }
        _interpolator_name_ = interp_name_;
      }
    size_t npoints = _points_.size ();
    if (g_debug) 
      {
        std::cerr << "DEBUG: tabulated_function::lock_table: npoints='" 
                  << npoints << "'" << std::endl;
      }
    if (npoints == 0) 
      {
        return;
      }
    bool polynomial_alert = false;
    size_t polynomial_alert_max_points = 4;
    const gsl_interp_type * git = 0;
    if (! interpolator_name_is_valid (_interpolator_name_)) 
      {
        std::ostringstream message;
        message << "tabulated_function::lock_table: "
                << "Interpolator '"
                << _interpolator_name_ << "' is not supported!";
        throw std::logic_error (message.str ());
      }
    else 
      {
        if (g_debug) 
          {
            std::cerr << "DEBUG: tabulated_function::lock_table: name='" 
                      << _interpolator_name_ << "'" << std::endl;
          }
        if (_interpolator_name_ == LINEAR_INTERP_NAME) 
          {
            git = gsl_interp_linear;
          }
        if (_interpolator_name_ == POLYNOMIAL_INTERP_NAME) 
          {
            git = gsl_interp_polynomial;
            if (npoints > polynomial_alert_max_points)
              {
                polynomial_alert = true;
              }
          }
        if (_interpolator_name_ == CSPLINE_INTERP_NAME) 
          {
            git = gsl_interp_cspline;
          }
        if (_interpolator_name_ == CSPLINE_PERIODIC_INTERP_NAME) 
          {
            git = gsl_interp_cspline_periodic;
          }
        if (_interpolator_name_ == AKIMA_INTERP_NAME) 
          {
            git = gsl_interp_akima;
          }
        if (_interpolator_name_ == AKIMA_PERIODIC_INTERP_NAME) 
          {
            git = gsl_interp_akima_periodic;
          }
      }
    /*
      std::cerr << "DEBUG: tabulated_function::lock_table: gsl_spline_alloc..."
      << std::endl;
    */
    _gs_             = gsl_spline_alloc (git, npoints);
    /*
      std::cerr << "DEBUG: tabulated_function::lock_table: gsl_spline_alloc done."
      << std::endl;
    */
    size_t min_size  = gsl_spline_min_size (_gs_);
    std::string name = gsl_spline_name (_gs_);
    if (g_debug) 
      {
        std::cerr << "DEBUG: tabulated_function::lock_table: #points=" 
                  << npoints << "" << std::endl;
        std::cerr << "DEBUG: tabulated_function::lock_table: min_size=" 
                  << min_size << "" << std::endl;
        std::cerr << "DEBUG: tabulated_function::lock_table: name='" 
                  << name << "'" << std::endl;
      }

    if (npoints < (int) min_size) 
      {
        std::ostringstream message;
        message << "tabulated_function::lock_table: "
                << "Not enough data points for '"
                << name << "' interpolator!";
        if (_gs_ != 0) gsl_spline_free (_gs_);
        _gs_ = 0;
        throw std::logic_error (message.str ());
      }
    if (polynomial_alert)
      {
        std::ostringstream message;
        message << "tabulated_function::lock_table: "
                << "Polynomial interpolation scheme uses a too large data set with more than "
                << polynomial_alert_max_points << " points!";
        if (is_verbose ()) std::clog << "WARNING: " << message.str () << endl;
      }

    _giacc_ = gsl_interp_accel_alloc ();
      
    double * x_tmp = new double[npoints];
    double * y_tmp = new double[npoints];
      
    int bin=0;
    for (points_map_t::const_iterator i = _points_.begin ();
         i != _points_.end ();
         i++) 
      {
        double x, y;
        x = i->first;
        y = i->second;
        if (_x_max_ < _x_min_) 
          {
            _x_min_ = x;
            _x_max_ = x;
          }
        else 
          {
            if (x > _x_max_) _x_max_ = x;
            if (x < _x_min_) _x_min_ = x;
          }
        x_tmp[bin] = i->first;
        y_tmp[bin] = i->second;
        bin++;
      } 
      
    gsl_spline_init (_gs_, x_tmp, y_tmp, npoints);
      
    delete[] x_tmp;
    delete[] y_tmp;
      
    _table_locked_ = true;
    return;
  }

  void tabulated_function::unlock_table ()
  {
    if (!is_table_locked ()) return;

    if (_gs_ != 0) 
      {
        gsl_interp_accel_free (_giacc_);
        gsl_spline_free (_gs_);
        _gs_    = 0;
        _giacc_ = 0;
        _x_min_ = 0.0;
        _x_max_ = -1.0;
      }
    _table_locked_ = false;
    return;
  }

  void tabulated_function::relock_table (const std::string & interp_name_)
  {
    if (is_verbose ()) 
      {
        std::clog << "WARNING: tabulated_function::relock_table:"
                  << "relock forced!" << std::endl;
      }
    unlock_table ();
    lock_table (interp_name_);
    return;
  }

  void tabulated_function::add_point (double x_, 
                                      double y_, 
                                      bool lock_after_)
  {
    bool local_debug = g_debug;
    //local_debug = false;
    if (local_debug) 
      {
        std::cerr << "DEBUG: tabulated_function::add_point: " 
                  << " (" << x_ << ", " << y_ << ")" << std::endl;
      }
    if (is_table_locked ()) unlock_table ();
    _points_[x_] = y_;
    if (lock_after_) 
      {
        if (local_debug) 
          {
            std::cerr << "DEBUG: tabulated_function::add_point: lock!"
                      << std::endl;
          }
        lock_table (_interpolator_name_);
      }
    return;
  }

  tabulated_function::tabulated_function (const std::string & interp_name_)
  {
    _table_locked_ = false;
    _gs_     = 0;
    _giacc_  = 0;
    _x_min_  = 0.0;
    _x_max_  = -1.0;
    _interpolator_name_ = interp_name_;
    return;
  }

  tabulated_function::tabulated_function (const tabulated_function & tab_func_)
  {
    _table_locked_ = false;
    _gs_     = 0;
    _giacc_  = 0;
    _x_min_  = 0.0;
    _x_max_  = -1.0;
    _interpolator_name_ = tab_func_._interpolator_name_;
    _points_ = tab_func_._points_;
    if (tab_func_.is_table_locked ()) 
      {
        lock_table ();
      }
    return;
  }

  tabulated_function & tabulated_function::operator= (const tabulated_function & tab_func_)
  {
    if (& (*this) == &tab_func_) return *this;
    reset ();
    _interpolator_name_ = tab_func_._interpolator_name_;
    _points_ = tab_func_._points_;
    if (tab_func_.is_table_locked ()) {
      lock_table ();
    }    
    return *this;
  }

  double tabulated_function::x_min () const
  {
    return _x_min_;
  }

  double tabulated_function::x_max () const
  {
    return _x_max_;
  }

  void tabulated_function::reset ()
  {
    unlock_table ();
    _points_.clear ();
    _interpolator_name_ = DEFAULT_INTERP_NAME;
    return;
  }

  tabulated_function::~tabulated_function ()
  {
    reset ();
    return;
  }

  double tabulated_function::eval (double x_) const 
  {
    if (! is_table_locked ()) 
      {
        throw std::logic_error ("tabulated_function::eval: Object not locked !");
      }
    double y = gsl_spline_eval (_gs_, x_, _giacc_);
    return y;
  }

  void tabulated_function::tabfunc_load (std::istream & in_, void * context_)
  {
    std::string interpolator_name;
    in_ >> interpolator_name >> std::ws;
    if (! interpolator_name_is_valid (interpolator_name)) 
      {
        std::ostringstream message;
        message << "tabulated_function::tabfunc_load: "
                << "Interpolator '"
                << _interpolator_name_ << "' is not supported!";
        throw std::out_of_range (message.str ());
      }
    size_t n = 0;
    in_ >> n >> std::ws;
    for (int i = 0; i < n ; i++) 
      {
        double x, y;
        in_ >> x >> std::ws >> y >> std::ws;
        if (! in_) 
          {
            throw std::logic_error (
                                    "tabulated_function::tabfunc_load: invalid format for (x, y) data point");
          }
        add_point (x, y, false);
      }
    lock_table (interpolator_name);
    return;
  }

  void tabulated_function::tabfunc_store (std::ostream & out_, void * context_) const
  {
    out_ << _interpolator_name_ << ' ';
    out_ << _points_.size () << std::endl;
    for (points_map_t::const_iterator i = _points_.begin ();
         i != _points_.end ();
         i++) 
      {
        out_ << i->first << ' ' << i->second << ' ';
      } 
    return;
  }

  void tabulated_function::print_points (std::ostream & out_, 
                                         const std::string & header_comment_, 
                                         const std::string & footer_comment_  
                                         ) const
  {
    if (! header_comment_.empty ()) 
      {
        out_ << "# " << header_comment_ << std::endl;
      }
    for (points_map_t::const_iterator i = _points_.begin ();
         i != _points_.end ();
         i++) 
      {
        out_ << i->first << ' ' << i->second << std::endl;
      } 
    if (! footer_comment_.empty ()) 
      {
        out_ << "# " << footer_comment_ << std::endl;
      }
    return;
  }


} // end of namespace mygsl

// end of mygsl::tabfunc.cc
