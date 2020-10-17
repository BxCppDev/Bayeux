/// \file geomtools/elliptical_cylinder.h
/* Author(s):     A. Chapon <chapon@lpccaen.in2p3.fr>
 *                F. Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2014-11-17
 * Last modified: 2015-03-11
 *
 * License:
 *
 * Description:
 *   3D elliptical cylinder description
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_ELLIPTICAL_CYLINDER_H
#define GEOMTOOLS_ELLIPTICAL_CYLINDER_H 1

// Standard library:
#include <iostream>
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/object_configuration_description.h>

// This project:
#include <geomtools/i_shape_3d.h>
#include <geomtools/i_stackable.h>

namespace datatools {
  // Forward class declaration:
  class properties;
}

namespace geomtools {

  // Forward class declarations:
  class elliptical_sector;
  class elliptical_cylinder_sector;

  /// \brief The 3D shape model for an elliptical cylinder
  class elliptical_cylinder : public i_shape_3d,
                              public i_stackable
  {
  public:

    /// \brief Masks used for the faces of the elliptical cylinder
    enum faces_mask_type {
      FACE_NONE   = face_identifier::FACE_BITS_NONE,
      FACE_SIDE   = datatools::bit_mask::bit00,
      FACE_BOTTOM = datatools::bit_mask::bit01,
      FACE_TOP    = datatools::bit_mask::bit02,
      FACE_ALL    = (FACE_SIDE
                     | FACE_BOTTOM
                     | FACE_TOP)
    };

    /// Return the elliptical cylinder shape label
    static const std::string & elliptical_cylinder_label();

    /// Compute the top or bottom face and placement
    void compute_top_bottom_face(faces_mask_type face_id_,
                                 elliptical_sector & face_,
                                 placement & face_placement_) const;

    /// Compute the side face and placement
    void compute_side_face(elliptical_cylinder_sector & face_,
                           placement & face_placement_) const;

    /// Compute informations about the faces of this solid shape
    unsigned int compute_faces(face_info_collection_type & faces) const override;

    /// Return the min X coordinates (bounding box)
    double get_xmin () const override;

    /// Return the max X coordinates (bounding box)
    double get_xmax () const override;

    /// Return the min Y coordinates (bounding box)
    double get_ymin () const override;

    /// Return the max Y coordinates (bounding box)
    double get_ymax () const override;

    /// Return the min Z coordinates (bounding box)
    double get_zmin () const override;

    /// Return the max Z coordinates (bounding box)
    double get_zmax () const override;

    /// Return the Z dimension
    double get_z () const;

    /// Set the Z dimension
    void set_z (double);

    /// Return half the Z dimension
    double get_half_z() const;

    /// Set half the Z dimension
    void set_half_z (double);

    /// Return the X radius
    double get_x_radius () const;

    /// Set the X radius
    void set_x_radius (double);

    /// Return the Y radius
    double get_y_radius () const;

    /// Set the Y radius
    void set_y_radius (double);

    /// Set the X and Y radii and Z
    void set (double xr_, double yr_, double z_);

    /// Default constructor
    elliptical_cylinder();

    /// Constructor
    elliptical_cylinder (double xr_, double yr_, double z_);

    /// Destructor
    ~elliptical_cylinder () override;

    /// Return the name of the shape
    std::string get_shape_name () const override;

    /// Return a parameter by name
    virtual double get_parameter (const std::string &) const;

    /// Check the validity
    bool is_valid () const override;

    /// Initialize from properties
    void initialize(const datatools::properties &,
                            const handle_dict_type *) override;

    /// Reset
    void reset() override;

    /// Compute the surface of the cylinder
    double get_surface (uint32_t mask_ = FACE_ALL) const override;

    /// Compute the volume of the cylinder
    double get_volume (uint32_t flags_ = 0) const override;

    /// Check if a point is inside the cylinder
    bool is_inside (const vector_3d &,
                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Check if a point is outside the cylinder
    bool is_outside (const vector_3d &,
                             double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Return the surface bit a point belongs to
    face_identifier on_surface(const vector_3d &,
                                       const face_identifier & a_surface_mask = face_identifier::face_bits_any(),
                                       double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Return the vector normal to the surface at some position
    vector_3d get_normal_on_surface(const vector_3d & a_position,
                                            const face_identifier & a_surface_bit) const override;

    /// Find the intercept point of a segment with the surface
    bool find_intercept (const vector_3d & from_,
                                 const vector_3d & direction_,
                                 face_intercept_info & intercept_,
                                 double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    friend std::ostream &
    operator<< (std::ostream &, const elliptical_cylinder &);

    friend std::istream &
    operator>> (std::istream &, elliptical_cylinder &);

    /// Smart print
    void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const override;

    /// \brief 3D rendering options
    enum elliptical_cylinder_wires_rendering_option_type {
      WR_ELLCYL_NO_BOTTOM_FACE = (WR_BASE_LAST << 1),    //!< Do not render the bottom face
      WR_ELLCYL_NO_TOP_FACE    = (WR_BASE_LAST << 2),    //!< Do not render the top face
      WR_ELLCYL_NO_SIDE_FACE   = (WR_BASE_LAST << 3),    //!< Do not render the side face
      WR_ELLCYL_LAST           = (WR_ELLCYL_NO_SIDE_FACE),  //!< Last defined bit
      WR_ELLCYL_MASK           = (WR_ELLCYL_NO_BOTTOM_FACE
                                  | WR_ELLCYL_NO_TOP_FACE
                                  | WR_ELLCYL_NO_SIDE_FACE) //!< Rendering options bit mask
    };

    /// Generate a sequence of polylines for wires 3D rendering
    void generate_wires_self(wires_type & wires_,
                                     uint32_t options_ = 0) const override;

    /// OCD support
    static void init_ocd(datatools::object_configuration_description &);

  protected:

    /// Set default attributes
    void _set_default();

    /// Build bounding data
    void _build_bounding_data() override;

  private:

    double _x_radius_; //!< X dimension
    double _y_radius_; //!< Y dimension
    double _z_;        //!< Z dimension

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(elliptical_cylinder)

  };

  // Type alias:
  typedef elliptical_cylinder elliptical_tube;

} // end of namespace geomtools

/// OCD support declaration
// @arg geomtools::elliptical_cylinder the name the class
DOCD_CLASS_DECLARATION(geomtools::elliptical_cylinder)

#endif // GEOMTOOLS_ELLIPTICAL_CYLINDER_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
