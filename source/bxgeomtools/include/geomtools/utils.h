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

#include <geomtools/clhep.h>

namespace geomtools {

  enum orientation_type
    {
      VERTICAL   = 0,
      HORIZONTAL = 1
    };

  enum direction_type
    {
      BACK   = 0,
      FRONT  = 1,
      LEFT   = 2,
      RIGHT  = 3,
      TOP    = 4,
      BOTTOM = 5
    };

  // see also CLHEP/Vector/TwoVector.h and CLHEP/Vector/ThreeVector.h
  enum axis_type
    {
      X = 0,
      Y = 1,
      Z = 2
    };
    
  /* Initialize a rotation matrix for 
   * "World coordinates system->Local coordinates system":
   */
  void
  create_rotation (rotation & rot_,
		   double phi_,
		   double theta_,
		   double delta_);

  void
  roundoff_rotation (rotation & rot_,
		     double epsilon_);
  
  void
  tree_dump (const rotation & rot_,
	     std::ostream & out_, 
	     const std::string & title_ = "", 
	     const std::string & indent_ = "");
  
  void 
  print (std::ostream & out_, const vector_3d & vec_);
  
  void 
  print (std::ostream & out_, const rotation & rot_);
    
} // end of namespace geomtools

#endif // __geomtools__utils_h

// end of utils.h
