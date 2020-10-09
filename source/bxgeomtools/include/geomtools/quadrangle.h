/// \file geomtools/quadrangle.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-02-21
 * Last modified: 2015-02-21
 *
 * License:
 *
 * Description:
 *
 *   A planar quadrangle in 3D space
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_QUADRANGLE_H
#define GEOMTOOLS_QUADRANGLE_H 1

// Standard library:
#include <string>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>

// This project:
#include <geomtools/i_shape_2d.h>
#include <geomtools/i_polygon.h>

namespace geomtools {

  // Forward declaration:
  class triangle;

  /// \brief A quadrangle
  class quadrangle : public i_shape_2d,
                     public i_polygon
  {

  public:

    /// Return the class label
    static const std::string & quadrangle_label();

    /// Check the validity of the quadrangle
    bool is_valid() const override;

    /// Return the perimeter
    double get_perimeter(uint32_t flags_ = ALL_PIECES) const override;

    /// Return the surface
    double get_surface(uint32_t flags_ = ALL_PIECES) const override;

    /// Check the alignment of 4 points candidate to make a quadrangle
    static bool check_vertexes(const geomtools::vector_3d & v0_,
                               const geomtools::vector_3d & v1_,
                               const geomtools::vector_3d & v2_,
                               const geomtools::vector_3d & v3_,
                               double tolerance_ = 0.0);

    /// Set the 3 vertexes
    void set_vertexes(const vector_3d & p0_, const vector_3d & p1_, const vector_3d & p2_, const vector_3d & p3_);

    /// Set the 3 vertexes as handle to external vertexes
    void set_vertexes_ext(const vector_3d & p0_, const vector_3d & p1_, const vector_3d & p2_, const vector_3d & p3_);

    /// Check if the first vertex is referenced from an outside object
    bool has_vertex_0_ext() const;

    /// Check if the second vertex is referenced from an outside object
    bool has_vertex_1_ext() const;

    /// Check if the third vertex is referenced from an outside object
    bool has_vertex_2_ext() const;

    /// Check if the fourth vertex is referenced from an outside object
    bool has_vertex_3_ext() const;

    /// Return the vertex 0
    const vector_3d & get_vertex_0() const;

    /// Return the vertex 1
    const vector_3d & get_vertex_1() const;

    /// Return the vertex 2
    const vector_3d & get_vertex_2() const;

    /// Return the vertex 3
    const vector_3d & get_vertex_3() const;

    /// Return the vertex by index
    const vector_3d & get_vertex(int) const;

    /// Default constructor
    quadrangle();

    /// Constructor
    quadrangle(const vector_3d & p0_, const vector_3d & p1_, const vector_3d & p2_, const vector_3d & p3_);

    /// Destructor
    ~quadrangle() override;

    /// Initialize from properties and a dictionary of 3D-objects
    void initialize(const datatools::properties &, const handle_dict_type * = 0) override;

    /// Reset
    void reset() override;

    /// Return the identifier/name of the shape
    std::string get_shape_name() const override;

    /// Check is a given point belongs to the surface of the 2D shape
    bool is_on_surface(const vector_3d &,
                               double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Return the normal direction at some position on the 2D shape's path
    vector_3d get_normal_on_surface(const vector_3d & position_,
                                            bool check_ = false,
                                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Find the intercept of a ray with the 2D shape's surfaces
    bool find_intercept(const vector_3d & from_,
                                const vector_3d & direction_,
                                face_intercept_info & intercept_,
                                double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Smart print
    void tree_dump(std::ostream & out_ = std::clog,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_= false) const override;

    /// \brief Index of an inner triangle
    enum inner_triangle_index_type {
      IT_NONE   = 0, //!< None
      IT_FIRST  = 1, //!< First triangle (v0, v1, v2)
      IT_SECOND = 2  //!< Second triangle (v0, v2, v3)
    };

    /// Generate a pair of complementary triangles from the quadrangle
    void make_pair_of_triangles(triangle & ta_, triangle & tb_, int = 0) const;

    /// Return a non mutable reference to one of the inner triangles
    const triangle & get_triangle(inner_triangle_index_type i_) const;

    /// Compute the vectors of the proper frame (V0, VOV1, V0V2, V0V1^V0V1) system
    void compute_proper_frame(vector_3d & u0_, vector_3d & u1_, vector_3d & u2_) const;

    /// Compute the coordinates of a point with respect to the (V0, VOV1, V0V2, V0V1^V0V1) system
    bool compute_proper_frame_coordinates(const vector_3d & position_, double & xp_, double & yp_, double & zp_) const;


    /// \brief 3D rendering options
    enum quadrangle_wires_rendering_option_type {
      WR_QUADRANGLE_NO_FIRST_EDGE     = (WR_BASE_LAST << 1),             //!< Do not render the first external edges
      WR_QUADRANGLE_NO_SECOND_EDGE    = (WR_BASE_LAST << 2),             //!< Do not render the second external edges
      WR_QUADRANGLE_NO_THIRD_EDGE     = (WR_BASE_LAST << 3),             //!< Do not render the third external edges
      WR_QUADRANGLE_NO_FOURTH_EDGE    = (WR_BASE_LAST << 4),             //!< Do not render the fourth external edges
      WR_QUADRANGLE_NO_EXTERNAL_EDGES = (WR_QUADRANGLE_NO_FIRST_EDGE
                                       | WR_QUADRANGLE_NO_SECOND_EDGE
                                       | WR_QUADRANGLE_NO_THIRD_EDGE
                                       | WR_QUADRANGLE_NO_FOURTH_EDGE
                                       ),                              //!< Do not render the external edges
      WR_QUADRANGLE_LAST              = (WR_QUADRANGLE_NO_FOURTH_EDGE),     //!< Last defined bit
      WR_QUADRANGLE_MASK              = (WR_QUADRANGLE_NO_FIRST_EDGE
                                       | WR_QUADRANGLE_NO_SECOND_EDGE
                                       | WR_QUADRANGLE_NO_THIRD_EDGE
                                       | WR_QUADRANGLE_NO_FOURTH_EDGE
                                       )                               //!< Rendering options bit mask
    };

    /// Generate a sequence of polylines for wires 3D rendering
    void generate_wires_self(wires_type & wires_,
                                     uint32_t options_ = 0) const override;

    /// Build an ordered collection of vertexes
    unsigned int compute_vertexes(vertex_col_type & vertexes_) const override;

  protected:

    /// Set defaults attributes
    void _set_defaults();

  private:

    const vector_3d * _vertex_0_ext_; //!< Handle to the first vertex
    const vector_3d * _vertex_1_ext_; //!< Handle to the second vertex
    const vector_3d * _vertex_2_ext_; //!< Handle to the third vertex
    const vector_3d * _vertex_3_ext_; //!< Handle to the fourth vertex
    vector_3d _vertex_0_; //! The first vertex
    vector_3d _vertex_1_; //! The second vertex
    vector_3d _vertex_2_; //! The third vertex
    vector_3d _vertex_3_; //! The fourth vertex

    // Working data:
    boost::scoped_ptr<triangle> _t012_;
    boost::scoped_ptr<triangle> _t023_;

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(quadrangle)

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_QUADRANGLE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
