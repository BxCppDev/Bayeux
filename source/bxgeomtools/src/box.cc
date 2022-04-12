// box.cc

// Ourselves:
#include <geomtools/box.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/exception.h>
#include <datatools/units.h>
#include <datatools/properties.h>

// This project:
#include <geomtools/rectangle.h>
#include <geomtools/placement.h>
#include <geomtools/i_shape_2d.h>

namespace geomtools {

  // Registration :
  GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(box, "geomtools::box")

  const std::string & box::box_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "box";
    }
    return label;
  }

  void box::_build_bounding_data()
  {
    _grab_bounding_data().make_box(get_xmin(), get_xmax(),
                                   get_ymin(), get_ymax(),
                                   get_zmin(), get_zmax());
    DT_LOG_DEBUG(get_logging_priority(), "Box now has bounding data.");
    return;
  }

  double box::get_xmin() const
  {
    return -0.5 * _x_;
  }

  double box::get_xmax() const
  {
    return +0.5 * _x_;
  }

  double box::get_ymin() const
  {
    return -0.5 * _y_;
  }

  double box::get_ymax() const
  {
    return +0.5 * _y_;
  }

  double box::get_zmin() const
  {
    return -0.5 * _z_;
  }

  double box::get_zmax() const
  {
    return +0.5 * _z_;
  }

  double
  box::get_x() const
  {
    return _x_;
  }

  void
  box::set_x(double a_x)
  {
    DT_THROW_IF (a_x < 0.0,std::logic_error,
                 "Invalid '" << a_x << "' X value !");
    _x_ = a_x;
    return;
  }

  double
  box::get_half_x() const
  {
    return _x_ * 0.5;
  }

  void
  box::set_half_x(double a_half_x)
  {
    set_x(a_half_x + a_half_x);
    return;
  }

  double
  box::get_y() const
  {
    return _y_;
  }

  double
  box::get_half_y() const
  {
    return _y_ * 0.5;
  }

  void
  box::set_half_y(double a_half_y)
  {
    set_y(a_half_y + a_half_y);
    return;
  }

  void
  box::set_y( double a_y )
  {
    DT_THROW_IF (a_y < 0.0,std::logic_error,
                 "Invalid '" << a_y << "' Y value !");
    _y_ = a_y;
    return;
  }

  double
  box::get_z() const
  {
    return _z_;
  }

  void
  box::set_z(double a_z)
  {
    DT_THROW_IF (a_z < 0.0,std::logic_error,
                 "Invalid '" << a_z << "' Z value !");
    _z_ = a_z;
    return;
  }

  double
  box::get_half_z() const
  {
    return _z_ * 0.5;
  }

  void
  box::set_half_z(double a_half_z)
  {
    set_z(a_half_z + a_half_z);
    return;
  }

  void
  box::set(double a_x, double a_y, double a_z)
  {
    set_x(a_x);
    set_y(a_y);
    set_z(a_z);
    return;
  }

  void
  box::set_half(double a_half_x, double a_half_y, double a_half_z)
  {
    set_half_x(a_half_x);
    set_half_y(a_half_y);
    set_half_z(a_half_z);
    return;
  }

  box::box() : i_shape_3d(DEFAULT_SKIN)
  {
    _set_defaults();
    return;
  }

  box::box(double a_x, double a_y,double a_z ) : i_shape_3d(DEFAULT_SKIN)
  {
    _set_defaults();
    set_x(a_x);
    set_y(a_y);
    set_z(a_z);
    return;
  }

  box::~box()
  {
    return;
  }

  std::string
  box::get_shape_name() const
  {
    return box_label();
  }

  double
  box::get_surface(uint32_t a_mask) const
  {
    double s = 0.0;
    uint32_t mask = a_mask;

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
    if (mask & FACE_BOTTOM) {
      s += _x_ * _y_;
    }
    if (mask & FACE_TOP) {
      s += _x_ * _y_;
    }
    return s;
  }

  double
  box::get_parameter(const std::string & a_flag) const
  {
    if (a_flag == "x") return get_x();
    if (a_flag == "y") return get_y();
    if (a_flag == "z") return get_z();
    if (a_flag == "volume")         return get_volume();
    if (a_flag == "surface.top")    return get_surface(FACE_TOP);
    if (a_flag == "surface.bottom") return get_surface(FACE_BOTTOM);
    if (a_flag == "surface.back")   return get_surface(FACE_BACK);
    if (a_flag == "surface.front")  return get_surface(FACE_FRONT);
    if (a_flag == "surface.left")   return get_surface(FACE_LEFT);
    if (a_flag == "surface.right")  return get_surface(FACE_RIGHT);
    if (a_flag == "surface")        return get_surface(FACE_ALL);

    DT_THROW_IF (true, std::logic_error, "Unknown flag '" << a_flag << "' !");
  }

  double
  box::get_volume(uint32_t /*flags*/) const
  {
    return _x_ * _y_ * _z_;
  }

  bool
  box::is_valid() const
  {
    return datatools::is_valid(_x_)
      && datatools::is_valid(_y_)
      && datatools::is_valid(_z_);
  }

  void box::initialize(const datatools::properties & config_,
                       const handle_dict_type * objects_)
  {
    this->i_shape_3d::_initialize(config_, objects_);
    DT_LOG_DEBUG(get_logging_priority(), "Box validity = " << is_valid());

    if (! is_valid()) {

      double lunit = CLHEP::mm;
      if (config_.has_key("length_unit")) {
        const std::string lunit_str = config_.fetch_string("length_unit");
        lunit = datatools::units::get_length_unit_from(lunit_str);
      }

      DT_THROW_IF(! config_.has_key("x"), std::logic_error, "Missing box 'x' property !");
      double x = config_.fetch_real("x");
      if (! config_.has_explicit_unit("x")) {
        x *= lunit;
      }

      DT_THROW_IF(! config_.has_key("y"), std::logic_error, "Missing box 'y' property !");
      double y = config_.fetch_real("y");
      if (! config_.has_explicit_unit("y")) {
        y *= lunit;
      }

      DT_THROW_IF(! config_.has_key("z"), std::logic_error, "Missing box 'z' property !");
      double z = config_.fetch_real("z");
      if (! config_.has_explicit_unit("z")) {
        z *= lunit;
      }

      set(x, y, z);
    }

    lock();
    return;
  }

  void box::_set_defaults()
  {
    datatools::invalidate(_x_);
    datatools::invalidate(_y_);
    datatools::invalidate(_z_);
    return;
  }

  void box::reset()
  {
    unlock();
    _set_defaults();
    this->i_shape_3d::_reset();
    return;
  }

  unsigned int box::compute_faces(face_info_collection_type & faces_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid box!");
    unsigned int nfaces = 0;
    for (faces_mask_type face_id = _FACE_BEGIN;
         face_id != _FACE_END;
         face_id = static_cast<faces_mask_type>(face_id << 1)) {
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      rectangle & rect = finfo.add_face<rectangle>();
      compute_face(face_id, rect, finfo.grab_positioning());
      face_identifier fid;
      fid.set_face_bit(face_id);
      finfo.set_identifier(fid);
      switch (face_id) {
      case FACE_BACK:
        finfo.set_label("back");
        break;
      case FACE_FRONT:
        finfo.set_label("front");
        break;
      case FACE_LEFT:
        finfo.set_label("left");
        break;
      case FACE_RIGHT:
        finfo.set_label("right");
        break;
      case FACE_BOTTOM:
        finfo.set_label("bottom");
        break;
      case FACE_TOP:
        finfo.set_label("top");
        break;
      default:
        break;
      }
      nfaces++;
    }
    return nfaces;
  }

  void box::compute_face(faces_mask_type face_id_,
                         rectangle & face_,
                         placement & face_placement_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid box!");
    switch(face_id_) {
    case FACE_BACK:
      face_.set_x(_z_);
      face_.set_y(_y_);
      face_placement_.set(-0.5 * _x_, 0.0, 0.0,
                          AXIS_Y,
                          get_special_rotation_angle(ROTATION_ANGLE_270));
      break;
    case FACE_FRONT:
      face_.set_x(_z_);
      face_.set_y(_y_);
      face_placement_.set(+0.5 * _x_, 0.0, 0.0,
                          AXIS_Y,
                          get_special_rotation_angle(ROTATION_ANGLE_90));
      break;
    case FACE_LEFT:
      face_.set_x(_z_);
      face_.set_y(_x_);
      face_placement_.set(0.0, -0.5 * _y_, 0.0,
                          get_special_rotation_angle(ROTATION_ANGLE_90),
                          get_special_rotation_angle(ROTATION_ANGLE_270),
                          0.0);
      break;
    case FACE_RIGHT:
      face_.set_x(_z_);
      face_.set_y(_x_);
      face_placement_.set(0.0, +0.5 * _y_, 0.0,
                          get_special_rotation_angle(ROTATION_ANGLE_90),
                          get_special_rotation_angle(ROTATION_ANGLE_90),
                          0.0);
      break;
    case FACE_BOTTOM:
      face_.set_x(_x_);
      face_.set_y(_y_);
      face_placement_.set(0.0, 0.0, -0.5 * _z_,
                          0.0, get_special_rotation_angle(ROTATION_ANGLE_180), 0.0);
      break;
    case FACE_TOP:
      face_.set_x(_x_);
      face_.set_y(_y_);
      face_placement_.set(0.0, 0.0, +0.5 * _z_,
                          0.0, 0.0, 0.0);
      break;
    default:
      DT_THROW(std::logic_error, "Invalid box face bit!");
    }
    return;
  }

  void box::compute_deflated(box & deflated_,
                             double by_x_,
                             double by_y_,
                             double by_z_)
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid box!");
    deflated_.reset();
    double x_eps = 0.0;
    double y_eps = 0.0;
    double z_eps = 0.0;
    if (datatools::is_valid(by_x_) and by_x_ > 0.0) x_eps = by_x_;
    if (datatools::is_valid(by_y_) and by_y_ > 0.0) y_eps = by_y_;
    if (datatools::is_valid(by_z_) and by_z_ > 0.0) z_eps = by_z_;
    double x = get_x();
    double y = get_y();
    double z = get_z();
    x -= (2 * x_eps);
    y -= (2 * y_eps);
    z -= (2 * z_eps);
    deflated_.set(x, y, z);
    deflated_.lock();
    return;
  }

  void box::compute_inflated(box & inflated_,
                             double by_x_,
                             double by_y_,
                             double by_z_)
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid box!");
    double x_eps = 0.0;
    double y_eps = 0.0;
    double z_eps = 0.0;
    if (datatools::is_valid(by_x_) and by_x_ > 0.0) x_eps = by_x_;
    if (datatools::is_valid(by_y_) and by_y_ > 0.0) y_eps = by_y_;
    if (datatools::is_valid(by_z_) and by_z_ > 0.0) z_eps = by_z_;
    double x = get_x();
    double y = get_y();
    double z = get_z();
    x += (2 * x_eps);
    y += (2 * y_eps);
    z += (2 * z_eps);
    inflated_.reset();
    inflated_.set(x, y, z);
    inflated_.lock();
    return;
  }

  void box::compute_envelope(box & envelope_,
                             double tolerance_x_,
                             double tolerance_y_,
                             double tolerance_z_)
  {
    compute_inflated(envelope_, tolerance_x_, tolerance_y_, tolerance_z_);
  }
  
  bool
  box::is_outside(const vector_3d & a_position, double a_skin) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid box!");
    double skin = get_skin(a_skin);
    double hskin = 0.5 * skin;
    if (   (std::abs(a_position.x()) >= (0.5 * _x_ + hskin))
           || (std::abs(a_position.y()) >= (0.5 * _y_ + hskin))
           || (std::abs(a_position.z()) >= (0.5 * _z_ + hskin))
           ) {
      return true;
    }
    return false;
  }

  bool
  box::is_inside(const vector_3d & a_position, double a_skin) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid box!");
    double skin = get_skin(a_skin);
    double hskin = 0.5 * skin;
    if (   (std::abs(a_position.x()) <= (0.5 * _x_ - hskin))
           && (std::abs(a_position.y()) <= (0.5 * _y_ - hskin))
           && (std::abs(a_position.z()) <= (0.5 * _z_ - hskin))
           ) {
      return true;
    }
    return false;
  }

  // virtual
  face_identifier box::on_surface(const vector_3d & a_position,
                                  const face_identifier & a_surface_mask,
                                  double a_skin) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid box!");
    double skin = get_skin(a_skin);

    face_identifier mask;
    if (a_surface_mask.is_valid()) {
      DT_THROW_IF(! a_surface_mask.is_face_bits_mode(), std::logic_error,
                  "Face mask uses no face bits!");
      mask = a_surface_mask;
    } else {
      mask.set_face_bits_any();
    }

    const face_info_collection_type & faces = get_computed_faces();
    for (face_info_collection_type::const_iterator iface = faces.begin();
         iface != faces.end();
         iface++) {
      const face_info & finfo = *iface;
      if (finfo.is_valid() && mask.has_face_bit(finfo.get_identifier().get_face_bits())) {
        vector_3d position_c;
        finfo.get_positioning().mother_to_child(a_position, position_c);
        if (finfo.get_face_ref().is_on_surface(position_c, skin)) {
          return finfo.get_identifier();
        }
      }
    }

    return face_identifier::face_invalid();
  }

  vector_3d
  box::get_normal_on_surface(const vector_3d & /*a_position*/,
                             const face_identifier & a_surface_bit) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid box!");
    vector_3d normal;
    geomtools::invalidate(normal);
    switch(a_surface_bit.get_face_bits()) {
    case FACE_BACK:
      normal.set(-1.0, 0.0, 0.0);
      break;
    case FACE_FRONT:
      normal.set(+1.0, 0.0, 0.0);
      break;
    case FACE_LEFT:
      normal.set(0.0, -1.0, 0.0);
      break;
    case FACE_RIGHT:
      normal.set(0.0, +1.0, 0.0);
      break;
    case FACE_BOTTOM:
      normal.set(0.0, 0.0, -1.0);
      break;
    case FACE_TOP:
      normal.set(0.0, 0.0, +1.0);
      break;
    }
    return(normal);
  }

  bool box::find_intercept(const vector_3d & from_,
                           const vector_3d & direction_,
                           face_intercept_info & intercept_,
                           double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid box!");
    intercept_.reset();
    double skin = compute_tolerance(skin_);

    const unsigned int NFACES = 6;
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

    if (candidate_impact_counter > 0) {
      double min_length_to_impact = -1.0;
      for (unsigned int iface = 0; iface < NFACES; iface++) {
        if (intercepts[iface].is_ok()) {
          double length_to_impact = (intercepts[iface].get_impact() - from_).mag();
          if (min_length_to_impact < 0.0 || length_to_impact < min_length_to_impact) {
            min_length_to_impact = length_to_impact;
            intercept_.grab_face_id().set_face_bit(intercepts[iface].get_face_id().get_face_bits());
            intercept_.set_impact(intercepts[iface].get_impact());
          }
        }
      }
    }

    return intercept_.is_ok ();
  }

  std::ostream &
  operator<<(std::ostream & a_out, const box & a_box)
  {
    a_out << '{' << box::box_label() << ' '
          << a_box._x_ << ' '
          << a_box._y_ << ' '
          << a_box._z_ << '}';
    return a_out;
  }

  std::istream &
  operator>>(std::istream & a_in, box & a_box)
  {
    a_box.reset();
    char c = 0;
    a_in.get(c);
    if (c != '{') {
      a_in.clear(std::ios_base::failbit);
      return a_in;
    }
    std::string name;
    a_in >> name;
    if (name != box::box_label()) {
      a_in.clear(std::ios_base::failbit);
      return a_in;
    }
    double x, y, z;
    a_in >> x >> y >> z;
    if (! a_in) {
      a_in.clear(std::ios_base::failbit);
      return a_in;
    }
    c = 0;
    a_in.get(c);
    if (c != '}') {
      a_in.clear(std::ios_base::failbit);
      return a_in;
    }
    try {
      a_box.set(x,y,z);
    } catch (...) {
      a_box.reset();
      a_in.clear(std::ios_base::failbit);
    }
    return a_in;
  }


  void box::tree_dump(std::ostream & a_out,
                      const std::string & a_title,
                      const std::string & a_indent,
                      bool a_inherit) const
  {
    i_shape_3d::tree_dump(a_out, a_title, a_indent, true);

    a_out << a_indent << datatools::i_tree_dumpable::tag
          << "X : " << get_x() / CLHEP::mm << " mm" << std::endl;
    a_out << a_indent << datatools::i_tree_dumpable::tag
          << "Y : " << get_y() / CLHEP::mm << " mm" << std::endl;
    a_out << a_indent << datatools::i_tree_dumpable::inherit_tag(a_inherit)
          << "Z : " << get_z() / CLHEP::mm << " mm" << std::endl;
    return;
  }

  void box::generate_wires_self(wires_type & wires_,
                                uint32_t options_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid box!");
    // bool debug_explode = options_ & WR_BASE_EXPLODE;
    bool draw_back   = !(options_ & WR_BOX_NO_BACK_FACE);
    bool draw_front  = !(options_ & WR_BOX_NO_FRONT_FACE);
    bool draw_left   = !(options_ & WR_BOX_NO_LEFT_FACE);
    bool draw_right  = !(options_ & WR_BOX_NO_RIGHT_FACE);
    bool draw_bottom = !(options_ & WR_BOX_NO_BOTTOM_FACE);
    bool draw_top    = !(options_ & WR_BOX_NO_TOP_FACE);

    // Keep only base rendering bits:
    uint32_t base_options = options_ & WR_BASE_MASK;
    DT_LOG_DEBUG(datatools::logger::PRIO_ALWAYS, "base_options=" << base_options);
    // bool edge00 = false;
    // bool edge01 = false;
    // bool edge02 = false;
    // bool edge03 = false;
    // bool edge04 = false;
    // bool edge05 = false;
    // bool edge06 = false;
    // bool edge07 = false;
    // bool edge08 = false;
    // bool edge09 = false;
    // bool edge10 = false;
    // bool edge11 = false;

    // double explode_factor = 1.0;
    // if (debug_explode) {
    //   explode_factor = 1.5;
    // }
    if (draw_bottom) {
      rectangle face;
      placement face_placement;
      compute_face(FACE_BOTTOM, face, face_placement);
      uint32_t options = base_options;
      // edge00 = true;
      // edge02 = true;
      // edge04 = true;
      // edge06 = true;
      face.generate_wires(wires_, face_placement, options);
    }

    if (draw_top) {
      rectangle face;
      placement face_placement;
      compute_face(FACE_TOP, face, face_placement);
      uint32_t options = base_options;
      // edge01 = true;
      // edge03 = true;
      // edge05 = true;
      // edge07 = true;
      face.generate_wires(wires_, face_placement, options);
    }

    if (draw_back) {
      rectangle face;
      placement face_placement;
      compute_face(FACE_BACK, face, face_placement);
      uint32_t options = base_options;
      // if (edge00) options |= rectangle::WR_RECT_NO_XMINUS_SIDE;
      // else edge00 = true;
      // if (edge01) options |= rectangle::WR_RECT_NO_XPLUS_SIDE;
      // else edge01 = true;
      // edge00 = true;
      // edge01 = true;
      // edge08 = true;
      // edge09 = true;
      face.generate_wires(wires_, face_placement, options);
    }

    if (draw_front) {
      rectangle face;
      placement face_placement;
      compute_face(FACE_FRONT, face, face_placement);
      uint32_t options = base_options;
      // if (edge02) options |= rectangle::WR_RECT_NO_XMINUS_SIDE;
      // else edge02 = true;
      // if (edge03) options |= rectangle::WR_RECT_NO_XPLUS_SIDE;
      // else edge03 = true;
      // edge02 = true;
      // edge03 = true;
      // edge10 = true;
      // edge11 = true;
      face.generate_wires(wires_, face_placement, options);
    }

    if (draw_left) {
      rectangle face;
      placement face_placement;
      compute_face(FACE_LEFT, face, face_placement);
      uint32_t options = base_options;
      // if (edge04) options |= rectangle::WR_RECT_NO_XMINUS_SIDE;
      // else edge04 = true;
      // if (edge05) options |= rectangle::WR_RECT_NO_XPLUS_SIDE;
      // else edge05 = true;
      // if (edge08) options |= rectangle::WR_RECT_NO_YMINUS_SIDE;
      // else edge08 = true;
      // if (edge10) options |= rectangle::WR_RECT_NO_YPLUS_SIDE;
      // else edge10 = true;
      // edge04 = true;
      // edge05 = true;
      // edge08 = true;
      // edge10 = true;
      face.generate_wires(wires_, face_placement, options);
    }

    if (draw_right) {
      rectangle face;
      placement face_placement;
      compute_face(FACE_RIGHT, face, face_placement);
      uint32_t options = base_options;
      // if (edge06) options |= rectangle::WR_RECT_NO_XMINUS_SIDE;
      // else edge06 = true;
      // if (edge07) options |= rectangle::WR_RECT_NO_XPLUS_SIDE;
      // else edge07 = true;
      // if (edge09) options |= rectangle::WR_RECT_NO_YMINUS_SIDE;
      // else edge09 = true;
      // if (edge11) options |= rectangle::WR_RECT_NO_YPLUS_SIDE;
      // else edge11 = true;
      // edge06 = true;
      // edge07 = true;
      // edge09 = true;
      // edge11 = true;
      face.generate_wires(wires_, face_placement, options);
    }

    return;
  }

  void box::compute_vertexes(std::vector<vector_3d> & vv_) const
  {
    DT_THROW_IF(!is_valid(), std::logic_error, "Invalid box!");
    vv_.clear();
    vv_.reserve(8);
    double dx = 0.5 * get_x();
    double dy = 0.5 * get_y();
    double dz = 0.5 * get_z();
    for (int i = 0; i < 2; i++) {
      vector_3d vertex[4];
      vertex[0].set( dx,  dy, (-1 + 2 * i) * dz);
      vertex[1].set(-dx,  dy, (-1 + 2 * i) * dz);
      vertex[2].set(-dx, -dy, (-1 + 2 * i) * dz);
      vertex[3].set( dx, -dy, (-1 + 2 * i) * dz);
      for (int j = 0; j < 4; j++) {
        vv_.push_back(vertex[j]);
      }
    }
    return;
  }

  void box::compute_transformed_vertexes(const placement & p_,
                                         std::vector<vector_3d> & vv_) const
  {
    vv_.clear();
    std::vector<vector_3d> vv;
    compute_vertexes(vv);
    vv_.reserve(vv.size());
    for (int i = 0; i < (int) vv.size(); i++) {
      {
        vector_3d dummy;
        vv_.push_back(dummy);
      }
      vector_3d & last_vtx = vv_.back();
      p_.child_to_mother(vv[i], last_vtx);
    }
    return;
  }

  // static
  void box::init_ocd(datatools::object_configuration_description & ocd_)
  {
    i_shape_3d::init_ocd(ocd_);

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("length_unit")
        .set_from("geomtools::box")
        .set_terse_description("The unit symbol used for length unit")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_default_value_string("mm")
        .set_long_description("A character string that represents the default \n"
                              "length unit for length parameters of the box.  \n"
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
        .set_from("geomtools::box")
        .set_terse_description("The dimension of the box along the X axis")
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
        .set_from("geomtools::box")
        .set_terse_description("The dimension of the box along the Y axis")
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
        .set_from("geomtools::box")
        .set_terse_description("The dimension of the box along the Z axis")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(true)
        .add_example("Set the Z dimension::          \n"
                     "                               \n"
                     "  z : real as length = 5.0 mm  \n"
                     "                               \n"
                     )
        ;
    }

    return;
  }

} // end of namespace geomtools

/** Opening macro for implementation
 *  @arg geomtools::box the full class name
 *  @arg ocd_ is the identifier of the 'datatools::object_configuration_description'
 *            to be initialized(passed by mutable reference).
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(geomtools::box, ocd_)
{
  // The class name :
  ocd_.set_class_name("geomtools::box");

  // The class terse description :
  ocd_.set_class_description("A class representing a 3D box solid shape");

  // The library the class belongs to :
  ocd_.set_class_library("geomtools");

  // The class detailed documentation :
  ocd_.set_class_documentation("The box solid 3D-shape is defined by its three dimensions\n"
                               "along the X, Y and Z axis.                               \n"
                               );

  // Populate the specific OCD :
  geomtools::box::init_ocd(ocd_);

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Here is a full configuration example in the      \n"
                               "``datatools::properties`` ASCII format::         \n"
                               "                                                 \n"
                               "  tolerance.length : real = 1.0e-6 um            \n"
                               "  length_unit : string = \"mm\"                  \n"
                               "  x : real as length = 25.0 cm                   \n"
                               "  y : real           = 12.0                      \n"
                               "  z : real           =  2.5                      \n"
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

// Registration macro for class 'geomtools::box' :
DOCD_CLASS_SYSTEM_REGISTRATION(geomtools::box, "geomtools::box")
