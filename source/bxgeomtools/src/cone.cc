// cone.cc

// Ourselves:
#include <geomtools/cone.h>

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

// // This project:
// #include <geomtools/circle.h>
// #include <geomtools/disk.h>
// #include <geomtools/line_3d.h>
// #include <geomtools/quadrangle.h>
// #include <geomtools/triangle.h>

namespace geomtools {

  // static
  double cone::compute_frustrum_lateral_surface(double z1_, double z2_,
                                                double r1_, double r2_)
  {
    // See: http://en.wikipedia.org/wiki/Frustum#Surface_Area
    double s = 0.0;
    double z1 = z1_;
    double z2 = z2_;
    double r1 = r1_;
    double r2 = r2_;
    DT_THROW_IF(z1 == z2, std::logic_error,
                "Invalid cone Z dimensions !");
    double h = z2 - z1;
    if (r1 == r2) {
      return 2 * M_PI * r1 * h;
    }
    s = M_PI * (r1 + r2) * std::sqrt(gsl_pow_2(r1-r2)+h*h);
    if (s < 0.0) s *= -1.0;
    return s;
  }

  // static
  double cone::compute_frustrum_volume(double z1_, double z2_,
                                       double r1_, double r2_)
  {
    // See: http://en.wikipedia.org/wiki/Frustum#Volume
    double v = 0.0;
    double z1 = z1_;
    double z2 = z2_;
    double r1 = r1_;
    double r2 = r2_;
    DT_THROW_IF(z1 == z2, std::logic_error,
                "Invalid cone Z dimensions !");
    double h = std::abs(z2 - z1);
    if (r1 == r2) {
      return M_PI * r1 * r1 * h;
    }
    v = M_PI * h * (r1 * r1 + r1 * r2 + r2 * r2) / 3.0;
    return v;
  }

  /*

  // static
  const std::string & right_circular_conical_nappe::right_circular_conical_nappe_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "right_circular_conical_nappe";
    }
    return label;
  }

  std::string right_circular_conical_nappe::get_shape_name () const
  {
    return right_circular_conical_nappe::right_circular_conical_nappe_label();
  }


  void right_circular_conical_nappe::_set_defaults()
  {
    datatools::invalidate(_bottom_radius_);
    datatools::invalidate(_top_radius_);
    datatools::invalidate(_z_);
    datatools::invalidate(_start_angle_);
    datatools::invalidate(_delta_angle_);
    return;
  }

  double right_circular_conical_nappe::get_radius_from_z(double z_) const
  {
    DT_THROW_IF(!is_valid(), std::logic_error, "Conical nappe is not valid!");
    double r = \
      z_ * (_top_radius_ - _bottom_radius_) / _z_
      + 0.5 * (_top_radius_ + _bottom_radius_);
    return r;
  }

  double right_circular_conical_nappe::get_z () const
  {
    return _z_;
  }

  double right_circular_conical_nappe::get_bottom_radius () const
  {
    return _bottom_radius_;
  }

  double right_circular_conical_nappe::get_top_radius () const
  {
    return _top_radius_;
  }

  void right_circular_conical_nappe::set_bottom_radius(double new_value_)
  {
    DT_THROW_IF(new_value_ < 0.0, std::domain_error, "Invalid bottom radius value!");
    _bottom_radius_ = new_value_;
    return;
  }

  void right_circular_conical_nappe::set_top_radius(double new_value_)
  {
    DT_THROW_IF(new_value_ < 0.0, std::domain_error, "Invalid top radius value!");
    _top_radius_ = new_value_;
    return;
  }

  void right_circular_conical_nappe::set_z(double new_value_)
  {
    DT_THROW_IF(new_value_ <= 0.0, std::domain_error, "Invalid Z value!");
    _z_ = new_value_;
    return;
  }

  bool right_circular_conical_nappe::has_partial_angle() const
  {
    if (_delta_angle_ == 2 * M_PI) return false;
    if (_start_angle_ > 0.0) return true;
    return false;
  }

  bool right_circular_conical_nappe::has_start_angle() const
  {
    return datatools::is_valid(_start_angle_);
  }

  void right_circular_conical_nappe::set_start_angle(double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0 || new_value_ >= 2 * M_PI,
                 std::domain_error,
                 "Invalid '" << new_value_ << "' start angle value !");
    _start_angle_ = new_value_;
    return;
  }

  double right_circular_conical_nappe::get_start_angle() const
  {
    return _start_angle_;
  }

  bool right_circular_conical_nappe::has_delta_angle() const
  {
    return datatools::is_valid(_delta_angle_);
  }

  void right_circular_conical_nappe::set_delta_angle(double new_value_)
  {
    DT_THROW_IF (new_value_ <= 0.0 || new_value_ > 2 * M_PI,
                 std::domain_error,
                 "Invalid '" << new_value_ << "' delta angle value !");
    _delta_angle_ = new_value_;
    return;
  }

  double right_circular_conical_nappe::get_delta_angle() const
  {
    return _delta_angle_;
  }

  bool right_circular_conical_nappe::is_valid () const
  {
    if (!datatools::is_valid(_bottom_radius_)) {
      return false;
    }
    if (!datatools::is_valid(_top_radius_)) {
      return false;
    }
    if (!datatools::is_valid(_z_)) {
      return false;
    }
    if (has_start_angle()) {
      if (!datatools::is_valid(_delta_angle_)) {
        return false;
      }
    }
    return true;
  }

  right_circular_conical_nappe::right_circular_conical_nappe ()
  {
    _set_defaults();
    return;
  }

  right_circular_conical_nappe::right_circular_conical_nappe(double bottom_radius_,
                                                             double top_radius_,
                                                             double z_)
  {
    _set_defaults();
    set_bottom_radius(bottom_radius_);
    set_top_radius(top_radius_);
    set_z(z_);
    return;
  }

  right_circular_conical_nappe::right_circular_conical_nappe(double bottom_radius_,
                                                             double top_radius_,
                                                             double z_,
                                                             double start_angle_,
                                                             double delta_angle_
                                                             )
  {
    _set_defaults();
    set_bottom_radius(bottom_radius_);
    set_top_radius(top_radius_);
    set_z(z_);
    set_start_angle(start_angle_);
    set_delta_angle(delta_angle_);
    return;
  }

  right_circular_conical_nappe::~right_circular_conical_nappe ()
  {
    return;
  }

  double right_circular_conical_nappe::get_surface(uint32_t flags_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid circular conical nappe!");
    double s;
    datatools::invalidate(s);
    s = cone::compute_frustrum_lateral_surface(-0.5 * _z_, +0.5 * _z_,
                                               _bottom_radius_, _top_radius_);
    if (has_start_angle()) {
      s *= (_delta_angle_ / (2 * M_PI));
    }
    return s;
  }

  bool right_circular_conical_nappe::is_on_surface (const vector_3d & position_,
                                                    double tolerance_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid circular conical nappe!");
    double normal_tolerance = compute_tolerance(tolerance_);
    double half_normal_tolerance = 0.5 * normal_tolerance;
    double angular_tolerance = get_angular_tolerance();
    double r = hypot(position_.x(), position_.y());
    double z = position_.z();
    if (std::abs(z) > 0.5 * _z_) {
      return false;
    }
    if (has_partial_angle()) {
      double angle = std::atan2(position_.y(), position_.x());
      if (!angle_is_in(angle, _start_angle_, _delta_angle_, angular_tolerance, true)) {
        return false;
      }
    }
    double rz = get_radius_from_z(position_.z());
    double theta = get_cone_angle();
    double half_radial_tolerance = half_normal_tolerance / std::abs(std::cos(theta));
    if (r > rz + half_radial_tolerance) {
      return false;
    }
    if (r < rz - half_radial_tolerance) {
      return false;
    }
    return true;
  }

  double right_circular_conical_nappe::get_cone_angle() const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid circular conical nappe!");
    double angle = std::atan2(_top_radius_ - _bottom_radius_, _z_);
    return angle;
  }

  vector_3d right_circular_conical_nappe::get_normal_on_surface(const vector_3d & position_,
                                                                bool check_,
                                                                double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid circular conical nappe!");
    vector_3d normal;
    geomtools::invalidate(normal);
    DT_THROW_IF(check_ && !is_on_surface(position_, skin_), std::logic_error,
                "Position " << position_ << " is not on surface!");
    double theta = 0.5 * M_PI + std::atan2(_top_radius_ - _bottom_radius_, _z_);
    create_spherical(normal, 1.0, position_.phi(), theta);
    return normal;
  }

  bool right_circular_conical_nappe::find_intercept (const vector_3d & from_,
                                                     const vector_3d & direction_,
                                                     intercept_data_type & intercept_,
                                                     double tolerance_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid circular conical nappe!");
    bool devel = false;
    // devel = true;
    intercept_.reset();
    double tolerance = compute_tolerance(tolerance_);
    double a, b, c;
    double p0 = from_.x();
    double p1 = from_.y();
    double p2 = from_.z();
    double d0 = direction_.x();
    double d1 = direction_.y();
    double d2 = direction_.z();
    double alpha = (_top_radius_ - _bottom_radius_) / _z_;
    double beta = 0.5 * (_top_radius_ + _bottom_radius_);
    a = gsl_pow_2(d0) + gsl_pow_2(d1) - gsl_pow_2(alpha * d2);
    b = 2. * (d0 * p0 + d1 * p1 - alpha * d2 * (alpha * p2 + beta));
    c = gsl_pow_2(p0) + gsl_pow_2(p1) - gsl_pow_2(alpha * p2 + beta);
    if (devel) std::cerr << "DEVEL:   a = " << a << std::endl;
    if (devel) std::cerr << "DEVEL:   b = " << b << std::endl;
    if (devel) std::cerr << "DEVEL:   c = " << c << std::endl;
    unsigned int nsols = 0;
    double t0, t1;
    vector_3d impact0;
    geomtools::invalidate(impact0);
    vector_3d impact1;
    geomtools::invalidate(impact1);
    if (mygsl::polynomial::solve_quadratic(c, b, a, nsols, t0, t1)) {
      if (devel) std::cerr << "DEVEL: nsols=" << nsols << std::endl;
      if (nsols > 0) {
        if (devel) std::cerr << "DEVEL:   t0=" << t0 << std::endl;
        if (t0 >= 0) {
          double z0 = p2 + t0 * d2;
          // if (std::abs(z0) < (0.5 * _z_ + tolerance)) {
          impact0.set(p0 + t0 * d0, p1 + t0 * d1, z0);
          if (devel) std::cerr << "DEVEL:   impact0=" << impact0 << std::endl;
          if (! is_on_surface(impact0, tolerance)) {
            if (devel) std::cerr << "DEVEL:   impact0 is not on surface" << std::endl;
            geomtools::invalidate(impact0);
          } else {
            if (devel) std::cerr << "DEVEL:  use impact0 =" << impact0 << std::endl;
            intercept_.set_shape_index(0);
            intercept_.set_face(FACE_ALL);
            intercept_.set_impact(impact0);
          }
          // }
        } // if (t0 >= 0)
      } // if (nsols > 0)
      if (nsols > 1) {
        if (devel) std::cerr << "DEVEL:   t1=" << t1 << std::endl;
        if (t1 >= 0) {
          double z1 = p2 + t1 * d2;
          // if (std::abs(z1) < (0.5 * _z_ + tolerance)) {
          impact1.set(p0 + t1 * d0, p1 + t1 * d1, z1);
          if (devel) std::cerr << "DEVEL:   impact1=" << impact1 << std::endl;
          if (! is_on_surface(impact1, tolerance)) {
            if (devel) std::cerr << "DEVEL:   impact1 is not on surface" << std::endl;
            geomtools::invalidate(impact1);
          } else {
            if (devel) std::cerr << "DEVEL:   impact1 is on surface" << std::endl;
            if (intercept_.is_valid()) {
              if (devel) std::cerr << "DEVEL:   compare impact1 and impact0" << std::endl;
              double l0 = (intercept_.get_impact() - from_).mag();
              if (devel) std::cerr << "DEVEL:   l0=" << l0 << std::endl;
              double l1 = (impact1 - from_).mag();
              if (devel) std::cerr << "DEVEL:   l1=" << l1 << std::endl;
              if (l1 < l0) {
                if (devel) std::cerr << "DEVEL:   l1 < l0: use impact1 = " << impact1 << std::endl;
                intercept_.set_impact(impact1);
              } else {
                if (devel) std::cerr << "DEVEL:   l1 >= l0: use impact0 = " << impact0 << std::endl;
              }
            } else {
              if (devel) std::cerr << "DEVEL:   use impact1 = " << impact1 << std::endl;
              intercept_.set_shape_index(0);
              intercept_.set_face(FACE_ALL);
              intercept_.set_impact(impact1);
            }
            // }
          }
        } // if (t1 >= 0)
      } // if (nsols > 1)
    } // solve_quadratic
    if (devel) intercept_.print(std::cerr, "DEVEL: right_circular_conical_nappe::find_intercept: ");
    if (devel) std::cerr << std::endl;
    return intercept_.is_ok();
  }

  void right_circular_conical_nappe::tree_dump (std::ostream & out_,
                                                const std::string & title_,
                                                const std::string & indent_,
                                                bool inherit_) const
  {
    i_object_3d::tree_dump (out_, title_, indent_, true);

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Bottom radius : ";
    if (datatools::is_valid(_bottom_radius_)) {
      out_ << _bottom_radius_ / CLHEP::mm << " mm";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Top radius : ";
    if (datatools::is_valid(_top_radius_)) {
      out_ << _top_radius_ / CLHEP::mm << " mm";
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

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Start angle : ";
    if (has_start_angle()) {
      out_ << _start_angle_ / CLHEP::degree << " degree";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Delta angle : ";
    if (has_delta_angle()) {
      out_ << _delta_angle_ / CLHEP::degree << " degree";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    return;
  }

  void right_circular_conical_nappe::generate_wires_self(wires_type & wires_,
                                                         uint32_t options_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid circular conical nappe!");
    bool no_bottom_edge      = (options_ & WR_RCCN_NO_BOTTOM_EDGE);
    bool no_top_edge         = (options_ & WR_RCCN_NO_TOP_EDGE);
    bool no_start_angle_edge = (options_ & WR_RCCN_NO_START_ANGLE_EDGE);
    bool no_stop_angle_edge  = (options_ & WR_RCCN_NO_STOP_ANGLE_EDGE);

    // Keep only base rendering bits:
    uint32_t base_options = options_ & WR_BASE_MASK;

    uint32_t nsamples_z = linear_sampling_from_options(base_options);
    // std::cerr << "DEVEL: right_circular_conical_nappe::generate_wires_self: "
    //           << "nsamples_z = " << nsamples_z
    //           << std::endl;

    double rb = get_bottom_radius();
    // std::cerr << "DEVEL: right_circular_conical_nappe::generate_wires_self: "
    //           << "rb = " << rb
    //           << std::endl;
    double rt = get_top_radius();
    // std::cerr << "DEVEL: right_circular_conical_nappe::generate_wires_self: "
    //           << "rt = " << rt
    //           << std::endl;
    double z  = get_z();
    // std::cerr << "DEVEL: right_circular_conical_nappe::generate_wires_self: "
    //           << "z = " << z
    //           << std::endl;
    double dz   = z / (nsamples_z - 1);
    // std::cerr << "DEVEL: right_circular_conical_nappe::generate_wires_self: "
    //           << "dz = " << dz
    //           << std::endl;

    if (nsamples_z > 0) {

      // Draw parallels:
      for (int iz = 0; iz < nsamples_z; iz++) {
        uint32_t options = base_options;
        if (iz == 0 && no_bottom_edge) continue;
        if (iz == (nsamples_z - 1) && no_top_edge) continue;
        double zz = -0.5 * z + iz * dz;
        double rz = zz * (rt - rb) / z + 0.5 * (rt + rb);
        // std::cerr << "DEVEL: right_circular_conical_nappe::generate_wires_self: "
        //           << "rz = " << rz
        //           << std::endl;
        circle arc(rz);
        if (has_partial_angle()) {
          arc.set_start_angle(_start_angle_);
          arc.set_delta_angle(_delta_angle_);
        }
        placement arc_placement(0.0, 0.0, zz);
        arc.generate_wires(wires_, arc_placement, options);
      }
    }

    uint32_t nsamples_phi = 0;
    if (has_partial_angle()) {
      nsamples_phi = 2;
    }
    nsamples_phi = angular_sampling_from_options(base_options);
    // std::cerr << "DEVEL: right_circular_conical_nappe::generate_wires_self: "
    //           << "nsamples_phi = " << nsamples_phi
    //           << std::endl;
    double start_phi = 0.0;
    double delta_phi = 2.0 * M_PI;
    if (has_partial_angle()) {
      start_phi = _start_angle_;
      delta_phi = _delta_angle_;
    }
    double dphi = delta_phi / nsamples_phi;
    // std::cerr << "DEVEL: right_circular_conical_nappe::generate_wires_self: "
    //           << "dphi = " << dphi
    //           << std::endl;

    // Draw meridians:
    {
      uint32_t options = base_options;
      line_3d l(vector_3d(rb, 0.0, -0.5 * z),
                vector_3d(rt, 0.0, +0.5 * z));
      uint32_t max_nphi = nsamples_phi;
      if (has_partial_angle()) {
        max_nphi++;
      }
      for (uint32_t iphi = 0; iphi < max_nphi ; ++iphi) {
        if (has_partial_angle()) {
          if ((iphi == 0) && no_start_angle_edge) continue;
          if ((iphi == (max_nphi - 1)) && no_stop_angle_edge) continue;
        }
        double phi = start_phi + iphi * dphi;
        placement lp(0.0, 0.0, 0.0, AXIS_Z, phi);
        l.generate_wires(wires_, lp, options);
      }
    }

    return;
  }
*/

  // Frustrum:

  /*
  // Registration :
  GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(right_circular_conical_frustrum, "geomtools::right_circular_conical_frustrum");

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
    if (has_start_angle()) {
      if (!datatools::is_valid(_delta_angle_)) {
        return false;
      }
    }
    return true;
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

  bool right_circular_conical_frustrum::has_start_angle() const
  {
    return datatools::is_valid(_start_angle_);
  }

  void right_circular_conical_frustrum::set_start_angle(double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0 || new_value_ >= 2 * M_PI,
                 std::domain_error,
                 "Invalid '" << new_value_ << "' start angle value !");
    _start_angle_ = new_value_;
    return;
  }

  double right_circular_conical_frustrum::get_start_angle() const
  {
    return _start_angle_;
  }

  bool right_circular_conical_frustrum::has_delta_angle() const
  {
    return datatools::is_valid(_delta_angle_);
  }

  void right_circular_conical_frustrum::set_delta_angle(double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0 || new_value_ > 2 * M_PI,
                 std::domain_error,
                 "Invalid '" << new_value_ << "' delta angle value !");
    _delta_angle_ = new_value_;
    return;
  }

  double right_circular_conical_frustrum::get_delta_angle() const
  {
    return _delta_angle_;
  }

  bool right_circular_conical_frustrum::has_partial_angle() const
  {
    if (_delta_angle_ == 2 * M_PI) return false;
    if (_start_angle_ > 0.0) return true;
    return false;
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
    set_start_angle(start_angle_);
    set_delta_angle(delta_angle_);
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
    datatools::invalidate(_start_angle_);
    datatools::invalidate(_delta_angle_);
    return;
  }

  void right_circular_conical_frustrum::reset ()
  {
    unlock();
    _set_defaults();
    this->i_shape_3d::reset();
    return;
  }
  void right_circular_conical_frustrum::tree_dump (std::ostream & out_,
                                                   const std::string & title_,
                                                   const std::string & indent_,
                                                   bool inherit_) const
  {
    i_object_3d::tree_dump (out_, title_, indent_, true);

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

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Start angle : ";
    if (has_start_angle()) {
      out_ << _start_angle_ / CLHEP::degree << " degree";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Delta angle : ";
    if (has_delta_angle()) {
      out_ << _delta_angle_ / CLHEP::degree << " degree";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

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
        ibr = get_inner_top_radius();
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
      top_disk_.set(_inner_top_radius_, _outer_top_radius_, _start_angle_, _delta_angle_);
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
      bottom_disk_.set(_inner_bottom_radius_, _outer_bottom_radius_, _start_angle_, _delta_angle_);
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
    on_.set_start_angle(get_start_angle());
    on_.set_delta_angle(get_delta_angle());
    return;
  }

  void right_circular_conical_frustrum::compute_start_stop_angle_face(quadrangle & face_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Frustrum is not valid !");
    face_.reset();
    if (has_partial_angle()) {
      double rib = 0.0;
      if (has_inner_bottom_radius()) {
        rib = _inner_bottom_radius_;
      }
      double rit = 0.0;
      if (has_inner_top_radius()) {
        rit = _inner_top_radius_;
      }
      vector_3d q0(rib, 0.0, -0.5 * _z_);
      vector_3d q1(_outer_bottom_radius_, 0.0, -0.5 * _z_);
      vector_3d q2(_outer_top_radius_, 0.0, +0.5 * _z_);
      vector_3d q3(rit, 0.0, +0.5 * _z_);
      face_.set_vertexes(q0, q1, q2, q3);
    }
    return;
  }

  void right_circular_conical_frustrum::compute_start_angle_face(quadrangle & face_,
                                                                 placement & face_placement_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Frustrum is not valid !");
    compute_start_stop_angle_face(face_);
    if (has_partial_angle()) {
      face_placement_.set(0.0, 0.0, 0.0, AXIS_Z, _start_angle_);
    }
    return;
  }

  void right_circular_conical_frustrum::compute_stop_angle_face(quadrangle & face_,
                                                                placement & face_placement_) const
  {
    DT_THROW_IF (! is_valid(), std::logic_error, "Frustrum is not valid !");
    compute_start_stop_angle_face(face_);
    if (has_partial_angle()) {
      face_placement_.set(0.0, 0.0, 0.0, AXIS_Z, _start_angle_ + _delta_angle_);
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
        face_info_type dummy;
        faces_.push_back(dummy);
      }
      face_info_type & side_face_info = faces_.back();
      side_face_info.set_face(new right_circular_conical_nappe);
      compute_outer_face(side_face_info.grab_face<right_circular_conical_nappe>());
      side_face_info.grab_positioning().set_identity();
      nfaces++;
    }

    if (has_inner_face()) {
      // Inner side face:
      {
        face_info_type dummy;
        faces_.push_back(dummy);
      }
      face_info_type & side_face_info = faces_.back();
      side_face_info.set_face(new right_circular_conical_nappe);
      compute_inner_face(side_face_info.grab_face<right_circular_conical_nappe>());
      side_face_info.grab_positioning().set_identity();
      nfaces++;
    }

    if (has_bottom_face()) {
      // Bottom face:
      {
        face_info_type dummy;
        faces_.push_back(dummy);
      }
      face_info_type & bottom_face_info = faces_.back();
      bottom_face_info.set_face(new disk);
      compute_bottom_face(bottom_face_info.grab_face<disk>(),
                          bottom_face_info.grab_positioning());
      nfaces++;
    }

    if (has_top_face()) {
      // Top face:
      {
        face_info_type dummy;
        faces_.push_back(dummy);
      }
      face_info_type & top_face_info = faces_.back();
      top_face_info.set_face(new disk);
      compute_top_face(top_face_info.grab_face<disk>(),
                       top_face_info.grab_positioning());
      nfaces++;
    }

    if (has_partial_angle()) {

      {
        // Start angle face:
        {
          face_info_type dummy;
          faces_.push_back(dummy);
        }
        face_info_type & top_face_info = faces_.back();
        top_face_info.set_face(new quadrangle);
        compute_start_angle_face(top_face_info.grab_face<quadrangle>(),
                                 top_face_info.grab_positioning());
        nfaces++;
      }

      {
        // Stop angle face:
        {
          face_info_type dummy;
          faces_.push_back(dummy);
        }
        face_info_type & top_face_info = faces_.back();
        top_face_info.set_face(new quadrangle);
        compute_start_angle_face(top_face_info.grab_face<quadrangle>(),
                                 top_face_info.grab_positioning());
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
      std::cerr << "DEVEL: Test 0 BEGIN" << std::endl;
      if (flags_ & FACE_INNER_SIDE) {
        std::cerr << "DEVEL: Test 0.0 BEGIN" << std::endl;
        right_circular_conical_nappe inner_face;
        compute_inner_face(inner_face);
        double s_in = inner_face.get_surface();
        s += s_in;
        std::cerr << "DEVEL: Test 0.0 END" << std::endl;
      }
      std::cerr << "DEVEL: Test 0 END" << std::endl;
    }

    if (flags_ & FACE_OUTER_SIDE) {
      std::cerr << "DEVEL: Test 1 BEGIN" << std::endl;
      right_circular_conical_nappe outer_face;
      compute_outer_face(outer_face);
      double s_out = outer_face.get_surface();
      s += s_out;
      std::cerr << "DEVEL: Test 1 END" << std::endl;
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
      quadrangle angle_face;
      compute_start_stop_angle_face(angle_face);
      if (flags_ & FACE_START_ANGLE) {
        s += angle_face.get_surface();
      }
      if (flags_ & FACE_STOP_ANGLE) {
        s += angle_face.get_surface();
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
      v *= (_delta_angle_ / 2 * M_PI);
    }
    return v;
  }

  void right_circular_conical_frustrum::initialize(const datatools::properties & config_,
                                                   const handle_dict_type * objects_)
  {
    reset();
    this->i_shape_3d::initialize(config_, objects_);

    double lunit = CLHEP::mm;
    if (config_.has_key("length_unit")) {
      const std::string lunit_str = config_.fetch_string("length_unit");
      lunit = datatools::units::get_length_unit_from(lunit_str);
    }

    double aunit = CLHEP::degree;
    if (config_.has_key("angle_unit")) {
      const std::string aunit_str = config_.fetch_string("angle_unit");
      aunit = datatools::units::get_angle_unit_from(aunit_str);
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

    set_inner_bottom_radius(inner_bottom_r);
    set_inner_top_radius(inner_top_r);
    set_outer_bottom_radius(outer_bottom_r);
    set_outer_top_radius(outer_top_r);
    set_z(z);
    if (not_full_angle) {
      set_start_angle(start_angle);
      set_delta_angle(delta_angle);
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
      if (!angle_is_in(angle, _start_angle_, _delta_angle_, angular_tolerance, true)) {
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
      if (!angle_is_in(angle, _start_angle_, _delta_angle_, angular_tolerance, true)) {
        return false;
      }
    }
    return false;
  }

  bool right_circular_conical_frustrum::is_on_surface (const vector_3d & point_,
                                                       int mask_,
                                                       double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid conical frustrum!");
    double skin = compute_tolerance(skin_);
    double angular_tolerance = get_angular_tolerance();

    int mask = mask_;
    if (mask_ == (int) ALL_SURFACES) mask = FACE_ALL;

    double r = hypot(point_.x(), point_.y());
    double phi = std::atan2(point_.y(), point_.x());

    if (mask & FACE_OUTER_SIDE) {
      conical_nappe side;
      placement side_placement;
      side_placement.set_identity();
      compute_outer_face(side);
      vector_3d p_side;
      side_placement.mother_to_child(point_, p_side);
      if (side.is_on_surface(p_side, skin)) {
        return true;
      }
    }

    if (has_inner_face()) {
      if (mask & FACE_INNER_SIDE) {
        conical_nappe side;
        placement side_placement;
        side_placement.set_identity();
        compute_inner_face(side);
        vector_3d p_side;
        side_placement.mother_to_child(point_, p_side);
        if (side.is_on_surface(p_side, skin)) {
          return true;
        }
      }
    }

    if (has_top_face()) {
      if (mask & FACE_TOP) {
        disk top;
        placement top_placement;
        compute_top_face(top, top_placement);
        vector_3d p_top;
        top_placement.mother_to_child(point_, p_top);
        if (top.is_on_surface(p_top, skin)) {
          return true;
        }
      }
    }

    if (has_bottom_face()) {
      if (mask & FACE_BOTTOM) {
        disk bottom;
        placement bottom_placement;
        compute_bottom_face(bottom, bottom_placement);
        vector_3d p_bottom;
        bottom_placement.mother_to_child(point_, p_bottom);
        if (bottom.is_on_surface(p_bottom, skin)) {
          return true;
        }
      }
    }

    if (has_partial_angle()) {
      if (mask & FACE_START_ANGLE) {
        quadrangle face;
        placement face_placement;
        compute_start_angle_face(face, face_placement);
        vector_3d p_face;
        face_placement.mother_to_child(point_, p_face);
        if (face.is_on_surface(p_face, skin)) {
          return true;
        }
      }

      if (mask & FACE_STOP_ANGLE) {
        quadrangle face;
        placement face_placement;
        compute_stop_angle_face(face, face_placement);
        vector_3d p_face;
        face_placement.mother_to_child(point_, p_face);
        if (face.is_on_surface(p_face, skin)) {
          return true;
        }
      }

    }
    return false;
  }

  vector_3d right_circular_conical_frustrum::get_normal_on_surface (const vector_3d & position_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid conical frustrum!");
    vector_3d normal;
    geomtools::invalidate(normal);
    if (is_on_surface(position_, FACE_OUTER_SIDE)) {
      double phi = position_.phi();
      normal.set(std::cos(phi), std::sin(phi), 0.0);
    } else if (has_inner_face()) {
      if (is_on_surface(position_, FACE_INNER_SIDE)) {
      }
    } else if (is_on_surface(position_, FACE_BOTTOM)) {
      normal.set(0.0, 0.0, -1.0);
    } else if (is_on_surface(position_, FACE_TOP)) {
      normal.set(0.0, 0.0, +1.0);
    }
    return normal;
  }

  bool right_circular_conical_frustrum::find_intercept (const vector_3d & from_,
                                                        const vector_3d & direction_,
                                                        intercept_data_type & intercept_,
                                                        double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid conical frustrum!");
    intercept_.reset ();
    double skin = compute_tolerance(skin_);

    const unsigned int NFACES = 6;
    intercept_data_type intercepts[NFACES];
    unsigned int candidate_impact_counter = 0;

    if (has_bottom_face()) {
      const int FACE_INDEX = 0;
      disk bottom_face;
      placement bottom_face_placement;
      compute_bottom_face(bottom_face, bottom_face_placement);
      if (bottom_face.i_find_intercept::find_intercept(from_,
                                                       direction_,
                                                       bottom_face_placement,
                                                       intercepts[FACE_INDEX],
                                                       skin)) {
        intercepts[FACE_INDEX].set_shape_index(0);
        intercepts[FACE_INDEX].set_face(FACE_BOTTOM);
        candidate_impact_counter++;
      }
    }

    if (has_top_face()) {
      const int FACE_INDEX = 1;
      disk top_face;
      placement top_face_placement;
      compute_top_face(top_face, top_face_placement);
      if (top_face.i_find_intercept::find_intercept(from_,
                                                    direction_,
                                                    top_face_placement,
                                                    intercepts[FACE_INDEX],
                                                    skin)) {
        intercepts[FACE_INDEX].set_shape_index(0);
        intercepts[FACE_INDEX].set_face(FACE_TOP);
        candidate_impact_counter++;
      }
    }

    {
      const int FACE_INDEX = 2;
      conical_nappe outer_side_face;
      placement outer_side_face_placement;
      outer_side_face_placement.set_identity();
      compute_outer_face(outer_side_face);
      if (outer_side_face.i_find_intercept::find_intercept(from_,
                                                           direction_,
                                                           outer_side_face_placement,
                                                           intercepts[FACE_INDEX], skin)) {
        intercepts[FACE_INDEX].set_shape_index(0);
        intercepts[FACE_INDEX].set_face(FACE_OUTER_SIDE);
        candidate_impact_counter++;
      }
    }

    if (has_inner_face()) {
      const int FACE_INDEX = 3;
      conical_nappe inner_side_face;
      placement inner_side_face_placement;
      inner_side_face_placement.set_identity();
      compute_inner_face(inner_side_face);
      if (inner_side_face.i_find_intercept::find_intercept(from_,
                                                           direction_,
                                                           inner_side_face_placement,
                                                           intercepts[FACE_INDEX],
                                                           skin)) {
        intercepts[FACE_INDEX].set_shape_index(0);
        intercepts[FACE_INDEX].set_face(FACE_INNER_SIDE);
        candidate_impact_counter++;
      }
    }

    if (has_partial_angle()) {

      {
        const int FACE_INDEX = 4;
        quadrangle start_phi_face;
        placement start_phi_face_placement;
        compute_start_angle_face(start_phi_face, start_phi_face_placement);
        if (start_phi_face.i_find_intercept::find_intercept(from_,
                                                            direction_,
                                                            start_phi_face_placement,
                                                            intercepts[FACE_INDEX],
                                                            skin)) {
          intercepts[FACE_INDEX].set_shape_index(0);
          intercepts[FACE_INDEX].set_face(FACE_START_ANGLE);
          candidate_impact_counter++;
        }
      }

      {
        const int FACE_INDEX = 5;
        quadrangle stop_phi_face;
        placement stop_phi_face_placement;
        compute_stop_angle_face(stop_phi_face, stop_phi_face_placement);
        if (stop_phi_face.i_find_intercept::find_intercept(from_,
                                                           direction_,
                                                           stop_phi_face_placement,
                                                           intercepts[FACE_INDEX],
                                                           skin)) {
          intercepts[FACE_INDEX].set_shape_index(0);
          intercepts[FACE_INDEX].set_face(FACE_STOP_ANGLE);
          candidate_impact_counter++;
        }
      }

    }

    if (candidate_impact_counter > 0) {
      double min_length_to_impact = -1.0;
      for (unsigned int iface = 0; iface < NFACES; iface++) {
        if (intercepts[iface].is_ok()) {
          double length_to_impact = (intercepts[iface].get_impact() - from_).mag();
          if (min_length_to_impact < 0.0 || length_to_impact < min_length_to_impact) {
            min_length_to_impact = length_to_impact;
            intercept_.set_shape_index(intercepts[iface].get_shape_index());
            intercept_.set_face(intercepts[iface].get_face());
            intercept_.set_impact(intercepts[iface].get_impact());
          }
        }
      }
    }

    return intercept_.is_ok ();
  }


  void right_circular_conical_frustrum::generate_wires_self(wires_type & wires_,
                                                            uint32_t options_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid conical frustrum!");
    bool debug_explode    = options_ & WR_BASE_EXPLODE;
    bool draw_bottom      = !(options_ & WR_RCCF_NO_BOTTOM_FACE);
    bool draw_top         = !(options_ & WR_RCCF_NO_TOP_FACE);
    bool draw_inner       = !(options_ & WR_RCCF_NO_INNER_FACE);
    bool draw_outer       = !(options_ & WR_RCCF_NO_OUTER_FACE);
    bool draw_start_angle = !(options_ & WR_RCCF_NO_START_ANGLE_FACE);
    bool draw_stop_angle  = !(options_ & WR_RCCF_NO_STOP_ANGLE_FACE);

    // Extract base rendering options:
    uint32_t base_options = options_ & WR_BASE_MASK;

    double explode_factor = 1.0;
    if (debug_explode) {
      explode_factor = 1.25;
    }

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
        quadrangle angle_face;
        placement p;
        compute_start_angle_face(angle_face, p);
        angle_face.generate_wires(wires_, p, options);
      }
      if (draw_stop_angle) {
        quadrangle angle_face;
        placement p;
        compute_stop_angle_face(angle_face, p);
        angle_face.generate_wires(wires_, p, options);
      }
    }

    return;
  }

  void right_circular_conical_frustrum::_build_bounding_data()
  {
    _grab_bounding_data().make_cylinder(get_xmax(),
                                        -0.5 * get_z(),
                                        +0.5 * get_z());
    return;
  }

  */

} // end of namespace geomtools
