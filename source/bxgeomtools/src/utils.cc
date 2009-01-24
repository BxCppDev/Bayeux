// -*- mode: c++; -*- 
/* utils.cc
 */

#include <geomtools/utils.h>

namespace geomtools {

  const double constants::DEFAULT_TOLERANCE = 1.e-14 * CLHEP::mm;

  void 
  invalidate (vector_3d & vec_)
  {
    double qnan = std::numeric_limits<double>::quiet_NaN();
    vec_.set (qnan, qnan, qnan); 
  }
  
  bool 
  is_valid (const vector_3d & vec_)
  {
    double qnan = std::numeric_limits<double>::quiet_NaN();
    return (vec_.x () != qnan) && (vec_.y () != qnan) && (vec_.z () != qnan);
  }

  bool 
  are_near (const vector_3d & vec1_, 
	    const vector_3d & vec2_,
	    double tolerance_)
  {
    return vec1_.isNear (vec2_, tolerance_); // from CLHEP
  }
   
  void
  create_rotation (rotation & rot_,
		   double phi_,
		   double theta_,
		   double delta_)
  {
    rotation r1, r2, r3;
    r1.rotateZ (-phi_);
    r2.rotateY (-theta_); 
    r3.rotateZ (-delta_);
    rot_ = r3 * r2 * r1;
  }

  void
  reset_rotation (rotation & rot_)
  {
    rot_ = rotation ();
  }
  
  void
  tree_dump (const rotation & rot_,
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
    out_ << indent << last_tag    << "[ ( " <<
      std::setw (11) << std::setprecision (6) << rot_.xx () << "   " <<
      std::setw (11) << std::setprecision (6) << rot_.xy () << "   " <<
      std::setw (11) << std::setprecision (6) << rot_.xz () << ")" << std::endl;
    out_ << indent << last_tagc   << "  ( " <<
      std::setw (11) << std::setprecision (6) << rot_.yx () << "   " <<
      std::setw (11) << std::setprecision (6) << rot_.yy () << "   " <<
      std::setw (11) << std::setprecision (6) << rot_.yz () << ")" << std::endl;
    out_ << indent << last_tagc   << "  ( " <<
      std::setw (11) << std::setprecision (6) << rot_.zx () << "   " <<
      std::setw (11) << std::setprecision (6) << rot_.zy () << "   " <<
      std::setw (11) << std::setprecision (6) << rot_.zz () << ") ]" << std::endl;
  }

  
 
} // end of namespace geomtools

// end of utils.cc
