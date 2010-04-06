// -*- mode: c++; -*- 
/* utils.cc 
 */

#include <geomtools/utils.h>

namespace geomtools {

  using namespace std;

  const double constants::DEFAULT_TOLERANCE = 1.e-7 * CLHEP::mm;

  const std::string io::VECTOR_2D_SERIAL_TAG   = "__geomtools::vector_2d__";
  const std::string io::VECTOR_3D_SERIAL_TAG   = "__geomtools::vector_3d__";
  const std::string io::ROTATION_3D_SERIAL_TAG = "__geomtools::rotation_3d__";

  /*******/

  std::string 
  to_xy (const vector_2d & p_)
  {
    std::ostringstream out;
    out << p_.x () << ' ' <<  p_.y ();
    return out.str ();
  }

  std::string 
  vector_2d_to_xy (const vector_2d & p_)
  {
    std::ostringstream out;
    out << p_.x () << ' ' <<  p_.y ();
    return out.str ();    
  }

  void 
  print_xy (std::ostream & out_, 
	    const vector_2d & p_,
	    bool endl_)
  {
    out_ << p_.x () << ' ' <<  p_.y ();
    if (endl_) out_ << std::endl;
  }
  
  void 
  print_xy_stdout (const vector_2d & p_)
  {
    print_xy (std::cout, p_, true);
  }
  
  void 
  print_xy_stderr (const vector_2d & p_)
  {
    print_xy (std::cerr, p_, true);
  }
 
  void
  invalidate (vector_2d & vec_)
  {
    double qnan = std::numeric_limits<double>::quiet_NaN();
    vec_.set (qnan, qnan); 
  }
  
  void 
  invalidate_vector_2d (vector_2d & vec_)
  {
    invalidate (vec_);
  }
  
  bool 
  is_valid (const vector_2d & vec_)
  {
    //double qnan = std::numeric_limits<double>::quiet_NaN();
    return ((vec_.x () == vec_.x ()) && (vec_.y () == vec_.y ()));
  }

  bool 
  is_valid_vector_2d (const vector_2d & vec_)
  {
    return is_valid (vec_);
  }

  void 
  vector_2d_to_vector_3d (const vector_2d & v2d_, vector_3d & v3d_)
  {
    v3d_.set (v2d_.x(), v2d_.y(), 0.0);
  }

  void 
  vector_3d_to_vector_2d (const vector_3d & v3d_, vector_2d & v2d_)
  {
    v2d_.set (v3d_.x(), v3d_.y());
  }

  void 
  make_phi_theta (vector_3d & vec_, double phi_, double theta_)
  {
    double sin_theta = std::sin (theta_);
    double x = sin_theta * std::cos (phi_);
    double y = sin_theta * std::sin (phi_);
    double z = std::cos (theta_);   
    vec_.set (x, y, z);
  }

  /******/

  std::string 
  to_xyz (const vector_3d & p_)
  {
    std::ostringstream out;
    out << p_.x () << ' ' <<  p_.y () << ' ' << p_.z ();
    return out.str ();
  }

  std::string 
  vector_3d_to_xyz (const vector_3d & p_)
  {
    std::ostringstream out;
    out << p_.x () << ' ' <<  p_.y () << ' ' << p_.z ();
    return out.str ();
  }

  void 
  print_xyz (std::ostream & out_, 
	     const vector_3d & p_,
	     bool endl_)
  {
    out_ << p_.x () << ' ' <<  p_.y () << ' ' << p_.z ();
    if (endl_) out_ << std::endl;
  }

  void 
  print (std::ostream & out_, const vector_3d & p_)
  {
    out_ << p_;
  }

  void 
  print_xyz_stdout (const vector_3d & p_)
  {
    print_xyz (std::cout, p_);
  }

  void 
  print_xyz_stderr (const vector_3d & p_)
  {
    print_xyz (std::cerr, p_);
  }

  void
  create (vector_3d & v_,
	  double x_,
	  double y_,
	  double z_)
  {
    create_xyz (v_, x_, y_, z_);
  }

  void
  create_xyz (vector_3d & v_,
	      double x_,
	      double y_,
	      double z_)
  {
    v_.setX (x_);
    v_.setY (y_);
    v_.setZ (z_);
  }

  void
  create_polar (vector_3d & v_,
		double r_,
		double theta_,
		double z_)
  {
    create_xyz (v_,
		r_ * cos (theta_),
		r_ * sin (theta_),
		z_);   
  }

  void
  create_spherical (vector_3d & v_,
		    double r_,
		    double phi_,
		    double theta_)
  {
    create_xyz (v_,
		r_ * sin (theta_) * cos (phi_),
		r_ * sin (theta_) * sin (phi_),
		r_ * cos (theta_));   
  }

  void 
  invalidate (vector_3d & vec_)
  {
    double qnan = std::numeric_limits<double>::quiet_NaN();
    vec_.set (qnan, qnan, qnan); 
  }

  void 
  invalidate_vector_3d (vector_3d & vec_)
  {
    invalidate (vec_);
  }
  
  bool 
  is_valid (const vector_3d & vec_)
  {
    //double qnan = std::numeric_limits<double>::quiet_NaN();
    return (vec_.x () == vec_.x ()) && (vec_.y () == vec_.y () ) && (vec_.z () == vec_.z ());
  }

  bool 
  is_valid_vector_3d (const vector_3d & vec_)
  {
    return is_valid (vec_);
  }

  bool 
  are_near (const vector_3d & vec1_, 
	    const vector_3d & vec2_,
	    double tolerance_)
  {
    return vec1_.isNear (vec2_, tolerance_); // from CLHEP
  }

  bool 
  are_near_vector_3d (const vector_3d & vec1_, 
		      const vector_3d & vec2_,
		      double tolerance_)
  {
    return are_near (vec1_, vec2_, tolerance_);
  }

  /******/

  void 
  print_xy (std::ostream & out_, 
	     const basic_polyline_2d & p_,
	     bool endl_)
  {
    for (basic_polyline_2d::const_iterator i = p_.begin ();
	 i != p_.end ();
	 i++)
      {
	print_xy (out_, *i, true);
      }
    if (endl_) out_ << std::endl;
  }

  std::string 
  to_xy (const basic_polyline_2d & p_)
  {
    std::ostringstream out;
    print_xy (out, p_, true);
    return out.str ();
  }

  std::string 
  basic_polyline_2d_to_xy (const basic_polyline_2d & p_)
  {
    std::ostringstream out;
    print_xy (out, p_, true);
    return out.str ();
  }

  void 
  print_xy_stdout (const basic_polyline_2d & p_)
  {
    print_xy (std::cout, p_, true);
  }

  void 
  print_xy_stderr (const basic_polyline_2d & p_)
  {
    print_xy (std::cerr, p_, true);
  }

  /******/

  void 
  print_xyz (std::ostream & out_, 
	     const basic_polyline_3d & p_,
	     bool endl_)
  {
    for (basic_polyline_3d::const_iterator i = p_.begin ();
	 i != p_.end ();
	 i++)
      {
	print_xyz (out_, *i, true);
      }
    if (endl_) out_ << std::endl;
  }

  std::string 
  to_xyz (const basic_polyline_3d & p_)
  {
    std::ostringstream out;
    print_xyz (out, p_, true);
    return out.str ();
  }

  std::string 
  basic_polyline_3d_to_xyz (const basic_polyline_3d & p_)
  {
    std::ostringstream out;
    print_xyz (out, p_, true);
    return out.str ();
  }

  void 
  print_xyz_stdout (const basic_polyline_3d & p_)
  {
    print_xyz (std::cout, p_, true);
  }

  void 
  print_xyz_stderr (const basic_polyline_3d & p_)
  {
    print_xyz (std::cerr, p_, true);
  }

  /******/

  void 
  invalidate (rotation_3d & rot_)
  {
    rot_ = geomtools::rotation_3d ();
    double qnan = std::numeric_limits<double>::quiet_NaN();
    double * xx_addr = static_cast<double *>(static_cast<void *> (&rot_));
    double * last_addr = xx_addr + 9;
    for (double * p = xx_addr; p < last_addr; p++)
      {
	*p = 0.0;
      }
    *xx_addr = qnan;
    xx_addr+=4;
    *xx_addr = qnan;
    xx_addr+=4;
    *xx_addr = qnan;
  }

  void 
  invalidate_rotation_3d (rotation_3d & rot_)
  {
    invalidate (rot_);
  }

  bool 
  is_valid (const rotation_3d & rot_)
  {
    return (rot_.xx() == rot_.xx());
  }

  bool 
  is_valid_rotation_3d (const rotation_3d & rot_)
  {
    return is_valid (rot_);
  }
   
  void
  create_zyz (rotation_3d & rot_,
	  double phi_,
	  double theta_,
	  double delta_)
  {
    rotation_3d r1, r2, r3;
    r1.rotateZ (-phi_);
    r2.rotateY (-theta_); 
    r3.rotateZ (-delta_);
    rot_ = r3 * r2 * r1;
  }

  void
  create (rotation_3d & rot_,
	  double phi_,
	  double theta_,
	  double delta_)
  {
    create_zyz (rot_, phi_, theta_, delta_);
  }
   
  void
  create_zxz (rotation_3d & rot_,
	      double phi_,
	      double theta_,
	      double psi_)
  {
    rotation_3d r1, r2, r3;
    r1.rotateZ (-phi_);
    r2.rotateX (-theta_); 
    r3.rotateZ (-psi_);
    rot_ = r3 * r2 * r1;
  }
   
  void
  create_xyz (rotation_3d & rot_,
	      double phi_,
	      double theta_,
	      double psi_)
  {
    rotation_3d r1, r2, r3;
    r1.rotateX (-phi_);
    r2.rotateY (-theta_); 
    r3.rotateZ (-psi_);
    rot_ = r3 * r2 * r1;
  }

  void
  create_rotation_3d (rotation_3d & rot_,
		      double phi_,
		      double theta_,
		      double delta_)
  {
    create (rot_, phi_, theta_, delta_);
  }

  void
  create_rotation_from_zyz_euler_angles (rotation_3d & rot_,
					 double phi_,
					 double theta_,
					 double delta_)
  {
    create (rot_, phi_, theta_, delta_);    
  }

  void
  create_rotation (rotation_3d & rot_,
		   double phi_,
		   double theta_,
		   double delta_)
  {
    create (rot_, phi_, theta_, delta_);
  }

  void
  reset (rotation_3d & rot_)
  {
    rot_ = rotation_3d ();
  }

  double get_special_rotation_angle (int flag_angle_)
  {
    switch (flag_angle_)
      {
      case ROTATION_ANGLE_0: return 0.0 * CLHEP::degree;
      case ROTATION_ANGLE_90: return 90.0 * CLHEP::degree;
      case ROTATION_ANGLE_180: return 180.0 * CLHEP::degree;
      case ROTATION_ANGLE_270: return 270.0 * CLHEP::degree;
      }
    return std::numeric_limits<double>::quiet_NaN();
  }

  int get_special_rotation_angle_from_label (const string & s_)
  {
    if (s_ == "0") return ROTATION_ANGLE_0;
    if (s_ == "90") return ROTATION_ANGLE_90;
    if (s_ == "180") return ROTATION_ANGLE_180;
    if (s_ == "270") return ROTATION_ANGLE_270;
    return ROTATION_ANGLE_INVALID;
  }

  bool check_special_rotation_angle (int flag_angle_)
  {
    return (flag_angle_ >= ROTATION_ANGLE_0) && (flag_angle_ <= ROTATION_ANGLE_270);
  }

  string get_special_rotation_angle_label (int flag_angle_)
  {
    switch (flag_angle_)
      {
      case ROTATION_ANGLE_0: return "0";
      case ROTATION_ANGLE_90: return "90";
      case ROTATION_ANGLE_180: return "180";
      case ROTATION_ANGLE_270: return "270";
      }
    return "";
  }

  bool check_rotation_axis (int axis_)
  {
    return (axis_ >= ROTATION_AXIS_X) && (axis_ <= ROTATION_AXIS_Z);
  }

  int get_rotation_axis_from_label (const string & s_)
  {
    if (s_ == "x") return ROTATION_AXIS_X;
    if (s_ == "y") return ROTATION_AXIS_Y;
    if (s_ == "z") return ROTATION_AXIS_Z;
    return ROTATION_AXIS_INVALID;
  }

  string get_rotation_label (int axis_)
  {
    switch (axis_)
      {
      case ROTATION_AXIS_X: return "x";
      case ROTATION_AXIS_Y: return "y";
      case ROTATION_AXIS_Z: return "z";
      }
    return "";
  }

  void 
  create_rotation_from_axis (rotation_3d & rot_,
			     int axis_,
			     double angle_)
  {
    /*
      clog << "DEVEL: create_rotation_from_axis: "
      << "axis=" << axis_ << " angle=" << angle_ / CLHEP::degree << " °" << endl;
    */
    if (! check_rotation_axis (axis_))
      {
	throw runtime_error ("create_rotation_from_axis: Invalid rotation axis !");
      }
    rotation_3d r;
    if (axis_ == ROTATION_AXIS_X)
      {
	r.rotateX (-angle_);
      }
    if (axis_ == ROTATION_AXIS_Y)
      {
	r.rotateY (-angle_);
      }
    if (axis_ == ROTATION_AXIS_Z)
      {
	r.rotateZ (-angle_);
      }
    rot_ = r;
  }

  void 
  create_rotation (rotation_3d & rot_,
		   int axis_,
		   double angle_)
  {
    create_rotation_from_axis (rot_, axis_, angle_);
  }

  void create_rotation (rotation_3d & rot_,
			int axis_,
			int special_angle_)
  {
    if (! check_special_rotation_angle (special_angle_))
      {
	throw runtime_error ("create_rotation_from: Invalid special angle !");
      }
    double angle = get_special_rotation_angle (special_angle_);
    create_rotation (rot_, axis_, angle);
  }

  void create_rotation_from (rotation_3d & rot_,
			     const string & s_)
  {
    throw runtime_error ("create_rotation_from: Not implemented yet !");
    /*
    istringstream iss (s_);
    string token;
    getline (iss, token, '(');
    if (! iss)
      {
	invalidate_rotation_3d (rot_);
	return;
      }
    {
      if (token != "rotation")
      {
      }
      }
    */
  }

  bool
  is_identity (const rotation_3d & rot_)
  {
    return rot_.isIdentity ();
  }

  void
  reset_rotation_3d (rotation_3d & rot_)
  {
    reset (rot_);
  }
  
  void
  tree_dump (const rotation_3d & rot_,
	     std::ostream & out_ , 
	     const std::string & title_, 
	     const std::string & indent_)
  {
    std::string title = title_;
    std::string indent = indent_;
    if (title.empty ()) title = title_;
    out_ << title << std::endl;
    std::string last_tag  = "`-- ";
    std::string last_tagc = "    ";
    if (is_valid (rot_))
      {
	out_ << indent << last_tag << "[ ( " <<
	  std::setw (11) << std::setprecision (6) << rot_.xx () << "   " <<
	  std::setw (11) << std::setprecision (6) << rot_.xy () << "   " <<
	  std::setw (11) << std::setprecision (6) << rot_.xz () << ")" << std::endl;
	out_ << indent << last_tagc << "  ( " <<
	  std::setw (11) << std::setprecision (6) << rot_.yx () << "   " <<
	  std::setw (11) << std::setprecision (6) << rot_.yy () << "   " <<
	  std::setw (11) << std::setprecision (6) << rot_.yz () << ")" << std::endl;
	out_ << indent << last_tagc << "  ( " <<
	  std::setw (11) << std::setprecision (6) << rot_.zx () << "   " <<
	  std::setw (11) << std::setprecision (6) << rot_.zy () << "   " <<
	  std::setw (11) << std::setprecision (6) << rot_.zz () << ") ]" << std::endl;
      }
    else 
      {
	out_ << indent << last_tag << "[" << "invalid" << "]" << std::endl;
      }
  }
 
} // end of namespace geomtools

// end of utils.cc
