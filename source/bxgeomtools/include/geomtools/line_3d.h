// -*- mode: c++; -*-
/// \file geomtools/line_3d.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2008-12-18
 * Last modified: 2008-12-18
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

namespace geomtools {

  // Forward declaration:
  class polyline_3d;
  class placement;

  /// \brief An line/segment (1D shape) in a 3D space
  class line_3d :
    public i_shape_1d,
    DATATOOLS_SERIALIZABLE_CLASS
  {
  public:

    /// Return the identifier label for the class
    static const std::string & line_3d_label();

    /// Check if the shape is normal
    bool is_normal();

    /// Return the identifier/name of the shape
    virtual std::string get_shape_name() const;

    /// Check if the line is valid
    bool is_valid() const;

    /// Invalidate the line
    void invalidate();

    /// Return the start position on the line
    const vector_3d & get_first() const;

    /// Set the start position on the line
    void set_first(const vector_3d &);

    /// Return the stop position on the line
    const vector_3d & get_last() const;

    /// Set the stop position on the line
    void set_last(const vector_3d &);

    /// Return the position at given parameter
    vector_3d get_point(double t_) const;

    /// Return the length of the linear segment
    virtual double get_length(uint32_t flags_ = PATH_ALL_BITS) const;

    /// Default constructor
    line_3d();

    /// Constructor
    line_3d(const vector_3d & first_,
            const vector_3d & last_);

    /// Destructor
    virtual ~line_3d();

    /// Smart print
    virtual void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_          = false) const;

    /// Basic print
    void dump() const;

    /// Print approximated data (x,y,z) triplets representing the helix
    static void print_xyz(std::ostream & out_,
                          const line_3d & line_);

    /// Return the distance of a point to the line
    double get_distance_to_line( const vector_3d & position_ ) const;

    /// Check if a position in on the line
    virtual bool is_on_curve(const vector_3d & position_,
                             double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    /// Return the direction ar some position in on the line
    virtual vector_3d get_direction_on_curve(const vector_3d & position_) const;

    /// Compute a collection of vertexes representing the line
    void make_vertex_collection(basic_polyline_3d &) const;

    /// Return a collection of vertexes representing the line
    basic_polyline_3d make_vertex_collection() const;

    /// Generate a list of polylines using some placement
    void generate_wires(std::list<polyline_3d> &, const placement & )const;

  private:

    vector_3d _first_; /// Start point of the segment
    vector_3d _last_;  /// Stop point of the segment

    // Serialization interface
    DATATOOLS_SERIALIZATION_DECLARATION();

  };

} // end of namespace geomtools

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(geomtools::line_3d, "geomtools::line_3d")

#endif // GEOMTOOLS_LINE_3D_H
