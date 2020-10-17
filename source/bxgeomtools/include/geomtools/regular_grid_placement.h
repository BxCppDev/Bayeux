/// \file geomtools/regular_grid_placement.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-26
 * Last modified: 2010-02-26
 *
 * License:
 *
 * Description:
 *  XY grid 2D placement
 *
 *   y^
 *    |   column  0      1      2      3      4
 *    |           :      :      :      :      :     row
 *    |           x......x......x......x......x......2
 *    |           :10    :11    :12    :13    :14
 * yc +.......... x......x......c......x......x......1
 *    |      dy   :5     :6     :7     :8     :9
 * yo +...........o......x......x......x......x......0
 *    |           :0 dx   1     :2      3      4
 *    |           :             :
 *    +-----------+-------------+--------------------> x
 *   O           xo             xc
 *
 *
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_REGULAR_GRID_PLACEMENT_H_
#define GEOMTOOLS_REGULAR_GRID_PLACEMENT_H_ 1

// Standard library:
#include <iostream>
#include <iomanip>
#include <string>

// This project:
#include <geomtools/i_placement.h>
#include <geomtools/placement.h>

namespace geomtools {

  class regular_grid_placement
    : public i_placement
  {
  public:

    //! \brief Orientation of the grid placement
    enum mode_type {
      MODE_XY = 0,
      MODE_YZ = 1,
      MODE_XZ = 2
    };

    static const bool CENTERED = true;
    static const bool UNCENTERED = false;

  public:

    bool is_valid () const;

    void invalidate ();

    int get_mode () const;

    bool is_mode_xy () const;

    bool is_mode_yz () const;

    bool is_mode_xz () const;

    void set_mode (int m_);

    bool is_centered () const;

    void set_centered (bool c_);

    void set_column_step (double dx_);

    void set_row_step (double dx_);

    void set_steps (double dx_, double dy_);

    double get_column_step () const;

    double get_row_step () const;

    double get_x_step () const;

    double get_y_step () const;

    double get_z_step () const;

    void set_basic_placement (const placement & bp_);

    const placement & get_basic_placement () const;

    /// \deprecated Please use grab_basic_placement
    placement & get_basic_placement ();

    placement & grab_basic_placement ();

    void set_number_of_rows (size_t nr_);

    void set_number_of_columns (size_t nc_);

    size_t get_number_of_rows () const;

    size_t get_number_of_columns () const;

    void get_placement (int col_, int row_, placement & p_) const;

    placement get_placement (int col_, int row_) const;

    bool has_only_one_rotation () const override;

    size_t get_dimension () const override;

    bool is_replica () const override;

    size_t get_number_of_items () const override;

    void get_placement (int item_, placement & p_) const override;

    size_t compute_index_map (std::vector<uint32_t> & map_,
                                      int item_) const override;

    regular_grid_placement ();

    regular_grid_placement (const placement & basic_placement_,
                            double column_step_,
                            double row_step_,
                            size_t number_of_columns_,
                            size_t number_of_rows_,
                            int mode_,
                            bool centered_ = true);

    ~regular_grid_placement () override;

    void init (const placement & basic_placement_,
               double column_step_,
               double row_step_,
               size_t number_of_columns_,
               size_t number_of_rows_,
               int mode_,
               bool centered_ = true);

    virtual void reset ();

    void tree_dump (std::ostream & out_  = std::clog,
                            const std::string & title_ = "geomutils::regular_grid_placement",
                            const std::string & indent_ = "",
                            bool inherit_ = false) const override;

  private:

    placement _basic_placement_;
    double    _column_step_;       ///< y
    double    _row_step_;          ///< x
    size_t    _number_of_columns_;
    size_t    _number_of_rows_;
    int       _mode_;
    bool      _centered_;

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_REGULAR_GRID_PLACEMENT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
