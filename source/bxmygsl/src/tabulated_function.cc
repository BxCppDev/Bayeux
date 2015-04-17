/// tabulated_function.cc

// Ourselves:
#include <mygsl/tabulated_function.h>

// Standard library:
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <limits>

// Third party:
// - GSL:
#include <gsl/gsl_interp.h>
#include <gsl/gsl_spline.h>
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/logger.h>
#include <datatools/units.h>
#include <datatools/utils.h>
#include <datatools/properties.h>

namespace mygsl {

  MYGSL_UNARY_FUNCTOR_REGISTRATION_IMPLEMENT(tabulated_function,
                                             "mygsl::tabulated_function");

  const std::string & tabulated_function::linear_interp_name()
  {
    static std::string name = "linear";
    return name;
  }

  const std::string & tabulated_function::polynomial_interp_name()
  {
    static std::string name = "polynomial";
    return name;
  }

  const std::string & tabulated_function::cspline_interp_name()
  {
    static std::string name = "cspline";
    return name;
  }

  const std::string & tabulated_function::cspline_periodic_interp_name()
  {
    static std::string name = "cspline_periodic";
    return name;
  }

  const std::string & tabulated_function::akima_interp_name()
  {
    static std::string name = "akima";
    return name;
  }

  const std::string & tabulated_function::akima_periodic_interp_name()
  {
    static std::string name = "akima_periodic";
    return name;
  }

  const std::string & tabulated_function::default_interp_name()
  {
    return linear_interp_name();
  }

  //----------------------------------------------------------------------
  // Internal implementation struct
  //
  struct tabulated_function::tabfunc_impl {
    tabfunc_impl();
    ~tabfunc_impl();
    void reset();
    bool             _verbose_;
    bool             _table_locked_;
    std::string      _interpolator_name_;
    tabulated_function::points_map_type _points_;
    gsl_spline       *_gs_;
    gsl_interp_accel *_giacc_;
    double           _x_min_;
    double           _x_max_;
    // std::vector<double> _xvalues_;
    // std::vector<double> _yvalues_;
  };

  void tabulated_function::tabfunc_impl::reset()
  {
    if (_gs_ != 0) {
      gsl_spline_free(_gs_);
      _gs_    = 0;
    }
    if (_giacc_ != 0) {
      gsl_interp_accel_free(_giacc_);
      _giacc_ = 0;
    }
    _gs_    = 0;
    _giacc_ = 0;
    _x_min_ = 0.0;
    _x_max_ = -1.0;
    _interpolator_name_.clear();
    _points_.clear();
    _verbose_ = false;
    _table_locked_ = false;
  }

  tabulated_function::tabfunc_impl::tabfunc_impl()
  {
    _verbose_ = false;
    _table_locked_ = false;
    _gs_    = 0;
    _giacc_ = 0;
    _x_min_ = 0.0;
    _x_max_ = -1.0;
    return;
  }

  tabulated_function::tabfunc_impl::~tabfunc_impl()
  {
    reset();
    return;
  }

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
    if (pImpl->_interpolator_name_.empty()) {
      pImpl->_interpolator_name_ = default_interp_name();
    }
  }

  tabulated_function::tabulated_function(const tabulated_function& tab_func_) : i_unary_function(tab_func_) {
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
    return linear_interp_name();
  }

  bool tabulated_function::interpolator_name_is_valid(const std::string& name_){
    if (name_ == linear_interp_name()) return true;
    if (name_ == polynomial_interp_name()) return true;
    if (name_ == cspline_interp_name()) return true;
    if (name_ == cspline_periodic_interp_name()) return true;
    if (name_ == akima_interp_name()) return true;
    if (name_ == akima_periodic_interp_name()) return true;
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
      pImpl->_interpolator_name_ = interp_name_;
    }
    size_t npoints = pImpl->_points_.size();

    if (npoints == 0) return;

    bool polynomial_alert = false;
    size_t polynomial_alert_max_points = 4;
    const gsl_interp_type *git = 0;
    DT_THROW_IF (!interpolator_name_is_valid(pImpl->_interpolator_name_),
                 std::logic_error, "Interpolator '"
                 << pImpl->_interpolator_name_
                 << "' is not supported !");
    if (pImpl->_interpolator_name_ == linear_interp_name()) {
      git = gsl_interp_linear;
    }
    if (pImpl->_interpolator_name_ == polynomial_interp_name()) {
      git = gsl_interp_polynomial;
      if (npoints > polynomial_alert_max_points) polynomial_alert = true;
    }
    if (pImpl->_interpolator_name_ == cspline_interp_name()) {
      git = gsl_interp_cspline;
    }
    if (pImpl->_interpolator_name_ == cspline_periodic_interp_name()) {
      git = gsl_interp_cspline_periodic;
    }
    if (pImpl->_interpolator_name_ == akima_interp_name()) {
      git = gsl_interp_akima;
    }
    if (pImpl->_interpolator_name_ == akima_periodic_interp_name()) {
      git = gsl_interp_akima_periodic;
    }

    pImpl->_gs_      = gsl_spline_alloc(git, npoints);
    size_t min_size  = gsl_spline_min_size(pImpl->_gs_);
    std::string name = gsl_spline_name(pImpl->_gs_);

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
      double x;
      // double y;
      x = i->first;
      // y = i->second;
      if (pImpl->_x_max_ < pImpl->_x_min_) {
        pImpl->_x_min_ = x;
        pImpl->_x_max_ = x;
      } else {
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
      if (pImpl->_giacc_ != 0) {
        gsl_interp_accel_free(pImpl->_giacc_);
      }
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
      DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, "Relock forced !");
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

  void tabulated_function::initialize(const datatools::properties & config_,
                                      unary_function_dict_type & /* functors_ */)
  {
    double x_unit = 1.0;
    double f_unit = 1.0;

    // XXX
    if (config_.has_key("unit.x")) {
      const std::string & x_unit_label = config_.fetch_string("unit.x");
      x_unit = datatools::units::get_unit(x_unit_label);
    }

    if (config_.has_key("unit.f")) {
      const std::string & f_unit_label = config_.fetch_string("unit.f");
      f_unit = datatools::units::get_unit(f_unit_label);
    }

    std::vector<double> xs;
    std::vector<double> fs;
    if (config_.has_key("points.x")) {
      config_.fetch("points.x", xs);
      DT_THROW_IF(xs.size() < 2, std::logic_error, "Invalid number of X points!");
      if (!config_.has_explicit_unit("points.x")) {
        for (int i = 0; i < (int) xs.size(); i++) xs[i] *= x_unit;
      }
      if (config_.has_key("points.f")) {
        config_.fetch("points.f", fs);
        DT_THROW_IF(fs.size() != xs.size(), std::logic_error, "Invalid number of F points!");
        if (!config_.has_explicit_unit("points.f")) {
          for (int i = 0; i < (int) fs.size(); i++) fs[i] *= f_unit;
        }
      }
    }

    std::string interp_scheme;
    if (config_.has_key("interpolation")) {
      interp_scheme = config_.fetch_string("interpolation");
      DT_THROW_IF(!interpolator_name_is_valid(interp_scheme),
                  std::logic_error,
                  "Invalid interpolation scheme '"
                  << interp_scheme << "'!");
    }
    if (interp_scheme.empty()) {
      interp_scheme = default_interpolator_name();
    }

    if (xs.size() > 0) {
      for (int i = 0; i < (int) xs.size(); i++) {
        add_point(xs[i], fs[i], i == ((int) xs.size() - 1));
      }
      lock_table(interp_scheme);
    }

    if (!is_table_locked()) {
      if (config_.has_key("datafile")) {
        std::string datafilename = config_.fetch_path("datafile");
        load_from_file(datafilename);
      }
    }

    DT_THROW_IF(!is_table_locked(), std::logic_error,
                "Tabulated function is not properly initialized!");
    return;
  }

  bool tabulated_function::is_initialized() const
  {
    return this->size() > 2 && is_table_locked();
  }

  void tabulated_function::reset() {
    unlock_table();
    pImpl->_points_.clear();
    pImpl->_interpolator_name_ = default_interp_name();
  }

  tabulated_function::~tabulated_function() {
    unlock_table();
    pImpl->_points_.clear();
  }

  double tabulated_function::_eval(double x_) const {
    DT_THROW_IF (!is_table_locked(), std::logic_error, "Object not locked !");
    double y = gsl_spline_eval(pImpl->_gs_, x_, pImpl->_giacc_);
    return y;
  }

  void tabulated_function::load_from_file(const std::string & filename_, uint32_t /* options_ */)
  {
    reset();
    double x_unit = 1.0;
    double f_unit = 1.0;
    std::string interpolation_scheme;
    std::ifstream fin(filename_.c_str());
    DT_THROW_IF(!fin, std::runtime_error, "Cannot open file '" << filename_ << "'!");
    while (fin && !fin.eof()) {
      std::string line;
      std::getline(fin, line);
      std::clog << "Line='" << line << "'" << std::endl;
      {
        std::istringstream tmp_iss(line);
        std::string word;
        tmp_iss >> word >> std::ws;
        if (word.empty()) {
          continue;
        } else if (word[0] == '#') {
          if (word == "#@unit.x") {
            std::string x_unit_symbol;
            tmp_iss >> x_unit_symbol >> std::ws;
            DT_THROW_IF(x_unit_symbol.empty(), std::logic_error, "Missing X unit symbol!");
            x_unit = datatools::units::get_unit(x_unit_symbol);
          } else if (word == "#@unit.f") {
            std::string f_unit_symbol;
            tmp_iss >> f_unit_symbol >> std::ws;
            DT_THROW_IF(f_unit_symbol.empty(), std::logic_error, "Missing X unit symbol!");
            f_unit = datatools::units::get_unit(f_unit_symbol);
          } else if (word == "#@interpolation") {
            tmp_iss >> interpolation_scheme >> std::ws;
            DT_THROW_IF(interpolation_scheme.empty(), std::logic_error,
                        "Missing interpolation scheme!");
          } else {
            // Skip comment
          }
          continue;
        }

        if (fin.eof()) break;
      }
      std::istringstream line_iss(line);
      double x;
      double f;
      datatools::invalidate(x);
      datatools::invalidate(f);
      line_iss >> x >> f >> std::ws;
      DT_THROW_IF(!line_iss, std::logic_error,
                  "Invalid format for (X, F(x)) points!");
      add_point(x * x_unit, f * f_unit, false);
    }
    lock_table(interpolation_scheme);
    return;
  }

  void tabulated_function::tabfunc_load(std::istream& in_, void* /*context_*/) {
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
                                         void* /*context_*/) const {
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
