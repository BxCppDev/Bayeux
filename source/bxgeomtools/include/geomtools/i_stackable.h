/// \file geomtools/i_stackable.h
/* Author(s) :    Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-02
 * Last modified: 2017-02-22
 *
 * Description:
 *
 *   Abstract interface for stackable shape objects
 *   and stackable data.
 */

#ifndef GEOMTOOLS_I_STACKABLE_H
#define GEOMTOOLS_I_STACKABLE_H 1

// Standard library:
#include <iostream>
#include <string>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/properties.h>
#include <datatools/i_tree_dump.h>
#include <datatools/bit_mask.h>

// This project:
#include <geomtools/utils.h>

namespace geomtools {

  /// \brief Abstract base stackable object
  class i_stackable
  {

  public:

    i_stackable();
    virtual ~i_stackable();
    bool has_xmin() const;
    bool has_xmax() const;
    bool has_ymin() const;
    bool has_ymax() const;
    bool has_zmin() const;
    bool has_zmax() const;
    virtual double get_xmin() const = 0;
    virtual double get_xmax() const = 0;
    virtual double get_ymin() const = 0;
    virtual double get_ymax() const = 0;
    virtual double get_zmin() const = 0;
    virtual double get_zmax() const = 0;

  };

  /// \brief Static methods and constants to handle stackable data/information
  class stackable
  {
  public:
    static const std::string STACKABLE_PREFIX;
    static const std::string STACKABLE_LENGTH_UNIT_PROPERTY;
    static const std::string STACKABLE_XMIN_PROPERTY;
    static const std::string STACKABLE_XMAX_PROPERTY;
    static const std::string STACKABLE_YMIN_PROPERTY;
    static const std::string STACKABLE_YMAX_PROPERTY;
    static const std::string STACKABLE_ZMIN_PROPERTY;
    static const std::string STACKABLE_ZMAX_PROPERTY;
    static const std::string STACKABLE_PLAY_PROPERTY;
    static const std::string STACKABLE_LIMITS_PROPERTY;

    enum stackability_mode {
      STACKABILITY_NONE      = 0,                           // 00000000
      STACKABILITY_XMIN      = datatools::bit_mask::bit00,  // 00000001
      STACKABILITY_XMAX      = datatools::bit_mask::bit01,  // 00000010
      STACKABILITY_X         = STACKABILITY_XMIN | STACKABILITY_XMAX,     // 00000011
      STACKABILITY_YMIN      = datatools::bit_mask::bit02,  // 00000100
      STACKABILITY_YMAX      = datatools::bit_mask::bit03,  // 00001000
      STACKABILITY_Y         = STACKABILITY_YMIN | STACKABILITY_YMAX,     // 00001100
      STACKABILITY_ZMIN      = datatools::bit_mask::bit04,  // 00010000
      STACKABILITY_ZMAX      = datatools::bit_mask::bit05,  // 00100000
      STACKABILITY_Z         = STACKABILITY_ZMIN | STACKABILITY_ZMAX,     // 00110000
      STACKABILITY_XY        = STACKABILITY_X | STACKABILITY_Y,           // 00001111
      STACKABILITY_XZ        = STACKABILITY_X | STACKABILITY_Z,           // 00110011
      STACKABILITY_YZ        = STACKABILITY_Y | STACKABILITY_Z,           // 00111100
      STACKABILITY_VERY_WEAK = datatools::bit_mask::bit06,  // 01000000
      STACKABILITY_WEAK      = datatools::bit_mask::bit07,  // 10000000
      STACKABILITY_STRONG    = STACKABILITY_X | STACKABILITY_Y | STACKABILITY_Z  // 00111111
    };

    static std::string make_key(const std::string & flag_);

    static void extract(const datatools::properties & source_,
                        datatools::properties & target_);

    static bool has_xmin(const datatools::properties & source_);

    static bool has_xmax(const datatools::properties & source_);

    static bool has_ymin(const datatools::properties & source_);

    static bool has_ymax(const datatools::properties & source_);

    static bool has_zmin(const datatools::properties & source_);

    static bool has_zmax(const datatools::properties & source_);

    static double get_xmin(const datatools::properties & source_,
                           double length_unit_ = -1.0);

    static double get_xmax(const datatools::properties & source_,
                           double length_unit_ = -1.0);

    static double get_ymin(const datatools::properties & source_,
                           double length_unit_ = -1.0);

    static double get_ymax(const datatools::properties & source_,
                           double length_unit_ = -1.0);

    static double get_zmin(const datatools::properties & source_,
                           double length_unit_ = -1.0);

    static double get_zmax(const datatools::properties & source_,
                           double length_unit_ = -1.0);

    static void set_xmin(datatools::properties & target_,
                         double xmin_);

    static void set_xmax(datatools::properties & target_,
                         double xmax_);

    static void set_ymin(datatools::properties & target_,
                         double ymin_);

    static void set_ymax(datatools::properties & target_,
                         double ymax_);

    static void set_zmin(datatools::properties & target_,
                         double zmin_);

    static void set_zmax(datatools::properties & target_,
                         double zmax_);

    static void unset_xmin(datatools::properties & target_);

    static void unset_xmax(datatools::properties & target_);

    static void unset_ymin(datatools::properties & target_);

    static void unset_ymax(datatools::properties & target_);

    static void unset_zmin(datatools::properties & target__);

    static void unset_zmax(datatools::properties & target_);

    static void unset(datatools::properties & target_);

    static void set(datatools::properties & target_,
                    const i_stackable & sd_);

  };

  /// \brief Data for stacking along X, Y and/or Z axis
  class stackable_data
    : public i_stackable
    , public datatools::i_tree_dumpable
  {
  public:

    /// Constructor
    stackable_data();

    /// Destructor
    ~stackable_data() override;

    bool check(const stackable::stackability_mode flags_ = stackable::STACKABILITY_STRONG) const;

    /// Check the validity of stacking information along the X axis
    bool is_valid_x() const;

    /// Check the validity of stacking information along the Y axis
    bool is_valid_y() const;

    /// Check the validity of stacking information along the Z axis
    bool is_valid_z() const;

    /// Check the validity of stacking information for all axis
    bool is_valid() const;

    /// Check the validity of stacking information for one given axis
    bool is_valid_by_axis(axis_type) const;

    /// Check the validity of stacking information at least for one axis
    bool is_valid_weak() const;

    /// Check if at least one stacking information is set (X/Y/Z, min/max)
    bool is_valid_very_weak() const;

    /// Invalidate
    void invalidate();

    /// Return the minimum X
    double get_xmin() const override;

    /// Return the maximum X
    double get_xmax() const override;

    /// Return the minimum Y
    double get_ymin() const override;

    /// Return the maximum Y
    double get_ymax() const override;

    /// Return the minimum Z
    double get_zmin() const override;

    /// Return the maximum Z
    double get_zmax() const override;

    /// Smart print
    void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_          = false) const override;

    /// Print
    void dump(std::ostream & out_ = std::clog) const;

    /// Initialization
    bool initialize(const datatools::properties & config_);

    /// Reset
    void reset();

  public:

    double xmin; //!< Minimum X
    double xmax; //!< Maximum X
    double ymin; //!< Minimum Y
    double ymax; //!< Maximum Y
    double zmin; //!< Minimum Z
    double zmax; //!< Maximum Z

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_I_STACKABLE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
