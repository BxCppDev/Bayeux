/// \file geomtools/i_wires_3d_rendering.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2012-10-22
 * Last modified: 2015-02-15
 *
 * License:
 *
 * Description:
 *
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
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/bit_mask.h>
#include <datatools/properties.h>

// This project:
#include <geomtools/placement.h>
#include <geomtools/utils.h>

namespace geomtools {

  // Forward declaration:
  class i_shape_3d;

  /// brief Abstract interface for objects that can describe themselves as a collection of polylines for 3D rendering
  class i_wires_3d_rendering
  {
  public:

    //! \brief Wires rendering optional flags
    enum wires_rendering_option_type {
      WR_NONE                          = 0,
      WR_BASE_GRID                     = datatools::bit_mask::bit00, //!< Activate grid rendering
      WR_BASE_GRID_LOW_DENSITY         = (WR_BASE_GRID << 1),  //!< Activate grid with low density
      WR_BASE_GRID_HIGH_DENSITY        = (WR_BASE_GRID << 2),  //!< Activate grid with high density
      WR_BASE_GRID_VERY_HIGH_DENSITY   = (WR_BASE_GRID << 3),  //!< Activate grid with very high density
      WR_BASE_GRID_HUGE_DENSITY        = (WR_BASE_GRID << 4),  //!< Activate grid with huge density
      WR_BASE_LOW_ANGLE_SAMPLING       = (WR_BASE_GRID << 5),  //!< Use low sampling for angles
      WR_BASE_HIGH_ANGLE_SAMPLING      = (WR_BASE_GRID << 6),  //!< Use high sampling for angles
      WR_BASE_VERY_HIGH_ANGLE_SAMPLING = (WR_BASE_GRID << 7),  //!< Use very high sampling for angles
      WR_BASE_HUGE_ANGLE_SAMPLING      = (WR_BASE_GRID << 8),  //!< Use huge sampling for angles
      WR_BASE_BEST_GRID_SAMPLING       = (WR_BASE_GRID << 9),  //!< Activate grid with guessed best density
      WR_BASE_BEST_ANGLE_SAMPLING      = (WR_BASE_GRID << 10), //!< Use guessed best sampling for angles
      WR_BASE_UNUSED_11                = (WR_BASE_GRID << 11), //!< Reserved for future usage
      WR_BASE_UNUSED_12                = (WR_BASE_GRID << 12), //!< Reserved for future usage
      WR_BASE_UNUSED_13                = (WR_BASE_GRID << 13), //!< Reserved for future usage
      WR_BASE_BOUNDINGS                = (WR_BASE_GRID << 14), //!< Display the bounding volume
      WR_BASE_EXPLODE                  = (WR_BASE_GRID << 15), //!< Explode the shape view to help debugging
      WR_BASE_LAST                     = (WR_BASE_EXPLODE),    //!< Last defined bit
      WR_BASE_MASK                     = datatools::bit_mask::nbits16 //!< Base rendering options bit mask
    };

    //! \brief Sampling levels
    enum sampling_level_type {
      SL_NONE      = 0, //!< No sampling
      SL_LOW       = 1, //!< Low density sampling
      SL_NORMAL    = 2, //!< Default/normal density sampling
      SL_HIGH      = 3, //!< High density sampling
      SL_VERY_HIGH = 4, //!< Very high density sampling
      SL_HUGE      = 5  //!< Huge density sampling
    };

    //! Return linear sampling from sampling level
    static uint32_t linear_sampling(sampling_level_type);

    //! Return linear sampling from rendering options
    static uint32_t linear_sampling_from_options(uint32_t);

    //! Return angular sampling from sampling level
    static uint32_t angular_sampling(sampling_level_type);

    //! Return angular sampling from rendering options
    static uint32_t angular_sampling_from_options(uint32_t);

    //! Boost the linear sampling
    static void boost_linear_sampling(uint32_t &);

    //! Destructor
    virtual ~i_wires_3d_rendering();

    //! Generate a collection of polylines representing a 3D object in 'wires'
    //! display mode at some given placement
    void generate_wires(wires_type & wires_,
                        const placement & positioning_,
                        uint32_t options_ = 0) const;

    //! Gener(ate a collection of polylines representing a 3D object in 'wires'
    //! display mode at some given position and orientation
    void generate_wires(wires_type & wires_,
                        const geomtools::vector_3d & position_,
                        const geomtools::rotation_3d & rotation_,
                        uint32_t options_ = 0) const;

    //! Generate a collection of polylines representing a 3D object in 'wires'
    //! display mode using the object coordinate frame
    virtual void generate_wires_self(wires_type & wires_,
                                     uint32_t options_ = 0) const = 0;

  };

  //! Transform a polyline
  void transform_wire_to(const placement & p_,
                         const polyline_type & wire_,
                         polyline_type & tr_wire_);

  //! Transform a collection of polylines
  void transform_wires_to(const placement & p_,
                          const wires_type & wires_,
                          wires_type & tr_wires_);

  //! Parse a collection of polylines from an ASCII stream
  bool parse_wires(std::istream & in_, wires_type & wires_);

  //! Save a collection of polylines in an ASCII stream
  void save_wires(std::ostream & out_, const wires_type & wires_, uint32_t flags_ = 0);

  //! \brief A classified segment consists in a 3D segment with
  //!        a first and a last point in an arbitrary reference frame
  //!        and which is assigned a property that details if the segment
  //!        lies inside a volume, outside a volume or is contained on the surface
  //!        of a volume.
  struct classified_segment
  {
    //! Default constructor
    classified_segment();

    //! Reset
    void reset();

    //! Convert to a 3D segment object
    void to_line(segment_type & segment_) const;

    //! Convert to a 3D polyline object
    void to_wire(polyline_type & wire_) const;

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
  void classify_in_out_segment(const segment_type & segment_,
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
    void add(const segment_type & segment_);

    //! Process a polyline
    void add(const polyline_type & path_);

    //! Process a collection of polyline
    void add(const wires_type & paths_);

    //! Fetch and build the segments with 'inside' classification
    unsigned int fetch_inside_segments(std::list<segment_type> & segments_);

    //! Fetch and build the wires with 'inside' classification
    unsigned int fetch_inside_wires(wires_type & wires_);

    //! Fetch and build the segments with 'outside' classification
    unsigned int fetch_outside_segments(std::list<segment_type> & segments_);

    //! Fetch and build the wires with 'outside' classification
    unsigned int fetch_outside_wires(wires_type & wires_);

    //! Fetch and build the segments with 'on surface' classification
    unsigned int fetch_surface_segments(std::list<segment_type> & segments_);

    //! Fetch and build the wires with 'surface' classification
    unsigned int fetch_surface_wires(wires_type & wires_);

    //! Fetch and build all the segments
    unsigned int fetch_all_segments(std::list<segment_type> & segments_);

    //! Fetch and build all the wires
    unsigned int fetch_all_wires(wires_type & wires_);

  private:

    // Setup:
    const i_shape_3d * _shape_;           //!< Reference to a 3D shape
    placement          _shape_placement_; //!< Placement of the shape
    double             _step_;            //!< Step size for first stage splitting of segment
    double             _tolerance_;       //!< Absolute tolerance for bisection

    // Work:
    std::list<classified_segment> _clsegs_; //!< Collection of accumulated classified segments

  };


  struct wires_3d_rendering_hints
  {
    uint32_t wr_options = 0;

    void initialize(const datatools::properties & config_);
    
  };

} // end of namespace geomtools

#endif // GEOMTOOLS_I_WIRES_3D_RENDERING_H
