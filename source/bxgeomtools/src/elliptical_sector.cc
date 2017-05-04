// elliptical_sector.cc

// Ourselves:
#include <geomtools/elliptical_sector.h>

// Standard library:
#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>

// Third party:
// - GSL:
#include <gsl/gsl_poly.h>
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/units.h>

// This project:
#include <geomtools/ellipse.h>
#include <geomtools/line_3d.h>

namespace geomtools {

  // Registration :
  GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(elliptical_sector, "geomtools::elliptical_sector")

  const std::string & elliptical_sector::elliptical_sector_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "elliptical_sector";
    }
    return label;
  }

  void elliptical_sector::_set_defaults()
  {
    datatools::invalidate(_x_radius_);
    datatools::invalidate(_y_radius_);
    _angle_domain_.set_type(angular_range::RANGE_TYPE_AZIMUTHAL);
    _angle_domain_.reset_partial_angles();
    return;
  }

  std::string elliptical_sector::get_shape_name () const
  {
    return elliptical_sector_label();
  }


  double elliptical_sector::get_x_radius() const
  {
    return _x_radius_;
  }

  void elliptical_sector::set_x_radius(double a_radius)
  {
    DT_THROW_IF (a_radius <= 0.0, std::domain_error,
                 "Invalid '" << a_radius << "' X radius value !");
    _x_radius_ = a_radius;
    return;
  }

  double elliptical_sector::get_y_radius() const
  {
    return _y_radius_;
  }

  void elliptical_sector::set_y_radius(double a_radius)
  {
    DT_THROW_IF (a_radius <= 0.0, std::domain_error,
                 "Invalid '" << a_radius << "' Y radius value !");
    _y_radius_ = a_radius;
    return;
  }

  void elliptical_sector::set_radii(double x_radius_, double y_radius_)
  {
    set_x_radius(x_radius_);
    set_y_radius(y_radius_);
    return;
  }

  const angular_range &elliptical_sector::get_angle_domain() const
  {
    return _angle_domain_;
  }

  bool elliptical_sector::has_partial_angle() const
  {
    return _angle_domain_.is_partial();
  }

  bool elliptical_sector::has_start_angle() const
  {
    return _angle_domain_.has_start_angle();
  }

  void elliptical_sector::set_start_angle(double new_value_)
  {
    _angle_domain_.set_start_angle(new_value_);
    return;
  }

  double elliptical_sector::get_start_angle() const
  {
    return _angle_domain_.get_start_angle();
  }

  bool elliptical_sector::has_delta_angle() const
  {
    return _angle_domain_.has_delta_angle();
  }

  void elliptical_sector::set_delta_angle(double new_value_)
  {
    _angle_domain_.set_delta_angle(new_value_);
    return;
  }

  double elliptical_sector::get_delta_angle() const
  {
    return _angle_domain_.get_delta_angle();
  }

  double elliptical_sector::get_perimeter(uint32_t /* flags_ */) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid elliptical sector!");
    double p;
    datatools::invalidate(p);
    return p;
  }

  double elliptical_sector::get_surface (uint32_t /* flags_ */) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid elliptical sector!");
    double surface;
    datatools::invalidate(surface);
    return surface;
  }

  bool elliptical_sector::is_valid () const
  {
    if (! datatools::is_valid(_x_radius_)) {
      return false;
    }
    if (! datatools::is_valid(_y_radius_)) {
      return false;
    }
    return _angle_domain_.is_valid();
  }

  void elliptical_sector::initialize(const datatools::properties & config_,
                                     const handle_dict_type * objects_)
  {
    if (!is_valid()) {
      this->i_object_3d::_initialize(config_, objects_);

      double lunit = CLHEP::mm;
      if (config_.has_key("length_unit")) {
        const std::string lunit_str = config_.fetch_string("length_unit");
        lunit = datatools::units::get_length_unit_from(lunit_str);
      }

      DT_THROW_IF(! config_.has_key("x_radius"), std::logic_error, "Missing elliptical sector 'x_radius' property !");
      double x_radius = config_.fetch_real("x_radius");
      if (! config_.has_explicit_unit("x_radius")) {
        x_radius *= lunit;
      }

      DT_THROW_IF(! config_.has_key("y_radius"), std::logic_error, "Missing elliptical sector 'y_radius' property !");
      double y_radius = config_.fetch_real("y_radius");
      if (! config_.has_explicit_unit("y_radius")) {
        y_radius *= lunit;
      }

      set_radii(x_radius, y_radius);

      datatools::properties angle_config;
      config_.export_and_rename_starting_with(angle_config, "angle.", "");
      if (angle_config.size()) {
        _angle_domain_.initialize(angle_config);
      } else {
        // Deprecated:
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

    return;
  }

  void elliptical_sector::reset()
  {
    _set_defaults();
    this->i_object_3d::reset();
    return;
  }

  void elliptical_sector::set(double x_radius_, double y_radius_)
  {
    reset();
    set_x_radius(x_radius_);
    set_y_radius(y_radius_);
    return;
  }

  void elliptical_sector::set(double x_radius_, double y_radius_,
                              double start_angle_, double delta_angle_)
  {
    reset();
    set_x_radius(x_radius_);
    set_y_radius(y_radius_);
    _angle_domain_.set_partial_angles(start_angle_, delta_angle_);
    return;
  }

  void elliptical_sector::compute_edge(ellipse & edge_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid elliptical sector!");
    edge_.reset();
    if (is_valid()) {
      edge_.set_x_radius(_x_radius_);
      edge_.set_y_radius(_y_radius_);
      if (has_partial_angle()) {
        edge_.set_start_angle(get_start_angle());
        edge_.set_delta_angle(get_delta_angle());
      }
    }
    return;
  }

  elliptical_sector::elliptical_sector ()
  {
    _set_defaults();
    return;
  }

  elliptical_sector::elliptical_sector(double x_radius_, double y_radius_)
  {
    _set_defaults();
    set_x_radius(x_radius_);
    set_y_radius(y_radius_);
    return;
  }

  elliptical_sector::elliptical_sector(double x_radius_, double y_radius_,
                                       double start_angle_, double delta_angle_)
  {
    _set_defaults();
    set_x_radius(x_radius_);
    set_y_radius(y_radius_);
    _angle_domain_.set_partial_angles(start_angle_, delta_angle_);
    return;
  }

  elliptical_sector::~elliptical_sector ()
  {
    return;
  }

  bool elliptical_sector::is_on_surface (const vector_3d & position_,
                                         double tolerance_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid elliptical sector!");
    double z_tolerance = compute_tolerance(tolerance_);
    double half_z_tolerance = 0.5 * z_tolerance;
    double angular_tolerance = get_angular_tolerance();
    double z = position_.z();
    if (std::abs(z) > half_z_tolerance) return false;
    double x = position_.x();
    double y = position_.y();
    double r = hypot(x, y);
    double theta = std::atan2(y, x);
    if (has_partial_angle()) {
      if (! angle_is_in(theta, get_start_angle(), get_delta_angle(), angular_tolerance, true)) {
        return false;
      }
    }
    ellipse edge;
    edge.set_radii(_x_radius_, _y_radius_);
    if (has_partial_angle()) {
      edge.set_start_angle(get_start_angle());
      edge.set_delta_angle(get_delta_angle());
    }

    // Compute the edge;
    double xt, yt;
    edge.compute_x_y_from_theta(theta, xt, yt);
    double rt = hypot(xt, yt);
    if (r  > rt) {
      return false;
    }
    return true;
  }

  vector_3d elliptical_sector::get_normal_on_surface(const vector_3d & position_,
                                                     bool check_,
                                                     double skin_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid elliptical sector!");
    vector_3d normal;
    geomtools::invalidate(normal);
    DT_THROW_IF(check_ && !is_on_surface(position_, skin_), std::logic_error,
                "Position " << position_ << " is not on surface!");
    normal.set(0.0, 0.0, 1.0);
    return normal;
  }

  bool elliptical_sector::find_intercept(const vector_3d & from_,
                                         const vector_3d & direction_,
                                         face_intercept_info & intercept_,
                                         double tolerance_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid elliptical sector!");
    double tolerance = compute_tolerance(tolerance_);

    double ux = direction_.x();
    double uy = direction_.y();
    double uz = direction_.z();
    double xf = from_.x();
    double yf = from_.y();
    double zf = from_.z();

    intercept_.reset();
    if (!is_valid()) {
      return intercept_.is_ok();
    }

    if (uz == 0.0) {
      return intercept_.is_ok();
    }

    intercept_.reset();
    double lambda = - zf / uz;
    if (lambda < 0.0) {
      return intercept_.is_ok();
    }
    double xi, yi, zi;
    xi = xf + lambda * ux;
    yi = yf + lambda * uy;
    zi = 0.0;
    vector_3d impact(xi, yi, zi);
    if (! is_on_surface(impact, tolerance)) {
      return intercept_.is_ok();
    } else {
      // int face = FACE_UP;
      // if (from_.z()  < 0.0) {
      //   face = FACE_DOWN;
      // }
      intercept_.grab_face_id().set_face_bit(FACE_UNIQUE);
      intercept_.set_impact(impact);
    }
    return intercept_.is_ok();
  }

  void elliptical_sector::tree_dump (std::ostream & out_,
                                     const std::string & title_,
                                     const std::string & indent_,
                                     bool inherit_) const
  {
    i_object_3d::tree_dump (out_, title_, indent_, true);

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "X radius : " << get_x_radius() / CLHEP::mm << " mm" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Y radius : " << get_y_radius() / CLHEP::mm << " mm" << std::endl;

    {
      out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
           << "Angle domain : " << std::endl;
      std::ostringstream indent2;
      indent2 << indent_ << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
      _angle_domain_.tree_dump(out_, "", indent2.str());
    }

    return;
  }

  void elliptical_sector::generate_wires_self(wires_type & wires_,
                                              uint32_t options_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid elliptical sector!");
    bool devel = false;
    // devel = true;
    bool no_ext_edge = (options_ & WR_ELLIPTICAL_SECTOR_NO_EXT_EDGE);
    bool no_start_angle_edge = (options_ & WR_ELLIPTICAL_SECTOR_NO_START_ANGLE_EDGE);
    bool no_stop_angle_edge  = (options_ & WR_ELLIPTICAL_SECTOR_NO_STOP_ANGLE_EDGE);

    uint32_t base_options = options_ & WR_BASE_MASK;

    uint32_t nsamples_r = linear_sampling_from_options(base_options);
    double rxmin = 0.0;
    double rymin = 0.0;
    double rxmax = get_x_radius();
    double rymax = get_y_radius();
    double drx = (rxmax - rxmin) / (nsamples_r - 1);
    double dry = (rymax - rymin) / (nsamples_r - 1);
    // double dr = std::min(drx, dry);
    double theta_min = 0.0;
    double theta_max = 2 * M_PI;
    if (has_partial_angle()) {
      theta_min = get_start_angle();
      theta_max = theta_min + get_delta_angle();
      if (devel) std::cerr << "DEVEL: elliptical_sector::generate_wires_self: theta_min = " << theta_min / CLHEP::degree << std::endl;
      if (devel) std::cerr << "DEVEL: elliptical_sector::generate_wires_self: theta_max = " << theta_max / CLHEP::degree << std::endl;
    }
    for (size_t ir = 1; ir < nsamples_r; ir++) {
      if (no_ext_edge && (ir == (nsamples_r-1))) continue;
      double rxi = rxmin + ir * drx;
      double ryi = rymin + ir * dry;
      ellipse ellarc(rxi, ryi);
      if (has_partial_angle()) {
        ellarc.set_start_angle(get_start_angle());
        ellarc.set_delta_angle(get_delta_angle());
      }
      ellarc.generate_wires_self(wires_, base_options);
    }

    size_t nsamples_th = 0;
    if (has_partial_angle()) {
      nsamples_th = 1;
    }
    if (options_ & i_wires_3d_rendering::WR_BASE_GRID) {
      nsamples_th = i_wires_3d_rendering::angular_sampling_from_options(base_options);
    }
    if (nsamples_th > 0) {
      double dtheta = (theta_max - theta_min) / (nsamples_th);
      size_t nth_max = nsamples_th;
      if (has_partial_angle()) {
        nth_max++;
      }
      for (size_t it = 0; it < nth_max; it++) {
        if (has_partial_angle()) {
          if (no_start_angle_edge && it == 0) continue;
          if (no_stop_angle_edge && it == (nsamples_th - 1)) continue;
        }
        double thetai = theta_min + it * dtheta;
        double rx_min = rxmin;
        double ry_min = rymin;
        if (devel) std::cerr << "DEVEL: elliptical_sector::generate_wires_self:   thetai = "
                             << thetai / CLHEP::degree << " degree" << std::endl;
        {
          size_t nth = nsamples_th;
          int factor = 1;
          // factor = 4;
          if (devel) std::cerr << "DEVEL: elliptical_sector::generate_wires_self:   nth = "
                               << nth << std::endl;
          unsigned int ref_nsampling = angular_sampling(SL_LOW);
          ref_nsampling /= factor;
          int factor2 = 1;
          if (nth > ref_nsampling) {
            int scale = 1;
            while (true) {
              int f = nth / (scale * ref_nsampling);
              if (f == 0) {
                break;
              }
              int itest = (it % f);
              if (itest != 0) {
                rx_min = rxmin + factor2 * scale * drx;
                ry_min = rymin + factor2 * scale * dry;
              }
              scale *= 2;
            }
          }
        }
        if (devel) {
          std::cerr << "DEVEL: elliptical_sector::generate_wires_self: rx_min="
                    << rx_min << std::endl;
          std::cerr << "DEVEL: elliptical_sector::generate_wires_self: ry_min="
                    << ry_min << std::endl;
        }
        vector_3d v1, v2;
        if (rx_min > 0.1 * drx && ry_min > 0.1 * dry) {
          ellipse int_edge(rx_min, ry_min);
          double x, y;
          int_edge.compute_x_y_from_theta(thetai, x, y);
          v1.set(x, y, 0.0);
        } else {
          v1.set(0.0, 0.0, 0.0);
        }
        if (devel) std::cerr << "DEVEL: elliptical_sector::generate_wires_self: v1="
                             << v1 << std::endl;
        {
          ellipse ext_edge(_x_radius_, _y_radius_);
          double x, y;
          ext_edge.compute_x_y_from_theta(thetai, x, y);
          v2.set(x, y, 0.0);
        }
        if (devel) std::cerr << "DEVEL: elliptical_sector::generate_wires_self: v2="
                             << v2 << std::endl;
        {
          line_3d radial(v1, v2);
          radial.generate_wires_self(wires_, 0);
          if (devel) std::cerr << "DEVEL: elliptical_sector::generate_wires_self: radial OK"
                               << std::endl;
        }
      }
    }
    if (devel) std::cerr << "DEVEL: elliptical_sector::generate_wires_self: Exiting."
                         << std::endl;
    return;
  }

} // end of namespace geomtools
