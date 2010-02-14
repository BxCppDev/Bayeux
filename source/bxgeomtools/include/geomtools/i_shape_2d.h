// -*- mode: c++; -*- 
/* i_shape_2d.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2008-05-23
 * 
 * License: 
 * 
 * Description: 
 *  Interface for 2D shaped volumes
 *
 * History: 
 * 
 */

#ifndef __geomtools__i_shape_2d_h
#define __geomtools__i_shape_2d_h 1

#include <string>

#include <datatools/utils/properties.h>

#include <geomtools/utils.h>
#include <geomtools/i_object_3d.h>

namespace geomtools {

  class i_shape_2d : public i_object_3d
    {
    public:
      enum face_t
      {
	FACE_NONE = 0x0,
	FACE_UP   = 0x1,
	FACE_DOWN = 0x2,
	FACE_ALL  = FACE_UP | FACE_DOWN
      };

      virtual int get_dimensional () const;

      i_shape_2d (double tolerance_ = i_object_3d::DEFAULT_TOLERANCE);

      virtual ~i_shape_2d ();
      
      virtual bool is_on_surface (const vector_3d & position_,
				  double skin_ = USING_PROPER_TOLERANCE) const = 0; 
      
      virtual vector_3d get_normal_on_surface (const vector_3d & position_,
					       bool up_ = true) const = 0;

      virtual bool find_intercept (const vector_3d & from_, 
				   const vector_3d & direction_,
				   intercept_t & intercept_,
				   double skin_ = USING_PROPER_TOLERANCE) const = 0;
 
  };
    
} // end of namespace geomtools

#endif // __geomtools__i_shape_2d_h

// end of i_shape_2d.h
