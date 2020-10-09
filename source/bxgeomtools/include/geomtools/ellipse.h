/// \file geomtools/ellipse.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-14
 * Last modified: 2015-02-15
 *
 * License:
 *
 * Description:
 *
 *   A elliptic arc in x-y plane
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_ELLIPSE_H
#define GEOMTOOLS_ELLIPSE_H 1

// Standard library:
#include <iostream>
#include <string>

// This project:
#include <geomtools/i_shape_1d.h>
#include <geomtools/i_wires_3d_rendering.h>
#include <geomtools/angular_range.h>

namespace geomtools {

  /// \brief A circular arc (1D shape)
  class ellipse : public i_shape_1d,
                  public i_wires_3d_rendering
  {

  public:

    /// Return the label of 1D shape
    static const std::string & ellipse_label();

    /// Check the validity of the ellipse
    bool is_valid() const override;

    /// Return the X radius
    double get_x_radius() const;

    /// Return the Y radius
    double get_y_radius() const;

    /// Set the radii
    void set_radii(double x_radius_, double y_radius_);

    /// Set the X radius
    void set_x_radius(double x_radius_);

    /// Set the Y radius
    void set_y_radius(double y_radius_);

    /// Return the angle domain
    const angular_range & get_angle_domain() const;

    /// Check for the partial arc
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

    /// Return the circumference
    double get_circumference() const;

    /// Return the number of path
    virtual unsigned int get_number_of_path() const;

    /// Return the length of the target paths
    double get_length(uint32_t flags_ = PATH_ALL_BITS) const override;

    /// Check if the ellipse is X oriented
    bool is_x_oriented() const;

    /// Check if the ellipse is Y oriented
    bool is_y_oriented() const;

    /// Check if the ellipse is a circle
    bool is_circle() const;

    /// Return the excentricity
    double get_excentricity() const;

    /// Return the distance between the focal and the center
    double get_focal_to_center() const;

    /// Default constructor
    ellipse();

    /// Constructor
    ellipse(double x_radius_, double y_radius_);

    /// Constructor
    ellipse(double x_radius_, double y_radius_, double start_angle_, double delta_angle_);

    /// Compute the X and Y coordinates of the point on the ellipse with angle theta
    void compute_x_y_from_theta(double theta_, double & x_, double & y_) const;

    /// Compute the theta angle of the point on the ellipse with X coordinate
    void compute_theta_from_x(double x_, double & theta_) const;

    /// Compute the squared distance between a point H on the helix
    /// with parameter t (xh = a * cos(t), yh = b * sin(t)) and
    /// an arbitrary position (xm, ym) in the X,Y plane.
    /// Also compute the derivative of the squared distance with respect
    /// to the parameter t
    static void squared_distance_to_point_with_derivative(double t_,
                                                          void * params_,
                                                          double * f_,
                                                          double * df_);

    /// Compute the squared distance between a point H on the helix
    /// with parameter t (xh = a * cos(t), yh = b * sin(t)) and
    /// an arbitrary position (xm, ym) in the X,Y plane
    static double squared_distance_to_point(double t_,
                                            void * params_);


    /// Compute the squared distance between a point H on the helix
    /// with parameter t (xh = a * cos(t), yh = b * sin(t)) and
    /// an arbitrary position (xm, ym) in the X,Y plane
    double compute_squared_distance_to_point(double t_, double xm_, double ym_) const;

    // /// Compute the orthogonal projection of a point on the elliptical arc
    // bool compute_orthogonal_projection(const vector_3d & a_position,
    //                                    vector_3d & a_projection,
    //                                    double a_tolerance) const;

    /// Destructor
    ~ellipse() override;

    /// Return the name of this solid shape class
    std::string get_shape_name() const override;

    /// Smart print
    void tree_dump(std::ostream & a_out = std::clog,
                           const std::string & a_title = "",
                           const std::string & a_indent = "",
                           bool a_inherit= false) const override;

    /// Check if a point is on the curve (approximative algorithm)
    bool is_on_curve(const vector_3d &,
                             double a_tolerance = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Returns the direction of a point on the curve
    vector_3d get_direction_on_curve(const vector_3d & a_tposition) const override;

    /// Generate a sequence of polylines for wires 3D rendering
    void generate_wires_self(wires_type & wires_,
                                     uint32_t options_ = 0) const override;

    /// Initialize from properties and a dictionary of 3D-objects
    void initialize(const datatools::properties &, const handle_dict_type * = 0) override;

    /// Reset
    void reset() override;

  protected:

    /// Set default attributes values
    void _set_defaults();

  private:

    double        _x_radius_;     //!< The X radius of the ellipse (in arbitrary units).
    double        _y_radius_;     //!< The Y radius of the ellipse (in arbitrary units).
    angular_range _angle_domain_; //!< Angular spread of the elliptical arc (azimuthal)

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(ellipse)

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_ELLIPSE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
