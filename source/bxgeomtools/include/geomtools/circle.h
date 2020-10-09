/// \file geomtools/circle.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-14
 * Last modified: 2015-02-15
 *
 * License:
 *
 * Description:
 *
 *   A circular arc in x-y plane
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_CIRCLE_H
#define GEOMTOOLS_CIRCLE_H 1

// Standard library:
#include <iostream>
#include <string>

// This project:
#include <geomtools/i_shape_1d.h>
#include <geomtools/i_wires_3d_rendering.h>
#include <geomtools/angular_range.h>

namespace geomtools {

  /// \brief A circular arc (1D shape)
  class circle : public i_shape_1d,
                 public i_wires_3d_rendering
  {

  public:

    /// Return the label of 1D shape
    static const std::string & circle_label();

    /// Check the validity of the circle
    bool is_valid() const override;

    /// Return the radius
    double get_r() const;

    /// Return the radius
    double get_radius() const;

    /// Set the radius
    void set_r(double a_radius);

    /// Set the radius
    void set_radius(double a_radius);

    /// Set the diameter
    void set_diameter(double a_diameter);

    /// Return the diameter
    double get_diameter() const;

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

    /// Return the surface
    double get_surface() const;

    /// Return the number of path
    virtual unsigned int get_number_of_path() const;

    /// Return the length of the target paths
    double get_length(uint32_t flags_ = PATH_ALL_BITS) const override;

    /// Default constructor
    circle();

    /// Constructor
    circle(double radius_);

    /// Constructor
    circle(double radius_, double start_angle_, double delta_angle_);

    /// Destructor
    ~circle() override;

    /// Return the name of this solid shape class
    std::string get_shape_name() const override;

    /// Smart print
    void tree_dump(std::ostream & a_out = std::clog,
                           const std::string & a_title = "",
                           const std::string & a_indent = "",
                           bool a_inherit= false) const override;

    /// Check if a point is on the curve
    bool is_on_curve(const vector_3d &,
                             double a_tolerance = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Returns the direction of a point on the curve
    vector_3d get_direction_on_curve(const vector_3d & a_tposition) const override;

    /// Generate a sequence of polylines for wires 3D rendering
    void generate_wires_self(wires_type & wires_, uint32_t options_ = 0) const override;

    /// Initialize from properties and a dictionary of 3D-objects
    void initialize(const datatools::properties &, const handle_dict_type * = 0) override;

    /// Reset
    void reset() override;

  protected:

    /// Set default attributes values
    void _set_defaults();

  private:

    double _radius_;      //!< The radius of the circle (in arbitrary units).
    angular_range _angle_domain_; //!< Angular spread of the circular arc

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(circle)

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_CIRCLE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
