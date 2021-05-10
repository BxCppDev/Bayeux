// cylinder.cc

// Ourselves:
#include <geomtools/cylinder.h>

// Standard library:
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/properties.h>
#include <datatools/units.h>

// This project:
#include <geomtools/circle.h>
#include <geomtools/disk.h>
#include <geomtools/line_3d.h>
#include <geomtools/cylindrical_sector.h>
#include <geomtools/placement.h>
#include <geomtools/i_shape_2d.h>

namespace geomtools {

  // Registration :
  GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(cylinder, "geomtools::cylinder")

  const std::string & cylinder::cylinder_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "cylinder";
    }
    return label;
  }

  double cylinder::get_xmin () const
  {
    return -_radius_;
  }

  double cylinder::get_xmax () const
  {
    return +_radius_;
  }

  double cylinder::get_ymin () const
  {
    return -_radius_;
  }

  double cylinder::get_ymax () const
  {
    return +_radius_;
  }

  double cylinder::get_zmin () const
  {
    return -0.5*_z_;
  }

  double cylinder::get_zmax () const
  {
    return +0.5*_z_;
  }

  double
  cylinder::get_r () const
  {
    return _radius_;
  }

  void
  cylinder::set_r (double a_radius)
  {
    DT_THROW_IF (a_radius < 0.0, std::logic_error,
                 "Invalid '" << a_radius << "' R value !");
    _radius_ = a_radius;
    return;
  }

  double
  cylinder::get_radius () const
  {
    return get_r ();
  }

  double
  cylinder::get_diameter () const
  {
    return (_radius_ + _radius_);
  }

  void
  cylinder::set_diameter (double a_diameter)
  {
    set_r (a_diameter * 0.5);
    return;
  }

  double
  cylinder::get_z () const
  {
    return _z_;
  }

  void
  cylinder::set_z (double a_z)
  {
    DT_THROW_IF (a_z < 0.0, std::logic_error,
                 "Invalid '" << a_z << "' Z value !");
    _z_ = a_z;
    return;
  }

  double
  cylinder::get_half_z () const
  {
    return _z_ * 0.5;
  }

  void
  cylinder::set_half_z (double a_half_z)
  {
    set_z (a_half_z + a_half_z);
    return;
  }

  void
  cylinder::set (double a_radius, double a_z)
  {
    set_r (a_radius);
    set_z (a_z);
    return;
  }

  void
  cylinder::set_alternative (double a_diameter, double a_half_z)
  {
    set_diameter (a_diameter);
    set_half_z (a_half_z);
    return;
  }

  cylinder::cylinder ()
  {
    _set_defaults();
    return;
  }

  cylinder::cylinder (double a_radius, double a_z)
  {
    _set_defaults();
    set(a_radius, a_z);
    return;
  }

  cylinder::~cylinder ()
  {
    return;
  }

  std::string cylinder::get_shape_name () const
  {
    return cylinder_label();
  }

  double
  cylinder::get_surface (uint32_t a_mask) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Cylinder is not valid !");
    double s = 0.0;
    uint32_t mask = a_mask;

    if (mask & FACE_SIDE) {
      s += 2 * M_PI * _radius_ * _z_;
    }
    if (mask & FACE_BOTTOM) {
      s += M_PI * _radius_ * _radius_;
    }
    if (mask & FACE_TOP) {
      s += M_PI * _radius_ * _radius_;
    }
    return s;
  }

  double
  cylinder::get_volume (uint32_t /*flags*/) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Cylinder is not valid !");
    return M_PI * _radius_ * _radius_ * _z_;
  }

  double
  cylinder::get_parameter ( const std::string & flag_ ) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Cylinder is not valid !");
    if ( flag_ == "r" ) return get_r ();
    if ( flag_ == "radius" ) return get_r ();
    if ( flag_ == "diameter" ) return get_diameter ();
    if ( flag_ == "z" ) return get_z ();
    if ( flag_ == "volume" ) return get_volume ();
    if ( flag_ == "surface.top" ) return get_surface (FACE_TOP);
    if ( flag_ == "surface.bottom" ) return get_surface (FACE_BOTTOM);
    if ( flag_ == "surface.side" ) return get_surface (FACE_SIDE);
    if ( flag_ == "surface" ) return get_surface (FACE_ALL);

    DT_THROW(std::logic_error, "Unknown flag '" << flag_ << "' !");
  }

  bool
  cylinder::is_valid () const
  {
    return (datatools::is_valid(_radius_) && datatools::is_valid(_z_));
  }

  void
  cylinder::init ()
  {
    return;
  }

  void cylinder::_set_defaults()
  {
    datatools::invalidate(_radius_);
    datatools::invalidate(_z_);
    return;
  }

  void
  cylinder::reset ()
  {
    unlock();
    _set_defaults();
    this->i_shape_3d::_reset();
    return;
  }

  bool
  cylinder::is_inside(const vector_3d & a_position, double a_skin) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid cylinder!");
    double skin = get_skin(a_skin);
    double hskin = 0.5 * skin;
    double r = hypot(a_position.x(), a_position.y());
    if ( (r <= (_radius_ - hskin))
         && ( std::abs(a_position.z()) <= (0.5 * _z_ - hskin) ) ) {
      return true;
    }
    return false;
  }

  bool
  cylinder::is_outside(const vector_3d & a_position, double a_skin) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid cylinder!");
    double skin = get_skin(a_skin);
    double hskin = 0.5 * skin;
    double r = hypot(a_position.x(), a_position.y());
    if ( (r >= (_radius_ + hskin))
         || ( std::abs(a_position.z()) >= (0.5 * _z_ + hskin) ) ) {
      return true;
    }
    return false;
  }

  vector_3d
  cylinder::get_normal_on_surface (const vector_3d & a_position,
                                   const face_identifier & a_surface_bit) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid cylinder!");
    vector_3d normal;
    invalidate(normal);
    switch(a_surface_bit.get_face_bits()) {
    case FACE_SIDE:
      {
        double phi = a_position.phi();
        normal.set(std::cos(phi), std::sin(phi), 0.0);
      }
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

  face_identifier cylinder::on_surface(const vector_3d & a_position,
                                       const face_identifier & a_surface_mask,
                                       double a_skin) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid cylinder!");
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

  bool
  cylinder::find_intercept (const vector_3d & a_from,
                            const vector_3d & a_direction,
                            face_intercept_info & a_intercept,
                            double a_skin) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid cylinder!");
    a_intercept.reset ();

    double skin = compute_tolerance(a_skin);

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
      if (face.i_find_intercept::find_intercept(a_from,
                                                a_direction,
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
      disk bottom_face;
      placement bottom_face_placement;
      compute_top_bottom_face(FACE_BOTTOM, bottom_face, bottom_face_placement);
      if (bottom_face.i_find_intercept::find_intercept(a_from,
      a_direction,
      bottom_face_placement,
      intercepts[FACE_INDEX],
      skin)) {
      intercepts[FACE_INDEX].grab_face_id().set_face_bit(FACE_BOTTOM);
      candidate_impact_counter++;
      }
      }

      {
      const int FACE_INDEX = 1;
      disk top_face;
      placement top_face_placement;
      compute_top_bottom_face(FACE_TOP, top_face, top_face_placement);
      if (top_face.i_find_intercept::find_intercept(a_from,
      a_direction,
      top_face_placement,
      intercepts[FACE_INDEX],
      skin)) {
      intercepts[FACE_INDEX].grab_face_id().set_face_bit(FACE_TOP);
      candidate_impact_counter++;
      }
      }

      {
      const int FACE_INDEX = 2;
      cylindrical_sector side_face;
      placement side_face_placement;
      compute_side_face(side_face, side_face_placement);
      if (side_face.i_find_intercept::find_intercept(a_from,
      a_direction,
      side_face_placement,
      intercepts[FACE_INDEX],
      skin)) {
      intercepts[FACE_INDEX].grab_face_id().set_face_bit(FACE_SIDE);
      candidate_impact_counter++;
      }
      }
    */

    if (candidate_impact_counter > 0) {
      double min_length_to_impact = -1.0;
      for (unsigned int iface = 0; iface < NFACES; iface++) {
        if (intercepts[iface].is_ok()) {
          double length_to_impact = (intercepts[iface].get_impact() - a_from).mag();
          if (min_length_to_impact < 0.0 || length_to_impact < min_length_to_impact) {
            min_length_to_impact = length_to_impact;
            a_intercept.set_face_id(intercepts[iface].get_face_id());
            a_intercept.set_impact(intercepts[iface].get_impact());
          }
        }
      }
    }

    return a_intercept.is_ok();
  }

  std::ostream & operator<< (std::ostream & a_out, const cylinder & a_cylinder)
  {
    a_out << '{' << cylinder::cylinder_label() << ' '
          << a_cylinder._radius_ << ' '
          << a_cylinder._z_ << '}';
    return a_out;
  }

  std::istream & operator>> ( std::istream & a_in, cylinder & a_cylinder)
  {
    a_cylinder.reset ();
    char c = 0;
    a_in.get (c);
    if (c != '{')
      {
        a_in.clear (std::ios_base::failbit);
        return a_in;
      }
    std::string name;
    a_in >> name;
    if (name != cylinder::cylinder_label())
      {
        a_in.clear (std::ios_base::failbit);
        return a_in;
      }
    double r, z;
    a_in >> r >> z;
    if (! a_in)
      {
        a_in.clear (std::ios_base::failbit);
        return a_in;
      }
    c = 0;
    a_in.get (c);
    if (c != '}')
      {
        a_in.clear (std::ios_base::failbit);
        return a_in;
      }
    try
      {
        a_cylinder.set (r,z);
      }
    catch (...)
      {
        a_cylinder.reset ();
        a_in.clear (std::ios_base::failbit);
      }
    return a_in;
  }

  void cylinder::tree_dump (std::ostream & a_out,
                            const std::string & a_title,
                            const std::string & a_indent,
                            bool a_inherit) const
  {
    i_object_3d::tree_dump(a_out, a_title, a_indent, true);

    a_out << a_indent << datatools::i_tree_dumpable::tag
          << "Radius : " << get_r () / CLHEP::mm << " mm" << std::endl;

    a_out << a_indent << datatools::i_tree_dumpable::inherit_tag (a_inherit)
          << "Z : " << get_z () / CLHEP::mm << " mm" << std::endl;

    return;
  }

  void cylinder::compute_side_face(cylindrical_sector & face_,
                                   placement & face_placement_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Cylinder is not valid !");
    face_.reset();
    face_placement_.reset();
    face_.set_radius(get_r());
    face_.set_z(get_z());
    face_placement_.set(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
    return;
  }

  void cylinder::compute_top_bottom_face(faces_mask_type face_id_,
                                         disk & face_,
                                         placement & face_placement_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Cylinder is not valid !");
    face_.reset();
    face_placement_.reset();
    switch(face_id_) {
    case FACE_BOTTOM:
      face_.set_r(get_r());
      face_placement_.set(0.0, 0.0, -0.5 * _z_,
                          0.0, 0.0, 0.0);
      break;
    case FACE_TOP:
      face_.set_r(get_r());
      face_placement_.set(0.0, 0.0, +0.5 * _z_,
                          0.0, 0.0, 0.0);
      break;
    default:
      break;
    }
    return;
  }


  unsigned int cylinder::compute_faces(face_info_collection_type & faces_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Cylinder is not valid !");
    unsigned int nfaces = 0;
    {
      // Side face:
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      cylindrical_sector & cs = finfo.add_face<cylindrical_sector>();
      compute_side_face(cs, finfo.grab_positioning());
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
      disk & d = finfo.add_face<disk>();
      compute_top_bottom_face(FACE_BOTTOM, d, finfo.grab_positioning());
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
      disk & d = finfo.add_face<disk>();
      compute_top_bottom_face(FACE_TOP, d, finfo.grab_positioning());
      face_identifier fid;
      fid.set_face_bit(FACE_TOP);
      finfo.set_identifier(fid);
      finfo.set_label("top");
      nfaces++;
    }
    return nfaces;
  }

  void cylinder::compute_deflated(cylinder & deflated_,
                                  double by_r_,
                                  double by_z_)
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid cylinder!");
    double r_eps = 0.0;
    double z_eps = 0.0;
    if (datatools::is_valid(by_r_) and by_r_ > 0.0) r_eps = by_r_;
    if (datatools::is_valid(by_z_) and by_z_ > 0.0) z_eps = by_z_;
    double r = get_r();
    double z = get_z();
    r -= r_eps;
    z -= (2 * z_eps);
    deflated_.reset();
    deflated_.set(r, z);
    deflated_.lock();
    return;
  }

  void cylinder::compute_inflated(cylinder & inflated_,
                                  double by_r_,
                                  double by_z_)
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid cylinder!");
    double r_eps = 0.0;
    double z_eps = 0.0;
    if (datatools::is_valid(by_r_) and by_r_ > 0.0) r_eps = by_r_;
    if (datatools::is_valid(by_z_) and by_z_ > 0.0) z_eps = by_z_;
    double r = get_r();
    double z = get_z();
    r += r_eps;
    z += (2 * z_eps);
    inflated_.reset();
    inflated_.set(r, z);
    inflated_.lock();
    return;
  }

  void cylinder::compute_envelope(cylinder & envelope_,
                                  double tolerance_r_,
                                  double tolerance_z_)
  {
    compute_inflated(envelope_, tolerance_r_, tolerance_z_);
  }
  
  void cylinder::generate_wires_self(wires_type & wires_,
                                     uint32_t options_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid cylinder!");
    // bool debug_explode = options_ & WR_BASE_EXPLODE;
    bool draw_bottom = !(options_ & WR_CYL_NO_BOTTOM_FACE);
    bool draw_top    = !(options_ & WR_CYL_NO_TOP_FACE);
    bool draw_side   = !(options_ & WR_CYL_NO_SIDE_FACE);

    bool draw_boundings = (options_ & WR_BASE_BOUNDINGS);

    if (draw_boundings) {
      get_bounding_data().generate_wires_self(wires_, 0);
    }

    // Keep only base rendering bits:
    uint32_t base_options = options_ & WR_BASE_MASK;

    bool edge_top = false;
    bool edge_bottom = false;

    if (draw_bottom) {
      disk bottom_face;
      placement bottom_face_placement;
      compute_top_bottom_face(FACE_BOTTOM, bottom_face, bottom_face_placement);
      uint32_t options = base_options;
      bottom_face.generate_wires(wires_, bottom_face_placement, options);
      edge_bottom = true;
    }

    if (draw_top) {
      disk top_face;
      placement top_face_placement;
      compute_top_bottom_face(FACE_TOP, top_face, top_face_placement);
      uint32_t options = base_options;
      top_face.generate_wires(wires_, top_face_placement, options);
      edge_top = true;
    }

    if (draw_side) {
      cylindrical_sector side_face;
      placement side_face_placement;
      compute_side_face(side_face, side_face_placement);
      uint32_t options = base_options;
      if (edge_bottom) {
        options |= cylindrical_sector::WR_CYLSEC_NO_BOTTOM_EDGE;
      }
      if (edge_top) {
        options |= cylindrical_sector::WR_CYLSEC_NO_TOP_EDGE;
      }
      side_face.generate_wires(wires_, side_face_placement, options);
    }

    return;
  }

  void cylinder::initialize(const datatools::properties & config_,
                            const handle_dict_type * objects_)
  {
    this->i_shape_3d::_initialize(config_, objects_);

    if (!is_valid()) {
      double lunit = CLHEP::mm;
      if (config_.has_key("length_unit")) {
        const std::string lunit_str = config_.fetch_string("length_unit");
        lunit = datatools::units::get_length_unit_from(lunit_str);
      }

      double r;
      datatools::invalidate(r);
      if(config_.has_key("r")) {
        r = config_.fetch_real("r");
        if(! config_.has_explicit_unit("r")) {
          r *= lunit;
        }
      } else if(config_.has_key("radius")) {
        r = config_.fetch_real("radius");
        if(! config_.has_explicit_unit("radius")) {
          r *= lunit;
        }
      } else if(config_.has_key("diameter")) {
        r = 0.5 * config_.fetch_real("diameter");
        if(! config_.has_explicit_unit("diameter")) {
          r *= lunit;
        }
      }
      DT_THROW_IF(! datatools::is_valid(r), std::logic_error, "Missing cylinder 'r', 'radius' or 'diameter' property !");

      DT_THROW_IF (! config_.has_key("z"), std::logic_error, "Missing cylinder 'z' property !");
      double z = config_.fetch_real("z");
      if (! config_.has_explicit_unit("z")) {
        z *= lunit;
      }

      set_r(r);
      set_z(z);

    }
    lock();
    return;
  }

  void cylinder::_build_bounding_data()
  {
    _grab_bounding_data().make_cylinder(get_r(), -0.5 * get_z(), +0.5 * get_z());
    return;
  }

  // static
  void cylinder::init_ocd(datatools::object_configuration_description & ocd_)
  {
    i_shape_3d::init_ocd(ocd_);

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("length_unit")
        .set_from("geomtools::cylinder")
        .set_terse_description("The unit symbol used for length unit")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_default_value_string("mm")
        .set_long_description("A character string that represents the default    \n"
                              "length unit for length parameters of the cylinder.\n"
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
      cpd.set_name_pattern("r")
        .set_from("geomtools::cylinder")
        .set_terse_description("The radius of the cylinder")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(true)
        .add_example("Set the radius::               \n"
                     "                               \n"
                     "  r : real as length = 5.0 cm  \n"
                     "                               \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("radius")
        .set_from("geomtools::cylinder")
        .set_terse_description("The radius of the cylinder")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_long_description("Alias of the ``r`` property.\n"
                              "Not used if the ``r`` property is set."
                              )
        .add_example("Set the radius::               \n"
                     "                               \n"
                     "  radius : real as length = 5.0 cm  \n"
                     "                               \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("diameter")
        .set_from("geomtools::cylinder")
        .set_terse_description("The diameter of the cylinder")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_long_description("Not used if the ``r`` or ``radius`` property is set.")
        .add_example("Set the diameter::                    \n"
                     "                                      \n"
                     "  diameter : real as length = 10.0 cm \n"
                     "                                      \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("z")
        .set_from("geomtools::cylinder")
        .set_terse_description("The dimension of the cylinder along the Z axis")
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
 *  @arg geomtools::cylinder the full class name
 *  @arg ocd_ is the identifier of the 'datatools::object_configuration_description'
 *            to be initialized(passed by mutable reference).
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(geomtools::cylinder, ocd_)
{
  // The class name :
  ocd_.set_class_name("geomtools::cylinder");

  // The class terse description :
  ocd_.set_class_description("A class representing a 3D cylinder solid shape");

  // The library the class belongs to :
  ocd_.set_class_library("geomtools");

  // The class detailed documentation :
  ocd_.set_class_documentation("The cylinder solid 3D-shape is defined by its two \n"
                               "dimensions.                                       \n"
                               );

  // Populate the specific OCD :
  geomtools::cylinder::init_ocd(ocd_);

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Here is a full configuration example in the      \n"
                               "``datatools::properties`` ASCII format::         \n"
                               "                                                 \n"
                               "  tolerance.length : real = 1.0e-6 um            \n"
                               "  length_unit      : string = \"mm\"             \n"
                               "  r : real as length = 25.0 cm                   \n"
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

// Registration macro for class 'geomtools::cylinder' :
DOCD_CLASS_SYSTEM_REGISTRATION(geomtools::cylinder, "geomtools::cylinder")
