/// \file geomtools/union_3d.h
/*
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef GEOMTOOLS_UNION_3D_H
#define GEOMTOOLS_UNION_3D_H 1

// This project:
#include <geomtools/i_composite_shape_3d.h>

namespace geomtools {

  //! \brief Union of two 3D shapes
  class union_3d : public i_composite_shape_3d
  {
  public:

    /// Return the label of the union solid type
    static const std::string & union_3d_label();

    /// Return the name of the shape type
    std::string get_shape_name() const override;

    /// Default constructor
    union_3d();

    /// Destructor
    ~union_3d() override;

    /// Check if a point is inside the cylinder
    bool is_inside(const vector_3d & position_,
                           double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Check if a point is outside the cylinder
    bool is_outside(const vector_3d & position_,
                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Return the surface bit a point belongs to
    face_identifier on_surface(const vector_3d &,
                                       const face_identifier & a_surface_mask = face_identifier::face_invalid(),
                                       double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Return the vector normal to the surface at some position
    vector_3d get_normal_on_surface(const vector_3d & a_position,
                                            const face_identifier & a_surface_bit) const override;

    /// Find the intercept point of a segment with the surface
    bool find_intercept (const vector_3d & from_,
                                 const vector_3d & direction_,
                                 face_intercept_info & intercept_,
                                 double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Generate a sequence of polylines for wires 3D rendering
    void generate_wires_self(wires_type & wires_,
                                     uint32_t options_ = 0) const override;

  protected:

    /// Destructor
    void _build_bounding_data() override;

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(union_3d)

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_UNION_3D_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
