// ellipse.cc

// Ourselves:
#include <geomtools/ellipse.h>

// Standard library:
#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <sstream>

// Third party:
// - GSL:
#include <gsl/gsl_poly.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_min.h>
#include <gsl/gsl_errno.h>
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/utils.h>
#include <datatools/units.h>
// - Bayeux/mygsl:
#include <mygsl/polynomial.h>

// This project:

namespace geomtools {

  // Registration :
  GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(ellipse, "geomtools::ellipse")

  const std::string & ellipse::ellipse_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "ellipse";
    }
    return label;
  }

  std::string ellipse::get_shape_name() const
  {
    return ellipse_label();
  }

  void ellipse::_set_defaults()
  {
    datatools::invalidate(_x_radius_);
    datatools::invalidate(_y_radius_);
    _angle_domain_.set_type(angular_range::RANGE_TYPE_AZIMUTHAL);
    _angle_domain_.reset_partial_angles();
    return;
  }

  unsigned int ellipse::get_number_of_path() const
  {
    return 1;
  }

  double ellipse::get_length(uint32_t flags_) const
  {
    if (flags_ == PATH_NONE) return 0.0;
    return get_circumference();
  }

  double ellipse::get_x_radius() const
  {
    return _x_radius_;
  }

  void ellipse::set_x_radius(double a_radius)
  {
    DT_THROW_IF (a_radius <= 0.0, std::domain_error,
                 "Invalid '" << a_radius << "' X radius value !");
    _x_radius_ = a_radius;
    return;
  }

  double ellipse::get_y_radius() const
  {
    return _y_radius_;
  }

  void ellipse::set_y_radius(double a_radius)
  {
    DT_THROW_IF (a_radius <= 0.0, std::domain_error,
                 "Invalid '" << a_radius << "' Y radius value !");
    _y_radius_ = a_radius;
    return;
  }

  void ellipse::set_radii(double x_radius_, double y_radius_)
  {
    set_x_radius(x_radius_);
    set_y_radius(y_radius_);
    return;
  }

  const angular_range & ellipse::get_angle_domain() const
  {
    return _angle_domain_;
  }

  bool ellipse::has_partial_angle() const
  {
    return _angle_domain_.is_partial();
  }

  bool ellipse::has_start_angle() const
  {
    return _angle_domain_.has_start_angle();
  }

  void ellipse::set_start_angle(double new_value_)
  {
    _angle_domain_.set_start_angle(new_value_);
    return;
  }

  double ellipse::get_start_angle() const
  {
    return _angle_domain_.get_start_angle();
  }

  bool ellipse::has_delta_angle() const
  {
    return _angle_domain_.has_delta_angle();
  }

  void ellipse::set_delta_angle(double new_value_)
  {
    _angle_domain_.set_delta_angle(new_value_);
    return;
  }

  double ellipse::get_delta_angle() const
  {
    return _angle_domain_.get_delta_angle();
  }

  double ellipse::get_circumference() const
  {
    double c;
    datatools::invalidate(c);
    // Not implemented yet:
    return c;
  }

  bool ellipse::is_valid() const
  {
    if (!datatools::is_valid(_x_radius_)) {
      return false;
    }
    if (!datatools::is_valid(_y_radius_)) {
      return false;
    }
    return _angle_domain_.is_valid();
  }

  ellipse::ellipse()
  {
    _set_defaults();
    return;
  }

  ellipse::ellipse(double xr_, double yr_)
  {
    _set_defaults();
    set_x_radius(xr_);
    set_y_radius(yr_);
    return;
  }

  ellipse::ellipse(double xr_, double yr_, double start_angle_, double delta_angle_)
  {
    _set_defaults();
    set_x_radius(xr_);
    set_y_radius(yr_);
    _angle_domain_.set_partial_angles(start_angle_, delta_angle_);
    return;
  }

  ellipse::~ellipse()
  {
    return;
  }

  void ellipse::initialize(const datatools::properties & config_, const handle_dict_type * objects_)
  {
    if (!is_valid()) {
      this->i_object_3d::_initialize(config_, objects_);

      double lunit = CLHEP::mm;
      if (config_.has_key("length_unit")) {
        const std::string lunit_str = config_.fetch_string("length_unit");
        lunit = datatools::units::get_length_unit_from(lunit_str);
      }

      DT_THROW_IF(! config_.has_key("x_radius"), std::logic_error, "Missing ellipse 'x_radius' property !");
      double x_radius = config_.fetch_real("x_radius");
      if (! config_.has_explicit_unit("x_radius")) {
        x_radius *= lunit;
      }

      DT_THROW_IF(! config_.has_key("y_radius"), std::logic_error, "Missing ellipse 'y_radius' property !");
      double y_radius = config_.fetch_real("y_radius");
      if (! config_.has_explicit_unit("y_radius")) {
        y_radius *= lunit;
      }

      set_radii(x_radius, y_radius);

      datatools::properties angle_config;
      config_.export_and_rename_starting_with(angle_config, "angle.", "");
      if (angle_config.size()) {
        _angle_domain_.initialize(angle_config);
      } else {
        // deprecated ;
        double aunit = CLHEP::degree;
        if (config_.has_key("angle_unit")) {
          const std::string aunit_str = config_.fetch_string("angle_unit");
          aunit = datatools::units::get_angle_unit_from(aunit_str);
        }

        double start_angle = 0.0;
        double delta_angle = 2 * M_PI * CLHEP::radian;
        bool not_full_angle = false;
        if (config_.has_key ("start_angle")) {
          start_angle = config_.fetch_real ("start_angle");
          if (! config_.has_explicit_unit ("start_angle")) {
            start_angle *= aunit;
          }
          not_full_angle = true;
        }
        if (config_.has_key ("delta_angle")) {
          delta_angle = config_.fetch_real ("delta_angle");
          if (! config_.has_explicit_unit ("delta_angle")) {
            delta_angle *= aunit;
          }
          not_full_angle = true;
        }
        if (not_full_angle) {
          set_start_angle(start_angle);
          set_delta_angle(delta_angle);
        }
      }
    }

    return;
  }

  void ellipse::reset()
  {
    _set_defaults();
    this->i_object_3d::reset();
    return;
  }

  void ellipse::compute_theta_from_x(double x_, double & theta_) const
  {
    datatools::invalidate(theta_);
    if (!this->is_valid()) {
      return;
    }
    double y = _y_radius_ * std::sqrt(1.0 - gsl_pow_2(x_ / _x_radius_));
    theta_ = std::atan2(y, x_);
    return;
  }

  void ellipse::compute_x_y_from_theta(double theta_, double & x_, double & y_) const
  {
    datatools::invalidate(x_);
    datatools::invalidate(y_);
    if (!this->is_valid()) {
      return;
    }
    if (has_partial_angle()) {
      if (!_angle_domain_.contains(theta_, 0.0)) {
        return;
      }
    }
    double ct = std::cos(theta_);
    double st = std::sin(theta_);

    double a = _x_radius_;
    double b = _y_radius_;
    double a2 = gsl_pow_2(a);
    double b2 = gsl_pow_2(b);
    double rho = a * b / std::sqrt(gsl_pow_2(ct) * (b2 - a2) + a2);
    x_ = rho * ct;
    y_ = rho * st;
    return;
  }

  void ellipse::tree_dump(std::ostream & out_,
                          const std::string & title_,
                          const std::string & indent_,
                          bool inherit_) const
  {
    i_object_3d::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "X radius : " << get_x_radius() / CLHEP::mm << " mm" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Y radius : " << get_y_radius() / CLHEP::mm << " mm" << std::endl;

    {
      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Angle domain : " << std::endl;
      std::ostringstream indent2;
      indent2 << indent_ << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
      _angle_domain_.tree_dump(out_, "", indent2.str());
    }
    return;
  }

  struct distance_data_type {
    double a;
    double b;
    double xm;
    double ym;
  };

  // static
  void ellipse::squared_distance_to_point_with_derivative(double t_,
                                                          void * params_,
                                                          double * f_,
                                                          double * df_)
  {
    const distance_data_type * dd = static_cast<const distance_data_type *>(params_);
    double ct = std::cos(t_);
    double st = std::sin(t_);
    double xh = dd->a * ct;
    double yh = dd->b * st;
    double xm = dd->xm;
    double ym = dd->ym;
    *f_ = gsl_pow_2(xm  - xh) + gsl_pow_2(ym  - yh);
    *df_ =
      2 * dd->a * xm * st
      + 2 * dd->b * ym * ct
      + 2 * (dd->b * dd->b - dd->a * dd->a) * ct * st;
    return;
  }

  // static
  double ellipse::squared_distance_to_point(double t_,
                                            void * params_)
  {
    const distance_data_type * dd = static_cast<const distance_data_type *>(params_);
    double ct = std::cos(t_);
    double st = std::sin(t_);
    double xh = dd->a * ct;
    double yh = dd->b * st;
    double xm = dd->xm;
    double ym = dd->ym;
    return gsl_pow_2(xm  - xh) + gsl_pow_2(ym  - yh);
  }

  double ellipse::compute_squared_distance_to_point(double t_, double xm_, double ym_) const
  {
    struct distance_data_type params;
    params.a = _x_radius_;
    params.b = _y_radius_;
    params.xm = xm_;
    params.ym = ym_;
    return squared_distance_to_point(t_, &params);
  }

  /*
    bool ellipse::compute_orthogonal_projection(const vector_3d & a_position,
    vector_3d & a_projection,
    double a_tolerance) const
    {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid ellipse!");
    bool debug = false;
    debug = true;
    if (debug) {
    std::cerr << "DEBUG: ellipse::compute_orthogonal_projection: "
    << "Entering...\n";
    }
    double tolerance = compute_tolerance(a_tolerance);
    double half_tolerance = 0.5 * tolerance;
    geomtools::invalidate(a_projection);
    double xm = a_position.x();
    double ym = a_position.y();
    double theta = std::atan2(ym, xm);
    if (theta < 0.0) {
    theta += 2 * M_PI;
    }
    double zm = a_position.z();
    double tmin;
    double tmax;
    if (xm >= 0.0 && ym >= 0.0) {
    if (debug) {
    std::cerr << "DEBUG: ellipse::compute_orthogonal_projection: "
    << "First quadrant.\n";
    }
    tmin = 0.0;
    tmax = 0.5 * M_PI;
    } else if (xm < 0.0 && ym >= 0.0) {
    if (debug) {
    std::cerr << "DEBUG: ellipse::compute_orthogonal_projection: "
    << "Second quadrant.\n";
    }
    tmin = 0.5 * M_PI;
    tmax = M_PI;
    } else if (xm < 0.0 && ym < 0.0) {
    if (debug) {
    std::cerr << "DEBUG: ellipse::compute_orthogonal_projection: "
    << "Third quadrant.\n";
    }
    tmin = M_PI;
    tmax = 1.5 * M_PI;
    } else if (xm >= 0.0 && ym < 0.0) {
    if (debug) {
    std::cerr << "DEBUG: ellipse::compute_orthogonal_projection: "
    << "Fourth quadrant.\n";
    }
    tmin = 1.5 * M_PI;
    tmax = 2.0 * M_PI;
    }
    tmin -= 1. * CLHEP::degree;
    tmax += 1. * CLHEP::degree;
    {
    // This problem implies in principle to solve a quartic equation.
    // A numerical minimization method is used for now:
    struct distance_data_type params;
    params.a = _x_radius_;
    params.b = _y_radius_;
    params.xm = xm;
    params.ym = ym;
    // gsl_function_fdf FDF;
    // FDF.fdf = &ellipse::squared_distance_to_point_with_derivative;
    // FDF.params = &params;
    gsl_function F;
    F.function = &ellipse::squared_distance_to_point;
    F.params = &params;
    double epsabs = std::min(half_tolerance / _x_radius_, half_tolerance /_y_radius_);
    epsabs /= 50;
    double epsrel = 0.0;
    double t_guess = theta; // 0.5 * (tmin + tmax);
    if (debug) {
    std::cerr.precision(15);
    std::cerr << "DEBUG: ellipse::compute_orthogonal_projection: "
    << "tmin = " << tmin << "\n";
    std::cerr << "DEBUG: ellipse::compute_orthogonal_projection: "
    << "tmax = " << tmax << "\n";
    std::cerr << "DEBUG: ellipse::compute_orthogonal_projection: "
    << "t_guess = " << t_guess << "\n";
    }
    const gsl_min_fminimizer_type * T = gsl_min_fminimizer_goldensection;
    T = gsl_min_fminimizer_brent;
    gsl_min_fminimizer * s = gsl_min_fminimizer_alloc(T);
    double t_minimum  = t_guess;
    double t_lower    = tmin;
    double t_upper    = tmax;
    double d2_minimum = this->compute_squared_distance_to_point(t_minimum, xm, ym);
    double d2_lower   = this->compute_squared_distance_to_point(t_lower, xm, ym);
    double d2_upper   = this->compute_squared_distance_to_point(t_upper, xm, ym);
    if (debug) {
    std::cerr << "DEBUG: ellipse::compute_orthogonal_projection: "
    << "Guess : \n";
    std::cerr << "DEBUG: ellipse::compute_orthogonal_projection: "
    << "    D2(t_lower)   = " << d2_lower << "\n";
    std::cerr << "DEBUG: ellipse::compute_orthogonal_projection: "
    << "    D2(t_upper)   = " << d2_upper << "\n";
    std::cerr << "DEBUG: ellipse::compute_orthogonal_projection: "
    << "    D2(t_minimum) = " << d2_minimum << "\n";
    }
    if (d2_minimum >= d2_lower || d2_minimum >= d2_upper) {
    std::cerr << "DEBUG: ellipse::compute_orthogonal_projection: "
    << "    Find a better guess range...\n";
    if (d2_minimum >= d2_lower) {
    while (d2_minimum >= d2_lower) {
    t_upper = t_minimum;
    d2_upper = d2_minimum;
    t_minimum = 0.5 * (t_minimum + t_lower);
    d2_minimum = this->compute_squared_distance_to_point(t_minimum, xm, ym);
    }
    } else if (d2_minimum >= d2_upper) {
    while (d2_minimum >= d2_upper) {
    t_lower = t_minimum;
    d2_lower = d2_minimum;
    t_minimum = 0.5 * (t_minimum + t_upper);
    d2_minimum = this->compute_squared_distance_to_point(t_minimum, xm, ym);
    }
    }
    // {
    //   if ((d2_t0 < d2_lower) && (d2_t0 < d2_upper)) {
    //     t_minimum = t_0;
    //     d2_minimum = this->compute_squared_distance_to_point(t_minimum, xm, ym);
    //   } else {
    //     DT_THROW(std::logic_error, "Cannot accept the guess!");
    //   }
    // }

    }
    t_lower -= 1.0 * CLHEP::degree;
    t_upper += 1.0 * CLHEP::degree;
    const int max_iter = 50;
    if (debug) {
    std::cerr << "DEBUG: ellipse::compute_orthogonal_projection: "
    << "Using '" << gsl_min_fminimizer_name (s) << "' method...\n";
    std::cerr << "DEBUG: ellipse::compute_orthogonal_projection: "
    << "  t_lower   = " << t_lower << "\n";
    std::cerr << "DEBUG: ellipse::compute_orthogonal_projection: "
    << "  D2(t_lower)   = " << d2_lower << "\n";
    std::cerr << "DEBUG: ellipse::compute_orthogonal_projection: "
    << "  t_upper   = " << t_upper << "\n";
    std::cerr << "DEBUG: ellipse::compute_orthogonal_projection: "
    << "  D2(t_upper)   = " << d2_upper << "\n";
    std::cerr << "DEBUG: ellipse::compute_orthogonal_projection: "
    << "  t_minimum = " << t_minimum << "\n";
    std::cerr << "DEBUG: ellipse::compute_orthogonal_projection: "
    << "  D2(t_minimum)   = " << d2_minimum << "\n";
    std::cerr << "DEBUG: ellipse::compute_orthogonal_projection: "
    << "  max_iter  = " << max_iter << "\n";
    std::cerr << "DEBUG: ellipse::compute_orthogonal_projection: "
    << "  epsabs    = " << epsabs << std::endl;
    }
    int status = GSL_CONTINUE;
    gsl_min_fminimizer_set(s, &F, t_minimum, t_lower, t_upper);
    if (debug) {
    std::cerr << "DEBUG: ellipse::compute_orthogonal_projection: "
    << "GSL minimizer is set.\n";
    }
    bool converged = false;
    int iter = 0;
    do {
    if (debug) {
    std::cerr << "DEBUG: ellipse::compute_orthogonal_projection: "
    << "Entering iteration = " << iter << std::endl;
    }
    iter++;
    status = gsl_min_fminimizer_iterate(s);
    if (status == GSL_FAILURE) {
    std::cerr << "ERROR: Minimization iteration failed!" << std::endl;
    break;
    } else if (status == GSL_EBADFUNC) {
    std::cerr << "ERROR: Singular point!" << std::endl;
    break;
    }
    double t_minimum = gsl_min_fminimizer_x_minimum(s);
    double t_upper   = gsl_min_fminimizer_x_upper(s);
    double t_lower   = gsl_min_fminimizer_x_lower(s);
    status = gsl_min_test_interval(t_lower, t_upper, epsabs, epsrel);
    if (status == GSL_SUCCESS) {
    converged = true;
    if (debug) {
    std::cerr << "DEBUG: Converged to the requested tolerance after " << iter << " iteration(s) :" << "\n";
    std::cerr << "    t_lower=" << t_lower;
    std::cerr << "    t_upper=" << t_upper;
    std::cerr << "    t_minimum=" << t_minimum << "\n";
    }
    } else if (status == GSL_CONTINUE) {
    if (debug) {
    std::cerr << "DEBUG: ellipse::compute_orthogonal_projection: "
    << "No convergence yet: continuing..." << std::endl;
    }
    } else {
    if (debug) {
    std::cerr << "ERROR: ellipse::compute_orthogonal_projection: "
    << "Error status = " << status << " at iteration " << iter << std::endl;
    }
    break;
    }
    if (converged) {
    break;
    }
    } while (status == GSL_CONTINUE && iter < max_iter);
    if (debug) {
    t_minimum = gsl_min_fminimizer_x_minimum(s);
    t_upper   = gsl_min_fminimizer_x_upper(s);
    t_lower   = gsl_min_fminimizer_x_lower(s);
    std::cerr << "DEBUG: ellipse::compute_orthogonal_projection: ";
    std::cerr << "Converged :\n";
    std::cerr << "  iter = " << iter << "\n";
    std::cerr << "  t_lower = " << t_lower << "\n";
    std::cerr << "  t_upper = " << t_upper << "\n";
    std::cerr << "  t_minimum = " << t_minimum << "\n";
    }
    gsl_min_fminimizer_free(s);
    if (converged) {
    a_projection.set(_x_radius_ * std::cos(t_minimum),
    _y_radius_ * std::sin(t_minimum),
    0.0);
    }
    }
    if (debug) {
    std::cerr << "DEBUG: ellipse::compute_orthogonal_projection: "
    << "Exiting.\n\n";
    }
    return geomtools::is_valid(a_projection);
    }
  */

  bool ellipse::is_on_curve(const vector_3d & a_position,
                            double a_tolerance) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid ellipse!");
    double tolerance = compute_tolerance(a_tolerance);
    double half_tolerance = 0.5 * tolerance;

    if (std::abs(a_position.z()) > half_tolerance) {
      return false;
    }
    if (std::abs(a_position.x()) < half_tolerance
        && std::abs(a_position.y()) < half_tolerance) {
      double ab = std::min(_x_radius_, _y_radius_);
      if (ab > half_tolerance) {
        return false;
      } else {
        return true;
      }
    }
    double x, y;
    double theta = std::atan2(a_position.y(), a_position.x());
    compute_x_y_from_theta(theta, x, y);
    vector_3d p(x, y, 0.0);
    double dist = (a_position - p).mag();
    if (dist > 0.5 * tolerance) {
      return false;
    }
    if (_angle_domain_.is_partial()) {
      double angular_tolerance = 0.0;
      if (! _angle_domain_.contains(theta, angular_tolerance)) {
        return false;
      }
    }
    return true;
  }

  vector_3d ellipse::get_direction_on_curve(const vector_3d & a_position) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid ellipse!");
    vector_3d dir;
    invalidate(dir);
    double t = std::atan2(a_position.x()/_x_radius_, a_position.y()/_y_radius_);
    double tx = - _x_radius_ * std::sin(t);
    double ty =   _y_radius_ * std::cos(t);
    dir.set(tx, ty, 0.0);
    return dir.unit();
  }

  bool ellipse::is_x_oriented() const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid ellipse!");
    return _x_radius_ > _y_radius_;
  }

  bool ellipse::is_y_oriented() const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid ellipse!");
    return _x_radius_ < _y_radius_;
  }

  bool ellipse::is_circle() const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid ellipse!");
    return _x_radius_ == _y_radius_;
  }

  double ellipse::get_excentricity() const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid ellipse!");
    if (is_x_oriented()) {
      return std::sqrt(_x_radius_ * _x_radius_ - _y_radius_ - _y_radius_) / _x_radius_;
    } else if (is_y_oriented()) {
      return std::sqrt(_y_radius_ * _y_radius_ - _x_radius_ - _x_radius_) / _y_radius_;
    }
    return 0.0;
  }

  double ellipse::get_focal_to_center() const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid ellipse!");
    if (is_x_oriented()) {
      return _x_radius_ / get_excentricity();
    } else if (is_y_oriented()) {
      return _y_radius_ / get_excentricity();
    }
    return 0.0;
  }

  void ellipse::generate_wires_self(wires_type & wires_,
                                    uint32_t options_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid ellipse!");
    uint32_t options = options_ & WR_BASE_MASK;

    uint32_t nsamples_angle = angular_sampling_from_options(options);
    {
      polyline_type dummy;
      wires_.push_back(dummy);
    }
    polyline_type & wire = wires_.back();
    // double t0 = 0.0;
    // double t1 = 2 * M_PI;
    // if (has_start_angle()) {
    //   t0 = get_start_angle();
    //   t1 = t0 + get_delta_angle();
    // }
    // double dtheta = (t1 - t0) / nsamples_angle;
    // for (size_t it = 0; it <= nsamples_angle; it++) {
    for (angular_range::iterator iter(_angle_domain_, nsamples_angle);
         !iter;
         ++iter) {
      double thetai = *iter;
      double x, y;
      compute_x_y_from_theta(thetai, x, y);
      vector_3d v(x, y, 0.0);
      wire.push_back(v);
    }
    return;
  }

} // end of namespace geomtools
