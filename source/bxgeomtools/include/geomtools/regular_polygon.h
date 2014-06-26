// -*- mode: c++ ; -*-
/// \file geomtools/regular_polygon.h
/* Author(s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-14
 * Last modified: 2010-02-14
 *
 * License:
 *
 * Description:
 *
 *   A regular_polygon in x-y plane
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_REGULAR_POLYGON_H
#define GEOMTOOLS_REGULAR_POLYGON_H 1

// Standard library:
#include <string>

// This project:
#include <geomtools/i_shape_2d.h>

namespace geomtools {

  /// \brief A regular polygon
  /**
   *       C         B
   *        *-------*
   *       /       / \
   *      /       /   * M
   *     /    O  /     \
   *  D *       * <-r-> *-------------------> y
   *     \      :      / A
   *      \     :     /
   *       \    :    /
   *        *-------*
   *       E    |    F
   *            |
   *            V x
   *
   *   A, B... : vertices
   *   OA = OB : radius (or circumradius)
   *   OM      : apothem (http://en.wikipedia.org/wiki/Apothem)
   *   AB      : side's length
   *   (A0B)   : reference angle
   */
  class regular_polygon : public i_shape_2d
  {

  public:
    static const std::string & regular_polygon_label();

    /// \brief Construction mode
    enum mode_type {
      BUILD_BY_RADIUS  = 1,
      BUILD_BY_APOTHEM = 2
    };

    /// Check the validity of the polygon
    bool is_valid() const;

    /// Return the number of sides
    uint32_t get_n_sides() const;

    /// Set  the number of sides
    void set_n_sides(uint32_t);

    /// Return the apothem
    double get_apothem() const;

    /// Return the perimeter
    virtual double get_perimeter(uint32_t flags_ = ALL_PIECES) const;

    /// Return the length of a side
    double get_side_length() const;

    /// Return the reference angle
    double get_reference_angle() const;

    /// Return the radius
    double get_r() const;

    /// Return the radius
    double get_radius() const;

    /// Set the radius
    void set_r(double);

    /// Set the diameter
    void set_diameter(double);

    /// Return the diameter
    virtual double get_diameter() const;

    /// Return the surface
    virtual double get_surface(uint32_t flags_ = ALL_PIECES) const;

    /// Compute the coordinates of a vertex given its index
    void get_vertex(int n_, double & x_, double & y_) const;

    /// Compute the vertex given its index
    void get_vertex(int n_, vector_3d & vertex_) const;

    /// Default constructor
    regular_polygon();

    /// Constructor
    regular_polygon(uint32_t n_sides_, double r_, int mode_ = BUILD_BY_RADIUS);

    /// Destructor
    virtual ~regular_polygon();

    /// Return the identifier/name of the shape
    virtual std::string get_shape_name() const;

    /// Check is a given point belongs to the surface of the 2D shape
    virtual bool is_on_surface(const vector_3d &,
                               double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Return the normal direction at some position on the 2D shape's path
    virtual vector_3d get_normal_on_surface(const vector_3d & position_,
                                            bool up_ = true) const;

    /// Find the intercept of a ray with the 2D shape's surfaces
    virtual bool find_intercept(const vector_3d & from_,
                                const vector_3d & direction_,
                                intercept_t & intercept_,
                                double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Smart print
    virtual void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_= false) const;

  private:
    uint32_t _n_sides_; //! The number of sides/vertices of the polygon
    double   _r_;       //! The radius of the polygon, i.e. the distance from center to vertices (circumradius)

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_REGULAR_POLYGON_H
