// extruded_box.cc

// Ourselves:
#include <geomtools/extruded_box.h>

// Standard library:
#include <datatools/utils.h>
#include <datatools/exception.h>
#include <datatools/units.h>
#include <datatools/properties.h>

// This project:
#include <geomtools/box.h>
#include <geomtools/rectangle.h>
#include <geomtools/quadrangle.h>
#include <geomtools/composite_surface.h>

namespace geomtools {

  // Registration :
  GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(extruded_box, "geomtools::extruded_box")

  const std::string & extruded_box::extruded_box_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "extruded_box";
    }
    return label;
  }

  void extruded_box::_build_bounding_data()
  {
    _grab_bounding_data().make_box(get_xmin(), get_xmax(),
                                   get_ymin(), get_ymax(),
                                   get_zmin(), get_zmax());
    return;
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
    DT_THROW_IF(datatools::is_valid(_x_) && t_ >= 0.5 * _x_,
                std::domain_error,
                "Thickness is too large [" << t_ << "]!");
    DT_THROW_IF(datatools::is_valid(_y_) && t_ >= 0.5 * _y_,
                std::domain_error,
                "Thickness is too large [" << t_ << "]!");
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

  std::string extruded_box::get_shape_name() const
  {
    return extruded_box_label();
  }

  double
  extruded_box::get_surface(uint32_t mask_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid extruded box!");
    double s = 0.0;
    uint32_t mask = mask_;

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
    } else {
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
    } else {
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
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid extruded box!");
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
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid extruded box!");
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
    if (!datatools::is_valid(_x_)) return false;
    if (!datatools::is_valid(_y_)) return false;
    if (!datatools::is_valid(_z_)) return false;
    if (!datatools::is_valid(_thickness_)) return false;
    double z_inner = _z_ - 2. * _thickness_;
    if (!has_bottom()) {
      z_inner += _thickness_;
    }
    if (!has_top()) {
      z_inner += _thickness_;
    }
    if (z_inner <= 0.0) return false;
    return true;
  }

  void extruded_box::initialize(const datatools::properties & config_,
                                const handle_dict_type * objects_)
  {
    this->i_shape_3d::_initialize(config_, objects_);

    if (!is_valid()) {
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

    bool local_has_top    = true;
    bool local_has_bottom = true;

    if (config_.has_flag("remove_top")) {
      local_has_top = false;
    }

    if (config_.has_key("has_top")) {
      local_has_top = config_.fetch_boolean("has_top");
    }

    if (config_.has_flag("remove_bottom")) {
      local_has_bottom = false;
    }

    if (config_.has_key("has_bottom")) {
      local_has_bottom = config_.fetch_boolean ("has_bottom");
    }

    set(x, y, z, thickness);
    set_top(local_has_top);
    set_bottom(local_has_bottom);

    }
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

    this->i_shape_3d::_reset();
    return;
  }

  bool
  extruded_box::is_outside(const vector_3d & position_, double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid extruded box!");
    double skin = get_skin(skin_);

    box outer_box;
    compute_outer_box(outer_box);
    if (outer_box.is_outside(position_, skin)) {
      return true;
    }

    box inner_box;
    placement inner_box_placement;
    compute_inner_box(inner_box, inner_box_placement);
    vector_3d position_c;
    inner_box_placement.mother_to_child(position_, position_c);
    if (inner_box.is_inside(position_c, skin)) {
      return true;
    }

    if (has_top()) {
      if (inner_box.is_on_surface(position_c,
                                  face_identifier(box::FACE_TOP,
                                                  face_identifier::MODE_FACE_BITS),
                                  skin)) {
        return true;
      }
    }

    if (has_bottom()) {
      if (inner_box.is_on_surface(position_c,
                                  face_identifier(box::FACE_BOTTOM,
                                                  face_identifier::MODE_FACE_BITS),
                                  skin)) {
        return true;
      }
    }

    return false;
  }

  bool
  extruded_box::is_inside(const vector_3d & position_, double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid extruded box!");
    double skin = get_skin(skin_);

    box outer_box;
    compute_outer_box(outer_box);

    box inner_box;
    placement inner_box_placement;
    compute_inner_box(inner_box, inner_box_placement);
    vector_3d position_c;
    inner_box_placement.mother_to_child(position_, position_c);
    if (outer_box.is_inside(position_, skin) && inner_box.is_outside(position_c, skin)) {
      return true;
    }

    return false;
  }

  vector_3d
  extruded_box::get_normal_on_surface (const vector_3d & /*a_position*/,
                                       const face_identifier & a_surface_bit) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid extruded box!");
    vector_3d normal;
    geomtools::invalidate(normal);
    switch (a_surface_bit.get_face_bits()) {
    case FACE_BACK:
      normal.set(-1., 0., 0.);
      break;
    case FACE_FRONT:
      normal.set(+1., 0., 0.);
      break;
    case FACE_LEFT:
      normal.set(0., -1., 0.);
      break;
    case FACE_RIGHT:
      normal.set(0., +1., 0.);
      break;
    case FACE_BOTTOM:
      normal.set(0., 0., -1.);
      break;
    case FACE_TOP:
      normal.set(0., 0., +1.);
      break;
    case FACE_INSIDE_BACK:
      normal.set(+1., 0., 0.);
      break;
    case FACE_INSIDE_FRONT:
      normal.set(-1., 0., 0.);
      break;
    case FACE_INSIDE_LEFT:
      normal.set(0., +1., 0.);
      break;
    case FACE_INSIDE_RIGHT:
      normal.set(0., -1., 0.);
      break;
    case FACE_INSIDE_BOTTOM:
      {
        if (has_bottom()) {
          normal.set(0., 0., +1.);
        }
      }
      break;
    case FACE_INSIDE_TOP:
      {
        if (has_top()) {
          normal.set(0., 0., -1.);
        }
      }
      break;
    }
    return(normal);
  }

  face_identifier extruded_box::on_surface(const vector_3d & position_,
                                           const face_identifier & surface_mask_,
                                           double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid extruded box!");
    const double skin = compute_tolerance(skin_);
    face_identifier mask;
    if (surface_mask_.is_valid()) {
      DT_THROW_IF(! surface_mask_.is_face_bits_mode(), std::logic_error,
                  "Face mask uses no face bits!");
      mask = surface_mask_;
    } else {
      mask.set_face_bits_any();
    }

    const face_info_collection_type & faces = get_computed_faces();
    for (face_info_collection_type::const_iterator iface = faces.begin();
         iface != faces.end();
         iface++) {
      const face_info & finfo = *iface;
      if (finfo.is_valid() && mask.has_face_bit( finfo.get_identifier().get_face_bits() )) {
        vector_3d position_c;
        finfo.get_positioning().mother_to_child(position_, position_c);
        if (finfo.get_face_ref().is_on_surface(position_c, skin)) {
          return finfo.get_identifier();
        }
      }
    }

    return face_identifier::face_invalid();
  }

  bool
  extruded_box::find_intercept(const vector_3d & from_,
                               const vector_3d & direction_,
                               face_intercept_info & intercept_,
                               double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid extruded box!");
    intercept_.reset ();

    double skin = compute_tolerance(skin_);

    const unsigned int NFACES = 12;
    face_intercept_info intercepts[NFACES];
    unsigned int candidate_impact_counter = 0;

    int face_counter = 0;
    const face_info_collection_type & faces = get_computed_faces();
    for (face_info_collection_type::const_iterator iface = faces.begin();
         iface != faces.end();
         iface++) {
      const face_info & finfo = *iface;
      if (!finfo.is_valid()) {
        continue;
      }
      const i_shape_2d & face = finfo.get_face_ref();
      const placement & face_placement = finfo.get_positioning();
      const face_identifier & face_id = finfo.get_identifier();
      if (face.i_find_intercept::find_intercept(from_,
                                                direction_,
                                                face_placement,
                                                intercepts[face_counter],
                                                skin)) {
        intercepts[face_counter].set_face_id(face_id);
        candidate_impact_counter++;
      }
      face_counter++;
    }

    /*
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
      intercept_.grab_face_id().set_face_bit(face_min);
      intercept_.set_impact(from_ + direction_ * t_min);
    }
    */

    if (candidate_impact_counter > 0) {
      double min_length_to_impact = -1.0;
      for (unsigned int iface = 0; iface < NFACES; iface++) {
        if (intercepts[iface].is_ok()) {
          double length_to_impact = (intercepts[iface].get_impact() - from_).mag();
          if (min_length_to_impact < 0.0 || length_to_impact < min_length_to_impact) {
            min_length_to_impact = length_to_impact;
            intercept_.set_face_id(intercepts[iface].get_face_id());
            intercept_.set_impact(intercepts[iface].get_impact());
          }
        }
      }
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
    i_shape_3d::tree_dump(out_, title_, indent_, true);

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

  /// Compute the top/bottom face and placement
  void extruded_box::compute_extruded_top_bottom_face(faces_mask_type face_id_,
                                                      composite_surface & face_,
                                                      placement & face_placement_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid extruded box!");
    face_.reset();
    face_placement_.reset();
    DT_THROW_IF(face_id_ != FACE_TOP && face_id_ != FACE_BOTTOM,
                std::logic_error,
                "Invalid face identifier (" << face_id_ << "]!");
    if (face_id_ == FACE_TOP && has_top()) {
      return;
    }
    if (face_id_ == FACE_BOTTOM && has_bottom()) {
      return;
    }

    box outer;
    compute_outer_box(outer);
    rectangle outer_tb_rect;
    placement outer_tb_rect_placement;
    box::faces_mask_type box_face_id = box::FACE_TOP;
    if (face_id_ == FACE_BOTTOM) {
      box_face_id = box::FACE_BOTTOM;
    }
    outer.compute_face(box_face_id, outer_tb_rect, outer_tb_rect_placement);
    vertex_col_type outer_tb_col_type;
    outer_tb_rect.compute_vertexes(outer_tb_col_type);
    const vector_3d & A = outer_tb_col_type[0];
    const vector_3d & B = outer_tb_col_type[1];
    const vector_3d & C = outer_tb_col_type[2];
    const vector_3d & D = outer_tb_col_type[3];

    box inner;
    placement inner_placement;
    compute_inner_box(inner, inner_placement);
    rectangle inner_tb_rect;
    placement inner_tb_rect_placement;
    inner.compute_face(box_face_id, inner_tb_rect, inner_tb_rect_placement);
    vertex_col_type inner_tb_col_type;
    inner_tb_rect.compute_vertexes(inner_tb_col_type);
    const vector_3d & E = inner_tb_col_type[0];
    const vector_3d & F = inner_tb_col_type[1];
    const vector_3d & G = inner_tb_col_type[2];
    const vector_3d & H = inner_tb_col_type[3];
    /*
     *
     *  B +----------------------+ A
     *    |\                    /|
     *    | \                  / |
     *    |  \                /  |
     *    |   +--------------+   |
     *    |   |F            E|   |
     *    |   |              |   |
     *    |   |G            H|   |
     *    |   +--------------+   |
     *    |  /                \  |
     *    | /                  \ |
     *    |/                    \|
     *  C +----------------------+ D
     *
     */


    {
      // std::cerr << "DEVEL: extruded_box::compute_extruded_top_bottom_face: FEAB..." << std::endl;
      geomtools::face_info & finfo = face_.add();
      geomtools::quadrangle & q = finfo.add_face<geomtools::quadrangle>();
      q.set_vertexes(F, E, A, B);
      finfo.set_identity_positioning();
      finfo.grab_identifier().set_face_index(0);
      // std::cerr << "DEVEL: extruded_box::compute_extruded_top_bottom_face: FEAB done." << std::endl;
    }
    {
      // std::cerr << "DEVEL: extruded_box::compute_extruded_top_bottom_face: GFBC..." << std::endl;
      geomtools::face_info & finfo = face_.add();
      geomtools::quadrangle & q = finfo.add_face<geomtools::quadrangle>();
      q.set_vertexes(G, F, B, C);
      finfo.set_identity_positioning();
      finfo.grab_identifier().set_face_index(1);
      // std::cerr << "DEVEL: extruded_box::compute_extruded_top_bottom_face: GFBC done." << std::endl;
    }
    {
      geomtools::face_info & finfo = face_.add();
      geomtools::quadrangle & q = finfo.add_face<geomtools::quadrangle>();
      q.set_vertexes(H, G, C, D);
      finfo.set_identity_positioning();
      finfo.grab_identifier().set_face_index(2);
    }
    {
      geomtools::face_info & finfo = face_.add();
      geomtools::quadrangle & q = finfo.add_face<geomtools::quadrangle>();
      q.set_vertexes(E, H, D, A);
      finfo.set_identity_positioning();
      finfo.grab_identifier().set_face_index(3);
    }

    face_placement_ = outer_tb_rect_placement;
    return;
  }

  void extruded_box::compute_outer_box(box & outer_box_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid extruded box!");
    outer_box_.set(_x_, _y_, _z_);
    return;
  }

  void extruded_box::compute_inner_box(box & inner_box_, placement & inner_box_placement_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid extruded box!");
    double z_inner = _z_ - 2 * _thickness_;
    double z_inner_pos = 0.0;
    if (!has_bottom()) {
      z_inner += _thickness_;
      z_inner_pos -= 0.5 * _thickness_;
    }
    if (!has_top()) {
      z_inner += _thickness_;
      z_inner_pos += 0.5 * _thickness_;
    }
    inner_box_.set(_x_ - 2 * _thickness_, _y_ - 2 * _thickness_, z_inner);
    inner_box_placement_.set(0.0, 0.0, z_inner_pos, 0.0, 0.0, 0.0);
    return;
  }

  unsigned int extruded_box::compute_faces(face_info_collection_type & faces_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid extruded box!");
    unsigned int nfaces = 0;

    {
      // Base outer box:
      box outer;
      compute_outer_box(outer);
      for (box::faces_mask_type face_id = box::_FACE_BEGIN;
           face_id != box::_FACE_END;
           face_id = static_cast<box::faces_mask_type>(face_id << 1)) {
        bool do_it = true;
        if (face_id == box::FACE_BOTTOM && !has_bottom()) {
          do_it = false;
        }
        if (face_id == box::FACE_TOP && !has_top()) {
          do_it = false;
        }
        if (do_it) {
          {
            face_info dummy;
            faces_.push_back(dummy);
          }
          face_info & finfo = faces_.back();
          rectangle & rect = finfo.add_face<rectangle>();
          outer.compute_face(face_id,
                             rect,
                             finfo.grab_positioning());
          face_identifier fid;
          faces_mask_type face_bit = FACE_NONE;
          if (face_id == box::FACE_BACK) {
            face_bit = FACE_BACK;
            finfo.set_label("back");
          }
          if (face_id == box::FACE_FRONT) {
            face_bit = FACE_FRONT;
            finfo.set_label("front");
          }
          if (face_id == box::FACE_LEFT) {
            face_bit = FACE_LEFT;
            finfo.set_label("left");
          }
          if (face_id == box::FACE_RIGHT) {
            face_bit = FACE_RIGHT;
            finfo.set_label("right");
          }
          if (face_id == box::FACE_TOP) {
            face_bit = FACE_TOP;
            finfo.set_label("top");
          }
          if (face_id == box::FACE_BOTTOM) {
            face_bit = FACE_BOTTOM;
            finfo.set_label("bottom");
          }
          fid.set_face_bit(face_bit);
          finfo.set_identifier(fid);
          nfaces++;
        }
      }
    }

    if (!has_top()){
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      composite_surface & cs = finfo.add_face<composite_surface>();
      compute_extruded_top_bottom_face(FACE_TOP, cs, finfo.grab_positioning());
      if (! cs.is_valid()) {
        faces_.pop_back();
      } else {
        face_identifier fid;
        fid.set_face_bit(FACE_TOP);
        finfo.set_identifier(fid);
        finfo.set_label("top");
        nfaces++;
      }
    }

    if (!has_bottom()){
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      composite_surface & cs = finfo.add_face<composite_surface>();
      compute_extruded_top_bottom_face(FACE_BOTTOM, cs, finfo.grab_positioning());
      if (! cs.is_valid()) {
        faces_.pop_back();
      } else {
        face_identifier fid;
        fid.set_face_bit(FACE_BOTTOM);
        finfo.set_identifier(fid);
        finfo.set_label("bottom");
        nfaces++;
      }
    }

    {
      box inner;
      placement inner_placement;
      compute_inner_box(inner, inner_placement);

      for (box::faces_mask_type face_id = box::_FACE_BEGIN;
           face_id != box::_FACE_END;
           face_id = static_cast<box::faces_mask_type>(face_id << 1)) {
        bool do_it = true;
        if (face_id == box::FACE_BOTTOM && !has_bottom()) {
          do_it = false;
        }
        if (face_id == box::FACE_TOP && !has_top()) {
          do_it = false;
        }
        if (do_it) {
          {
            face_info dummy;
            faces_.push_back(dummy);
          }
          face_info & finfo = faces_.back();
          rectangle & rect = finfo.add_face<rectangle>();
          inner.compute_face(face_id,
                             rect,
                             finfo.grab_positioning());
          finfo.grab_positioning().translate(inner_placement.get_translation());
          face_identifier fid;
          faces_mask_type face_bit = FACE_NONE;
          if (face_id == box::FACE_BACK) {
            face_bit = FACE_INSIDE_BACK;
            finfo.set_label("inside_back");
          }
          if (face_id == box::FACE_FRONT) {
            face_bit = FACE_INSIDE_FRONT;
            finfo.set_label("inside_front");
          }
          if (face_id == box::FACE_LEFT) {
            face_bit = FACE_INSIDE_LEFT;
            finfo.set_label("inside_left");
          }
          if (face_id == box::FACE_RIGHT) {
            face_bit = FACE_INSIDE_RIGHT;
            finfo.set_label("inside_right");
          }
          if (face_id == box::FACE_TOP) {
            face_bit = FACE_INSIDE_TOP;
            finfo.set_label("inside_top");
          }
          if (face_id == box::FACE_BOTTOM) {
            face_bit = FACE_INSIDE_BOTTOM;
            finfo.set_label("inside_bottom");
          }
          fid.set_face_bit(face_bit);
          finfo.set_identifier(fid);
          nfaces++;
        }
      }
    }

    return nfaces;
  }
  
  void extruded_box::compute_envelope(box & envelope_,
                                      double x_tolerance_,
                                      double y_tolerance_,
                                      double z_tolerance_)
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid extruded box!");
    double x_eps = 0.0;
    double y_eps = 0.0;
    double z_eps = 0.0;
    if (datatools::is_valid(x_tolerance_) and x_tolerance_ > 0.0) x_eps = x_tolerance_;
    if (datatools::is_valid(y_tolerance_) and y_tolerance_ > 0.0) y_eps = y_tolerance_;
    if (datatools::is_valid(z_tolerance_) and z_tolerance_ > 0.0) z_eps = z_tolerance_;
    double x = get_x();
    double y = get_y();
    double z = get_z();
    x += (2 * x_eps);
    y += (2 * y_eps);
    z += (2 * z_eps);
    envelope_.reset();
    envelope_.set(x, y, z);
    envelope_.lock();    
  }

  void extruded_box::generate_wires_self(wires_type & wires_,
                                         uint32_t options_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid extruded box!");

    bool draw_external = !(options_ & WR_EXTRBOX_NO_EXTERNAL_FACES);
    bool draw_internal = !(options_ & WR_EXTRBOX_NO_INTERNAL_FACES);
    bool draw_boundings   =  (options_ & WR_BASE_BOUNDINGS);

    if (draw_boundings) {
      get_bounding_data().generate_wires_self(wires_, 0);
    }

    // Keep only base rendering bits:
    uint32_t base_options = options_ & WR_BASE_MASK;

    const face_info_collection_type & faces = get_computed_faces();
    for (face_info_collection_type::const_iterator iface = faces.begin();
         iface != faces.end();
         iface++) {
      const face_info & finfo = *iface;
      if (finfo.is_valid()) {
        uint32_t face_bit = finfo.get_identifier().get_face_bits();
        if (!draw_external && face_bit <= FACE_TOP) {
          continue;
        }
        if (!draw_internal && face_bit >= FACE_TOP) {
          continue;
        }
        uint32_t options = base_options;
        finfo.get_face_ref().generate_wires(wires_, finfo.get_positioning(), options);
      }
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
