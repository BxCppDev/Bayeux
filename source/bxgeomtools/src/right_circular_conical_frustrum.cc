// right_circular_conical_frustrum.cc

// Ourselves:
#include <geomtools/right_circular_conical_frustrum.h>

// Standard library:
#include <cmath>
#include <iostream>

// Third-party:
// - GSL:
#include <gsl/gsl_math.h>
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/units.h>
// - Bayeux/mygsl:
#include <mygsl/polynomial.h>
#include <mygsl/min_max.h>

// This project:
#include <geomtools/circle.h>
#include <geomtools/disk.h>
#include <geomtools/line_3d.h>
#include <geomtools/quadrangle.h>
#include <geomtools/triangle.h>
#include <geomtools/right_circular_conical_nappe.h>

namespace geomtools {

  // Registration :
  GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(right_circular_conical_frustrum,
                                             "geomtools::right_circular_conical_frustrum")

  const std::string & right_circular_conical_frustrum::rccf_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "right_circular_conical_frustrum";
    }
    return label;
  }

  std::string right_circular_conical_frustrum::get_shape_name () const
  {
    return right_circular_conical_frustrum::rccf_label();
  }

  bool right_circular_conical_frustrum::is_valid () const
  {
    if (!datatools::is_valid(_outer_bottom_radius_)) {
      return false;
    }
    if (!datatools::is_valid(_outer_top_radius_)) {
      return false;
    }
    if (!datatools::is_valid(_z_)) {
      return false;
    }
    return _angle_domain_.is_valid();
  }

  bool right_circular_conical_frustrum::has_top_face() const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid conical frustrum!");
    // std::cerr << "DEVEL: right_circular_conical_frustrum::has_top_face: "
    //           << "Entering..." << std::endl;
    // std::cerr << "DEVEL: right_circular_conical_frustrum::has_top_face: "
    //           << "  _outer_top_radius_ = " << _outer_top_radius_ << std::endl;
    // std::cerr << "DEVEL: right_circular_conical_frustrum::has_top_face: "
    //           << "  _inner_top_radius_ = " << _inner_top_radius_ << std::endl;
    if (datatools::is_valid(_outer_top_radius_)) {
      // std::cerr << "DEVEL: right_circular_conical_frustrum::has_top_face: "
      //           << "  _outer_top_radius_ is valid."
      //           << std::endl;
      if (_outer_top_radius_ == 0.0) {
        // std::cerr << "DEVEL: right_circular_conical_frustrum::has_top_face: "
        //           << "  _outer_top_radius_ is zero."
        //           << std::endl;
        return false;
      } else if (datatools::is_valid(_inner_top_radius_)) {
        // std::cerr << "DEVEL: right_circular_conical_frustrum::has_top_face: "
        //           << "  _inner_top_radius_ is valid."
        //           << std::endl;
        if (_outer_top_radius_ == _inner_top_radius_) {
          // std::cerr << "DEVEL: right_circular_conical_frustrum::has_top_face: "
          //           << "  _outer_top_radius_ == _inner_top_radius_."
          //           << std::endl;
          return false;
        }
      }
    }
    return true;
  }

  bool right_circular_conical_frustrum::has_bottom_face() const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid conical frustrum!");
    if (datatools::is_valid(_outer_bottom_radius_)) {
      if (_outer_bottom_radius_ == 0.0) {
        return false;
      } else if (datatools::is_valid(_inner_bottom_radius_)) {
        if (_outer_bottom_radius_ == _inner_bottom_radius_) {
          return false;
        }
      }
    }
    return true;
  }

  bool right_circular_conical_frustrum::has_inner_face() const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid conical frustrum!");
    if (has_inner_bottom_radius() || has_inner_top_radius()) {
      return true;
    }
    return false;
  }

  double right_circular_conical_frustrum::get_xmin () const
  {
    return -std::max(_outer_bottom_radius_, _outer_top_radius_);
  }

  double right_circular_conical_frustrum::get_xmax () const
  {
    return +std::max(_outer_bottom_radius_, _outer_top_radius_);
  }

  double right_circular_conical_frustrum::get_ymin () const
  {
    return -std::max(_outer_bottom_radius_, _outer_top_radius_);
  }

  double right_circular_conical_frustrum::get_ymax () const
  {
    return +std::max(_outer_bottom_radius_, _outer_top_radius_);
  }

  double right_circular_conical_frustrum::get_zmin () const
  {
    return -0.5 *_z_;
  }

  double right_circular_conical_frustrum::get_zmax () const
  {
    return +0.5 *_z_;
  }

  double right_circular_conical_frustrum::get_z() const
  {
    return _z_;
  }

  void right_circular_conical_frustrum::set_z(double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0, std::domain_error ,
                 "Invalid '" << new_value_ << "' Z value !");
    _z_ = new_value_;
    return;
  }

  bool right_circular_conical_frustrum::has_inner_bottom_radius() const
  {
    return datatools::is_valid(_inner_bottom_radius_) && _inner_bottom_radius_ > 0;
  }

  void right_circular_conical_frustrum::set_inner_bottom_radius(double inner_r_)
  {
    DT_THROW_IF (inner_r_ < 0.0, std::domain_error, "Invalid inner bottom radius '" << inner_r_ << "' !");
    DT_THROW_IF (_outer_bottom_radius_ < inner_r_,
                 std::domain_error,
                 "Outer bottom radius '"
                 << _outer_bottom_radius_ << "' <= inner bottom radius '"
                 << inner_r_ << "'!");
    _inner_bottom_radius_ = inner_r_;
    if (!datatools::is_valid(_inner_top_radius_) && datatools::is_valid(_inner_bottom_radius_)) {
      _inner_top_radius_ = 0.0;
    }
    return;
  }

  double right_circular_conical_frustrum::get_inner_bottom_radius() const
  {
    return _inner_bottom_radius_;
  }

  void right_circular_conical_frustrum::set_outer_bottom_radius(double outer_r_)
  {
    DT_THROW_IF(outer_r_ < 0.0, std::domain_error, "Invalid outer bottom radius '" << outer_r_ << "' !");
    DT_THROW_IF (outer_r_ < _inner_bottom_radius_,
                 std::domain_error,
                 "Outer bottom radius '"
                 << outer_r_ << "' <= inner bottom radius '"
                 << _inner_bottom_radius_ << "'!");
    _outer_bottom_radius_ = outer_r_;
    return;
  }

  double right_circular_conical_frustrum::get_outer_bottom_radius() const
  {
    return _outer_bottom_radius_;
  }

  bool right_circular_conical_frustrum::has_inner_top_radius() const
  {
    return datatools::is_valid(_inner_top_radius_) && _inner_top_radius_ > 0;
  }

  void right_circular_conical_frustrum::set_inner_top_radius(double inner_r_)
  {
    DT_THROW_IF (inner_r_ < 0.0, std::domain_error, "Invalid inner top radius '" << inner_r_ << "' !");
    DT_THROW_IF (_outer_top_radius_ < inner_r_,
                 std::domain_error,
                 "Outer top radius '"
                 << _outer_top_radius_ << "' <= inner top radius '"
                 << inner_r_ << "'!");
    _inner_top_radius_ = inner_r_;
    if (!datatools::is_valid(_inner_bottom_radius_) && datatools::is_valid(_inner_top_radius_)) {
      _inner_bottom_radius_ = 0.0;
    }
    return;
  }

  double right_circular_conical_frustrum::get_inner_top_radius() const
  {
    return _inner_top_radius_;
  }

  void right_circular_conical_frustrum::set_outer_top_radius(double outer_r_)
  {
    DT_THROW_IF(outer_r_ < 0.0, std::domain_error, "Invalid outer top radius '" << outer_r_ << "' !");
    DT_THROW_IF (outer_r_ < _inner_top_radius_,
                 std::domain_error,
                 "Outer top radius '"
                 << outer_r_ << "' <= inner top radius '"
                 << _inner_top_radius_ << "'!");
    _outer_top_radius_ = outer_r_;
    return;
  }

  double right_circular_conical_frustrum::get_outer_top_radius() const
  {
    return _outer_top_radius_;
  }

  const angular_range & right_circular_conical_frustrum::get_angle_domain() const
  {
    return _angle_domain_;
  }

  bool right_circular_conical_frustrum::has_partial_angle() const
  {
    return _angle_domain_.is_partial();
  }

  bool right_circular_conical_frustrum::has_start_angle() const
  {
    return _angle_domain_.has_start_angle();
  }

  void right_circular_conical_frustrum::set_start_angle(double new_value_)
  {
    _angle_domain_.set_start_angle(new_value_);
    return;
  }

  double right_circular_conical_frustrum::get_start_angle() const
  {
    return _angle_domain_.get_start_angle();
  }

  bool right_circular_conical_frustrum::has_delta_angle() const
  {
    return _angle_domain_.has_delta_angle();
  }

  void right_circular_conical_frustrum::set_delta_angle(double new_value_)
  {
    _angle_domain_.set_delta_angle(new_value_);
    return;
  }

  double right_circular_conical_frustrum::get_delta_angle() const
  {
    return _angle_domain_.get_delta_angle();
  }

  right_circular_conical_frustrum::right_circular_conical_frustrum()
  {
    _set_defaults();
    return;
  }

  right_circular_conical_frustrum::right_circular_conical_frustrum(double inner_bottom_radius_,
                                                                   double outer_bottom_radius_,
                                                                   double inner_top_radius_,
                                                                   double outer_top_radius_,
                                                                   double z_)
  {
    _set_defaults();
    set_inner_bottom_radius(inner_bottom_radius_);
    set_outer_bottom_radius(outer_bottom_radius_);
    set_inner_top_radius(inner_top_radius_);
    set_outer_top_radius(outer_top_radius_);
    set_z(z_);
    return;
  }

  right_circular_conical_frustrum::right_circular_conical_frustrum(double inner_bottom_radius_,
                                                                   double outer_bottom_radius_,
                                                                   double inner_top_radius_,
                                                                   double outer_top_radius_,
                                                                   double z_,
                                                                   double start_angle_,
                                                                   double delta_angle_)
  {
    _set_defaults();
    set_inner_bottom_radius(inner_bottom_radius_);
    set_outer_bottom_radius(outer_bottom_radius_);
    set_inner_top_radius(inner_top_radius_);
    set_outer_top_radius(outer_top_radius_);
    set_z(z_);
    _angle_domain_.set_partial_angles(start_angle_, delta_angle_);
    return;
  }

  right_circular_conical_frustrum::~right_circular_conical_frustrum()
  {
    return;
  }

  void right_circular_conical_frustrum::_set_defaults()
  {
    datatools::invalidate(_inner_bottom_radius_);
    datatools::invalidate(_outer_bottom_radius_);
    datatools::invalidate(_inner_top_radius_);
    datatools::invalidate(_outer_top_radius_);
    datatools::invalidate(_z_);
    _angle_domain_.set_type(angular_range::RANGE_TYPE_AZIMUTHAL);
    _angle_domain_.reset_partial_angles();
    return;
  }

  void right_circular_conical_frustrum::reset ()
  {
    unlock();
    _set_defaults();
    this->i_shape_3d::_reset();
    return;
  }
  void right_circular_conical_frustrum::tree_dump (std::ostream & out_,
                                                   const std::string & title_,
                                                   const std::string & indent_,
                                                   bool inherit_) const
  {
    i_shape_3d::tree_dump (out_, title_, indent_, true);

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Inner bottom radius : ";
    if (datatools::is_valid(_inner_bottom_radius_)) {
      out_ << _inner_bottom_radius_ / CLHEP::mm << " mm";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Outer bottom radius : ";
    if (datatools::is_valid(_outer_bottom_radius_)) {
      out_ << _outer_bottom_radius_ / CLHEP::mm << " mm";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Inner top radius : ";
    if (datatools::is_valid(_inner_top_radius_)) {
      out_ << _inner_top_radius_ / CLHEP::mm << " mm";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Outer top radius : ";
    if (datatools::is_valid(_outer_top_radius_)) {
      out_ << _outer_top_radius_ / CLHEP::mm << " mm";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Z : ";
    if (datatools::is_valid(_z_)) {
      out_ << _z_ / CLHEP::mm << " mm";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    {
      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Angle domain : " << std::endl;
      std::ostringstream indent2;
      indent2 << indent_ << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
      _angle_domain_.tree_dump(out_, "", indent2.str());
    }

    return;
  }

  void right_circular_conical_frustrum::compute_inner_face(right_circular_conical_nappe & in_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Frustrum is not valid !");
    in_.reset();
    if (has_inner_face()) {
      double ibr = 0.0;
      if (has_inner_bottom_radius()) {
        ibr = get_inner_bottom_radius();
      }
      double itr = 0.0;
      if (has_inner_top_radius()) {
        itr = get_inner_top_radius();
      }
      if (ibr > 0.0 || itr > 0.0) {
        in_.set_bottom_radius(ibr);
        in_.set_top_radius(itr);
        in_.set_z(get_z());
        if (has_partial_angle()) {
          in_.set_start_angle(get_start_angle());
          in_.set_delta_angle(get_delta_angle());
        }
      }
    }
    return;
  }

  void right_circular_conical_frustrum::compute_top_face(disk & top_disk_,
                                                         placement & face_placement_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Frustrum is not valid !");
    top_disk_.reset();
    if (has_top_face()) {
      top_disk_.set(_inner_top_radius_, _outer_top_radius_);
      if (has_partial_angle()) {
        top_disk_.set_start_angle(get_start_angle());
        top_disk_.set_delta_angle(get_delta_angle());
      }
      face_placement_.set(0.0, 0.0, +0.5 * _z_, 0.0, 0.0, 0.0);
    }
    return;
  }

  void right_circular_conical_frustrum::compute_bottom_face(disk & bottom_disk_,
                                                            placement & face_placement_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Frustrum is not valid !");
    bottom_disk_.reset();
    if (has_bottom_face()) {
      bottom_disk_.set(_inner_bottom_radius_, _outer_bottom_radius_);
      if (has_partial_angle()) {
        bottom_disk_.set_start_angle(get_start_angle());
        bottom_disk_.set_delta_angle(get_delta_angle());
      }
      face_placement_.set(0.0, 0.0, -0.5 * _z_, 0.0, 0.0, 0.0);
    }
    return;
  }

  void right_circular_conical_frustrum::compute_outer_face(right_circular_conical_nappe & on_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Frustrum is not valid !");
    on_.reset();
    on_.set_bottom_radius(get_outer_bottom_radius());
    on_.set_top_radius(get_outer_top_radius());
    on_.set_z(get_z());
    if (has_partial_angle()) {
      on_.set_start_angle(get_start_angle());
      on_.set_delta_angle(get_delta_angle());
    }
    return;
  }

  right_circular_conical_frustrum::ssaf_type
  right_circular_conical_frustrum::get_start_stop_angle_face_type() const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Frustrum is not valid !");
    ssaf_type ssaf = SSAF_INVALID;
    if (has_partial_angle()) {
      if (has_bottom_face() && ! has_top_face()) {
        ssaf = SSAF_TRIANGLE_BOTTOM_BASE;
      } else if (!has_bottom_face() && has_top_face()) {
        ssaf = SSAF_TRIANGLE_TOP_BASE;
      } else if (has_bottom_face() && has_top_face()) {
        ssaf = SSAF_QUADRANGLE;
      }
    }
    return ssaf;
  }

  bool right_circular_conical_frustrum::start_stop_angle_face_is_triangle_top() const
  {
    return get_start_stop_angle_face_type() == SSAF_TRIANGLE_TOP_BASE;
  }

  bool right_circular_conical_frustrum::start_stop_angle_face_is_triangle_bottom() const
  {
    return get_start_stop_angle_face_type() == SSAF_TRIANGLE_BOTTOM_BASE;
  }

  bool right_circular_conical_frustrum::start_stop_angle_face_is_triangle() const
  {
    return start_stop_angle_face_is_triangle_top() || start_stop_angle_face_is_triangle_bottom();
  }

  bool right_circular_conical_frustrum::start_stop_angle_face_is_quadrangle() const
  {
    return get_start_stop_angle_face_type() == SSAF_QUADRANGLE;
  }

  void right_circular_conical_frustrum::compute_start_stop_angle_face(quadrangle & qface_,
                                                                      triangle & tface_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Frustrum is not valid !");
    qface_.reset();
    tface_.reset();
    if (has_partial_angle()) {
      double rob = _outer_bottom_radius_;
      double rot = _outer_top_radius_;
      double rib = 0.0;
      if (has_inner_bottom_radius()) {
        rib = _inner_bottom_radius_;
      }
      double rit = 0.0;
      if (has_inner_top_radius()) {
        rit = _inner_top_radius_;
      }
      if (start_stop_angle_face_is_quadrangle()) {
        vector_3d q0(rib, 0.0, -0.5 * _z_);
        vector_3d q1(rob, 0.0, -0.5 * _z_);
        vector_3d q2(rot, 0.0, +0.5 * _z_);
        vector_3d q3(rit, 0.0, +0.5 * _z_);
        qface_.set_vertexes(q0, q1, q2, q3);
      } else if (start_stop_angle_face_is_triangle_bottom()) {
        vector_3d q0(rib, 0.0, -0.5 * _z_);
        vector_3d q1(rob, 0.0, -0.5 * _z_);
        vector_3d q2(rot, 0.0, +0.5 * _z_);
        tface_.set_vertexes(q0, q1, q2);
      } else if (start_stop_angle_face_is_triangle_top()) {
        vector_3d q0(rit, 0.0, -0.5 * _z_);
        vector_3d q1(rot, 0.0, -0.5 * _z_);
        vector_3d q2(rob, 0.0, +0.5 * _z_);
        tface_.set_vertexes(q0, q1, q2);
      }
    }
    return;
  }

  void right_circular_conical_frustrum::compute_start_angle_face(quadrangle & qface_,
                                                                 triangle & tface_,
                                                                 placement & face_placement_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Frustrum is not valid !");
    compute_start_stop_angle_face(qface_, tface_);
    if (has_partial_angle()) {
      face_placement_.set(0.0, 0.0, 0.0, AXIS_Z, get_start_angle());
    }
    return;
  }

  void right_circular_conical_frustrum::compute_stop_angle_face(quadrangle & qface_,
                                                                triangle & tface_,
                                                                placement & face_placement_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Frustrum is not valid !");
    compute_start_stop_angle_face(qface_, tface_);
    if (has_partial_angle()) {
      face_placement_.set(0.0, 0.0, 0.0, AXIS_Z, get_start_angle() + get_delta_angle());
    }
    return;
  }

  // virtual
  unsigned int right_circular_conical_frustrum::compute_faces(face_info_collection_type & faces_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Conical frustrum is not valid !");
    unsigned int nfaces = 0;

    {
      // Outer side face:
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      right_circular_conical_nappe & rccn = finfo.add_face<right_circular_conical_nappe>();
      compute_outer_face(rccn);
      finfo.grab_positioning().set_identity();
      face_identifier fid;
      fid.set_face_bit(FACE_OUTER_SIDE);
      finfo.set_identifier(fid);
      nfaces++;
    }

    if (has_inner_face()) {
      // Inner side face:
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      right_circular_conical_nappe & rccn = finfo.add_face<right_circular_conical_nappe>();
      compute_inner_face(rccn);
      finfo.grab_positioning().set_identity();
      face_identifier fid;
      fid.set_face_bit(FACE_INNER_SIDE);
      finfo.set_identifier(fid);
      nfaces++;
    }

    if (has_bottom_face()) {
      // Bottom face:
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      disk & d = finfo.add_face<disk>();
      compute_bottom_face(d, finfo.grab_positioning());
      face_identifier fid;
      fid.set_face_bit(FACE_BOTTOM);
      finfo.set_identifier(fid);
      nfaces++;
    }

    if (has_top_face()) {
      // Top face:
      {
        face_info dummy;
        faces_.push_back(dummy);
      }
      face_info & finfo = faces_.back();
      disk & d = finfo.add_face<disk>();
      compute_top_face(d, finfo.grab_positioning());
      face_identifier fid;
      fid.set_face_bit(FACE_TOP);
      finfo.set_identifier(fid);
      nfaces++;
    }

    if (has_partial_angle()) {

      {
        // Start angle face:
        {
          face_info dummy;
          faces_.push_back(dummy);
        }
        face_info & finfo = faces_.back();
        if (start_stop_angle_face_is_quadrangle()) {
          quadrangle & q = finfo.add_face<quadrangle>();
          triangle dummy;
          compute_start_angle_face(q, dummy, finfo.grab_positioning());
        } else if (start_stop_angle_face_is_triangle()) {
          quadrangle dummy;
          triangle & t = finfo.add_face<triangle>();
          compute_start_angle_face(dummy, t, finfo.grab_positioning());
        }
        face_identifier fid;
        fid.set_face_bit(FACE_START_ANGLE);
        finfo.set_identifier(fid);
        nfaces++;
      }

      {
        // Stop angle face:
        {
          face_info dummy;
          faces_.push_back(dummy);
        }
        face_info & finfo = faces_.back();
        if (start_stop_angle_face_is_quadrangle()) {
          quadrangle & q = finfo.add_face<quadrangle>();
          triangle dummy;
          compute_stop_angle_face(q, dummy, finfo.grab_positioning());
        } else if (start_stop_angle_face_is_triangle()) {
          quadrangle dummy;
          triangle & t = finfo.add_face<triangle>();
          compute_stop_angle_face(dummy, t, finfo.grab_positioning());
        }
        face_identifier fid;
        fid.set_face_bit(FACE_STOP_ANGLE);
        finfo.set_identifier(fid);
        nfaces++;
      }

    }
    return nfaces;
  }

  double right_circular_conical_frustrum::get_surface(uint32_t flags_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Frustrum is not valid !");
    double s;
    datatools::invalidate(s);

    s = 0.0;
    if (has_inner_face()){
      // std::cerr << "DEVEL: Test 0 BEGIN" << std::endl;
      if (flags_ & FACE_INNER_SIDE) {
        // std::cerr << "DEVEL: Test 0.0 BEGIN" << std::endl;
        right_circular_conical_nappe inner_face;
        compute_inner_face(inner_face);
        double s_in = inner_face.get_surface();
        s += s_in;
        // std::cerr << "DEVEL: Test 0.0 END" << std::endl;
      }
      // std::cerr << "DEVEL: Test 0 END" << std::endl;
    }

    if (flags_ & FACE_OUTER_SIDE) {
      // std::cerr << "DEVEL: Test 1 BEGIN" << std::endl;
      right_circular_conical_nappe outer_face;
      compute_outer_face(outer_face);
      double s_out = outer_face.get_surface();
      s += s_out;
      // std::cerr << "DEVEL: Test 1 END" << std::endl;
    }

    if (has_bottom_face()){
      if (flags_ & FACE_BOTTOM) {
        disk bottom_face;
        placement dummy;
        compute_bottom_face(bottom_face, dummy);
        s += bottom_face.get_surface();
      }
    }

    if (has_top_face()){
      if (flags_ & FACE_TOP) {
        disk top_face;
        placement dummy;
        compute_top_face(top_face, dummy);
        s += top_face.get_surface();
      }
    }

    if (has_partial_angle()) {
      quadrangle angle_qface;
      triangle   angle_tface;
      compute_start_stop_angle_face(angle_qface, angle_tface);
      if (flags_ & FACE_START_ANGLE) {
        if (angle_qface.is_valid()) {
          s += angle_qface.get_surface();
        } else if (angle_tface.is_valid()) {
          s += angle_tface.get_surface();
        }
      }
      if (flags_ & FACE_STOP_ANGLE) {
        if (angle_qface.is_valid()) {
          s += angle_qface.get_surface();
        } else if (angle_tface.is_valid()) {
          s += angle_tface.get_surface();
        }
      }
    }

    return s;
  }

  double right_circular_conical_frustrum::get_volume (uint32_t flags_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid conical frustrum!");
    double v;
    datatools::invalidate(v);
    if (! is_valid()) {
      return v;
    }
    double ibr = 0.0;
    double itr = 0.0;
    if (has_inner_bottom_radius()) {
      ibr = _inner_bottom_radius_;
    }
    if (has_inner_top_radius()) {
      itr = _inner_top_radius_;
    }
    double vin = cone::compute_frustrum_volume(-0.5 * _z_,
                                               +0.5 * _z_,
                                               ibr,
                                               itr);
    double vout = cone::compute_frustrum_volume(-0.5 * _z_,
                                                +0.5 * _z_,
                                                _outer_bottom_radius_,
                                                _outer_top_radius_);
    v = 0.0;
    if (flags_ & VOLUME_BULK) {
      v += (vout - vin);
    }
    if (has_inner_face()) {
      if (flags_ & VOLUME_CAVITY) {
        v += vin;
      }
    }
    if (has_partial_angle()) {
      v *= (get_delta_angle() / 2 * M_PI);
    }
    return v;
  }

  void right_circular_conical_frustrum::initialize(const datatools::properties & config_,
                                                   const handle_dict_type * objects_)
  {
    this->i_shape_3d::_initialize(config_, objects_);

    if (!is_valid()) {
      double lunit = CLHEP::mm;
      if (config_.has_key("length_unit")) {
        const std::string lunit_str = config_.fetch_string("length_unit");
        lunit = datatools::units::get_length_unit_from(lunit_str);
      }

      double inner_top_r;
      datatools::invalidate (inner_top_r);
      if (config_.has_key ("inner_top_r")) {
        inner_top_r = config_.fetch_real ("inner_top_r");
        if (! config_.has_explicit_unit ("inner_top_r")) {
          inner_top_r *= lunit;
        }
      }

      DT_THROW_IF (! config_.has_key("outer_top_r"), std::logic_error,
                   "Missing tube 'outer_top_r' property !");
      double outer_top_r;
      datatools::invalidate (outer_top_r);
      if (config_.has_key ("outer_top_r")) {
        outer_top_r = config_.fetch_real ("outer_top_r");
        if (! config_.has_explicit_unit ("outer_top_r")) {
          outer_top_r *= lunit;
        }
      }

      double inner_bottom_r;
      datatools::invalidate (inner_bottom_r);
      if (config_.has_key ("inner_bottom_r")) {
        inner_bottom_r = config_.fetch_real ("inner_bottom_r");
        if (! config_.has_explicit_unit ("inner_bottom_r")) {
          inner_bottom_r *= lunit;
        }
      }

      double outer_bottom_r;
      datatools::invalidate (outer_bottom_r);
      DT_THROW_IF (! config_.has_key("outer_bottom_r"), std::logic_error,
                   "Missing tube 'outer_bottom_r' property !");
      if (config_.has_key ("outer_bottom_r")) {
        outer_bottom_r = config_.fetch_real ("outer_bottom_r");
        if (! config_.has_explicit_unit ("outer_bottom_r")) {
          outer_bottom_r *= lunit;
        }
      }

      DT_THROW_IF (! config_.has_key("z"), std::logic_error,
                   "Missing tube 'z' property !");
      double z = config_.fetch_real("z");
      if (! config_.has_explicit_unit("z")) {
        z *= lunit;
      }

      set_inner_bottom_radius(inner_bottom_r);
      set_inner_top_radius(inner_top_r);
      set_outer_bottom_radius(outer_bottom_r);
      set_outer_top_radius(outer_top_r);
      set_z(z);

      datatools::properties angle_config;
      config_.export_and_rename_starting_with(angle_config, "angle.", "");
      if (angle_config.size()) {
        _angle_domain_.initialize(angle_config);
      } else {
        // deprecated ;
        double aunit = CLHEP::degree;
        if (config_.has_key("angle_unit")) {
          const std::string aunit_str = config_.fetch_string("angle_unit");
          aunit = datatools::units::get_angle_unit_from(aunit_str);
        }
        double start_angle = 0.0;
        double delta_angle = 2 * M_PI * CLHEP::radian;
        bool not_full_angle = false;
        if (config_.has_key ("start_angle")) {
          start_angle = config_.fetch_real ("start_angle");
          if (! config_.has_explicit_unit ("start_angle")) {
            start_angle *= aunit;
          }
          not_full_angle = true;
        }
        if (config_.has_key ("delta_angle")) {
          delta_angle = config_.fetch_real ("delta_angle");
          if (! config_.has_explicit_unit ("delta_angle")) {
            delta_angle *= aunit;
          }
          not_full_angle = true;
        }
        if (not_full_angle) {
          set_start_angle(start_angle);
          set_delta_angle(delta_angle);
        }
      }
    }

    lock();
    return;
  }

  bool right_circular_conical_frustrum::is_inside( const vector_3d & point_, double skin_) const
  {
    DT_THROW_IF(!is_valid(), std::logic_error, "Invalid shape!");
    double skin = compute_tolerance(skin_);
    double hskin = 0.5 * skin;
    if (std::abs(point_.z()) > 0.5 * _z_ - hskin) {
      return false;
    }
    right_circular_conical_nappe in_rccn_face;
    if (has_inner_face()) {
      compute_inner_face(in_rccn_face);
    }
    right_circular_conical_nappe out_rccn_face;
    compute_outer_face(out_rccn_face);
    double rz_out = out_rccn_face.get_radius_from_z(point_.z());
    double rz = hypot(point_.x(), point_.y());
    {
      double out_face_theta = out_rccn_face.get_cone_angle();
      double half_radial_tolerance = hskin / std::abs(std::cos(out_face_theta));
      if (rz > rz_out - half_radial_tolerance) {
        return false;
      }
    }
    if (has_inner_face()) {
      double rz_in = in_rccn_face.get_radius_from_z(point_.z());
      double in_face_theta = in_rccn_face.get_cone_angle();
      double half_radial_tolerance = hskin / std::abs(std::cos(in_face_theta));
      if (rz < rz_in + half_radial_tolerance) {
        return false;
      }
    }
    if (has_partial_angle()) {
      double angular_tolerance = get_angular_tolerance();
      double angle = std::atan2(point_.y(), point_.x());
      if (!angle_is_in(angle, get_start_angle(), get_delta_angle(), angular_tolerance, true)) {
        return false;
      }
    }
    return true;
  }

  bool right_circular_conical_frustrum::is_outside(const vector_3d & point_, double skin_) const
  {
    DT_THROW_IF(!is_valid(), std::logic_error, "Invalid shape!");
    double skin = compute_tolerance(skin_);
    double hskin = 0.5 * skin;
    if (std::abs(point_.z()) > 0.5 * _z_ + hskin) {
      return true;
    }
    right_circular_conical_nappe in_rccn_face;
    if (has_inner_face()) {
      compute_inner_face(in_rccn_face);
    }
    right_circular_conical_nappe out_rccn_face;
    compute_outer_face(out_rccn_face);
    double rz_out = out_rccn_face.get_radius_from_z(point_.z());
    double rz = hypot(point_.x(), point_.y());
    {
      double out_face_theta = out_rccn_face.get_cone_angle();
      double half_radial_tolerance = hskin / std::abs(std::cos(out_face_theta));
      if (rz > rz_out + half_radial_tolerance) {
        return true;
      }
    }
    if (has_inner_face()) {
      double rz_in = in_rccn_face.get_radius_from_z(point_.z());
      double in_face_theta = in_rccn_face.get_cone_angle();
      double half_radial_tolerance = hskin / std::abs(std::cos(in_face_theta));
      if (rz < rz_in - half_radial_tolerance) {
        return true;
      }
    }
    if (has_partial_angle()) {
      double angular_tolerance = get_angular_tolerance();
      double angle = std::atan2(point_.y(), point_.x());
      if (!angle_is_in(angle, get_start_angle(), get_delta_angle(), angular_tolerance, true)) {
        return false;
      }
    }
    return false;
  }

  face_identifier right_circular_conical_frustrum::on_surface(const vector_3d & position_,
                                                              const face_identifier & surface_mask_,
                                                              double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid conical frustrum!");
    double skin = compute_tolerance(skin_);

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

  vector_3d
  right_circular_conical_frustrum::get_normal_on_surface (const vector_3d & position_,
                                                          const face_identifier & a_surface_bit) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid conical frustrum!");
    vector_3d normal;
    geomtools::invalidate(normal);
    switch(a_surface_bit.get_face_bits()) {
    case FACE_OUTER_SIDE:
      {
        right_circular_conical_nappe face;
        compute_outer_face(face);
        normal = face.get_normal_on_surface(position_, false);
      }
      break;
    case FACE_INNER_SIDE:
      if (has_inner_face()) {
        right_circular_conical_nappe face;
        compute_inner_face(face);
        normal = -face.get_normal_on_surface(position_, false);
      }
      break;
    case FACE_BOTTOM:
      if (has_bottom_face()) {
        disk face;
        placement face_pl;
        compute_bottom_face(face, face_pl);
        vector_3d position_c;
        face_pl.mother_to_child(position_c);
        vector_3d normal_c = -face.get_normal_on_surface(position_c, false);
        normal = face_pl.child_to_mother_direction(normal_c);
      }
      break;
    case FACE_TOP:
      if (has_top_face()) {
        disk face;
        placement face_pl;
        compute_top_face(face, face_pl);
        vector_3d position_c;
        face_pl.mother_to_child(position_c);
        vector_3d normal_c = face.get_normal_on_surface(position_c, false);
        normal = face_pl.child_to_mother_direction(normal_c);
      }
      break;
    case FACE_START_ANGLE:
      if (has_partial_angle())  {
        quadrangle q;
        triangle t;
        placement face_pl;
        compute_start_angle_face(q, t, face_pl);
        vector_3d position_c;
        face_pl.mother_to_child(position_c);
        if (q.is_valid()) {
          vector_3d normal_c = -q.get_normal_on_surface(position_c, false);
          normal = face_pl.child_to_mother_direction(normal_c);
        } else if (t.is_valid()) {
          vector_3d normal_c = -t.get_normal_on_surface(position_c, false);
          normal = face_pl.child_to_mother_direction(normal_c);
        }
      }
      break;
    case FACE_STOP_ANGLE:
      if (has_partial_angle())  {
        quadrangle q;
        triangle t;
        placement face_pl;
        compute_stop_angle_face(q, t, face_pl);
        vector_3d position_c;
        face_pl.mother_to_child(position_c);
        if (q.is_valid()) {
          vector_3d normal_c = -q.get_normal_on_surface(position_c, false);
          normal = face_pl.child_to_mother_direction(normal_c);
        } else if (t.is_valid()) {
          vector_3d normal_c = -t.get_normal_on_surface(position_c, false);
          normal = face_pl.child_to_mother_direction(normal_c);
        }
      }
      break;
    }
    return normal;
  }

  bool right_circular_conical_frustrum::find_intercept (const vector_3d & from_,
                                                        const vector_3d & direction_,
                                                        face_intercept_info & intercept_,
                                                        double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid conical frustrum!");
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


  void right_circular_conical_frustrum::generate_wires_self(wires_type & wires_,
                                                            uint32_t options_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid conical frustrum!");
    // bool debug_explode    = options_ & WR_BASE_EXPLODE;
    bool draw_bottom      = !(options_ & WR_RCCF_NO_BOTTOM_FACE);
    bool draw_top         = !(options_ & WR_RCCF_NO_TOP_FACE);
    bool draw_inner       = !(options_ & WR_RCCF_NO_INNER_FACE);
    bool draw_outer       = !(options_ & WR_RCCF_NO_OUTER_FACE);
    bool draw_start_angle = !(options_ & WR_RCCF_NO_START_ANGLE_FACE);
    bool draw_stop_angle  = !(options_ & WR_RCCF_NO_STOP_ANGLE_FACE);
    bool draw_boundings   =  (options_ & WR_BASE_BOUNDINGS);

    if (draw_boundings && has_bounding_data()) {
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

    if (draw_bottom && has_bottom_face()) {
      disk bottom_face;
      placement bottom_pl;
      compute_bottom_face(bottom_face, bottom_pl);
      // placement bottom_pl(0.0, 0.0, -0.5 * get_z() * explode_factor);
      uint32_t options = base_options;
      bottom_face.generate_wires(wires_, bottom_pl, options);
      edge_bottom = true;
    }

    if (draw_top && has_top_face()) {
      disk top_face;
      placement top_pl;
      compute_top_face(top_face, top_pl);
      uint32_t options = base_options;
      top_face.generate_wires(wires_, top_pl, options);
      edge_bottom = true;
    }

    if (has_inner_face()) {
      if (draw_inner) {
        uint32_t options = base_options;
        if (edge_bottom) {
          options |= right_circular_conical_nappe::WR_RCCN_NO_BOTTOM_EDGE;
        }
        right_circular_conical_nappe in_rccn_face;
        compute_inner_face(in_rccn_face);
        in_rccn_face.generate_wires_self(wires_, options);
      }
    }

    if (draw_outer) {
      uint32_t options = base_options;
      if (edge_top) {
        options |= right_circular_conical_nappe::WR_RCCN_NO_TOP_EDGE;
      }
      right_circular_conical_nappe out_rccn_face;
      compute_outer_face(out_rccn_face);
      out_rccn_face.generate_wires_self(wires_, options);
    }

    if (has_partial_angle()) {
      uint32_t options = base_options;
      if (draw_start_angle) {
        quadrangle angle_qface;
        triangle angle_tface;
        placement p;
        compute_start_angle_face(angle_qface, angle_tface, p);
        if (angle_qface.is_valid()) angle_qface.generate_wires(wires_, p, options);
        else if (angle_tface.is_valid()) angle_tface.generate_wires(wires_, p, options);
      }
      if (draw_stop_angle) {
        quadrangle angle_qface;
        triangle angle_tface;
        placement p;
        compute_stop_angle_face(angle_qface, angle_tface, p);
        if (angle_qface.is_valid()) angle_qface.generate_wires(wires_, p, options);
        else if (angle_tface.is_valid()) angle_tface.generate_wires(wires_, p, options);
      }
    }

    return;
  }

  void right_circular_conical_frustrum::_build_bounding_data()
  {
    if (! has_partial_angle()) {
      _grab_bounding_data().make_cylinder(get_xmax(),
                                          -0.5 * get_z(),
                                          +0.5 * get_z());
    } else {
      mygsl::min_max xrange;
      mygsl::min_max yrange;
      double dphi = std::min(0.5 * CLHEP::degree, _angle_domain_.get_angle_spread() / 100);
      if (!has_inner_face()) {
        xrange.add(0.0);
        yrange.add(0.0);
      }
      double ro = get_outer_top_radius();
      if (get_outer_bottom_radius() > get_outer_top_radius()) {
        ro = get_outer_bottom_radius();
      }
      for (double phi = _angle_domain_.get_first_angle();
           phi < _angle_domain_.get_last_angle();
           phi += dphi) {
        double cp = std::cos(phi);
        double sp = std::sin(phi);
        double xo = ro * cp;
        if (xo > ro * 0.99) {
          xo = ro;
        }
        if (xo < - ro * 0.99) {
          xo = -ro;
        }
        double yo = ro * sp;
        if (yo > ro * 0.99) {
          yo = ro;
        }
        if (yo < - ro * 0.99) {
          yo = -ro;
        }
        xrange.add(xo);
        yrange.add(yo);
        if (has_inner_face()) {
          double ri = get_inner_top_radius();
          if (get_inner_bottom_radius() < get_inner_top_radius()) {
            ri = get_inner_bottom_radius();
          }
          double xi = ri * cp;
          double yi = ri * sp;
          xrange.add(xi);
          yrange.add(yi);
        }
      }
      _grab_bounding_data().make_box(xrange.get_min(), xrange.get_max(),
                                     yrange.get_min(), yrange.get_max(),
                                     get_zmin(), get_zmax());
    }
    return;
  }
  
  void right_circular_conical_frustrum::compute_deflated(right_circular_conical_frustrum & deflated_,
                                                         double by_r_,
                                                         double by_z_,
                                                         double by_angle_)
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid right circular conical frustrum!");
    double r_eps = 0.0;
    double z_eps = 0.0;
    if (datatools::is_valid(by_r_) and by_r_ > 0.0) r_eps = by_r_;
    if (datatools::is_valid(by_z_) and by_z_ > 0.0) z_eps = by_z_;
    double z = get_z();
    double rib = get_inner_bottom_radius();
    double rit = get_inner_top_radius();
    double rob = get_outer_bottom_radius();
    double rot = get_outer_top_radius();
    z -= (2 * z_eps);
    rib += r_eps;
    rit += r_eps;
    rob -= r_eps;
    rot -= r_eps;
    deflated_.reset();
    if (has_inner_bottom_radius()) {
      deflated_.set_inner_bottom_radius(rib);
    }
    if (has_inner_top_radius()) {
      deflated_.set_inner_top_radius(rit);
    }
    deflated_.set_outer_bottom_radius(rob);
    deflated_.set_outer_top_radius(rot);
    deflated_.set_z(z);
    double eps_angle = by_angle_;
    if (eps_angle < 0.0) {
      double delta_angle_bot = r_eps / rob;
      double delta_angle_top = r_eps / rot;
      eps_angle = std::max(delta_angle_bot, delta_angle_top);
    }
    if (has_partial_angle()) {
      double start_angle = get_start_angle();
      double delta_angle = get_delta_angle();
      if (eps_angle > 0.0) {
        start_angle -= eps_angle;
        delta_angle += 2 * eps_angle;
      }
      if (delta_angle < 0.0) {
          start_angle = get_start_angle() + 0.5 * get_delta_angle() - 0.25 * eps_angle;
          delta_angle = 0.5 * eps_angle;
      }
      deflated_.set_start_angle(start_angle);
      deflated_.set_delta_angle(delta_angle);
    }
    deflated_.lock();
    return;
  }

  void right_circular_conical_frustrum::compute_inflated(right_circular_conical_frustrum & inflated_,
                                                         double by_r_,
                                                         double by_z_,
                                                         double by_angle_)
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid right circular conical frustrum!");
    double r_eps = 0.0;
    double z_eps = 0.0;
    if (datatools::is_valid(by_r_) and by_r_ > 0.0) r_eps = by_r_;
    if (datatools::is_valid(by_z_) and by_z_ > 0.0) z_eps = by_z_;
    double z = get_z();
    double rib = get_inner_bottom_radius();
    double rit = get_inner_top_radius();
    double rob = get_outer_bottom_radius();
    double rot = get_outer_top_radius();
    z += (2 * z_eps);
    rib -= r_eps;
    rit -= r_eps;
    rob += r_eps;
    rot += r_eps;
    inflated_.reset();
    if (has_inner_bottom_radius()) {
      inflated_.set_inner_bottom_radius(rib);
    }
    if (has_inner_top_radius()) {
      inflated_.set_inner_top_radius(rit);
    }
    inflated_.set_outer_bottom_radius(rob);
    inflated_.set_outer_top_radius(rot);
    inflated_.set_z(z);
    double eps_angle = by_angle_;
    if (eps_angle < 0.0) {
      double delta_angle_bot = r_eps / rob;
      double delta_angle_top = r_eps / rot;
      eps_angle = std::max(delta_angle_bot, delta_angle_top);
    }
    if (has_partial_angle()) {
      double start_angle = get_start_angle();
      double delta_angle = get_delta_angle();
      if (eps_angle > 0.0) {
        start_angle -= eps_angle;
        delta_angle += 2 * eps_angle;
      }
      if (delta_angle < 2 * M_PI) {
        inflated_.set_start_angle(start_angle);
        inflated_.set_delta_angle(delta_angle);
      }
    }
    inflated_.lock();
    return;
  }

  void right_circular_conical_frustrum::compute_envelope(right_circular_conical_frustrum & envelope_,
                                                         double r_tolerance_,
                                                         double z_tolerance_,
                                                         double angle_tolerance_)
  {
    compute_inflated(envelope_, r_tolerance_, z_tolerance_, angle_tolerance_);
    /*
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid right circular conical frustrum!");
    double r_eps = 0.0;
    double z_eps = 0.0;
    if (datatools::is_valid(by_r_) and by_r_ > 0.0) r_eps = by_r_;
    if (datatools::is_valid(by_z_) and by_z_ > 0.0) z_eps = by_z_;
    double z = get_z();
    double rob = get_outer_bottom_radius();
    double rot = get_outer_top_radius();
    z += (2 * z_eps);
    rib -= r_eps;
    rit -= r_eps;
    rob += r_eps;
    rot += r_eps;
    envelope_.reset();
    envelope_.set_outer_bottom_radius(rob);
    envelope_.set_outer_top_radius(rot);
    envelope_.set_z(z);
    double eps_angle = angle_tolerance_;
    if (eps_angle_ < 0.0) {
      double dangleb = r_eps / rob;
      double danglet = r_eps / rot;
      doubel eps_angle = std::max(dangleb, danglet);
      eps_angle = r_eps / r;
    }
    if (eps_angle > 0.0 and has_partial_angle()) {
      double start_angle = get_start_angle();
      double delta_angle = get_delta_angle();
      start_angle -= eps_angle;
      delta_angle += 2 * eps_angle;
      if (delta_angle < 2 * M_PI) {
        envelope_.set_start_angle(start_angle);
        envelope_.set_delta_angle(delta_angle);
      }
    }
    envelope_.lock();
    */
    return;
  }

} // end of namespace geomtools
