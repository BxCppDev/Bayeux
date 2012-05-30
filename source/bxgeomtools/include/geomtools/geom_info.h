// -*- mode: c++ ; -*- 
/* geom_info.h
 * Author (s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-30
 * Last modified: 2010-04-30
 * 
 * License: 
 * 
 * Description: 
 *
 *   Geometry information about an object from a geomertry setup.
 *
 * History: 
 * 
 */

#ifndef __geomtools__geom_info_h
#define __geomtools__geom_info_h 1

#include <iostream>

#include <datatools/utils/properties.h>
#include <datatools/utils/i_predicate.h>
#include <geomtools/geom_id.h>
#include <geomtools/placement.h>
#include <geomtools/logical_volume.h>

namespace geomtools {

  class logical_volume;

  class geom_info
  {

  public:

    geom_info ();

    geom_info (const geom_id &);

    geom_info (const geom_id &, 
               const placement &,
               const logical_volume &);

    bool has_logical () const;

    const placement & get_world_placement () const;

    const logical_volume & get_logical () const;

    const geom_id & get_id () const;

    const geom_id & get_gid () const;

    const geom_id & get_geom_id () const;

    const datatools::utils::properties & get_properties () const;

    datatools::utils::properties & get_properties ();

    friend std::ostream & operator<< (std::ostream &, const geom_info &);

  private:

    datatools::utils::properties _properties_;
    geom_id                      _gid_;
    placement                    _world_placement_;
    const logical_volume *       _logical_;

  };

  struct has_geom_type_predicate : public datatools::utils::i_predicate<geom_info>
  {
    uint32_t _geom_type_;
    has_geom_type_predicate (uint32_t geom_type_) : _geom_type_ (geom_type_)
    {
      return;
    }
    bool operator () (const geom_info & ginfo_) const
    {
      return (ginfo_.get_geom_id ().get_type () == _geom_type_);
    }
  };

  typedef std::map<geom_id, geom_info> geom_info_dict_t;

} // end of namespace geomtools

#endif // __geomtools__geom_info_h

// end of geom_info.h
