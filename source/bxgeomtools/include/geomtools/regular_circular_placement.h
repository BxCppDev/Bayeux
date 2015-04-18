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

    virtual size_t get_dimension() const;

    virtual bool is_replica() const;

    virtual bool has_only_one_rotation() const;

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

    virtual size_t get_number_of_items() const;

    virtual void get_placement(int item_, placement & p_) const;

    virtual size_t compute_index_map(std::vector<uint32_t> & map_,
                                     int item_) const;

    /// Destructor constructor
    regular_circular_placement();

    /// Constructor
    regular_circular_placement(const vector_3d & center_,
                               double radius_,
                               double step_angle_,
                               double start_angle_,
                               size_t number_of_items_,
                               rotation_axis_type rotation_axis_ = ROTATION_AXIS_Z);

    /// Destructor
    virtual ~regular_circular_placement();

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
    virtual void tree_dump(std::ostream & out_  = std::clog,
                           const std::string & title_ = "geomutils::regular_circular_placement",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const;

  protected:

    /// Set default:
    void _set_default();

  private:

    vector_3d _center_;          /// Intersection of the rotation axis with the plane
    double    _radius_;
    double    _step_angle_;
    double    _start_angle_;
    size_t    _number_of_items_;
    bool      _follow_rotation_;
    rotation_axis_type _rotation_axis_;

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_REGULAR_CIRCULAR_PLACEMENT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
