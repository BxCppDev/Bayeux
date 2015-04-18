// right_circular_conical_nappe.cc

// Ourselves:
#include <geomtools/right_circular_conical_nappe.h>

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

// This project:
#include <geomtools/circle.h>
#include <geomtools/line_3d.h>

namespace geomtools {

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
                                                             double delta_angle_)
  {
    _set_defaults();
    set_bottom_radius(bottom_radius_);
    set_top_radius(top_radius_);
    set_z(z_);
    set_start_angle(start_angle_);
    set_delta_angle(delta_angle_);
    return;
  }

  right_circular_conical_nappe::~right_circular_conical_nappe()
  {
    return;
  }

  double right_circular_conical_nappe::get_surface(uint32_t /* flags_ */) const
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

  bool right_circular_conical_nappe::is_on_surface(const vector_3d & position_,
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

  bool right_circular_conical_nappe::find_intercept(const vector_3d & from_,
                                                    const vector_3d & direction_,
                                                    face_intercept_info & intercept_,
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
          /* if (std::abs(z0) < (0.5 * _z_ + tolerance)) { */
          impact0.set(p0 + t0 * d0, p1 + t0 * d1, z0);
          if (devel) std::cerr << "DEVEL:   impact0=" << impact0 << std::endl;
          if (! is_on_surface(impact0, tolerance)) {
            if (devel) std::cerr << "DEVEL:   impact0 is not on surface" << std::endl;
            geomtools::invalidate(impact0);
          } else {
            if (devel) std::cerr << "DEVEL:  use impact0 =" << impact0 << std::endl;
            intercept_.grab_face_id().set_face_bits(FACE_UNIQUE);
            intercept_.set_impact(impact0);
          }
          /* } */
        } // if (t0 >= 0)
      } // if (nsols > 0)
      if (nsols > 1) {
        if (devel) std::cerr << "DEVEL:   t1=" << t1 << std::endl;
        if (t1 >= 0) {
          double z1 = p2 + t1 * d2;
          /* if (std::abs(z1) < (0.5 * _z_ + tolerance)) { */
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
                intercept_.grab_face_id().set_face_bits(FACE_UNIQUE);
                intercept_.set_impact(impact1);
              } else {
                if (devel) std::cerr << "DEVEL:   l1 >= l0: use impact0 = " << impact0 << std::endl;
              }
            } else {
              if (devel) std::cerr << "DEVEL:   use impact1 = " << impact1 << std::endl;
              intercept_.grab_face_id().set_face_bits(FACE_UNIQUE);
              intercept_.set_impact(impact1);
            }
            /* }*/
          }
        } // if (t1 >= 0)
      } // if (nsols > 1)
    } // solve_quadratic
    if (devel) intercept_.print(std::cerr, "DEVEL: right_circular_conical_nappe::find_intercept: ");
    if (devel) std::cerr << std::endl;
    return intercept_.is_ok();
  }

  void right_circular_conical_nappe::tree_dump(std::ostream & out_,
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
        //           << "rt = " << rt
        //           << std::endl;
        // std::cerr << "DEVEL: right_circular_conical_nappe::generate_wires_self: "
        //           << "rb = " << rb
        //           << std::endl;
        // std::cerr << "DEVEL: right_circular_conical_nappe::generate_wires_self: "
        //           << "rz = " << rz
        //           << std::endl;
        if (rz > 0.0) {
          circle arc(rz);
          // std::cerr << "DEVEL: right_circular_conical_nappe::generate_wires_self: "
          //           << "rz OK"
          //           << std::endl;
          if (has_partial_angle()) {
            arc.set_start_angle(_start_angle_);
            arc.set_delta_angle(_delta_angle_);
          }
          placement arc_placement(0.0, 0.0, zz);
          arc.generate_wires(wires_, arc_placement, options);
        }
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

} // end of namespace geomtools
