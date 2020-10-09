/// \file geomtools/regular_circular_placement.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-26
 * Last modified: 2010-03-31
 *
 * License:
 *
 * Description:
 *  Utilities.
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_REGULAR_CIRCULAR_PLACEMENT_H
#define GEOMTOOLS_REGULAR_CIRCULAR_PLACEMENT_H 1

// Standard library:
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

// This project:
#include <geomtools/i_placement.h>
#include <geomtools/placement.h>
#include <geomtools/utils.h>

namespace geomtools {

  /// \brief Regular circular placement
  class regular_circular_placement : public i_placement
  {
  public:

    bool is_valid() const;

    void invalidate();

    size_t get_dimension() const override;

    bool is_replica() const override;

    bool has_only_one_rotation() const override;

    void set_rotation_axis(rotation_axis_type);

    rotation_axis_type get_rotation_axis() const;

    bool is_rotation_x_axis() const;

    bool is_rotation_y_axis() const;

    bool is_rotation_z_axis() const;

    void set_follow_rotation(bool);

    bool is_follow_rotation() const;

    void set_radius(double radius_);

    double get_radius() const;

    void set_start_angle(double angle_);

    double get_start_angle() const;

    void set_step_angle(double angle_);

    double get_step_angle() const;

    void set_center(const vector_3d &);

    const vector_3d & get_center() const;

    void set_number_of_items(size_t n_);

    size_t get_number_of_items() const override;

    void get_placement(int item_, placement & p_) const override;

    size_t compute_index_map(std::vector<uint32_t> & map_,
                                     int item_) const override;

    /// Default constructor
    regular_circular_placement();

    /// Constructor
    regular_circular_placement(const vector_3d & center_,
                               double radius_,
                               double step_angle_,
                               double start_angle_,
                               size_t number_of_items_,
                               rotation_axis_type rotation_axis_ = ROTATION_AXIS_Z);

    /// Destructor
    ~regular_circular_placement() override;

    /// Initialization
    void initialize(const vector_3d & center_,
                    double radius_,
                    double step_angle_,
                    double start_angle_,
                    size_t number_of_items_,
                    rotation_axis_type rotation_axis_ = ROTATION_AXIS_Z);

    /// Reset
    virtual void reset();

    /// Smart print
    void tree_dump(std::ostream & out_  = std::clog,
                           const std::string & title_ = "geomutils::regular_circular_placement",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const override;

  protected:

    /// Set default:
    void _set_default();

  private:

    vector_3d _center_; ///< Intersection of the rotation axis with the plane
    double    _radius_; ///< Radius of the circle
    double    _step_angle_; ///< Step angle
    double    _start_angle_; ///< Start angle
    size_t    _number_of_items_; ///< Number of items places along the arc
    bool      _follow_rotation_; ///< Flag that indicates that items'orientation follow the rotation
    rotation_axis_type _rotation_axis_; ///< Rotation axis identifier

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_REGULAR_CIRCULAR_PLACEMENT_H


// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
