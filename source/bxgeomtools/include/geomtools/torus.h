/// \file geomtools/torus.h
/* Author(s):     F. Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2016-02-18
 * Last modified: 2016-02-19
 *
 * License:
 *
 * Description:
 *   3D solid torus sector
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_TORUS_H
#define GEOMTOOLS_TORUS_H 1

// Standard library:
#include <iostream>
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/object_configuration_description.h>

// This project:
#include <geomtools/i_shape_3d.h>
#include <geomtools/i_stackable.h>
#include <geomtools/angular_range.h>

namespace datatools {
  // Forward class declaration:
  class properties;
}

namespace geomtools {

  // Forward class declarations:
  class toroid_nappe;
  class disk;

  /// \brief The 3D shape model for a solid torus
  class torus : public i_shape_3d,
                public i_stackable
  {
  public:

    /// \brief Masks used for the faces of the solid torus
    enum faces_mask_type {
      FACE_NONE      = face_identifier::FACE_BITS_NONE,
      FACE_INSIDE    = datatools::bit_mask::bit00,
      FACE_OUTSIDE   = datatools::bit_mask::bit01,
      FACE_START_PHI = datatools::bit_mask::bit02,
      FACE_STOP_PHI  = datatools::bit_mask::bit03,
      FACE_ALL    = (FACE_INSIDE
                     | FACE_OUTSIDE
                     | FACE_START_PHI
                     | FACE_STOP_PHI)
    };

    /// Return the solid torus shape label
    static const std::string & torus_label();

    /// Compute the inside/outside face and placement
    void compute_inside_outside_face(faces_mask_type face_id_,
                                     toroid_nappe & face_,
                                     placement & face_placement_) const;

    /// Compute the start/stop phi face and placement
    void compute_start_stop_phi_face(faces_mask_type face_id_,
                                     disk & face_,
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

    /// Return the radius of the solid torus
    double get_sweep_radius() const;

    /// Set the radius of the solid torus
    void set_sweep_radius(double);

    /// Check inside radius
    bool has_inside_radius() const;

    /// Return the inside radius
    double get_inside_radius () const;

    /// Set the inside radius
    void set_inside_radius (double);

    /// Return the outside radius
    double get_outside_radius () const;

    /// Set the outside radius
    void set_outside_radius (double);

    /// Set the dimensions and angular limits
    void set(double sweep_radius_, double outside_radius_, double inside_radius_, double start_phi_, double delta_phi_);

    /// Set the radii
    void set(double sweep_radius_, double outside_radius_, double inside_radius_ = 0.0);

    /// Set phis
    void set_phi(double start_phi_, double delta_phi_);

    /// Check if the sector has partial phi
    bool has_partial_phi() const;

    /// Check the start phi
    bool has_start_phi() const;

    /// Set the start phi
    void set_start_phi(double);

    /// Return the start phi
    double get_start_phi() const;

    /// Check the delta phi
    bool has_delta_phi() const;

    /// Set the delta phi
    void set_delta_phi(double);

    /// Return the delta phi
    double get_delta_phi() const;

    /// Default constructor
    torus();

    /// Constructor
    torus(double sweep_radius_, double outside_radius_, double inside_radius_);

    /// Constructor
    torus(double sweep_radius_, double outside_radius_, double inside_radius_, double start_phi_, double delta_phi_);

    /// Destructor
    ~torus() override;

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
    operator<< (std::ostream &, const torus &);

    friend std::istream &
    operator>> (std::istream &, torus &);

    /// Smart print
    void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const override;

    /// \brief 3D rendering options
    enum torus_wires_rendering_option_type {
      WR_TORUS_NO_INSIDE_FACE    = (WR_BASE_LAST << 1),    //!< Do not render the inside face
      WR_TORUS_NO_OUTSIDE_FACE   = (WR_BASE_LAST << 2),    //!< Do not render the outside face
      WR_TORUS_NO_START_PHI_FACE = (WR_BASE_LAST << 3),    //!< Do not render the start phi face
      WR_TORUS_NO_STOP_PHI_FACE  = (WR_BASE_LAST << 3),    //!< Do not render the stop phi face
      WR_TORUS_LAST              = (WR_TORUS_NO_STOP_PHI_FACE),  //!< Last defined bit
      WR_TORUS_MASK              = (WR_TORUS_NO_INSIDE_FACE
                                    | WR_TORUS_NO_OUTSIDE_FACE
                                    | WR_TORUS_NO_START_PHI_FACE
                                    | WR_TORUS_NO_STOP_PHI_FACE) //!< Rendering options bit mask
    };

    /// Generate a sequence of polylines for wires 3D rendering
    void generate_wires_self(wires_type & wires_,
                                     uint32_t options_ = 0) const override;

    /// OCD support
    static void init_ocd(datatools::object_configuration_description &);

  protected:

    /// Set default attributes
    void _set_defaults();

    /// Build bounding data
    void _build_bounding_data() override;

  private:

    double _sweep_radius_;   //!< Sweep radius of solid torus
    double _inside_radius_;  //!< Inside radius
    double _outside_radius_; //!< Outside radius
    angular_range _phi_domain_; //!< Angular spread of the torus

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(torus)

  };

} // end of namespace geomtools

/// OCD support declaration
// @arg geomtools::torus the name the class
DOCD_CLASS_DECLARATION(geomtools::torus)

#endif // GEOMTOOLS_TORUS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
