// -*- mode: c++; -*- 
/* utils.cc 
 */

#include <geomtools/utils.h>

namespace geomtools {

  const double constants::DEFAULT_TOLERANCE = 1.e-14 * CLHEP::mm;

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
  create (rotation_3d & rot_,
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
  create_rotation_3d (rotation_3d & rot_,
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
