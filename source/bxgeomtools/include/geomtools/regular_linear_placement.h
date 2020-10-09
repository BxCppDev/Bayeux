/// \file geomtools/regular_linear_placement.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-26
 * Last modified: 2010-03-31
 *
 * License:
 *
 * Description:
 *
 *  Regular linear placement.
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_REGULAR_LINEAR_PLACEMENT_H
#define GEOMTOOLS_REGULAR_LINEAR_PLACEMENT_H 1

// Standard library:
#include <iostream>
#include <iomanip>
#include <string>

// This project:
#include <geomtools/i_placement.h>
#include <geomtools/placement.h>

namespace geomtools {


  /// \brief Regular linear placement
  class regular_linear_placement : public i_placement
  {
  public:

    /// \brief Type of the replication axis
    enum axis_t
      {
        REPLICANT_AXIS_NONE = -1,
        REPLICANT_AXIS_X = 0,
        REPLICANT_AXIS_Y = 1,
        REPLICANT_AXIS_Z = 2
      };

  public:

    size_t get_dimension() const override;

    bool is_replica() const override;

    bool has_only_one_rotation() const override;

    void set_replicant_axis(int);
    int  get_replicant_axis() const;
    bool is_replicant_x_axis() const;
    bool is_replicant_y_axis() const;
    bool is_replicant_z_axis() const;

    bool is_valid() const;

    void invalidate();

    void set_replicant_step_x(double x_);

    void set_replicant_step_y(double y_);

    void set_replicant_step_z(double z_);

    void set_step(double x_, double y_, double z_);

    void set_step(const vector_3d & step_);

    const vector_3d & get_step() const;

    void set_basic_placement(const placement & bp_);

    const placement & get_basic_placement() const;

    placement & get_basic_placement();

    void set_number_of_items(size_t n_);

    size_t get_number_of_items() const override;

    void get_placement(int item_, placement & p_) const override;

    size_t compute_index_map(std::vector<uint32_t> & map_,
                                     int item_) const override;

    /// Default constructor
    regular_linear_placement();

    /// Constructor
    regular_linear_placement(const placement & basic_placement_,
                             const vector_3d & step_,
                             size_t number_of_items_);

    /// Constructor
    regular_linear_placement(const placement & basic_placement_,
                             double step_,
                             size_t number_of_items_,
                             int replicant_axis_ = REPLICANT_AXIS_NONE);

    /// Destructor
    ~regular_linear_placement() override;

    /// Initialization
    void init(const placement & basic_placement_,
              const vector_3d & step_,
              size_t number_of_items_);

    /// Initialization
    void init(const placement & basic_placement_,
              double step_,
              size_t number_of_items_,
              int replicant_axis_ = REPLICANT_AXIS_X);

    /// Reset
    virtual void reset();

    /// Smart print
    void tree_dump(std::ostream & out_  = std::clog,
                           const std::string & title_ = "",
                           const std::string & indent_ = "",
                           bool inherit_ = false) const override;

  private:

    placement _basic_placement_; //!< Base reference placement
    vector_3d _step_;            //!< Step
    size_t    _number_of_items_; //!< Number of placements
    int       _replicant_axis_;  //!< Replcation axis

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_REGULAR_LINEAR_PLACEMENT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
