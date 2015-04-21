// plane.cc

// Ourselves:
#include <geomtools/plane.h>

// Standard library:
#include <sstream>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>

// This project:
#include <geomtools/geomtools_config.h>

namespace geomtools {

  const std::string & plane::plane_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "plane";
    }
    return label;
  }

  std::string plane::get_shape_name () const
  {
    return plane_label();
  }

  bool plane::is_valid () const
  {
    return datatools::is_valid(_a_);
  }

  double plane::a () const
  {
    return _a_;
  }

  double plane::b () const
  {
    return _b_;
  }

  double plane::c () const
  {
    return _c_;
  }

  double plane::d () const
  {
    return _d_;
  }

  void plane::_set_defaults()
  {
    datatools::invalidate(_a_);
    datatools::invalidate(_b_);
    datatools::invalidate(_c_);
    datatools::invalidate(_d_);
    return;
  }

  void plane::reset ()
  {
    _set_defaults();
    return;
  }

  geomtools::vector_3d plane::normal () const
  {
    geomtools::vector_3d n(_a_, _b_, _c_);
    return n.unit();
  }

  plane::plane ()
  {
    //http://fr.wikipedia.org/wiki/Plan_%28math%C3%A9matiques%29#D.C3.A9finition_par_deux_vecteurs_et_un_point
    _set_defaults();
    return;
  }

  plane::plane (double a_, double b_, double c_, double d_)
  {
    set (a_, b_, c_, d_);
    return;
  }

  void plane::set(double a_, double b_, double c_, double d_)
  {
    DT_THROW_IF (!datatools::is_normal(a_), std::logic_error, "Invalid 'a' value !");
    _a_ = a_;
    DT_THROW_IF (!datatools::is_normal(b_), std::logic_error, "Invalid 'b' value !");
    _b_ = b_;
    DT_THROW_IF (!datatools::is_normal(c_), std::logic_error, "Invalid 'c' value !");
    _c_ = c_;
    DT_THROW_IF (!datatools::is_normal(d_), std::logic_error, "Invalid 'd' value !");
    _d_ = d_;
    return;
  }



  plane::plane (const geomtools::vector_3d & point_,
                const geomtools::vector_3d & normal_)
  {
    set (point_, normal_);
    return;
  }

  void plane::set (const geomtools::vector_3d & point_,
                   const geomtools::vector_3d & normal_)
  {
    DT_THROW_IF (normal_.mag2() < 1.0e-15, std::logic_error, "Invalid normal vector !");
    DT_THROW_IF (! geomtools::is_valid(point_), std::logic_error, "Invalid reference point !");
    _a_ = normal_.x();
    _b_ = normal_.y();
    _c_ = normal_.z();
    _d_ = (-normal_).dot(point_);
    return;
  }

  plane::plane (const geomtools::vector_3d & point_,
                const geomtools::vector_3d & u1_,
                const geomtools::vector_3d & u2_)
  {
    set(point_, u1_, u2_);
    return;
  }

  void plane::set (const geomtools::vector_3d & point_,
                   const geomtools::vector_3d & u1_,
                   const geomtools::vector_3d & u2_)
  {
    DT_THROW_IF (! geomtools::is_valid(point_), std::logic_error, "Invalid reference point !");
    DT_THROW_IF (u1_.mag2() < 1.0e-15, std::logic_error, "Invalid 'u1' vector !");
    DT_THROW_IF (u2_.mag2() < 1.0e-15, std::logic_error, "Invalid 'u2' vector !");
    geomtools::vector_3d u1 = u1_.unit();
    geomtools::vector_3d u2 = u2_.unit();
    const geomtools::vector_3d normal = u1.cross(u2);
    DT_THROW_IF (normal.mag2() < 1.0e-15, std::logic_error, "Vectors are colinear !");
    _a_ = normal.x();
    _b_ = normal.y();
    _c_ = normal.z();
    _d_ = (-normal).dot(point_);
    return;
  }

  double plane::distance(const geomtools::vector_3d & position_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid plane!");
    double d = _a_ * position_.x ()
      + _b_ * position_.y ()
      + _c_ * position_.z ()
      + _d_;
    d /= std::sqrt(_a_ * _a_ + _b_ * _b_ + _c_ * _c_);
    return std::abs(d);
  }

  geomtools::vector_3d
  plane::orthogonal_projection (const geomtools::vector_3d & position_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid plane!");
    DT_THROW_IF (! geomtools::is_valid (position_), std::logic_error, "Invalid projection initial position !");
    double t = - ( + _a_ * position_.x ()
                   + _b_ * position_.y ()
                   + _c_ * position_.z ()
                   + _d_ ) / (_a_ * _a_ + _b_ * _b_ + _c_ * _c_ );
    return geomtools::vector_3d (position_.x () + _a_ * t,
                                 position_.y () + _b_ * t,
                                 position_.z () + _c_ * t);
  }

  geomtools::vector_3d
  plane::projection (const geomtools::vector_3d & position_,
                     const geomtools::vector_3d & direction_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid plane!");
    DT_THROW_IF (! geomtools::is_valid (position_), std::logic_error, "Invalid projection initial position !");
    DT_THROW_IF (! geomtools::is_valid (direction_), std::logic_error, "Invalid projection direction !");
    geomtools::vector_3d proj;
    geomtools::invalidate(proj);
    double term = _a_ * position_.x ()
      + _b_ * position_.y ()
      + _c_ * position_.z ()
      + _d_;
    if (term == 0.0) {
      proj = position_;
    } else  {
      double dotp = geomtools::vector_3d (_a_, _b_, _c_).dot (direction_);
      if (dotp != 0) {
        double t = - term / dotp;
        proj.set (position_.x () + direction_.x () * t,
                  position_.y () + direction_.y () * t,
                  position_.z () + direction_.z () * t);
      }
    }
    return proj;
  }

  bool plane::is_on_surface (const geomtools::vector_3d & position_,
                             double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid plane!");
    double d = plane::distance(position_);
    double normal_tolerance = compute_tolerance(skin_);
    double half_normal_tolerance = 0.5 * normal_tolerance;
    if (std::abs(d) > half_normal_tolerance) return false;
    return true;
  }

  geomtools::vector_3d
  plane::get_normal_on_surface (const geomtools::vector_3d & position_,
                                bool check_,
                                double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid plane!");
    geomtools::vector_3d n;
    geomtools::invalidate(n);
    DT_THROW_IF(check_ && !is_on_surface(position_, skin_), std::logic_error,
                "Position " << position_ << " is not on surface!");
    n = normal();
    return n;
  }

  bool plane::find_intercept (const vector_3d & from_,
                              const vector_3d & direction_,
                              face_intercept_info & intercept_,
                              double /* tolerance_ */) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid plane!");
    intercept_.reset();
    const geomtools::vector_3d & P = from_;
    geomtools::vector_3d I = this->projection(P, direction_);
    if (geomtools::is_valid(I)) {
      // std::cerr << "DEVEL: plane::find_intercept: Projection is valid." << std::endl;
      geomtools::vector_3d PI = I - P;
      if (PI.dot(direction_) > 0.0) {
        // std::cerr << "DEVEL: plane::find_intercept: Candidate intercept is valid." << std::endl;
        // uint32_t face = FACE_UP;
        // if (direction_.dot(normal ()) > 0) {
        //   face = FACE_DOWN;
        // }
        intercept_.grab_face_id().set_face_bit(FACE_UNIQUE);
        // std::cerr << "DEVEL: plane::find_intercept: Candidate intercept face bit is set." << std::endl;
        intercept_.set_impact(I);
        // std::cerr << "DEVEL: plane::find_intercept: Candidate intercept is built." << std::endl;
        // intercept_.print(std::cerr, "DEVEL: ");
      }
    }
    return intercept_.is_ok();
  }

  void plane::print_grid (std::ostream & out_, double padding_,
                          int n1_, int n2_) const
  {
    int n1 = n1_;
    int n2 = n2_;
    double padding = padding_;
    if (n1 <= 0) n1 = 6;
    if (n2 <= 0) n2 = 6;
    if (padding <= 0.0) padding = 1.0;
    double x0 = 0.0;
    double y0 = 0.0;
    double z0 = 0.0;
    if (_d_ != 0.0) {
        if (_c_ != 0.0) {
          z0 = -_d_ / _c_;
        } else if (_b_ != 0.0) {
          y0 = -_d_ / _b_;
        } else {
          x0 = -_d_ / _a_;
        }
    }
    vector_3d u1 = normal ().orthogonal ();
    u1 = u1.unit ();
    vector_3d u2 = normal ().cross (u1);
    u2 = u2.unit ();
    vector_3d R (x0, y0, z0);
    for (int i = 0; i <= 2 * n1; i++) {
      vector_3d Ai = R + (-n1 + i) * padding * u1 - (n2 * padding * u2);
      vector_3d Bi = R + (-n1 + i) * padding * u1 + (n2 * padding * u2);
      out_.precision (15);
      out_ << Ai.x () << ' ' << Ai.y () << ' ' << Ai.z () << std::endl;
      out_ << Bi.x () << ' ' << Bi.y () << ' ' << Bi.z () << std::endl;
      out_ << std::endl;
    }
    for (int i = 0; i <= 2 * n2; i++) {
      vector_3d Ai = R + (-n2 + i) * padding * u2 - (n1 * padding * u1);
      vector_3d Bi = R + (-n2 + i) * padding * u2 + (n1 * padding * u1);
      out_.precision (15);
      out_ << Ai.x () << ' ' << Ai.y () << ' ' << Ai.z () << std::endl;
      out_ << Bi.x () << ' ' << Bi.y () << ' ' << Bi.z () << std::endl;
      out_ << std::endl;
    }
    out_ << std::endl;
    return;
  }

  void plane::generate_wires_self(wires_type & /*wires_*/,
                                  uint32_t /*options_*/) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid plane!");

    // NIY

    return;
  }

} // end of namespace geomtools
