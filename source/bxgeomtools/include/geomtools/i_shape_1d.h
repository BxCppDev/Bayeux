// -*- mode: c++; -*- 
/* i_shape_1d.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2008-05-23
 * 
 * License: 
 * 
 * Description: 
 *  Interface for 1D shaped volumes (lines)
 *
 * History: 
 * 
 */

#ifndef __geomtools__i_shape_1d_h
#define __geomtools__i_shape_1d_h 1

#include <string>

#include <datatools/utils/properties.h>

#include <geomtools/utils.h>
#include <geomtools/i_object_3d.h>

namespace geomtools {

  class i_shape_1d : public i_object_3d
    {
    public:

      virtual int get_dimensional () const;

      i_shape_1d (double tolerance_ = i_object_3d::DEFAULT_TOLERANCE);

      virtual ~i_shape_1d ();

      virtual bool is_on_curve (const vector_3d &, 
				double tolerance_ = i_object_3d::USING_PROPER_TOLERANCE) const = 0;

      virtual vector_3d get_direction_on_curve (const vector_3d & position_) const = 0;
 
  };
    
} // end of namespace geomtools

#endif // __geomtools__i_shape_1d_h

// end of i_shape_1d.h
