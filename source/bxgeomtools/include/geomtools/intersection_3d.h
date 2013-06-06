// -*- mode: c++; -*-
/* intersection_3d.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2009-01-11
 * Last modified: 2009-01-11
 *
 * License:
 *
 * Description:
 *  Interface:
 *
 * History:
 *
 */

#ifndef __geomtools__intersection_3d_h
#define __geomtools__intersection_3d_h 1

#include <geomtools/i_composite_shape_3d.h>

namespace geomtools {

  class intersection_3d : public i_composite_shape_3d
  {
  public:

    static const std::string INTERSECTION_3D_LABEL;

    std::string get_shape_name () const;

    intersection_3d ();

    virtual ~intersection_3d ();

    virtual bool is_inside (const vector_3d & position_,
                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual bool is_on_surface (const vector_3d & position_,
                                int mask_    = ALL_SURFACES,
                                double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual vector_3d get_normal_on_surface (const vector_3d & position_) const;

    virtual  bool find_intercept (const vector_3d & from_,
                                  const vector_3d & direction_,
                                  intercept_t & intercept_,
                                  double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

  };

} // end of namespace geomtools

#endif // __geomtools__intersection_3d_h

// end of intersection_3d.h
