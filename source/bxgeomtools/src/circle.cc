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
#include <datatools/units.h>

// This project:
#include <geomtools/polyline_3d.h>

namespace geomtools {

  // Registration :
  GEOMTOOLS_OBJECT_3D_REGISTRATION_IMPLEMENT(circle, "geomtools::circle")

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
    _angle_domain_.set_type(angular_range::RANGE_TYPE_AZIMUTHAL);
    _angle_domain_.reset_partial_angles();
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

  const angular_range & circle::get_angle_domain() const
  {
    return _angle_domain_;
  }

  bool circle::has_partial_angle() const
  {
    return _angle_domain_.is_partial();
  }

  bool circle::has_start_angle() const
  {
    return _angle_domain_.has_start_angle();
  }

  void circle::set_start_angle(double new_value_)
  {
    _angle_domain_.set_start_angle(new_value_);
    return;
  }

  double circle::get_start_angle() const
  {
    return _angle_domain_.get_start_angle();
  }

  bool circle::has_delta_angle() const
  {
    return _angle_domain_.has_delta_angle();
  }

  void circle::set_delta_angle(double new_value_)
  {
    _angle_domain_.set_delta_angle(new_value_);
    return;
  }

  double circle::get_delta_angle() const
  {
    return _angle_domain_.get_delta_angle();
  }

  double circle::get_circumference() const
  {
    double circum;
    datatools::invalidate(circum);
    if (is_valid()) {
      circum = _angle_domain_.get_angle_spread() * _radius_;
    }
    return circum;
  }

  double circle::get_surface() const
  {
    double surface;
    datatools::invalidate(surface);
    if (is_valid()) {
      surface =  0.5 * _angle_domain_.get_angle_spread() * _radius_ * _radius_;
    }
    return surface;
  }

  bool circle::is_valid() const
  {
    if (!datatools::is_valid(_radius_)) {
      return false;
    }
    return _angle_domain_.is_valid();
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
    _angle_domain_.set_partial_angles(start_angle_, delta_angle_);
    return;
  }

  circle::~circle()
  {
    return;
  }

  void circle::initialize(const datatools::properties & config_, const handle_dict_type * objects_)
  {
    if (!is_valid()) {
      this->i_object_3d::_initialize(config_, objects_);

      double lunit = CLHEP::mm;
      if (config_.has_key("length_unit")) {
        const std::string lunit_str = config_.fetch_string("length_unit");
        lunit = datatools::units::get_length_unit_from(lunit_str);
      }

      DT_THROW_IF(! config_.has_key("radius"), std::logic_error, "Missing circle 'radius' property !");
      double radius = config_.fetch_real("radius");
      if (! config_.has_explicit_unit("radius")) {
        radius *= lunit;
      }
      set_radius(radius);

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

    {
      out_ << indent_ << datatools::i_tree_dumpable::tag
           << "Angle domain : " << std::endl;
      std::ostringstream indent2;
      indent2 << indent_ << datatools::i_tree_dumpable::skip_tag;
      _angle_domain_.tree_dump(out_, "", indent2.str());
    }

    out_ << indent_ << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Valid : " << (is_valid() ? "<yes>" : "<no>") << std::endl;

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
    double angular_tolerance = 0.0;
    return _angle_domain_.contains(theta, angular_tolerance);
  }

  vector_3d circle::get_direction_on_curve(const vector_3d & position_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid circle!");
    double theta = std::atan2(position_.y(), position_.x());
    double ct = std::cos(theta);
    double st = std::sin(theta);
    vector_3d proj(_radius_ * ct, _radius_ * st, 0.0);
    vector_3d dir(-st, ct, 0.0);
    double angular_tolerance = 0.0;
    if (! _angle_domain_.contains(theta, angular_tolerance)) {
      geomtools::invalidate(dir);
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

    for (angular_range::iterator iter(_angle_domain_, nsamples_angle);
         !iter;
         ++iter) {
      vector_3d v(get_r() * std::cos(*iter),
                  get_r() * std::sin(*iter),
                  0.0);
      wire.push_back(v);
    }
    return;
  }

} // end of namespace geomtools
