// -*- mode: c++; -*- 
/* i_shape_1d.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2012-04-10
 * 
 * License: 
 * 
 * Description: 
 *  Interface for 1D shaped volumes (lines)
 *
 * History: 
 * 
 */

#ifndef GEOMTOOLS_I_SHAPE_1D_H_
#define GEOMTOOLS_I_SHAPE_1D_H_ 1

#include <string>

#include <geomtools/utils.h>
#include <geomtools/i_object_3d.h>

namespace geomtools {

  class i_shape_1d : public i_object_3d
  {
  public:

    virtual int get_dimensional () const;

    i_shape_1d (double tolerance_ = GEOMTOOLS_DEFAULT_TOLERANCE);

    virtual ~i_shape_1d ();

    virtual bool is_on_curve (const vector_3d &, 
                              double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const = 0;

    virtual vector_3d get_direction_on_curve (const vector_3d & position_) const = 0;
 
  };
    
} // end of namespace geomtools

#endif // GEOMTOOLS_I_SHAPE_1D_H_

// end of i_shape_1d.h
