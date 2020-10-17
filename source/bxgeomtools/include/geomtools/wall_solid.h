/// \file geomtools/wall_solid.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-07-16
 * Last modified: 2015-07-16
 *
 * License: GPL3
 *
 * Description:
 *
 *  A 3D solid representing a vertical wall with arbitrary height
 *  and composed of several segments of arbitrary thickness
 *
 */

#ifndef GEOMTOOLS_WALL_SOLID_H
#define GEOMTOOLS_WALL_SOLID_H 1

// Standard library:
#include <sstream>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <datatools/object_configuration_description.h>

// This project:
#include <geomtools/i_shape_3d.h>
#include <geomtools/i_stackable.h>
#include <geomtools/simple_polygon.h>
#include <geomtools/placement.h>
#include <geomtools/composite_surface.h>

namespace datatools {
  // Forward class declaration:
  class properties;
}

namespace geomtools {

  class tessellated_solid;

  /// \brief The 3D shape model for a wall solid
  class wall_solid : public i_shape_3d,
                     public i_stackable
  {

  public:

    /// Return the wall shape label
    static const std::string & wall_solid_label();

    /// \brief Masks used for the faces of the cylinder
    enum faces_mask_type {
      FACE_NONE   = face_identifier::FACE_BITS_NONE,
      FACE_SIDE   = 0x1,
      FACE_BOTTOM = 0x2,
      FACE_TOP    = 0x4,
      FACE_ALL    = (FACE_SIDE
                     | FACE_BOTTOM
                     | FACE_TOP)
    };

    double get_xmin() const override;

    double get_xmax() const override;

    double get_ymin() const override;

    double get_ymax() const override;

    double get_zmin() const override;

    double get_zmax() const override;

    /// Return the Z dimension
    double get_z() const;

    /// Set the Z dimension
    void set_z(double);

    /// Return a non mutable reference to the base
    const simple_polygon & get_base() const;

    /// Return a mutable reference to the base
    simple_polygon & grab_base();

    //! Default constructor
    wall_solid();

    //! Destructor
    ~wall_solid() override;

    /// Return the identifier/name of the shape
    std::string get_shape_name() const override;

    /// Check if the solid is valid
    bool is_valid() const override;

    /// Main initialization method from a container of configuration parameters
    void initialize(const datatools::properties & setup_, const handle_dict_type * = 0) override;

    /// Reset/invalidate the solid
    void reset() override;

    /// Compute the bottom face
    void compute_bottom_face(composite_surface & face_,
                             placement & face_placement_) const;

    /// Compute the top face
    void compute_top_face(composite_surface & face_,
                          placement & face_placement_) const;

    /// Compute the side bottom face and placement
    void compute_side_face(composite_surface & face_) const;

    /// Compute informations about the faces of this solid shape
    unsigned int compute_faces(face_info_collection_type &) const override;

    /// Compute the volume
    double get_volume(uint32_t flags_ = 0) const override;

    /// Compute the surface
    double get_surface(uint32_t mask_ = FACE_ALL) const override;

    /// Check if a point is inside the frustrum
    bool is_inside(const vector_3d &,
                           double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Check if a point is outside the frustrum
    bool is_outside(const vector_3d &,
                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Return the surface bit a point belongs to
    face_identifier on_surface(const vector_3d &,
                                       const face_identifier & a_surface_mask = face_identifier::face_bits_any(),
                                       double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Compute the normal to the surface of the furstrum
    vector_3d get_normal_on_surface (const vector_3d & position_,
                                             const face_identifier & ) const override;

    /// Find the intercept point with a face of the frustrum
    bool find_intercept(const vector_3d & from_,
                                const vector_3d & direction_,
                                face_intercept_info & intercept_,
                                double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_          = false) const override;

    /// \brief 3D rendering options
    enum wall_wires_rendering_option_type {
      WR_WALL_NO_BOTTOM_FACE      = (WR_BASE_LAST << 1),           //!< Do not render the bottom face
      WR_WALL_NO_TOP_FACE         = (WR_BASE_LAST << 2),           //!< Do not render the top face
      WR_WALL_NO_SIDE_FACE        = (WR_BASE_LAST << 3),           //!< Do not render the side face
      WR_WALL_LAST                = (WR_WALL_NO_SIDE_FACE),  //!< Last defined bit
      WR_WALL_MASK                = (WR_WALL_NO_BOTTOM_FACE
                                     | WR_WALL_NO_TOP_FACE
                                     | WR_WALL_NO_SIDE_FACE
                                     ) //!< Rendering options bit mask
    };

    /// Generate a list of polylines representing the contour of the shape (for display clients)
    void generate_wires_self(wires_type & wires_,
                                     uint32_t options_ = 0) const override;

    /// Convert the wall to a tessellated solid
    void convert_to_tessellated(tessellated_solid &) const;

  protected:

    /// Build bounding data
    void _build_bounding_data() override;

    /// Set default attributes
    void _set_defaults();

  private:

    double         _z_;    //!< Height of the wall
    simple_polygon _base_; //!< Simple polygon base of the wall

    // Working data (not used yet):
    // bool    _computed_; //!< Flag indicating that internal working data have been computed
    // boost::logic::tribool _has_top_face_;
    // boost::logic::tribool _has_bottom_face_;
    // boost::logic::tribool _has_inner_face_;
    // double  _top_surface_;
    // double  _bottom_surface_;
    // double  _side_surface_;
    // double  _volume_;

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(wall_solid)

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_WALL_SOLID_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
