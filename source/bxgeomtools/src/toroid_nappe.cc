// toroid_nappe.cc

// Ourselves:
#include <geomtools/toroid_nappe.h>

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
#include <geomtools/circle.h>
#include <geomtools/line_3d.h>

namespace geomtools {

  // Registration :
  GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(toroid_nappe,
                                             "geomtools::toroid_nappe")

  // static
  const std::string & toroid_nappe::toroid_nappe_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "toroid_nappe";
    }
    return label;
  }


  std::string toroid_nappe::get_shape_name() const
  {
    return toroid_nappe::toroid_nappe_label();
  }


  void toroid_nappe::_set_defaults()
  {
    datatools::invalidate(_sweep_radius_);
    datatools::invalidate(_radius_);
    _phi_domain_.set_type(angular_range::RANGE_TYPE_AZIMUTHAL);
    _phi_domain_.reset_partial_angles();
    return;
  }

  void toroid_nappe::_at_init()
  {
    DT_THROW_IF(_radius_ > _sweep_radius_, std::logic_error,
                "Toroid nappe radius is larger than the sweep radius!");
    return;
  }

  void toroid_nappe::initialize(const datatools::properties & config_,
                                const handle_dict_type * objects_)
  {
    if (!is_valid()) {

      this->i_object_3d::_initialize(config_, objects_);

      double lunit = CLHEP::mm;
      if (config_.has_key("length_unit")) {
        const std::string lunit_str = config_.fetch_string("length_unit");
        lunit = datatools::units::get_length_unit_from(lunit_str);
      }

      DT_THROW_IF(! config_.has_key("sweep_radius"), std::logic_error, "Missing toroid nappe 'sweep_radius' property !");
      double sweep_radius = config_.fetch_real("sweep_radius");
      if (! config_.has_explicit_unit("sweep_radius")) {
        sweep_radius *= lunit;
      }

      set_sweep_radius(sweep_radius);

      DT_THROW_IF(! config_.has_key("radius"), std::logic_error, "Missing toroid nappe 'radius' property !");
      double radius = config_.fetch_real("radius");
      if (! config_.has_explicit_unit("radius")) {
        radius *= lunit;
      }

      set_radius(radius);

      double aunit = CLHEP::degree;
      if (config_.has_key("angle_unit")) {
        const std::string aunit_str = config_.fetch_string("angle_unit");
        aunit = datatools::units::get_angle_unit_from(aunit_str);
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

    _at_init();

    return;
  }

  void toroid_nappe::reset()
  {
    _set_defaults();
    this->i_object_3d::reset();
    return;
  }

  void toroid_nappe::set_sweep_radius(double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0, std::domain_error, "Invalid sweep radius value!");
    _sweep_radius_ = new_value_;
    return;
  }

  double toroid_nappe::get_sweep_radius() const
  {
    return _sweep_radius_;
  }

  void toroid_nappe::set_radius(double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0, std::domain_error, "Invalid radius value!");
    _radius_ = new_value_;
    return;
  }

  double toroid_nappe::get_radius() const
  {
    return _radius_;
  }

  bool toroid_nappe::has_partial_phi() const
  {
    return _phi_domain_.is_partial();
  }

  bool toroid_nappe::has_start_phi() const
  {
    return _phi_domain_.has_start_angle();
  }

  void toroid_nappe::set_start_phi(double new_value_)
  {
    _phi_domain_.set_start_angle(new_value_);
    return;
  }

  double toroid_nappe::get_start_phi() const
  {
    return _phi_domain_.get_start_angle();
  }

  bool toroid_nappe::has_delta_phi() const
  {
    return _phi_domain_.has_delta_angle();
  }

  void toroid_nappe::set_delta_phi(double new_value_)
  {
    _phi_domain_.set_delta_angle(new_value_);
    return;
  }

  double toroid_nappe::get_delta_phi() const
  {
    return _phi_domain_.get_delta_angle();
  }

  bool toroid_nappe::is_valid() const
  {
    return(datatools::is_valid(_sweep_radius_)
	   && datatools::is_valid(_radius_));
  }

  toroid_nappe::toroid_nappe()
  {
    _set_defaults();
    return;
  }

  toroid_nappe::toroid_nappe(double sweep_radius_,
                             double radius_)
  {
    _set_defaults();
    set_sweep_radius(sweep_radius_);
    set_radius(radius_);
    return;
  }

  toroid_nappe::toroid_nappe(double sweep_radius_,
                             double radius_,
                             double start_phi_,
                             double delta_phi_)
  {
    _set_defaults();
    set_sweep_radius(sweep_radius_);
    set_radius(radius_);
    set_start_phi(start_phi_);
    set_delta_phi(delta_phi_);
    return;
  }

  toroid_nappe::~toroid_nappe()
  {
    return;
  }

  double toroid_nappe::get_surface(uint32_t /* flags_ */) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid toroid nappe!");
    double s;
    datatools::invalidate(s);
    if (is_valid()) {
      double delta_phi = 2 * M_PI;
      if (has_start_phi()) {
        delta_phi = get_delta_phi();
      }
      s = 2 * M_PI *_radius_ * _sweep_radius_ * delta_phi / (2 * M_PI);
    }
    return s;
  }

  bool toroid_nappe::is_on_surface(const vector_3d & position_,
                                   double tolerance_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid toroid nappe!");
    if (is_valid()) {
      double r_tolerance = compute_tolerance(tolerance_);
      //double half_r_tolerance = 0.5 * r_tolerance;
      double angular_tolerance = get_angular_tolerance();
      double r = hypot(position_.x(), position_.y());
      double x = r - _sweep_radius_;
      double z = position_.z();
      double phi = position_.phi();
      if (has_partial_phi()) {
	if (!angle_is_in(phi,
			 get_start_phi(),
			 get_delta_phi(),
			 angular_tolerance,
			 true)) {
	  return false;
	}
      }
      circle c(_radius_);
      vector_3d p(x, z, 0.0);
      if (! c.is_on_curve(p, r_tolerance)) {
        return false;
      }
    }
    return true;
  }

  vector_3d toroid_nappe::get_normal_on_surface(const vector_3d & position_,
                                                bool check_,
                                                double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid toroid nappe!");
    vector_3d normal;
    geomtools::invalidate(normal);
    DT_THROW_IF(check_ && !is_on_surface(position_, skin_), std::logic_error,
                "Position " << position_ << " is not on surface!");
    double r = hypot(position_.x(), position_.y());
    double x = r - _sweep_radius_;
    double z = position_.z();
    double phi = position_.phi();
    double theta = std::atan2(z, x);
    double xt = _radius_ * std::cos(theta);
    double zt = _radius_ * std::sin(theta);
    double rt = xt + _sweep_radius_;
    vector_3d t(rt * std::cos(phi), rt * std::sin(phi), zt);
    normal = (position_ - t).unit();
    return normal;
  }

  bool toroid_nappe::find_intercept(const vector_3d & from_,
                                    const vector_3d & direction_,
                                    face_intercept_info & intercept_,
                                    double tolerance_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid toroid nappe!");
    intercept_.reset();
    double tolerance = compute_tolerance(tolerance_);
    double p0 = from_.x();
    double p1 = from_.y();
    double p2 = from_.z();
    double d0 = direction_.x();
    double d1 = direction_.y();
    double d2 = direction_.z();

    double a2 = d0 * d0 + d1 * d1;
    double b2 = 2 * (d0 * p0 + d1 * p1);
    double c2 = p0 * p0 + p1 * p1;
    double a1 = a2 + d2 * d2;
    double b1 = b2 + 2 * (d2 * p2);
    double R2 = _sweep_radius_ * _sweep_radius_;
    double c1 = c2 + p2 * p2 + R2 - _radius_ * _radius_;
    a2 *= (R2 * 4);
    b2 *= (R2 * 4);
    c2 *= (R2 * 4);

    // Equation:
    //  a.x^4 + b.x^3 + c.x^2 + d.x + e = 0
    double a = a1 * a1;
    double b = 2 * a1 * b1;
    double c = b1 * b1 + 2 * a1 * c1 - a2;
    double d = 2 * b1 * c1 - b2;
    double e = c1 * c1 - c2;
    std::vector<double> polynomial_coeffs;
    polynomial_coeffs.reserve(5);
    polynomial_coeffs.push_back(e);
    polynomial_coeffs.push_back(d);
    polynomial_coeffs.push_back(c);
    polynomial_coeffs.push_back(b);
    polynomial_coeffs.push_back(a);
    mygsl::polynomial p(polynomial_coeffs);
    mygsl::polynomial::solver ps(5);
    if (ps.solve(p)) {
      std::vector<double> sols;
      // Extract only real solutions:
      unsigned int nsols = ps.fetch_real_solutions(sols, 100 * std::numeric_limits<double>::epsilon());
      vector_3d impact;
      if (nsols > 0) {
        // Choose the impact point which is closest to the source:
        for (int isol = 0; isol < (int) nsols; isol++) {
          geomtools::invalidate(impact);
          double t = sols[isol];
          if (t >= 0.0) {
            impact.set(p0 + t * d0, p1 + t * d1, p2 + t * d2);
            if (! is_on_surface(impact, tolerance)) {
              geomtools::invalidate(impact);
            } else {
              if (! intercept_.is_valid()) {
                intercept_.grab_face_id().set_face_bit(FACE_UNIQUE);
                intercept_.set_impact(impact);
              } else {
                double l0 = (intercept_.get_impact() - from_).mag();
                double l1 = (impact - from_).mag();
                if (l1 < l0) {
                  intercept_.grab_face_id().set_face_bit(FACE_UNIQUE);
                  intercept_.set_impact(impact);
                }
              }
            }
          }
        }
      }
    }
    return intercept_.is_ok();
  }

  void toroid_nappe::tree_dump(std::ostream & out_,
                                   const std::string & title_,
                                   const std::string & indent_,
                                   bool inherit_) const
  {
    i_object_3d::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Sweep radius : ";
    if (datatools::is_valid(_sweep_radius_)) {
      out_ << _sweep_radius_ / CLHEP::mm << " mm";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Radius : ";
    if (datatools::is_valid(_radius_)) {
      out_ << _radius_ / CLHEP::mm << " mm";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Start phi : ";
    if (has_start_phi()) {
      out_ << get_start_phi() / CLHEP::degree << " degree";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Delta phi : ";
    if (has_delta_phi()) {
      out_ << get_delta_phi() / CLHEP::degree << " degree";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    return;
  }

  void toroid_nappe::generate_wires_self(wires_type & wires_, uint32_t options_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid toroid nappe!");
    bool no_start_phi_edge   = (options_ & WR_TOROID_NAPPE_NO_START_PHI_EDGE);
    bool no_stop_phi_edge    = (options_ & WR_TOROID_NAPPE_NO_STOP_PHI_EDGE);

    // Keep only base rendering bits:
    uint32_t base_options = options_ & WR_BASE_MASK;

    uint32_t nsamples_phi = 0;
    if (has_partial_phi()) {
      nsamples_phi = 2;
    }
    nsamples_phi = angular_sampling_from_options(base_options);

    double start_phi = 0.0;
    double delta_phi = 2.0 * M_PI;
    if (has_partial_phi()) {
      start_phi = get_start_phi();
      delta_phi = get_delta_phi();
    }
    double dphi = delta_phi / nsamples_phi;

    // Draw meridian arcs:
    {
      uint32_t options = base_options;
      circle arc(_radius_);
      for (int i_arc = 0; i_arc <= (int) nsamples_phi; i_arc++) {
        if (i_arc == 0) {
          if (has_partial_phi()) {
            if (no_start_phi_edge) continue;
          }
        }
        if (i_arc == (int) nsamples_phi) {
          if (has_partial_phi()) {
            if (no_stop_phi_edge) continue;
          } else {
            continue;
          }
        }
        double phi_arc = start_phi + i_arc * dphi;
        double x_arc = _sweep_radius_ * std::cos(phi_arc);
        double y_arc = _sweep_radius_ * std::sin(phi_arc);
        double z_arc = 0.0;
        placement arc_p;
        arc_p.set(x_arc,
                  y_arc,
                  z_arc,
                  90.0 * CLHEP::degree,
                  phi_arc,
                  0.0 * CLHEP::degree,
                  EULER_ANGLES_XYZ);
        arc.generate_wires(wires_, arc_p, options);
      }
    }

    // Draw parallel arcs:
    {
      uint32_t options = base_options;
      double start_theta = 0.0;
      double delta_theta = 2.0 * M_PI;
      uint32_t nsamples_theta = angular_sampling_from_options(base_options);
      double dtheta = delta_theta / nsamples_theta;
      for (int i_arc = 0; i_arc < (int) nsamples_theta; i_arc++) {
        double theta_arc = start_theta + i_arc * dtheta;
        double z_arc = _radius_ * std::sin(theta_arc);
        double r_arc = _sweep_radius_ + _radius_ * std::cos(theta_arc);
        circle arc(r_arc);
        if (has_partial_phi()) {
          arc.set_start_angle(get_start_phi());
          arc.set_delta_angle(get_delta_phi());
        }
        placement arc_p(0.0, 0.0, z_arc);
        arc.generate_wires(wires_, arc_p, options);
      }
    }

    return;
  }

} // end of namespace geomtools
