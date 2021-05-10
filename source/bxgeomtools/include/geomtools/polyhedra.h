/// \file geomtools/polyhedra.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-10-08
 * Last modified: 2021-04-23
 *
 * Description:
 *   Polyhedra 3D shape with regular polygon sections
 *   See also: http://en.wikipedia.org/wiki/Frustum
 *
 */

#ifndef GEOMTOOLS_POLYHEDRA_H
#define GEOMTOOLS_POLYHEDRA_H 1

// Standard library:
#include <iostream>
#include <string>
#include <map>

// This project:
#include <geomtools/i_shape_3d.h>
#include <geomtools/i_stackable.h>

namespace geomtools {

  // Forward class declaration:
  class composite_surface;
  class right_polygonal_frustrum;

  /// \brief Polyhedra 3D solid shape
  class polyhedra : public i_shape_3d,
                    public i_stackable
  {
  public:

    /// Return the identifier label for the class
    static const std::string & polyhedra_label();

    /// Minimum number of sides
    static const size_t MIN_NUMBER_OF_SIDES = 3;

    /// \brief Face flags
    enum faces_mask_type {
                          FACE_NONE         = face_identifier::FACE_BITS_NONE,
                          FACE_INNER_SIDE   = datatools::bit_mask::bit00,
                          FACE_OUTER_SIDE   = datatools::bit_mask::bit01,
                          FACE_BOTTOM       = datatools::bit_mask::bit02,
                          FACE_TOP          = datatools::bit_mask::bit03,
                          FACE_ALL          = (FACE_INNER_SIDE
                                               | FACE_OUTER_SIDE
                                               | FACE_BOTTOM
                                               | FACE_TOP)
    };

    /// \brief Format of the datafile for reading Z/Rmin/Rmax data
    enum datafile_column_mode {
                               RMIN_RMAX    = 0, //!< Use one first column for Rmin and second one for Rmax
                               IGNORE_RMIN  = 1, //!< Ignore first column as Rmin and use only second one for Rmax
                               RMIN_AS_RMAX = 2  //!< Use one first column as Rmax
    };

    /*
     * The polyhedra is defined by a list of (z, rmin, rmax) triplets
     * the radius are the distance from center to corners (OA, OB...).
     * Caution: the Geant4 and GDML use the tangential radius (radius of the inscribed
     * circles == apothem == OT) and not the distance from Z-axis to the corners.
     *
     * Example with a 'n_sides==6' polyhedra:
     *
     *         C           B
     *          +_________+
     *         /           \
     *        /            .+ T
     *       /          .    \
     *    D +        +        + A
     *       \      O        /
     *        \             /
     *         \___________/
     *         +           +
     *        E             F
     *
     */
    struct r_min_max
    {
      double rmin, rmax;
    };

    // Type alias
    typedef std::map<double, r_min_max> rz_col_type;

    /// \brief Data describing one frustrum
    struct frustrum_data {
      // Bottom parameters:
      double z1, a1, b1;
      // Top parameters:
      double z2, a2, b2;
    };

    /// Check if the polycone has a hole (Rmin != 0)
    bool is_extruded () const;

    /// Return the X min
    double get_xmin () const override;

    /// Return the X max
    double get_xmax () const override;

    /// Return the Y min
    double get_ymin () const override;

    /// Return the Y max
    double get_ymax () const override;

    /// Return the Z min
    double get_zmin () const override;

    /// Return the Z max
    double get_zmax () const override;

    /// Return the R max
    double get_r_max () const;

    /// Return the coordinates of a vertex on one of the polyhedra edge
    vector_3d get_corner (int zplane_index_,
                          int corner_index_,
                          bool inner_ = false) const;

    /// Set the number of sides
    void set_n_sides (size_t n_sides_);

    /// Return the number of sides
    size_t get_n_sides () const;

    /// Return the number of frustra
    unsigned int number_of_frustra() const;

    /// Compute frustrum data at given index
    void get_frustrum_data(size_t i_, frustrum_data &) const;

    /// Return the dictionary of points
    const rz_col_type & points () const;

    //! Default constructor
    polyhedra ();

    //! Destructor
    ~polyhedra () override;

    /// Return the identifier/name of the shape
    std::string get_shape_name () const override;

    /// Check if the solid is valid
    bool is_valid () const override;

    /// Add a new point with only Rmax
    void add (double z_, double rmax_, bool compute_ = true);

    /// Add a new point with Rmin and Rmax
    void add (double z_, double rmin_, double rmax_, bool compute_ = true);

    /// Main initialization method from a container of configuration parameters
    void initialize (const datatools::properties & setup_, const handle_dict_type * = 0) override;

    /// Reset/invalidate the solid
    void reset () override;

    /// Initialize from a file
    void initialize (const std::string & filename_, int mode_ = RMIN_RMAX);

    /// Check for a top face
    bool has_top_face() const;

    /// Check for a bottom face
    bool has_bottom_face() const;

    /// Check for an inner face
    bool has_inner_face() const;

    /// Check for angle faces
    bool has_angle_faces() const;

    /// Compute the top face
    void compute_top_face(composite_surface & top_, placement &) const;

    /// Compute the bottom face
    void compute_bottom_face(composite_surface & bottom_, placement &) const;

    /// Compute frustrum at given index
    void compute_frustrum(right_polygonal_frustrum & f_, placement &, int index_) const;

    /// Compute the inner face at given frustrum index
    void compute_inner_face(composite_surface & in_, placement &, int index_) const;

    /// Compute the outer face at given frustrum index
    void compute_outer_face(composite_surface & on_, placement &, int index_) const;

    /// Compute the inner face
    void compute_inner_face(composite_surface & in_, placement &) const;

    /// Compute the outer face
    void compute_outer_face(composite_surface & in_, placement &) const;

    /// Compute informations about the faces of this solid shape
    unsigned int compute_faces(face_info_collection_type &) const override;

    /// Compute the inner polyhedra
    void compute_inner_polyhedra (polyhedra & ip_);

    /// Compute the outer polyhedra
    void compute_outer_polyhedra (polyhedra & op_);

    /// Compute a deflated polyhedra
    void compute_deflated(polyhedra & deflated_,
                          double by_r_,
                          double by_z_);

    /// Compute an inflated polyhedra
    void compute_inflated(polyhedra & inflated_,
                          double by_r_,
                          double by_z_);

    /// Compute the polyhedra envelope
    void compute_envelope(polyhedra & envelope_,
                          double r_tolerance_,
                          double z_tolerance_);

    /// Compute the volume
    double get_volume (uint32_t flags_ = 0) const override;

    /// Compute the surface
    double get_surface (uint32_t mask_ = FACE_ALL) const override;

    /// Return the min Z
    double get_z_min () const;

    /// Return the max Z
    double get_z_max () const;

    /// Return the max X or Y
    double get_xy_max () const;

    /// Return a parameter by name
    double get_parameter ( const std::string & flag_ ) const;

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

    /// Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                   const std::string & title_  = "",
                   const std::string & indent_ = "",
                   bool inherit_          = false) const override;

    /// \brief 3D rendering options
    enum polyhedra_wires_rendering_option_type {
                                                WR_POLYHEDRA_NO_BOTTOM_FACE      = (WR_BASE_LAST << 1),           //!< Do not render the bottom face
                                                WR_POLYHEDRA_NO_TOP_FACE         = (WR_BASE_LAST << 2),           //!< Do not render the top face
                                                WR_POLYHEDRA_NO_INNER_FACE       = (WR_BASE_LAST << 3),           //!< Do not render the inner face
                                                WR_POLYHEDRA_NO_OUTER_FACE       = (WR_BASE_LAST << 4),           //!< Do not render the outer face
                                                // WR_POLYHEDRA_NO_START_ANGLE_FACE = (WR_BASE_LAST << 5),           //!< Do not render the start angle face
                                                // WR_POLYHEDRA_NO_STOP_ANGLE_FACE  = (WR_BASE_LAST << 6),           //!< Do not render the stop angle face
                                                WR_POLYHEDRA_LAST                = (WR_POLYHEDRA_NO_OUTER_FACE),  //!< Last defined bit
                                                WR_POLYHEDRA_MASK                = (WR_POLYHEDRA_NO_BOTTOM_FACE
                                                                                    | WR_POLYHEDRA_NO_TOP_FACE
                                                                                    | WR_POLYHEDRA_NO_INNER_FACE
                                                                                    | WR_POLYHEDRA_NO_OUTER_FACE
                                                                                    // | WR_POLYHEDRA_NO_START_ANGLE_FACE
                                                                                    // | WR_POLYHEDRA_NO_STOP_ANGLE_FACE
                                                                                    ) //!< Rendering options bit mask
    };

    /// Generate a list of polylines representing the contour of the shape (for display clients)
    void generate_wires_self(wires_type & wires_,
                             uint32_t options_ = 0) const override;

    friend std::ostream & operator<< (std::ostream &, const polyhedra &);

    friend std::istream & operator>> (std::istream &, polyhedra &);

  protected:

    /// Build bounding data
    void _build_bounding_data() override;

    /// Set default attributes
    void _set_defaults();

    /// Executed at lock stage
    void _at_lock() override;

  private:

    void _compute_surfaces_();

    void _compute_volume_();

    void _compute_limits_();

    void _compute_misc_();

    void _compute_all_();

  private:

    // Parameters:
    size_t      _n_sides_; //!< Number of sides
    rz_col_type _points_;  //!< Dictionary of points

    // Working data:
    bool    _computed_; //!< Flag indicating that internal working data have been computed
    boost::logic::tribool _has_top_face_;
    boost::logic::tribool _has_bottom_face_;
    boost::logic::tribool _has_inner_face_;
    double  _top_surface_;
    double  _bottom_surface_;
    double  _outer_side_surface_;
    double  _inner_side_surface_;
    double  _outer_volume_;
    double  _inner_volume_;
    double  _volume_;
    double  _z_min_;
    double  _z_max_;
    double  _r_max_;
    double  _xy_max_;
    bool    _extruded_;

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(polyhedra)

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_POLYHEDRA_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
