/// \file geomtools/tessellation.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-06-04
 * Last modified: 2021-04-13
 *
 * License:
 *
 * Description: Tessellated solid
 *
 */

#ifndef GEOMTOOLS_TESSELLATION_H
#define GEOMTOOLS_TESSELLATION_H 1

// Standard library:
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
// - Bayeux/mygsl:
#include <mygsl/min_max.h>

// This project:
#include <geomtools/i_shape_2d.h>
#include <geomtools/i_shape_3d.h>

namespace geomtools {

  // Forward declarations:
  class facet34;
  class box;
  class triangle;
  class quadrangle;

  /// \brief A vertex (corner) of a triangle or quadrangle facet in a tessellated solid
  struct facet_vertex
  {

  public:

    /// Check validity
    bool is_valid() const;

    /// Default constructor
    facet_vertex();

    /// Constructor
    facet_vertex(double x_, double y_, double z_);

    /// Print in 'x,y,z[,color]' format (ASCII)
    void print_xyz(std::ostream & out_, int color_ = 0) const;

    /// Simple print
    void print(std::ostream & out_) const;

    /// Add the reference of a facet the vertex is associated to
    void add_ref_facet(int facet_index_, int facet_node_);

    /// Return the position of the vertex
    const vector_3d & get_position() const;

    /// Set the position of the vertex
    void set_position(const vector_3d &);

    /// Return the position of the vertex
    vector_3d & grab_position();

    /// Reset
    void reset();

    /// Print
    friend std::ostream & operator<<(std::ostream &, const facet_vertex &);

  public:

    vector_3d position; //!< Position of the vertex
    std::map<int32_t, int32_t> ref_facets; //!< Collection of facets the vertex
                                           //!< belongs to; this is a map indexed
                                           //!< by the unique integer key of the
                                           //!< facet and storing the rank of the
                                           //!< vertex in the facet (using
                                           //!< counterclockwise order)
 
  };

  /// \brief A triangle or convex quadrangle facet of a tessellated solid
  class facet34
  {

  public:

    /// The maximum index of the category the facet belongs to
    static const unsigned int MAX_CATEGORY = 30;
    static const int32_t INVALID_CATEGORY = -1;

    bool has_category() const;

    void set_category(unsigned int c_);

    void unset_category();

    uint32_t get_number_of_vertices() const;

    const facet_vertex & get_vertex(int i_) const;

    int32_t get_vertex_key(int i_) const;

    bool has_normal() const;

    bool has_surface() const;

    bool is_valid() const;

    const vector_3d & get_normal() const;

    double get_surface() const;

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

    /// Default constructor
    facet34();

    /// Constructor for a triangle facet
    facet34(const facet_vertex & v0_,
            const facet_vertex & v1_,
            const facet_vertex & v2_,
            int, int, int);

    /// Constructor for a quadrangle facet
    facet34(const facet_vertex & v0_,
            const facet_vertex & v1_,
            const facet_vertex & v2_,
            const facet_vertex & v3_,
            int, int, int, int);

    /// Check the availability of face (triangle or quadrangle)
    bool has_face() const;

    /// Check the availability of triangle face
    bool has_tface() const;

    /// Check the availability of quadrangle face
    bool has_qface() const;

    /// Return the associated triangle face
    const triangle & get_tface() const;

    /// Return the associated quadrangle face
    const quadrangle & get_qface() const;

    const face_handle_type & get_tface_handle() const;

    face_handle_type & grab_tface_handle();

    const face_handle_type & get_qface_handle() const;

    face_handle_type & grab_qface_handle();

  protected:

    /// Set default attributes values
    void _set_defaults();

    void _compute_normal();

    void _compute_surface();

    void _compute_internal_angles(); // Not implemented yet

    void _compute_face();

    void _compute_internals();

  private:

    // Primary data:
    uint32_t             _number_of_vertices_; //!< 3 (triangle) or 4 (quadrangle)
    const facet_vertex * _vertices_[4];        //!< Addresses to the vertices
    int32_t              _vertices_keys_[4];   //!< Key of the vertices

    // Working data:
    geomtools::vector_3d _normal_;             //!< Normal vector to the surface (counterclockwise)
    double               _surface_;            //!< Surface of the facet
    double               _internal_angles_[4]; //!< Internal angles per vertex
    int32_t              _category_;           //!< Auxiliary integer property
    face_handle_type     _tface_;
    face_handle_type     _qface_;

  };

  /// \brief A segment associated to a facet
  class facet_segment
  {
  public :

    /// \brief The Id of the vertex
    enum vertex_id_type {
      VTX_ID_NONE   = -1,
      VTX_ID_FIRST  = 0,
      VTX_ID_SECOND = 1
    };

    /// Check validity
    bool is_valid() const;

    /// Check if the segment is shown
    bool is_shown() const;

    /// Set the shown flag
    void set_shown(bool);

    /// Default constructor
    facet_segment();

    /// Set vertex keys
    void set_vertex_keys(int, int);

    /// Set vertexes (associated to vertex keys)
    void set_vertexes(const facet_vertex & vtx0_, const facet_vertex & vtx1_);

    /// Check if the vertex at given rank is defined
    bool has_vertex(vertex_id_type) const;

    /// Return the vertex at given rank (first/second)
    const vector_3d & get_vertex_position(vertex_id_type) const;

    /// Set facet keys
    void set_facet_keys(int, int);

    void set(int, int, int, int);

    friend bool operator==(const facet_segment & fs1_, const facet_segment & fs2_);

    friend bool operator<(const facet_segment & fs1_, const facet_segment & fs2_);

    /// Print
    void dump(std::ostream & out_) const;

  public :

    int vertex0_key; //!< Key of the first vertex
    int vertex1_key; //!< Key of the second vertex
    int facet0_key;  //!< Key of the first facet
    int facet1_key;  //!< Key of the second facet

  private:

    bool _shown_;      //!< Flag indicating if this segment is displayed
    const facet_vertex * _vertex0_; //!< Handle to the first vertex
    const facet_vertex * _vertex1_; //!< Handle to the second vertex

  };

  /// \brief Tessellated solid made of facets
  class tessellated_solid : public i_shape_3d,
                            public i_stackable
  {
  public:

    static const int INVALID_VERTEX      = -1;
    static const int MAX_VERTEX          = 0x0FFFFFFF;
    static const unsigned int INVALID_FACET_INDEX = 0xFFFFFFFF;

    /// Return the identifier label for the class
    static const std::string & tessellated_label();

    typedef std::map<unsigned int, facet_vertex>  vertices_col_type;
    typedef std::map<unsigned int, facet34>       facets_col_type;
    typedef std::map<unsigned int, facet_segment> facet_segments_col_type;

    /// Return the identifier/name of the shape
    std::string get_shape_name() const override;

    static bool validate_index(int);

    /// Check the consistent flag
    bool is_consistent() const;

    /// Check if the face identification scheme is based on face bits
    bool using_face_id_bits() const override;

    /// Check if the face identification scheme is based on face index
    bool using_face_id_index() const override;

    /// Default constructor
    tessellated_solid();

    /// Destructor
    ~tessellated_solid() override;

    /// Return the collection of vertexes
    const vertices_col_type & vertices() const;

    /// Return the collection of facets
    const facets_col_type & facets() const;

    /// Return the collection of facet segments
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

    bool has_facet(unsigned int facet_key_) const;

    const facet34 & get_facet(unsigned int facet_key_) const;

    void compute_bounding_box();

    const mygsl::min_max & get_bounding_box_x() const;

    const mygsl::min_max & get_bounding_box_y() const;

    const mygsl::min_max & get_bounding_box_z() const;

    /// Return the min X coordinates (bounding box)
    double get_xmin() const override;

    /// Return the max X coordinates (bounding box)
    double get_xmax() const override;

    /// Return the min Y coordinates (bounding box)
    double get_ymin() const override;

    /// Return the max Y coordinates (bounding box)
    double get_ymax() const override;

    /// Return the min Z coordinates (bounding box)
    double get_zmin() const override;

    /// Return the max Z coordinates (bounding box)
    double get_zmax() const override;

    /// Print
    void print_xyz(std::ostream & out_) const;

    /// Print
    void dump(std::ostream & out_ = std::clog) const;

    /// Check if a point is inside the frustrum
    bool is_inside (const vector_3d &,
                    double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Check if a point is outside the frustrum
    bool is_outside (const vector_3d &,
                     double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Return the surface bit a point belongs to
    face_identifier on_surface(const vector_3d &,
                               const face_identifier & a_surface_mask = face_identifier::FACE_INDEX_ANY,
                               double a_skin = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Compute the normal to the surface of the furstrum
    vector_3d get_normal_on_surface (const vector_3d & position_,
                                             const face_identifier & ) const override;

    /// Find the intercept point with a face of the frustrum
    bool find_intercept(const vector_3d & from_,
                        const vector_3d & direction_,
                        face_intercept_info & intercept_,
                        double skin_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Compute informations about the faces of this solid shape
    unsigned int compute_faces(face_info_collection_type &) const override;

    /// \brief 3D rendering options
    enum tessella_wires_rendering_option_type {
      WR_TESSELLA_ALL_SEGMENTS   = (WR_BASE_LAST << 1),        //!< Render all segments
      WR_TESSELLA_LAST           = (WR_TESSELLA_ALL_SEGMENTS), //!< Last defined bit
      WR_TESSELLA_MASK           = (WR_TESSELLA_ALL_SEGMENTS)
    };

    /// Generate a list of polylines representing the contour of the shape (for display clients)
    void generate_wires_self(wires_type & wires_,
                             uint32_t options_ = 0) const override;


    /// Initialize from properties
    void initialize(const datatools::properties &, const handle_dict_type * = 0) override;

    /// Reset
    void reset() override;

    /// Initialize from STL file
    void initialize_from_stl(const std::string & filename_,
                             double length_unit_);

    /// Check the validity
    bool is_valid() const override;

    /// Smart print
    void tree_dump(std::ostream & a_out         = std::clog,
                   const std::string & a_title  = "",
                   const std::string & a_indent = "",
                   bool a_inherit          = false) const override;

  protected:

    /// Executed at lock stage
    void _at_lock() override;

    /// Executed at unlock stage
    void _at_unlock() override;

    /// Build the bounding data
    void _build_bounding_data() override;

    /// Compute the collection of facet segments
    void _compute_facet_segments();

    /// Check if a position is on a facet
    bool _on_facet(const vector_3d & position_, unsigned int & facet_index_, double skin_) const;

    /// Find the intercept point with a face of the solid
    bool _find_intercept_exclude(const vector_3d & from_,
                                 const vector_3d & direction_,
                                 face_intercept_info & intercept_,
                                 double skin_,
                                 const std::set<unsigned int> & excluded_facet_indexes_) const;

    /// Find the intercept points with several faces of the solid
    unsigned int _find_intercepts_exclude(const vector_3d & from_,
                                          const vector_3d & direction_,
                                          std::vector<face_intercept_info> & intercepts_,
                                          double skin_,
                                          const std::set<unsigned int> & excluded_facet_indexes_) const;

  public:
    
    /// Store in a stream 
    void store(std::ostream & out_, double length_unit_ = -1.0) const;

    /// Load form a file
    void load(std::istream & in_);
    
    /// Store in a file stream 
    void store(const std::string & filename_, double length_unit_ = -1.0) const;
    
    /// Load from a file stream 
    void load(const std::string & filename_);

  private:

    /// Check internal data
    bool _check_();

  private:

    bool              _consistent_; //!< Flag to tag coherence of the tessellated solid
    vertices_col_type _vertices_;   //!< List of vertexes
    facets_col_type   _facets_; //!< List of facets
    mygsl::min_max    _xrange_; //!< Range on the X coordinate
    mygsl::min_max    _yrange_; //!< Range on the Y coordinate
    mygsl::min_max    _zrange_; //!< Range on the Z coordinate
    facet_segments_col_type _facet_segments_; //!< List of facet segments

    // Registration interface :
    GEOMTOOLS_OBJECT_3D_REGISTRATION_INTERFACE(tessellated_solid)

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_TESSELLATION_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
