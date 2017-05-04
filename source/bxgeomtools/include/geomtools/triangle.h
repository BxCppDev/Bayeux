/// \file geomtools/triangle.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-02-18
 * Last modified: 2015-02-18
 *
 * License:
 *
 * Description:
 *
 *   A triangle in 3D space
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_TRIANGLE_H
#define GEOMTOOLS_TRIANGLE_H 1

// Standard library:
#include <string>

// This project:
#include <geomtools/i_shape_2d.h>
#include <geomtools/i_polygon.h>

namespace geomtools {

  /// \brief A triangle
  class triangle : public i_shape_2d,
                   public i_polygon
  {

  public:

    /// Return the class label
    static const std::string & triangle_label();

    /// Check the validity of the triangle
    bool is_valid() const;

    /// Return the perimeter
    virtual double get_perimeter(uint32_t flags_ = ALL_PIECES) const;

    /// Return the surface
    virtual double get_surface(uint32_t flags_ = ALL_PIECES) const;

    /// Check the alignment of 3 points candidate to make a triangle
    static bool check_alignement(const geomtools::vector_3d & v0_,
                                 const geomtools::vector_3d & v1_,
                                 const geomtools::vector_3d & v2_,
                                 double tolerance_ = 0.0);

    /// Set the 3 vertexes
    void set_vertexes(const vector_3d & p0_, const vector_3d & p1_, const vector_3d & p2_);

    /// Set the 3 vertexes as handle to external vertexes
    void set_vertexes_ext(const vector_3d & p0_, const vector_3d & p1_, const vector_3d & p2_);

    /// Check if the first vertex is referenced from an outside object
    bool has_vertex_0_ext() const;

    /// Check if the second vertex is referenced from an outside object
    bool has_vertex_1_ext() const;

    /// Check if the third vertex is referenced from an outside object
    bool has_vertex_2_ext() const;

    /// Return the first vertex
    const vector_3d & get_vertex_0() const;

    /// Return the second vertex
    const vector_3d & get_vertex_1() const;

    /// Return the third vertex
    const vector_3d & get_vertex_2() const;

    /// Return the vertex by index
    const vector_3d & get_vertex(int) const;

    /// Default constructor
    triangle();

    /// Constructor
    triangle(const vector_3d & p0_, const vector_3d & p1_, const vector_3d & p2_);

    /// Destructor
    virtual ~triangle();

    /// Initialize from properties and a dictionary of 3D-objects
    void initialize(const datatools::properties &, const handle_dict_type * = 0);

    /// Reset
    void reset();

    /// Return the identifier/name of the shape
    virtual std::string get_shape_name() const;

    /// Check is a given point belongs to the surface of the 2D shape
    virtual bool is_on_surface(const vector_3d &,
                               double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Return the normal direction at some position on the 2D shape's path
    virtual vector_3d get_normal_on_surface(const vector_3d & position_,
                                            bool check_ = false,
                                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Find the intercept of a ray with the 2D shape's surfaces
    virtual bool find_intercept(const vector_3d & from_,
                                const vector_3d & direction_,
                                face_intercept_info & intercept_,
                                double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Smart print
    virtual void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_= false) const;

    /// Compute the vectors of the proper frame (V0, VOV1, V0V2, V0V1^V0V1) system
    void compute_proper_frame(vector_3d & u0_, vector_3d & u1_, vector_3d & u2_) const;

    /// Compute the coordinates of a point with respect to the (V0, VOV1, V0V2, V0V1^V0V1) system
    bool compute_proper_frame_coordinates(const vector_3d & position_, double & xp_, double & yp_, double & zp_) const;


    /// \brief 3D rendering options
    enum triangle_wires_rendering_option_type {
      WR_TRIANGLE_NO_FIRST_EDGE     = (WR_BASE_LAST << 1),             //!< Do not render the first external edges
      WR_TRIANGLE_NO_SECOND_EDGE    = (WR_BASE_LAST << 2),             //!< Do not render the second external edges
      WR_TRIANGLE_NO_THIRD_EDGE     = (WR_BASE_LAST << 3),             //!< Do not render the third external edges
      WR_TRIANGLE_NO_EXTERNAL_EDGES = (WR_TRIANGLE_NO_FIRST_EDGE
                                       | WR_TRIANGLE_NO_SECOND_EDGE
                                       | WR_TRIANGLE_NO_THIRD_EDGE
                                       ),                              //!< Do not render the external edges
      WR_TRIANGLE_LAST              = (WR_TRIANGLE_NO_THIRD_EDGE),     //!< Last defined bit
      WR_TRIANGLE_MASK              = (WR_TRIANGLE_NO_FIRST_EDGE
                                       | WR_TRIANGLE_NO_SECOND_EDGE
                                       | WR_TRIANGLE_NO_THIRD_EDGE
                                       )                               //!< Rendering options bit mask
    };

    /// Generate a sequence of polylines for wires 3D rendering
    virtual void generate_wires_self(wires_type & wires_,
                                     uint32_t options_ = 0) const;

    /// Build an ordered collection of vertexes
    virtual unsigned int compute_vertexes(vertex_col_type & vertexes_) const;

  protected:

    /// Set defaults attributes
    void _set_defaults();

  private:

    const vector_3d * _vertex_0_ext_; //!< Handle to the first vertex
    const vector_3d * _vertex_1_ext_; //!< Handle to the second vertex
    const vector_3d * _vertex_2_ext_; //!< Handle to the third vertex
    vector_3d _vertex_0_; //!< The first vertex
    vector_3d _vertex_1_; //!< The second vertex
    vector_3d _vertex_2_; //!< The third vertex

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(triangle)

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_TRIANGLE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
