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

#ifndef __geomtools__smart_id_locator_h
#define __geomtools__smart_id_locator_h 1

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <list>

#include <boost/cstdint.hpp>

#include <datatools/utils/ioutils.h>
#include <datatools/utils/properties.h>

#include <geomtools/i_locator.h>
#include <geomtools/geom_map.h>
#include <geomtools/id_selector.h>

namespace geomtools {

  using namespace std;

  class smart_id_locator : public i_locator
  {
  public:
    enum mode_t
      {
	MODE_INVALID = 0,
	MODE_LAST    = 1, 
	MODE_DEFAULT = MODE_LAST
      };


  private:
    bool              __debug;
    bool              __initialized;
    int               __mode;
    uint32_t          __type;
    id_selector       __idsel;
    const geom_map *  __gmap;

    //! Optimization data
    list<const geom_info *> __ginfos;
    const geom_info * __last_found;

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
		      const string & rules_,
		      int mode_ = MODE_DEFAULT);

    virtual ~smart_id_locator ();

    void initialize (uint32_t type_, int mode_ = MODE_DEFAULT);

    void initialize (const string & selection_rules_, int mode_ = MODE_DEFAULT);

    void reset ();

    const list<const geom_info *> & get_ginfos () const;

  protected:

    void _construct ();

  public:
    // check if 'id_' is valid:
    virtual bool validate_id (const geom_id & id_) const;

    virtual const geom_info & get_geom_info (const geom_id & id_) const;

    virtual const geom_id & get_geom_id (const vector_3d & world_position_, 
					 int type_, 
					 double tolerance_ = i_object_3d::USING_PROPER_TOLERANCE) const;


    void dump (ostream & out_ = clog) const;

  };

} // end of namespace geomtools

#endif // __geomtools__smart_id_locator_h

// end of smart_id_locator.h
