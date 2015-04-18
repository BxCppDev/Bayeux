// spherical_sector.cc

// Ourselves:
#include <geomtools/spherical_sector.h>

// Standard library:
#include <cmath>
#include <iostream>

// Third-party:
// - GSL:
#include <gsl/gsl_math.h>
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/units.h>
#include <datatools/utils.h>
// - Bayeux/mygsl:
#include <mygsl/polynomial.h>

// This project:
// #include <geomtools/geomtools_config.h>
#include <geomtools/circle.h>
#include <geomtools/line_3d.h>

namespace geomtools {

  // static
  const std::string & spherical_sector::spherical_sector_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "spherical_sector";
    }
    return label;
  }

  std::string spherical_sector::get_shape_name() const
  {
    return spherical_sector::spherical_sector_label();
  }

  void spherical_sector::_set_defaults()
  {
    datatools::invalidate(_radius_);
    datatools::invalidate(_start_theta_);
    datatools::invalidate(_delta_theta_);
    datatools::invalidate(_start_phi_);
    datatools::invalidate(_delta_phi_);
    return;
  }

  void spherical_sector::set_radius(double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0, std::domain_error, "Invalid radius value!");
    _radius_ = new_value_;
    return;
  }

  double spherical_sector::get_radius() const
  {
    return _radius_;
  }

  bool spherical_sector::has_partial_theta() const
  {
    if (_delta_theta_ == M_PI) return false;
    if (_start_theta_ > 0.0) return true;
    return false;
  }

  bool spherical_sector::has_start_theta() const
  {
    return datatools::is_valid(_start_theta_);
  }

  void spherical_sector::set_start_theta(double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0 || new_value_ >= M_PI,
                 std::domain_error,
                 "Invalid '" << new_value_ << "' start theta value !");
    _start_theta_ = new_value_;
    return;
  }

  double spherical_sector::get_start_theta() const
  {
    return _start_theta_;
  }

  bool spherical_sector::has_delta_theta() const
  {
    return datatools::is_valid(_delta_theta_);
  }

  void spherical_sector::set_delta_theta(double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0 || new_value_ > M_PI,
                 std::domain_error,
                 "Invalid '" << new_value_ << "' delta theta value !");
    _delta_theta_ = new_value_;
    return;
  }

  double spherical_sector::get_delta_theta() const
  {
    return _delta_theta_;
  }

  bool spherical_sector::has_partial_phi() const
  {
    if (_delta_phi_ == 2 * M_PI) return false;
    if (_start_phi_ > 0.0) return true;
    return false;
  }

  bool spherical_sector::has_start_phi() const
  {
    return datatools::is_valid(_start_phi_);
  }

  void spherical_sector::set_start_phi(double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0 || new_value_ >= 2 * M_PI,
                 std::domain_error,
                 "Invalid '" << new_value_ << "' start phi value !");
    _start_phi_ = new_value_;
    return;
  }

  double spherical_sector::get_start_phi() const
  {
    return _start_phi_;
  }

  bool spherical_sector::has_delta_phi() const
  {
    return datatools::is_valid(_delta_phi_);
  }

  void spherical_sector::set_delta_phi(double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0 || new_value_ > 2 * M_PI,
                 std::domain_error,
                 "Invalid '" << new_value_ << "' delta phi value !");
    _delta_phi_ = new_value_;
    return;
  }

  double spherical_sector::get_delta_phi() const
  {
    return _delta_phi_;
  }

  bool spherical_sector::is_valid() const
  {
    if (!datatools::is_valid(_radius_)) {
      return false;
    }
    if (has_start_theta()) {
      if (!datatools::is_valid(_delta_theta_)) {
        return false;
      }
    }
    if (has_start_phi()) {
      if (!datatools::is_valid(_delta_phi_)) {
        return false;
      }
    }
    return true;
  }

  spherical_sector::spherical_sector()
  {
    _set_defaults();
    return;
  }

  spherical_sector::spherical_sector(double radius_)
  {
    _set_defaults();
    set_radius(radius_);
    return;
  }

  spherical_sector::spherical_sector(double radius_,
                                     double start_theta_,
                                     double delta_theta_,
                                     double start_phi_,
                                     double delta_phi_)
  {
    _set_defaults();
    set_radius(radius_);
    set_start_theta(start_theta_);
    set_delta_theta(delta_theta_);
    set_start_phi(start_phi_);
    set_delta_phi(delta_phi_);
    return;
  }

  spherical_sector::~spherical_sector()
  {
    return;
  }

  double spherical_sector::get_surface(uint32_t /* flags_ */) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid spherical sector!");
    double s;
    datatools::invalidate(s);
    if (is_valid()) {
      double start_theta = 0.0;
      double delta_theta = M_PI;
      if (has_start_theta()) {
        start_theta = _start_theta_;
        delta_theta = _delta_theta_;
      }
      double stop_theta = start_theta + delta_theta;
      double start_phi = 0.0;
      double delta_phi = 2 * M_PI;
      if (has_start_phi()) {
        start_phi = _start_phi_;
        delta_phi = _delta_phi_;
      }
      double stop_phi = start_phi + delta_phi;
      s = _radius_ * _radius_ *(delta_phi) *
        (std::cos(start_theta) - std::cos(stop_theta));
    }
    return s;
  }

  bool spherical_sector::is_on_surface(const vector_3d & position_,
                                       double tolerance_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid spherical sector!");
    double z_tolerance = compute_tolerance(tolerance_);
    double half_z_tolerance = 0.5 * z_tolerance;
    double angular_tolerance = get_angular_tolerance();
    if (is_valid()) {
      double r = position_.mag();
      if (std::abs(r - _radius_) > half_z_tolerance) {
        return false;
      }
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
      double theta = position_.theta();
      double start_theta = 0.0;
      double delta_theta = M_PI;
      if (has_start_theta()) {
        start_theta = _start_theta_;
        delta_theta = _delta_theta_;
      }
      if (!angle_is_in(theta,
                       start_theta,
                       delta_theta,
                       angular_tolerance,
                       true)) {
        return false;
      }
    }
    return true;
  }

  vector_3d spherical_sector::get_normal_on_surface(const vector_3d & position_,
                                                    bool check_,
                                                    double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid spherical sector!");
    vector_3d normal;
    geomtools::invalidate(normal);
    DT_THROW_IF(check_ && !is_on_surface(position_, skin_), std::logic_error,
                "Position " << position_ << " is not on surface!");
    create_spherical(normal, 1.0, position_.phi(), position_.theta());
    return normal;
  }

  bool spherical_sector::find_intercept(const vector_3d & from_,
                                        const vector_3d & direction_,
                                        face_intercept_info & intercept_,
                                        double tolerance_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid spherical sector!");
    bool devel = false;
    //devel = true;
    intercept_.reset();
    double tolerance = compute_tolerance(tolerance_);
    double p0 = from_.x();
    double p1 = from_.y();
    double p2 = from_.z();
    double d0 = direction_.x();
    double d1 = direction_.y();
    double d2 = direction_.z();
    double a = (d0 * d0 + d1 * d1 + d2 * d2);
    double b = 2 * (d0 * p0 + d1 * p1 + d2 * p2);
    double c = (p0 * p0 + p1 * p1 + p2 * p2 - _radius_ * _radius_);
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
          impact0.set(p0 + t0 * d0, p1 + t0 * d1, p2 + t0 * d2);
          if (devel) std::cerr << "DEVEL:   impact0=" << impact0 << std::endl;
          if (! is_on_surface(impact0, tolerance)) {
            if (devel) std::cerr << "DEVEL:   impact0 is not on surface" << std::endl;
            geomtools::invalidate(impact0);
          } else {
            if (devel) std::cerr << "DEVEL:  use impact0 =" << impact0 << std::endl;
            intercept_.grab_face_id().set_face_bit(FACE_UNIQUE);
            intercept_.set_impact(impact0);
          }
        }
      }
      if (nsols > 1) {
        if (devel) std::cerr << "DEVEL:   t1=" << t1 << std::endl;
        if (t1 >= 0) {
          impact1.set(p0 + t1 * d0, p1 + t1 * d1, p2 + t1 * d2);
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
                intercept_.grab_face_id().set_face_bit(FACE_UNIQUE);
                intercept_.set_impact(impact1);
              } else {
                if (devel) std::cerr << "DEVEL:   l1 >= l0: use impact0 = " << impact0 << std::endl;
              }
            } else {
              if (devel) std::cerr << "DEVEL:   use impact1 = " << impact1 << std::endl;
              intercept_.grab_face_id().set_face_bit(FACE_UNIQUE);
              intercept_.set_impact(impact1);
            }
          }
        }
      }
    }
    if (devel) intercept_.print(std::cerr, "DEVEL: spherical_sector::find_intercept: ");
    if (devel) std::cerr << "" << std::endl;
    return intercept_.is_ok();
  }

  void spherical_sector::tree_dump(std::ostream & out_,
                                   const std::string & title_,
                                   const std::string & indent_,
                                   bool inherit_) const
  {
    i_object_3d::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Radius : ";
    if (datatools::is_valid(_radius_)) {
      out_ << _radius_ / CLHEP::mm << " mm";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Start theta : ";
    if (has_start_theta()) {
      out_ << _start_theta_ / CLHEP::degree << " degree";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Delta theta : ";
    if (has_delta_theta()) {
      out_ << _delta_theta_ / CLHEP::degree << " degree";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Start phi : ";
    if (has_start_phi()) {
      out_ << _start_phi_ / CLHEP::degree << " degree";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Delta phi : ";
    if (has_delta_phi()) {
      out_ << _delta_phi_ / CLHEP::degree << " degree";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    return;
  }

  void spherical_sector::generate_wires_self(wires_type & wires_,
                                             uint32_t options_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid spherical sector!");
    bool no_start_theta_edge = (options_ & WR_SPHSEC_NO_START_THETA_EDGE);
    bool no_stop_theta_edge  = (options_ & WR_SPHSEC_NO_STOP_THETA_EDGE);
    bool no_start_phi_edge   = (options_ & WR_SPHSEC_NO_START_PHI_EDGE);
    bool no_stop_phi_edge    = (options_ & WR_SPHSEC_NO_STOP_PHI_EDGE);
    bool z_sampling          = (options_ & WR_SPHSEC_Z_SAMPLING);

    // Keep only base rendering bits:
    uint32_t base_options = options_ & WR_BASE_MASK;

    // uint32_t nsamples_phi = 0;
    // if (has_partial_phi()) {
    //   nsamples_phi = 2;
    // }
    uint32_t nsamples_phi = angular_sampling_from_options(base_options);

    double start_phi = 0.0;
    double delta_phi = 2.0 * M_PI;
    if (has_partial_phi()) {
      start_phi = _start_phi_;
      delta_phi = _delta_phi_;
    }
    double dphi = delta_phi / nsamples_phi;

    double start_theta = 0.0;
    double delta_theta = M_PI;
    if (has_partial_theta()) {
      start_theta = _start_theta_;
      delta_theta = _delta_theta_;
    }
    // std::cerr << "DEVEL: start_theta=" << start_theta / CLHEP::degree << " degree" << std::endl;
    // std::cerr << "DEVEL: delta_theta=" << delta_theta / CLHEP::degree << " degree" << std::endl;
    //double dtheta = delta_theta / nsamples_theta;

    // Draw parallel arcs:
    {
      if (z_sampling) {
        uint32_t options = base_options;
        double z0 = _radius_ * std::cos(start_theta + delta_theta);
        double z1 = _radius_ * std::cos(start_theta);
        uint32_t nsamples_z = linear_sampling_from_options(base_options);
        double dz = (z1 - z0) / (nsamples_z - 1);
        std::cerr << "DEVEL: dz=" << dz / CLHEP::mm << " mm" << std::endl;
        for (int iz = 0; iz <= nsamples_z; iz++) {
          double z = z0 + iz * dz;
          std::cerr << "DEVEL: z=" << z / CLHEP::mm << " mm" << std::endl;
          //        circle arc(_radius_ * std::cos(std::asin(z/_radius_)));
          circle arc(std::sqrt(gsl_pow_2(_radius_) - gsl_pow_2(z)));
          arc.set_start_angle(start_phi);
          arc.set_delta_angle(delta_phi);
          placement arc_p(0.0, 0.0, z);
          arc.generate_wires(wires_, arc_p, options);
        }
      } else {
        uint32_t options = base_options;
        uint32_t nsamples_theta = angular_sampling_from_options(base_options);
        uint32_t max_ntheta = nsamples_theta;
        if (has_partial_theta()) {
          max_ntheta++;
        }
        double dtheta = delta_theta / nsamples_theta;
        for (uint32_t itheta = 0; itheta < max_ntheta ; ++itheta) {
          if (has_partial_theta()) {
            if ((itheta == 0) && no_start_theta_edge) continue;
            if ((itheta ==(max_ntheta - 1)) && no_stop_theta_edge) continue;
          }
          double theta = start_theta + itheta * dtheta;
          double z = _radius_ * std::cos(theta);
          circle arc(std::sqrt(gsl_pow_2(_radius_) - gsl_pow_2(z)));
          arc.set_start_angle(start_phi);
          arc.set_delta_angle(delta_phi);
          placement arc_p(0.0, 0.0, z);
          arc.generate_wires(wires_, arc_p, options);
        }
      }
    }

    // Draw meridian arcs:
    {
      uint32_t options = base_options;
      circle arc(_radius_);
      arc.set_start_angle(start_theta);
      arc.set_delta_angle(delta_theta);
      uint32_t max_nphi = nsamples_phi;
      if (has_partial_phi()) {
        max_nphi++;
      }
      for (uint32_t iphi = 0; iphi < max_nphi ; ++iphi) {
        if (has_partial_phi()) {
          if ((iphi == 0) && no_start_phi_edge) continue;
          if ((iphi == (max_nphi - 1)) && no_stop_phi_edge) continue;
        }
        double phi = start_phi + iphi * dphi;
        placement arc_p(0.0, 0.0, 0.0,
                        -90 * CLHEP::degree,
                        -90 * CLHEP::degree,
                        phi,
                        EULER_ANGLES_ZYX);
        arc.generate_wires(wires_, arc_p, options);
      }
    }

    return;
  }

} // end of namespace geomtools
