/// \file geomtools/right_circular_conical_frustrum.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-03-15
 * Last modified: 2015-03-15
 *
 * License:
 *
 * Description:
 *
 *   Right circular conical frustrum
 *
 */

#ifndef GEOMTOOLS_RIGHT_CIRCULAR_CONICAL_FRUSTRUM_H
#define GEOMTOOLS_RIGHT_CIRCULAR_CONICAL_FRUSTRUM_H 1

// This project:
#include <geomtools/cone.h>
#include <geomtools/i_shape_3d.h>
#include <geomtools/i_stackable.h>
#include <geomtools/angular_range.h>

namespace geomtools {

  // Class forward declarations:
  class disk;
  class triangle;
  class quadrangle;
  class right_circular_conical_nappe;

  /// \brief Right circular conical frustrum (3D shape)
  class right_circular_conical_frustrum : public i_shape_3d,
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
    static const std::string & rccf_label();

    /// Return the identifier/name of the shape
    std::string get_shape_name() const override;

    /// Check if the frustrum is valid
    bool is_valid() const override;

    /// Return the minimum X
    double get_xmin () const override;

    /// Return the maximum X
    double get_xmax () const override;

    /// Return the minimum Y
    double get_ymin () const override;

    /// Return the maximum Y
    double get_ymax () const override;

    /// Return the minimum Z
    double get_zmin () const override;

    /// Return the maximum Z
    double get_zmax () const override;

    /// Set the Z dimension
    void set_z(double);

    /// Return the Z dimension
    double get_z() const;

    /// Check for a top face
    bool has_top_face() const;

    /// Check for a bottom face
    bool has_bottom_face() const;

    /// Check for an inner face
    bool has_inner_face() const;

    /// Check inner bottom radius
    bool has_inner_bottom_radius() const;

    /// Set the inner bottom radius
    void set_inner_bottom_radius(double);

    /// Return the inner bottom radius
    double get_inner_bottom_radius() const;

    /// Set the outer bottom radius
    void set_outer_bottom_radius(double);

    /// Return the outer bottom radius
    double get_outer_bottom_radius() const;

    /// Check inner top radius
    bool has_inner_top_radius() const;

    /// Set the inner top radius
    void set_inner_top_radius(double);

    /// Return the inner top radius
    double get_inner_top_radius() const;

    /// Set the outer top radius
    void set_outer_top_radius(double);

    /// Return the outer top radius
    double get_outer_top_radius() const;

    /// Return the angle domain
    const angular_range & get_angle_domain() const;

    /// Check is the angular spread is partial
    bool has_partial_angle() const;

    /// Check the start angle
    bool has_start_angle() const;

    /// Set the start angle
    void set_start_angle(double);

    /// Return the start angle
    double get_start_angle() const;

    /// Check the dedislta angle
    bool has_delta_angle() const;

    /// Set the delta angle
    void set_delta_angle(double);

    /// Return the delta angle
    double get_delta_angle() const;

    /// Default constructor
    right_circular_conical_frustrum();

    /// Constructor
    right_circular_conical_frustrum(double inner_bottom_radius_,
                                    double outer_bottom_radius_,
                                    double inner_top_radius_,
                                    double outer_top_radius_,
                                    double z_);

    /// Constructor
    right_circular_conical_frustrum(double inner_bottom_radius_,
                                    double outer_bottom_radius_,
                                    double inner_top_radius_,
                                    double outer_top_radius_,
                                    double z_,
                                    double start_angle_,
                                    double delta_angle_);

    /// Destructor
    ~right_circular_conical_frustrum() override;

    /// Smart print
    void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const override;

    /// Compute the inner face
    void compute_inner_face(right_circular_conical_nappe & in_) const;

    /// Compute the outer face
    void compute_outer_face(right_circular_conical_nappe & on_) const;

    /// Compute the top face
    void compute_top_face(disk & top_disk_, placement &) const;

    /// Compute the bottom face
    void compute_bottom_face(disk & bottom_disk_, placement &) const;

    /// \brief Start/stop angle face type
    ///
    ///
    ///
    ///
    enum ssaf_type {
      SSAF_INVALID              = 0, //!< Invalid/undefined type
      SSAF_QUADRANGLE           = 1, //!< Quadrangle type
      SSAF_TRIANGLE_BOTTOM_BASE = 2, //!< Triangle type with bottom base
      SSAF_TRIANGLE_TOP_BASE    = 3  //!< Triangle type with top base
    };

    /// Compute the start/stop phi angle face
    void compute_start_stop_angle_face(quadrangle & qface_, triangle & tface_) const;

    /// Return the type of the start/stop phi angle face
    ssaf_type get_start_stop_angle_face_type() const;

    /// Check if the start/stop angle face is a triangle with bottom base
    bool start_stop_angle_face_is_triangle_bottom() const;

    /// Check if the start/stop angle face is a triangle with top base
    bool start_stop_angle_face_is_triangle_top() const;

    /// Check if the start/stop angle face is a triangle with bottom or top base
    bool start_stop_angle_face_is_triangle() const;

    /// Check if the start/srop angle face is a quadrangle
    bool start_stop_angle_face_is_quadrangle() const;

    /// Compute the start phi angle face
    void compute_start_angle_face(quadrangle & qface_, triangle & tface_, placement &) const;

    /// Compute the stop phi angle face
    void compute_stop_angle_face(quadrangle & qface_, triangle & tface_, placement &) const;

    /// Compute informations about the faces of this solid shape
    unsigned int compute_faces(face_info_collection_type &) const override;

    /// Compute the surface
    double get_surface(uint32_t mask_ = FACE_ALL) const override;

    /// Compute the volume
    double get_volume(uint32_t flags_ = VOLUME_BULK) const override;

    /// Check if a point is inside the frustrum
    bool is_inside (const vector_3d &,
                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Check if a point is outside the frustrum
    bool is_outside (const vector_3d &,
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

    /// Initialize the frustrum from properties
    void initialize(const datatools::properties &, const handle_dict_type * = 0) override;

    /// Reset the frustrum
    void reset() override;

    /// \brief 3D rendering options
    enum rccf_wires_rendering_option_type {
      WR_RCCF_NO_BOTTOM_FACE      = (WR_BASE_LAST << 1),           //!< Do not render the bottom edge
      WR_RCCF_NO_TOP_FACE         = (WR_BASE_LAST << 2),           //!< Do not render the top edge
      WR_RCCF_NO_INNER_FACE       = (WR_BASE_LAST << 3),           //!< Do not render the bottom edge
      WR_RCCF_NO_OUTER_FACE       = (WR_BASE_LAST << 4),           //!< Do not render the bottom edge
      WR_RCCF_NO_START_ANGLE_FACE = (WR_BASE_LAST << 5),           //!< Do not render the start phi angle edge
      WR_RCCF_NO_STOP_ANGLE_FACE  = (WR_BASE_LAST << 6),           //!< Do not render the stop phi angle edge
      WR_RCCF_LAST                = (WR_RCCF_NO_STOP_ANGLE_FACE),  //!< Last defined bit
      WR_RCCF_MASK                = (WR_RCCF_NO_BOTTOM_FACE
                                     | WR_RCCF_NO_TOP_FACE
                                     | WR_RCCF_NO_INNER_FACE
                                     | WR_RCCF_NO_OUTER_FACE
                                     | WR_RCCF_NO_START_ANGLE_FACE
                                     | WR_RCCF_NO_STOP_ANGLE_FACE) //!< Rendering options bit mask
    };

    /// Generate a sequence of polylines for wires 3D rendering
    void generate_wires_self(wires_type & wires_,
                                     uint32_t options_ = 0) const override;

  protected:

    /// Set default attributes
    void _set_defaults();

    /// Build bounding data
    void _build_bounding_data() override;

  private:

    double _inner_bottom_radius_; //!< The inner bottom radius
    double _outer_bottom_radius_; //!< The outer bottom radius
    double _inner_top_radius_;    //!< The inner top radius
    double _outer_top_radius_;    //!< The outer top radius
    double _z_;                   //!< The Z dimension
    angular_range _angle_domain_; //!< Angular spread of the cylindrical sector (longitude/azimuth)

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(right_circular_conical_frustrum)

  };

  /// Type alias
  typedef right_circular_conical_frustrum conical_frustrum;

} // end of namespace geomtools

#endif // GEOMTOOLS_RIGHT_CIRCULAR_CONICAL_FRUSTRUM_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
