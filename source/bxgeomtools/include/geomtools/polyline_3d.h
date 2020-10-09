/// \file geomtools/polyline_3d.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2009-03-31
 * Last modified: 2009-03-31
 */

#ifndef GEOMTOOLS_POLYLINE_3D_H
#define GEOMTOOLS_POLYLINE_3D_H 1

// Standard library:
#include <iostream>
#include <stdexcept>
#include <list>

// Third party:
// - Bayeux/datatools:
#include <datatools/i_serializable.h>

// This project:
#include <geomtools/i_shape_1d.h>
#include <geomtools/utils.h>
#include <geomtools/i_wires_3d_rendering.h>

namespace geomtools {

  /// \brief A sequence of connected linear segments (3D)
  class polyline_3d : public i_shape_1d,
                      public i_wires_3d_rendering
  {
  public:

    /// Return the identifier of the polyline 3D-object
    static const std::string & polyline_3d_label();

    static const bool CLOSED = true;
    static const bool closed = CLOSED;
    static const bool OPEN   = false;
    static const bool open   = OPEN;
    static const bool DEFAULT_CLOSED = OPEN;

    /// A sequence of vertexes
    typedef polyline_type point_col;

    /// Return the name of the shape
    std::string get_shape_name() const override;

    /// Check if the polyline is valid
    bool is_valid() const override;

    /// Check if the polyline is closed
    bool is_closed() const;

    /// Set the 'closed' flag
    void set_closed(bool);

    /// Check if the polyline has no points
    bool is_empty() const;

    /// Default constructor
    polyline_3d();

    /// Constructor
    polyline_3d(bool closed_);

    /// Destructor
    ~polyline_3d() override;

    /// Remove all points
    void clear();

    /// Add a point at the end of the sequence
    void add(const vector_3d &);

    /// Return the number of points in the sequence
    int get_number_of_points() const;

    /// Return the point at given index
    const vector_3d & get_point(int i_) const;

    /// @deprecated Return the number of points in the sequence
    int get_number_of_vertex() const;

    /// Return the number of vertexes in the sequence
    int get_number_of_vertexes() const;

    /// Return the vertex at given index
    const vector_3d & get_vertex(int i_) const;

    /// Return the length of the linear segment
    double get_length(uint32_t flags_ = PATH_ALL_BITS) const override;

    // inefficient algorithm:
    void make_vertex_collection(basic_polyline_3d &) const;

    // inefficient algorithm:
    polyline_type make_vertex_collection() const;

    /// Check if a point belongs to the polyline
    bool is_on_curve(const vector_3d & position_,
                             double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Return the direction at some position along the polyline
    vector_3d get_direction_on_curve(const vector_3d & position_) const override;

    /// Return a non mutable reference to the collection of points
    const polyline_type & get_points() const;

    /// Return a mutable reference to the collection of points
    polyline_type & grab_points();

    /// Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_               = false) const override;

    /// Generate a sequence of polylines for wires 3D rendering
    void generate_wires_self(wires_type & wires_,
                                     uint32_t options_ = 0) const override;

  private:

    bool          _closed_; //!< Flag to close the polyline
    polyline_type _points_; //!< Sequence of points/vertexes

    //! Serialization support
    DATATOOLS_SERIALIZATION_DECLARATION()

  };

} // end of namespace geomtools

BOOST_CLASS_EXPORT_KEY2(geomtools::polyline_3d, "geomtools::polyline_3d")

// Explicit class version:
#include <boost/serialization/version.hpp>
BOOST_CLASS_VERSION(geomtools::polyline_3d, 2)

#endif // GEOMTOOLS_POLYLINE_3D_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
