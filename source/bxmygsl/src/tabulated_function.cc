// -*- mode: c++; -*-
/* tabulated_function.cc
 */

#include <mygsl/tabulated_function.h>

#include <sstream>
#include <stdexcept>
#include <limits>

#include <gsl/gsl_interp.h>
#include <gsl/gsl_spline.h>

#include <datatools/exception.h>
#include <datatools/logger.h>

namespace mygsl {

  using namespace std;

  const std::string tabulated_function::LINEAR_INTERP_NAME = "linear";
  const std::string tabulated_function::POLYNOMIAL_INTERP_NAME = "polynomial";
  const std::string tabulated_function::CSPLINE_INTERP_NAME = "cspline";
  const std::string tabulated_function::CSPLINE_PERIODIC_INTERP_NAME = "cspline_periodic";
  const std::string tabulated_function::AKIMA_INTERP_NAME = "akima";
  const std::string tabulated_function::AKIMA_PERIODIC_INTERP_NAME = "akima_periodic";
  const std::string tabulated_function::DEFAULT_INTERP_NAME = tabulated_function::LINEAR_INTERP_NAME;

  //----------------------------------------------------------------------
  // Internal implementation struct
  //
  struct tabulated_function::tabfunc_impl {
    bool             _verbose_;
    bool             _table_locked_;
    std::string      _interpolator_name_;
    tabulated_function::points_map_type _points_;
    gsl_spline       *_gs_;
    gsl_interp_accel *_giacc_;
    double           _x_min_;
    double           _x_max_;
  };

  bool tabulated_function::has_explicit_domain_of_definition() const
  {
    return true;
  }

  bool tabulated_function::is_in_domain_of_definition(double x_) const
  {
    if (x_ < x_min()) return false;
    if (x_ > x_max()) return false;
    return true;
  }

  tabulated_function::tabulated_function(const std::string& interp_name_) {
    pImpl = new tabfunc_impl;
    pImpl->_table_locked_ = false;
    pImpl->_gs_     = 0;
    pImpl->_giacc_  = 0;
    pImpl->_x_min_  = 0.0;
    pImpl->_x_max_  = -1.0;
    pImpl->_interpolator_name_ = interp_name_;
  }

  tabulated_function::tabulated_function(const tabulated_function& tab_func_) {
    pImpl = new tabfunc_impl;
    pImpl->_table_locked_ = false;
    pImpl->_gs_     = 0;
    pImpl->_giacc_  = 0;
    pImpl->_x_min_  = 0.0;
    pImpl->_x_max_  = -1.0;
    pImpl->_interpolator_name_ = tab_func_.interpolator_name();
    pImpl->_points_ = tab_func_.pImpl->_points_;
    if (tab_func_.is_table_locked()) {
      lock_table();
    }
  }

  tabulated_function& tabulated_function::operator=(const tabulated_function& tab_func_) {
    if (&(*this) == &tab_func_) return *this;
    reset();
    pImpl->_interpolator_name_ = tab_func_.interpolator_name();
    pImpl->_points_ = tab_func_.pImpl->_points_;
    if (tab_func_.is_table_locked()) {
      lock_table();
    }
    return *this;
  }



  bool tabulated_function::is_verbose() const {
    return pImpl->_verbose_;
  }

  void tabulated_function::set_verbose(bool v_) {
    pImpl->_verbose_ = v_;
  }

  bool tabulated_function::is_valid(double x_) const {
    return ((x_ >= pImpl->_x_min_) && (x_ <= pImpl->_x_max_));
  }

  const std::string& tabulated_function::interpolator_name() const {
    return pImpl->_interpolator_name_;
  }

  const tabulated_function::points_map_type& tabulated_function::points() const {
    return pImpl->_points_;
  }

  const std::string& tabulated_function::default_interpolator_name() {
    return DEFAULT_INTERP_NAME;
  }

  bool tabulated_function::interpolator_name_is_valid(const std::string& name_){
    if (name_ == LINEAR_INTERP_NAME) return true;
    if (name_ == POLYNOMIAL_INTERP_NAME) return true;
    if (name_ == CSPLINE_INTERP_NAME) return true;
    if (name_ == CSPLINE_PERIODIC_INTERP_NAME) return true;
    if (name_ == AKIMA_INTERP_NAME) return true;
    if (name_ == AKIMA_PERIODIC_INTERP_NAME) return true;
    return false;
  }

  void tabulated_function::scale(double s_) {
    for (points_map_type::iterator i = pImpl->_points_.begin();
         i != pImpl->_points_.end();
         i++) {
      i->second *= s_;
    }
  }

  size_t tabulated_function::size() const {
    return pImpl->_points_.size();
  }

  bool tabulated_function::is_table_locked() const {
    return pImpl->_table_locked_;
  }

  void tabulated_function::lock_table(const std::string& interp_name_) {
    if (is_table_locked()) unlock_table();

    if (!interp_name_.empty()) {
      // if (g_debug) {
      //   std::cerr << "DEBUG: mygsl::tabulated_function::lock_table: interpolator name='"
      //             << interp_name_ << "'" << std::endl;
      // }
      pImpl->_interpolator_name_ = interp_name_;
    }
    size_t npoints = pImpl->_points_.size();
    // if (g_debug) {
    //   std::cerr << "DEBUG: mygsl::tabulated_function::lock_table: npoints='"
    //             << npoints << "'" << std::endl;
    // }

    if (npoints == 0) return;

    bool polynomial_alert = false;
    size_t polynomial_alert_max_points = 4;
    const gsl_interp_type *git = 0;
    DT_THROW_IF (!interpolator_name_is_valid(pImpl->_interpolator_name_),
                 std::logic_error, "Interpolator '"
                 << pImpl->_interpolator_name_
                 << "' is not supported !");
    // if (g_debug) {
    //   std::cerr << "DEBUG: tabulated_function::lock_table: name='"
    //             << pImpl->_interpolator_name_ << "'" << std::endl;
    // }
    if (pImpl->_interpolator_name_ == LINEAR_INTERP_NAME) {
      git = gsl_interp_linear;
    }
    if (pImpl->_interpolator_name_ == POLYNOMIAL_INTERP_NAME) {
      git = gsl_interp_polynomial;
      if (npoints > polynomial_alert_max_points) polynomial_alert = true;
    }
    if (pImpl->_interpolator_name_ == CSPLINE_INTERP_NAME) {
      git = gsl_interp_cspline;
    }
    if (pImpl->_interpolator_name_ == CSPLINE_PERIODIC_INTERP_NAME) {
      git = gsl_interp_cspline_periodic;
    }
    if (pImpl->_interpolator_name_ == AKIMA_INTERP_NAME) {
      git = gsl_interp_akima;
    }
    if (pImpl->_interpolator_name_ == AKIMA_PERIODIC_INTERP_NAME) {
      git = gsl_interp_akima_periodic;
    }

    pImpl->_gs_      = gsl_spline_alloc(git, npoints);
    size_t min_size  = gsl_spline_min_size(pImpl->_gs_);
    std::string name = gsl_spline_name(pImpl->_gs_);

    // if (g_debug) {
    //   std::cerr << "DEBUG: tabulated_function::lock_table: #points="
    //             << npoints << "" << std::endl;
    //   std::cerr << "DEBUG: tabulated_function::lock_table: min_size="
    //             << min_size << "" << std::endl;
    //   std::cerr << "DEBUG: tabulated_function::lock_table: name='"
    //             << name << "'" << std::endl;
    // }

    if (npoints < min_size) {
      if (pImpl->_gs_ != 0) gsl_spline_free(pImpl->_gs_);
      pImpl->_gs_ = 0;
      DT_THROW_IF(true,std::logic_error,"Not enough data points for '"  << name << "' interpolator !");
    }

    if (polynomial_alert) {
      if (is_verbose()) {
        DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,
                      "Polynomial interpolation scheme uses a too large data set with more than "
                      << polynomial_alert_max_points << " points !");
      }
    }

    pImpl->_giacc_ = gsl_interp_accel_alloc();

    double *x_tmp = new double[npoints];
    double *y_tmp = new double[npoints];

    int bin(0);
    for (points_map_type::const_iterator i = pImpl->_points_.begin();
         i != pImpl->_points_.end();
         i++) {
      double x, y;
      x = i->first;
      y = i->second;
      if (pImpl->_x_max_ < pImpl->_x_min_) {
        pImpl->_x_min_ = x;
        pImpl->_x_max_ = x;
      } else
        {
          if (x > pImpl->_x_max_) pImpl->_x_max_ = x;
          if (x < pImpl->_x_min_) pImpl->_x_min_ = x;
        }
      x_tmp[bin] = i->first;
      y_tmp[bin] = i->second;
      bin++;
    }

    gsl_spline_init(pImpl->_gs_, x_tmp, y_tmp, npoints);

    delete [] x_tmp;
    delete [] y_tmp;

    pImpl->_table_locked_ = true;
  }

  void tabulated_function::unlock_table() {
    if (!is_table_locked()) return;

    if (pImpl->_gs_ != 0) {
      gsl_interp_accel_free(pImpl->_giacc_);
      gsl_spline_free(pImpl->_gs_);
      pImpl->_gs_    = 0;
      pImpl->_giacc_ = 0;
      pImpl->_x_min_ = 0.0;
      pImpl->_x_max_ = -1.0;
    }
    pImpl->_table_locked_ = false;
  }

  void tabulated_function::relock_table(const std::string& interp_name_) {
    if (is_verbose()) {
      DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE,"Relock forced !");
    }
    this->unlock_table();
    this->lock_table(interp_name_);
  }

  void tabulated_function::add_point(double x_, double y_, bool lock_after_) {
    // bool local_debug = false;
    /*
    if (local_debug) {
      std::cerr << "DEBUG: mygsl::tabulated_function::add_point: "
                << " (" << x_ << ", " << y_ << ")" << std::endl;
    }
    */
    if (is_table_locked()) unlock_table();
    (pImpl->_points_)[x_] = y_;
    if (lock_after_) {
      /*
      if (local_debug) {
        std::cerr << "DEBUG: mygsl::tabulated_function::add_point: lock!"
                  << std::endl;
      }
      */
      lock_table(pImpl->_interpolator_name_);
    }
  }

  double tabulated_function::x_min() const {
    return pImpl->_x_min_;
  }

  double tabulated_function::x_max() const {
    return pImpl->_x_max_;
  }

  void tabulated_function::reset() {
    unlock_table();
    pImpl->_points_.clear();
    pImpl->_interpolator_name_ = DEFAULT_INTERP_NAME;
  }

  tabulated_function::~tabulated_function() {
    reset();
  }

  double tabulated_function::_eval(double x_) const {
    DT_THROW_IF (!is_table_locked(), std::logic_error, "Object not locked !");
    double y = gsl_spline_eval(pImpl->_gs_, x_, pImpl->_giacc_);
    return y;
  }

  void tabulated_function::tabfunc_load(std::istream& in_, void* context_) {
    std::string interpolator_name;
    in_ >> interpolator_name >> std::ws;
    DT_THROW_IF (!interpolator_name_is_valid(interpolator_name),
                 std::out_of_range, "Interpolator '" << pImpl->_interpolator_name_ << "' is not supported !");
    size_t n = 0;
    in_ >> n >> std::ws;
    for (size_t i = 0; i < n ; ++i) {
      double x, y;
      in_ >> x >> std::ws >> y >> std::ws;
      DT_THROW_IF (!in_, std::logic_error, "Invalid format for (x, y) data point");
      add_point(x, y, false);
    }
    lock_table(interpolator_name);
  }

  void tabulated_function::tabfunc_store(std::ostream& out_,
                                         void *context_) const {
    out_ << pImpl->_interpolator_name_ << ' ';
    out_ << pImpl->_points_.size() << std::endl;
    for (points_map_type::const_iterator i = pImpl->_points_.begin();
         i != pImpl->_points_.end();
         ++i) {
      out_ << i->first << ' ' << i->second << ' ';
    }
  }

  void tabulated_function::print_points(std::ostream& out_,
                                        const std::string& header_comment_,
                                        const std::string& footer_comment_
                                        ) const {
    if (!header_comment_.empty()) {
      out_ << "# " << header_comment_ << std::endl;
    }
    for (points_map_type::const_iterator i = pImpl->_points_.begin();
         i != pImpl->_points_.end();
         ++i) {
      out_ << i->first << ' ' << i->second << std::endl;
    }
    if (!footer_comment_.empty()) {
      out_ << "# " << footer_comment_ << std::endl;
    }
  }

} // end of namespace mygsl

// end of tabulated_function.cc
