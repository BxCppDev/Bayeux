/// \file geomtools/cylinder.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-05-24
 * Last modified: 2021-04-24
 *
 * Description:
 *
 *  A cylinder 3D solid shape on z-axis
 *
 */

#ifndef GEOMTOOLS_CYLINDER_H
#define GEOMTOOLS_CYLINDER_H 1

// Standard library:
#include <sstream>
#include <stdexcept>

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

  // Class forward declarations:
  class disk;
  class cylindrical_sector;

  /// \brief The 3D shape model for a cylinder
  class cylinder : public i_shape_3d ,
                   public i_stackable
  {

  public:

    /// Return the cylinder shape label
    static const std::string & cylinder_label();

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

    /// Compute the top or bottom face and placement
    void compute_top_bottom_face(faces_mask_type face_id_,
                                 disk & face_,
                                 placement & face_placement_) const;

    /// Compute the side bottom face and placement
    void compute_side_face(cylindrical_sector & face_,
                           placement & face_placement_) const;

    /// Return a collection of face info objects
    unsigned int compute_faces(face_info_collection_type & faces_) const override;

    /// Compute a deflated version of the cylinder
    void compute_deflated(cylinder & deflated_,
                          double by_r_,
                          double by_z_);

    /// Compute an inflated version of the cylinder
    void compute_inflated(cylinder & deflated_,
                          double by_r_,
                          double by_z_);
 
    /// Compute an envelope cylinder at user tolerance
    void compute_envelope(cylinder & envelope_,
                          double tolerance_r_,
                          double tolerance_z_);
    
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

    /// Return the diameter
    void set_diameter (double);

    /// Return half the Z dimension
    double get_half_z () const;

    /// Set half the Z dimension
    void set_half_z (double);

    /// Return the radius
    double get_r () const;

    /// Return the radius
    double get_radius () const;

    /// Return the diameter
    double get_diameter () const;

    /// Set the radius
    void set_r (double);

    /// Set the radius ad Z dimension
    void set (double r_, double z_);

    void set_alternative (double, double);

    /// Default constructor
    cylinder ();

    /// Constructor
    cylinder (double a_radius, double a_z);

    /// Destructor
    ~cylinder () override;

    /// Return the name of the shape
    std::string get_shape_name () const override;

    /// Return a parameter by name
    virtual double get_parameter (const std::string &) const;

    /// Check the validity
    bool is_valid () const override;

    /// Initialize
    void init ();

    /// Initialize the cylinder from properties
    void initialize(const datatools::properties &, const handle_dict_type * = 0) override;

    /// Reset the cylinder
    void reset () override;

    /// Compute the surface of the cylinder
    double get_surface (uint32_t a_mask = FACE_ALL) const override;

    /// Compute the volume of the cylinder
    double get_volume (uint32_t flags_ = 0) const override;

    /// Check if a point is inside the cylinder
    bool is_inside (const vector_3d &,
                            double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Check if a point is outside the cylinder
    bool is_outside (const vector_3d &,
                             double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Return the surface bit a point belongs to
    face_identifier on_surface(const vector_3d &,
                                       const face_identifier & a_surface_mask = face_identifier::face_bits_any(),
                                       double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Return the vector normal to the surface at some position
    vector_3d get_normal_on_surface(const vector_3d & a_position,
                                            const face_identifier & a_surface_bit) const override;

    /// Find the intercept point of a segment with the surface
    bool find_intercept (const vector_3d & a_from,
                                 const vector_3d & a_direction,
                                 face_intercept_info & a_intercept,
                                 double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const override;

    friend std::ostream & operator<< (std::ostream &, const cylinder &);

    friend std::istream & operator>> (std::istream &, cylinder &);

    /// Smart print
    void tree_dump (std::ostream & a_out         = std::clog,
                            const std::string & a_title  = "",
                            const std::string & a_indent = "",
                            bool a_inherit          = false) const override;

    /// \brief 3D rendering options
    enum cylinder_wires_rendering_option_type {
      WR_CYL_NO_BOTTOM_FACE = (WR_BASE_LAST << 1),    //!< Do not render the bottom face
      WR_CYL_NO_TOP_FACE    = (WR_BASE_LAST << 2),    //!< Do not render the top face
      WR_CYL_NO_SIDE_FACE   = (WR_BASE_LAST << 3),    //!< Do not render the side face
      WR_CYL_LAST           = (WR_CYL_NO_SIDE_FACE),  //!< Last defined bit
      WR_CYL_MASK           = (WR_CYL_NO_BOTTOM_FACE
                               | WR_CYL_NO_TOP_FACE
                               | WR_CYL_NO_SIDE_FACE) //!< Rendering options bit mask
    };

    /// Generate a list of polylines representing the contour of the shape (for display clients)
    void generate_wires_self(wires_type & wires_,
                             uint32_t options_ = 0) const override;

    /// OCD support
    static void init_ocd(datatools::object_configuration_description &);

  protected:

    /// Set default attributes
    void _set_defaults();

    /// Build the bounding data
    void _build_bounding_data() override;

  private:

    double _z_;      //<! Height
    double _radius_; //<! Radius

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(cylinder)

  };

} // end of namespace geomtools

/// OCD support declaration
// @arg geomtools::cylinder the name the class
DOCD_CLASS_DECLARATION(geomtools::cylinder)

#endif // GEOMTOOLS_CYLINDER_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
