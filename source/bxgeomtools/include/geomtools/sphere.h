/// \file geomtools/sphere.h
/* Author(s):     F.Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2006-11-28
 * Last modified: 2014-04-08
 *
 * License:
 *
 * Description:
 *   3D sphere description
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_SPHERE_H
#define GEOMTOOLS_SPHERE_H 1

// Standard library:
#include <string>
#include <iostream>

// Third party
// - Bayeux/datatools:
#include <datatools/bit_mask.h>

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
  class disk;
  class spherical_sector;
  class right_circular_conical_nappe;

  /// \brief A spherical volume (3D solid)
  class sphere : public i_shape_3d,
                 public i_stackable
  {
  public:

    enum faces_mask_type {
      FACE_NONE             = face_identifier::FACE_BITS_NONE,
      FACE_SIDE             = datatools::bit_mask::bit00,
      FACE_OUTER_SIDE       = FACE_SIDE,
      FACE_INNER_SIDE       = datatools::bit_mask::bit01,
      FACE_START_THETA_SIDE = datatools::bit_mask::bit02,
      FACE_STOP_THETA_SIDE  = datatools::bit_mask::bit03,
      FACE_START_PHI_SIDE   = datatools::bit_mask::bit04,
      FACE_STOP_PHI_SIDE    = datatools::bit_mask::bit05,
      FACE_ALL              = FACE_OUTER_SIDE
      | FACE_INNER_SIDE
      | FACE_START_THETA_SIDE
      | FACE_STOP_THETA_SIDE
      | FACE_START_PHI_SIDE
      | FACE_STOP_PHI_SIDE
    };

    static const std::string & sphere_label();

    double get_xmin() const override;

    double get_xmax() const override;

    double get_ymin() const override;

    double get_ymax() const override;

    double get_zmin() const override;

    double get_zmax() const override;

    double get_r() const;

    double get_r_max() const;

    double get_radius() const;

    bool has_r_min() const;

    double get_r_min() const;

    void set_r(double);

    void set_r_max(double);

    void set_radius(double);

    void set_r_min(double);

    void reset_r_max();

    void reset_r_min();

    void set(double rmax_);

    void set(double rmin_, double rmax_);


    /// Return the theta domain (polar angle)
    const angular_range & get_theta_domain() const;

    /// Check if the sector has partial theta angle
    bool has_partial_theta() const;

    /// Check the start theta angle
    bool has_start_theta() const;

    /// Set the start theta angle
    void set_start_theta(double);

    /// Return the start theta angle
    double get_start_theta() const;

    /// Check the delta theta angle
    bool has_delta_theta() const;

    /// Set the delta theta angle
    void set_delta_theta(double);

    /// Return the delta angle
    double get_delta_theta() const;

    /// Return the phi domain (azimuthal angle)
    const angular_range & get_phi_domain() const;

    /// Check if the sector has partial phi angle
    bool has_partial_phi() const;

    /// Check the start phi angle
    bool has_start_phi() const;

    /// Set the start phi angle
    void set_start_phi(double);

    /// Return the start phi angle
    double get_start_phi() const;

    /// Check the delta phi angle
    bool has_delta_phi() const;

    /// Set the delta phi angle
    void set_delta_phi(double);

    /// Return the delta angle
    double get_delta_phi() const;

    void set_phi(double start_phi_, double delta_phi_);

    void set_theta(double start_theta_, double delta_theta_);

    bool is_orb() const;

    /// Default constructor
    sphere();

    /// Constructor
    sphere(double radius_);

    /// Constructor
    sphere(double radius_min_, double radius_max_);

    /// Destructor
    ~sphere() override;

    /// Return the name of the shape
    std::string get_shape_name() const override;

    /// Return the value of some parameter
    virtual double get_parameter(const std::string &) const;

    /// Check the validity of the sphere
    bool is_valid() const override;

    /// Initialize the sphere from properties
    void initialize(const datatools::properties &, const handle_dict_type * = 0) override;

    /// Reset the sphere
    void reset() override;

    /// Compute the surface
     double get_surface(uint32_t mask_ = FACE_ALL) const override;

    /// Compute the volume
    double get_volume(uint32_t flags_ = 0) const override;

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

    /// Find the intercept point with a face of the sphere
    bool find_intercept(const vector_3d & from_,
                                const vector_3d & direction_,
                                face_intercept_info & intercept_,
                                double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Compute the side face
    void compute_side_face(faces_mask_type, spherical_sector &) const;

    /// Compute the inner side face
    void compute_inner_side_face(spherical_sector &) const;

    /// Compute the outer side face
    void compute_outer_side_face(spherical_sector &) const;

    /// Compute the start/stop theta face
    void compute_start_stop_theta_face(faces_mask_type, right_circular_conical_nappe &, disk &, placement &) const;

    /// Compute the start theta face
    void compute_start_theta_face(right_circular_conical_nappe &, disk &, placement &) const;

    /// Compute the stop face
    void compute_stop_theta_face(right_circular_conical_nappe &, disk &, placement &) const;

    /// Compute the start/stop phi face
    void compute_start_stop_phi_face(faces_mask_type, disk &, placement &) const;

    /// Compute the start phi face
    void compute_start_phi_face(disk &, placement &) const;

    /// Compute the stop phi face
    void compute_stop_phi_face(disk &, placement &) const;

    /// Return a collection of face info objects
    unsigned int compute_faces(face_info_collection_type & faces_) const override;


    friend std::ostream &
    operator<<(std::ostream &, const sphere &);

    friend std::istream &
    operator>>(std::istream &, sphere &);

    void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_               = false) const override;

    /// \brief 3D rendering options
    enum sphere_wires_rendering_option_type {
      WR_SPHERE_NO_INNER_FACE       = (WR_BASE_LAST << 1),          //!< Do not render the internal face
      WR_SPHERE_NO_OUTER_FACE       = (WR_BASE_LAST << 2),          //!< Do not render the external face
      WR_SPHERE_NO_START_THETA_FACE = (WR_BASE_LAST << 3),          //!< Do not render the start theta face
      WR_SPHERE_NO_STOP_THETA_FACE  = (WR_BASE_LAST << 4),          //!< Do not render the stop theta face
      WR_SPHERE_NO_START_PHI_FACE   = (WR_BASE_LAST << 5),          //!< Do not render the start phi face
      WR_SPHERE_NO_STOP_PHI_FACE    = (WR_BASE_LAST << 6),          //!< Do not render the stop phi face
      WR_SPHERE_LAST                = (WR_SPHERE_NO_STOP_PHI_FACE), //!< Last defined bit
      WR_SPHERE_MASK                = (WR_SPHERE_NO_INNER_FACE
                                       | WR_SPHERE_NO_OUTER_FACE
                                       | WR_SPHERE_NO_START_THETA_FACE
                                       | WR_SPHERE_NO_STOP_THETA_FACE
                                       | WR_SPHERE_NO_START_PHI_FACE
                                       | WR_SPHERE_NO_STOP_PHI_FACE
                                       )  //!< Rendering options bit mask
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

    double _r_;           //!< Outer radius
    double _r_min_;       //!< Inner radius
    angular_range _theta_domain_; //!< Angular spread of the polar arc
    angular_range _phi_domain_;   //!< Angular spread of the azimuthal arc

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(sphere)

  };

} // end of namespace geomtools

/// OCD support declaration
// @arg geomtools::sphere the name the class
DOCD_CLASS_DECLARATION(geomtools::sphere)

#endif // GEOMTOOLS_SPHERE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
