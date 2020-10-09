/// \file geomtools/smart_id_locator.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-05-30
 * Last modified: 2010-05-30
 *
 * License:
 *
 * Description:
 *   A smart geometry locator based on a geom_map and optimized for fast search
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_SMART_ID_LOCATOR_H
#define GEOMTOOLS_SMART_ID_LOCATOR_H 1

// Standard library:
#include <iostream>
#include <string>
#include <list>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>

// This project:
#include <geomtools/i_locator.h>
#include <geomtools/id_selector.h>

namespace geomtools {

  class geom_map;

  /// \brief Locator of geometry volumes by thier geometry Id
  class smart_id_locator : public i_locator
  {
  public:

    /// \brief Mode
    enum mode_type {
      MODE_INVALID = 0,
      MODE_LAST    = 1,
      MODE_DEFAULT = MODE_LAST
    };

    /// Get logging priority threshold
    datatools::logger::priority get_logging_priority () const;

    void set_logging_priority (datatools::logger::priority);

    /// Check Initialization flag
    bool is_initialized () const;

    /// Set the reference geometry map
    void set_gmap (const geom_map & gmap_);

    /// Constructor
    smart_id_locator ();

    /// Constructor
    smart_id_locator (const geom_map & gmap_);

    /// Constructor
    smart_id_locator (const geom_map & gmap_,
                      uint32_t type_,
                      int mode_ = MODE_DEFAULT);

    /// Constructor
    smart_id_locator (const geom_map & gmap_,
                      const std::string & rules_,
                      int mode_ = MODE_DEFAULT);

    /// Destructor
    virtual ~smart_id_locator ();

    /// Initialize the locator from the geometry type
    void initialize (uint32_t type_, int mode_ = MODE_DEFAULT);

    /// Initialize the locator from a selection rule
    void initialize (const std::string & selection_rules_, int mode_ = MODE_DEFAULT);

    /// Reset the locator
    void reset ();

    /// Get the list of geometry info object handles
    const std::list<const geom_info *> & get_ginfos () const;

    /// Check if a given geom Id is valid
    bool validate_id (const geom_id & id_) const override;

    /// Retrieve the geometry info object from its geometry Id
    const geom_info & get_geom_info (const geom_id & id_) const override;

    /// Retrieve the geometry Id of the volume from the world position
    const geom_id & get_geom_id (const vector_3d & world_position_,
                                         int type_ = geom_id::INVALID_TYPE,
                                         double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const override;

    /// Smart print
    void tree_dump(std::ostream & out_ = std::clog,
                   const std::string & title_  = "",
                   const std::string & indent_ = "",
                   bool inherit_               = false) const;

    /// Dump
    void dump(std::ostream & out_ = std::clog) const;

    /// Print
    void print_infos(std::ostream & out_) const;

  protected:

    /// Construction
    void _construct ();

  private:

    datatools::logger::priority _logging_priority_; //!< Logging priority threshold
    bool              _initialized_; //!< Initialization flag
    int               _mode_;        //!< Running mode
    uint32_t          _type_;        //!< Geometry type associated to a given category
    id_selector       _idsel_;       //!< Geometry ID selector
    const geom_map *  _gmap_;        //!< Geometry map handle

    // Optimization data
    std::list<const geom_info *> _ginfos_; //!< List of handles to precomputed geometry informations
    const geom_info * _last_found_;        //!< Handle to the last found object

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_SMART_ID_LOCATOR_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
