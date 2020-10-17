/// \file geomtools/right_circular_conical_nappe.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-03-15
 * Last modified: 2015-03-15
 *
 * License:
 *
 * Description:
 *
 *   Right circular conical n-appe
 *
 */

#ifndef GEOMTOOLS_RIGHT_CIRCULAR_CONICAL_NAPPE_H
#define GEOMTOOLS_RIGHT_CIRCULAR_CONICAL_NAPPE_H 1

// This project:
#include <geomtools/cone.h>
#include <geomtools/i_shape_2d.h>
#include <geomtools/angular_range.h>

namespace geomtools {

  // Class forward declarations:
  class disk;
  class quadrangle;

  /// \brief Right circular conical nappe (2D shape)
  class right_circular_conical_nappe : public i_shape_2d
  {
  public:

    /// Return the identifier label for the class
    static const std::string & right_circular_conical_nappe_label();

    /// Return the identifier/name of the shape
    std::string get_shape_name() const override;

    /// Check if the rectangle is valid
    bool is_valid() const override;

    /// Set the Z dimension
    void set_z(double);

    /// Return the Z dimension
    double get_z() const;

    /// Set the bottom radius
    void set_bottom_radius(double);

    /// Return the bottom radius
    double get_bottom_radius() const;

    /// Set the top radius
    void set_top_radius(double);

    /// Return the top radius
    double get_top_radius() const;

    /// Return the angle domain
    const angular_range & get_angle_domain() const;

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

    /// Compute the radius of the nappe at given z
    double get_radius_from_z(double z_) const;

    /// Return the angle of the cone
    double get_cone_angle() const;

    /// Return the surface
    double get_surface(uint32_t flags_ = ALL_PIECES) const override;

    /// Default constructor
    right_circular_conical_nappe();

    /// Constructor
    right_circular_conical_nappe(double bottom_radius_,
                                 double top_radius_,
                                 double z_);

    /// Constructor
    right_circular_conical_nappe(double bottom_radius_,
                                 double top_radius_,
                                 double z_,
                                 double start_angle_,
                                 double delta_angle_);

    /// Destructor
    ~right_circular_conical_nappe() override;

    /// Check if a point is on the surface
    bool is_on_surface(const vector_3d &,
                               double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Return the normal at a given position of the surface
    vector_3d get_normal_on_surface(const vector_3d & position_,
                                            bool check_ = true,
                                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Find the intercept point of a segment with the surface
    bool find_intercept(const vector_3d & from_,
                                const vector_3d & direction_,
                                face_intercept_info & intercept_,
                                double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Smart print
    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_= false) const override;

    /// \brief 3D rendering options
    enum rccn_wires_rendering_option_type {
      WR_RCCN_NO_BOTTOM_EDGE      = (WR_BASE_LAST << 1),           //!< Do not render the bottom edge
      WR_RCCN_NO_TOP_EDGE         = (WR_BASE_LAST << 2),           //!< Do not render the top edge
      WR_RCCN_NO_START_ANGLE_EDGE = (WR_BASE_LAST << 3),           //!< Do not render the start phi angle edge
      WR_RCCN_NO_STOP_ANGLE_EDGE  = (WR_BASE_LAST << 4),           //!< Do not render the stop phi angle edge
      WR_RCCN_LAST                = (WR_RCCN_NO_STOP_ANGLE_EDGE),  //!< Last defined bit
      WR_RCCN_MASK                = (WR_RCCN_NO_BOTTOM_EDGE
                                     | WR_RCCN_NO_TOP_EDGE
                                     | WR_RCCN_NO_START_ANGLE_EDGE
                                     | WR_RCCN_NO_STOP_ANGLE_EDGE) //!< Rendering options bit mask
    };

    /// Generate a sequence of polylines for wires 3D rendering
    void generate_wires_self(wires_type & wires_,
                                     uint32_t options_ = 0) const override;

    /// Initialize from properties and a dictionary of 3D-objects
    void initialize(const datatools::properties &, const handle_dict_type * = 0) override;

    /// Reset the shape
    void reset() override;

  protected:

    /// Set default attributes
    void _set_defaults();

  private:

    double _bottom_radius_; //!< The bottom radius
    double _top_radius_;    //!< The top radius
    double _z_;             //!< The height
    angular_range _angle_domain_; //!< Angular spread of the cylindrical sector (longitude/azimuth)

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(right_circular_conical_nappe)

  };

  /// Type alias
  typedef right_circular_conical_nappe conical_nappe;

} // end of namespace geomtools

#endif // GEOMTOOLS_RIGHT_CIRCULAR_CONICAL_NAPPE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
