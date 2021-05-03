/// rectangle.cc

/// Ourselves:
#include <geomtools/rectangle.h>

// Standard library:
#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <sstream>

// Third party:
// - GSL:
#include <gsl/gsl_poly.h>
// - Bayeux/datatools:
//#include <datatools/utils.h>
#include <datatools/units.h>
//#include <datatools/properties.h>

// This project:
#include <geomtools/geomtools_config.h>
#include <geomtools/placement.h>

namespace geomtools {

  using namespace std;

  // Registration :
  GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(rectangle, "geomtools::rectangle")

  const std::string & rectangle::rectangle_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "rectangle";
    }
    return label;
  }

  std::string rectangle::get_shape_name () const
  {
    return rectangle::rectangle_label();
  }

  double rectangle::get_x () const
  {
    return _x_;
  }

  double rectangle::get_y () const
  {
    return _y_;
  }

  void rectangle::set(double new_x_value_, double new_y_value_)
  {
    set_x(new_x_value_);
    set_y(new_y_value_);
    return;
  }

  void rectangle::set_x(double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0, std::domain_error, "Invalid '" << new_value_ << "' X value!");
    _x_ = new_value_;
  }

  void rectangle::set_y(double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0, std::domain_error, "Invalid '" << new_value_ << "' Y value!");
    _y_ = new_value_;
  }

  double rectangle::get_diagonal() const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid rectangle!");
    return  hypot (_x_, _y_);
  }

  double rectangle::get_surface(uint32_t /* flags_ */) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid rectangle!");
    return  _x_ * _y_;
  }

  double rectangle::get_perimeter(uint32_t /* flags_ */) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid rectangle!");
    return get_circumference();
  }

  double rectangle::get_circumference() const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid rectangle!");
    return 2 * (_x_ + _y_);
  }

  bool rectangle::is_valid() const
  {
    return datatools::is_valid(_x_) && (_x_ > 0.0)
      && datatools::is_valid(_y_) && (_y_ > 0.0);
  }

  void rectangle::_set_defaults()
  {
    datatools::invalidate(_x_);
    datatools::invalidate(_y_);
    return;
  }

  rectangle::rectangle ()
  {
    _set_defaults();
    return;
  }

  rectangle::rectangle (double x_, double y_)
  {
    _set_defaults();
    set_x (x_);
    set_y (y_);
    return;
  }

  rectangle::~rectangle ()
  {
    return;
  }

  bool rectangle::is_on_surface (const vector_3d & position_,
                                 double tolerance_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid rectangle!");
    double z_tolerance = compute_tolerance(tolerance_);
    double half_z_tolerance = 0.5 * z_tolerance;
    double z = position_.z();
    if (std::abs(z) > half_z_tolerance) {
      return false;
    }
    double x = position_.x();
    double y = position_.y();
    if (std::abs(x) > 0.5 * _x_) {
      return false;
    }
    if (std::abs(y) > 0.5 * _y_) {
      return false;
    }
    return true;
  }

  vector_3d rectangle::get_normal_on_surface (const vector_3d & position_,
                                              bool check_,
                                              double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid rectangle!");
    vector_3d normal;
    geomtools::invalidate (normal);
    DT_THROW_IF(check_ && !is_on_surface(position_, skin_), std::logic_error,
                "Position " << position_ << " is not on surface!");
    normal.set(0.0, 0.0, 1.0);
    return normal;
  }

  bool rectangle::find_intercept (const vector_3d & from_,
                                  const vector_3d & direction_,
                                  face_intercept_info & intercept_,
                                  double tolerance_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid rectangle!");
    intercept_.reset();
    double ux = direction_.x();
    double uy = direction_.y();
    double uz = direction_.z();
    double xf = from_.x();
    double yf = from_.y();
    double zf = from_.z();
    if (uz == 0.0) {
      if (zf != 0.0) {
        return intercept_.is_ok();
      }
      /*
        double p0;
        double p1;
        double p2;
        double lambda1;
        double lambda2;
        p2 = (ux * ux + uy * uy);
        p1 = -2 * (ux * xf + uy * yf);
        double r = pow (__r + 0.5 * tolerance_, 2);
        p0 = xf * xf + yf * yf - r * r;
        size_t nsol = 0;
        nsol = gsl_poly_solve_quadratic (p2, p1, p0, &lambda1, &lambda2);
        if (nsol == 1)
        {
        double xi = xf + lambda1 * ux;
        double yi = yf + lambda1 * uy;
        double zi = zf;
        intercept_.set (0, 0, vector_3d (xi, yi, zi));
        }
        else if (nsol == 2)
        {
        double lambda = -1.0;
        if (lambda1 >= 0.0)
        {
        lambda = lambda1;
        }
        if ((lambda2 >= 0.0) && (lambda2 < lambda1))
        {
        lambda = lambda2;
        }
        double xi = xf + lambda * ux;
        double yi = yf + lambda * uy;
        double zi = zf;
        intercept_.set (0, 0, vector_3d (xi, yi, zi));
        }
      */
      return intercept_.is_ok();
    }

    intercept_.reset();
    double lambda = - zf / uz;
    if (lambda < 0.0) {
      return intercept_.is_ok();
    }
    double xi, yi, zi;
    zi = 0.0;
    xi = xf + lambda * ux;
    yi = yf + lambda * uy;
    vector_3d impact(xi, yi, zi);
    if (! is_on_surface(impact, tolerance_)) {
      intercept_.is_ok();
    } else {
      // uint32_t face = FACE_UP;
      // if (from_.z () < 0.0) {
      //   face = FACE_DOWN;
      // }
      intercept_.grab_face_id().set_face_bit(FACE_UNIQUE);
      intercept_.set_impact(impact);
    }
    return intercept_.is_ok();
  }

  // virtual
  unsigned int rectangle::compute_vertexes(vertex_col_type & vertexes_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid rectangle!");
    vertexes_.clear();
    vertexes_.reserve(4);
    vertexes_.push_back(vector_3d(+0.5 * _x_, +0.5 * _y_, 0.0));
    vertexes_.push_back(vector_3d(-0.5 * _x_, +0.5 * _y_, 0.0));
    vertexes_.push_back(vector_3d(-0.5 * _x_, -0.5 * _y_, 0.0));
    vertexes_.push_back(vector_3d(+0.5 * _x_, -0.5 * _y_, 0.0));
    return vertexes_.size();
  }

  void rectangle::tree_dump (std::ostream & out_,
                             const std::string & title_,
                             const std::string & indent_,
                             bool inherit_) const
  {
    i_object_3d::tree_dump (out_, title_, indent_, true);

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "X : " << get_x () / CLHEP::mm << " mm" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag (inherit_)
         << "Y : " << get_y () / CLHEP::mm << " mm" << std::endl;

    return;
  }

  void rectangle::generate_wires_self(wires_type & wires_, uint32_t options_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid rectangle!");
    /*                             y
     *                            ^
     *                            :  y+ side
     *             -------------------------------
     *            |         |     :     |         |
     *            |         |     :     |         |
     *       x-   |         |     :     |         |
     *     side   |-------------------------------|
     *            |         |     :     |         |
     *    - - - - | - - - - | - - + - - | - - - - | - - -> x
     *            |         |     :     |         |
     *            |-------------------------------|  x+
     *            |         |     :     |         | side
     *            |         |     :     |         |
     *            |         |     :     |         |
     *             -------------------------------
     *                            : y- side
     *                            :
     */

    bool no_xminus_side = (options_ & WR_RECT_NO_XMINUS_SIDE);
    bool no_xplus_side  = (options_ & WR_RECT_NO_XPLUS_SIDE);
    bool no_yminus_side = (options_ & WR_RECT_NO_YMINUS_SIDE);
    bool no_yplus_side  = (options_ & WR_RECT_NO_YPLUS_SIDE);

    uint32_t nsegments = 0;
    uint32_t nsamples = 2;
    if (options_ & i_wires_3d_rendering::WR_BASE_GRID) {
      nsamples = i_wires_3d_rendering::linear_sampling_from_options(options_);
    }
    uint32_t nsamples_x = nsamples;
    uint32_t nsamples_y = nsamples;
    if (options_ & WR_BASE_BEST_GRID_SAMPLING) {
      double lstep =(_x_+_y_) / 3;
      int guessed_nsamples_x = (int) (_x_ / lstep);
      if (guessed_nsamples_x < 2) guessed_nsamples_x = 2;
      nsamples_x = guessed_nsamples_x;
      int guessed_nsamples_y = (int) (_y_ / lstep);
      if (guessed_nsamples_y < 2) guessed_nsamples_y = 2;
      nsamples_y = guessed_nsamples_y;
    }
    
    double dx = _x_ / (nsamples_x - 1);
    double dy = _y_ / (nsamples_y - 1);
    for (int j = AXIS_X; j <= AXIS_Y; j++) {
      double dt = dx;
      double t0 = _x_;
      double t1 = _y_;
      uint32_t ns = nsamples_x;
      if (j == AXIS_Y) {
        dt = dy;
        t0 = _y_;
        t1 = _x_;
        ns = nsamples_y;
      }
      for (uint32_t i = 0; i < ns; i++) {
        if (j == AXIS_X) {
          if (no_xminus_side && (i == 0)) continue;
          if (no_xplus_side  && (i == ns-1)) continue;
        }
        if (j == AXIS_Y) {
          if (no_yminus_side && (i == 0)) continue;
          if (no_yplus_side  && (i == ns-1)) continue;
        }
        vector_3d first;
        if (j == 0) {
          first.set(-0.5 * t0 + i * dt, -0.5 * t1, 0.0);
        } else {
          first.set(-0.5 * t1, -0.5 * t0 + i * dt, 0.0);
        }
        vector_3d last;
        if (j == 0) {
          last.set(-0.5 * t0 + i * dt, +0.5 * t1, 0.0);
        } else {
          last.set(+0.5 * t1, -0.5 * t0 + i * dt, 0.0);
        }
        {
          polyline_type dummy;
          wires_.push_back(dummy);
        }
        polyline_type & wire = wires_.back();
        wire.push_back(first);
        wire.push_back(last);
        nsegments++;
      }
    }

    return;
  }

  void rectangle::initialize(const datatools::properties & config_, const handle_dict_type * objects_)
  {
    if (!is_valid()) {
      this->i_object_3d::_initialize(config_, objects_);

      double lunit = CLHEP::mm;
      if (config_.has_key("length_unit")) {
        const std::string lunit_str = config_.fetch_string("length_unit");
        lunit = datatools::units::get_length_unit_from(lunit_str);
      }
       DT_THROW_IF(! config_.has_key("x"), std::logic_error, "Missing rectangle 'x' property !");
      double x = config_.fetch_real("x");
      if (! config_.has_explicit_unit("x")) {
        x *= lunit;
      }

      DT_THROW_IF(! config_.has_key("y"), std::logic_error, "Missing rectangle 'y' property !");
      double y = config_.fetch_real("y");
      if (! config_.has_explicit_unit("y")) {
        y *= lunit;
      }

      set(x, y);
    }

    return;
  }

  void rectangle::reset()
  {
    _set_defaults();
    this->i_object_3d::_reset();
    return;
  }

} // end of namespace geomtools
