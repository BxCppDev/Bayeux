// -*- mode: c++; -*-
/* helix_3d.cc
 */

#include <geomtools/helix_3d.h>

namespace geomtools {

  using namespace std;

  const string helix_3d::HELIX_3D_LABEL = "helix_3d";

  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION (helix_3d,"geomtools::helix_3d")


  bool helix_3d::is_normal ()
  {
    return isfinite(_radius_) &&
      isfinite(_step_) &&
      isfinite(_t1_) &&
      isfinite(_t2_) &&
      isfinite(_center_.x()) &&
      isfinite(_center_.y()) &&
      isfinite(_center_.z());
  }

  string helix_3d::get_shape_name () const
  {
    return helix_3d::HELIX_3D_LABEL;
  }

  bool
  helix_3d::is_valid () const
  {
    return (_radius_ == _radius_);
  }

  void
  helix_3d::invalidate ()
  {
    double qnan = numeric_limits<double>::quiet_NaN();
    _radius_ = qnan;
    return;
  }

  const vector_3d &
  helix_3d::get_center () const
  {
    return _center_;
  }

  vector_3d &
  helix_3d::get_center ()
  {
    return _center_;
  }

  const vector_3d &
  helix_3d::get_center_const () const
  {
    return get_center ();
  }

  void
  helix_3d::set_center (const vector_3d & new_value_)
  {
    _center_ = new_value_;
    return;
  }

  double
  helix_3d::get_radius () const
  {
    return _radius_;
  }

  void
  helix_3d::set_radius (double new_value_)
  {
    _radius_ = new_value_;
    return;
  }

  double
  helix_3d::get_step () const
  {
    return _step_;
  }

  void
  helix_3d::set_step (double new_value_)
  {
    _step_ = new_value_;
    return;
  }

  double
  helix_3d::get_t1 () const
  {
    return _t1_;
  }

  void
  helix_3d::set_t1 (double new_value_)
  {
    _t1_ = new_value_;
    return;
  }

  double
  helix_3d::get_t2 () const
  {
    return _t2_;
  }

  void
  helix_3d::set_t2 (double new_value_)
  {
    _t2_ = new_value_;
    return;
  }

  void
  helix_3d::set_angle1 (double new_value_)
  {
    _t1_ = angle_to_t (new_value_);
    return;
  }

  void
  helix_3d::set_angle2 (double new_value_)
  {
    _t2_ = angle_to_t (new_value_);
    return;
  }

  double
  helix_3d::get_angle1 () const
  {
    return t_to_angle (_t1_);
  }

  double
  helix_3d::get_angle2 () const
  {
    return t_to_angle (_t2_);
  }

  double
  helix_3d::get_length () const
  {
    double l = fabs (get_angle2 () - get_angle1 () )
      * sqrt ( _radius_ * _radius_ + _step_ * _step_ );
    return l;
  }

  // ctor/dtor:
  helix_3d::helix_3d () : i_shape_1d ()
  {
    _radius_ = 1.0;
    _center_.set (0.0, 0.0, 0.0);
    _step_ = 0.0;
    _t1_ = 0.0;
    _t2_ = 1.0;
    return;
  }

  helix_3d::~helix_3d ()
  {
  }

  void
  helix_3d::invalidate_angles ()
  {
    double qnan = numeric_limits<double>::quiet_NaN();
    _t1_ = qnan;
    _t2_ = qnan;
    return;
  }

  bool
  helix_3d::angles_are_valid () const
  {
    return ((_t1_ == _t1_) && (_t2_ == _t2_));
  }

  double
  helix_3d::t_to_angle (double t_)
  {
    return (2 * M_PI * t_);
  }

  double
  helix_3d::angle_to_t (double angle_)
  {
    return (angle_ / (2 * M_PI));
  }

  bool
  helix_3d::t_is_valid (double t_) const
  {
    return (t_ >= _t1_) && (t_ <= _t2_);
  }

  // XXXXXX
  /*
    int
    helix_3d::find_t (const vector_3d & pos_, double & t_) const
    {
    int err = EXIT_SUCCESS;
    double h = _step_;
    double x = pos_.x() - _center_.x ();
    double y = pos_.y() - _center_.y ();
    double z = pos_.z() - _center_.z ();
    t_ = z / h;
    return err;
    }
  */

  double
  helix_3d::get_theta_from_xy (double x_, double y_) const
  {
    double x = x_ - _center_.x ();
    double y = y_ - _center_.y ();
    double theta = atan2 (y, x);
    if (theta < 0.0) theta += 2. * M_PI;
    return theta;
  }

  double
  helix_3d::get_t_from_z (double z_) const
  {
    bool devel = false;
    //devel = true;
    double t = 0.0;
    if (! isnormal (z_))
      {
        t = numeric_limits<double>::quiet_NaN ();
      }
    else
      {
        if (_step_ != 0.0 )
          {
            t = (z_ - _center_.z ()) / _step_;
            if (devel) clog << "DEVEL:  helix_3d::get_t_from_z: h=" << _step_ << endl;
            if (devel) clog << "DEVEL:  helix_3d::get_t_from_z: t=" << t << endl;
          }
        else
          {
            t = numeric_limits<double>::infinity ();
            if (devel) clog << "DEVEL:  helix_3d::get_t_from_z: t_inf=" << t << endl;
            // we have ! is_normal (t)
          }
      }
    return t;
  }

  // XXX

  double
  helix_3d::get_t (vector_3d point_) const
  {
    return get_t_from_xyz (point_.x(), point_.y(), point_.z());
  }

  double
  helix_3d::get_t_from_xyz (double x_, double y_, double z_) const
  {
    bool devel = false;
    //devel = true;
    if (devel) clog << "DEVEL:  helix_3d::get_t_from_xyz: go..." << endl;
    double theta = get_theta_from_xy (x_, y_);
    double t_xy = helix_3d::angle_to_t (theta);
    double t = t_xy;
    if (devel) clog << "DEVEL:  helix_3d::get_t_from_xyz: z=" << z_  << endl;
    if (devel) clog << "DEVEL:  helix_3d::get_t_from_xyz: z_c=" << _center_.z ()  << endl;
    double t_z = get_t_from_z (z_);
    if (devel) clog << "DEVEL:  helix_3d::get_t_from_xyz: t=" << t  << endl;
    if (devel) clog << "DEVEL:  helix_3d::get_t_from_xyz: t_z=" << t_z  << endl;
    if (devel) clog << "DEVEL:  helix_3d::get_t_from_xyz: h=" << get_step ()  << endl;
    if (isfinite (t_z))
      {
        while (t < (t_z - 0.5)) t++;
        while (t > (t_z + 0.5)) t--;
      }
    if (devel) clog << "DEVEL:  helix_3d::get_t_from_xyz: stop" << endl;
    return t;
  }

  double
  helix_3d::get_z_from_t (double t_) const
  {
    double z = _center_.z () + _step_ * t_;
    return z;
  }

  vector_3d
  helix_3d::get_point_xy (double theta_) const
  {
    double angle = theta_;
    double x = _center_.x () + _radius_ * cos (angle);
    double y = _center_.y () + _radius_ * sin (angle);
    return vector_3d (x, y, 0.0);
  }

  vector_3d
  helix_3d::get_point (double t_) const
  {
    double angle = t_to_angle (t_);
    double x = _center_.x () + _radius_ * cos (angle);
    double y = _center_.y () + _radius_ * sin (angle);
    double z = get_z_from_t (t_);
    return vector_3d (x, y, z);
  }

  vector_3d
  helix_3d::get_first () const
  {
    return get_point (_t1_);
  }

  vector_3d
  helix_3d::get_last () const
  {
    return get_point (_t2_);
  }

  double
  helix_3d::get_curvilinear_position (double t_) const
  {
    double c = hypot (2. * M_PI *_radius_, _step_);
    return c * t_;
  }

  void
  helix_3d::print_xyz (ostream & out_,
                       const helix_3d & helix_,
                       double step_angle_,
                       int expand_)
  {
    double delta_t = 1. / 360.; // default
    if (step_angle_ > 0.0) delta_t = angle_to_t (step_angle_);
    //delta_t = 0.1;

    double t_skip = 0.0;
    //t_skip = 1.0;
    double t_min = helix_.get_t1 () - t_skip;
    double t_max = helix_.get_t2 () + t_skip;
    t_min -= expand_;
    t_max += expand_;
    double t = t_min;
    bool stop = false;
    do
      {
        vector_3d v = helix_.get_point (t);
        geomtools::print_xyz (out_, v);
        if (stop) break;
        t += delta_t;
        if (t > t_max)
          {
            t = t_max;
            stop = true;
          }
      } while (true);
    //out_ << endl << endl;
    out_ << endl;
    geomtools::print_xyz (out_, helix_.get_center ());
    //out_ << endl << endl;
    out_ << endl;
    return;
  }

  void helix_3d::dump () const
  {
    tree_dump (clog);
  }

  void helix_3d::tree_dump (ostream &      out_,
                            const string & title_,
                            const string & indent_,
                            bool           inherit_) const
  {
    string indent;
    if (! indent_.empty ())
      {
        indent = indent_;
      }
    i_object_3d::tree_dump (out_, title_, indent_, true);

    namespace du = datatools::utils;
    out_ << indent << du::i_tree_dumpable::tag
         << "Center : "
         << _center_  / CLHEP::mm << " mm"
         << endl;
    out_ << indent << du::i_tree_dumpable::tag
         << "Radius : "
         << _radius_ / CLHEP::mm << " mm"
         << endl;
    out_ << indent << du::i_tree_dumpable::tag
         << "Step   : "
         << _step_ // << " mm/round"
         << endl;
    out_ << indent << du::i_tree_dumpable::tag
         << "T1     : "
         << _t1_ << " (" << get_angle1 () / CLHEP::radian << " radian)"
         << endl;
    out_ << indent << du::i_tree_dumpable::tag
         << "T2     : "
         << _t2_ << " (" << get_angle2 () / CLHEP::radian << " radian)"
         << endl;
    out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_)
         << "Length : "
         << get_length() / CLHEP::mm << " mm"
         << endl;
    return;
  }

  void
  helix_3d::make_vertex_collection (basic_polyline_3d & bpl_,
                                    double angular_step_) const
  {
    bpl_.clear ();
    double dt = angular_step_ / (2. * M_PI);
    if (angular_step_ <= 0.0) dt = 0.01;
    bpl_.push_back (get_first ());
    for (double t = get_t1 () + dt;
         t < get_t2 ();
         t += dt)
      {
        if (t >= get_t2 ()) break;
        bpl_.push_back (get_point (t));
      }
    bpl_.push_back (get_last ());
    return;
  }

  basic_polyline_3d
  helix_3d::make_vertex_collection () const
  {
    basic_polyline_3d bpl;
    make_vertex_collection (bpl);
    return bpl;
  }

  bool helix_3d::is_on_curve (const vector_3d & position_,
                              double tolerance_) const
  {
    bool on_curve = false;
    throw runtime_error ("helix_3d::is_on_curve: Not implemented yet !");
    return on_curve;
  }

  vector_3d helix_3d::get_direction_on_curve (const vector_3d & position_) const
  {
    double    step_angle = 0.001;
    vector_3d dir;
    double    t_angle    = get_t ( position_ );
    vector_3d pos_plus   = get_point ( t_angle + step_angle );
    vector_3d pos_minus  = get_point ( t_angle - step_angle );

    dir = pos_plus - pos_minus;
    dir /= dir.mag();

    return dir;
  }

} // end of namespace geomtools

// end of helix_3d.cc
