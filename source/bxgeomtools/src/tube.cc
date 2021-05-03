/// \file geomtools/tube.cc

// Ourselves:
#include <geomtools/tube.h>

// Standard library:
#include <cmath>
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/units.h>
// - Bayeux/mygsl:
#include <mygsl/min_max.h>

// This project:
#include <geomtools/cylinder.h>
#include <geomtools/rectangle.h>
#include <geomtools/cylindrical_sector.h>
#include <geomtools/disk.h>

namespace geomtools {

  // Registration :
  GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(tube, "geomtools::tube")

  using namespace std;

  const std::string & tube::tube_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "tube";
    }
    return label;
  }

  bool tube::has_inner_face() const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid tube!");
    if (has_inner_r()) {
      return true;
    }
    return false;
  }

  void tube::compute_top_bottom_face(faces_mask_type face_id_,
                                     disk & face_,
                                     placement & face_placement_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Box is not valid !");
    face_.reset();
    face_placement_.reset();
    switch (face_id_) {
    case FACE_BOTTOM:
      face_.set(get_inner_r(), get_outer_r());
      face_placement_.set(0.0, 0.0, -0.5 * get_z(), 0.0, 0.0, 0.0);
      break;
    case FACE_TOP:
      face_.set(get_inner_r(), get_outer_r());
      face_placement_.set(0.0, 0.0, +0.5 * get_z(), 0.0, 0.0, 0.0);
      break;
    default:
      DT_THROW(std::logic_error, "Invalid top/bottom mask!");
    }
    if (has_partial_phi()) {
      face_.set_start_angle(get_start_phi());
      face_.set_delta_angle(get_delta_phi());
    }
    return;
  }

  void tube::compute_side_face(faces_mask_type face_id_,
                               cylindrical_sector & face_,
                               placement & face_placement_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Box is not valid !");
    face_.reset();
    face_placement_.reset();
    switch (face_id_) {
    case FACE_OUTER_SIDE:
      face_.set_radius(get_outer_r());
      break;
    case FACE_INNER_SIDE:
      DT_THROW_IF(!has_inner_r(), std::logic_error,
                  "No inner side face!");
      face_.set_radius(get_inner_r());
      break;
    default:
      DT_THROW(std::logic_error, "Invalid side face mask!");
    }
    face_.set_z(get_z());
    if (has_partial_phi()) {
      face_.set_start_angle(get_start_phi());
      face_.set_delta_angle(get_delta_phi());
    }
    face_placement_.set(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
    return;
  }

  void tube::compute_start_stop_angle_face(faces_mask_type face_id_,
                                           rectangle & face_,
                                           placement & face_placement_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Tube is not valid !");
    face_.reset();
    face_placement_.reset();
    DT_THROW_IF (! has_partial_phi(), std::logic_error, "Tube has no partial phi !");
    double ir = 0.0;
    if (has_inner_r()) {
      ir = _inner_r_;
    }
    double phi = 0.0;
    switch (face_id_) {
    case FACE_START_ANGLE:
      phi = get_start_phi();
      break;
    case FACE_STOP_ANGLE:
      phi = get_start_phi() + get_delta_phi();
      break;
    default:
      DT_THROW(std::logic_error, "Invalid side face mask!");
    }
    face_.set(_z_, _outer_r_ - ir);
    double r = 0.5 * (_outer_r_ + ir);
    double x = r * std::cos(phi);
    double y = r * std::sin(phi);
    face_placement_.set(x,
                        y,
                        0.0,
                        phi + get_special_rotation_angle(ROTATION_ANGLE_90),
                        get_special_rotation_angle(ROTATION_ANGLE_90),
                        0.0
                        );
    return;
  }

  unsigned int tube::compute_faces(face_info_collection_type & faces_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Tube is not valid !");
    unsigned int nfaces = 0;

    {
      // Outer side face:
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      cylindrical_sector & cs = finfo.add_face<cylindrical_sector>();
      compute_side_face(FACE_OUTER_SIDE, cs, finfo.grab_positioning());
      face_identifier fid;
      fid.set_face_bit(FACE_OUTER_SIDE);
      finfo.set_identifier(fid);
      finfo.set_label("outer_side");
      nfaces++;
    }

    if (has_inner_r()) {
      // Inner side face:
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      cylindrical_sector & cs = finfo.add_face<cylindrical_sector>();
      compute_side_face(FACE_INNER_SIDE, cs, finfo.grab_positioning());
      face_identifier fid;
      fid.set_face_bit(FACE_INNER_SIDE);
      finfo.set_identifier(fid);
      finfo.set_label("inner_side");
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

    if (has_partial_phi()) {

      {
        // Start angle face:
        {
          face_info dummy;
          faces_.push_back(dummy);
        }
        face_info & finfo = faces_.back();
        rectangle & r = finfo.add_face<rectangle>();
        compute_start_stop_angle_face(FACE_START_ANGLE, r, finfo.grab_positioning());
        face_identifier fid;
        fid.set_face_bit(FACE_START_ANGLE);
        finfo.set_identifier(fid);
        finfo.set_label("start_angle");
        nfaces++;
      }

      {
        // Stop angle face:
        {
          face_info dummy;
          faces_.push_back(dummy);
        }
        face_info & finfo = faces_.back();
        rectangle & r = finfo.add_face<rectangle>();
        compute_start_stop_angle_face(FACE_STOP_ANGLE, r, finfo.grab_positioning());
        face_identifier fid;
        fid.set_face_bit(FACE_STOP_ANGLE);
        finfo.set_identifier(fid);
        finfo.set_label("stop_angle");
        nfaces++;
      }

    }

    return nfaces;
  }

  double tube::get_xmin () const
  {
    return -_outer_r_;
  }

  double tube::get_xmax () const
  {
    return +_outer_r_;
  }

  double tube::get_ymin () const
  {
    return -_outer_r_;
  }

  double tube::get_ymax () const
  {
    return +_outer_r_;
  }

  double tube::get_zmin () const
  {
    return -0.5 * _z_;
  }

  double tube::get_zmax () const
  {
    return +0.5 * _z_;
  }

  double tube::get_z () const
  {
    return _z_;
  }

  void tube::set_z (double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0, std::logic_error ,
                 "Invalid '" << new_value_ << "' Z value !");
    _z_ = new_value_;
    return;
  }

  double tube::get_half_z () const
  {
    return 0.5 * _z_;
  }

  void tube::set_half_z (double new_value_)
  {
    set_z(2 * new_value_);
    return;
  }

  bool tube::is_extruded () const
  {
    return has_inner_r() && _inner_r_ > 0.0;
  }

  bool tube::has_inner_r() const
  {
    return datatools::is_valid(_inner_r_);
  }

  double tube::get_inner_r () const
  {
    return _inner_r_;
  }

  void tube::set_inner_r(double inner_r_)
  {
    DT_THROW_IF (inner_r_ < 0.0, std::domain_error, "Invalid inner radius '" << inner_r_ << "' !");
    DT_THROW_IF (datatools::is_valid(_outer_r_) && (inner_r_ >= _outer_r_),
                 std::domain_error, "Invalid inner radius '" << inner_r_ << "' (> outer radius) !");
    _inner_r_ = inner_r_;
    return;
  }

  void tube::set_outer_r(double outer_r_)
  {
    DT_THROW_IF (outer_r_ <= 0.0, std::logic_error, "Invalid outer radius '" << outer_r_ << "' !");
    DT_THROW_IF (datatools::is_valid(_inner_r_) && (_inner_r_ >= outer_r_),
                 std::domain_error, "Invalid outer radius '" << outer_r_ << "' (< inner radius) !");
    _outer_r_ = outer_r_;
    return;
  }

  double tube::get_outer_r () const
  {
    return _outer_r_;
  }

  void tube::set_radii (double inner_r_, double outer_r_)
  {
    datatools::invalidate(_inner_r_);
    set_outer_r(outer_r_);
    set_inner_r(inner_r_);
    return;
  }

  void tube::set(double or_, double z_)
  {
    datatools::invalidate(_inner_r_);
    set_outer_r(or_);
    set_z(z_);
    return;
  }

  void tube::set(double ir_, double or_, double z_)
  {
    set_radii(ir_ ,or_);
    set_z(z_);
    return;
  }

  void tube::set(double ir_, double or_, double z_, double start_phi_, double delta_phi_)
  {
    set_radii(ir_ ,or_);
    set_z(z_);
    _phi_domain_.set_partial_angles(start_phi_, delta_phi_);
    return;
  }

  void tube::set_half(double ir_, double or_, double hz_)
  {
    set_radii(ir_, or_);
    set_half_z(hz_);
    return;
  }

  void tube::set_phi(double start_phi_, double delta_phi_)
  {
    DT_THROW_IF(start_phi_ < 0.0,
                std::domain_error,
                "Start phi is negative !");
    DT_THROW_IF(start_phi_ >= 2 * M_PI,
                std::domain_error,
                "Start phi is too large (> 2 pi) !");
    DT_THROW_IF(delta_phi_ > 2 * M_PI,
                std::domain_error,
                "Delta phi is too large (> 2 pi)!");
    DT_THROW_IF(delta_phi_ < 0,
                std::domain_error,
                "Delta phi is negative (< 0)!");
    _phi_domain_.set_partial_angles(start_phi_, delta_phi_);
    return;
  }

  const angular_range & tube::get_phi_domain() const
  {
    return _phi_domain_;
  }

  bool tube::has_partial_phi() const
  {
    return _phi_domain_.is_partial();
  }

  bool tube::has_start_phi() const
  {
    return _phi_domain_.has_start_angle();
  }

  void tube::set_start_phi(double new_value_)
  {
    _phi_domain_.set_start_angle(new_value_);
    return;
  }

  double tube::get_start_phi() const
  {
    return _phi_domain_.get_start_angle();
  }

  bool tube::has_delta_phi() const
  {
    return _phi_domain_.has_delta_angle();
  }

  void tube::set_delta_phi(double new_value_)
  {
    _phi_domain_.set_delta_angle(new_value_);
    return;
  }

  double tube::get_delta_phi() const
  {
    return _phi_domain_.get_delta_angle();
  }

  tube::tube ()
  {
    reset ();
    return;
  }

  tube::tube ( double or_, double z_)
  {
    reset ();
    set (or_, z_);
    return;
  }

  tube::tube (double ir_, double or_, double z_)
  {
    reset ();
    set (ir_, or_, z_);
    return;
  }

  tube::tube (double ir_, double or_, double z_, double start_phi_, double delta_phi_)
  {
    reset ();
    set (ir_, or_, z_, start_phi_, delta_phi_);
    return;
  }

  tube::~tube ()
  {
    return;
  }

  double tube::get_surface (uint32_t mask_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid tube!");
    double s = 0.0;
    double ir = 0.0;
    if (has_inner_r()) {
      ir = _inner_r_;
    }
    double angle = 2 * M_PI;
    if (has_partial_phi()) {
      angle = get_delta_phi();
    }
    // std::cerr << "DEVEL: geomtools::tube::get_surface: angle = " << angle << std::endl;
    // std::cerr << "DEVEL: geomtools::tube::get_surface: ir    = " << ir << std::endl;
    if (mask_ & FACE_INNER_SIDE) {
      s += angle * ir * _z_;
      // std::cerr << "DEVEL: geomtools::tube::get_surface: s(+FACE_INNER_SIDE) = " << s << std::endl;
    }
    if ( mask_ & FACE_OUTER_SIDE ) {
      s += angle * _outer_r_ * _z_;
      // std::cerr << "DEVEL: geomtools::tube::get_surface: s(+FACE_OUTER_SIDE) = " << s << std::endl;
    }
    if ( mask_ & FACE_BOTTOM ) {
      s += 0.5 * angle * (_outer_r_ * _outer_r_ - ir * ir);
      // std::cerr << "DEVEL: geomtools::tube::get_surface: s(+FACE_BOTTOM) = " << s << std::endl;
    }
    if ( mask_ & FACE_TOP ) {
      s += 0.5 * angle * (_outer_r_ * _outer_r_ - ir * ir);
      // std::cerr << "DEVEL: geomtools::tube::get_surface: s(+FACE_TOP) = " << s << std::endl;
    }
    if (has_partial_phi()) {
      if (mask_ & FACE_START_ANGLE) {
        s += (_outer_r_ - ir) * _z_;
        // std::cerr << "DEVEL: geomtools::tube::get_surface: s(+FACE_START_ANGLE) = " << s << std::endl;
      }
      if (mask_ & FACE_STOP_ANGLE) {
        s += (_outer_r_ - ir) * _z_;
        // std::cerr << "DEVEL: geomtools::tube::get_surface: s(+FACE_STOP_ANGLE) = " << s << std::endl;
      }
    }
    return s;
  }

  double tube::get_volume(uint32_t flags_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid tube!");
    double angle = 2 * M_PI;
    if (has_partial_phi()) {
      angle = get_delta_phi();
    }
    double ir = 0.0;
    if (has_inner_r()) {
      ir = _inner_r_;
    }
    double s = 0.0;
    if (flags_ & VOLUME_BULK) {
      s += 0.5 * angle * (_outer_r_ * _outer_r_ - ir * ir);
    }
    if (flags_ & VOLUME_CAVITY) {
      s += 0.5 * angle * (ir * ir);
    }
    return s * _z_;
  }

  bool tube::is_valid () const
  {
    if (! datatools::is_valid(_outer_r_)) {
      return false;
    }
    if (! datatools::is_valid(_z_)) {
      return false;
    }
    return _phi_domain_.is_valid();
  }

  void tube::_set_defaults()
  {
    datatools::invalidate(_inner_r_);
    datatools::invalidate(_outer_r_);
    datatools::invalidate(_z_);
    _phi_domain_.set_type(angular_range::RANGE_TYPE_AZIMUTHAL);
    _phi_domain_.reset_partial_angles();
    return;
  }

  void tube::reset ()
  {
    unlock();
    _set_defaults();
    this->i_shape_3d::reset();
    return;
  }

  std::string tube::get_shape_name () const
  {
    return tube::tube_label();
  }

  bool tube::is_inside ( const vector_3d & point_ , double skin_ ) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid tube!");
    double skin = get_skin (skin_);
    double angular_tolerance = get_tolerance();
    double hskin = 0.5 * skin;
    double r = hypot(point_.x(), point_.y());
    if ( (r <= (_outer_r_ - hskin))
         && (r >= (_inner_r_ + hskin))
         && (std::abs(point_.z()) <= (0.5 * _z_ - hskin))
         ) {
      if (has_partial_phi()) {
        double phi = std::atan2(point_.y(), point_.x());
        return angle_is_in(phi, get_start_phi(), get_delta_phi(), angular_tolerance, true);
      } else {
        return true;
      }
    }
    return false;
  }

  bool tube::is_outside ( const vector_3d & point_ , double skin_ ) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid tube!");
    double skin = get_skin (skin_);
    double angular_tolerance = get_tolerance();
    double hskin = 0.5 * skin;
    double r = hypot(point_.x(), point_.y());
    if (r >= (_outer_r_ + hskin)) {
      return true;
    }
    if (has_inner_r()) {
      if (r <= (_inner_r_ - hskin)) {
        return true;
      }
    }
    if (std::abs(point_.z()) >= (0.5 * _z_ + hskin)) {
      return true;
    }
    if (has_partial_phi()) {
      double phi = std::atan2(point_.y(), point_.x());
      if (! angle_is_in(phi, get_start_phi(), get_delta_phi(), angular_tolerance, true)) {
        return true;
      }
    }
    return false;
  }

  double tube::get_parameter ( const std::string & flag_ ) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid tube!");
    if ( flag_ == "inner.r" ) return get_inner_r ();
    if ( flag_ == "outer.r" ) return get_outer_r ();
    if ( flag_ == "z" ) return get_z ();
    if ( flag_ == "volume" ) return get_volume ();
    if ( flag_ == "surface.top" ) return get_surface (FACE_TOP);
    if ( flag_ == "surface.bottom" ) return get_surface (FACE_BOTTOM);
    if ( flag_ == "surface.inner.side" ) return get_surface (FACE_INNER_SIDE);
    if ( flag_ == "surface.outer.side" ) return get_surface (FACE_OUTER_SIDE);
    if ( flag_ == "surface" ) return get_surface (FACE_ALL);
    DT_THROW_IF(true, std::runtime_error, "Unknown flag !");
  }

  vector_3d
  tube::get_normal_on_surface (const vector_3d & a_position,
                               const face_identifier & a_surface_bit) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid tube!");
    // DT_THROW_IF(! a_surface_bit.is_unique(), std::logic_error, "Invalid face identifier!");
    vector_3d normal;
    geomtools::invalidate(normal);
    switch(a_surface_bit.get_face_bits()) {
    case FACE_OUTER_SIDE:
      {
        double phi = a_position.phi();
        normal.set(std::cos(phi), std::sin(phi), 0.0);
      }
      break;
    case FACE_INNER_SIDE:
      if (has_inner_r()) {
        double phi = a_position.phi();
        normal.set(std::cos(phi), std::sin(phi), 0.0);
        normal *= -1.;
      }
      break;
    case FACE_BOTTOM:
      normal.set(0.0, 0.0, -1.0);
      break;
    case FACE_TOP:
      normal.set(0.0, 0.0, +1.0);
      break;
    case FACE_START_ANGLE:
      if (has_partial_phi()) {
        normal.set(std::sin(get_start_phi()), -std::cos(get_start_phi()), 0.0);
      }
      break;
    case FACE_STOP_ANGLE:
      if (has_partial_phi()) {
        normal.set(std::sin(get_start_phi() + get_delta_phi()), -std::cos(get_start_phi() + get_delta_phi()), 0.0);
      }
      break;
    }
    return normal;
  }

  face_identifier tube::on_surface(const vector_3d & position_,
                                   const face_identifier & surface_mask_,
                                   double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid tube!");
    double skin = get_skin(skin_);

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
      if (finfo.is_valid() && mask.has_face_bit(finfo.get_identifier().get_face_bits())) {
        vector_3d position_c;
        finfo.get_positioning().mother_to_child(position_, position_c);
        if (finfo.get_face_ref().is_on_surface(position_c, skin)) {
          return finfo.get_identifier();
        }
      }
    }

    return face_identifier::face_invalid();
  }

  bool tube::find_intercept (const vector_3d & from_,
                             const vector_3d & direction_,
                             face_intercept_info & intercept_,
                             double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid tube!");
    intercept_.reset ();
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
            intercept_.set_face_id(intercepts[iface].get_face_id());
            intercept_.set_impact(intercepts[iface].get_impact());
          }
        }
      }
    }

    return intercept_.is_ok();
  }

  std::ostream & operator<< (std::ostream & out_, const tube & t_)
  {
    out_ << '{' << tube::tube_label() << ' '
         << t_._inner_r_ << ' '
         << t_._outer_r_ << ' '
         << t_._z_ << ' '
         << t_.get_start_phi() << ' '
         << t_.get_delta_phi() << '}';
    return out_;
  }

  std::istream & operator>> (std::istream & in_, tube & t_)
  {
    // Questionnable!!!
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
    if (name != tube::tube_label())
      {
        in_.clear (std::ios_base::failbit);
        return in_;
      }
    double in_r, out_r, z;
    in_ >> in_r >> out_r >> z;
    if (! in_)
      {
        in_.clear (std::ios_base::failbit);
        return in_;
      }
    double start_phi, delta_phi;
    in_ >> start_phi >> delta_phi;
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
        t_.set (in_r, out_r, z, start_phi, delta_phi);
      }
    catch (...)
      {
        t_.reset ();
        in_.clear (std::ios_base::failbit);
      }
    return in_;
  }

  void tube::compute_inner_cylinder (cylinder & ic_)
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid tube!");
    ic_.reset();
    ic_.set(get_inner_r (), get_z ());
    ic_.lock();
    return;
  }

  void tube::compute_outer_cylinder (cylinder & oc_)
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid tube!");
    oc_.reset ();
    oc_.set(get_outer_r(), get_z());
    oc_.lock();
    return;
  }

  void tube::compute_deflated(tube & deflated_,
                              double by_r_,
                              double by_z_,
                              double by_angle_)
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid tube!");
    double r_eps = 0.0;
    double z_eps = 0.0;
    if (datatools::is_valid(by_r_) and by_r_ > 0.0) r_eps = by_r_;
    if (datatools::is_valid(by_z_) and by_z_ > 0.0) z_eps = by_z_;
    double rmax = get_outer_r();
    double z = get_z();
    double rmin = get_inner_r();
    rmax -= r_eps;
    if (has_inner_r()) {
      rmin += r_eps;
    }
    z -= (2 * z_eps);
    deflated_.reset();
    if (has_inner_r()) {
      deflated_.set(rmin, rmax, z);
    } else {
      deflated_.set(rmax, z);
    }
    double eps_angle = by_angle_;
    if (eps_angle < 0.0) {
      eps_angle = r_eps / rmax;
    }
    if (has_partial_phi()) {
      double start_phi = get_start_phi();
      double delta_phi = get_delta_phi();
      if (eps_angle > 0.0) {
        start_phi += eps_angle;
        delta_phi -= 2 * eps_angle;
        if (delta_phi < 0.0)  {   
          // DT_THROW_IF(std::range_error, "Cannot compute deflated tube!");
          start_phi = get_start_phi() + 0.5 * get_delta_phi() - 0.25 * eps_angle;
          delta_phi = 0.5 * eps_angle;
        }
      }
      deflated_.set_start_phi(start_phi);
      deflated_.set_delta_phi(delta_phi);
    }
    deflated_.lock();
    return;
  }

  void tube::compute_inflated(tube & inflated_,
                              double by_r_,
                              double by_z_,
                              double by_angle_)
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid tube!");
    double r_eps = 0.0;
    double z_eps = 0.0;
    if (datatools::is_valid(by_r_) and by_r_ > 0.0) r_eps = by_r_;
    if (datatools::is_valid(by_z_) and by_z_ > 0.0) z_eps = by_z_;
    double rmax = get_outer_r();
    double z = get_z();
    double rmin = get_inner_r();
    rmax += r_eps;
    if (has_inner_r()) {
      rmin -= r_eps;
    }
    z += (2 * z_eps);
    inflated_.reset();
    if (has_inner_r() and rmin > 0.0) {
      inflated_.set(rmin, rmax, z);
    } else {
      inflated_.set(rmax, z);
    }
    double eps_angle = by_angle_;
    if (eps_angle < 0.0) {
      eps_angle = r_eps / rmax;
    }
    if (has_partial_phi()) {
      double start_phi = get_start_phi();
      double delta_phi = get_delta_phi();
      if (eps_angle > 0.0) {
        start_phi -= eps_angle;
        delta_phi += 2 * eps_angle;
      }
      if (delta_phi < 2 * M_PI) {
        inflated_.set_start_phi(start_phi);
        inflated_.set_delta_phi(delta_phi);
      }
    }
    inflated_.lock();
    return;
  }

  void tube::compute_envelope(tube & envelope_,
                              double r_tolerance_,
                              double z_tolerance_,
                              double angle_tolerance_)
  {
    compute_inflated(envelope_, r_tolerance_, z_tolerance_, angle_tolerance_);
    return;
  }

  void tube::compute_envelope(cylinder & envelope_,
                              double r_tolerance_,
                              double z_tolerance_)
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid tube!");
    double r_eps = 0.0;
    double z_eps = 0.0;
    if (datatools::is_valid(r_tolerance_) and r_tolerance_ > 0.0) r_eps = r_tolerance_;
    if (datatools::is_valid(z_tolerance_) and z_tolerance_ > 0.0) z_eps = z_tolerance_;
    double rmax = get_outer_r();
    double z = get_z();
    rmax += r_eps;
    z += (2 * z_eps);
    envelope_.reset();
    envelope_.set(rmax, z);
    envelope_.lock();
    return;
  }

  void tube::_build_bounding_data()
  {
    // std::cerr << "DEVEL: tube::_build_bounding_data: "
    //           << "Entering..." << std::endl;
    if (! has_partial_phi()) {
      _grab_bounding_data().make_cylinder(get_outer_r(), -0.5 * get_z(), +0.5 * get_z());
    } else {
      mygsl::min_max xrange;
      mygsl::min_max yrange;
      double dphi = std::min(0.5 * CLHEP::degree, get_delta_phi() / 100);
      if (!has_inner_r()) {
        xrange.add(0.0);
        yrange.add(0.0);
      }
      for (double phi = get_start_phi();
           phi <  get_start_phi() + get_delta_phi();
           phi += dphi) {
        double cp = std::cos(phi);
        double sp = std::sin(phi);
        double xo = get_outer_r() * cp;
        if (xo > get_outer_r() * 0.99) {
          xo = get_outer_r();
        }
        if (xo < - get_outer_r() * 0.99) {
          xo = -get_outer_r();
        }
        double yo = get_outer_r() * sp;
        if (yo > get_outer_r() * 0.99) {
          yo = get_outer_r();
        }
        if (yo < - get_outer_r() * 0.99) {
          yo = -get_outer_r();
        }
        xrange.add(xo);
        yrange.add(yo);
        if (has_inner_r()) {
          double xi = get_inner_r() * cp;
          double yi = get_inner_r() * sp;
          xrange.add(xi);
          yrange.add(yi);
        }
      }
      _grab_bounding_data().make_box(xrange.get_min(), xrange.get_max(),
                                     yrange.get_min(), yrange.get_max(),
                                     get_zmin(), get_zmax());
    }
    // std::cerr << "DEVEL: tube::_build_bounding_data: "
    //           << "Exiting." << std::endl;
    return;
  }

  void tube::tree_dump (ostream & out_,
                        const string & title_,
                        const string & indent_,
                        bool inherit_) const
  {
    i_shape_3d::tree_dump (out_, title_, indent_, true);

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "R(internal) : " << get_inner_r () / CLHEP::mm << " mm" << endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "R(external) : " << get_outer_r () / CLHEP::mm << " mm" << endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Z : " << get_z () / CLHEP::mm << " mm" << endl;

    {
      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Phi domain : " << std::endl;
      std::ostringstream indent2;
      indent2 << indent_ << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
      _phi_domain_.tree_dump(out_, "", indent2.str());
    }

    return;
  }

  void tube::generate_wires_self(wires_type & wires_,
                                 uint32_t options_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid tube!");

    // bool debug_explode    = options_ & WR_BASE_EXPLODE;
    bool draw_bottom      = !(options_ & WR_TUBE_NO_BOTTOM_FACE);
    bool draw_top         = !(options_ & WR_TUBE_NO_TOP_FACE);
    bool draw_inner       = !(options_ & WR_TUBE_NO_INNER_FACE);
    bool draw_outer       = !(options_ & WR_TUBE_NO_OUTER_FACE);
    // bool draw_start_angle = !(options_ & WR_TUBE_NO_START_ANGLE_FACE);
    // bool draw_stop_angle  = !(options_ & WR_TUBE_NO_STOP_ANGLE_FACE);

    bool draw_boundings   =  (options_ & WR_BASE_BOUNDINGS);
    if (draw_boundings) {
      get_bounding_data().generate_wires_self(wires_, 0);
    }

    // Extract base rendering options:
    uint32_t base_options = options_ & WR_BASE_MASK;

    // double explode_factor = 1.0;
    // if (debug_explode) {
    //   explode_factor = 1.25;
    // }

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

    if (has_partial_phi()) {

      {
        rectangle start_face;
        placement start_face_placement;
        compute_start_stop_angle_face(FACE_START_ANGLE, start_face, start_face_placement);
        uint32_t options = base_options;
        start_face.generate_wires(wires_, start_face_placement, options);
      }

      {
        rectangle stop_face;
        placement stop_face_placement;
        compute_start_stop_angle_face(FACE_STOP_ANGLE, stop_face, stop_face_placement);
        uint32_t options = base_options;
        stop_face.generate_wires(wires_, stop_face_placement, options);
      }

    }

    if (has_inner_r()) {
      if (draw_inner) {
        cylindrical_sector inner_side_face;
        placement inner_side_face_placement;
        compute_side_face(FACE_INNER_SIDE, inner_side_face, inner_side_face_placement);
        uint32_t options = base_options;
        if (edge_bottom) {
          options |= cylindrical_sector::WR_CYLSEC_NO_BOTTOM_EDGE;
        }
        if (edge_top) {
          options |= cylindrical_sector::WR_CYLSEC_NO_TOP_EDGE;
        }
        inner_side_face.generate_wires(wires_, inner_side_face_placement, options);
      }
    }

    if (draw_outer) {
      cylindrical_sector outer_side_face;
      placement outer_side_face_placement;
      compute_side_face(FACE_OUTER_SIDE, outer_side_face, outer_side_face_placement);
      uint32_t options = base_options;
      if (edge_bottom) {
        options |= cylindrical_sector::WR_CYLSEC_NO_BOTTOM_EDGE;
      }
      if (edge_top) {
        options |= cylindrical_sector::WR_CYLSEC_NO_TOP_EDGE;
      }
      outer_side_face.generate_wires(wires_, outer_side_face_placement, options);
    }

    return;
  }

  void tube::initialize(const datatools::properties & config_,
                        const handle_dict_type * objects_)
  {
    bool devel = false;
    // devel = true;
    this->i_shape_3d::_initialize(config_, objects_);

    if (!is_valid()) {

      double lunit = CLHEP::mm;
      if (config_.has_key("length_unit")) {
        const std::string lunit_str = config_.fetch_string("length_unit");
        lunit = datatools::units::get_length_unit_from(lunit_str);
      }
      if (devel) std::cerr << "DEVEL: tube::initialize: "
                           << "lunit = " << lunit / CLHEP::mm << " mm" << std::endl;

      double inner_r;
      datatools::invalidate(inner_r);
      if (config_.has_key ("inner_r")) {
        inner_r = config_.fetch_real ("inner_r");
        if (devel) std::cerr << "DEVEL: geomtools::tube::initialize: inner_r = " << inner_r << std::endl;
        if (! config_.has_explicit_unit ("inner_r")) {
          inner_r *= lunit;
        }
      } else if (config_.has_key ("inner_radius")) {
        inner_r = config_.fetch_real ("inner_radius");
        if (! config_.has_explicit_unit ("inner_radius")) {
          inner_r *= lunit;
        }
      } else if (config_.has_key ("inner_diameter")) {
        inner_r = 0.5 * config_.fetch_real ("inner_diameter");
        if (! config_.has_explicit_unit ("inner_diameter")) {
          inner_r *= lunit;
        }
      }
      if (! datatools::is_valid(inner_r)) {
        // DT_LOG_WARNING (get_logging_priority (),
        //                 "Missing tube 'inner_r' property ! Using 0 default inner radius !");
        inner_r = 0.0;
      }
      if (devel) std::cerr << "DEVEL: geomtools::tube::initialize: FINAL inner_r = " << inner_r << std::endl;

      double outer_r;
      datatools::invalidate(outer_r);
      if (config_.has_key ("outer_r")) {
        outer_r = config_.fetch_real ("outer_r");
        if (! config_.has_explicit_unit ("outer_r")) {
          outer_r *= lunit;
        }
      } else if (config_.has_key ("outer_radius")) {
        outer_r = config_.fetch_real ("outer_radius");
        if (! config_.has_explicit_unit ("outer_radius")) {
          outer_r *= lunit;
        }
      } else if (config_.has_key ("outer_diameter")) {
        outer_r = 0.5 * config_.fetch_real ("outer_diameter");
        if (! config_.has_explicit_unit ("outer_diameter")) {
          outer_r *= lunit;
        }
      }
      DT_THROW_IF (! datatools::is_valid(outer_r), std::logic_error,
                   "Missing tube 'outer_r' property !");

      DT_THROW_IF (! config_.has_key("z"), std::logic_error,
                   "Missing tube 'z' property !");
      double z = config_.fetch_real("z");
      if (! config_.has_explicit_unit("z")) {
        z *= lunit;
      }
      set_radii(inner_r, outer_r);
      set_z(z);

      datatools::properties phi_config;
      config_.export_and_rename_starting_with(phi_config, "phi.", "");
      if (phi_config.size()) {
        _phi_domain_.initialize(phi_config);
      } else {
        // Deprecated:
        double aunit = CLHEP::degree;
        if (config_.has_key("angle_unit")) {
          const std::string aunit_str = config_.fetch_string("angle_unit");
          aunit = datatools::units::get_angle_unit_from(aunit_str);
        }
        if (devel) std::cerr << "DEVEL: tube::initialize: "
                             << "aunit = " << aunit / CLHEP::degree << " degree" << std::endl;
        double start_phi = 0.0;
        double delta_phi = 2 * M_PI * CLHEP::radian;
        bool not_full_phi = false;
        if (config_.has_key ("start_phi")) {
          start_phi = config_.fetch_real ("start_phi");
          if (! config_.has_explicit_unit ("start_phi")) {
            start_phi *= aunit;
          }
          not_full_phi = true;
        }
        if (config_.has_key ("delta_phi")) {
          delta_phi = config_.fetch_real ("delta_phi");
          if (! config_.has_explicit_unit ("delta_phi")) {
            delta_phi *= aunit;
          }
          not_full_phi = true;
        }
        if (not_full_phi) {
          set_phi(start_phi, delta_phi);
        }
      }
    }

    lock();
    return;
  }

  // static
  void tube::init_ocd(datatools::object_configuration_description & ocd_)
  {
    i_shape_3d::init_ocd(ocd_);

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("length_unit")
        .set_from("geomtools::tube")
        .set_terse_description("The unit symbol used for length unit")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_default_value_string("mm")
        .set_long_description("A character string that represents the default    \n"
                              "length unit for length parameters of the tube.\n"
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
      cpd.set_name_pattern("inner_r")
        .set_from("geomtools::tube")
        .set_terse_description("The inner radius of the tube")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_default_value_real(0.0)
        .add_example("Set the inner radius::               \n"
                     "                                     \n"
                     "  inner_r : real as length = 2.0 cm  \n"
                     "                                     \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("inner_radius")
        .set_from("geomtools::cylinder")
        .set_terse_description("The inner radius of the cylinder")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_long_description("Alias of the ``inner_r`` property.\n"
                              "Not used if the ``inner_r`` property is set."
                              )
        .add_example("Set the inner radius::                    \n"
                     "                                          \n"
                     "  inner_radius : real as length = 2.0 cm  \n"
                     "                                          \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("inner_diameter")
        .set_from("geomtools::cylinder")
        .set_terse_description("The inner diameter of the cylinder")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_long_description("Not used if the ``inner_r`` or ``inner_radius`` property is set.")
        .add_example("Set the inner diameter::                    \n"
                     "                                            \n"
                     "  inner_diameter : real as length = 4.0 cm  \n"
                     "                                            \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("outer_r")
        .set_from("geomtools::tube")
        .set_terse_description("The outer radius of the tube")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(true)
        .add_example("Set the outer radius::               \n"
                     "                                     \n"
                     "  outer_r : real as length = 5.0 cm  \n"
                     "                                     \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("outer_radius")
        .set_from("geomtools::cylinder")
        .set_terse_description("The outer radius of the cylinder")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_long_description("Not used if the ``outer_r`` property is set.")
        .add_example("Set the outer radius::                   \n"
                     "                                         \n"
                     "  outer_radius : real as length = 5.0 cm \n"
                     "                                         \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("outer_diameter")
        .set_from("geomtools::cylinder")
        .set_terse_description("The outer diameter of the cylinder")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_long_description("Not used if the ``outer_r`` or ``outer_radius`` property is set.")
        .add_example("Set the outer diameter::                    \n"
                     "                                            \n"
                     "  outer_diameter : real as length = 10.0 cm \n"
                     "                                            \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("z")
        .set_from("geomtools::tube")
        .set_terse_description("The dimension of the tube along the Z axis")
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
      cpd.set_name_pattern("start_phi")
        .set_from("geomtools::tube")
        .set_terse_description("The starting phi angle")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_default_value_real(0.0)
        .add_example("Set the starting phi angle::              \n"
                     "                                          \n"
                     "  start_phi : real as angle = 30.0 degree \n"
                     "                                          \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd
        = ocd_.add_property_info();
      cpd.set_name_pattern("delta_phi")
        .set_from("geomtools::tube")
        .set_terse_description("The delta phi angle")
        .set_traits(datatools::TYPE_REAL)
        .set_mandatory(false)
        .set_default_value_real(2 * M_PI * CLHEP::radian)
        .add_example("Set the delta phi angle::                 \n"
                     "                                          \n"
                     "  delta_phi : real as angle = 60.0 degree \n"
                     "                                          \n"
                     )
        ;
    }

    return;
  }

} // end of namespace geomtools

  /** Opening macro for implementation
   *  @arg geomtools::tube the full class name
   *  @arg ocd_ is the identifier of the 'datatools::object_configuration_description'
   *            to be initialized(passed by mutable reference).
   *  This macro must be used outside of any namespace.
   */
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(geomtools::tube, ocd_)
{
  // The class name :
  ocd_.set_class_name("geomtools::tube");

  // The class terse description :
  ocd_.set_class_description("A class representing a 3D tube solid shape");

  // The library the class belongs to :
  ocd_.set_class_library("geomtools");

  // The class detailed documentation :
  ocd_.set_class_documentation("The tube solid 3D-shape is defined by its three  \n"
                               "dimensions.                                       \n"
                               );

  // Populate the specific OCD :
  geomtools::tube::init_ocd(ocd_);

  // Additionnal configuration hints :
  ocd_.set_configuration_hints("Here is a full configuration example in the      \n"
                               "``datatools::properties`` ASCII format::         \n"
                               "                                                 \n"
                               "  tolerance.length : real = 1.0e-6 um            \n"
                               "  length_unit      : string = \"mm\"             \n"
                               "  inner_r : real as length = 15.0 mm             \n"
                               "  outer_r : real as length = 25.0 mm             \n"
                               "  z       : real           = 25.3 cm             \n"
                               "  phi.start : real as angle = 30 degree          \n"
                               "  phi.delta : real as angle = 60 degree          \n"
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

// Registration macro for class 'geomtools::tube' :
DOCD_CLASS_SYSTEM_REGISTRATION(geomtools::tube, "geomtools::tube")
