// -*- mode: c++ ; -*- 
/* geom_info.cc
 */

#include <geomtools/geom_info.h>

namespace geomtools {


  const datatools::utils::properties & 
  geom_info::get_properties () const
  {
    return __properties;
  }

  datatools::utils::properties & 
  geom_info::get_properties ()
  {
    return __properties;
  }
  
  const placement & geom_info::get_world_placement () const
  {
    return __world_placement;
  }
  
  const logical_volume & geom_info::get_logical () const
  {
    return *__logical;
  }
  
  const geom_id & geom_info::get_id () const
  {
    return __id;
  }
    
  geom_info::geom_info (const geom_id & id_, 
				  const placement & wp_,
				  const logical_volume & l_)
  {
    __id = id_;
    __world_placement = wp_;
    __logical = &l_;
  }


} // end of namespace geomtools

// end of geom_info.cc
