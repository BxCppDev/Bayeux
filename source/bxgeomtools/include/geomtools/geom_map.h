// -*- mode: c++ ; -*- 
/* geom_map.h
 * Author (s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-04-30
 * Last modified: 2010-04-30
 * 
 * License: 
 * 
 * Description: 
 *   Geometry mapping of geom_id's
 * 
 * A geometry map:
 *
 * History: 
 * 
 */

#ifndef __geomtools__geom_map_h
#define __geomtools__geom_map_h 1

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <algorithm>

#include <boost/cstdint.hpp>

#include <datatools/utils/properties.h>

#include <geomtools/utils.h>
#include <geomtools/id_mgr.h>
#include <geomtools/placement.h>
#include <geomtools/model_factory.h>
#include <geomtools/geom_info.h>

namespace geomtools {

  using namespace std;

  class geom_map 
  {
  public:

    typedef map<geom_id, geom_info> geom_info_dict_t;

  private:  
    geom_id          __invalid_geom_id;
    const id_mgr *   __id_manager;
    geom_info_dict_t __geom_infos;
    
  protected:
    
    const id_mgr & _get_id_manager () const;
    
    const geom_info_dict_t & _get_geom_infos () const;

  public:

    geom_map ();

    virtual ~geom_map ();  

    bool has_id_manager () const;

    void set_id_manager (const id_mgr & mgr_);

    bool has_geom_info (const geom_id &) const;

    const geom_info & get_geom_info (const geom_id &) const;

    void get_geom_id (const vector_3d & world_position_, 
		      const string & category_,
		      geom_id & gid_,
		      double tolerance_ = i_object_3d::USING_PROPER_TOLERANCE) const;

    const geom_id & get_invalid_geom_id () const;    

    virtual const geom_id & get_geom_id (const vector_3d & world_position_, 
					 const string &  category_, 
					 double tolerance_ = i_object_3d::USING_PROPER_TOLERANCE) const;
    
    virtual void build_from (const model_factory &,
			     const string & mother_ = "world");

  };

} // end of namespace geomtools

#endif // __geomtools__geom_map_h

// end of geom_map.h
