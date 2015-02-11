// -*- mode: c++; -*-
/// \file geomtools/i_shape_2d.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef GEOMTOOLS_I_SHAPE_2D_H
#define GEOMTOOLS_I_SHAPE_2D_H 1

// Standard library:
#include <string>

// This project:
#include <geomtools/utils.h>
#include <geomtools/i_object_3d.h>

namespace geomtools {

  /// \brief The abstract base class for all 2D shapes
  class i_shape_2d : public i_object_3d
  {
  public:

    /// \brief The up/down identifier of the face
    enum face_type {
      FACE_NONE = 0x0,
      FACE_UP   = 0x1,
      FACE_DOWN = 0x2,
      FACE_ALL  = FACE_UP | FACE_DOWN
    };

    /// Special flag representing all pieces composing a 2D shape
    static const uint32_t ALL_PIECES = 0xFFFFFFFF;

    /// Check if the 2D shape knows its number of associated sides
    virtual bool has_number_of_sides() const;

    /// Return the number of associated sides
    virtual unsigned int get_number_of_sides() const;

    /// Check if the 2D shape known its perimeter
    virtual bool has_perimeter(uint32_t flags_ = ALL_PIECES) const;

    /// Return the perimeter
    virtual double get_perimeter(uint32_t flags_ = ALL_PIECES) const;

    /// Check if the 2D shape known its surface
    virtual bool has_surface(uint32_t flags_ = ALL_PIECES) const;

    /// Return the surface
    virtual double get_surface(uint32_t flags_ = ALL_PIECES) const;

    /// Return the dimension of the object
    virtual int get_dimensional() const;

    /// Default constructor
    i_shape_2d(double tolerance_ = GEOMTOOLS_DEFAULT_TOLERANCE);

    /// Destructor
    virtual ~i_shape_2d();

    /// Check is a given point belongs to the surface of the 2D shape
    virtual bool is_on_surface(const vector_3d & position_,
                               double skin_ = GEOMTOOLS_PROPER_TOLERANCE
                               ) const = 0;

    /// Return the normal direction at some position on the 2D shape's path
    virtual vector_3d get_normal_on_surface(const vector_3d & position_,
                                            bool up_ = true) const = 0;

    /// Find the intercept of a ray with the 2D shape's surfaces
    virtual bool find_intercept (const vector_3d & from_,
                                 const vector_3d & direction_,
                                 intercept_t & intercept_,
                                 double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const = 0;

    // Serialization interface
    DATATOOLS_SERIALIZATION_DECLARATION();

  };

} // end of namespace geomtools

/*
// Class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(geomtools::i_shape_2d, 0)
*/

#endif // GEOMTOOLS_I_SHAPE_2D_H
