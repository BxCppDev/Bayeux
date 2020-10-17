/// \file geomtools/tube.h
/* Author(s):     F.Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2006-11-28
 * Last modified: 2015-02-22
 *
 * License:
 *
 * Description:
 *   3D tube description
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_TUBE_H
#define GEOMTOOLS_TUBE_H 1

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

  // Class forward declarations:
  class cylinder;
  class disk;
  class cylindrical_sector;
  class rectangle;

  /// \brief The 3D shape model for a tube
  class tube : public i_shape_3d,
               public i_stackable
  {
  public:

    /// \brief Face flags
    enum faces_mask_type {
      FACE_NONE       = face_identifier::FACE_BITS_NONE,
      FACE_OUTER_SIDE = datatools::bit_mask::bit00,
      FACE_BOTTOM     = datatools::bit_mask::bit01,
      FACE_TOP        = datatools::bit_mask::bit02,
      FACE_INNER_SIDE = datatools::bit_mask::bit03,
      FACE_START_ANGLE = datatools::bit_mask::bit04,
      FACE_STOP_ANGLE = datatools::bit_mask::bit05,
      FACE_ALL        = (FACE_OUTER_SIDE
                         | FACE_BOTTOM
                         | FACE_TOP
                         | FACE_INNER_SIDE
                         | FACE_START_ANGLE
                         | FACE_STOP_ANGLE)
    };

    /// \brief Volume flags
    enum volumes_mask_type {
      VOLUME_NONE   = 0,
      VOLUME_BULK   = datatools::bit_mask::bit00,
      VOLUME_CAVITY = datatools::bit_mask::bit01
    };

    /// Return the identifier label for the class
    static const std::string & tube_label();

    /// Compute the top or bottom face and placement
    void compute_top_bottom_face(faces_mask_type face_id_,
                                 disk & face_,
                                 placement & face_placement_) const;

    /// Compute the side bottom face and placement
    void compute_side_face(faces_mask_type face_id_,
                           cylindrical_sector & face_,
                           placement & face_placement_) const;

    /// Compute the top or bottom face and placement
    void compute_start_stop_angle_face(faces_mask_type face_id_,
                                       rectangle & face_,
                                       placement & face_placement_) const;

    /// Check for an inner face
    bool has_inner_face() const;

    /// Return a collection of face info objects
    unsigned int compute_faces(face_info_collection_type & faces_) const override;

    /// Return the minimum X
    double get_xmin() const override;

    /// Return the maximum X
    double get_xmax() const override;

    /// Return the minimum Y
    double get_ymin() const override;

    /// Return the maximum Y
    double get_ymax() const override;

    /// Return the minimum Z
    double get_zmin() const override;

    /// Return the maximum Z
    double get_zmax() const override;

    /// Return the Z dimension
    double get_z() const;

    /// Set the Z dimension
    void set_z(double);

    /// Return half the Z dimension
    double get_half_z() const;

    /// Set half the Z dimension
    void set_half_z(double);

    /// Check the inner radius
    bool has_inner_r() const;

    /// Return the inner radius
    double get_inner_r() const;

    /// Set the inner radius
    void set_inner_r(double);

    /// Set the inner and outer radii
    void set_radii(double inner_r_, double outer_r_);

    /// Return the outer radius
    double get_outer_r() const;

    /// Set the inner radius
    void set_outer_r(double);

    /// Set the dimensions
    void set(double outer_r_, double z_);

    /// Set the dimensions
    void set(double inner_r_, double outer_r_, double z_);

    /// Set the dimensions by halves
    void set_half(double inner_r_, double outer_r_, double half_z_);

    /// Set the dimensions and angular limits
    void set(double inner_r_, double outer_r_, double z_, double start_phi_, double delta_phi_);

    /// Return the phi domain
    const angular_range & get_phi_domain() const;

    /// Check for the partial arc
    bool has_partial_phi() const;

    /// Check the start phi
    bool has_start_phi() const;

    /// Set the start phi
    void set_start_phi(double);

    /// Return the start phi
    double get_start_phi() const;

    /// Check the dedislta phi
    bool has_delta_phi() const;

    /// Set the delta phi
    void set_delta_phi(double);

    /// Return the delta phi
    double get_delta_phi() const;

    void set_phi(double start_phi_, double delta_phi_);

    /// Compute the inner nappe
    void compute_inner_cylinder(cylinder & ic_);

    /// Compute the outer nappe
    void compute_outer_cylinder(cylinder & oc_);

    /// Default constructor
    tube();

    /// Constructor
    tube(double outer_radius_, double z_);

    /// Constructor
    tube(double inner_radius_, double outer_radius_, double z_);

    /// Constructor
    tube(double inner_radius_, double outer_radius_, double z_, double start_phi_, double delta_phi_);

    /// Destructor
    ~tube() override;

    /// Return the identifier/name of the shape
    std::string get_shape_name() const override;

    /// Return a parameter given its name
    virtual double get_parameter(const std::string &) const;

    /// Check if the tube is extruded
    bool is_extruded() const;

    /// Check if the tube is valid
    bool is_valid() const override;

    /// Initialize the tube from properties
    void initialize(const datatools::properties &, const handle_dict_type * = 0) override;

    /// Reset the tube
    void reset() override;

    /// Compute the surface
    double get_surface(uint32_t mask_ = FACE_ALL) const override;

    /// Compute the volume
    double get_volume(uint32_t flags_ = VOLUME_BULK) const override;

    /// Check if a point is inside the tube
    bool is_inside(const vector_3d &,
                           double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Check if a point is outside the tube
    bool is_outside(const vector_3d &,
                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Return the surface bit a point belongs to
    face_identifier on_surface(const vector_3d &,
                                       const face_identifier & a_surface_mask = face_identifier::face_bits_any(),
                                       double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Return the vector normal to the surface at some position
    vector_3d get_normal_on_surface(const vector_3d & a_position,
                                            const face_identifier & a_surface_bit) const override;

    /// Find the intercept point with a face of the tube
    bool find_intercept(const vector_3d & from_,
                                const vector_3d & direction_,
                                face_intercept_info & intercept_,
                                double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Output operator
    friend std::ostream &
    operator<<(std::ostream &, const tube &);

    /// Input operator
    friend std::istream &
    operator>>(std::istream &, tube &);

    /// Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_          = false) const override;

    /// \brief 3D rendering options
    enum tube_wires_rendering_option_type {
      WR_TUBE_NO_BOTTOM_FACE      = (WR_BASE_LAST << 1),           //!< Do not render the bottom face
      WR_TUBE_NO_TOP_FACE         = (WR_BASE_LAST << 2),           //!< Do not render the top face
      WR_TUBE_NO_INNER_FACE       = (WR_BASE_LAST << 3),           //!< Do not render the inner face
      WR_TUBE_NO_OUTER_FACE       = (WR_BASE_LAST << 4),           //!< Do not render the outer face
      WR_TUBE_NO_START_ANGLE_FACE = (WR_BASE_LAST << 5),           //!< Do not render the start angle face
      WR_TUBE_NO_STOP_ANGLE_FACE  = (WR_BASE_LAST << 6),           //!< Do not render the stop angle face
      WR_TUBE_LAST                = (WR_TUBE_NO_STOP_ANGLE_FACE),  //!< Last defined bit
      WR_TUBE_MASK                = (WR_TUBE_NO_BOTTOM_FACE
                                     | WR_TUBE_NO_TOP_FACE
                                     | WR_TUBE_NO_INNER_FACE
                                     | WR_TUBE_NO_OUTER_FACE
                                     | WR_TUBE_NO_START_ANGLE_FACE
                                     | WR_TUBE_NO_STOP_ANGLE_FACE) //!< Rendering options bit mask
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

    double _z_;       //!< Length of the tube along the Z axis
    double _inner_r_; //!< Inner radius
    double _outer_r_; //!< Outer radius
    angular_range _phi_domain_; //!< Angular spread of the tube

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(tube)

  };

} // end of namespace geomtools

/// OCD support declaration
// @arg geomtools::tube the name the class
DOCD_CLASS_DECLARATION(geomtools::tube)

#endif // GEOMTOOLS_TUBE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
