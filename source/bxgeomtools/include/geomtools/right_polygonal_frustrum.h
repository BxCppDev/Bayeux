/// \file geomtools/right_polygonal_frustrum.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-03-15
 * Last modified: 2015-03-21
 *
 * License:
 *
 * Description:
 *
 *   Right polygonal frustrum
 *
 */

#ifndef GEOMTOOLS_RIGHT_POLYGONAL_FRUSTRUM_H
#define GEOMTOOLS_RIGHT_POLYGONAL_FRUSTRUM_H 1

// This project:
#include <geomtools/i_shape_3d.h>
#include <geomtools/i_stackable.h>

namespace geomtools {

  // Class forward declarations:
  class composite_surface;

  /// \brief Right polygonal frustrum (3D shape)
  class right_polygonal_frustrum : public i_shape_3d,
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
      FACE_ALL        = (FACE_OUTER_SIDE
                         | FACE_BOTTOM
                         | FACE_TOP
                         | FACE_INNER_SIDE)
    };

    /// \brief Volume flags
    enum volumes_mask_type {
      VOLUME_NONE   = 0,
      VOLUME_BULK   = datatools::bit_mask::bit00,
      VOLUME_CAVITY = datatools::bit_mask::bit01
    };

    /// The minimum number of sides
    static const unsigned int MIN_NUMBER_OF_SIDES = 3;

    /// Return the identifier label for the class
    static const std::string & rpf_label();

    /// Return the identifier/name of the shape
    std::string get_shape_name() const override;

    /// Check if the frustrum is valid
    bool is_valid() const override;

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

    /// Return the number of sides
    uint32_t get_n_sides() const;

    /// Set  the number of sides
    void set_n_sides(uint32_t);

    /// Return the starting sector index
    uint32_t get_sector_start() const;

    /// Return the stopping sector index
    uint32_t get_sector_stop() const;

    /// Get the sector angle
    double get_sector_angle() const;

    /// Get the start angle per sector
    double get_start_angle(unsigned int sector_) const;

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

    /// Default constructor
    right_polygonal_frustrum();

    /// Constructor
    right_polygonal_frustrum(unsigned int n_sides_,
                             double inner_bottom_radius_,
                             double outer_bottom_radius_,
                             double inner_top_radius_,
                             double outer_top_radius_,
                             double z_);

    /// Destructor
    ~right_polygonal_frustrum() override;

    /// Smart print
    void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const override;

    /// Compute the inner face
    void compute_inner_face(composite_surface & face_) const;

    /// Compute the outer face
    void compute_outer_face(composite_surface & face_) const;

    /// Compute the top face
    void compute_top_face(composite_surface & top_face_, placement &) const;

    /// Compute the bottom face
    void compute_bottom_face(composite_surface & bottom_face_, placement &) const;

    /// Compute informations about the faces of this solid shape
    unsigned int compute_faces(face_info_collection_type &) const override;

    /// Compute the outer radius at given z
    double compute_outer_radius(double z_) const;

    /// Compute the inner radius at given z
    double compute_inner_radius(double z_) const;

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
    enum rpf_wires_rendering_option_type {
      WR_RPF_NO_BOTTOM_FACE      = (WR_BASE_LAST << 1),           //!< Do not render the bottom edge
      WR_RPF_NO_TOP_FACE         = (WR_BASE_LAST << 2),           //!< Do not render the top edge
      WR_RPF_NO_INNER_FACE       = (WR_BASE_LAST << 3),           //!< Do not render the bottom edge
      WR_RPF_NO_OUTER_FACE       = (WR_BASE_LAST << 4),           //!< Do not render the bottom edge
      WR_RPF_LAST                = (WR_RPF_NO_OUTER_FACE),  //!< Last defined bit
      WR_RPF_MASK                = (WR_RPF_NO_BOTTOM_FACE
                                     | WR_RPF_NO_TOP_FACE
                                     | WR_RPF_NO_INNER_FACE
                                     | WR_RPF_NO_OUTER_FACE) //!< Rendering options bit mask
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

    uint32_t _n_sides_;             //!< The number of sides/vertices of the polygonal frustrum
    uint32_t _sector_start_;        //!< The first angular sector
    uint32_t _sector_stop_;         //!< The last angular sector
    double   _inner_bottom_radius_; //!< The inner bottom radius
    double   _outer_bottom_radius_; //!< The outer bottom radius
    double   _inner_top_radius_;    //!< The inner top radius
    double   _outer_top_radius_;    //!< The outer top radius
    double   _z_;                   //!< The Z dimension

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(right_polygonal_frustrum)

  };

  /// Type alias
  typedef right_polygonal_frustrum polygonal_frustrum;

} // end of namespace geomtools

#endif // GEOMTOOLS_RIGHT_POLYGONAL_FRUSTRUM_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
