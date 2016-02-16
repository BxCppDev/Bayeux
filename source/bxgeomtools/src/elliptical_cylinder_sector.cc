// elliptical_cylinder_sector.cc

// Ourselves:
#include <geomtools/elliptical_cylinder_sector.h>

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
#include <geomtools/line_3d.h>

namespace geomtools {

  // Registration :
  GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(elliptical_cylinder_sector, "geomtools::elliptical_cylinder_sector");

  // static
  const std::string & elliptical_cylinder_sector::elliptical_cylinder_sector_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "elliptical_cylinder_sector";
    }
    return label;
  }

  std::string elliptical_cylinder_sector::get_shape_name() const
  {
    return elliptical_cylinder_sector::elliptical_cylinder_sector_label();
  }

  void elliptical_cylinder_sector::_set_defaults()
  {
    datatools::invalidate(_x_radius_);
    datatools::invalidate(_y_radius_);
    datatools::invalidate(_z_);
    datatools::invalidate(_start_angle_);
    datatools::invalidate(_delta_angle_);
    return;
  }

  void elliptical_cylinder_sector::set_x_radius(double new_value_)
  {
    DT_THROW_IF (new_value_ <= 0.0, std::domain_error, "Invalid X radius value!");
    _x_radius_ = new_value_;
    return;
  }

  double elliptical_cylinder_sector::get_x_radius() const
  {
    return _x_radius_;
  }

  void elliptical_cylinder_sector::set_y_radius(double new_value_)
  {
    DT_THROW_IF (new_value_ <= 0.0, std::domain_error, "Invalid Y radius value!");
    _y_radius_ = new_value_;
    return;
  }

  double elliptical_cylinder_sector::get_y_radius() const
  {
    return _y_radius_;
  }

  void elliptical_cylinder_sector::set_z(double new_value_)
  {
    DT_THROW_IF (new_value_ <= 0.0, std::domain_error, "Invalid Z value!");
    _z_ = new_value_;
    return;
  }

  double elliptical_cylinder_sector::get_z() const
  {
    return _z_;
  }

  void elliptical_cylinder_sector::set(double rx_, double ry_, double z_)
  {
    set_x_radius(rx_);
    set_y_radius(ry_);
    set_z(z_);
    return;
  }

  void elliptical_cylinder_sector::set(double rx_, double ry_, double z_,
                                       double start_angle_, double delta_angle_)
  {
    set(rx_, ry_, z_);
    set_start_angle(start_angle_);
    set_delta_angle(delta_angle_);
    return;
  }

  bool elliptical_cylinder_sector::has_partial_angle() const
  {
    if (has_start_angle() && has_delta_angle()) {
      return true;
    }
    return false;
  }

  bool elliptical_cylinder_sector::has_start_angle() const
  {
    return datatools::is_valid(_start_angle_);
  }

  void elliptical_cylinder_sector::set_start_angle(double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0 || new_value_ > 2 * M_PI,
                 std::domain_error,
                 "Invalid '" << new_value_ << "' start angle value !");
    _start_angle_ = new_value_;
    return;
  }

  double elliptical_cylinder_sector::get_start_angle() const
  {
    return _start_angle_;
  }

  bool elliptical_cylinder_sector::has_delta_angle() const
  {
    return datatools::is_valid(_delta_angle_);
  }

  void elliptical_cylinder_sector::set_delta_angle(double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0 || new_value_ > 2 * M_PI,
                 std::domain_error,
                 "Invalid '" << new_value_ << "' delta angle value !");
    _delta_angle_ = new_value_;
    return;
  }

  double elliptical_cylinder_sector::get_delta_angle() const
  {
    return _delta_angle_;
  }

  bool elliptical_cylinder_sector::is_valid() const
  {
    if (!datatools::is_valid(_x_radius_)) {
      return false;
    }
    if (!datatools::is_valid(_y_radius_)) {
      return false;
    }
    if (!datatools::is_valid(_z_)) {
      return false;
    }
    if (has_start_angle()) {
      if (!has_delta_angle()) {
        return false;
      }
    }
    return true;
  }

  elliptical_cylinder_sector::elliptical_cylinder_sector()
  {
    _set_defaults();
    return;
  }

  elliptical_cylinder_sector::elliptical_cylinder_sector(double x_radius_, double y_radius_, double z_)
  {
    _set_defaults();
    set_x_radius(x_radius_);
    set_y_radius(y_radius_);
    set_z(z_);
    return;
  }

  elliptical_cylinder_sector::elliptical_cylinder_sector(double x_radius_,
                                                         double y_radius_,
                                                         double z_,
                                                         double start_angle_,
                                                         double delta_angle_)
  {
    _set_defaults();
    set_x_radius(x_radius_);
    set_y_radius(y_radius_);
    set_z(z_);
    set_start_angle(start_angle_);
    set_delta_angle(delta_angle_);
    return;
  }

  elliptical_cylinder_sector::~elliptical_cylinder_sector()
  {
    return;
  }

  void elliptical_cylinder_sector::initialize(const datatools::properties & config_, const handle_dict_type * objects_)
  {
    if (!is_valid()) {
      this->i_object_3d::_initialize(config_, objects_);

      double lunit = CLHEP::mm;
      if (config_.has_key("length_unit")) {
        const std::string lunit_str = config_.fetch_string("length_unit");
        lunit = datatools::units::get_length_unit_from(lunit_str);
      }

      DT_THROW_IF(! config_.has_key("x_radius"), std::logic_error, "Missing elliptical cylinder sector 'x_radius' property !");
      double x_radius = config_.fetch_real("x_radius");
      if (! config_.has_explicit_unit("x_radius")) {
        x_radius *= lunit;
      }

      DT_THROW_IF(! config_.has_key("y_radius"), std::logic_error, "Missing elliptical cylinder sector 'y_radius' property !");
      double y_radius = config_.fetch_real("y_radius");
      if (! config_.has_explicit_unit("y_radius")) {
        y_radius *= lunit;
      }

      DT_THROW_IF(! config_.has_key("z"), std::logic_error, "Missing elliptical cylinder sector 'z' property !");
      double z = config_.fetch_real("z");
      if (! config_.has_explicit_unit("z")) {
        z *= lunit;
      }

      set(x_radius, y_radius, z);

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

    return;
  }

  void elliptical_cylinder_sector::reset()
  {
    _set_defaults();
    this->i_object_3d::reset();
    return;
  }

  double elliptical_cylinder_sector::get_surface(uint32_t /* flags_ */) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid elliptical cylinder sector!");
    double s;
    datatools::invalidate(s);
    return s;
  }

  bool elliptical_cylinder_sector::is_on_surface(const vector_3d & position_,
                                                 double tolerance_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid elliptical cylinder sector!");
    double tolerance = compute_tolerance(tolerance_);
    double angular_tolerance = get_angular_tolerance();
    if (is_valid()) {
      double z = position_.z();
      if (std::abs(z) > 0.5 * _z_) {
        return false;
      }
      if (has_partial_angle()) {
        double angle = position_.phi();
        if (!angle_is_in(angle,
                         _start_angle_,
                         _delta_angle_,
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
        ellipse ell(a, b);
        if (has_partial_angle()) {
          ell.set_start_angle(_start_angle_);
          ell.set_delta_angle(_delta_angle_);
        }
        vector_3d pos(x, y, 0.0);
        if (ell.is_on_curve(pos, tolerance)) {
          return true;
        }
      }
    }
    return false;
  }

  vector_3d elliptical_cylinder_sector::get_normal_on_surface(const vector_3d & position_,
                                                              bool check_,
                                                              double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid elliptical cylinder sector!");
    vector_3d normal;
    geomtools::invalidate(normal);
    DT_THROW_IF(check_ && !is_on_surface(position_, skin_), std::logic_error,
                "Position " << position_ << " is not on surface!");
    normal.set(position_.x() / _x_radius_,
               position_.y() / _y_radius_,
               0.0);
    normal /= normal.mag();
    return normal;
  }

  bool elliptical_cylinder_sector::find_intercept(const vector_3d & from_,
                                                  const vector_3d & direction_,
                                                  face_intercept_info & intercept_,
                                                  double tolerance_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid elliptical cylinder sector!");
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
    double a = gsl_pow_2(d0/rx) + gsl_pow_2(d1/ry);
    double b = 2 * (d0 * p0 / gsl_pow_2(rx) + d1 * p1/ gsl_pow_2(ry));
    double c = gsl_pow_2(p0/rx) + gsl_pow_2(p1/ry) - 1.0;
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
    if (devel) intercept_.print(std::cerr, "DEVEL: elliptical_cylinder_sector::find_intercept: ");
    if (devel) std::cerr << "" << std::endl;
    return intercept_.is_ok();
  }

  void elliptical_cylinder_sector::tree_dump(std::ostream & out_,
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

  void elliptical_cylinder_sector::generate_wires_self(wires_type & wires_,
                                                       uint32_t options_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid elliptical cylinder sector!");
    bool devel = false;
    // devel  = true;
    bool no_bottom_edge = (options_ & WR_ELLCYLSEC_NO_BOTTOM_EDGE);
    bool no_top_edge     = (options_ & WR_ELLCYLSEC_NO_TOP_EDGE);
    bool no_start_angle_edge  = (options_ & WR_ELLCYLSEC_NO_START_ANGLE_EDGE);
    bool no_stop_angle_edge   = (options_ & WR_ELLCYLSEC_NO_STOP_ANGLE_EDGE);

    // Keep only base rendering bits:
    uint32_t base_options = options_ & WR_BASE_MASK;

    double start_angle = 0.0;
    double delta_angle = 2.0 * M_PI;
    if (has_partial_angle()) {
      start_angle = _start_angle_;
      delta_angle = _delta_angle_;
    }
    unsigned int nsamples_angle = angular_sampling_from_options(base_options);
    double dangle = delta_angle / nsamples_angle;

    // Draw parallel arcs:
    {
      if (devel) std::cerr << "DEVEL: elliptical_cylinder_sector::generate_wires_self: Draw parallel arcs" << std::endl;
      uint32_t options = base_options;
      double z0 = -0.5 * _z_;
      double z1 = +0.5 * _z_;
      uint32_t nsamples_z = linear_sampling_from_options(base_options);
      if (devel) std::cerr << "DEVEL: elliptical_cylinder_sector::generate_wires_self: nsamples_z=" << nsamples_z << std::endl;
      double dz = (z1 - z0) / (nsamples_z - 1);
      // if (devel) std::cerr << "DEVEL: dz=" << dz / CLHEP::mm << " mm" << std::endl;
      for (size_t iz = 0; iz < nsamples_z; iz++) {
        if (no_bottom_edge && iz == 0) continue;
        if (no_top_edge && iz == nsamples_z) continue;
        double z = z0 + iz * dz;
        ellipse parallel(_x_radius_, _y_radius_);
        if (has_partial_angle()) {
          parallel.set_start_angle(start_angle);
          parallel.set_delta_angle(delta_angle);
        }
        placement parallel_placement(0.0, 0.0, z);
        parallel.generate_wires(wires_, parallel_placement, options);
      }
    }

    // Draw meridian lines:
    {
      if (devel) std::cerr << "DEVEL: elliptical_cylinder_sector::generate_wires_self: Draw meridian lines" << std::endl;
      uint32_t options = base_options;
      uint32_t max_nangle = nsamples_angle;
      if (has_partial_angle()) {
        max_nangle++;
      }
      if (devel) std::cerr << "DEVEL: elliptical_cylinder_sector::generate_wires_self: max_nangle=" << max_nangle << std::endl;
      ellipse xy_ell(_x_radius_, _y_radius_);
      for (uint32_t iangle = 0; iangle < max_nangle ; ++iangle) {
        if (has_partial_angle()) {
          if ((iangle == 0) && no_start_angle_edge) continue;
          if ((iangle == (max_nangle - 1)) && no_stop_angle_edge) continue;
        }
        double angle = start_angle + iangle * dangle;
        double x0, y0;
        xy_ell.compute_x_y_from_theta(angle, x0, y0);
        // double b = hypot(x0, y0);
        line_3d meridian(vector_3d(x0, y0, -0.5 * _z_),
                         vector_3d(x0, y0, +0.5 * _z_));
        double start_theta = 0.0;
        double stop_theta  = M_PI;
        if (devel) std::cerr << "DEVEL: elliptical_cylinder_sector::generate_wires_self: start_theta = " << start_theta / CLHEP::degree << " degree" << std::endl;
        if (devel) std::cerr << "DEVEL: elliptical_cylinder_sector::generate_wires_self: stop_theta  = " << stop_theta / CLHEP::degree << " degree" << std::endl;
        meridian.generate_wires_self(wires_, options);
      }
    }

    if (devel) std::cerr << "DEVEL: elliptical_cylinder_sector::generate_wires_self: Done" << std::endl;
    return;
  }

} // end of namespace geomtools
