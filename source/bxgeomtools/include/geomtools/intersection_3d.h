// -*- mode: c++; -*-
/// \file geomtools/intersection_3d.h
/*
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

#ifndef GEOMTOOLS_INTERSECTION_3D_H
#define GEOMTOOLS_INTERSECTION_3D_H 1

// This project:
#include <geomtools/i_composite_shape_3d.h>

namespace geomtools {

  class intersection_3d : public i_composite_shape_3d
  {
  public:

    static const std::string & intersection_3d_label();

    std::string get_shape_name () const;

    intersection_3d ();

    virtual ~intersection_3d ();

    virtual bool is_inside (const vector_3d & position_,
                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual bool is_outside (const vector_3d & position_,
                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual bool is_on_surface (const vector_3d & position_,
                                int mask_    = COMPONENT_SHAPE_ALL,
                                double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual vector_3d get_normal_on_surface (const vector_3d & position_) const;

    virtual  bool find_intercept (const vector_3d & from_,
                                  const vector_3d & direction_,
                                  intercept_t & intercept_,
                                  double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

  protected:

    virtual void _build_bounding_data();

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(intersection_3d);

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_INTERSECTION_3D_H
