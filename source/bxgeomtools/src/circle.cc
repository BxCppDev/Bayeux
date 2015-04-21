// circle.cc

// Ourselves:
#include <geomtools/circle.h>

// Standard library:
#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <sstream>

// Third party:
// - GSL:
#include <gsl/gsl_poly.h>
#include <gsl/gsl_math.h>
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/utils.h>

// This project:
#include <geomtools/polyline_3d.h>

namespace geomtools {

  const std::string & circle::circle_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "circle";
    }
    return label;
  }

  std::string circle::get_shape_name() const
  {
    return circle_label();
  }

  void circle::_set_defaults()
  {
    datatools::invalidate(_radius_);
    datatools::invalidate(_start_angle_);
    datatools::invalidate(_delta_angle_);
    return;
  }

  unsigned int circle::get_number_of_path() const
  {
    return 1;
  }

  double circle::get_length(uint32_t flags_) const
  {
    if (flags_ == PATH_NONE) return 0.0;
    return get_circumference();
  }

  double circle::get_r() const
  {
    return _radius_;
  }

  double circle::get_radius() const
  {
    return get_r();
  }

  void circle::set_diameter(double a_diameter)
  {
    set_r(a_diameter * 0.5);
    return;
  }

  double circle::get_diameter() const
  {
    return(_radius_ + _radius_);
  }

  void circle::set_r(double a_radius)
  {
    set_radius(a_radius);
    return;
  }

  void circle::set_radius(double a_radius)
  {
    DT_THROW_IF (a_radius < 0.0, std::domain_error,
                 "Invalid '" << a_radius << "' R value !");
    _radius_ = a_radius;
    return;
  }

  bool circle::has_partial_angle() const
  {
    return has_start_angle() && has_delta_angle();
  }

  bool circle::has_start_angle() const
  {
    return datatools::is_valid(_start_angle_);
  }

  void circle::set_start_angle(double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0 || new_value_ >= 2 * M_PI,
                 std::domain_error,
                 "Invalid '" << new_value_ << "' start angle value !");
    _start_angle_ = new_value_;
    return;
  }

  double circle::get_start_angle() const
  {
    return _start_angle_;
  }

  bool circle::has_delta_angle() const
  {
    return datatools::is_valid(_delta_angle_);
  }

  void circle::set_delta_angle(double new_value_)
  {
    DT_THROW_IF (new_value_ < 0.0 || new_value_ > 2 * M_PI,
                 std::domain_error,
                 "Invalid '" << new_value_ << "' delta angle value !");
    _delta_angle_ = new_value_;
    return;
  }

  double circle::get_delta_angle() const
  {
    return _delta_angle_;
  }

  double circle::get_circumference() const
  {
    double circum;
    datatools::invalidate(circum);
    if (is_valid()) {
      double c = 2 * M_PI;
      if (has_partial_angle()) {
        c = _delta_angle_;
      }
      circum = c * _radius_;
    }
    return circum;
  }

  double circle::get_surface() const
  {
    double surface;
    datatools::invalidate(surface);
    if (is_valid()) {
      double angle = 2 * M_PI;
      if (has_partial_angle()) {
        angle = _delta_angle_;
      }
      surface =  0.5 * angle * _radius_ * _radius_;
    }
    return surface;
  }

  bool circle::is_valid() const
  {
    if (!datatools::is_valid(_radius_)) {
      return false;
    }
    if (has_start_angle()) {
      if (!datatools::is_valid(_delta_angle_)) {
        return false;
      }
    }
    return true;
  }

  circle::circle()
  {
    _set_defaults();
    return;
  }

  circle::circle(double a_radius)
  {
    _set_defaults();
    set_radius(a_radius);
    return;
  }

  circle::circle(double r_, double start_angle_, double delta_angle_)
  {
    _set_defaults();
    set_radius(r_);
    set_start_angle(start_angle_);
    set_delta_angle(delta_angle_);
    return;
  }

  circle::~circle()
  {
    return;
  }

  void circle::reset()
  {
    _set_defaults();
    this->i_object_3d::reset();
    return;
  }

  void circle::tree_dump(std::ostream & out_,
                         const std::string & title_,
                         const std::string & indent_,
                         bool inherit_) const
  {
    i_object_3d::tree_dump(out_, title_, indent_, true);

    out_ << indent_ << datatools::i_tree_dumpable::tag
          << "Radius : " << get_r() / CLHEP::mm << " mm" << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::tag
         << "Start angle : ";
    if (has_start_angle()) {
      out_ << get_start_angle() / CLHEP::degree << " degree";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Delta angle : ";
    if (has_delta_angle()) {
      out_ << get_delta_angle() / CLHEP::degree << " degree";
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    return;
  }

  bool circle::is_on_curve(const vector_3d & position_,
                           double tolerance_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid circle!");
    double tolerance = compute_tolerance(tolerance_);
    double half_tolerance = 0.5 * tolerance;
    double theta = std::atan2(position_.y(), position_.x());
    double ct = std::cos(theta);
    double st = std::sin(theta);
    vector_3d proj(_radius_ * ct, _radius_ * st, 0.0);
    if ((position_ - proj).mag() > half_tolerance) {
      return false;
    }
    if (has_partial_angle()) {
      double angular_tolerance = 0.0;
      if (! angle_is_in(theta, _start_angle_, _delta_angle_, angular_tolerance)) {
        return false;
      }
    }
    return true;
  }

  vector_3d circle::get_direction_on_curve(const vector_3d & position_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid circle!");
    double theta = std::atan2(position_.y(), position_.x());
    double ct = std::cos(theta);
    double st = std::sin(theta);
    vector_3d proj(_radius_ * ct, _radius_ * st, 0.0);
    vector_3d dir(-st, ct, 0.0);
    if (has_partial_angle()) {
      double angular_tolerance = 0.0;
      if (! angle_is_in(theta, _start_angle_, _delta_angle_, angular_tolerance)) {
        geomtools::invalidate(dir);
      }
    }
    return dir;
  }

  void circle::generate_wires_self(wires_type & wires_,
                                   uint32_t options_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid circle!");
    uint32_t options = options_ & WR_BASE_MASK;

    uint32_t nsamples_angle = angular_sampling_from_options(options);
    {
      polyline_type dummy;
      wires_.push_back(dummy);
    }
    polyline_type & wire = wires_.back();
    double t0 = 0.0;
    double t1 = 2 * M_PI;
    if (has_start_angle()) {
      t0 = get_start_angle();
      t1 = t0 + get_delta_angle();
    }
    double dtheta = (t1 - t0) / nsamples_angle;
    for (size_t it = 0; it <= nsamples_angle; it++) {
      double thetai = t0 + it * dtheta;
      vector_3d v(get_r() * std::cos(thetai),
                  get_r() * std::sin(thetai),
                  0.0);
      wire.push_back(v);
    }
    return;
  }

} // end of namespace geomtools
