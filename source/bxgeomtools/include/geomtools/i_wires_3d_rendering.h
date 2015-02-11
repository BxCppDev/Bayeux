// -*- mode: c++ ; -*-
/// \file geomtools/i_wires_3d_rendering.h
/* Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-10-22
 * Last modified: 2010-10-22
 *
 * License:
 *
 * Description:
 *   A abstract interface for wires 3D rendering
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_I_WIRES_3D_RENDERING_H
#define GEOMTOOLS_I_WIRES_3D_RENDERING_H 1

// Standard library:
#include <list>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>

// This project:
#include <geomtools/polyline_3d.h>
#include <geomtools/placement.h>
#include <geomtools/utils.h>
#include <geomtools/line_3d.h>

namespace geomtools {

  // Forward declaration:
  class i_shape_3d;

  /// brief Abstract interface for object able to describe themselves as a collection of polylines for 3D rendering
  struct i_wires_3d_rendering
  {
  public:

    //! Destructor
    virtual ~i_wires_3d_rendering();

    //! Generate a collection of polylines representing a 3D object in 'wires' display mode at some given position
    virtual void generate_wires(std::list<polyline_3d> & wires_,
                                const placement & positioning_,
                                uint32_t options_ = 0) const = 0;

    //! Generate a collection of polylines representing a 3D object in 'wires' display mode using the object coordinate frame
    void generate_wires_self(std::list<polyline_3d> & wires_, uint32_t options_ = 0) const;

  };

  //! Parse a collection of polylines from an ASCII stream
  bool parse_wires(std::istream & in_, std::list<polyline_3d> & wires_);

  //! Save a collection of polylines in an ASCII stream
  void save_wires(std::ostream & out_, const std::list<polyline_3d> & wires_, uint32_t flags_ = 0);

  //! \brief A classified segment consists in a 3D segment with
  //!        a first and a last point in an arbitrary reference frame
  //!        and which is assigned a property that details if the segment
  //!        lies inside a volume, outside a volume or is contained by the surface
  //!        of a volume.
  struct classified_segment
  {
    //! Default constructor
    classified_segment();

    //! Reset
    void reset();

    //! Convert to a 3D segment object
    void to_line(line_3d & segment_) const;

    //! Check if the segment is inside the volume
    bool is_inside() const;

    //! Check if the segment is outside the volume
    bool is_outside() const;

    //! Check if the segment is on the surface of the volume
    bool is_on_surface() const;

    vector_3d first; //!< First point of a segment
    vector_3d last;  //!< Last point of a segment
    shape_domain_flags_type domain; //!< Domain flag of the segment

  };

  //! Split a given segment into a collection of classified segments with respect
  //! to a given 3D shape/solid volume.
  void classify_in_out_segment(const line_3d & segment_,
                               const i_shape_3d & shape_,
                               const placement & shape_placement_,
                               double step_,
                               double tolerance_,
                               std::list<classified_segment> & classified_);

  //! \brief This algorithm splits a segment, a polyline or a collection of polylines
  //!        in a sequence of smaller segments that are classified as inside, outside
  //!        or on surface of a given shape
  class split_segment_wrt_shape
  {
  public:

    //! Default constructor
    split_segment_wrt_shape();

    //! Reset the current configuration
    void reset();

    //! Clear former results
    void clear();

    //! Configure the algorithm
    void configure(const i_shape_3d & shape_,
                   double step_,
                   double tolerance_);

    //! Configure the algorithm
    void configure(const i_shape_3d & shape_,
                   const placement & shape_placement_,
                   double step_,
                   double tolerance_);

    //! Check configuration flag
    bool is_configured() const;

    //! Process a segment
    void add(const line_3d & segment_);

    //! Process a polyline
    void add(const polyline_3d & path_);

    //! Process a collection of polyline
    void add(const std::list<polyline_3d> & paths_);

    //! Fetch and build the segments with 'inside' classification
    unsigned int fetch_inside_segments(std::list<line_3d> & segments_);

    //! Fetch and build the segments with 'outside' classification
    unsigned int fetch_outside_segments(std::list<line_3d> & segments_);

    //! Fetch and build the segments with 'on surface' classification
    unsigned int fetch_surface_segments(std::list<line_3d> & segments_);

    //! Fetch and build all the segments
    unsigned int fetch_all_segments(std::list<line_3d> & segments_);

  private:

    // Setup:
    const i_shape_3d * _shape_;       //!< Reference to a 3D shape
    placement          _shape_placement_; //!< Placement of the shape
    double             _step_;        //!< Step size for first stage splitting of segment
    double             _tolerance_;   //!< Absolute tolerance for bisection

    // Work:
    std::list<classified_segment> _clsegs_; //!< Collection of accumulated classified segments

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_I_WIRES_3D_RENDERING_H
