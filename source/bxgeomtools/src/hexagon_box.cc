// -*- mode: c++; -*-
/* hexagon_box.cc
 */

#include <geomtools/hexagon_box.h>

namespace geomtools {

  using namespace std;

  const std::string & hexagon_box::hexagon_box_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "hexagon_box";
    }
    return label;
  }

  double
  hexagon_box::get_radius () const
  {
    return _radius_;
  }

  double
  hexagon_box::get_diameter () const
  {
    return 2. * get_radius ();
  }

  double
  hexagon_box::get_side () const
  {
    return get_radius ();
  }

  void
  hexagon_box::set_radius (double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0, std::logic_error, "Invalid '" << new_value_ << "' radius value!");
    _radius_ = new_value_;
  }

  double
  hexagon_box::get_z () const
  {
    return _z_;
  }

  void
  hexagon_box::set_z (double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0, std::logic_error, "Invalid '" << new_value_ << "' Z value!");
    _z_ = new_value_;
  }

  double
  hexagon_box::get_half_z () const
  {
    return _z_ * 0.5;
  }

  void
  hexagon_box::set_half_z (double new_value_)
  {
    set_z (new_value_ + new_value_);
  }

  void
  hexagon_box::set (double radius_, double z_)
  {
    set_radius (radius_);
    set_z (z_);
  }

  // ctor/dtor:
  hexagon_box::hexagon_box ()
  {
    reset ();
  }

  hexagon_box::hexagon_box (double radius_, double z_)
  {
    set_radius (radius_);
    set_z (z_);
  }

  hexagon_box::~hexagon_box ()
  {
  }

  std::string
  hexagon_box::get_shape_name () const
  {
    return hexagon_box_label();
  }

  double
  hexagon_box::get_surface (uint32_t mask_) const
  {
    double s = 0.0;
    int mask = mask_;
    if ((int) mask_ == (int) ALL_SURFACES) mask = FACE_ALL;

    double side_surf = get_side () * _z_;
    double top_bottom_surf = 1.5 * std::sqrt(3.) * get_side () * get_side ();
    if (mask & FACE_BACK)
      {
        s += side_surf;
      }
    if (mask & FACE_FRONT)
      {
        s += side_surf;
      }
    if (mask & FACE_BACK_LEFT)
      {
        s += side_surf;
      }
    if (mask & FACE_BACK_RIGHT)
      {
        s += side_surf;
      }
    if (mask & FACE_FRONT_RIGHT)
      {
        s += side_surf;
      }
    if (mask & FACE_FRONT_LEFT)
      {
        s += side_surf;
      }
    if (mask & FACE_BOTTOM)
      {
        s += top_bottom_surf;
      }
    if (mask & FACE_TOP)
      {
        s += top_bottom_surf;
      }
    return s;
  }

  double
  hexagon_box::get_parameter (const std::string & flag_) const
  {
    if (flag_ == "radius") return get_radius ();
    if (flag_ == "diameter") return get_diameter ();
    if (flag_ == "side") return get_side ();
    if (flag_ == "z") return get_z ();
    if (flag_ == "volume")         return get_volume ();
    if (flag_ == "surface.top")    return get_surface (FACE_TOP);
    if (flag_ == "surface.bottom") return get_surface (FACE_BOTTOM);
    if (flag_ == "surface.back")        return get_surface (FACE_BACK);
    if (flag_ == "surface.back_left")   return get_surface (FACE_BACK_LEFT);
    if (flag_ == "surface.back_right")  return get_surface (FACE_BACK_RIGHT);
    if (flag_ == "surface.front")       return get_surface (FACE_FRONT);
    if (flag_ == "surface.front_left")  return get_surface (FACE_FRONT_LEFT);
    if (flag_ == "surface.front_right") return get_surface (FACE_FRONT_RIGHT);
    if (flag_ == "surface")             return get_surface (FACE_ALL);

    DT_THROW_IF (true, std::logic_error, "Unknown '" << flag_ << "' flag!");
  }

  double
  hexagon_box::get_volume (uint32_t /*flags*/) const
  {
    return hexagon_box::get_surface (FACE_BOTTOM) * _z_;
  }

  bool
  hexagon_box::is_valid () const
  {
    return (_radius_ > 0.0 && _z_ > 0.0);
  }

  void
  hexagon_box::init ()
  {
  }

  void
  hexagon_box::reset ()
  {
    _radius_ = -1.0;
    _z_ = -1.0;
  }

  bool
  hexagon_box::xy_is_in_hexagon (double r_, double x_, double y_, double skin_)
  {
    double x = std::abs (x_);
    double y = std::abs (y_);
    double r = r_;
    if (x > (r * 0.5 * std::sqrt(3.) - 0.5 * skin_)) return false;
    double b = r - skin_ * 0.5 * std::sqrt(3.);
    if (y > (-0.5 * x + b)) return false;
    return true;
  }

  bool
  hexagon_box::xy_is_out_hexagon (double r_, double x_, double y_, double skin_)
  {
    return ! xy_is_in_hexagon (r_ + 0.5 * skin_, x_, y_, 0.0);
  }

  bool
  hexagon_box::xy_is_on_hexagon (double r_, double x_, double y_, double skin_)
  {
    return ! xy_is_in_hexagon (r_, x_, y_, skin_)
      && ! xy_is_out_hexagon (r_, x_, y_, skin_);
  }

  bool
  hexagon_box::is_inside (const vector_3d & point_, double skin_) const
  {
    double skin = get_skin (skin_);
    double hskin = 0.5 * skin;
    if ( (std::abs(point_.z ()) < (0.5 * _z_ - hskin))
         && ( xy_is_in_hexagon (_radius_ - hskin,
                                point_.x (),
                                point_.y (),
                                0.0))
         ) return true;
    return false;
  }

  bool
  hexagon_box::is_outside (const vector_3d & point_, double skin_) const
  {
    double skin = get_skin (skin_);
    double hskin = 0.5 * skin;
    if ( (std::abs(point_.z ()) > (0.5 * _z_ + hskin))
         || ( xy_is_out_hexagon (_radius_ + hskin,
                                 point_.x (),
                                 point_.y (),
                                 0.0))
         ) return true;
    return false;
  }

  vector_3d
  hexagon_box::get_normal_on_surface (const vector_3d & position_) const
  {
    vector_3d normal;
    invalidate (normal);
    if (     is_on_surface (position_, FACE_BACK))        normal.set (-1.0,  0.0, 0.0);
    else if (is_on_surface (position_, FACE_BACK_LEFT))   normal.set (-0.5, -0.5 * std::sqrt(3.), 0.0);
    else if (is_on_surface (position_, FACE_BACK_RIGHT))  normal.set (-0.5, +0.5 * std::sqrt(3.), 0.0);
    else if (is_on_surface (position_, FACE_FRONT))       normal.set (+1.0,  0.0, 0.0);
    else if (is_on_surface (position_, FACE_FRONT_LEFT))  normal.set (+0.5, -0.5 * std::sqrt(3.), 0.0);
    else if (is_on_surface (position_, FACE_FRONT_RIGHT)) normal.set (+0.5, +0.5 * std::sqrt(3.), 0.0);
    else if (is_on_surface (position_, FACE_BOTTOM))      normal.set ( 0.0,  0.0, -1.0);
    else if (is_on_surface (position_, FACE_TOP))         normal.set ( 0.0,  0.0, +1.0);
    return (normal);
  }

  bool
  hexagon_box::is_on_surface (const vector_3d & point_ ,
                              int    mask_ ,
                              double skin_) const
  {
    double skin = get_skin (skin_);
    double hskin = 0.5 * skin;
    int mask = mask_;
    if (mask_ == (int) ALL_SURFACES) mask = FACE_ALL;

    if (is_outside(point_, skin_)) return false;
    if (is_inside(point_, skin_)) return false;

    if (mask & FACE_BOTTOM) {
      if ( (std::abs(point_.z() + 0.5 * _z_) < hskin)) return true;
    }

    if (mask & FACE_TOP) {
      if ( (std::abs(point_.z() - 0.5 * _z_) < hskin)) return true;
    }

    double phi = point_.phi();
    double cp = std::cos(phi);
    double sp = std::sin(phi);

    if (mask & FACE_FRONT) {
      if ((cp > 0.) && (std::abs(sp) <= 0.5)) return true;
    }

    if (mask & FACE_FRONT_RIGHT) {
      if ((cp >= 0.) && (sp > 0.5)) return true;
    }

    if (mask & FACE_FRONT_LEFT) {
      if ((cp >= 0.) && (sp < -0.5)) return true;
    }

    if (mask & FACE_BACK) {
      if ((cp < 0.) && (std::abs(sp) <= 0.5)) return true;
    }

    if (mask & FACE_BACK_RIGHT) {
      if ((cp <= 0.) && (sp > 0.5)) return true;
    }

    if (mask & FACE_BACK_LEFT) {
      if ((cp <= 0.) && (sp < -0.5)) return true;
    }

    return false;
  }

  bool
  hexagon_box::find_intercept (const vector_3d & /*from_*/,
                               const vector_3d & /*direction_*/,
                               intercept_t & intercept_,
                               double /*skin_*/) const
  {
    /*
      bool debug = false;
      const unsigned int NFACES = 6;
      double t[NFACES];
      t[BACK]   = -(get_half_x () + from_[vector_3d::X])
      / direction_[vector_3d::X];
      t[FRONT]  = +(get_half_x () - from_[vector_3d::X])
      / direction_[vector_3d::X];
      t[LEFT]   = -(get_half_y () + from_[vector_3d::Y])
      / direction_[vector_3d::Y];
      t[RIGHT]  = +(get_half_y () - from_[vector_3d::Y])
      / direction_[vector_3d::Y];
      t[BOTTOM] = -(get_half_z () + from_[vector_3d::Z])
      / direction_[vector_3d::Z];
      t[TOP]    = +(get_half_z () - from_[vector_3d::Z])
      / direction_[vector_3d::Z];

      double t_min = -1.0;
      int face_min = 0;
      for (int i = 0; i < (int) NFACES; i++)
      {
      double ti = t[i];
      if (debug)
      {
      std::clog << "DEVEL: hexagon_box::find_intercept: t[" << i << "]= "
      << ti << " t_min=" << t_min
      << " face_min=" << face_min
      << std::endl;
      }
      if (std::isnormal (ti) && (ti > 0.0))
      {
      int face_bit = (0x1 << i); // face mask
      vector_3d intercept = from_ + direction_ * ti;
      if (is_on_surface (intercept, face_bit, skin_))
      {
      if ((t_min < 0.0) || (ti < t_min))
      {
      t_min = ti;
      face_min = face_bit;
      }
      }
      }
      }
    */
    intercept_.reset ();
    /*
      if (face_min > 0)
      {
      intercept_.set (0, face_min, from_ + direction_ * t_min);
      }
    */
    return intercept_.is_ok ();
  }

  std::ostream &
  operator<< (std::ostream & out_, const hexagon_box & b_)
  {
    out_ << '{' << hexagon_box::hexagon_box_label() << ' '
         << b_._radius_ << ' '
         << b_._z_ << '}';
    return out_;
  }

  std::istream &
  operator>> (std::istream & in_, hexagon_box & b_)
  {
    b_.reset ();
    char c = 0;
    in_.get (c);
    if (c != '{')
      {
        in_.clear (std::ios_base::failbit);
        return in_;
      }
    std::string name;
    in_ >> name;
    if (name != hexagon_box::hexagon_box_label())
      {
        in_.clear (std::ios_base::failbit);
        return in_;
      }
    double r, z;
    in_ >> r >> z;
    if (! in_)
      {
        in_.clear (std::ios_base::failbit);
        return in_;
      }
    c = 0;
    in_.get (c);
    if (c != '}')
      {
        in_.clear (std::ios_base::failbit);
        return in_;
      }
    try
      {
        b_.set (r,z);
      }
    catch (...)
      {
        b_.reset ();
        in_.clear (std::ios_base::failbit);
      }
    return in_;
  }

  void hexagon_box::compute_deflated(hexagon_box & deflated_,
                                     double by_r_,
                                     double by_z_)
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid hexagon box!");
    double r_eps = 0.0;
    double z_eps = 0.0;
    if (datatools::is_valid(by_r_) and by_r_ > 0.0) r_eps = by_r_;
    if (datatools::is_valid(by_z_) and by_z_ > 0.0) z_eps = by_z_;
    double r = get_radius();
    double z = get_z();
    r -= r_eps;
    z -= (2 * z_eps);
    deflated_.reset();
    deflated_.set(r, z);
    deflated_.lock();
    return;
  }

  void hexagon_box::compute_inflated(hexagon_box & inflated_,
                                     double by_r_,
                                     double by_z_)
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid hexagon box!");
    double r_eps = 0.0;
    double z_eps = 0.0;
    if (datatools::is_valid(by_r_) and by_r_ > 0.0) r_eps = by_r_;
    if (datatools::is_valid(by_z_) and by_z_ > 0.0) z_eps = by_z_;
    double r = get_radius();
    double z = get_z();
    r += r_eps;
    z += (2 * z_eps);
    inflated_.reset();
    inflated_.set(r, z);
    inflated_.lock();
    return;
  }

} // end of namespace geomtools

// end of hexagon_box.cc
