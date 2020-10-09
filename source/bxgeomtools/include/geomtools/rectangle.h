/// \file geomtools/rectangle.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-14
 * Last modified: 2015-02-15
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
#include <geomtools/i_polygon.h>

namespace geomtools {

  /// \brief A rectangular surface (2D shape)
  class rectangle : public i_shape_2d,
                    public i_polygon
  {

  public:

    /// Return the identifier label for the class
    static const std::string & rectangle_label();

    /// Check if the rectangle is valid
    bool is_valid() const override;

    /// Return the X dimension
    double get_x() const;

    /// Return the Y dimension
    double get_y() const;

    /// Set the X dimension
    void set_x(double);

    /// Set the Y dimension
    void set_y(double);

    /// Set the X and Y dimensions
    void set(double x_, double y_);

    /// Return the surface
    double get_surface(uint32_t flags_ = ALL_PIECES) const override;

    /// Return the perimeter
    double get_perimeter(uint32_t flags_ = ALL_PIECES) const override;

    /// Return the circumference
    double get_circumference() const;

    // Return the diagonal
    double get_diagonal() const;

    /// Default constructor
    rectangle();

    /// Constructor
    rectangle(double x_, double y_);

    /// Destructor
    ~rectangle() override;

    /// Return the identifier/name of the shape
    std::string get_shape_name() const override;

    bool is_on_surface(const vector_3d &,
                               double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    vector_3d get_normal_on_surface(const vector_3d & position_,
                                            bool check_ = true,
                                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

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
    enum rectangle_wires_rendering_option_type {
      WR_RECT_NO_XMINUS_SIDE = (WR_BASE_LAST << 1),      //!< Do not render the X- side
      WR_RECT_NO_XPLUS_SIDE  = (WR_BASE_LAST << 2),      //!< Do not render the X+ side
      WR_RECT_NO_YMINUS_SIDE = (WR_BASE_LAST << 3),      //!< Do not render the Y- side
      WR_RECT_NO_YPLUS_SIDE  = (WR_BASE_LAST << 4),      //!< Do not render the Y+ side
      WR_RECT_LAST           = (WR_RECT_NO_YPLUS_SIDE),  //!< Last defined bit
      WR_RECT_MASK           = (WR_RECT_NO_XMINUS_SIDE
                                | WR_RECT_NO_XPLUS_SIDE
                                | WR_RECT_NO_YMINUS_SIDE
                                | WR_RECT_NO_YPLUS_SIDE) //!< Rendering options bit mask
    };

    /// Generate a sequence of polylines for wires 3D rendering
    void generate_wires_self(wires_type & wires_,
                                     uint32_t options_ = 0) const override;

    /// Build an ordered collection of vertexes
    unsigned int compute_vertexes(vertex_col_type & vertexes_) const override;

    /// Initialize from properties and a dictionary of 3D-objects
    void initialize(const datatools::properties &, const handle_dict_type * = 0) override;

    /// Reset
    void reset() override;

  protected:

    /// Set default attributes values
    void _set_defaults();

  private:

    double _x_; //!< X dimension
    double _y_; //!< Y dimension

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(rectangle)

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_RECTANGLE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
