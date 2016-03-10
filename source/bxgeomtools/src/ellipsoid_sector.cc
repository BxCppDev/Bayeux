// ellipsoid_sector.cc

// Ourselves:
#include <geomtools/ellipsoid_sector.h>

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
#include <geomtools/ellipse.h>
#include <geomtools/utils.h>

namespace geomtools {

  // Registration :
  GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(ellipsoid_sector, "geomtools::ellipsoid_sector")

  // static
  const std::string & ellipsoid_sector::ellipsoid_sector_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "ellipsoid_sector";
    }
    return label;
  }

  std::string ellipsoid_sector::get_shape_name() const
  {
    return ellipsoid_sector::ellipsoid_sector_label();
  }

  void ellipsoid_sector::_set_defaults()
  {
    datatools::invalidate(_x_radius_);
    datatools::invalidate(_y_radius_);
    datatools::invalidate(_z_radius_);
    datatools::invalidate(_bottom_z_cut_);
    datatools::invalidate(_top_z_cut_);
    _angle_domain_.set_type(angular_range::RANGE_TYPE_AZIMUTHAL);
    _angle_domain_.reset_partial_angles();
    return;
  }

  void ellipsoid_sector::set_x_radius(double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0, std::domain_error, "Invalid X radius value!");
    _x_radius_ = new_value_;
    return;
  }

  double ellipsoid_sector::get_x_radius() const
  {
    return _x_radius_;
  }

  void ellipsoid_sector::set_y_radius(double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0, std::domain_error, "Invalid Y radius value!");
    _y_radius_ = new_value_;
    return;
  }

  double ellipsoid_sector::get_y_radius() const
  {
    return _y_radius_;
  }

  void ellipsoid_sector::set_z_radius(double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0, std::domain_error, "Invalid Z radius value!");
    _z_radius_ = new_value_;
    return;
  }

  double ellipsoid_sector::get_z_radius() const
  {
    return _z_radius_;
  }

  void ellipsoid_sector::set_bottom_z_cut(double new_value_)
  {
    DT_THROW_IF (new_value_ < -_z_radius_, std::domain_error, "Invalid bottom Z cut value!");
    _bottom_z_cut_ = new_value_;
    return;
  }

  double ellipsoid_sector::get_bottom_z_cut() const
  {
    return _bottom_z_cut_;
  }

  void ellipsoid_sector::set_top_z_cut(double new_value_)
  {
    DT_THROW_IF (new_value_ > _z_radius_, std::domain_error, "Invalid top Z cut value!");
    _top_z_cut_ = new_value_;
    return;
  }

  double ellipsoid_sector::get_top_z_cut() const
  {
    return _top_z_cut_;
  }

  const angular_range &ellipsoid_sector::get_angle_domain() const
  {
    return _angle_domain_;
  }

  bool ellipsoid_sector::has_partial_angle() const
  {
    return _angle_domain_.is_partial();
  }

  bool ellipsoid_sector::has_start_angle() const
  {
    return _angle_domain_.has_start_angle();
  }

  void ellipsoid_sector::set_start_angle(double new_value_)
  {
    _angle_domain_.set_start_angle(new_value_);
    return;
  }

  double ellipsoid_sector::get_start_angle() const
  {
    return _angle_domain_.get_start_angle();
  }

  bool ellipsoid_sector::has_delta_angle() const
  {
    return _angle_domain_.has_delta_angle();
  }

  void ellipsoid_sector::set_delta_angle(double new_value_)
  {
    _angle_domain_.set_delta_angle(new_value_);
    return;
  }

  double ellipsoid_sector::get_delta_angle() const
  {
    return _angle_domain_.get_delta_angle();
  }

  bool ellipsoid_sector::is_valid() const
  {
    if (!datatools::is_valid(_x_radius_)) {
      return false;
    }
    if (!datatools::is_valid(_y_radius_)) {
      return false;
    }
    if (!datatools::is_valid(_z_radius_)) {
      return false;
    }
    return _angle_domain_.is_valid();
  }

  ellipsoid_sector::ellipsoid_sector()
  {
    _set_defaults();
    return;
  }

  ellipsoid_sector::ellipsoid_sector(double x_radius_, double y_radius_, double z_radius_)
  {
    _set_defaults();
    set_x_radius(x_radius_);
    set_y_radius(y_radius_);
    set_z_radius(z_radius_);
    return;
  }

  ellipsoid_sector::ellipsoid_sector(double x_radius_,
                                     double y_radius_,
                                     double z_radius_,
                                     double bottom_z_cut_,
                                     double top_z_cut_)
  {
    _set_defaults();
    set_x_radius(x_radius_);
    set_y_radius(y_radius_);
    set_z_radius(z_radius_);
    set_bottom_z_cut(bottom_z_cut_);
    set_top_z_cut(top_z_cut_);
    return;
  }

  ellipsoid_sector::ellipsoid_sector(double x_radius_,
                                     double y_radius_,
                                     double z_radius_,
                                     double bottom_z_cut_,
                                     double top_z_cut_,
                                     double start_angle_,
                                     double delta_angle_)
  {
    _set_defaults();
    set_x_radius(x_radius_);
    set_y_radius(y_radius_);
    set_z_radius(z_radius_);
    set_bottom_z_cut(bottom_z_cut_);
    set_top_z_cut(top_z_cut_);
    _angle_domain_.set_partial_angles(start_angle_, delta_angle_);
    return;
  }


  ellipsoid_sector::~ellipsoid_sector()
  {
    return;
  }

  void ellipsoid_sector::initialize(const datatools::properties & config_, const handle_dict_type * objects_)
  {
    if (!is_valid()) {
      this->i_object_3d::_initialize(config_, objects_);

      double lunit = CLHEP::mm;
      if (config_.has_key("length_unit")) {
        const std::string lunit_str = config_.fetch_string("length_unit");
        lunit = datatools::units::get_length_unit_from(lunit_str);
      }

      DT_THROW_IF(! config_.has_key("x_radius"), std::logic_error, "Missing ellipsoid sector 'x_radius' property !");
      double x_radius = config_.fetch_real("x_radius");
      if (! config_.has_explicit_unit("x_radius")) {
        x_radius *= lunit;
      }
      set_x_radius(x_radius);

      DT_THROW_IF(! config_.has_key("y_radius"), std::logic_error, "Missing ellipsoid sector 'y_radius' property !");
      double y_radius = config_.fetch_real("y_radius");
      if (! config_.has_explicit_unit("y_radius")) {
        y_radius *= lunit;
      }
      set_y_radius(y_radius);

      DT_THROW_IF(! config_.has_key("z_radius"), std::logic_error, "Missing ellipsoid sector 'z_radius' property !");
      double z_radius = config_.fetch_real("z_radius");
      if (! config_.has_explicit_unit("z_radius")) {
        z_radius *= lunit;
      }
      set_z_radius(z_radius);

      datatools::properties angle_config;
      config_.export_and_rename_starting_with(angle_config, "angle.", "");
      if (angle_config.size()) {
        _angle_domain_.initialize(angle_config);
      }

      if (config_.has_key("bottom_z_cut")) {
        double bottom_z_cut = config_.fetch_real("bottom_z_cut");
        if (! config_.has_explicit_unit("bottom_z_cut")) {
          bottom_z_cut *= lunit;
        }
        set_bottom_z_cut(bottom_z_cut);
      }

      if (config_.has_key("top_z_cut")) {
        double top_z_cut = config_.fetch_real("top_z_cut");
        if (! config_.has_explicit_unit("top_z_cut")) {
          top_z_cut *= lunit;
        }
        set_top_z_cut(top_z_cut);
      }

    }

    return;
  }

  void ellipsoid_sector::reset()
  {
    _set_defaults();
    return;
  }

  double ellipsoid_sector::get_surface(uint32_t /* flags_ */) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid ellipsoid sector!");
    double s;
    datatools::invalidate(s);
    // Not implemented yet !
    return s;
  }

  bool ellipsoid_sector::is_on_surface(const vector_3d & position_,
                                       double tolerance_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid ellipsoid sector!");
    double tolerance = compute_tolerance(tolerance_);
    double angular_tolerance = get_angular_tolerance();
    double half_tolerance = 0.5 * tolerance;
    double z = position_.z();
    if (datatools::is_valid(_top_z_cut_)) {
      if (z > _top_z_cut_ + half_tolerance) {
        return false;
      }
    } else {
      if (z > _z_radius_ + half_tolerance) {
        return false;
      }
    }
    if (datatools::is_valid(_bottom_z_cut_)) {
      if (z < _bottom_z_cut_ - half_tolerance) {
        return false;
      }
    } else {
      if (z < -_z_radius_ - half_tolerance) {
        return false;
      }
    }
    if (has_partial_angle()) {
      double phi = position_.phi();
      if (!::geomtools::angle_is_in(phi,
                                    _angle_domain_.get_first_angle(),
                                    _angle_domain_.get_angle_spread(),
                                    angular_tolerance,
                                    true)) {
        return false;
      }
    }
    {
      double x = position_.x();
      double y = position_.y();
      double a = _x_radius_;
      double b = _y_radius_;
      double c = _z_radius_;

      double term = std::sqrt(1.0 - gsl_pow_2(z/c));
      if (datatools::is_valid(term)) {
        double a2 = a * term;
        double b2 = b * term;
        ellipse ell(a2, b2);
        if (has_partial_angle()) {
          ell.set_start_angle(get_start_angle());
          ell.set_delta_angle(get_delta_angle());
        }
        vector_3d pos(x, y, 0.0);
        if (ell.is_on_curve(pos, tolerance)) {
          return true;
        }
      }
    }
    return false;
  }

  vector_3d ellipsoid_sector::get_normal_on_surface(const vector_3d & position_,
                                                    bool check_,
                                                    double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid ellipsoid sector!");
    vector_3d normal;
    geomtools::invalidate(normal);
    DT_THROW_IF(check_ && !is_on_surface(position_, skin_), std::logic_error,
                "Position " << position_ << " is not on surface!");
    normal.set(position_.x() / _x_radius_,
               position_.y() / _y_radius_,
               position_.z() / _z_radius_
               );
    normal /= normal.mag();
    return normal;
  }

  bool ellipsoid_sector::find_intercept(const vector_3d & from_,
                                        const vector_3d & direction_,
                                        face_intercept_info & intercept_,
                                        double tolerance_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid ellipsoid sector!");
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
    double rx = _x_radius_;
    double ry = _y_radius_;
    double rz = _z_radius_;
    double a = gsl_pow_2(d0/rx) + gsl_pow_2(d1/ry) + gsl_pow_2(d2/rz);
    double b = 2 * (d0 * p0 / gsl_pow_2(rx) + d1 * p1/ gsl_pow_2(ry) + d2 * p2/ gsl_pow_2(rz));
    double c = gsl_pow_2(p0/rx) + gsl_pow_2(p1/ry) + gsl_pow_2(p2/rz) - 1.0;
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
    if (devel) intercept_.print(std::cerr, "DEVEL: ellipsoid_sector::find_intercept: ");
    if (devel) std::cerr << "" << std::endl;
    return intercept_.is_ok();
  }

  void ellipsoid_sector::tree_dump(std::ostream & out_,
                                   const std::string & title_,
                                   const std::string & indent_,
                                   bool inherit_) const
  {
    i_object_3d::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "X radius : ";
    if (datatools::is_valid(_x_radius_)) {
      out_ << _x_radius_ / CLHEP::mm << " mm";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Y radius : ";
    if (datatools::is_valid(_y_radius_)) {
      out_ << _y_radius_ / CLHEP::mm << " mm";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Z radius : ";
    if (datatools::is_valid(_z_radius_)) {
      out_ << _z_radius_ / CLHEP::mm << " mm";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Bottom Z cut: ";

    if (datatools::is_valid(_bottom_z_cut_)) {
      out_ << _bottom_z_cut_ / CLHEP::mm << " mm";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    if (datatools::is_valid(_top_z_cut_)) {
      out_ << _top_z_cut_ / CLHEP::mm << " mm";
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

  void ellipsoid_sector::generate_wires_self(wires_type & wires_,
                                             uint32_t options_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid ellipsoid sector!");
    bool devel = false;
    //  devel  = true;
    bool no_bottom_edge = (options_ & WR_ELLSEC_NO_BOTTOM_EDGE);
    bool no_top_edge     = (options_ & WR_ELLSEC_NO_TOP_EDGE);
    bool no_start_angle_edge  = (options_ & WR_ELLSEC_NO_START_ANGLE_EDGE);
    bool no_stop_angle_edge   = (options_ & WR_ELLSEC_NO_STOP_ANGLE_EDGE);

    // Keep only base rendering bits:
    uint32_t base_options = options_ & WR_BASE_MASK;

    double start_angle = 0.0;
    double delta_angle = 2.0 * M_PI;
    if (has_partial_angle()) {
      start_angle = get_start_angle();
      delta_angle = get_delta_angle();
    }
    unsigned int nsamples_angle = angular_sampling_from_options(base_options);
    double dangle = delta_angle / nsamples_angle;

    // Draw parallel arcs:
    {
      if (devel) std::cerr << "DEVEL: ellipsoid_sector::generate_wires_self: Draw parallel arcs" << std::endl;
      uint32_t options = base_options;
      double z0 = -_z_radius_;
      if (datatools::is_valid(_bottom_z_cut_)) {
        z0 = _bottom_z_cut_;
      }
      double z1 = +_z_radius_;
      if (datatools::is_valid(_top_z_cut_)) {
        z1 = _top_z_cut_;
      }
      uint32_t nsamples_z = linear_sampling_from_options(base_options);
      if (devel) std::cerr << "DEVEL: ellipsoid_sector::generate_wires_self: nsamples_z=" << nsamples_z << std::endl;
      double dz = (z1 - z0) / (nsamples_z - 1);
      // if (devel) std::cerr << "DEVEL: dz=" << dz / CLHEP::mm << " mm" << std::endl;
      for (size_t iz = 0; iz < nsamples_z; iz++) {
        if (no_bottom_edge && iz == 0) continue;
        if (no_top_edge && iz == nsamples_z) continue;
        double z = z0 + iz * dz;
        double term = std::sqrt(1 - gsl_pow_2(z/_z_radius_));
        if (term > 1.e-7) {
          ellipse parallel(_x_radius_ * term, _y_radius_ * term);
          if (has_partial_angle()) {
            parallel.set_start_angle(start_angle);
            parallel.set_delta_angle(delta_angle);
          }
          placement parallel_placement(0.0, 0.0, z);
          parallel.generate_wires(wires_, parallel_placement, options);
        }
      }
    }

    // Draw meridian lines:
    {
      if (devel) std::cerr << "DEVEL: ellipsoid_sector::generate_wires_self: Draw meridian lines" << std::endl;
      uint32_t options = base_options;
      uint32_t max_nangle = nsamples_angle;
      if (has_partial_angle()) {
        max_nangle++;
      }
      if (devel) std::cerr << "DEVEL: ellipsoid_sector::generate_wires_self: max_nangle=" << max_nangle << std::endl;
      ellipse xy_ell(_x_radius_, _y_radius_);
      for (uint32_t iangle = 0; iangle < max_nangle ; ++iangle) {
        if (has_partial_angle()) {
          if ((iangle == 0) && no_start_angle_edge) continue;
          if ((iangle == (max_nangle - 1)) && no_stop_angle_edge) continue;
        }
        double angle = start_angle + iangle * dangle;
        double x0, y0;
        xy_ell.compute_x_y_from_theta(angle, x0, y0);
        double a = _z_radius_;
        double b = hypot(x0, y0);
        ellipse meridian(a, b);
        double start_theta = 0.0;
        double stop_theta  = M_PI;
        if (devel) std::cerr << "DEVEL: ellipsoid_sector::generate_wires_self: _top_z_cut_    = " << _top_z_cut_ << std::endl;
        if (devel) std::cerr << "DEVEL: ellipsoid_sector::generate_wires_self: _bottom_z_cut_ = " << _bottom_z_cut_ << std::endl;
        if (datatools::is_valid(_top_z_cut_)) {
          meridian.compute_theta_from_x(_top_z_cut_, start_theta);
        }
        if (datatools::is_valid(_bottom_z_cut_)) {
          meridian.compute_theta_from_x(_bottom_z_cut_, stop_theta);
        }
        if (devel) std::cerr << "DEVEL: ellipsoid_sector::generate_wires_self: start_theta = " << start_theta / CLHEP::degree << " degree" << std::endl;
        if (devel) std::cerr << "DEVEL: ellipsoid_sector::generate_wires_self: stop_theta  = " << stop_theta / CLHEP::degree << " degree" << std::endl;
        meridian.set_start_angle(start_theta);
        meridian.set_delta_angle(stop_theta - start_theta);
        // if (devel) std::cerr << "DEVEL: ellipsoid_sector::generate_wires_self: x=" << x << std::endl;
        // if (devel) std::cerr << "DEVEL: ellipsoid_sector::generate_wires_self: y=" << y << std::endl;
        placement meridian_placement(0.0, 0.0, 0.0,
                                     -90 * CLHEP::degree,
                                     -90 * CLHEP::degree + angle,
                                     0.0,
                                     EULER_ANGLES_YXZ
                                     );
        meridian.generate_wires(wires_, meridian_placement, options);
      }
    }

    if (devel) std::cerr << "DEVEL: ellipsoid_sector::generate_wires_self: Done" << std::endl;
    return;
  }

} // end of namespace geomtools
