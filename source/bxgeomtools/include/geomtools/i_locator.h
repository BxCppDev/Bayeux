// -*- mode: c++ ; -*- 
/* i_locator.h
 * Author (s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-02-09
 * Last modified: 2010-02-09
 * 
 * License: 
 * 
 * Description: 
 *   Geometry locator abstract interface
 * 
 * History: 
 * 
 */

#ifndef __geomtools__i_locator_h
#define __geomtools__i_locator_h 1

#include <datatools/utils/properties.h>

#include <geomtools/utils.h>
#include <geomtools/geom_id.h>
#include <geomtools/geom_info.h>

namespace geomtools {

  using namespace std;
  using namespace datatools::utils;

  class i_locator 
  {

  public:

    // the following pure abstract methods should be implemented!

    // check if 'id_' is valid:
    virtual bool validate_id (const geom_id & id_) const = 0;

    virtual const geom_info & get_geom_info (const geom_id & id_) const = 0;

    virtual const geom_id & get_geom_id (const vector_3d & world_position_, 
					 int type_, 
					 double tolerance_ = i_object_3d::USING_PROPER_TOLERANCE) const = 0;

  };

} // end of namespace geomtools

#endif // __geomtools__i_locator_h

// end of i_locator.h
