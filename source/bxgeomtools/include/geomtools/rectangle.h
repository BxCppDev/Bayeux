// -*- mode: c++ ; -*-
/* rectangle.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-14
 * Last modified: 2010-02-14
 *
 * License:
 *
 * Description:
 *
 *   A rectangle in x-y plane
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_RECTANGLE_H
#define GEOMTOOLS_RECTANGLE_H 1

// Standard library:
#include <iostream>
#include <string>

// This project:
#include <geomtools/i_shape_2d.h>
#include <geomtools/polyline_3d.h>
#include <geomtools/placement.h>
#include <geomtools/i_wires_3d_rendering.h>

namespace geomtools {

  /// \brief A rectangle (2D shape) in a 3D space
  class rectangle : public i_shape_2d,
                    public i_wires_3d_rendering
  {

  public:

    /// Return the identifier label for the class
    static const std::string & rectangle_label();

    /// Check if the rectangle is valid
    bool is_valid() const;

    /// Return the X dimension
    double get_x() const;

    /// Return the Y dimension
    double get_y() const;

    /// Set the X dimension
    void set_x(double);

    /// Set the Y dimension
    void set_y(double);

    /// Return the surface
    virtual double get_surface(uint32_t flags_ = ALL_PIECES) const;

    /// Return the perimeter
    virtual double get_perimeter(uint32_t flags_ = ALL_PIECES) const;

    /// Return the circumference
    double get_circumference() const;

    double get_diagonal() const;

    /// Default constructor
    rectangle();

    /// Constructor
    rectangle(double x_, double y_);

    /// Destructor
    virtual ~rectangle();

    /// Return the identifier/name of the shape
    virtual std::string get_shape_name() const;

    virtual bool is_on_surface(const vector_3d &,
                               double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual vector_3d get_normal_on_surface(const vector_3d & position_,
                                            bool up_ = true) const;

    virtual bool find_intercept(const vector_3d & from_,
                                const vector_3d & direction_,
                                intercept_t & intercept_,
                                double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Smart print
    virtual void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_= false) const;

    virtual void generate_wires(std::list<polyline_3d> &,
                                const placement & ,
                                uint32_t options_ = 0) const;

  private:

    double _x_; /// X dimension
    double _y_; /// Y dimension

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_RECTANGLE_H
