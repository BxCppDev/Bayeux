/// \file geomtools/i_shape_2d.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-23
 * Last modified: 2015-02-24
 *
 * License:
 *
 * Description:
 *
 *  Interface for 2D shaped surfaces
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_I_SHAPE_2D_H
#define GEOMTOOLS_I_SHAPE_2D_H 1

// Standard library:
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/bit_mask.h>
#include <datatools/handle.h>
#include <datatools/ocd_macros.h>

// This project:
#include <geomtools/utils.h>
#include <geomtools/i_object_3d.h>
#include <geomtools/i_find_intercept.h>
#include <geomtools/i_wires_3d_rendering.h>

namespace geomtools {

  /// \brief The abstract base class for all 2D shapes/surfaces
  class i_shape_2d : public i_object_3d,
                     public i_find_intercept,
                     public i_wires_3d_rendering
  {
  public:

    /// \brief The up/down identifier of the face
    enum faces_mask_type {
      FACE_NONE   = face_identifier::FACE_BITS_NONE, //!< Invalid face
      FACE_UNIQUE = datatools::bit_mask::bit00, //!< Up face
      FACE_UP     = FACE_UNIQUE,                //!< Unique face
      FACE_DOWN   = datatools::bit_mask::bit01, //!< Down face
      FACE_ALL    = FACE_UNIQUE
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

    /// Check if the 2D shape surface is defined
    virtual bool has_surface(uint32_t flags_ = ALL_PIECES) const;

    /// Return the surface
    virtual double get_surface(uint32_t flags_ = ALL_PIECES) const;

    /// Return the dimension of the object
    int get_dimensional() const override;

    /// Default constructor
    i_shape_2d();

    /// Constructor
    i_shape_2d(double tolerance_);

    /// Constructor
    i_shape_2d(double tolerance_, double angular_tolerance_);

    /// Destructor
    ~i_shape_2d() override;

    /// Check is a given point belongs to the surface of the 2D shape
    ///
    /// @arg position_ The position to be checked
    /// @arg skin_ The tolerance (effective thickness of the surface)
    /// @return true if the checked position belongs to the surface
    virtual bool is_on_surface(const vector_3d & position_,
                               double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const = 0;

    /// Return the normal direction at some position on the 2D shape's path
    ///
    /// @arg position_ The position to be checked
    /// @return the normal 3D vector at the checked position that is assumes
    ///         belonging to the surface
    virtual vector_3d get_normal_on_surface(const vector_3d & position_,
                                            bool check_ = false,
                                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const = 0;

    /// Document the base object configuration description (OCD) associated to a
    /// 2D shape.
    ///
    /// @arg ocd_ The object configuration description to be documented
    static void init_ocd(datatools::object_configuration_description & ocd_);

    // Serialization interface
    DATATOOLS_SERIALIZATION_DECLARATION()

  };

  //! Type aliases for a handle to a 2D shape
  typedef datatools::handle<i_shape_2d> shape_2d_handle_type;
  typedef shape_2d_handle_type          face_handle_type;

} // end of namespace geomtools

/*
// Class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(geomtools::i_shape_2d, 0)
*/

#endif // GEOMTOOLS_I_SHAPE_2D_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
