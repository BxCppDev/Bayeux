// -*- mode: c++; -*-
/* i_shape_2d.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2012-04-10
 *
 * License:
 *
 * Description:
 *  Interface for 2D shaped volumes
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_I_SHAPE_2D_H_
#define GEOMTOOLS_I_SHAPE_2D_H_ 1

#include <string>

#include <geomtools/utils.h>
#include <geomtools/i_object_3d.h>

namespace geomtools {

  class i_shape_2d : public i_object_3d
  {
  public:

    enum face_type
      {
        FACE_NONE = 0x0,
        FACE_UP   = 0x1,
        FACE_DOWN = 0x2,
        FACE_ALL  = FACE_UP | FACE_DOWN
      };

    virtual bool has_number_of_sides() const;

    virtual unsigned int get_number_of_sides() const;

    virtual bool has_perimeter(uint32_t flags_ = SIDE_ALL_BITS) const;

    virtual double get_perimeter(uint32_t flags_ = SIDE_ALL_BITS) const;

    virtual bool has_surface(uint32_t flags_ = FACE_ALL_BITS) const;

    virtual double get_surface(uint32_t flags_ = FACE_ALL_BITS) const;

    virtual int get_dimensional () const;

    i_shape_2d (double tolerance_ = GEOMTOOLS_DEFAULT_TOLERANCE);

    virtual ~i_shape_2d ();

    virtual bool is_on_surface (const vector_3d & position_,
                                double skin_ = GEOMTOOLS_PROPER_TOLERANCE
                                ) const = 0;

    virtual vector_3d get_normal_on_surface (const vector_3d & position_,
                                             bool up_ = true) const = 0;

    virtual bool find_intercept (const vector_3d & from_,
                                 const vector_3d & direction_,
                                 intercept_t & intercept_,
                                 double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const = 0;

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_I_SHAPE_2D_H_

// end of i_shape_2d.h
