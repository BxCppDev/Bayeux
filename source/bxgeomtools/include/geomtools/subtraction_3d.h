/// \file geomtools/subtraction_3d.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-24
 * Last modified: 2015-03-18
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

  //! \brief Subtraction of two 3D shapes
  class subtraction_3d : public i_composite_shape_3d
  {
  public:

    /// Return the label of the subtraction solid type
    static const std::string & subtraction_3d_label();

    /// Return the name of the shape type
    std::string get_shape_name () const;

    /// Default constructor
    subtraction_3d ();

    /// Destructor
    virtual ~subtraction_3d ();

    /// Check if a point is inside the cylinder
    virtual bool is_inside(const vector_3d & position_,
                           double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Check if a point is outside the cylinder
    virtual bool is_outside(const vector_3d & position_,
                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Return the surface bit a point belongs to
    virtual face_identifier on_surface(const vector_3d &,
                                       const face_identifier & a_surface_mask = face_identifier::face_invalid(),
                                       double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Return the vector normal to the surface at some position
    virtual vector_3d get_normal_on_surface(const vector_3d & a_position,
                                            const face_identifier & a_surface_bit) const;

    /// Find the intercept point of a segment with the surface
    virtual bool find_intercept (const vector_3d & from_,
                                 const vector_3d & direction_,
                                 face_intercept_info & intercept_,
                                 double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Generate a sequence of polylines for wires 3D rendering
    virtual void generate_wires_self(wires_type & wires_,
                                     uint32_t options_ = 0) const;

  protected:

    /// Destructor
    virtual void _build_bounding_data();

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(subtraction_3d);

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_SUBTRACTION_3D_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
