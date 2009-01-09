// -*- mode: c++; -*- 
/* utils.cc
 */

#include <geomtools/utils.h>

namespace geomtools {

   
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
