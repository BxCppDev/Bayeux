// -*- mode: c++; -*-
/// \file geomtools/tessellation.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-06-04
 * Last modified: 2012-09-26
 *
 * License:
 *
 * Description:
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_TESSELLATION_H
#define GEOMTOOLS_TESSELLATION_H 1

// Standard library:
#include <iostream>
#include <string>
#include <map>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/mygsl:
#include <mygsl/min_max.h>

// This project:
#include <geomtools/i_shape_3d.h>

namespace geomtools {

  // Forward declarations:
  class facet34;
  class box;

  /// \brief A vertex (corner) of a triangle or quadrangle facet in a tessellated solid
  struct facet_vertex
  {
    vector_3d position; /// Position of the vertex
    std::map<int32_t, int32_t> ref_facets; /// Collection of facets the vertex
                                           /// belongs this is a map indexed
                                           /// by the unique integer key of the
                                           /// facet and storing the rank of the
                                           /// vertex in the facet (using
                                           /// counterclockwise order)

  public:

    bool is_valid() const;

    facet_vertex();

    facet_vertex(double x_, double y_, double z_);

    void print_xyz(std::ostream & out_, int color_ = 0) const;

    void print(std::ostream & out_) const;

    void add_ref_facet(int facet_index_, int facet_node_);

    const vector_3d & get_position() const;

    void reset();

    friend std::ostream & operator<<(std::ostream &, const facet_vertex &);

  };

  /// \brief A triangle or convex quadrangle facet of a tessellated solid
  class facet34
  {

  public:

    /// The maximum index of the category the facet belongs to
    static const unsigned int MAX_CATEGORY = 30;
    static const int INVALID_CATEGORY = -1;

    bool has_category() const;

    void set_category(unsigned int c_);

    void unset_category();

    uint32_t get_number_of_vertices() const;

    const facet_vertex & get_vertex(int i_) const;

    int32_t get_vertex_key(int i_) const;

    void compute_normal();

    void compute_surface();

    void compute_internal_angles(); // Not implemented yet

    void compute_internals();

    bool has_normal() const;

    bool has_surface() const;

    bool is_valid() const;

    const vector_3d & get_normal() const;

    double get_surface() const;

    double get_surface_tri() const;

    double get_surface_tri_bis() const;

    double get_internal_angle(int i_) const;

    void print(std::ostream & out_) const;

    static bool check_triangle(const geomtools::vector_3d & v0_,
                                const geomtools::vector_3d & v1_,
                                const geomtools::vector_3d & v2_,
                                double tolerance_ = 0.0);

    static bool check_quadrangle(const geomtools::vector_3d & v0_,
                                  const geomtools::vector_3d & v1_,
                                  const geomtools::vector_3d & v2_,
                                  const geomtools::vector_3d & v3_,
                                  double tolerance_ = 0.0);

    bool is_triangle() const;

    bool is_quadrangle() const;

    void set_triangle(const facet_vertex & v0_,
                       const facet_vertex & v1_,
                       const facet_vertex & v2_,
                       int iv0_,
                       int iv1_,
                       int iv2_);

    void set_quadrangle(const facet_vertex & v0_,
                         const facet_vertex & v1_,
                         const facet_vertex & v2_,
                         const facet_vertex & v3_,
                         int iv0_,
                         int iv1_,
                         int iv2_,
                         int iv3_);

    facet34();

    facet34(const facet_vertex & v0_,
             const facet_vertex & v1_,
             const facet_vertex & v2_,
             int, int, int);

    facet34(const facet_vertex & v0_,
             const facet_vertex & v1_,
             const facet_vertex & v2_,
             const facet_vertex & v3_,
             int, int, int, int);

  protected:

    void _set_defaults();

  private:

    uint32_t             _number_of_vertices_; /// 3 (triangle) or 4 (quadrangle)
    const facet_vertex * _vertices_[4]; /// Addresses to the vertices
    int32_t              _vertices_keys_[4]; /// Key of the vertices
    geomtools::vector_3d _normal_; /// Normal vector to the surface (counterclockwise)
    double               _surface_tri_; /// Surface of the triangle or first triangle of the quadrangle
    double               _surface_tri_bis_; /// Surface of the second triangle of the quadrangle
    double               _internal_angles_[4]; /// Internal angles per vertex
    int32_t              _category_; /// Auxiliary integer property

  };

  /// \brief A segment associated to a facet
  class facet_segment
  {
  public :
    int vertex0_key;
    int vertex1_key;
    int facet0_key;
    int facet1_key;
    bool shown;

    bool is_valid() const;

    bool is_shown() const;

    void set_shown(bool);

    facet_segment();

    void set_vertex_keys(int,int);

    void set_facet_keys(int,int);

    void set(int,int,int,int);

    friend bool operator==(const facet_segment & fs1_, const facet_segment & fs2_);

    friend bool operator<(const facet_segment & fs1_, const facet_segment & fs2_);

    void dump(std::ostream & out_) const;
  };

  /// \brief Tesselated solid made of facets
  class tessellated_solid : public i_shape_3d
  {
  public:

    static const uint32_t FACE_ALL  = FACE_ALL_BITS;
    static const int INVALID_VERTEX = -1;
    static const int MAX_VERTEX     = 0x0FFFFFFF;

    /// Return the identifier label for the class
    static const std::string & tessellated_label();

    typedef std::map<unsigned int, facet_vertex>  vertices_col_type;
    typedef std::map<unsigned int, facet34>       facets_col_type;
    typedef std::map<unsigned int, facet_segment> facet_segments_col_type;

    /// Return the identifier/name of the shape
    virtual std::string get_shape_name() const;

    static bool validate_index(int);

    /// Check the consistent flag
    bool is_consistent() const;

    /// Default constructor
    tessellated_solid();

    /// Destructor
    virtual ~tessellated_solid();

    const vertices_col_type & vertices() const;

    const facets_col_type & facets() const;

    const facet_segments_col_type & facet_segments() const;

    vertices_col_type & grab_vertices();

    facets_col_type & grab_facets();

    facet_segments_col_type & grab_facet_segments();

    int add_vertex(unsigned int vtx_key_, const facet_vertex & vtx_);

    int add_vertex(unsigned int vtx_key_,
                    double x_, double y_, double z_);

    void add_facet(unsigned int facet_key_,
                    int ivtx0_, int ivtx1_, int ivtx2_, int ivtx3_ = INVALID_VERTEX);

    void add_facet3(unsigned int facet_key_,
                     int ivtx0_, int ivtx1_, int ivtx2_);

    void add_facet4(unsigned int facet_key_,
                     int ivtx0_, int ivtx1_, int ivtx2_, int ivtx3_);

    void remove_facet(unsigned int facet_key_);

    void compute_bounding_box();

    void compute_facet_segments();

    const mygsl::min_max & get_bounding_box_x() const;

    const mygsl::min_max & get_bounding_box_y() const;

    const mygsl::min_max & get_bounding_box_z() const;

    void print_xyz(std::ostream & out_) const;

    void dump(std::ostream & out_ = std::clog) const;

    virtual bool is_inside(const vector_3d &,
                            double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    // if 'skin' < 0 no skin is taken into account:
    virtual bool is_on_surface(const vector_3d & ,
                                int index_   = FACE_ALL ,
                                double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual vector_3d get_normal_on_surface(const vector_3d & position_) const;

    virtual bool find_intercept(const vector_3d & from_,
                                 const vector_3d & direction_,
                                 intercept_t & intercept_,
                                 double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Initialize from properties
    virtual void initialize(const datatools::properties &, const handle_dict_type * = 0);

    /// Reset
    virtual void reset();

    /// Initialize from STL file
    void initialize_from_stl(const std::string & filename_,
                             double length_unit_);

  protected:

    /// Executed at lock stage
    virtual void _at_lock();

    /// Executed at unlock stage
    virtual void _at_unlock();

    /// Build the bounding data
    virtual void _build_bounding_data();

  private:

    bool _check_();

  private:

    bool              _consistent_;
    vertices_col_type _vertices_;
    facets_col_type   _facets_;
    mygsl::min_max    _xrange_;
    mygsl::min_max    _yrange_;
    mygsl::min_max    _zrange_;
    facet_segments_col_type _facet_segments_;

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(tessellated_solid);

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_TESSELLATION_H
