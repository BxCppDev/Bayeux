// -*- mode: c++; -*-
// plane.cc

#include <geomtools/plane.h>

#include <sstream>
#include <stdexcept>

#include <datatools/utils.h>

#include <geomtools/geomtools_config.h>

namespace geomtools {

  const std::string plane::PLANE_LABEL = "plane";

  std::string plane::get_shape_name () const
  {
    return PLANE_LABEL;
  }

  bool plane::is_valid () const
  {
    return datatools::is_valid (_a_);
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

  void plane::reset ()
  {
    datatools::invalidate (_a_);
    datatools::invalidate (_b_);
    datatools::invalidate (_c_);
    datatools::invalidate (_d_);
    return;
  }

  geomtools::vector_3d plane::normal () const
  {
    geomtools::vector_3d n (_a_, _b_, _c_);
    return n.unit ();
  }

  plane::plane ()
  {
    //http://fr.wikipedia.org/wiki/Plan_%28math%C3%A9matiques%29#D.C3.A9finition_par_deux_vecteurs_et_un_point
    datatools::invalidate (_a_);
    datatools::invalidate (_b_);
    datatools::invalidate (_c_);
    datatools::invalidate (_d_);
    return;
  }

  plane::plane (double a_, double b_, double c_, double d_)
  {
    set (a_, b_, c_, d_);
    return;
  }

  void plane::set(double a_, double b_, double c_, double d_)
  {
    DT_THROW_IF (!datatools::is_normal (a_), std::logic_error, "Invalid 'a' value !");
    _a_ = a_;
    DT_THROW_IF (!datatools::is_normal (b_), std::logic_error, "Invalid 'b' value !");
    _b_ = b_;
    DT_THROW_IF (!datatools::is_normal (c_), std::logic_error, "Invalid 'c' value !");
    _c_ = c_;
    DT_THROW_IF (!datatools::is_normal (d_), std::logic_error, "Invalid 'd' value !");
    _d_ = d_;
    return;
  }



  plane::plane (const geomtools::vector_3d & ref_,
                const geomtools::vector_3d & n_)
  {
    set (ref_, n_);
    return;
  }

  void plane::set (const geomtools::vector_3d & point_,
                   const geomtools::vector_3d & n_)
  {
    DT_THROW_IF (n_.mag2 () < 1.0e-15, std::logic_error, "Invalid normal vector !");
    DT_THROW_IF (! geomtools::is_valid (point_), std::logic_error, "Invalid reference point !");
    _a_ = n_.x ();
    _b_ = n_.y ();
    _c_ = n_.z ();
    _d_ = (-n_).dot (point_);
    return;
  }

  plane::plane (const geomtools::vector_3d & point_,
                const geomtools::vector_3d & u1_,
                const geomtools::vector_3d & u2_)
  {
    set (point_,u1_,u2_);
    return;
  }

  void plane::set (const geomtools::vector_3d & point_,
            const geomtools::vector_3d & u1_,
            const geomtools::vector_3d & u2_)
  {

    DT_THROW_IF (! geomtools::is_valid (point_), std::logic_error, "Invalid reference point !");
    DT_THROW_IF (u1_.mag2 () < 1.0e-15, std::logic_error, "Invalid 'u1' vector !");
    DT_THROW_IF (u2_.mag2 () < 1.0e-15, std::logic_error, "Invalid 'u2' vector !");
    const geomtools::vector_3d normal = u1_.cross (u2_);
    DT_THROW_IF (normal.mag2() == 0.0, std::logic_error, "Vectors are colinear !");
    _a_ = normal.x ();
    _b_ = normal.y ();
    _c_ = normal.z ();
    _d_ = (-normal).dot (point_);
    return;
  }

  double plane::distance (const geomtools::vector_3d & position_) const
  {
    double d;
    d = _a_ * position_.x ()
      + _b_ * position_.y ()
      + _c_ * position_.z ()
      + _d_;
    d /= std::sqrt (_a_ * _a_ + _b_ * _b_ + _c_ * _c_);
    d = fabs(d);
    return d;
  }

  geomtools::vector_3d
  plane::orthogonal_projection (const geomtools::vector_3d & position_) const
  {
    DT_THROW_IF (! geomtools::is_valid (position_), std::logic_error, "Invalid projection initial position !");
    double t = - ( + _a_ * position_.x ()
                   + _b_ * position_.y ()
                   + _c_ * position_.z ()
                   + _d_ ) /
      (_a_ * _a_ + _b_ * _b_ + _c_ * _c_ );
    return geomtools::vector_3d (position_.x () + _a_ * t,
                                 position_.y () + _b_ * t,
                                 position_.z () + _c_ * t);
  }

  geomtools::vector_3d
  plane::projection (const geomtools::vector_3d & position_,
                     const geomtools::vector_3d & direction_) const
  {
    DT_THROW_IF (! geomtools::is_valid (position_), std::logic_error, "Invalid projection initial position !");
    DT_THROW_IF (! geomtools::is_valid (direction_), std::logic_error, "Invalid projection direction !");
    geomtools::vector_3d proj;
    double term = _a_ * position_.x ()
      + _b_ * position_.y ()
      + _c_ * position_.z ()
      + _d_;
    if (term == 0.0)
      {
        proj = position_;
      }
    else
      {
        double dotp = geomtools::vector_3d (_a_, _b_, _c_).dot (direction_);
        if (dotp != 0)
          {
            double t = - term / dotp;
            proj.set (position_.x () + direction_.x () * t,
                      position_.y () + direction_.y () * t,
                      position_.z () + direction_.z () * t);
          }
        else
          {
            geomtools::invalidate (proj);
          }
      }
    return proj;
  }

  bool plane::is_on_surface (const geomtools::vector_3d & position_,
                             double skin_) const
  {
    double d = plane::distance (position_);
    double skin = get_tolerance ();
    if (skin_ > GEOMTOOLS_PROPER_TOLERANCE) skin = skin_;
    if (std::abs (d) > 0.5 * skin) return false;
    return true;
  }

  geomtools::vector_3d
  plane::get_normal_on_surface (const geomtools::vector_3d & position_,
                                bool up_) const
  {
    if (is_on_surface (position_))
      {
      }
    return normal ();
  }

  bool plane::find_intercept (const vector_3d & from_,
                              const vector_3d & direction_,
                              intercept_t & intercept_,
                              double tolerance_) const
  {
    const geomtools::vector_3d & P = from_;
    intercept_.reset ();
    geomtools::vector_3d I = this->projection (P, direction_);
    if (geomtools::is_valid (I))
      {
        geomtools::vector_3d PI = I - P;
        if (PI.dot (direction_) > 0.0)
          {
            int face = FACE_UP;
            if (direction_.dot (normal ()) > 0)
              {
                face = FACE_DOWN;
              }
            intercept_.set (0, face, I);
          }
      }
    else
      {
      }
    return intercept_.is_ok ();
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
    if (_d_ != 0.0)
      {
        if (_c_ != 0.0)
          {
            z0 = -_d_ / _c_;
          }
        else if (_b_ != 0.0)
          {
            y0 = -_d_ / _b_;
          }
        else
          {
            x0 = -_d_ / _a_;
          }
      }
    vector_3d u1 = normal ().orthogonal ();
    u1 = u1.unit ();
    vector_3d u2 = normal ().cross (u1);
    u2 = u2.unit ();
    vector_3d R (x0, y0, z0);
    for (int i = 0; i <= 2 * n1; i++)
      {
        vector_3d Ai = R + (-n1 + i) * padding * u1 - (n2 * padding * u2);
        vector_3d Bi = R + (-n1 + i) * padding * u1 + (n2 * padding * u2);
        out_.precision (15);
        out_ << Ai.x () << ' ' << Ai.y () << ' ' << Ai.z () << std::endl;
        out_ << Bi.x () << ' ' << Bi.y () << ' ' << Bi.z () << std::endl;
        out_ << std::endl;
      }
    for (int i = 0; i <= 2 * n2; i++)
      {
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

} // end of namespace geomtools

// end of plane.cc
