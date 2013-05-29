// -*- mode: c++ ; -*-
/* smart_id_locator.h
 * Author (s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-05-30
 * Last modified: 2010-05-30
 *
 * License:
 *
 * Description:
 *   A smart geometry locator based on a geom_map and optimized
 *     for fast search
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_SMART_ID_LOCATOR_H_
#define GEOMTOOLS_SMART_ID_LOCATOR_H_ 1

#include <iostream>
#include <string>
#include <list>

#include <boost/cstdint.hpp>

#include <geomtools/i_locator.h>
#include <geomtools/id_selector.h>

namespace geomtools {

  class geom_map;

  class smart_id_locator : public i_locator
  {
  public:

    enum mode_t
      {
        MODE_INVALID = 0,
        MODE_LAST    = 1,
        MODE_DEFAULT = MODE_LAST
      };

  public:

    void set_debug (bool);

    bool is_debug () const;

    bool is_initialized () const;

    void set_gmap (const geom_map & gmap_);

    smart_id_locator ();

    smart_id_locator (const geom_map & gmap_);

    smart_id_locator (const geom_map & gmap_,
                      uint32_t type_,
                      int mode_ = MODE_DEFAULT);

    smart_id_locator (const geom_map & gmap_,
                      const std::string & rules_,
                      int mode_ = MODE_DEFAULT);

    virtual ~smart_id_locator ();

    void initialize (uint32_t type_, int mode_ = MODE_DEFAULT);

    void initialize (const std::string & selection_rules_, int mode_ = MODE_DEFAULT);

    void reset ();

    const std::list<const geom_info *> & get_ginfos () const;

  protected:

    void _construct ();

  public:
    // check if 'id_' is valid:
    virtual bool validate_id (const geom_id & id_) const;

    virtual const geom_info & get_geom_info (const geom_id & id_) const;

    virtual const geom_id & get_geom_id (const vector_3d & world_position_,
                                         int type_ = geom_id::INVALID_TYPE,
                                         double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;


    void dump (std::ostream & out_ = std::clog) const;

  private:

    bool              _debug_; /// Debug flag
    bool              _initialized_; /// Initialization flag
    int               _mode_; /// Running mode
    uint32_t          _type_; /// Geometry type associated to a given category
    id_selector       _idsel_; /// Geometry ID selector
    const geom_map *  _gmap_; /// Geometry map handle

    // Optimization data
    std::list<const geom_info *> _ginfos_; /// List of precomputed geometry informations
    const geom_info * _last_found_; /// Handle to the last found object

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_SMART_ID_LOCATOR_H_

// end of smart_id_locator.h
