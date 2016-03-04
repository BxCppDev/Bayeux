/// \file geomtools/elliptical_cylinder.cc

// Ourselves:
#include <geomtools/elliptical_cylinder.h>

// Standard library:
#include <cmath>
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/units.h>

// This project:
#include <geomtools/elliptical_cylinder_sector.h>
#include <geomtools/elliptical_sector.h>

namespace geomtools {

  // Registration :
  GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(elliptical_cylinder,
                                             "geomtools::elliptical_cylinder")

  const std::string & elliptical_cylinder::elliptical_cylinder_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "elliptical_cylinder";
    }
    return label;
  }

  void elliptical_cylinder::compute_top_bottom_face(faces_mask_type face_id_,
                                                    elliptical_sector & face_,
                                                    placement & face_placement_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Elliptical cylinder is not valid !");
    face_.reset();
    face_placement_.reset();
    switch (face_id_) {
    case FACE_BOTTOM:
      face_.set_radii(_x_radius_, _y_radius_);
      face_placement_.set(0.0, 0.0, -0.5 * get_z(), 0.0, 0.0, 0.0);
      break;
    case FACE_TOP:
      face_.set_radii(_x_radius_, _y_radius_);
      face_placement_.set(0.0, 0.0, +0.5 * get_z(), 0.0, 0.0, 0.0);
      break;
    default:
      DT_THROW(std::logic_error, "Invalid top/bottom mask!");
    }
    return;
  }

  /// Compute the side face and placement
  void elliptical_cylinder::compute_side_face(elliptical_cylinder_sector & face_,
                                              placement & face_placement_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Elliptical cylinder is not valid !");
    face_.reset();
    face_placement_.reset();
    face_.set(_x_radius_, _y_radius_, _z_);
    face_placement_.set(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
    return;
  }

  // virtual
  unsigned int elliptical_cylinder::compute_faces(face_info_collection_type & faces_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Elliptical cylinder is not valid !");
    unsigned int nfaces = 0;

    {
      // Side face:
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      elliptical_cylinder_sector & ecs = finfo.add_face<elliptical_cylinder_sector>();
      compute_side_face(ecs, finfo.grab_positioning());
      face_identifier fid;
      fid.set_face_bit(FACE_SIDE);
      finfo.set_identifier(fid);
      finfo.set_label("side");
      nfaces++;
    }

    {
      // Bottom face:
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      elliptical_sector & es = finfo.add_face<elliptical_sector>();
      compute_top_bottom_face(FACE_BOTTOM, es, finfo.grab_positioning());
      face_identifier fid;
      fid.set_face_bit(FACE_BOTTOM);
      finfo.set_identifier(fid);
      finfo.set_label("bottom");
      nfaces++;
    }

    {
      // Top face:
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      elliptical_sector & es = finfo.add_face<elliptical_sector>();
      compute_top_bottom_face(FACE_TOP, es, finfo.grab_positioning());
      face_identifier fid;
      fid.set_face_bit(FACE_TOP);
      finfo.set_identifier(fid);
      finfo.set_label("top");
      nfaces++;
    }

    return nfaces;
  }

  void elliptical_cylinder::_build_bounding_data()
  {
    _grab_bounding_data().make_box(get_xmin(), get_xmax(),
                                   get_ymin(), get_ymax(),
                                   get_zmin(), get_zmax());
    return;
  }

  double elliptical_cylinder::get_xmin () const
  {
    return -_x_radius_;
  }

  double elliptical_cylinder::get_xmax () const
  {
    return _x_radius_;
  }

  double elliptical_cylinder::get_ymin () const
  {
    return -_y_radius_;
  }

  double elliptical_cylinder::get_ymax () const
  {
    return _y_radius_;
  }

  double elliptical_cylinder::get_zmin () const
  {
    return -0.5*_z_;
  }

  double elliptical_cylinder::get_zmax () const
  {
    return +0.5*_z_;
  }

  double elliptical_cylinder::get_z () const
  {
    return _z_;
  }

  void elliptical_cylinder::set_z (double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0,std::logic_error ,
                 "Invalid '" << new_value_ << "' Z value !");
    _z_ = new_value_;
    return;
  }

  double elliptical_cylinder::get_half_z () const
  {
    return 0.5 * _z_;
  }

  void elliptical_cylinder::set_half_z (double new_value_)
  {
    set_z (new_value_ + new_value_);
    return;
  }

  double elliptical_cylinder::get_x_radius () const
  {
    return _x_radius_;
  }

  double elliptical_cylinder::get_y_radius () const
  {
    return _y_radius_;
  }

  void elliptical_cylinder::set_x_radius (double xr_)
  {
    DT_THROW_IF (xr_ < 0.0, std::logic_error, "Invalid inner radius '" << xr_ << "' !");
    _x_radius_ = xr_;
    return;
  }

  void elliptical_cylinder:: set_y_radius (double yr_)
  {
    DT_THROW_IF (yr_ < 0.0, std::logic_error, "Invalid inner radius '" << yr_ << "' !");
    _y_radius_ = yr_;
    return;
  }

  void elliptical_cylinder::set (double xr_, double yr_, double z_)
  {
    set_x_radius (xr_);
    set_y_radius (yr_);
    set_z (z_);
    return;
  }

  elliptical_cylinder::elliptical_cylinder ()
  {
    _set_default();
    return;
  }

  elliptical_cylinder::elliptical_cylinder (double xr_, double yr_, double z_)
  {
    _set_default();
    set(xr_, yr_, z_);
    return;
  }

  elliptical_cylinder::~elliptical_cylinder ()
  {
    reset();
    return;
  }

  double elliptical_cylinder::get_surface (uint32_t mask_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Elliptical cylinder is not valid !");
    double s = 0.0;
    if ( mask_ & FACE_SIDE ) {
      // Ramanujan's approximation
      s += M_PI * ( 3 * (_x_radius_+_y_radius_)
                    - std::sqrt( (3*_x_radius_+_y_radius_)*(_x_radius_+3*_y_radius_) ) ) * _z_;
    }
    if ( mask_ & FACE_BOTTOM ) {
      s += M_PI * _x_radius_ * _y_radius_;
    }
    if ( mask_ & FACE_TOP ) {
      s += M_PI * _x_radius_ * _y_radius_;
    }
    return s;
  }

  double elliptical_cylinder::get_volume (uint32_t /*flags*/) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Elliptical cylinder is not valid !");
    return M_PI * _x_radius_ * _y_radius_ * _z_;
  }

  bool elliptical_cylinder::is_valid () const
  {
    return datatools::is_valid(_x_radius_) && datatools::is_valid(_y_radius_) && datatools::is_valid(_z_);
  }

  void elliptical_cylinder::_set_default()
  {
    datatools::invalidate(_x_radius_);
    datatools::invalidate(_y_radius_);
    datatools::invalidate(_z_);
    return;
  }

  void elliptical_cylinder::reset ()
  {
    DT_LOG_DEBUG(get_logging_priority(), "Reset shape type '" << get_shape_name() << "'");
    unlock();

    _set_default();

    this->i_shape_3d::_reset();
    return;
  }

  std::string elliptical_cylinder::get_shape_name () const
  {
    return elliptical_cylinder::elliptical_cylinder_label();
  }

  double elliptical_cylinder::get_parameter ( const std::string & flag_ ) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Elliptical cylinder is not valid !");
    if ( flag_ == "x_radius" ) return get_x_radius ();
    if ( flag_ == "y_radius" ) return get_y_radius ();
    if ( flag_ == "z" ) return get_z ();
    if ( flag_ == "volume" ) return get_volume ();
    if ( flag_ == "surface.top" ) return get_surface (FACE_TOP);
    if ( flag_ == "surface.bottom" ) return get_surface (FACE_BOTTOM);
    if ( flag_ == "surface.side" ) return get_surface (FACE_SIDE);
    if ( flag_ == "surface" ) return get_surface (FACE_ALL);
    DT_THROW_IF(true, std::runtime_error, "Unknown flag !");
  }

  bool elliptical_cylinder::is_inside ( const vector_3d & point_ , double skin_ ) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Elliptical cylinder is not valid !");
    double skin = get_skin (skin_);
    double hskin = 0.5 * skin;

    if ( (point_.x()*point_.x())/((_x_radius_-hskin)*(_x_radius_-hskin))
         + (point_.y()*point_.y())/((_y_radius_-hskin)*(_y_radius_-hskin)) < 1.
         && (std::abs(point_.z()) <= (0.5*_z_-hskin))
         ) return true;
    return false;
  }

  bool elliptical_cylinder::is_outside ( const vector_3d & point_ , double skin_ ) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Elliptical cylinder is not valid !");
    double skin = get_skin (skin_);
    double hskin = 0.5 * skin;

    if ( (point_.x()*point_.x())/((_x_radius_+hskin)*(_x_radius_+hskin))
         + (point_.y()*point_.y())/((_y_radius_+hskin)*(_y_radius_+hskin)) > 1.
         || (std::abs(point_.z()) <= (0.5*_z_-hskin))
         ) return true;
    return false;
  }

  vector_3d
  elliptical_cylinder::get_normal_on_surface (const vector_3d & a_position,
                                              const face_identifier & a_surface_bit) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid elliptical cylinder!");
    vector_3d normal;
    invalidate(normal);
    switch (a_surface_bit.get_face_bits()) {
    case FACE_SIDE:
      normal.set (a_position.x() / (_x_radius_*_x_radius_),
                  a_position.y() / (_y_radius_*_y_radius_), 0.0);
      break;
    case FACE_BOTTOM:
      normal.set(0.0, 0.0, -1.0);
      break;
    case FACE_TOP:
      normal.set(0.0, 0.0, +1.0);
      break;
    }
    return normal;
  }

  face_identifier elliptical_cylinder::on_surface(const vector_3d & a_position,
                                                  const face_identifier & a_surface_mask,
                                                  double a_skin) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Elliptical cylinder is not valid !");
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

    /*

      double skin = compute_tolerance(a_skin);
      double angular_tolerance = get_angular_tolerance();

      uint32_t mask = a_surface_mask.get_face_bits() & elliptical_cylinder::FACE_ALL;

      if (mask & FACE_SIDE) {
      elliptical_cylinder_sector side;
      placement side_placement;
      compute_side_face(side, side_placement);
      vector_3d p_side;
      side_placement.mother_to_child(a_point, p_side);
      if (side.is_on_surface(p_side, skin)) {
      return face_identifier(FACE_SIDE);
      }
      }

      if (mask & FACE_BOTTOM) {
      elliptical_sector bottom;
      placement bottom_placement;
      compute_top_bottom_face(FACE_BOTTOM, bottom, bottom_placement);
      vector_3d p_bottom;
      bottom_placement.mother_to_child(a_point, p_bottom);
      if (bottom.is_on_surface(p_bottom, skin)) {
      return face_identifier(FACE_BOTTOM);
      }
      }

      if (mask & FACE_TOP) {
      elliptical_sector top;
      placement top_placement;
      compute_top_bottom_face(FACE_TOP, top, top_placement);
      vector_3d p_top;
      top_placement.mother_to_child(a_point, p_top);
      if (top.is_on_surface(p_top, skin)) {
      return face_identifier(FACE_TOP);
      }
      }
    */

    return face_identifier(FACE_NONE);
  }

  bool elliptical_cylinder::find_intercept (const vector_3d & from_,
                                            const vector_3d & direction_,
                                            face_intercept_info & intercept_,
                                            double skin_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Elliptical cylinder is not valid !");
    intercept_.reset ();
    double skin = compute_tolerance(skin_);

    const unsigned int NFACES = 3;
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
      {
      const int FACE_INDEX = 0;
      elliptical_cylinder_sector side_face;
      placement side_face_placement;
      compute_side_face(side_face, side_face_placement);
      if (side_face.i_find_intercept::find_intercept(from_,
      direction_,
      side_face_placement,
      intercepts[FACE_INDEX], skin)) {
      intercepts[FACE_INDEX].grab_face_id().set_face_bit(FACE_SIDE);
      candidate_impact_counter++;
      }
      }

      {
      const int FACE_INDEX = 1;
      elliptical_sector bottom_face;
      placement bottom_face_placement;
      compute_top_bottom_face(FACE_BOTTOM, bottom_face, bottom_face_placement);
      if (bottom_face.i_find_intercept::find_intercept(from_,
      direction_,
      bottom_face_placement,
      intercepts[FACE_INDEX],
      skin)) {
      intercepts[FACE_INDEX].grab_face_id().set_face_bit(FACE_BOTTOM);
      candidate_impact_counter++;
      }
      }

      {
      const int FACE_INDEX = 2;
      elliptical_sector top_face;
      placement top_face_placement;
      compute_top_bottom_face(FACE_TOP, top_face, top_face_placement);
      if (top_face.i_find_intercept::find_intercept(from_,
      direction_,
      top_face_placement,
      intercepts[FACE_INDEX],
      skin)) {
      intercepts[FACE_INDEX].grab_face_id().set_face_bit(FACE_TOP);
      candidate_impact_counter++;
      }
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

    return intercept_.is_ok ();
  }

  std::ostream & operator<< (std::ostream & out_, const elliptical_cylinder & t_)
  {
    out_ << '{' << elliptical_cylinder::elliptical_cylinder_label() << ' '
         << t_._x_radius_ << ' '
         << t_._y_radius_ << ' '
         << t_._z_ << '}';
    return out_;
  }

  std::istream & operator>> (std::istream & in_, elliptical_cylinder & t_)
  {
    t_.reset ();
    char c = 0;
    in_.get (c);
    if (c != '{')
      {
        in_.clear (std::ios_base::failbit);
        return in_;
      }
    std::string name;
    in_ >> name;
    if (name != elliptical_cylinder::elliptical_cylinder_label())
      {
        in_.clear (std::ios_base::failbit);
        return in_;
      }
    double xr, yr, z;
    in_ >> xr >> yr >> z;
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
        t_.set (xr, yr, z);
      }
    catch (...)
      {
        t_.reset ();
        in_.clear (std::ios_base::failbit);
      }
    return in_;
  }

  void elliptical_cylinder::tree_dump (std::ostream & out_,
                                       const std::string & title_,
                                       const std::string & indent_,
                                       bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty ()) indent = indent_;
    i_shape_3d::tree_dump (out_, title_, indent_, true);

    out_ << indent << datatools::i_tree_dumpable::tag
         << "X radius : " << get_x_radius () / CLHEP::mm << " mm" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Y radius : " << get_y_radius () / CLHEP::mm << " mm" << std::endl;
    out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
         << "Z : " << get_z () / CLHEP::mm << " mm" << std::endl;
    return;
  }

  void elliptical_cylinder::generate_wires_self(wires_type & wires_,
                                                uint32_t options_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid cylinder!");
    // bool debug_explode  =  (options_ & WR_BASE_EXPLODE);
    bool draw_bottom    = !(options_ & WR_ELLCYL_NO_BOTTOM_FACE);
    bool draw_top       = !(options_ & WR_ELLCYL_NO_TOP_FACE);
    bool draw_side      = !(options_ & WR_ELLCYL_NO_SIDE_FACE);
    bool draw_boundings =  (options_ & WR_BASE_BOUNDINGS);

    if (draw_boundings) {
      get_bounding_data().generate_wires_self(wires_, 0);
    }

    // Keep only base rendering bits:
    uint32_t base_options = options_ & WR_BASE_MASK;

    // double factor = 1.0;
    // if (debug_explode) {
    //   factor = 1.25;
    // }

    bool edge_top = false;
    bool edge_bottom = false;

    if (draw_bottom) {
      elliptical_sector bottom_face;
      placement bottom_face_placement;
      compute_top_bottom_face(FACE_BOTTOM, bottom_face, bottom_face_placement);
      uint32_t options = base_options;
      bottom_face.generate_wires(wires_, bottom_face_placement, options);
      edge_bottom = true;
    }

    if (draw_top) {
      elliptical_sector top_face;
      placement top_face_placement;
      compute_top_bottom_face(FACE_TOP, top_face, top_face_placement);
      uint32_t options = base_options;
      top_face.generate_wires(wires_, top_face_placement, options);
      edge_top = true;
    }


    if (draw_side) {
      elliptical_cylinder_sector side_face;
      placement side_face_placement;
      compute_side_face(side_face, side_face_placement);
      uint32_t options = base_options;
      if (edge_bottom) {
        options |= elliptical_cylinder_sector::WR_ELLCYLSEC_NO_BOTTOM_EDGE;
      }
      if (edge_top) {
        options |= elliptical_cylinder_sector::WR_ELLCYLSEC_NO_TOP_EDGE;
      }
      side_face.generate_wires(wires_, side_face_placement, options);
    }

    /*    const int nsamples = 36;
          for (int j = 0; j < 2; j++) {
          vector_3d vertex[nsamples];
          for (int i = 0; i < nsamples; i++) {
          double thetai = i * 2. * M_PI/nsamples;
          vertex[i].set(_x_radius_ * std::cos(thetai),
          _y_radius_ * std::sin(thetai),
          -0.5 * get_z() + j * get_z());
          }
          {
          polyline_type dummy;
          wires_.push_back (dummy);
          }
          polyline_type & pl = wires_.back();
          for (int i = 0; i <= 36; i++) {
          pl.push_back(vertex[i]);
          }

          for (int i = 0; i < nsamples; i++) {
          vector_3d vertex[2];
          double thetai = i * 2. * M_PI/nsamples;
          double x = _x_radius_ * std::cos(thetai);
          double y = _y_radius_ * std::sin(thetai);
          vertex[0].set(x, y, -0.5 * get_z());
          vertex[1].set(x, y, +0.5 * get_z());
          {
          polyline_type dummy;
          wires_.push_back(dummy);
          }
          polyline_type & pl = wires_.back();
          for (int i = 0; i < 2; i++) {
          pl.push_back(vertex[i]);
          }
          }
          }
    */
    return;
  }

  void elliptical_cylinder::initialize(const datatools::properties & config_,
                                       const handle_dict_type * objects_)
  {
    this->i_shape_3d::_initialize(config_, objects_);

    if (!is_valid()) {

      double lunit = CLHEP::mm;
      if (config_.has_key("length_unit")) {
        const std::string lunit_str = config_.fetch_string("length_unit");
        lunit = datatools::units::get_length_unit_from(lunit_str);
      }

      // double aunit = CLHEP::degree;
      // if (config_.has_key("angle_unit")) {
      //   const std::string aunit_str = config_.fetch_string("angle_unit");
      //   aunit = datatools::units::get_angle_unit_from(aunit_str);
      // }

      double x_radius;
      datatools::invalidate (x_radius);
      if (config_.has_key ("x_radius")) {
        x_radius = config_.fetch_real ("x_radius");
        if (! config_.has_explicit_unit ("x_radius")) {
          x_radius *= lunit;
        }
      } else if (config_.has_key ("x_diameter")) {
        x_radius = 0.5 * config_.fetch_real ("x_diameter");
        if (! config_.has_explicit_unit ("x_diameter")) {
          x_radius *= lunit;
        }
      }
      DT_THROW_IF (! datatools::is_valid (x_radius), std::logic_error,
                   "Missing elliptical_cylinder 'x_radius' property !");

      double y_radius;
      datatools::invalidate (y_radius);
      if (config_.has_key ("y_radius")) {
        y_radius = config_.fetch_real ("y_radius");
        if (! config_.has_explicit_unit ("y_radius")) {
          y_radius *= lunit;
        }
      } else if (config_.has_key("y_diameter")) {
        y_radius = 0.5 * config_.fetch_real ("y_diameter");
        if (! config_.has_explicit_unit ("y_diameter")) {
          y_radius *= lunit;
        }
      }
      DT_THROW_IF (! datatools::is_valid(y_radius), std::logic_error,
                   "Missing elliptical_cylinder 'y_radius' property !");

      DT_THROW_IF (! config_.has_key("z"), std::logic_error,
                   "Missing elliptical_cylinder 'z' property !");
      double z = config_.fetch_real("z");
      if (! config_.has_explicit_unit("z")) {
        z *= lunit;
      }
      set(x_radius, y_radius, z);
    }

    lock();
    return;
  }

  // static
  void elliptical_cylinder::init_ocd(datatools::object_configuration_description & ocd_)
  {
    i_shape_3d::init_ocd(ocd_);

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("length_unit")
        .set_from("geomtools::elliptical_cylinder")
        .set_terse_description("The unit symbol used for length unit")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_default_value_string("mm")
        .set_long_description("A character string that represents the default    \n"
                              "length unit for length parameters of the elliptical cylinder.\n"
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
      cpd.set_name_pattern("x_radius")
        .set_from("geomtools::elliptical_cylinder")
        .set_terse_description("The x radius of the elliptical cylinder")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .add_example("Set the x radius::                    \n"
                     "                                      \n"
                     "  x_radius : real as length = 2.0 cm  \n"
                     "                                      \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("x_diameter")
        .set_from("geomtools::elliptical_cylinder")
        .set_terse_description("The x diameter of the elliptical cylinder")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_long_description("Not used if the ``x_radius`` property is set.")
        .add_example("Set the x diameter::                    \n"
                     "                                        \n"
                     "  x_diameter : real as length = 4.0 cm  \n"
                     "                                        \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("y_radius")
        .set_from("geomtools::elliptical_cylinder")
        .set_terse_description("The y radius of the elliptical cylinder")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .add_example("Set the y radius::                   \n"
                     "                                     \n"
                     "  y_radius : real as length = 5.0 cm \n"
                     "                                     \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("y_diameter")
        .set_from("geomtools::elliptical_cylinder")
        .set_terse_description("The y diameter of the elliptical cylinder")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_long_description("Not used if the ``y_radius`` property is set.")
        .add_example("Set the y diameter::                    \n"
                     "                                        \n"
                     "  y_diameter : real as length = 10.0 cm \n"
                     "                                        \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("z")
        .set_from("geomtools::elliptical_cylinder")
        .set_terse_description("The dimension of the elliptical cylinder along the Z axis")
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
 *  @arg geomtools::elliptical_cylinder the full class name
 *  @arg ocd_ is the identifier of the 'datatools::object_configuration_description'
 *            to be initialized(passed by mutable reference).
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(geomtools::elliptical_cylinder, ocd_)
{
  // The class name :
  ocd_.set_class_name("geomtools::elliptical_cylinder");

  // The class terse description :
  ocd_.set_class_description("A class representing a 3D elliptical_cylinder solid shape");

  // The library the class belongs to :
  ocd_.set_class_library("geomtools");

  // The class detailed documentation :
  ocd_.set_class_documentation("The elliptical_cylinder solid 3D-shape is defined by \n"
                               "its three dimensions.                            \n"
                               );

  // Populate the specific OCD :
  geomtools::elliptical_cylinder::init_ocd(ocd_);

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Here is a full configuration example in the      \n"
                               "``datatools::properties`` ASCII format::         \n"
                               "                                                 \n"
                               "  tolerance.length : real = 1.0e-6 um            \n"
                               "  length_unit      : string = \"mm\"             \n"
                               "  x_radius : real as length = 50.0 mm            \n"
                               "  y_radius : real as length = 10.0 mm            \n"
                               "  z        : real           = 20.0 cm            \n"
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

// Registration macro for class 'geomtools::elliptical_cylinder' :
DOCD_CLASS_SYSTEM_REGISTRATION(geomtools::elliptical_cylinder, "geomtools::elliptical_cylinder")
