// geomtools/quadric.cc
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of geomtools.
//
// geomtools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// geomtools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with geomtools. If not, see <http://www.gnu.org/licenses/>.

// Ourselves:
#include <geomtools/quadric.h>

// Standard library:
#include <limits>
#include <fstream>

// Third-party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Bayeux/datatools:
#include <datatools/logger.h>
// - Bayeux/mygsl:
#include <mygsl/polynomial.h>

// This project:
#include <geomtools/point_on_quadric_finder.h>
#include <geomtools/foot_point_on_quadric_finder.h>

namespace geomtools {

  // static
  const std::string & quadric::quadric_label()
  {
    static const std::string _label("quadric");
    return _label;
  }

  std::string quadric::get_shape_name () const
  {
    return quadric_label();
  }

  bool quadric::is_valid() const
  {
    //bool all_zero = true;
    bool all_zero_AF = true;
    for (int i = 0; i < (int) NCOEFFS; i++) {
      if (std::abs(_coefficients_[i]) > 10.0 * std::numeric_limits<double>::epsilon()) {
        if (i <= A && i >= F) {
          all_zero_AF = false;
        }
        //all_zero = false;
      }
    }
    return /*!all_zero &&*/ !all_zero_AF;
  }

  void quadric::reset()
  {
    _set_defaults();
    return;
  }

  quadric::quadric()
  {
    _set_defaults();
    return;
  }

  void quadric::_set_defaults()
  {
    for (int i = 0; i < (int) NCOEFFS; i++) {
      _coefficients_[i] = 0.0;
    }
    return;
  }

  void quadric::set_coefficient(coef_index i_, double c_)
  {
    DT_THROW_IF(!datatools::is_valid(c_), std::logic_error, "Invalid quadric coefficient!");
    _coefficients_[(int) i_] = c_;
    return;
  }

  double quadric::get_coefficient(coef_index i_) const
  {
    return _coefficients_[(int) i_];
  }

  double quadric::eval_dfdx(double x_, double y_, double z_) const
  {
    double dfdx = 2.0 * ( _coefficients_[A] * x_
                          + _coefficients_[F] * y_
                          + _coefficients_[E] * z_ ) + _coefficients_[G];
    return dfdx;
  }

  double quadric::eval_dfdy(double x_, double y_, double z_) const
  {
    double dfdy = 2.0 * ( _coefficients_[F] * x_
                          + _coefficients_[B] * y_
                          + _coefficients_[D] * z_ ) + _coefficients_[H];
    return dfdy;
  }

  double quadric::eval_dfdz(double x_, double y_, double z_) const
  {
    double dfdz = 2.0 * ( _coefficients_[E] * x_
                          + _coefficients_[D] * y_
                          + _coefficients_[C] * z_ ) + _coefficients_[I];
    return dfdz;
  }

  void quadric::eval_gradient(double x_, double y_, double z_,
                              double & dfdx_, double & dfdy_, double & dfdz_) const
  {
    dfdx_ = eval_dfdx(x_, y_, z_);
    dfdy_ = eval_dfdy(x_, y_, z_);
    dfdz_ = eval_dfdz(x_, y_, z_);
   return;
  }

  void quadric::eval_gradient(const geomtools::vector_3d & position_,
                              geomtools::vector_3d & gradient_) const
  {
    double dfdx, dfdy, dfdz;
    eval_gradient(position_.x(), position_.y(), position_.z(), dfdx, dfdy, dfdz);
    gradient_.set(dfdx, dfdy, dfdz);
    return;
  }

  double quadric::eval_f(double x_, double y_, double z_) const
  {
    double res = 0.0;
    res += _coefficients_[A] * x_ * x_;
    res += _coefficients_[B] * y_ * y_;
    res += _coefficients_[C] * z_ * z_;
    res += 2.0 * _coefficients_[D] * y_ * z_;
    res += 2.0 * _coefficients_[E] * x_ * z_;
    res += 2.0 * _coefficients_[F] * x_ * y_;
    res += _coefficients_[G] * x_;
    res += _coefficients_[H] * y_;
    res += _coefficients_[I] * z_;
    res += _coefficients_[J];
    return res;
  }

  double quadric::eval_f(const geomtools::vector_3d & position_) const
  {
    return eval_f(position_.x(), position_.y(), position_.z());
  }

  double quadric::distance(const geomtools::vector_3d & position_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid quadric!");
    double d = datatools::invalid_real();
    geomtools::vector_3d proj = orthogonal_projection(position_, get_tolerance());
    if (geomtools::is_valid(proj)) {
      d = (proj - position_).mag();
    }
    return d;
  }

  geomtools::vector_3d
  quadric::orthogonal_projection(const geomtools::vector_3d & position_,
                                 double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid quadric!");
    double normal_tolerance = compute_tolerance(skin_);
    geomtools::vector_3d proj;
    geomtools::invalidate(proj);
    foot_point_on_quadric_finder fpoqf(*this, normal_tolerance);
    if (fpoqf.find(position_, proj) != EXIT_SUCCESS) {
      // std::cerr << "error: Cannot find foot point on quadric!" << std::endl;
    }
    return proj;
  }

  geomtools::vector_3d
  quadric::projection(const geomtools::vector_3d & position_,
                      const geomtools::vector_3d & direction_,
                      double skin_) const
  {
    double normal_tolerance = compute_tolerance(skin_);
    geomtools::vector_3d impact;
    geomtools::invalidate(impact);
    double x0 = position_.x();
    double y0 = position_.y();
    double z0 = position_.z();
    double xd = direction_.x();
    double yd = direction_.y();
    double zd = direction_.z();
    double a = _coefficients_[A];
    double b = _coefficients_[B];
    double c = _coefficients_[C];
    double d = _coefficients_[D];
    double e = _coefficients_[E];
    double f = _coefficients_[F];
    double g = _coefficients_[G];
    double h = _coefficients_[H];
    double i = _coefficients_[I];
    double j = _coefficients_[J];
    double Aq =
      a * xd * xd + b * yd * yd + c * zd * zd +
      2 * (f * xd * yd + e * xd * zd  + d * yd * zd);
    double Bq =
      2 * (a * xd + b * yd + c * zd
           + f * (x0 * yd + xd * y0)
           + e * (x0 * zd + xd * z0)
           + d * (y0 * zd + yd * z0))
      + g * xd + h * yd + i * zd;
    double Cq =
      a * x0 * x0 + b * y0 * y0 + c * z0 * z0
      + 2 * (f * x0 * y0 + e * x0 * z0 + d * y0 * z0)
      + g * x0 + h * y0 + i * z0 + j;
    unsigned int nsols = 0;
    double t1, t2;
    vector_3d impact1;
    vector_3d impact2;
    geomtools::invalidate(impact1);
    geomtools::invalidate(impact2);
    if (mygsl::polynomial::solve_quadratic(Cq, Bq, Aq, nsols, t1, t2)) {
      if (nsols > 0) {
        if (t1 >= 0) {
          double x1 = x0 + t1 * xd;
          double y1 = y0 + t1 * yd;
          double z1 = z0 + t1 * zd;
          impact1.set(x1, y1, z1);
          if (! is_on_surface(impact1, normal_tolerance)) {
            geomtools::invalidate(impact1);
          } else {
            impact = impact1;
          }
        } // if (t1 >= 0)
      } // if (nsols > 0)
      if (nsols > 1) {
        if (t2 >= 0) {
          double x2 = x0 + t2 * xd;
          double y2 = y0 + t2 * yd;
          double z2 = z0 + t2 * zd;
          impact2.set(x2, y2, z2);
          if (! is_on_surface(impact2, normal_tolerance)) {
            geomtools::invalidate(impact2);
          } else {
            if (geomtools::is_valid(impact)) {
              double l1 = (impact - position_).mag();
              double l2 = (impact2 - position_).mag();
              if (l2 < l1) {
                impact = impact2;
              }
            } else {
              impact = impact1;
            }
          }
        } // if (t2 >= 0)
      } // if (nsols > 1)
    } // solve_quadratic
    return impact;
  }

  // virtual
  bool quadric::is_on_surface(const geomtools::vector_3d & position_,
                              double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid quadric!");
    geomtools::vector_3d gradient;
    eval_gradient(position_, gradient);
    if (gradient.mag() < std::numeric_limits<double>::epsilon()) {
      return false;
    }
    double d = quadric::distance(position_);
    double normal_tolerance = compute_tolerance(skin_);
    double half_normal_tolerance = 0.5 * normal_tolerance;
    if (std::abs(d) > half_normal_tolerance) return false;
    return true;
  }

  // virtual
  geomtools::vector_3d
  quadric::get_normal_on_surface(const geomtools::vector_3d & position_,
                                 bool check_,
                                 double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid quadric!");
    geomtools::vector_3d n;
    geomtools::invalidate(n);
    geomtools::vector_3d gradient;
    eval_gradient(position_, gradient);
    double nmag = gradient.mag();
    if (nmag < std::numeric_limits<double>::epsilon()) {
      DT_THROW_IF(check_, std::logic_error,
                  "Position " << position_ << " is not on quadric surface!");
      return n;
    }
    DT_THROW_IF(check_ && !is_on_surface(position_, skin_),
                std::logic_error,
                "Position " << position_ << " is not on quadric surface!");
    return gradient / nmag;
  }

  // virtual
  bool quadric::find_intercept(const geomtools::vector_3d & from_,
                               const geomtools::vector_3d & direction_,
                               face_intercept_info & intercept_,
                               double tolerance_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid plane!");
    intercept_.reset();
    const geomtools::vector_3d & P = from_;
    geomtools::vector_3d I = this->projection(P, direction_, tolerance_);
    if (geomtools::is_valid(I)) {
      geomtools::vector_3d PI = I - P;
      if (PI.dot(direction_) > 0.0) {
        intercept_.grab_face_id().set_face_bit(FACE_UNIQUE);
        intercept_.set_impact(I);
      }
    }
    return intercept_.is_ok();
  }

  // static
  void quadric::make_plane(quadric & q_, double a_, double b_, double c_, double d_)
  {
    q_.reset();
    q_.set_coefficient(G, a_);
    q_.set_coefficient(H, b_);
    q_.set_coefficient(I, c_);
    q_.set_coefficient(J, d_);
    return;
  }

  // static
  void quadric::make_plane_yz(quadric & q_, double a_)
  {
    q_.reset();
    q_.set_coefficient(G, 1.0);
    q_.set_coefficient(J, -a_);
     return;
  }

  // static
  void quadric::make_plane_xz(quadric & q_, double b_)
  {
    q_.reset();
    q_.set_coefficient(H, 1.0);
    q_.set_coefficient(J, -b_);
     return;
  }

  // static
  void quadric::make_plane_xy(quadric & q_, double c_)
  {
    q_.reset();
    q_.set_coefficient(I, 1.0);
    q_.set_coefficient(J, -c_);
     return;
  }

  // static
  void quadric::make_parallel_planes_yz(quadric & q_, double a_)
  {
    q_.reset();
    q_.set_coefficient(A, 1.0);
    q_.set_coefficient(J, -a_ * a_);
    return;
  }

  // static
  void quadric::make_parallel_planes_xz(quadric & q_, double b_)
  {
    q_.reset();
    q_.set_coefficient(B, 1.0);
    q_.set_coefficient(J, -b_ * b_);
    return;
  }

  // static
  void quadric::make_parallel_planes_xy(quadric & q_, double c_)
  {
    q_.reset();
    q_.set_coefficient(C, 1.0);
    q_.set_coefficient(J, -c_ * c_);
    return;
  }

  // static
  void quadric::make_ellipsoid(quadric & q_, double a_, double b_, double c_)
  {
    q_.reset();
    q_.set_coefficient(A, 1.0 / (a_ * a_));
    q_.set_coefficient(B, 1.0 / (b_ * b_));
    q_.set_coefficient(C, 1.0 / (c_ * c_));
    q_.set_coefficient(J, -1.0);
    return;
  }

  // static
  void quadric::make_spheroid_along_z(quadric & q_, double a_, double b_)
  {
    make_ellipsoid(q_, a_, a_, b_);
    return;
  }

  // static
  void quadric::make_sphere(quadric & q_, double a_)
  {
    make_ellipsoid(q_, a_, a_, a_);
    return;
  }

  // static
  void quadric::make_elliptic_paraboloid_along_z(quadric & q_, double a_, double b_)
  {
    q_.reset();
    q_.set_coefficient(A, 1.0 / (a_ * a_));
    q_.set_coefficient(B, 1.0 / (b_ * b_));
    q_.set_coefficient(I, -1.0);
    return;
  }

  // static
  void quadric::make_circular_paraboloid_along_z(quadric & q_, double a_)
  {
    make_elliptic_paraboloid_along_z(q_, a_, a_);
    return;
  }

  // static
  void quadric::make_hyperbolic_paraboloid_along_z(quadric & q_, double a_, double b_)
  {
    q_.reset();
    q_.set_coefficient(A,  1.0 / (a_ * a_));
    q_.set_coefficient(B, -1.0 / (b_ * b_));
    q_.set_coefficient(I, -1.0);
    return;
  }

  // static
  void quadric::make_elliptic_hyperbolic_one_sheet_along_z(quadric & q_,
                                                           double a_, double b_, double c_)
  {
    q_.set_coefficient(A,  1.0 / (a_ * a_));
    q_.set_coefficient(B,  1.0 / (b_ * b_));
    q_.set_coefficient(C, -1.0 / (c_ * c_));
    q_.set_coefficient(J, -1.0);
    return;
  }

  // static
  void quadric::make_elliptic_hyperbolic_two_sheets_along_z(quadric & q_,
                                                            double a_, double b_, double c_)
  {
    q_.set_coefficient(A,  1.0 / (a_ * a_));
    q_.set_coefficient(B,  1.0 / (b_ * b_));
    q_.set_coefficient(C, -1.0 / (c_ * c_));
    q_.set_coefficient(J, +1.0);
    return;
  }

  // static
  void quadric::make_elliptic_cone_along_z(quadric & q_, double a_, double b_, double c_)
  {
    q_.reset();
    q_.set_coefficient(A,  1.0 / (a_ * a_));
    q_.set_coefficient(B,  1.0 / (b_ * b_));
    q_.set_coefficient(C, -1.0 / (c_ * c_));
    return;
  }

  // static
  void quadric::make_circular_cone_along_z(quadric & q_, double a_, double b_)
  {
    make_elliptic_cone_along_z(q_, a_, a_, b_);
    return;
  }

  // static
  void quadric::make_elliptic_cylinder_along_z(quadric & q_, double a_, double b_)
  {
    q_.reset();
    q_.set_coefficient(A,  1.0 / (a_ * a_));
    q_.set_coefficient(B,  1.0 / (b_ * b_));
    q_.set_coefficient(J, -1.0);
    return;
  }

  // static
  void quadric::make_circular_cylinder_along_z(quadric & q_, double a_)
  {
    make_elliptic_cylinder_along_z(q_, a_, a_);
    return;
  }

  // static
  void quadric::make_hyperbolic_cylinder_along_z(quadric & q_, double a_, double b_)
  {
    q_.reset();
    q_.set_coefficient(A,  1.0 / (a_ * a_));
    q_.set_coefficient(B, -1.0 / (b_ * b_));
    q_.set_coefficient(J, -1.0);
    return;
  }

  // static
  void quadric::make_parabolic_cylinder_along_z(quadric & q_, double a_)
  {
    q_.reset();
    q_.set_coefficient(A,  1.0);
    q_.set_coefficient(H,  2 * a_);
    return;
  }

  void quadric::generate_wires_self(wires_type & /*wires_*/,
                                    uint32_t /*options_*/) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid plane!");

    // NIY

    return;
  }

  // static
  std::string quadric::coeff_label(coef_index i_)
  {
    switch (i_) {
    case A : return std::string("A");
    case B : return std::string("B");
    case C : return std::string("C");
    case D : return std::string("D");
    case E : return std::string("E");
    case F : return std::string("F");
    case G : return std::string("G");
    case H : return std::string("H");
    case I : return std::string("I");
    case J : return std::string("J");
    }
  }

  void quadric::tree_dump(std::ostream & out_,
                           const std::string & title_,
                           const std::string & indent_,
                           bool inherit_) const
  {
    i_object_3d::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Coefficients : " << std::endl;
    std::set<coef_index> cindexes;
    for (int i = 0; i < (int) NCOEFFS; i++) {
      if (std::abs(_coefficients_[i]) > 10.0 * std::numeric_limits<double>::epsilon()) {
        cindexes.insert((coef_index) i);
      }
    }
    for (std::set<coef_index>::const_iterator i = cindexes.begin();
         i != cindexes.end();
         i++) {
      std::set<coef_index>::const_iterator j = i;
      out_ << indent_ << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
      if (++j == cindexes.end()) {
        out_ << datatools::i_tree_dumpable::last_tag;
      } else {
        out_ << datatools::i_tree_dumpable::tag;
      }
      out_ << coeff_label(*i) << " = " << _coefficients_[(int) *i];
      out_ << std::endl;
    }

    return;
  }

} // end of namespace geomtools
