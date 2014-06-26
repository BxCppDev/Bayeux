/// regular_polygon.cc

// Ourselves:
#include <geomtools/regular_polygon.h>

// Standard library:
#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>

// - GSL:
#include <gsl/gsl_poly.h>

// This project:
#include <geomtools/i_shape_2d.h>
#include <geomtools/utils.h>

namespace geomtools {

  using namespace std;

  const std::string & regular_polygon::regular_polygon_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "regular_polygon";
    }
    return label;
  }

  string regular_polygon::get_shape_name() const
  {
    return regular_polygon_label();
  }

  uint32_t regular_polygon::get_n_sides() const
  {
    return _n_sides_;
  }

  void regular_polygon::set_n_sides(uint32_t ns_)
  {
    DT_THROW_IF(ns_ < 3, std::logic_error, "Invalid number of sides !");
    _n_sides_ = ns_;
    return;
  }

  double regular_polygon::get_r() const
  {
    return _r_;
  }

  double regular_polygon::get_radius() const
  {
    return get_r ();
  }

  void regular_polygon::set_diameter(double new_value_)
  {
    set_r (new_value_ * 0.5);
    return;
  }

  double regular_polygon::get_diameter() const
  {
    return (_r_ + _r_);
  }

  void regular_polygon::set_r(double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0, std::logic_error, "Invalid '" << new_value_ << "' R value!");
    _r_ = new_value_;
    return;
  }


  double regular_polygon::get_apothem() const
  {
    double a = _r_ * std::cos(M_PI / _n_sides_);
    return a;
  }

  double regular_polygon::get_side_length() const
  {
    double sl = 2 * _r_ * std::sin(M_PI / _n_sides_);
    return sl;
  }

  double regular_polygon::get_perimeter(uint32_t /* flags_ */) const
  {
    return get_side_length() * _n_sides_;
  }

  double regular_polygon::get_surface(uint32_t /* flags_ */) const
  {
    return get_apothem() * get_side_length() * _n_sides_ * 0.5;
  }

  bool regular_polygon::is_valid() const
  {
    return (_r_ > 0.0);
  }

  regular_polygon::regular_polygon()
  {
    _r_ = -1.0;
    return;
  }

  regular_polygon::regular_polygon(uint32_t n_sides_,
                                   double r_,
                                   int mode_)
  {
    double r = r_;
    set_n_sides(n_sides_);
    if (mode_ == BUILD_BY_APOTHEM) {
      r = r_ / std::cos(M_PI / n_sides_);
    }
    set_r(r);
    return;
  }

  regular_polygon::~regular_polygon()
  {
    return;
  }

  double regular_polygon::get_reference_angle() const
  {
    return 2 * M_PI / _n_sides_;
  }

  void regular_polygon::get_vertex(int i_, double & x_, double & y_) const
  {
    double alpha = get_reference_angle();
    double theta = (0.5 * alpha) + i_ * alpha;
    x_ = _r_ * std::cos(theta);
    y_ = _r_ * std::sin(theta);
    return;
  }

  void regular_polygon::get_vertex(int n_, vector_3d & vertex_) const
  {
    double x, y;
    get_vertex(n_, x, y);
    vertex_.set(x, y, 0.0);
    return;
  }

  bool regular_polygon::is_on_surface(const vector_3d & /*position_*/,
                                      double /*tolerance_*/) const
  {
    DT_THROW_IF(true, std::logic_error, "Not implemented yet !");
    /*
      double tolerance = get_tolerance ();
      if (tolerance_ > GEOMTOOLS_PROPER_TOLERANCE) tolerance = tolerance_;
      double z = position_.z ();
      if (abs (z) > 0.5 * tolerance) return false;
      double x = position_.x ();
      double y = position_.y ();
      double r2 = (_r_ + 0.5 * tolerance) * (_r_ + 0.5 * tolerance);
      double rho2 = x * x + y * y;
      if (rho2 > r2)
      {
      return false;
      }
    */
    return true;
  }

  vector_3d regular_polygon::get_normal_on_surface(const vector_3d & position_,
                                                   bool up_) const
  {
    vector_3d normal;
    geomtools::invalidate(normal);
    if (is_on_surface(position_)) {
      normal.set(0.0, 0.0, 1.0);
      if (! up_) normal.setZ(-1.0);
    }
    return normal;
  }

  bool regular_polygon::find_intercept(const vector_3d & /*from_*/,
                                       const vector_3d & /*direction_*/,
                                       intercept_t & intercept_,
                                       double /*tolerance_*/) const
  {
    intercept_.reset();
    DT_THROW_IF(true, std::logic_error, "Not implemented yet !");
    /*
      double ux = direction_.x();
      double uy = direction_.y();
      double uz = direction_.z();
      double xf = from_.x();
      double yf = from_.y();
      double zf = from_.z();
      if (uz == 0.0)
      {
      intercept_.reset ();
      if (zf != 0.0)
      {
      return intercept_.is_ok ();
      }
      double p0;
      double p1;
      double p2;
      double lambda1;
      double lambda2;
      p2 = (ux * ux + uy * uy);
      p1 = -2 * (ux * xf + uy * yf);
      double r = pow (_r_ + 0.5 * tolerance_, 2);
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
      return intercept_.is_ok ();
      }

      intercept_.reset ();
      double lambda = - zf / uz;
      if (lambda < 0.0)
      {
      return intercept_.is_ok ();
      }
      double xi, yi, zi;
      zi = 0.0;
      xi = xf + lambda * ux;
      yi = yf + lambda * uy;
      vector_3d impact (xi, yi, zi);
      if (! is_on_surface (impact, tolerance_))
      {
      intercept_.is_ok ();
      }
      else
      {
      int face = FACE_UP;
      if (from_.z ()  < 0.0)
      {
      face = FACE_DOWN;
      }
      intercept_.set (0, face, vector_3d (xi, yi, zi));
      }
    */
    return intercept_.is_ok();
  }

  void regular_polygon::tree_dump(std::ostream & out_,
                                  const std::string & title_,
                                  const std::string & indent_,
                                  bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty()) indent = indent_;
    i_object_3d::tree_dump(out_, title_, indent_, true);

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Number of sides : " << get_n_sides() << endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "R : " << get_r() / CLHEP::mm << " mm" << endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Side length : " << get_side_length() / CLHEP::mm << " mm" << endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Angle : " << get_reference_angle() / CLHEP::degree << " degree" << endl;
    out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Apothem : " << get_apothem() / CLHEP::mm << " mm" << endl;
    return;
  }

} // end of namespace geomtools
