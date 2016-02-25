// torus.cc

// Ourselves:
#include <geomtools/torus.h>

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
//#include <geomtools/line_3d.h>
#include <geomtools/toroid_nappe.h>
#include <geomtools/placement.h>
#include <geomtools/i_shape_2d.h>

namespace geomtools {

  // Registration :
  GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(torus, "geomtools::torus");

  const std::string & torus::torus_label()
  {
    static std::string label("torus");
    return label;
  }

  double torus::get_xmin() const
  {
    return -_sweep_radius_ - _outside_radius_;
  }

  double torus::get_xmax() const
  {
    return +_sweep_radius_ + _outside_radius_;
  }

  double torus::get_ymin() const
  {
    return -_sweep_radius_ - _outside_radius_;
  }

  double torus::get_ymax() const
  {
   return +_sweep_radius_ + _outside_radius_;
  }

  double torus::get_zmin() const
  {
    return -_outside_radius_;
  }

  double torus::get_zmax() const
  {
    return +_outside_radius_;
  }

  double
  torus::get_sweep_radius() const
  {
    return _sweep_radius_;
  }

  void
  torus::set_sweep_radius(double sr_)
  {
    DT_THROW_IF(sr_ < 0.0, std::logic_error,
                 "Invalid '" << sr_ << "' solid torus sweep radius value !");
    _sweep_radius_ = sr_;
    return;
  }

  bool torus::has_inside_radius() const
  {
    return datatools::is_valid(_inside_radius_);
  }

  double
  torus::get_inside_radius() const
  {
    return _inside_radius_;
  }

  void
  torus::set_inside_radius(double inr_)
  {
    DT_THROW_IF(inr_ < 0.0, std::logic_error,
                 "Invalid '" << inr_ << "' inside radius value !");
    _inside_radius_ = inr_;
    return;
  }

  double
  torus::get_outside_radius() const
  {
    return _outside_radius_;
  }

  void
  torus::set_outside_radius(double inr_)
  {
    DT_THROW_IF(inr_ < 0.0, std::logic_error,
                 "Invalid '" << inr_ << "' outside radius value !");
    _outside_radius_ = inr_;
    return;
  }

  bool torus::has_partial_phi() const
  {
    if(_delta_phi_ == 2 * M_PI) return false;
    if(_start_phi_ > 0.0) return true;
    return false;
  }

  bool torus::has_start_phi() const
  {
    return datatools::is_valid(_start_phi_);
  }

  void torus::set_start_phi(double new_value_)
  {
    DT_THROW_IF(new_value_ < 0.0 || new_value_ >= 2 * M_PI,
                 std::domain_error,
                 "Invalid '" << new_value_ << "' start phi value !");
    _start_phi_ = new_value_;
    return;
  }

  double torus::get_start_phi() const
  {
    return _start_phi_;
  }

  bool torus::has_delta_phi() const
  {
    return datatools::is_valid(_delta_phi_);
  }

  void torus::set_delta_phi(double new_value_)
  {
    DT_THROW_IF(new_value_ < 0.0 || new_value_ > 2 * M_PI,
                 std::domain_error,
                 "Invalid '" << new_value_ << "' delta phi value !");
    _delta_phi_ = new_value_;
    return;
  }

  double torus::get_delta_phi() const
  {
    return _delta_phi_;
  }

  void torus::set(double sweep_radius_, double outside_radius_, double inside_radius_)
  {
    set_sweep_radius(sweep_radius_);
    set_outside_radius(outside_radius_);
    set_inside_radius(inside_radius_);
    return;
  }

  torus::torus()
  {
    _set_defaults();
    return;
  }

  torus::torus(double sweep_radius_, double outside_radius_, double inside_radius_)
  {
    _set_defaults();
    set(sweep_radius_, outside_radius_, inside_radius_);
    return;
  }

  torus::~torus()
  {
    return;
  }

  std::string torus::get_shape_name() const
  {
    return torus_label();
  }

  double
  torus::get_surface(uint32_t a_mask) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Solid torus is not valid !");
    double s = 0.0;
    uint32_t mask = a_mask;

    if (has_inside_radius()) {
      if(mask & FACE_INSIDE) {
        toroid_nappe inside_face;
        placement dummy_placement;
        compute_inside_outside_face(FACE_INSIDE, inside_face, dummy_placement);
        s += inside_face.get_surface();
      }
    }
    if(mask & FACE_OUTSIDE) {
      toroid_nappe outside_face;
      placement dummy_placement;
      compute_inside_outside_face(FACE_OUTSIDE, outside_face, dummy_placement);
      s += outside_face.get_surface();
    }
    if (has_partial_phi()) {
      disk phi_face;
      placement dummy_placement;
      compute_start_stop_phi_face(FACE_START_PHI,
                                  phi_face,
                                  dummy_placement);
      if(mask & FACE_START_PHI) {
        s += phi_face.get_surface();
      }
      if(mask & FACE_STOP_PHI) {
        s += phi_face.get_surface();
      }
    }
    return s;
  }

  double
  torus::get_volume(uint32_t /*flags*/) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Solid torus is not valid !");
    double vo = 2 * M_PI * M_PI * _outside_radius_ * _outside_radius_ * _sweep_radius_;
    double vi = 2 * M_PI * M_PI * _inside_radius_  * _inside_radius_  * _sweep_radius_;
    double v = vo - vi;
    if (has_partial_phi()) {
      v *= (get_delta_phi() / (2 * M_PI));
    }
    return v;
  }

  double
  torus::get_parameter( const std::string & flag_ ) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Solid torus is not valid !");
    if( flag_ == "sweep_r" ) return get_sweep_radius();
    if( flag_ == "sweep_radius" ) return get_sweep_radius();
    if( flag_ == "inside_radius" ) return get_inside_radius();
    if( flag_ == "outside_radius" ) return get_outside_radius();
    if( flag_ == "volume" ) return get_volume();
    if( flag_ == "surface.inner_side" ) return get_surface(FACE_INSIDE);
    if( flag_ == "surface.outer_side" ) return get_surface(FACE_OUTSIDE);
    if( flag_ == "surface.start_phi" ) return get_surface(FACE_START_PHI);
    if( flag_ == "surface.stop_phi" ) return get_surface(FACE_STOP_PHI);
    if( flag_ == "surface" ) return get_surface(FACE_ALL);

    DT_THROW(std::logic_error, "Unknown flag '" << flag_ << "' !");
  }

  bool
  torus::is_valid() const
  {
    return(datatools::is_valid(_sweep_radius_)
            && datatools::is_valid(_inside_radius_)
            && datatools::is_valid(_outside_radius_));
  }

  void torus::_set_defaults()
  {
    datatools::invalidate(_sweep_radius_);
    datatools::invalidate(_inside_radius_);
    datatools::invalidate(_outside_radius_);
    return;
  }

  void
  torus::reset()
  {
    unlock();
    _set_defaults();
    this->i_shape_3d::_reset();
    return;
  }

  bool
  torus::is_inside(const vector_3d & position_, double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid solid torus!");
    double skin = get_skin(skin_);
    double hskin = 0.5 * skin;
    double angular_tolerance = get_angular_tolerance();
    double r = hypot(position_.x(), position_.y());
    double x = r - _sweep_radius_;
    double z = position_.z();
    double phi = position_.phi();
    double start_phi = 0.0;
    double delta_phi = 2 * M_PI;
    if (has_start_phi()) {
      start_phi = _start_phi_;
      delta_phi = _delta_phi_;
    }
    if (!angle_is_in(phi,
                     start_phi,
                     delta_phi,
                     angular_tolerance,
                     true)) {
      return false;
    }
    disk section;
    section.set_outer_r(_outside_radius_ - hskin);
    if (has_inside_radius()) {
      section.set_inner_r(_inside_radius_ + hskin);
    }
    vector_3d pos(x, z, 0.0);
    if (!section.is_on_surface(pos, 0.0)) {
      return false;
    }
    return true;
  }

  bool
  torus::is_outside(const vector_3d & position_, double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid solid torus!");
    double skin = get_skin(skin_);
    double hskin = 0.5 * skin;
    double angular_tolerance = get_angular_tolerance();
    double r = hypot(position_.x(), position_.y());
    double x = r - _sweep_radius_;
    double z = position_.z();
    double phi = position_.phi();
    double start_phi = 0.0;
    double delta_phi = 2 * M_PI;
    if (has_start_phi()) {
      start_phi = _start_phi_;
      delta_phi = _delta_phi_;
    }
    if (!angle_is_in(phi,
                     start_phi,
                     delta_phi,
                     angular_tolerance,
                     true)) {
      return true;
    }
    disk section;
    section.set_outer_r(_outside_radius_ + hskin);
    if (has_inside_radius()) {
      section.set_inner_r(_inside_radius_ - hskin);
    }
    vector_3d pos(x, z, 0.0);
    if (!section.is_on_surface(pos, 0.0)) {
      return true;
    }
    return false;
  }

  vector_3d
  torus::get_normal_on_surface(const vector_3d & /* a_position */,
                               const face_identifier & /* a_surface_bit */) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid solid torus!");
    vector_3d normal;
    invalidate(normal);
    /*
    switch(a_surface_bit.get_face_bits()) {
    case FACE_INSIDE:
      {
        double phi = a_position.phi();
        normal.set(std::cos(phi), std::sin(phi), 0.0);
      }
      break;
    case FACE_OUTSIDE:
      {
        double phi = a_position.phi();
        normal.set(std::cos(phi), std::sin(phi), 0.0);
      }
      break;
    case FACE_START_PHI:
      normal.set(0.0, 0.0, -1.0);
      break;
    case FACE_STOP_PHI:
      normal.set(0.0, 0.0, +1.0);
      break;
    }
    */
    return normal;
  }

  face_identifier torus::on_surface(const vector_3d & a_position,
                                    const face_identifier & a_surface_mask,
                                    double a_skin) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid solid torus!");
    double skin = get_skin(a_skin);

    face_identifier mask;
    if(a_surface_mask.is_valid()) {
      DT_THROW_IF(! a_surface_mask.is_face_bits_mode(), std::logic_error,
                  "Face mask uses no face bits!");
      mask = a_surface_mask;
    } else {
      mask.set_face_bits_any();
    }

    const face_info_collection_type & faces = get_computed_faces();
    for(face_info_collection_type::const_iterator iface = faces.begin();
         iface != faces.end();
         iface++) {
      const face_info & finfo = *iface;
      if(finfo.is_valid() && mask.has_face_bit(finfo.get_identifier().get_face_bits())) {
        vector_3d position_c;
        finfo.get_positioning().mother_to_child(a_position, position_c);
        if(finfo.get_face_ref().is_on_surface(position_c, skin)) {
          return finfo.get_identifier();
        }
      }
    }

    return face_identifier::face_invalid();
  }

  bool
  torus::find_intercept(const vector_3d & a_from,
                        const vector_3d & a_direction,
                        face_intercept_info & a_intercept,
                        double a_skin) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid solid torus!");
    a_intercept.reset();

    double skin = compute_tolerance(a_skin);

    const unsigned int NFACES = 3;
    face_intercept_info intercepts[NFACES];
    unsigned int candidate_impact_counter = 0;

    int face_counter = 0;
    const face_info_collection_type & faces = get_computed_faces();
    for(face_info_collection_type::const_iterator iface = faces.begin();
         iface != faces.end();
         iface++) {
      const face_info & finfo = *iface;
      if(!finfo.is_valid()) {
        continue;
      }
      const i_shape_2d & face = finfo.get_face_ref();
      const placement & face_placement = finfo.get_positioning();
      const face_identifier & face_id = finfo.get_identifier();
      if(face.i_find_intercept::find_intercept(a_from,
                                                a_direction,
                                                face_placement,
                                                intercepts[face_counter],
                                                skin)) {
        intercepts[face_counter].set_face_id(face_id);
        candidate_impact_counter++;
      }
      face_counter++;
    }

    if(candidate_impact_counter > 0) {
      double min_length_to_impact = -1.0;
      for(unsigned int iface = 0; iface < NFACES; iface++) {
        if(intercepts[iface].is_ok()) {
          double length_to_impact =(intercepts[iface].get_impact() - a_from).mag();
          if(min_length_to_impact < 0.0 || length_to_impact < min_length_to_impact) {
            min_length_to_impact = length_to_impact;
            a_intercept.set_face_id(intercepts[iface].get_face_id());
            a_intercept.set_impact(intercepts[iface].get_impact());
          }
        }
      }
    }

    return a_intercept.is_ok();
  }

  // std::ostream & operator<<(std::ostream & a_out, const torus & a_torus)
  // {
  //   a_out << '{' << torus::torus_label() << ' '
  //         << a_torus._sweep_radius_ << ' '
  //         << a_torus._inside_radius_<< ' '
  //         << a_torus._outside_radius_ << '}';
  //   return a_out;
  // }

  // std::istream & operator>>( std::istream & a_in, torus & a_torus)
  // {
  //   a_torus.reset();
  //   char c = 0;
  //   a_in.get(c);
  //   if(c != '{')
  //     {
  //       a_in.clear(std::ios_base::failbit);
  //       return a_in;
  //     }
  //   std::string name;
  //   a_in >> name;
  //   if(name != torus::torus_label())
  //     {
  //       a_in.clear(std::ios_base::failbit);
  //       return a_in;
  //     }
  //   double sr, ir, outr;
  //   a_in >> sr >> ir >> outr ;
  //   if(! a_in)
  //     {
  //       a_in.clear(std::ios_base::failbit);
  //       return a_in;
  //     }
  //   c = 0;
  //   a_in.get(c);
  //   if(c != '}')
  //     {
  //       a_in.clear(std::ios_base::failbit);
  //       return a_in;
  //     }
  //   try
  //     {
  //       a_torus.set(ir, outr, sr);
  //     }
  //   catch(...)
  //     {
  //       a_torus.reset();
  //       a_in.clear(std::ios_base::failbit);
  //     }
  //   return a_in;
  // }

  void torus::tree_dump(std::ostream & a_out,
                            const std::string & a_title,
                            const std::string & a_indent,
                            bool a_inherit) const
  {
    i_object_3d::tree_dump(a_out, a_title, a_indent, true);

    a_out << a_indent << datatools::i_tree_dumpable::tag
          << "Sweep radius : " << get_sweep_radius() / CLHEP::mm << " mm" << std::endl;

    a_out << a_indent << datatools::i_tree_dumpable::tag
          << "Inside radius : " << get_inside_radius() / CLHEP::mm << " mm" << std::endl;

    a_out << a_indent << datatools::i_tree_dumpable::inherit_tag(a_inherit)
          << "Outside radius : " << get_outside_radius() / CLHEP::mm << " mm" << std::endl;

    if (has_partial_phi()) {

      a_out << a_indent << datatools::i_tree_dumpable::tag
            << "Start phi : " << get_start_phi() / CLHEP::degree << " degree" << std::endl;

      a_out << a_indent << datatools::i_tree_dumpable::inherit_tag(a_inherit)
            << "Delta phi : " << get_delta_phi() / CLHEP::degree << " degree" << std::endl;

    } else {

      a_out << a_indent << datatools::i_tree_dumpable::inherit_tag(a_inherit)
            << "Partial phi: " << "<none>" << std::endl;

    }
    return;
  }

  void torus::compute_inside_outside_face(faces_mask_type face_id_,
                                          toroid_nappe & face_,
                                          placement & face_placement_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Torus is not valid !");
    face_.reset();
    face_placement_.reset();
    switch(face_id_) {
    case FACE_INSIDE:
      face_.set_sweep_radius(get_sweep_radius());
      face_.set_radius(get_inside_radius());
      if (has_partial_phi()) {
        face_.set_start_phi(get_start_phi());
        face_.set_delta_phi(get_delta_phi());
      }
      face_placement_.set(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
      break;
    case FACE_OUTSIDE:
      face_.set_sweep_radius(get_sweep_radius());
      face_.set_radius(get_outside_radius());
      if (has_partial_phi()) {
        face_.set_start_phi(get_start_phi());
        face_.set_delta_phi(get_delta_phi());
      }
      face_placement_.set(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
      break;
    default:
      break;
    }
    return;
  }

  void torus::compute_start_stop_phi_face(faces_mask_type /*face_id_*/,
                                          disk & face_,
                                          placement & face_placement_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Torus is not valid !");
    face_.reset();
    face_placement_.reset();
    // switch(face_id_) {
    // case FACE_START_PHI:
    //   face_.set_r(get_r());
    //   face_placement_.set(0.0, 0.0, -0.5 * _z_,
    //                       0.0, 0.0, 0.0);
    //   break;
    // case FACE_STOP_PHI:
    //   face_.set_r(get_r());
    //   face_placement_.set(0.0, 0.0, +0.5 * _z_,
    //                       0.0, 0.0, 0.0);
    //   break;
    // default:
    //   break;
    // }
    return;
  }


  unsigned int torus::compute_faces(face_info_collection_type & faces_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Torus is not valid !");
    unsigned int nfaces = 0;

    if (has_inside_radius()) {
      // Inside face:
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      toroid_nappe & tn = finfo.add_face<toroid_nappe>();
      compute_inside_outside_face(FACE_INSIDE, tn, finfo.grab_positioning());
      face_identifier fid;
      fid.set_face_bit(FACE_INSIDE);
      finfo.set_identifier(fid);
      finfo.set_label("inside");
      nfaces++;
    }

    {
      // Outside face:
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      toroid_nappe & tn = finfo.add_face<toroid_nappe>();
      compute_inside_outside_face(FACE_OUTSIDE, tn, finfo.grab_positioning());
      face_identifier fid;
      fid.set_face_bit(FACE_OUTSIDE);
      finfo.set_identifier(fid);
      finfo.set_label("outside");
      nfaces++;
    }

    if (has_partial_phi()) {
      {
        // Start phi face:
        {
          face_info dummy;
          faces_.push_back(dummy);
        }
        face_info & finfo = faces_.back();
        disk & d = finfo.add_face<disk>();
        compute_start_stop_phi_face(FACE_START_PHI, d, finfo.grab_positioning());
        face_identifier fid;
        fid.set_face_bit(FACE_START_PHI);
        finfo.set_identifier(fid);
        finfo.set_label("start_phi");
        nfaces++;
      }

      {
        // Stop phi face:
        {
          face_info dummy;
          faces_.push_back(dummy);
        }
        face_info & finfo = faces_.back();
        disk & d = finfo.add_face<disk>();
        compute_start_stop_phi_face(FACE_STOP_PHI, d, finfo.grab_positioning());
        face_identifier fid;
        fid.set_face_bit(FACE_STOP_PHI);
        finfo.set_identifier(fid);
        finfo.set_label("stop_phi");
        nfaces++;
      }
    }
    return nfaces;
  }


  void torus::generate_wires_self(wires_type & wires_,
                                     uint32_t options_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid solid torus!");
    bool draw_inside      = !(options_ & WR_TORUS_NO_INSIDE_FACE);
    bool draw_outside     = !(options_ & WR_TORUS_NO_OUTSIDE_FACE);
    bool draw_start_phi = !(options_ & WR_TORUS_NO_START_PHI_FACE);
    bool draw_stop_phi  = !(options_ & WR_TORUS_NO_STOP_PHI_FACE);

    bool draw_boundings =(options_ & WR_BASE_BOUNDINGS);

    if(draw_boundings) {
      get_bounding_data().generate_wires_self(wires_, 0);
    }

    // Keep only base rendering bits:
    uint32_t base_options = options_ & WR_BASE_MASK;

    // bool edge_top = false;
    // bool edge_bottom = false;

    if(has_inside_radius()) {
      if(draw_inside) {
        toroid_nappe inside_face;
        placement inside_face_placement;
        compute_inside_outside_face(FACE_INSIDE, inside_face, inside_face_placement);
        uint32_t options = base_options;
        inside_face.generate_wires(wires_, inside_face_placement, options);
      }
    }

    if(draw_outside) {
      toroid_nappe outside_face;
      placement outside_face_placement;
      compute_inside_outside_face(FACE_OUTSIDE, outside_face, outside_face_placement);
      uint32_t options = base_options;
      outside_face.generate_wires(wires_, outside_face_placement, options);
    }

    if(has_partial_phi()) {

      if (draw_start_phi) {
        disk start_face;
        placement start_face_placement;
        compute_start_stop_phi_face(FACE_START_PHI, start_face, start_face_placement);
        uint32_t options = base_options;
        start_face.generate_wires(wires_, start_face_placement, options);
      }

      if (draw_stop_phi) {
        disk stop_face;
        placement stop_face_placement;
        compute_start_stop_phi_face(FACE_STOP_PHI, stop_face, stop_face_placement);
        uint32_t options = base_options;
        stop_face.generate_wires(wires_, stop_face_placement, options);
      }

    }

    return;
  }

  void torus::initialize(const datatools::properties & config_,
                            const handle_dict_type * objects_)
  {
    this->i_shape_3d::_initialize(config_, objects_);

    if(!is_valid()) {
      double lunit = CLHEP::mm;
      if(config_.has_key("length_unit")) {
        const std::string lunit_str = config_.fetch_string("length_unit");
        lunit = datatools::units::get_length_unit_from(lunit_str);
      }

      double sweep_radius;
      datatools::invalidate(sweep_radius);
      if(config_.has_key("sweep_radius")) {
        sweep_radius = config_.fetch_real("sweep_radius");
        if(! config_.has_explicit_unit("sweep_radius")) {
          sweep_radius *= lunit;
        }
      }
      DT_THROW_IF(! datatools::is_valid(sweep_radius), std::logic_error, "Missing torus 'sweep_radius' property !");

      double outside_radius;
      datatools::invalidate(outside_radius);
      if(config_.has_key("outside_radius")) {
        outside_radius = config_.fetch_real("outside_radius");
        if(! config_.has_explicit_unit("outside_radius")) {
          outside_radius *= lunit;
        }
      }
      DT_THROW_IF(! datatools::is_valid(outside_radius), std::logic_error, "Missing torus 'outside_radius' property !");

      double inside_radius;
      datatools::invalidate(inside_radius);
      if(config_.has_key("inside_radius")) {
        inside_radius = config_.fetch_real("inside_radius");
        if(! config_.has_explicit_unit("inside_radius")) {
         inside_radius *= lunit;
        }
      }

      set(sweep_radius, outside_radius, inside_radius);

    }
    lock();
    return;
  }

  void torus::_build_bounding_data()
  {
    _grab_bounding_data().make_cylinder(get_sweep_radius() + get_outside_radius(),
                                        -0.5 * get_outside_radius(),
                                        +0.5 * get_outside_radius());
    return;
  }

  // static
  void torus::init_ocd(datatools::object_configuration_description & ocd_)
  {
    i_shape_3d::init_ocd(ocd_);

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("length_unit")
        .set_from("geomtools::torus")
        .set_terse_description("The unit symbol used for length unit")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_default_value_string("mm")
        .set_long_description("A character string that represents the default    \n"
                              "length unit for length parameters of the torus.\n"
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
      cpd.set_name_pattern("sweep_radius")
        .set_from("geomtools::torus")
        .set_terse_description("The sweep radius of the torus")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(true)
        .add_example("Set the sweep radius::                    \n"
                     "                                          \n"
                     "  sweep_radius : real as length = 5.0 cm  \n"
                     "                                          \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("outside_radius")
        .set_from("geomtools::torus")
        .set_terse_description("The outside radius of the torus")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(true)
        .add_example("Set the outside radius::                    \n"
                     "                                            \n"
                     "  outside_radius : real as length = 2.0 cm  \n"
                     "                                            \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("inside_radius")
        .set_from("geomtools::torus")
        .set_terse_description("The inside radius of the torus")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .add_example("Set the inside radius::                    \n"
                     "                                            \n"
                     "  inside_radius : real as length = 2.0 cm  \n"
                     "                                            \n"
                     )
        ;
    }

    return;
  }

} // end of namespace geomtools

/** Opening macro for implementation
 *  @arg geomtools::torus the full class name
 *  @arg ocd_ is the identifier of the 'datatools::object_configuration_description'
 *            to be initialized(passed by mutable reference).
 *  This macro must be used outside of any namespace.
 */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(geomtools::torus, ocd_)
{
  // The class name :
  ocd_.set_class_name("geomtools::torus");

  // The class terse description :
  ocd_.set_class_description("A class representing a 3D solid torus shape");

  // The library the class belongs to :
  ocd_.set_class_library("geomtools");

  // The class detailed documentation :
  ocd_.set_class_documentation("The solid torus 3D-shape is defined by its two or \n"
                               "three dimensions.                                 \n"
                               );

  // Populate the specific OCD :
  geomtools::torus::init_ocd(ocd_);

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Here is a full configuration example in the      \n"
                               "``datatools::properties`` ASCII format::         \n"
                               "                                                 \n"
                               "  tolerance.length : real as length = 1.0e-6 um  \n"
                               "  length_unit      : string = \"mm\"             \n"
                               "  sweep_radius     : real as length = 5.0 cm     \n"
                               "  outside_radius   : real as length = 2.0 cm     \n"
                               "  inside_radius    : real as length = 1.5 cm     \n"
                               "  start_phi        : real as angle  = 30 degree  \n"
                               "  delta_phi        : real as angle  = 20 degree  \n"
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

// Registration macro for class 'geomtools::torus' :
DOCD_CLASS_SYSTEM_REGISTRATION(geomtools::torus, "geomtools::torus")
