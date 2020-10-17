/// \file geomtools/polycone.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-03-14
 * Last modified: 2015-03-12
 *
 * License:
 *
 * Description:
 *
 *   Polycone 3D solid shape
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_POLYCONE_H
#define GEOMTOOLS_POLYCONE_H 1

// Standard library:
#include <iostream>
#include <string>
#include <map>
#include <limits>

// This project:
#include <geomtools/i_shape_3d.h>
#include <geomtools/i_stackable.h>
#include <geomtools/right_circular_conical_frustrum.h>

namespace geomtools {

  // Forward class declaration:
  class right_circular_conical_nappe;
  class quadrangle;
  class disk;
  class composite_surface;

  //! \brief Polycone 3D solid shape
  class polycone : public i_shape_3d,
                   public i_stackable
  {
  public:

    /// Return the identifier label for the class
    static const std::string & polycone_label();

    /// \brief Face flags
    enum faces_mask_type {
        FACE_NONE         = face_identifier::FACE_BITS_NONE,
        FACE_INNER_SIDE   = datatools::bit_mask::bit00,
        FACE_OUTER_SIDE   = datatools::bit_mask::bit01,
        FACE_BOTTOM       = datatools::bit_mask::bit02,
        FACE_TOP          = datatools::bit_mask::bit03,
        FACE_START_ANGLE  = datatools::bit_mask::bit04,
        FACE_STOP_ANGLE   = datatools::bit_mask::bit05,
        FACE_ALL    = (FACE_INNER_SIDE
                       | FACE_OUTER_SIDE
                       | FACE_BOTTOM
                       | FACE_TOP
                       | FACE_START_ANGLE
                       | FACE_STOP_ANGLE)
      };

    /// \brief Format of the datafile for reading Z/Rmin/Rmax data
    enum datafile_column_mode {
      RMIN_RMAX    = 0, //!< Use one first column for Rmin and second one for Rmax
      IGNORE_RMIN  = 1, //!< Ignore first column as Rmin and use only second one for Rmax
      RMIN_AS_RMAX = 2  //!< Use one first column as Rmax
    };

    /// \brief Rmin/Rmax doublet
    struct r_min_max
    {
      double rmin; //!< Rmin
      double rmax; //!< Rmax
    };

    /// \brief Data describing one frustrum
    struct frustrum_data {
      // Bottom parameters:
      double z1, a1, b1;
      // Top parameters:
      double z2, a2, b2;
    };

    // Type alias
    typedef std::map<double, r_min_max> rz_col_type;

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

    /// Return the Z dimension
    double get_z () const;

    /// Check if the nappe has partial phi angle
    bool has_partial_angle() const;

    /// Check the start phi angle
    bool has_start_angle() const;

    /// Set the start phi angle
    void set_start_angle(double);

    /// Return the start phi angle
    double get_start_angle() const;

    /// Check the delta phi angle
    bool has_delta_angle() const;

    /// Set the delta phi angle
    void set_delta_angle(double);

    /// Return the delta phi angle
    double get_delta_angle() const;

    /// Return the number of frustra
    unsigned int number_of_frustra() const;

    /// Compute frustrum data at given index
    void get_frustrum_data(size_t i_, frustrum_data &) const;

    /// Return the dictionary of points
    const rz_col_type & points() const;

    //! Default constructor
    polycone ();

    //! Destructor
    ~polycone () override;

    /// Return the identifier/name of the shape
    std::string get_shape_name() const override;

    /// Check if the solid is valid
    bool is_valid () const override;

    /// Add a new point with only Rmax
    void add(double z_, double rmax_, bool compute_ = true);

    /// Add a new point with Rmin and Rmax
    void add(double z_, double rmin_, double rmax_, bool compute_ = true);

    /// Initialize
    void initialize ();

    /// Initialize from a file
    void initialize (const std::string & filename_);

    /** Initialize the polycone from data in a file.
     * Format (ASCII) consists in lines with the
     * (Z,Rmax) format or the (Z,Rmin,Rmax) format.
     * Special metadata may be specified at the beginning
     * of the file:
     *
     * All data are given with the following length unit:
     * \code
     * #@length_unit mm
     * \endcode
     *
     * Data (Rmin) in the second column out of three columns are ignored:
     * \code
     * #@ignore_rmin
     * \endcode
     *
     * The thickness of the polycone:
     * \code
     * #@skin_thickness 0.001
     * \endcode
     *
     * The step of the polycone:
     * \code
     * #@skin_step 5.0
     * \endcode
     *
     * Example:
     * \code
     *  #@length_unit mm
     *  z1 rmin1 rmax1
     *  z2 rmin2 rmax2
     *  z3       rmax3
     *  z4 rmin4 rmax4
     * \endcode
     *
     */
    void initialize (const std::string & filename_,
                     double zmin_ = std::numeric_limits<double>::quiet_NaN (),
                     double zmax_ = std::numeric_limits<double>::quiet_NaN (),
                     int mode_ = RMIN_RMAX);

    /// Main initialization method from a container of configuration parameters
    void initialize (const datatools::properties & setup_, const handle_dict_type * = 0) override;

    /// Reset/invalidate the solid
    void reset () override;

    /// Check for a top face
    bool has_top_face() const;

    /// Check for a bottom face
    bool has_bottom_face() const;

    /// Check for an inner face
    bool has_inner_face() const;

    /// Check for angle faces
    bool has_angle_faces() const;

    /// Compute the top face
    void compute_top_face(disk & top_disk_, placement &) const;

    /// Compute the bottom face
    void compute_bottom_face(disk & bottom_disk_, placement &) const;

    /// Compute frustrum at given index
    void compute_frustrum(right_circular_conical_frustrum & f_, placement &, int index_) const;

    /// Compute the inner face at given frustrum index
    void compute_inner_face(right_circular_conical_nappe & in_, placement &, int index_) const;

    /// Compute the outer face at given frustrum index
    void compute_outer_face(right_circular_conical_nappe & on_, placement &, int index_) const;

    right_circular_conical_frustrum::ssaf_type
    get_start_stop_angle_face_type(int index_) const;

    /// Compute the start phi angle face
    void compute_start_angle_face(quadrangle & qface_, triangle & tface_, placement &, int index_) const;

    /// Compute the stop phi angle face
    void compute_stop_angle_face(quadrangle & qface_, triangle & tface_, placement &, int index_) const;

    /// Compute the outer face
    void compute_outer_face(composite_surface & face_, placement &) const;

    /// Compute the inner face
    void compute_inner_face(composite_surface & face_, placement &) const;

    /// Compute the start angle face
    void compute_start_angle_face(composite_surface & face_, placement &) const;

    /// Compute the stop angle face
    void compute_stop_angle_face(composite_surface & face_, placement &) const;

    /// Compute informations about the faces of this solid shape
    unsigned int compute_faces(face_info_collection_type &) const override;

    /// Compute the inner polycone
    void compute_inner_polycone (polycone & ip_);

    /// Compute the outer polycone
    void compute_outer_polycone (polycone & op_);

    /// Compute the volume
    double get_volume (uint32_t flags_ = 0) const override;

    /// Compute the surface
    double get_surface (uint32_t mask_ = FACE_ALL) const override;

    /// Return the min Z
    double get_z_min () const;

    /// Return the max Z
    double get_z_max () const;

    /// Return the max eadius
    double get_r_max () const;

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

    // Smart print
    void tree_dump (std::ostream & out_         = std::clog,
                            const std::string & title_  = "",
                            const std::string & indent_ = "",
                            bool inherit_          = false) const override;

    friend std::ostream & operator<< (std::ostream &, const polycone &);

    friend std::istream & operator>> (std::istream &, polycone &);

    /// \brief 3D rendering options
    enum polycone_wires_rendering_option_type {
      WR_POLYCONE_NO_BOTTOM_FACE      = (WR_BASE_LAST << 1),           //!< Do not render the bottom face
      WR_POLYCONE_NO_TOP_FACE         = (WR_BASE_LAST << 2),           //!< Do not render the top face
      WR_POLYCONE_NO_INNER_FACE       = (WR_BASE_LAST << 3),           //!< Do not render the inner face
      WR_POLYCONE_NO_OUTER_FACE       = (WR_BASE_LAST << 4),           //!< Do not render the outer face
      WR_POLYCONE_NO_START_ANGLE_FACE = (WR_BASE_LAST << 5),           //!< Do not render the start angle face
      WR_POLYCONE_NO_STOP_ANGLE_FACE  = (WR_BASE_LAST << 6),           //!< Do not render the stop angle face
      WR_POLYCONE_LAST                = (WR_POLYCONE_NO_STOP_ANGLE_FACE),  //!< Last defined bit
      WR_POLYCONE_MASK                = (WR_POLYCONE_NO_BOTTOM_FACE
                                         | WR_POLYCONE_NO_TOP_FACE
                                         | WR_POLYCONE_NO_INNER_FACE
                                         | WR_POLYCONE_NO_OUTER_FACE
                                         | WR_POLYCONE_NO_START_ANGLE_FACE
                                         | WR_POLYCONE_NO_STOP_ANGLE_FACE) //!< Rendering options bit mask
    };

    /// Generate a list of polylines representing the contour of the shape (for display clients)
    void generate_wires_self(wires_type & wires_, uint32_t options_ = 0) const override;

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

    // Interpolation:
    void _build_from_envelope_and_skin_ (double thickness_,
                                         double step_,
                                         double zmin_,
                                         double zmax_);

    void _build_from_envelope_and_skin_ (double thickness_, double step_ = 0.0);

  private:

    // Parameters:
    rz_col_type _points_; //!< The dictionary of dimensions of the frustra
    double _start_angle_; //!< The starting phi angle (longitude/azimuth)
    double _delta_angle_; //!< The delta phi angle (longitude/azimuth)

    // Working data:
    bool     _computed_; //!< Flag indicating that internam working data have been computed
    boost::logic::tribool _has_top_face_;
    boost::logic::tribool _has_bottom_face_;
    boost::logic::tribool _has_inner_face_;
    double  _top_surface_;
    double  _bottom_surface_;
    double  _outer_side_surface_;
    double  _inner_side_surface_;
    double  _start_angle_surface_;
    double  _stop_angle_surface_;
    double  _outer_volume_;
    double  _inner_volume_;
    double  _volume_;
    double  _z_min_;
    double  _z_max_;
    double  _r_max_;
    bool    _extruded_;

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(polycone)

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_POLYCONE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
