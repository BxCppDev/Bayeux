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

  // Registration :
  GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(spherical_sector,
                                             "geomtools::spherical_sector")

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
    _theta_domain_.set_type(angular_range::RANGE_TYPE_POLAR);
    _theta_domain_.reset_partial_angles();
    _phi_domain_.set_type(angular_range::RANGE_TYPE_AZIMUTHAL);
    _phi_domain_.reset_partial_angles();
    return;
  }

  void spherical_sector::initialize(const datatools::properties & config_, const handle_dict_type * objects_)
  {
    if (!is_valid()) {
      this->i_object_3d::_initialize(config_, objects_);

      double lunit = CLHEP::mm;
      if (config_.has_key("length_unit")) {
        const std::string lunit_str = config_.fetch_string("length_unit");
        lunit = datatools::units::get_length_unit_from(lunit_str);
      }

      DT_THROW_IF(! config_.has_key("radius"), std::logic_error, "Missing spherical sector 'radius' property !");
      double radius = config_.fetch_real("radius");
      if (! config_.has_explicit_unit("radius")) {
        radius *= lunit;
      }

      set_radius(radius);

      datatools::properties theta_config;
      config_.export_and_rename_starting_with(theta_config, "theta.", "");
      if (theta_config.size()) {
        _theta_domain_.initialize(theta_config);
      }
      datatools::properties phi_config;
      config_.export_and_rename_starting_with(phi_config, "phi.", "");
      if (phi_config.size()) {
        _phi_domain_.initialize(phi_config);
      }
      if (! theta_config.size() && !phi_config.size()) {
        // deprecated ;

        double aunit = CLHEP::degree;
        if (config_.has_key("angle_unit")) {
          const std::string aunit_str = config_.fetch_string("angle_unit");
          aunit = datatools::units::get_angle_unit_from(aunit_str);
        }

        double start_theta = 0.0;
        double delta_theta = M_PI * CLHEP::radian;
        bool not_full_theta = false;
        if (config_.has_key ("start_theta")) {
          start_theta = config_.fetch_real ("start_theta");
          if (! config_.has_explicit_unit ("start_theta")) {
            start_theta *= aunit;
          }
          not_full_theta = true;
        }
        if (config_.has_key ("delta_theta")) {
          delta_theta = config_.fetch_real ("delta_theta");
          if (! config_.has_explicit_unit ("delta_theta")) {
            delta_theta *= aunit;
          }
          not_full_theta = true;
        }
        if (not_full_theta) {
          set_start_theta(start_theta);
          set_delta_theta(delta_theta);
        }

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
          set_start_phi(start_phi);
          set_delta_phi(delta_phi);
        }
      }
    }

    return;
  }

  void spherical_sector::reset()
  {
    _set_defaults();
    this->i_object_3d::reset();
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

  const angular_range & spherical_sector::get_theta_domain() const
  {
    return _theta_domain_;
  }

  bool spherical_sector::has_partial_theta() const
  {
    return _theta_domain_.is_partial();
  }

  bool spherical_sector::has_start_theta() const
  {
    return _theta_domain_.has_start_angle();
  }

  void spherical_sector::set_start_theta(double new_value_)
  {
    _theta_domain_.set_start_angle(new_value_);
    return;
  }

  double spherical_sector::get_start_theta() const
  {
    return _theta_domain_.get_start_angle();
  }

  bool spherical_sector::has_delta_theta() const
  {
    return _theta_domain_.has_delta_angle();
  }

  void spherical_sector::set_delta_theta(double new_value_)
  {
    _theta_domain_.set_delta_angle(new_value_);
    return;
  }

  double spherical_sector::get_delta_theta() const
  {
    return _theta_domain_.get_delta_angle();
  }

  const angular_range & spherical_sector::get_phi_domain() const
  {
    return _phi_domain_;
  }

  bool spherical_sector::has_partial_phi() const
  {
    return _phi_domain_.is_partial();
  }

  bool spherical_sector::has_start_phi() const
  {
    return _phi_domain_.has_start_angle();
  }

  void spherical_sector::set_start_phi(double new_value_)
  {
    _phi_domain_.set_start_angle(new_value_);
    return;
  }

  double spherical_sector::get_start_phi() const
  {
    return _phi_domain_.get_start_angle();
  }

  bool spherical_sector::has_delta_phi() const
  {
    return _phi_domain_.has_delta_angle();
  }

  void spherical_sector::set_delta_phi(double new_value_)
  {
    _phi_domain_.set_delta_angle(new_value_);
    return;
  }

  double spherical_sector::get_delta_phi() const
  {
    return _phi_domain_.get_delta_angle();
  }

  bool spherical_sector::is_valid() const
  {
    if (!datatools::is_valid(_radius_)) {
      return false;
    }
    if (!_theta_domain_.is_valid()) {
      return false;
    }
    if (!_phi_domain_.is_valid()) {
      return false;
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
    _theta_domain_.set_partial_angles(start_theta_, delta_theta_);
    _phi_domain_.set_partial_angles(start_phi_, delta_phi_);
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
      if (has_partial_theta()) {
        start_theta = get_start_theta();
        delta_theta = get_delta_theta();
      }
      double stop_theta = start_theta + delta_theta;
      s = _radius_ * _radius_ * _phi_domain_.get_angle_spread() *
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
      if (!angle_is_in(phi,
                       _phi_domain_.get_first_angle(),
                       _phi_domain_.get_angle_spread(),
                       angular_tolerance,
                       true)) {
        return false;
      }
      double theta = position_.theta();
      if (!angle_is_in(theta,
                       _theta_domain_.get_first_angle(),
                       _theta_domain_.get_angle_spread(),
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

    {
      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Theta domain : " << std::endl;
      std::ostringstream indent2;
      indent2 << indent_ << datatools::i_tree_dumpable::skip_tag;
      _theta_domain_.tree_dump(out_, "", indent2.str());
    }

    {
      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Phi domain : " << std::endl;
      std::ostringstream indent2;
      indent2 << indent_ << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
      _phi_domain_.tree_dump(out_, "", indent2.str());
    }


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

    // std::cerr << "DEVEL: _radius_=" << _radius_ / CLHEP::mm << " mm" << std::endl;

    // Keep only base rendering bits:
    uint32_t base_options = options_ & WR_BASE_MASK;

    // uint32_t nsamples_phi = 0;
    // if (has_partial_phi()) {
    //   nsamples_phi = 2;
    // }
    uint32_t nsamples_phi = angular_sampling_from_options(base_options);

    double start_phi = _phi_domain_.get_first_angle();
    double delta_phi = _phi_domain_.get_angle_spread();
    double dphi = delta_phi / nsamples_phi;

    double start_theta = _theta_domain_.get_first_angle();
    double delta_theta = _theta_domain_.get_angle_spread();
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
        // std::cerr << "DEVEL: dz=" << dz / CLHEP::mm << " mm" << std::endl;
        for (size_t iz = 0; iz <= nsamples_z; iz++) {
          double z = z0 + iz * dz;
          // std::cerr << "DEVEL: z=" << z / CLHEP::mm << " mm" << std::endl;
          //        circle arc(_radius_ * std::cos(std::asin(z/_radius_)));
          double r_arc = std::sqrt(gsl_pow_2(_radius_) - gsl_pow_2(z));
          circle arc(r_arc);
          if (has_partial_phi()) {
            arc.set_start_angle(start_phi);
            arc.set_delta_angle(delta_phi);
          }
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
          //std::cerr << "DEVEL: z=" << z / CLHEP::mm << " mm" << std::endl;
          double r_arc = std::sqrt(gsl_pow_2(_radius_) - gsl_pow_2(z));
          //std::cerr << "DEVEL: r_arc=" << r_arc / CLHEP::mm << " mm" << std::endl;
          circle arc(r_arc);
          if (has_partial_phi()) {
            arc.set_start_angle(start_phi);
            arc.set_delta_angle(delta_phi);
          }
          placement arc_p(0.0, 0.0, z);
          arc.generate_wires(wires_, arc_p, options);
        }
      }
    }

    // Draw meridian arcs:
    {
      uint32_t options = base_options;
      circle arc(_radius_);
      arc.set_start_angle(_theta_domain_.get_first_angle());
      arc.set_delta_angle(_theta_domain_.get_angle_spread());
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
