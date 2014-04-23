// -*- mode: c++; -*-
/** \file geomtools/subtraction_3d.h
 */
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-24
 * Last modified: 2013-05-22
 *
 * License:
 *
 * Description:
 *  Interface:
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_SUBTRACTION_3D_H
#define GEOMTOOLS_SUBTRACTION_3D_H 1

// This project:
#include <geomtools/i_composite_shape_3d.h>

namespace geomtools {

  class subtraction_3d : public i_composite_shape_3d
  {
  public:

    static const std::string & subtraction_3d_label();

    std::string get_shape_name () const;

    subtraction_3d ();

    virtual ~subtraction_3d ();

    virtual bool is_inside (const vector_3d & position_,
                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual bool is_outside (const vector_3d & position_,
                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual bool is_on_surface (const vector_3d & position_,
                                int mask_    = FACE_ALL_BITS,
                                double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual vector_3d get_normal_on_surface (const vector_3d & position_) const;

    virtual  bool find_intercept (const vector_3d & from_,
                                  const vector_3d & direction_,
                                  intercept_t & intercept_,
                                  double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_SUBTRACTION_3D_H
