/// \file geomtools/simple_polygon.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2015-07-09
 * Last modified: 2015-07-09
 *
 * License:
 *
 * Description:
 *
 *   A simple polygon in 2D space
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_SIMPLE_POLYGON_H
#define GEOMTOOLS_SIMPLE_POLYGON_H

// Standard library:
#include <vector>
#include <string>

// Third party:
// - Bayeux/mygsl:
#include <mygsl/min_max.h>

// This project:
#include <geomtools/utils.h>
#include <geomtools/i_shape_2d.h>
#include <geomtools/i_polygon.h>

namespace geomtools {

  // Forward declaration:
  class triangle;

  /// \brief A simple polygon in the XY plane
  class simple_polygon : public i_shape_2d,
                         public i_polygon
  {

  public:

    /// \brief Build mode
    enum build_mode_type {
      BUILD_MODE_INVALID     = 0, ///< Invalid build mode
      BUILD_MODE_BY_VERTICES = 1, ///< Build mode by vertices
      BUILD_MODE_BY_SEGMENTS = 2  ///< Build mode by segments
    };

    /// Return the build mode from a label
    static build_mode_type build_mode_from_label(const std::string &);

    /// Return the label associated to a build mode
    static std::string label_from_build_mode(build_mode_type);

    /// Return the class label
    static const std::string & simple_polygon_label();

    /// Check the validity of the simple polygon
    bool is_valid() const override;

    /// Return the identifier/name of the shape
    std::string get_shape_name() const override;

    /// \brief Data for a wall segment
    struct wall_segment_type {
      wall_segment_type();
      vector_2d start;
      vector_2d stop;
      double left_thickness;
      double right_thickness;
      double start_angle;
      double stop_angle;
    };

    /// \brief Type alias for a collection of wall segment data
    typedef std::vector<wall_segment_type> wall_segment_col_type;

    /// Set the build mode
    void set_build_mode(build_mode_type);

    /// Return the build mode
    build_mode_type get_build_mode() const;

    /// Return the perimeter
    double get_perimeter(uint32_t flags_ = ALL_PIECES) const override;

    /// Return the surface
    double get_surface(uint32_t flags_ = ALL_PIECES) const override;

    /// Return the number of vertices
    unsigned int get_number_of_vertices() const;

    /// Return the vertex by index
    const vector_2d & get_vertex(int) const;

    /// Check if the simple polygon is convex
    bool is_convex() const;

    /// Append a vertex to the list of vertexes
    void add_vertex(double x_, double y_);

    /// Append a vertex to the list of vertexes
    void add_vertex(const vector_2d &);

    /// Add wall segment
    void add_wall_segment(const vector_2d & start_,
                          const vector_2d & stop_,
                          double left_thickness_,
                          double right_thickness_,
                          double start_angle_ = 0.0,
                          double stop_angle_ = std::numeric_limits<double>::quiet_NaN());

    /// Add wall segment
    void add_wall_segment(const vector_2d & stop_,
                          double left_thickness_,
                          double right_thickness_,
                          double stop_angle_ = std::numeric_limits<double>::quiet_NaN());

    /// Check if walls are terminated
    bool walls_are_terminated() const;

    /// Default constructor
    simple_polygon();

    /// Destructor
    ~simple_polygon() override;

    /// Check is a given point belongs to the surface of the 2D shape
    bool is_on_surface(const vector_3d &,
                               double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Return the normal direction at some position on the 2D shape's path
    vector_3d get_normal_on_surface(const vector_3d & position_,
                                            bool check_ = false,
                                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Check if the polygon is clockwise
    bool is_clockwise() const;

    /// Check if the polygon is anticlockwise
    bool is_anticlockwise() const;

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

    /// \brief 3D rendering options
    enum simple_polygon_wires_rendering_option_type {
      WR_SP_INTERNAL_EDGES = (WR_BASE_LAST << 1),    //!< Do render the internal triangle edges
      WR_SP_LAST           = (WR_SP_INTERNAL_EDGES), //!< Last defined bit
      WR_SP_MASK           = (WR_SP_INTERNAL_EDGES
                              )   //!< Rendering options bit mask
    };

    /// Generate a sequence of polylines for wires 3D rendering
    void generate_wires_self(wires_type & wires_,
                                     uint32_t options_ = 0) const override;

    /// Build an ordered collection of vertexes
    unsigned int compute_vertexes(vertex_col_type & vertexes_) const override;

    /// Return the number of triangles
    unsigned int number_of_triangles() const;

    /// Return a non mutable reference to one of the inner triangles
    const triangle & get_triangle(int index_) const;

    /// Return the bounding interval on the X axis
    const mygsl::min_max & get_xrange() const;

    /// Return the bounding interval on the Y axis
    const mygsl::min_max & get_yrange() const;

    /// Check initialization
    bool is_initialized() const;

    /// Initialization
    void initialize(const datatools::properties &, const handle_dict_type * objects_ = 0) override;

    /// Reset
    void reset() override;

    /// Find the intersection between two 2D segments
    static bool find_segment_2d_intersection(const vector_2d & a_,
                                             const vector_2d & b_,
                                             const vector_2d & c_,
                                             const vector_2d & d_,
                                             vector_2d & intersection_);

    /// Triangulation of a simple polygon
    static void triangulation(const std::vector<const vector_2d *> & vertices_,
                              const vector_3d & normal_,
                              std::vector<triangle> & triangles_);


  protected:

    /// Set defaults attributes
    void _set_defaults();

    /// Build by vertices
    void _build_by_vertices();

    /// Build by walls
    void _build_by_segments();

    /// Check if a vertex can be added
    bool _check_vertex(const vector_2d & vtx_, bool no_first_check_ = false) const;

    /// Compute normal
    void _compute_normal();

    /// Add a new vertex
    void _add_vertex(const vector_2d & vtx_);

    /// Add wall segment
    void _add_wall_segment(const vector_2d & start_,
                           const vector_2d & stop_,
                           double left_thickness_,
                           double right_thickness_,
                           double start_angle_,
                           double stop_angle_);

    /// Special init operations
    void _at_initialize();

  private:

    build_mode_type _build_mode_; //!< The build mode
    std::vector<vector_2d> _vertices_; //!< The ordered list of vertices
    wall_segment_col_type _wall_segments_; //!< The ordered list of wall segments

    // Working data:
    bool _convex_;
    mygsl::min_max _xrange_; //!< Bounding interval on the X axis
    mygsl::min_max _yrange_; //!< Bounding interval on the Y axis
    vector_3d _normal_; //!< Normal
    std::vector<triangle> _triangles_; //!< List of triangles

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(simple_polygon)

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_SIMPLE_POLYGON_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
