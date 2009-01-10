// -*- mode: c++; -*- 
/* utils.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2008-05-23
 * 
 * License: 
 * 
 * Description: 
 *  Utilities.
 *
 * History: 
 * 
 */

#ifndef __geomtools__utils_h
#define __geomtools__utils_h 1

#include <iostream>
#include <iomanip>
#include <string>
#include <limits>

#include <geomtools/clhep.h>

namespace geomtools {

  enum orientation_type
    {
      VERTICAL   = 0,
      HORIZONTAL = 1
    };

  enum direction_type
    {
      BACK   = 0, // -x
      FRONT  = 1, // +x
      LEFT   = 2, // -y
      RIGHT  = 3, // +y
      TOP    = 4, // -z
      BOTTOM = 5  // +z
    };

  // see also CLHEP/Vector/TwoVector.h and CLHEP/Vector/ThreeVector.h
  /*
  enum axis_type
    {
      X = 0,
      Y = 1,
      Z = 2
    };
  */
    
  /* Initialize a rotation matrix for 
   * "World coordinates system->Local coordinates system":
   */
  void
  create_rotation (rotation & rot_,
		   double phi_,
		   double theta_,
		   double delta_);

  void
  reset_rotation (rotation & rot_);
  
  void
  tree_dump (const rotation & rot_,
	     std::ostream & out_, 
	     const std::string & title_ = "", 
	     const std::string & indent_ = "");
  
  void 
  invalidate (vector_3d & vec_);
  
  bool 
  is_valid (const vector_3d & vec_);
    
} // end of namespace geomtools

#endif // __geomtools__utils_h

// end of utils.h
