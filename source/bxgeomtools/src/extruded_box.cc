// extruded_box.cc

// Ourselves:
#include <geomtools/extruded_box.h>

// Standard library:
#include <datatools/utils.h>
#include <datatools/exception.h>
#include <datatools/units.h>
#include <datatools/properties.h>

// This project:
#include <geomtools/gnuplot_draw.h>

namespace geomtools {

  // Registration :
  GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(extruded_box, "geomtools::extruded_box");

  const std::string & extruded_box::extruded_box_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "extruded_box";
    }
    return label;
  }

  double extruded_box::get_xmin() const
  {
    return -0.5 * _x_;
  }

  double extruded_box::get_xmax() const
  {
    return +0.5 * _x_;
  }

  double extruded_box::get_ymin() const
  {
    return -0.5 * _y_;
  }

  double extruded_box::get_ymax() const
  {
    return +0.5 * _y_;
  }

  double extruded_box::get_zmin() const
  {
    return -0.5 * _z_;
  }

  double extruded_box::get_zmax() const
  {
    return +0.5 * _z_;
  }

  double
  extruded_box::get_x() const
  {
    return _x_;
  }

  void
  extruded_box::set_x(double x_)
  {
    DT_THROW_IF (x_ <= 0.0, std::logic_error, "Invalid '" << x_ << "' X value !");
    _x_ = x_;
    return;
  }

  double
  extruded_box::get_half_x() const
  {
    return _x_ * 0.5;
  }

  void
  extruded_box::set_half_x(double half_x_)
  {
    set_x(half_x_ + half_x_);
    return;
  }

  double
  extruded_box::get_y() const
  {
    return _y_;
  }

  double
  extruded_box::get_half_y() const
  {
    return _y_ * 0.5;
  }

  void
  extruded_box::set_half_y(double half_y_)
  {
    set_y(half_y_ + half_y_);
    return;
  }

  void
  extruded_box::set_y( double y_ )
  {
    DT_THROW_IF (y_ <= 0.0,std::logic_error, "Invalid '" << y_ << "' Y value !");
    _y_ = y_;
    return;
  }

  double
  extruded_box::get_z() const
  {
    return _z_;
  }

  void
  extruded_box::set_z(double z_)
  {
    DT_THROW_IF (z_ < 0.0, std::logic_error, "Invalid '" << z_ << "' Z value !");
    _z_ = z_;
    return;
  }

  double
  extruded_box::get_half_z() const
  {
    return _z_ * 0.5;
  }

  void
  extruded_box::set_half_z(double half_z_)
  {
    set_z(half_z_ + half_z_);
    return;
  }

  double
  extruded_box::get_thickness() const
  {
    return _thickness_;
  }

  void
  extruded_box::set_thickness(double t_)
  {
    DT_THROW_IF (t_ <= 0.0, std::logic_error, "Invalid '" << t_ << "' thickness value !");
    _thickness_ = t_;
    return;
  }

  bool
  extruded_box::has_top() const
  {
    return _has_top_;
  }

  void
  extruded_box::set_top(bool top_)
  {
    _has_top_ = top_;
    return;
  }

  void
  extruded_box::add_top()
  {
    set_top (true);
    return;
  }

  void
  extruded_box::remove_top()
  {
    set_top (false);
    return;
  }

  bool
  extruded_box::has_bottom() const
  {
    return _has_bottom_;
  }

  void
  extruded_box::set_bottom(bool bottom_)
  {
    _has_bottom_ = bottom_;
    return;
  }

  void
  extruded_box::add_bottom()
  {
    set_bottom (true);
    return;
  }

  void
  extruded_box::remove_bottom()
  {
    set_bottom (false);
    return;
  }

  void
  extruded_box::set(double x_, double y_, double z_, double t_)
  {
    set_x(x_);
    set_y(y_);
    set_z(z_);
    set_thickness(t_);
    return;
  }

  void
  extruded_box::set_half(double half_x_, double half_y_, double half_z_, double t_)
  {
    set_half_x(half_x_);
    set_half_y(half_y_);
    set_half_z(half_z_);
    set_thickness(t_);
    return;
  }

  extruded_box::extruded_box() : i_shape_3d(DEFAULT_SKIN)
  {
    _set_default();
    return;
  }

  extruded_box::extruded_box(double x_, double y_, double z_, double t_,
                             bool top_, bool bottom_) : i_shape_3d(DEFAULT_SKIN)
  {
    _set_default();
    set_x(x_);
    set_y(y_);
    set_z(z_);
    set_thickness(t_);
    set_top(top_);
    set_bottom(bottom_);
    return;
  }

  extruded_box::~extruded_box()
  {
    return;
  }

  std::string
  extruded_box::get_shape_name() const
  {
    return extruded_box_label();
  }

  double
  extruded_box::get_surface(uint32_t mask_) const
  {
    double s = 0.0;
    uint32_t mask = mask_;
    if (mask_ == (uint32_t) ALL_SURFACES) mask = FACE_ALL;

    if (mask & FACE_BACK) {
      s += _y_ * _z_;
    }
    if (mask & FACE_FRONT) {
      s += _y_ * _z_;
    }
    if (mask & FACE_LEFT) {
      s += _x_ * _z_;
    }
    if (mask & FACE_RIGHT) {
      s += _x_ * _z_;
    }
    if (mask & FACE_TOP) {
      s += _x_ * _y_;
    }
    if (mask & FACE_BOTTOM) {
      s += _x_ * _y_;
    }
    if (mask & FACE_INSIDE_BACK) {
      s += (_y_- 2 * _thickness_) * (_z_ - 2 * _thickness_);
    }
    if (mask & FACE_INSIDE_FRONT) {
      s += (_y_ - 2 * _thickness_) * (_z_ - 2 * _thickness_);
    }
    if (mask & FACE_INSIDE_LEFT) {
      s += (_x_ - 2 * _thickness_) * (_z_ - 2 * _thickness_);
    }
    if (mask & FACE_INSIDE_RIGHT) {
      s += (_x_ - 2 * _thickness_) * (_z_ - 2 * _thickness_);
    }

    if (has_top()) {
      if (mask & FACE_INSIDE_TOP) {
        s += (_x_ - 2 * _thickness_) * (_y_ - 2 * _thickness_);
      }
    }
    else {
      if (mask & FACE_INSIDE_BACK) {
        s += (_y_ - 2 * _thickness_) * _thickness_;
      }
      if (mask & FACE_INSIDE_FRONT) {
        s += (_y_ - 2 * _thickness_) * _thickness_;
      }
      if (mask & FACE_INSIDE_LEFT) {
        s += (_x_ - 2 * _thickness_) * _thickness_;
      }
      if (mask & FACE_INSIDE_RIGHT) {
        s += (_x_ - 2 * _thickness_) * _thickness_;
      }
      if (mask & FACE_TOP) {
        s -= (_x_ - 2 * _thickness_) * (_y_ - 2 * _thickness_);
      }
    }

    if (has_bottom()) {
      if (mask & FACE_INSIDE_BOTTOM) {
        s += _x_ * _y_;
      }
    }
    else {
      if (mask & FACE_INSIDE_BACK) {
        s += (_y_ - 2 * _thickness_) * _thickness_;
      }
      if (mask & FACE_INSIDE_FRONT) {
        s += (_y_ - 2 * _thickness_) * _thickness_;
      }
      if (mask & FACE_INSIDE_LEFT) {
        s += (_x_ - 2 * _thickness_) * _thickness_;
      }
      if (mask & FACE_INSIDE_RIGHT) {
        s += (_x_- 2 * _thickness_) * _thickness_;
      }
      if (mask & FACE_BOTTOM) {
        s -= (_x_ - 2 * _thickness_) * (_y_ - 2 * _thickness_);
      }
    }

    return s;
  }

  double
  extruded_box::get_parameter(const std::string & flag_) const
  {
    if (flag_ == "x")          return get_x();
    if (flag_ == "y")          return get_y();
    if (flag_ == "z")          return get_z();
    if (flag_ == "thickness")  return get_thickness();
    if (flag_ == "has_top")    return has_top();
    if (flag_ == "has_bottom") return has_bottom();
    if (flag_ == "volume")     return get_volume();
    if (flag_ == "surface")    return get_surface(FACE_ALL);
    if (flag_ == "surface.top")           return get_surface(FACE_TOP);
    if (flag_ == "surface.bottom")        return get_surface(FACE_BOTTOM);
    if (flag_ == "surface.back")          return get_surface(FACE_BACK);
    if (flag_ == "surface.front")         return get_surface(FACE_FRONT);
    if (flag_ == "surface.left")          return get_surface(FACE_LEFT);
    if (flag_ == "surface.right")         return get_surface(FACE_RIGHT);
    if (flag_ == "surface.inside_top")    return get_surface(FACE_INSIDE_TOP);
    if (flag_ == "surface.inside_bottom") return get_surface(FACE_INSIDE_BOTTOM);
    if (flag_ == "surface.inside_back")   return get_surface(FACE_INSIDE_BACK);
    if (flag_ == "surface.inside_front")  return get_surface(FACE_INSIDE_FRONT);
    if (flag_ == "surface.inside_left")   return get_surface(FACE_INSIDE_LEFT);
    if (flag_ == "surface.inside_right")  return get_surface(FACE_INSIDE_RIGHT);

    DT_THROW_IF (true, std::logic_error, "Unknown flag '" << flag_ << "' !");
  }

  double
  extruded_box::get_volume(uint32_t /*flags*/) const
  {
    double v = _x_ * _y_ * _z_;
    double z_int = _z_;
    if (has_top()) z_int -= _thickness_;
    if (has_bottom()) z_int -= _thickness_;
    double v_int = (_x_ - 2 * _thickness_) * (_y_ - 2 * _thickness_) * z_int;
    return v - v_int;
  }

  bool
  extruded_box::is_valid() const
  {
    return(datatools::is_valid(_x_) && _x_ > 0.0
           && datatools::is_valid(_y_) && _y_ > 0.0
           && datatools::is_valid(_z_) && _z_ > 0.0
           && datatools::is_valid(_thickness_) && _thickness_ > 0.0
           && _thickness_ < _x_ / 2.0
           && _thickness_ < _y_ / 2.0
           && _thickness_ < _z_ / 2.0);
  }

  void extruded_box::initialize(const datatools::properties & config_,
                                const handle_dict_type * objects_)
  {
    reset();
    this->i_shape_3d::initialize(config_, objects_);

    double lunit = CLHEP::mm;
    if (config_.has_key("length_unit")) {
      const std::string lunit_str = config_.fetch_string("length_unit");
      lunit = datatools::units::get_length_unit_from(lunit_str);
    }

    DT_THROW_IF (! config_.has_key("x"), std::logic_error, "Missing extruded box 'x' property !");
    double x = config_.fetch_real("x");
    if (! config_.has_explicit_unit("x")) {
      x *= lunit;
    }
    DT_THROW_IF (! config_.has_key("y"), std::logic_error, "Missing extruded box 'y' property !");
    double y = config_.fetch_real("y");
    if (! config_.has_explicit_unit("y")) {
      y *= lunit;
    }
    DT_THROW_IF (! config_.has_key("z"), std::logic_error, "Missing extruded box 'z' property !");
    double z = config_.fetch_real("z");
    if (! config_.has_explicit_unit("z")) {
      z *= lunit;
    }

    DT_THROW_IF (! config_.has_key("thickness"), std::logic_error, "Missing extruded box 'thickness' property !");
    double thickness = config_.fetch_real("thickness");
    if (! config_.has_explicit_unit("thickness")) {
      thickness *= lunit;
    }

    bool has_top    = true;
    bool has_bottom = true;

    if (config_.has_flag("remove_top")) {
      has_top = false;
    }

    if (config_.has_key("has_top")) {
      has_top = config_.fetch_boolean("has_top");
    }

    if (config_.has_flag("remove_bottom")) {
      has_bottom = false;
    }

    if (config_.has_key("has_bottom")) {
      has_bottom = config_.fetch_boolean ("has_bottom");
    }

    set(x, y, z, thickness);
    set_top(has_top);
    set_bottom(has_bottom);

    lock();
    return;
  }

  void extruded_box::_set_default()
  {
    datatools::invalidate(_x_);
    datatools::invalidate(_y_);
    datatools::invalidate(_z_);
    datatools::invalidate(_thickness_);
    set_top(true);
    set_bottom(true);

    return;
  }

  void
  extruded_box::reset()
  {
    unlock();

    _set_default();

    this->i_shape_3d::reset();
    return;
  }

  bool
  extruded_box::is_outside(const vector_3d & position_, double skin_) const
  {
    double skin = get_skin(skin_);
    double hskin = 0.5 * skin;
    if ((std::abs(position_.x()) >= (0.5 * _x_ + hskin))
        || (std::abs(position_.y()) >= (0.5 * _y_ + hskin))
        || (std::abs(position_.z()) >= (0.5 * _z_ + hskin))) {
      return true;
    }
    if ((std::abs(position_.x()) <= (0.5 * _x_ - _thickness_ - hskin))
        && (std::abs(position_.y()) <= (0.5 * _y_ - _thickness_ - hskin))
        && (std::abs(position_.z()) <= (0.5 * _z_ - _thickness_ - hskin))) {
      return true;
    }
    if (!has_top() && !has_bottom()
        && (std::abs(position_.x()) <= (0.5 * _x_ - _thickness_ - hskin))
        && (std::abs(position_.y()) <= (0.5 * _y_ - _thickness_ - hskin))) {
      return true;
    }
    if (!has_top()
        && (std::abs(position_.x()) <= (0.5 * _x_ - _thickness_ - hskin))
        && (std::abs(position_.y()) <= (0.5 * _y_ - _thickness_ - hskin))
        && position_.z() >= (-0.5 * _z_ - _thickness_ - hskin)) {
      return true;
    }
    if (!has_bottom()
        && (std::abs(position_.x()) <= (0.5 * _x_ - _thickness_ - hskin))
        && (std::abs(position_.y()) <= (0.5 * _y_ - _thickness_ - hskin))
        && position_.z() <= (0.5 * _z_ - _thickness_ - hskin)) {
      return true;
    }
    return false;
  }

  bool
  extruded_box::is_inside(const vector_3d & position_, double skin_) const
  {
    double skin = get_skin(skin_);
    double hskin = 0.5 * skin;
    if ((std::abs(position_.x()) <= (0.5 * _x_ - hskin))
        && (std::abs(position_.x()) >= (0.5 * _x_ - _thickness_ + hskin))
        && (std::abs(position_.y()) <= (0.5 * _y_ - hskin))
        && (std::abs(position_.z()) <= (0.5 * _z_ - hskin))) {
      return true;
    }
    if ((std::abs(position_.x()) <= (0.5 * _x_ - hskin))
        && (std::abs(position_.y()) <= (0.5 * _y_ - hskin))
        && (std::abs(position_.y()) >= (0.5 * _y_ - _thickness_ + hskin))
        && (std::abs(position_.z()) <= (0.5 * _z_ - hskin))) {
      return true;
    }
    if (has_top()
        && (std::abs(position_.x()) <= (0.5 * _x_ - hskin))
        && (std::abs(position_.y()) <= (0.5 * _y_ - hskin))
        && (position_.z() <= (0.5 * _z_ - hskin))
        && (position_.z() >= (0.5 * _z_ - _thickness_ + hskin))) {
      return true;
    }
    if (has_bottom()
        && (std::abs(position_.x()) <= (0.5 * _x_ - hskin))
        && (std::abs(position_.y()) <= (0.5 * _y_ - hskin))
        && (position_.z() >= (-0.5 * _z_ + hskin))
        && (position_.z() <= (-0.5 * _z_ - _thickness_ - hskin))) {
      return true;
    }
    return false;
  }

  vector_3d
  extruded_box::get_normal_on_surface(const vector_3d & position_) const
  {
    vector_3d normal;
    geomtools::invalidate(normal);
    if (is_on_surface(position_, FACE_BACK))               normal.set(-1., 0., 0.);
    else if (is_on_surface(position_, FACE_FRONT))         normal.set(+1., 0., 0.);
    else if (is_on_surface(position_, FACE_LEFT))          normal.set(0., -1., 0.);
    else if (is_on_surface(position_, FACE_RIGHT))         normal.set(0., +1., 0.);
    else if (is_on_surface(position_, FACE_BOTTOM))        normal.set(0., 0., -1.);
    else if (is_on_surface(position_, FACE_TOP))           normal.set(0., 0., +1.);
    else if (is_on_surface(position_, FACE_INSIDE_BACK))   normal.set(+1., 0., 0.);
    else if (is_on_surface(position_, FACE_INSIDE_FRONT))  normal.set(-1., 0., 0.);
    else if (is_on_surface(position_, FACE_INSIDE_LEFT))   normal.set(0., +1., 0.);
    else if (is_on_surface(position_, FACE_INSIDE_RIGHT))  normal.set(0., -1., 0.);
    else if (is_on_surface(position_, FACE_INSIDE_BOTTOM)) normal.set(0., 0., +1.);
    else if (is_on_surface(position_, FACE_INSIDE_TOP))    normal.set(0., 0., -1.);
    return(normal);
  }

  bool
  extruded_box::is_on_surface(const vector_3d & position_ ,
                              int    mask_ ,
                              double skin_) const
  {
    double skin = get_skin(skin_);

    int mask = mask_;
    if (mask_ == (int) ALL_SURFACES) mask = FACE_ALL;

    double hskin = 0.5 * skin;

    // outside faces regardless top or bottom covers
    if (mask & FACE_BACK) {
      if ((std::abs(position_.x() + 0.5 * _x_) < hskin)
          && (std::abs(position_.y()) < 0.5 * _y_)
          && (std::abs(position_.z()) < 0.5 * _z_)) return true;
    }
    if (mask & FACE_FRONT) {
      if ((std::abs(position_.x() - 0.5 * _x_) < hskin)
          && (std::abs(position_.y()) < 0.5 * _y_)
          && (std::abs(position_.z()) < 0.5 * _z_)) return true;
    }
    if (mask & FACE_LEFT) {
      if ((std::abs(position_.y() + 0.5 * _y_) < hskin)
          && (std::abs(position_.x()) < 0.5 * _x_)
          && (std::abs(position_.z()) < 0.5 * _z_)) return true;
    }
    if (mask & FACE_RIGHT) {
      if ((std::abs(position_.y() - 0.5 * _y_) < hskin)
          && (std::abs(position_.x()) < 0.5 * _x_)
          && (std::abs(position_.z()) < 0.5 * _z_)) return true;
    }

    // inside faces regardless top or bottom covers
    if (mask & FACE_INSIDE_BACK) {
      if ((std::abs(position_.x() + 0.5 * _x_ - _thickness_) < hskin)
          && (std::abs(position_.y()) < 0.5 * _y_ - _thickness_)
          && (std::abs(position_.z()) < 0.5 * _z_ - _thickness_)) return true;
    }
    if (mask & FACE_INSIDE_FRONT) {
      if ((std::abs(position_.x() - 0.5 * _x_ + _thickness_) < hskin)
          && (std::abs(position_.y()) < 0.5 * _y_ - _thickness_)
          && (std::abs(position_.z()) < 0.5 * _z_ - _thickness_)) return true;
    }
    if (mask & FACE_INSIDE_LEFT) {
      if ((std::abs(position_.y() + 0.5 * _y_ - _thickness_) < hskin)
          && (std::abs(position_.x()) < 0.5 * _x_ - _thickness_)
          && (std::abs(position_.z()) < 0.5 * _z_ - _thickness_)) return true;
    }
    if (mask & FACE_INSIDE_RIGHT) {
      if ((std::abs(position_.y() - 0.5 * _y_ + _thickness_) < hskin)
          && (std::abs(position_.x()) < 0.5 * _x_ - _thickness_)
          && (std::abs(position_.z()) < 0.5 * _z_ - _thickness_)) return true;
    }

    // if has top cover
    if (has_top()) {
      if (mask & FACE_TOP) {
        if ((std::abs(position_.z() - 0.5 * _z_) < hskin)
            && (std::abs(position_.x()) < 0.5 * _x_)
            && (std::abs(position_.y()) < 0.5 * _y_)) return true;
      }
      if (mask & FACE_INSIDE_TOP) {
        if ((std::abs(position_.z() - 0.5 * _z_ + _thickness_) < hskin)
            && (std::abs(position_.x()) < 0.5 * _x_ - _thickness_)
            && (std::abs(position_.y()) < 0.5 * _y_ - _thickness_)) return true;
      }
    }

    // if has not top cover
    else {
      if (mask & FACE_TOP) {
        if ((std::abs(position_.z() - 0.5 * _z_) < hskin)
            && (std::abs(position_.x()) < 0.5 * _x_)
            && (std::abs(position_.x()) > 0.5 * _x_ - _thickness_)
            && (std::abs(position_.y()) < 0.5 * _y_)
            && (std::abs(position_.y()) > 0.5 * _y_ - _thickness_)) return true;
      }
      if (mask & FACE_INSIDE_BACK) {
        if ((std::abs(position_.x() + 0.5 * _x_ - _thickness_) < hskin)
            && (std::abs(position_.y()) < 0.5 * _y_ - _thickness_)
            && (position_.z() > 0. && position_.z() < 0.5 * _z_)) return true;
      }
      if (mask & FACE_INSIDE_FRONT) {
        if ((std::abs(position_.x() - 0.5 * _x_ + _thickness_) < hskin)
            && (std::abs(position_.y()) < 0.5 * _y_ - _thickness_)
            && (position_.z() > 0. && position_.z() < 0.5 * _z_)) return true;
      }
      if (mask & FACE_INSIDE_LEFT) {
        if ((std::abs(position_.y() + 0.5 * _y_ - _thickness_) < hskin)
            && (std::abs(position_.x()) < 0.5 * _x_ - _thickness_)
            && (position_.z() > 0. && position_.z() < 0.5 * _z_)) return true;
      }
      if (mask & FACE_INSIDE_RIGHT) {
        if ((std::abs(position_.y() - 0.5 * _y_ + _thickness_) < hskin)
            && (std::abs(position_.x()) < 0.5 * _x_ - _thickness_)
            && (position_.z() > 0. && position_.z() < 0.5 * _z_)) return true;
      }
    }

    // if has bottom cover
    if (has_bottom()) {
      if (mask & FACE_BOTTOM) {
        if ((std::abs(position_.z() + 0.5 * _z_) < hskin)
            && (std::abs(position_.x()) < 0.5 * _x_)
            && (std::abs(position_.y()) < 0.5 * _y_)) return true;
      }
      if (mask & FACE_INSIDE_BOTTOM) {
        if ((std::abs(position_.z() + 0.5 * _z_ - _thickness_) < hskin)
            && (std::abs(position_.x()) < 0.5 * _x_ - _thickness_)
            && (std::abs(position_.y()) < 0.5 * _y_ - _thickness_)) return true;
      }
    }

    // if has not bottom cover
    else {
      if (mask & FACE_BOTTOM) {
        if ((std::abs(position_.z() + 0.5 * _z_) < hskin)
            && (std::abs(position_.x()) < 0.5 * _x_)
            && (std::abs(position_.x()) > 0.5 * _x_ - _thickness_)
            && (std::abs(position_.y()) < 0.5 * _y_)
            && (std::abs(position_.y()) > 0.5 * _y_ - _thickness_)) return true;
      }
      if (mask & FACE_INSIDE_BACK) {
        if ((std::abs(position_.x() + 0.5 * _x_ - _thickness_) < hskin)
            && (std::abs(position_.y()) < 0.5 * _y_ - _thickness_)
            && (position_.z() < 0. && position_.z() > - 0.5 * _z_)) return true;
      }
      if (mask & FACE_INSIDE_FRONT) {
        if ((std::abs(position_.x() - 0.5 * _x_ + _thickness_) < hskin)
            && (std::abs(position_.y()) < 0.5 * _y_ - _thickness_)
            && (position_.z() < 0. && position_.z() > - 0.5 * _z_)) return true;
      }
      if (mask & FACE_INSIDE_LEFT) {
        if ((std::abs(position_.y() + 0.5 * _y_ - _thickness_) < hskin)
            && (std::abs(position_.x()) < 0.5 * _x_ - _thickness_)
            && (position_.z() < 0. && position_.z() > - 0.5 * _z_)) return true;
      }
      if (mask & FACE_INSIDE_RIGHT) {
        if ((std::abs(position_.y() - 0.5 * _y_ + _thickness_) < hskin)
            && (std::abs(position_.x()) < 0.5 * _x_ - _thickness_)
            && (position_.z() < 0. && position_.z() > - 0.5 * _z_)) return true;
      }
    }
    return false;
  }

  bool
  extruded_box::find_intercept(const vector_3d & from_,
                               const vector_3d & direction_,
                               intercept_t & intercept_,
                               double skin_) const
  {
    DT_THROW_IF(!has_top() || !has_bottom(), std::runtime_error,
                "Not implemented for extruded box without top or bottom !");

    bool debug = false;
    const unsigned int NFACES = 12;
    double t[NFACES];
    t[BACK]   = -(get_half_x() + from_[vector_3d::X])
      / direction_[vector_3d::X];
    t[FRONT]  = +(get_half_x() - from_[vector_3d::X])
      / direction_[vector_3d::X];
    t[LEFT]   = -(get_half_y() + from_[vector_3d::Y])
      / direction_[vector_3d::Y];
    t[RIGHT]  = +(get_half_y() - from_[vector_3d::Y])
      / direction_[vector_3d::Y];
    t[BOTTOM] = -(get_half_z() + from_[vector_3d::Z])
      / direction_[vector_3d::Z];
    t[TOP]    = +(get_half_z() - from_[vector_3d::Z])
      / direction_[vector_3d::Z];

    t[NFACES/2+BACK]   = -((get_half_x()-_thickness_) + from_[vector_3d::X])
      / direction_[vector_3d::X];
    t[NFACES/2+FRONT]  = +((get_half_x()-_thickness_) - from_[vector_3d::X])
      / direction_[vector_3d::X];
    t[NFACES/2+LEFT]   = -((get_half_y()-_thickness_) + from_[vector_3d::Y])
      / direction_[vector_3d::Y];
    t[NFACES/2+RIGHT]  = +((get_half_y()-_thickness_) - from_[vector_3d::Y])
      / direction_[vector_3d::Y];
    t[NFACES/2+BOTTOM] = -((get_half_z()-_thickness_) + from_[vector_3d::Z])
      / direction_[vector_3d::Z];
    t[NFACES/2+TOP]    = +((get_half_z()-_thickness_) - from_[vector_3d::Z])
      / direction_[vector_3d::Z];

    double t_min = -1.;
    int face_min = 0;
    for (int i = 0; i <(int) NFACES; i++) {
      double ti = t[i];
      if (debug) {
        std::clog << "DEVEL: extruded_box::find_intercept: t[" << i << "]= "
                  << ti << " t_min=" << t_min
                  << " face_min=" << face_min
                  << std::endl;
      }
      if (std::isnormal(ti) &&(ti > 0.)) {
        int face_bit =(0x1 << i); // face mask
        vector_3d intercept = from_ + direction_ * ti;
        if (is_on_surface(intercept, face_bit, skin_)) {
          if ((t_min < 0.) ||(ti < t_min)) {
            t_min = ti;
            face_min = face_bit;
          }
        }
      }
    }
    intercept_.reset();
    if (face_min > 0) {
      intercept_.set(0, face_min, from_ + direction_ * t_min);
    }
    return intercept_.is_ok();
  }

  std::ostream &
  operator<<(std::ostream & out_, const extruded_box & extruded_box_)
  {
    out_ << '{' << extruded_box::extruded_box_label() << ' '
         << extruded_box_._x_ << ' '
         << extruded_box_._y_ << ' '
         << extruded_box_._z_ << ' '
         << extruded_box_._thickness_ << '}';
    return out_;
  }

  std::istream &
  operator>>(std::istream & in_, extruded_box & extruded_box_)
  {
    extruded_box_.reset();
    char c = 0;
    in_.get(c);
    if (c != '{') {
      in_.clear(std::ios_base::failbit);
      return in_;
    }
    std::string name;
    in_ >> name;
    if (name != extruded_box::extruded_box_label()) {
      in_.clear(std::ios_base::failbit);
      return in_;
    }
    double x, y, z, t;
    in_ >> x >> y >> z >> t;
    if (! in_) {
      in_.clear(std::ios_base::failbit);
      return in_;
    }
    c = 0;
    in_.get(c);
    if (c != '}') {
      in_.clear(std::ios_base::failbit);
      return in_;
    }
    try {
      extruded_box_.set(x,y,z,t);
    } catch (...) {
      extruded_box_.reset();
      in_.clear(std::ios_base::failbit);
    }
    return in_;
  }


  void extruded_box::tree_dump(std::ostream & out_,
                               const std::string & title_,
                               const std::string & indent_,
                               bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty()) indent = indent_;
    i_object_3d::tree_dump(out_, title_, indent_, true);

    out_ << indent << datatools::i_tree_dumpable::tag
         << "X          : " << get_x() / CLHEP::mm << " mm" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Y          : " << get_y() / CLHEP::mm << " mm" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Z          : " << get_z() / CLHEP::mm << " mm" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "thickness  : " << get_thickness() / CLHEP::mm << " mm" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "has top    : " << has_top() << std::endl;
    out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "has bottom : " << has_bottom() << std::endl;
    return;
  }

  void extruded_box::generate_wires(std::list<polyline_3d> & lpl_,
                                    const placement & p_,
                                    uint32_t /*options_*/) const
  {
    double dim[3];
    dim[0] = 0.5 * get_x();
    dim[1] = 0.5 * get_y();
    dim[2] = 0.5 * get_z();

    for (int i = 0; i < 6; i++) {
      for (int j = 0; j < 2; j++) {
        vector_3d vertex[4];
        if (i == 0) {
          vertex[0].set((1-2*j) * dim[0],   dim[1],  dim[2]);
          vertex[1].set((1-2*j) * dim[0],   dim[1], -dim[2]);
          vertex[2].set((1-2*j) * dim[0],  -dim[1], -dim[2]);
          vertex[3].set((1-2*j) * dim[0],  -dim[1],  dim[2]);
        }
        if (i == 1) {
          vertex[0].set( dim[0],(1-2*j) * dim[1],   dim[2]);
          vertex[1].set(-dim[0],(1-2*j) * dim[1],   dim[2]);
          vertex[2].set(-dim[0],(1-2*j) * dim[1],  -dim[2]);
          vertex[3].set( dim[0],(1-2*j) * dim[1],  -dim[2]);
        }
        if (i == 2) {
          vertex[0].set( dim[0],  dim[1],(1-2*j) * dim[2]);
          vertex[1].set(-dim[0],  dim[1],(1-2*j) * dim[2]);
          vertex[2].set(-dim[0], -dim[1],(1-2*j) * dim[2]);
          vertex[3].set( dim[0], -dim[1],(1-2*j) * dim[2]);
        }
        if (i == 3) {
          vertex[0].set((1-2*j) * (dim[0]-_thickness_),   (dim[1]-_thickness_),  (dim[2]-_thickness_));
          vertex[1].set((1-2*j) * (dim[0]-_thickness_),   (dim[1]-_thickness_), -(dim[2]-_thickness_));
          vertex[2].set((1-2*j) * (dim[0]-_thickness_),  -(dim[1]-_thickness_), -(dim[2]-_thickness_));
          vertex[3].set((1-2*j) * (dim[0]-_thickness_),  -(dim[1]-_thickness_),  (dim[2]-_thickness_));
        }
        if (i == 4) {
          vertex[0].set( (dim[0]-_thickness_),(1-2*j) * (dim[1]-_thickness_),   (dim[2]-_thickness_));
          vertex[1].set(-(dim[0]-_thickness_),(1-2*j) * (dim[1]-_thickness_),   (dim[2]-_thickness_));
          vertex[2].set(-(dim[0]-_thickness_),(1-2*j) * (dim[1]-_thickness_),  -(dim[2]-_thickness_));
          vertex[3].set( (dim[0]-_thickness_),(1-2*j) * (dim[1]-_thickness_),  -(dim[2]-_thickness_));
        }
        if (i == 5) {
          vertex[0].set( (dim[0]-_thickness_),  (dim[1]-_thickness_),(1-2*j) * (dim[2]-_thickness_));
          vertex[1].set(-(dim[0]-_thickness_),  (dim[1]-_thickness_),(1-2*j) * (dim[2]-_thickness_));
          vertex[2].set(-(dim[0]-_thickness_), -(dim[1]-_thickness_),(1-2*j) * (dim[2]-_thickness_));
          vertex[3].set( (dim[0]-_thickness_), -(dim[1]-_thickness_),(1-2*j) * (dim[2]-_thickness_));
        }
        {
          polyline_3d dummy;
          lpl_.push_back(dummy);
        }
        polyline_3d & pl = lpl_.back();
        pl.set_closed(true);
        for (int i = 0; i < 4; i++) {
          vector_3d v;
          p_.child_to_mother(vertex[i], v);
          pl.add(v);
        }
      }
    }
    return;
  }

  extruded_box::wires_drawer::wires_drawer(const extruded_box & eb_)
  {
    DT_THROW_IF(!eb_.is_locked(), std::logic_error, "Extruded box is not locked!");
    _exbox_ = &eb_;
    return;
  }

  extruded_box::wires_drawer::~wires_drawer()
  {
    return;
  }

  void extruded_box::wires_drawer::generate_wires(std::ostream & out_,
                                                  const geomtools::vector_3d & position_,
                                                  const geomtools::rotation_3d & rotation_)
  {
    datatools::logger::priority local_priority = datatools::logger::PRIO_FATAL;
    if (!_wires_ptr_) {
      _wires_ptr_.reset(new std::list<polyline_3d>);
      _exbox_->generate_wires_self(*_wires_ptr_);
    }
    for (std::list<polyline_3d>::const_iterator i = _wires_ptr_->begin();
         i != _wires_ptr_->end();
         i++) {
      geomtools::gnuplot_draw::draw_polyline(out_, position_, rotation_, *i);
    }
    return;
  }

  // static
  void extruded_box::init_ocd(datatools::object_configuration_description & ocd_)
  {
    i_shape_3d::init_ocd(ocd_);

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("length_unit")
        .set_from("geomtools::extruded_box")
        .set_terse_description("The unit symbol used for length unit")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_default_value_string("mm")
        .set_long_description("A character string that represents the default \n"
                              "length unit for length parameters of the extruded_box.  \n"
                              )
        .add_example("Set the default length unit::       \n"
                     "                                    \n"
                     "  length_unit : string = \"mm\"     \n"
                     "                                    \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("x")
        .set_from("geomtools::extruded_box")
        .set_terse_description("The dimension of the extruded box along the X axis")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(true)
        .add_example("Set the X dimension::          \n"
                     "                               \n"
                     "  x : real as length = 5.0 cm  \n"
                     "                               \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("y")
        .set_from("geomtools::extruded_box")
        .set_terse_description("The dimension of the extruded box along the X axis")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(true)
        .add_example("Set the Y dimension::          \n"
                     "                               \n"
                     "  y : real as length = 10.0 mm \n"
                     "                               \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("z")
        .set_from("geomtools::extruded_box")
        .set_terse_description("The dimension of the extruded box along the Z axis")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(true)
        .add_example("Set the Z dimension::          \n"
                     "                               \n"
                     "  z : real as length = 5.0 mm  \n"
                     "                               \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("thickness")
        .set_from("geomtools::extruded_box")
        .set_terse_description("The dimension of the thickness of the walls along the X,Y,Z axis")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(true)
        .add_example("Set the Z dimension::                  \n"
                     "                                       \n"
                     "  thickness : real as length = 1.0 mm  \n"
                     "                                       \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("has_top")
        .set_from("geomtools::extruded_box")
        .set_terse_description("The flag to close the box on its top (Z axis)")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .add_example("Open the box top::           \n"
                     "                             \n"
                     "  has_top : boolean = false  \n"
                     "                             \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("has_bottom")
        .set_from("geomtools::extruded_box")
        .set_terse_description("The flag to close the box on its bottom (Z axis)")
        .set_traits(datatools::TYPE_BOOLEAN)
        .set_mandatory(false)
        .add_example("Open the box bottom::           \n"
                     "                                \n"
                     "  has_bottom : boolean = false  \n"
                     "                                \n"
                     )
        ;
    }

    return;
  }

} // end of namespace geomtools

/** Opening macro for implementation
 *  @arg geomtools::extruded_box the full class name
 *  @arg ocd_ is the identifier of the 'datatools::object_configuration_description'
 *            to be initialized(passed by mutable reference).
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(geomtools::extruded_box, ocd_)
{
  // The class name :
  ocd_.set_class_name("geomtools::extruded_box");

  // The class terse description :
  ocd_.set_class_description("A class representing a 3D extruded_box solid shape");

  // The library the class belongs to :
  ocd_.set_class_library("geomtools");

  // The class detailed documentation :
  ocd_.set_class_documentation("The extruded_box solid 3D-shape is defined by its three dimensions \n"
                               "along the X, Y and Z axis.                                         \n"
                               );

  // Populate the specific OCD :
  geomtools::extruded_box::init_ocd(ocd_);

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Here is a full configuration example in the      \n"
                               "``datatools::properties`` ASCII format::         \n"
                               "                                                 \n"
                               "  tolerance.length : real = 1.0e-6 um            \n"
                               "  length_unit : string = \"mm\"                  \n"
                               "  x           : real as length = 15.0 cm         \n"
                               "  y :           real as length = 22.0            \n"
                               "  z           : real as length = 12.5            \n"
                               "  thickness   : real as length =  2.5 mm         \n"
                               "  has_top     : boolean        = true            \n"
                               "  has_bottom  : boolean        = true            \n"
                               "                                                 \n"
                               );

  /* Set the validation support flag :
   *  we activate this if the description of all configuration
   *  properties has been provides(see above). This enables the
   *  OCD tools to check the syntax and validaty of a given
   *  configuration file.
   */
  ocd_.set_validation_support(true);

  // Lock the description:
  ocd_.lock();

  // ... and we are done.
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END() // Closing macro for implementation

// Registration macro for class 'geomtools::extruded_box' :
DOCD_CLASS_SYSTEM_REGISTRATION(geomtools::extruded_box, "geomtools::extruded_box")
