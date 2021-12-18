/// helix_3d.cc

// Ourselves:
#include <geomtools/helix_3d.h>

// Standard library:
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <limits>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>

namespace geomtools {

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION (helix_3d, "geomtools::helix_3d")

  const std::string & helix_3d::helix_3d_label()
  {
    static std::string label;
    if (label.empty()) {
      label = "helix_3d";
    }
    return label;
  }

  bool helix_3d::is_normal()
  {
    return std::isfinite(_radius_) &&
      std::isfinite(_step_) &&
      std::isfinite(_t1_) &&
      std::isfinite(_t2_) &&
      std::isfinite(_center_.x()) &&
      std::isfinite(_center_.y()) &&
      std::isfinite(_center_.z());
  }

  std::string helix_3d::get_shape_name () const
  {
    return helix_3d::helix_3d_label();
  }

  bool helix_3d::is_valid() const
  {
    return datatools::is_valid(_radius_)
      && datatools::is_valid(_step_)
      && datatools::is_valid(_t1_)
      && datatools::is_valid(_t2_)
      && geomtools::is_valid(_center_)
      ;
  }

  void helix_3d::invalidate ()
  {
    geomtools::invalidate(_center_);
    _step_ = std::numeric_limits<double>::quiet_NaN();
    invalidate_angles();
    _radius_ = std::numeric_limits<double>::quiet_NaN();
    this->i_object_3d::reset();
    return;
  }

  const vector_3d & helix_3d::get_center() const
  {
    return _center_;
  }

  vector_3d & helix_3d::grab_center()
  {
    return _center_;
  }

  void helix_3d::set_center(double x_, double y_, double z_)
  {
    _center_.set(x_, y_, z_);
    return;
  }

  void helix_3d::set_center(const vector_3d & new_value_)
  {
    _center_ = new_value_;
    return;
  }

  double helix_3d::get_radius() const
  {
    return _radius_;
  }

  void helix_3d::set_radius(double new_value_)
  {
    _radius_ = new_value_;
    return;
  }

  double helix_3d::get_step() const
  {
    return _step_;
  }

  void helix_3d::set_step(double new_value_)
  {
    _step_ = new_value_;
    return;
  }

  double helix_3d::get_t1() const
  {
    return _t1_;
  }

  void helix_3d::set_t1(double new_value_)
  {
    _t1_ = new_value_;
    return;
  }

  double helix_3d::get_t2() const
  {
    return _t2_;
  }

  void helix_3d::set_t2(double new_value_)
  {
    _t2_ = new_value_;
    return;
  }

  void helix_3d::set_angle1(double new_value_)
  {
    _t1_ = angle_to_t(new_value_);
    return;
  }

  void helix_3d::set_angle2(double new_value_)
  {
    _t2_ = angle_to_t(new_value_);
    return;
  }

  double helix_3d::get_angle1 () const
  {
    return t_to_angle (_t1_);
  }

  double helix_3d::get_angle2 () const
  {
    return t_to_angle (_t2_);
  }

  double helix_3d::get_length (uint32_t /* flags_ */) const
  {
    // 2012-11-29, FM: fix wrong formula inconsistent with the computation
    // of curvilinear values.
    // (see Manu/Pawel's message on 'snemo-software-l', 2012-11-28):
    /* Code was :
     *   double l = fabs (get_angle2 () - get_angle1 () )
     *   sqrt ( _radius_ * _radius_ + _step_ * _step_ ); // step should be reduced by 2*pi
     * Now is fixed to :
     */
    const double l = fabs (get_angle2 () - get_angle1 () )
      * hypot ( _radius_,  _step_ / (2.*M_PI) );
    return l;
  }

  void helix_3d::reset()
  {
    _set_defaults();
    return;
  }

  void helix_3d::_set_defaults()
  {
    /*
    _radius_ = 1.0;
    _center_.set (0.0, 0.0, 0.0);
    _step_ = 0.0;
    _t1_ = 0.0;
    _t2_ = 1.0;
    */
    datatools::invalidate(_radius_);
    geomtools::invalidate(_center_);
    datatools::invalidate(_step_);
    datatools::invalidate(_t1_);
    datatools::invalidate(_t2_);
    return;
  }

  helix_3d::helix_3d() : i_shape_1d()
  {
    _set_defaults();
    return;
  }

  helix_3d::~helix_3d ()
  {
    return;
  }

  void helix_3d::invalidate_angles ()
  {
    datatools::invalidate(_t1_);
    datatools::invalidate(_t2_);
    return;
  }

  bool helix_3d::angles_are_valid () const
  {
    return ((_t1_ == _t1_) && (_t2_ == _t2_));
  }

  double helix_3d::t_to_angle (double t_)
  {
    return (2 * M_PI * t_);
  }

  double helix_3d::angle_to_t (double angle_)
  {
    return (angle_ / (2 * M_PI));
  }

  bool helix_3d::t_is_valid (double t_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid helix!");
    return (t_ >= _t1_) && (t_ <= _t2_);
  }

  double helix_3d::get_theta_from_xy (double x_, double y_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid helix!");
    const double x = x_ - _center_.x();
    const double y = y_ - _center_.y();
    double theta = std::atan2(y, x);
    if (theta < 0.0) theta += 2. * M_PI;
    return theta;
  }

  double helix_3d::get_t_from_z (double z_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid helix!");
    datatools::logger::priority local_priority = datatools::logger::PRIO_FATAL;
    double t = 0.0;
    if (! std::isnormal(z_)) {
      t = std::numeric_limits<double>::quiet_NaN();
    } else {
      if (_step_ != 0.0 ) {
        t = (z_ - _center_.z()) / _step_;
        DT_LOG_TRACE(local_priority, "step =" << _step_);
        DT_LOG_TRACE(local_priority, "t    =" << t);
      } else {
        t = std::numeric_limits<double>::infinity();
        DT_LOG_TRACE(local_priority, "t_inf=" << t);
        // we have ! is_normal(t)
      }
    }
    return t;
  }

  double helix_3d::get_t(vector_3d point_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid helix!");
    return get_t_from_xyz(point_.x(), point_.y(), point_.z());
  }

  double helix_3d::get_t_from_xyz(double x_, double y_, double z_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid helix!");
    datatools::logger::priority local_priority = datatools::logger::PRIO_FATAL;
    DT_LOG_TRACE(local_priority, "Entering...");
    const double theta = get_theta_from_xy(x_, y_);
    const double t_xy = helix_3d::angle_to_t(theta);
    const double t_z = get_t_from_z(z_);
    double t = t_xy;
    DT_LOG_TRACE (local_priority, "z=" << z_);
    DT_LOG_TRACE (local_priority, "z_c=" << _center_.z());
    DT_LOG_TRACE (local_priority, "t=" << t);
    DT_LOG_TRACE (local_priority, "t_z=" << t_z);
    DT_LOG_TRACE (local_priority, "h=" << get_step());
    if (std::isfinite(t_z)) {
      while (t < (t_z - 0.5)) t++;
      while (t > (t_z + 0.5)) t--;
    }
    DT_LOG_TRACE (local_priority, "Exiting.");
    return t;
  }

  double helix_3d::get_z_from_t (double t_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid helix!");
    const double z = _center_.z () + _step_ * t_;
    return z;
  }

  vector_3d helix_3d::get_point_xy (double theta_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid helix!");
    const double angle = theta_;
    const double x = _center_.x() + _radius_ * std::cos(angle);
    const double y = _center_.y() + _radius_ * std::sin(angle);
    return vector_3d(x, y, 0.0);
  }

  vector_3d helix_3d::get_point (double t_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid helix!");
    const double angle = t_to_angle(t_);
    const double x = _center_.x() + _radius_ * std::cos(angle);
    const double y = _center_.y() + _radius_ * std::sin(angle);
    const double z = get_z_from_t(t_);
    return vector_3d(x, y, z);
  }

  vector_3d helix_3d::get_first() const
  {
    return get_point(_t1_);
  }

  vector_3d helix_3d::get_last() const
  {
    return get_point(_t2_);
  }

  double helix_3d::get_curvilinear_position (double t_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid helix!");
    const double c = hypot (2. * M_PI *_radius_, _step_);
    return c * t_;
  }

  void helix_3d::print_xyz (std::ostream & out_,
                            const helix_3d & helix_,
                            double step_angle_,
                            unsigned int flags_)
  {
    DT_THROW_IF(! helix_.is_valid(), std::logic_error, "Invalid helix!");
    double delta_t = 1. / 360.; // default
    if (step_angle_ > 0.0) delta_t = angle_to_t (step_angle_);

    const double t_skip = 0.0;
    double t_min = helix_.get_t1 () - t_skip;
    double t_max = helix_.get_t2 () + t_skip;
    int expand = 0;
    if (flags_ & PRINT_XYZ_EXPAND1)
      {
        expand += 1;
      }
    if (flags_ & PRINT_XYZ_EXPAND2)
      {
        expand += 2;
      }
    t_min -= expand;
    t_max += expand;
    bool stop = false;
    double t = t_min;
    do
      {
        const vector_3d v = helix_.get_point (t);
        geomtools::print_xyz (out_, v);
        if (stop) break;
        t += delta_t;
        if (t > t_max)
          {
            t = t_max;
            stop = true;
          }
      } while (true);
    out_ << std::endl;
    if (flags_ & PRINT_XYZ_CENTER)
      {
        geomtools::print_xyz (out_, helix_.get_center ());
        out_ << std::endl;
      }
    return;
  }

  void helix_3d::dump () const
  {
    tree_dump (std::clog);
    return;
  }

  void helix_3d::tree_dump (std::ostream &      out_,
                            const std::string & title_,
                            const std::string & indent_,
                            bool           inherit_) const
  {
    std::string indent;
    if (! indent_.empty ())
      {
        indent = indent_;
      }
    i_object_3d::tree_dump (out_, title_, indent_, true);

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Center : "
         << _center_  / CLHEP::mm << " mm"
         << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Radius : "
         << _radius_ / CLHEP::mm << " mm"
         << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Step   : "
         << _step_ // << " mm/round"
         << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "T1     : "
         << _t1_ << " (" << get_angle1 () / CLHEP::radian << " radian)"
         << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "T2     : "
         << _t2_ << " (" << get_angle2 () / CLHEP::radian << " radian)"
         << std::endl;
    out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
         << "Length : "
         << get_length() / CLHEP::mm << " mm"
         << std::endl;
    return;
  }

  void helix_3d::make_vertex_collection (basic_polyline_3d & bpl_,
                                         double angular_step_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid helix!");
    bpl_.clear ();
    double dt = angular_step_ / (2. * M_PI);
    if (angular_step_ <= 0.0) dt = 0.01;
    bpl_.push_back (get_first ());
    for (double t = get_t1 () + dt; t < get_t2 (); t += dt)
      {
        if (t >= get_t2 ()) break;
        bpl_.push_back (get_point (t));
      }
    bpl_.push_back (get_last ());
    return;
  }

  basic_polyline_3d helix_3d::make_vertex_collection () const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid helix!");
    basic_polyline_3d bpl;
    make_vertex_collection (bpl);
    return bpl;
  }

  bool helix_3d::is_on_curve (const vector_3d & position_,
                              double tolerance_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid helix!");
    bool on_curve = false;
    if (is_valid()) {
      double tmin = std::min(_t1_, _t2_);
      double tmax = std::max(_t1_, _t2_);
      double tolerance = compute_tolerance(tolerance_);
      double t_tolerance = tolerance / (2 * M_PI * _radius_);
      if (_step_ == 0.0) {
        if (std::abs(position_.z() - _center_.z()) > 0.5 * tolerance) {
           on_curve = false;
        } else {
          double x = position_.x() - _center_.x();
          double y = position_.y() - _center_.y();
          double r = hypot(x, y);
          if (std::abs(r - _radius_) > 0.5 * tolerance) {
            on_curve = false;
          } else {
            double angle = std::atan2(y, x);
            double angle1 = get_angle1();
            double angle2 = get_angle2();
            if (!angle_is_in(angle, angle1, angle2 - angle1, 0.0, true)) {
              on_curve =  false;
            } else {
              on_curve = true;
            }
          }
        }
      } else {
        double t = get_t(position_);
        if (t < tmin - 0.5 * t_tolerance) {
          on_curve = false;
        } else if (t > tmax + 0.5 * t_tolerance) {
          on_curve = false;
        } else {
          vector_3d pH = get_point(t);
          double dist = (pH - position_).mag();
          if (dist > 0.5 * tolerance) {
            on_curve = false;
          } else {
            // std::cerr << "DEVEL: helix_3d::is_on_curve: STEP != 0" << std::endl;
            // std::cerr << "DEVEL: helix_3d::is_on_curve:   t1 = " << _t1_ << std::endl;
            // std::cerr << "DEVEL: helix_3d::is_on_curve:   t2 = " << _t2_ << std::endl;
            // std::cerr << "DEVEL: helix_3d::is_on_curve:   t  = " << t << std::endl;
            // std::cerr << "DEVEL: helix_3d::is_on_curve:   pH        = " << pH << std::endl;
            // std::cerr << "DEVEL: helix_3d::is_on_curve:   position_ = " << position_ << std::endl;
            // std::cerr << "DEVEL: helix_3d::is_on_curve:   dist      = " << dist << std::endl;
            // std::cerr << std::endl;
            on_curve = true;
          }
        }
      }
    }
    // std::cerr << "DEVEL: helix_3d::is_on_curve: ==> on_curve = " << on_curve << std::endl;
    // std::cerr << std::endl;
    return on_curve;
  }

  vector_3d helix_3d::get_direction_on_curve (const vector_3d & position_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid helix!");
    vector_3d dir;
    geomtools::invalidate(dir);
    const double    step_angle = 0.001;
    const double    t_angle    = get_t(position_ );
    const vector_3d pos_plus   = get_point( t_angle + step_angle );
    const vector_3d pos_minus  = get_point( t_angle - step_angle );
    dir = pos_plus - pos_minus;
    dir /= dir.mag();
    return dir;
  }

  void helix_3d::generate_wires_self(wires_type & wires_, uint32_t options_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid helix!");
    bool do_angular_sample_boost = !(options_ & WR_HELIX_NO_ANGULAR_SAMPLE_BOOST);

    {
      polyline_type dummy;
      wires_.push_back(dummy);
    }
    polyline_type & wire = wires_.back();

    double t_min = get_t1();
    double t_max = get_t2();
    uint32_t nsample_angle = angular_sampling_from_options(options_);
    if (do_angular_sample_boost) {
      nsample_angle *= 10;
    }
    // double delta_t = (t_max - t_min) / nsample_angle;
    double delta_t = 1.0 / nsample_angle;
    double t = t_min;
    bool stop = false;
    do {
      const vector_3d v = get_point(t);
      wire.push_back(v);
      if (stop) break;
      t += delta_t;
      if (t > t_max) {
        t = t_max;
        stop = true;
      }
    } while(true);
    return;
  }

} // end of namespace geomtools
