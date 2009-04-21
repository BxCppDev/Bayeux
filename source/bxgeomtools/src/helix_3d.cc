// -*- mode: c++; -*- 
/* helix_3d.cc
 */

#include <geomtools/helix_3d.h>

namespace geomtools {
  
  const std::string helix_3d::SERIAL_TAG = "__geomtools::helix_3d__";

  const std::string & 
  helix_3d::get_serial_tag () const
  {
    return helix_3d::SERIAL_TAG;
  }

  bool 
  helix_3d::is_valid () const
  {
    return (__radius == __radius);
  }
  
  void 
  helix_3d::invalidate ()
  {
    double qnan = std::numeric_limits<double>::quiet_NaN();
    __radius = qnan;
  }
  
  const vector_3d & 
  helix_3d::get_center () const
  {
    return __center;
  }
  
  vector_3d & 
  helix_3d::get_center ()
  {
    return __center;
  }
  
  void 
  helix_3d::set_center (const vector_3d & new_value_)
  {
    __center = new_value_;
  }
  
  double 
  helix_3d::get_radius () const
  {
    return __radius;
  }
  
  void 
  helix_3d::set_radius (double new_value_)
  {
    __radius = new_value_;
  }
  
  double 
  helix_3d::get_step () const
  {
    return __step;
  }
  
  void 
  helix_3d::set_step (double new_value_)
  {
    __step = new_value_;
  }
  
  double 
  helix_3d::get_t1 () const
  {
    return __t1;
  }
  
  void 
  helix_3d::set_t1 (double new_value_)
  {
    __t1 = new_value_;
  }
  
  double 
  helix_3d::get_t2 () const
  {
    return __t2;
  }
  
  void 
  helix_3d::set_t2 (double new_value_)
  {
    __t2 = new_value_;
  }

  void 
  helix_3d::set_angle1 (double new_value_)
  {
    __t1 = angle_to_t (new_value_);
  }

  void 
  helix_3d::set_angle2 (double new_value_)
  {
    __t2 = angle_to_t (new_value_);
  }

  double 
  helix_3d::get_angle1 () const
  {
    return t_to_angle (__t1);
  }
  
  double 
  helix_3d::get_angle2 () const
  {
    return t_to_angle (__t2);
  }
  
  // ctor/dtor:
  helix_3d::helix_3d ()
  {
    __radius = 1.0;
    __center.set (0.0, 0.0, 0.0);
    __step = 0.0;
    __t1 = 0.0;
    __t2 = 1.0;
  }
  
  helix_3d::~helix_3d ()
  {
  }

  void 
  helix_3d::invalidate_angles ()
  {
    double qnan = std::numeric_limits<double>::quiet_NaN();
    __t1 = qnan;
    __t2 = qnan;
  }

  bool
  helix_3d::angles_are_valid () const
  {
    return ((__t1 == __t1) && (__t2 == __t2));
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
    return (t_ >= __t1) && (t_ <= __t2);
  }

  // XXXXXX
  /*
  int 
  helix_3d::find_t (const vector_3d & pos_, double & t_) const
  {
    int err = EXIT_SUCCESS;
    double h = __step;
    double x = pos_.x() - __center.x ();
    double y = pos_.y() - __center.y ();
    double z = pos_.z() - __center.z ();
    t_ = z / h;
    return err;
  }
  */

  double 
  helix_3d::get_theta_from_xy (double x_, double y_) const
  {
    double x = x_ - __center.x ();
    double y = y_ - __center.y ();
    double theta = std::atan2 (y, x);
    if (theta < 0.0) theta += 2. * M_PI;
    return theta;
  }

  double 
  helix_3d::get_t_from_z (double z_) const
  {
    bool devel = false;
    //devel = true;
    double t = 0.0;
    if (! std::isnormal (z_)) 
      {
	t = std::numeric_limits<double>::quiet_NaN (); 
      }
    else 
      {
	if (__step != 0.0 ) 
	  {
	    t = (z_ - __center.z ()) / __step;
	    if (devel) std::clog << "DEVEL:  helix_3d::get_t_from_z: h=" << __step << std::endl;
	    if (devel) std::clog << "DEVEL:  helix_3d::get_t_from_z: t=" << t << std::endl;
	  }
	else 
	  {
	    t = std::numeric_limits<double>::infinity (); 
	    if (devel) std::clog << "DEVEL:  helix_3d::get_t_from_z: t_inf=" << t << std::endl;
	    // we have ! is_normal (t)
	  }
      }
    return t;
  }

  // XXX

  double 
  helix_3d::get_t_from_xyz (double x_, double y_, double z_) const
  {
    bool devel = false;
    //devel = true;
    if (devel) std::clog << "DEVEL:  helix_3d::get_t_from_xyz: go..." << std::endl;
    double theta = get_theta_from_xy (x_, y_);
    double t_xy = helix_3d::angle_to_t (theta);
    double t = t_xy;
    if (devel) std::clog << "DEVEL:  helix_3d::get_t_from_xyz: z=" << z_  << std::endl;
    if (devel) std::clog << "DEVEL:  helix_3d::get_t_from_xyz: z_c=" << __center.z ()  << std::endl;
    double t_z = get_t_from_z (z_);
    if (devel) std::clog << "DEVEL:  helix_3d::get_t_from_xyz: t=" << t  << std::endl;
    if (devel) std::clog << "DEVEL:  helix_3d::get_t_from_xyz: t_z=" << t_z  << std::endl;
    if (devel) std::clog << "DEVEL:  helix_3d::get_t_from_xyz: h=" << get_step ()  << std::endl;
    while (t < (t_z - 0.5)) t++;
    while (t > (t_z + 0.5)) t--;
    if (devel) std::clog << "DEVEL:  helix_3d::get_t_from_xyz: stop" << std::endl;
    return t;
  }

  double 
  helix_3d::get_z_from_t (double t_) const
  {
    double z = __center.z () + __step * t_;
    return z;
  }

  vector_3d 
  helix_3d::get_point_xy (double theta_) const
  {
    double angle = theta_;
    double x = __center.x () + __radius * std::cos (angle);
    double y = __center.y () + __radius * std::sin (angle);
    return vector_3d (x, y, 0.0);    
  }

  vector_3d 
  helix_3d::get_point (double t_) const
  {
    double angle = t_to_angle (t_);
    double x = __center.x () + __radius * std::cos (angle);
    double y = __center.y () + __radius * std::sin (angle);
    double z = get_z_from_t (t_);
    return vector_3d (x, y, z);
  }

  vector_3d 
  helix_3d::get_first () const
  {
    return get_point (__t1);
  }

  vector_3d 
  helix_3d::get_last () const
  {
    return get_point (__t2);
  }

  double 
  helix_3d::get_curvilinear_position (double t_) const
  {
    double c = hypot (2. * M_PI *__radius, __step);
    return c * t_;
  }

  void 
  helix_3d::print_xyz (std::ostream & out_, 
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
    //out_ << std::endl << std::endl;
    out_ << std::endl;
    geomtools::print_xyz (out_, helix_.get_center ());
    //out_ << std::endl << std::endl;
    out_ << std::endl;
  }
  
  void 
  helix_3d::tree_dump (std::ostream &      out_, 
			 const std::string & title_,
			 const std::string & indent_,
			 bool                inherit_) const
  {
    std::string indent;
    if (! indent_.empty ())
      {
        indent = indent_;
      }
    if ( !title_.empty () ) {
      out_ << indent << title_ << std::endl;
    }

    namespace du = datatools::utils;
    out_ << indent << du::i_tree_dumpable::tag 
	 << "center : " 
	 << __center 
	 << std::endl;
    out_ << indent << du::i_tree_dumpable::tag 
	 << "radius : " 
	 << __radius 
	 << std::endl;
    out_ << indent << du::i_tree_dumpable::tag 
	 << "step : " 
	 << __step 
	 << std::endl;
    out_ << indent << du::i_tree_dumpable::tag 
	 << "t1 : " 
	 << __t1 << " (" << get_angle1 () << ')'
	 << std::endl;
    out_ << indent << du::i_tree_dumpable::inherit_tag (inherit_) 
	 << "t2 : " 
	 << __t2 << " (" << get_angle2 () << ')'
	 << std::endl;
  }

  void 
  helix_3d::make_vertex_collection (basic_polyline_3d & bpl_) const
  {
    bpl_.clear ();
    double dt = 0.01;
    bpl_.push_back (get_first ());
    for (double t = get_t1 () + dt;
	 t < get_t2 ();
	 t += dt)
      {
	if (t >= get_t2 ()) break;
	bpl_.push_back (get_point (t));
      }
    bpl_.push_back (get_last ());
  }

  basic_polyline_3d 
  helix_3d::make_vertex_collection () const
  {
    basic_polyline_3d bpl;
    make_vertex_collection (bpl);
    return bpl;
  }

} // end of namespace geomtools

// end of helix_3d.cc
