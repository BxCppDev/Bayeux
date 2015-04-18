/// \file geomtools/i_stackable.h
/* Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-02
 * Last modified: 2010-04-02
 *
 * License:
 *
 * Description:
 *
 *   Abstract interface for stackable shape.
 *
 * History:
 *
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

namespace geomtools {

  /// \brief Abstract base stackable object
  class i_stackable
  {

  public:

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
  class stackable_data : public i_stackable,
                         public datatools::i_tree_dumpable
  {
  public:

    /// Check the validity of stacking information along the X axis
    bool is_valid_x() const;

    /// Check the validity of stacking information along the Y axis
    bool is_valid_y() const;

    /// Check the validity of stacking information along the Z axis
    bool is_valid_z() const;

    /// Check the validity of stacking information
    bool is_valid() const;

    /// Check the validity of stacking information
    bool is_valid_weak() const;

    /// Invalidate
    void invalidate();

    /// Constructor
    stackable_data();

    /// Destructor
    virtual ~stackable_data();

    /// Return the minimum X
    virtual double get_xmin() const {return xmin;}

    /// Return the maximum X
    virtual double get_xmax() const {return xmax;}

    /// Return the minimum Y
    virtual double get_ymin() const {return ymin;}

    /// Return the maximum Y
    virtual double get_ymax() const {return ymax;}

    /// Return the minimum Z
    virtual double get_zmin() const {return zmin;}

    /// Return the maximum Z
    virtual double get_zmax() const {return zmax;}

    /// Smart print
    virtual void tree_dump(std::ostream & out_         = std::clog,
                           const std::string & title_  = "",
                           const std::string & indent_ = "",
                           bool inherit_          = false) const;

    /// Print
    void dump(std::ostream & out_ = std::clog) const;

    /// Initialization
    bool initialize(const datatools::properties & config_);

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

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
