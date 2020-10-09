/// \file geomtools/line_3d.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-12-18
 * Last modified: 2015-02-22
 *
 * License:
 *
 * Description:
 *   Line 3D model
 *
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_LINE_3D_H
#define GEOMTOOLS_LINE_3D_H 1

// Standard library:
#include <iostream>
#include <string>

// - Bayeux/datatools:
#include <datatools/i_tree_dump.h>
#include <datatools/i_serializable.h>

// This project:
#include <geomtools/i_shape_1d.h>
#include <geomtools/utils.h>
#include <geomtools/i_wires_3d_rendering.h>

namespace geomtools {

  // Forward declaration:
  class placement;

  /// \brief An line/segment (1D shape) in a 3D space
  class line_3d : public i_shape_1d,
                  public i_wires_3d_rendering
  {
  public:

    /// Return the identifier label for the class
    static const std::string & line_3d_label();

    /// Check if the shape is normal
    bool is_normal();

    /// Return the identifier/name of the shape
    std::string get_shape_name() const override;

    /// Check if the line is valid
    bool is_valid() const override;

    /// Invalidate the line
    void invalidate();

    /// Return the start position on the line
    const vector_3d & get_first() const;

    /// Set the start position on the line
    void set_first(double x_, double y_, double z_);

    /// Set the start position on the line
    void set_first(const vector_3d &);

    /// Return the stop position on the line
    const vector_3d & get_last() const;

    /// Set the last position on the line
    void set_last(double x_, double y_, double z_);

    /// Set the stop position on the line
    void set_last(const vector_3d &);

    /// Return the position at given parameter
    vector_3d get_point(double t_) const;

    /// Return the length of the linear segment
    double get_length(uint32_t flags_ = PATH_ALL_BITS) const override;

    /// Default constructor
    line_3d();

    /// Constructor
    line_3d(const vector_3d & first_,
            const vector_3d & last_);

    /// Constructor
    line_3d(double x0_, double y0_, double z0_,
            double x1_, double y1_, double z1_);

    /// Constructor
    line_3d(const segment_type & segment_);

    /// Destructor
    ~line_3d() override;

    /// Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_               = false) const override;

    /// Print approximated data (x,y,z) triplets representing the line
    static void print_xyz(std::ostream & out_, const line_3d & line_);

    /// Compute the orthogonal projection of a point on the line
    ///
    ///                   P
    ///                  +
    ///                 /:
    ///                / :
    ///               /  :
    ///              /   :
    ///  -----------+==>-+-------
    ///             M u   H
    ///
    static void orthogonal_projection(const vector_3d & p_,
                                      const vector_3d & m_,
                                      const vector_3d & u_,
                                      vector_3d & h_);

    /// Compute the distance of a point to a line
    ///
    ///                   P
    ///                  +
    ///                 /:
    ///                / : d
    ///               /  :
    ///              /   :
    ///  -----------+==>-+-------
    ///             M  u
    ///
    static double distance_to_line(const vector_3d & p_,
                                   const vector_3d & m_,
                                   const vector_3d & u_);

    /// Return the distance of a point to the segment
    ///
    ///                P
    ///               +
    ///               |
    ///        :      | d      :
    ///        :      |        :
    ///        +------*--------+
    ///      F :               : L
    ///        :               :
    ///
    double get_distance_to_line(const vector_3d & position_ ) const;

    /// Compute the orthogonal projection on the segment
    bool compute_orthogonal_projection(const vector_3d & position_, vector_3d & proj_) const;

    /// Check if a position is along the support line of the segment
    bool is_on_line(const vector_3d & position_,
                    double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Check if a position is on the line segment
    bool is_on_segment(const vector_3d & position_,
                       double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Check if a position is on the line segment
    bool is_on_curve(const vector_3d & position_,
                             double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Return the direction at some position in on the line
    vector_3d get_direction_on_curve(const vector_3d & position_) const override;

    /// Compute a collection of vertexes representing the line
    void make_vertex_collection(polyline_type &) const;

    /// Return a collection of vertexes representing the line
    polyline_type make_vertex_collection() const;

    /// Generate a sequence of polylines for wires 3D rendering
    void generate_wires_self(wires_type & wires_,
                                     uint32_t options_ = 0) const override;

  private:

    vector_3d _first_; //!< Start point of the segment
    vector_3d _last_;  //!< Stop point of the segment

    // Serialization interface
    DATATOOLS_SERIALIZATION_DECLARATION()

  };

  //! Type alias for the line_3d class
  typedef line_3d segment_3d;

} // end of namespace geomtools

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(geomtools::line_3d, "geomtools::line_3d")

// Class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(geomtools::line_3d, 2)

#endif // GEOMTOOLS_LINE_3D_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
