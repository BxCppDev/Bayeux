// -*- mode: c++ ; -*- 
/* geom_info.h
 * Author (s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-30
 * Last modified: 2010-04-30
 * 
 * License: 
 * 
 * Description: 
 *   Geometry information about an object from a geomertry setup.
 *
 * History: 
 * 
 */

#ifndef __geomtools__geom_info_h
#define __geomtools__geom_info_h 1

#include <datatools/utils/properties.h>

#include <geomtools/geom_id.h>
#include <geomtools/placement.h>
#include <geomtools/logical_volume.h>


namespace geomtools {

  using namespace std;

    class geom_info
    {
    private:

      datatools::utils::properties __properties;
      geom_id                      __id;
      placement                    __world_placement;
      const logical_volume *       __logical;

    public:

      geom_info (const geom_id &, 
		 const placement &,
		 const logical_volume &);

      const placement & get_world_placement () const;

      const logical_volume & get_logical () const;

      const geom_id & get_id () const;

      const datatools::utils::properties & get_properties () const;

      datatools::utils::properties & get_properties ();

  };

} // end of namespace geomtools

#endif // __geomtools__geom_info_h

// end of geom_info.h
