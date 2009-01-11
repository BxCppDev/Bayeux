
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
#include <cmath>

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
      BOTTOM = 4, // -z
      TOP    = 5  // +z
    };

  struct face_3d
  {
    static const int FACE_NONE_BIT = 0x0;
    static const int FACE_ALL_BITS = 0xFFFFFFFF;
  };
  
  struct constants
  {
    static const int NO_INTERCEPT = -1;
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


  /* the 'ran_func' (functor) must have the following operator
   * defined:
   *   double operator() (double);
   * it returns a uniform deviated random number in the [0,1) range
   * like 'drand48' does.
   */
  template <class ran_func>
  void
  randomize_direction (ran_func ran_, vector_3d & dir_)
  {
    double phi = 2. * M_PI * ran_ ();
    double cos_theta = -1 + 2 * ran_ ();
    double sin_theta = std::sqrt (1. - cos_theta * cos_theta);
    double x = sin_theta * std::cos (phi);
    double y = sin_theta * std::sin (phi);
    double z = cos_theta;
    dir_.set (x, y, z);
  }
    
} // end of namespace geomtools

#endif // __geomtools__utils_h

// end of utils.h
