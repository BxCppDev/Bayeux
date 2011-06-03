// -*- mode: c++ ; -*- 
/* geom_info.cc
 */

#include <geomtools/geom_info.h>

#include <geomtools/logical_volume.h>

namespace geomtools {

  using namespace std;

  const datatools::utils::properties & 
  geom_info::get_properties () const
  {
    return properties_;
  }

  datatools::utils::properties & 
  geom_info::get_properties ()
  {
    return properties_;
  }
  
  const placement & geom_info::get_world_placement () const
  {
    return world_placement_;
  }
  
  const logical_volume & geom_info::get_logical () const
  {
    return *logical_;
  }
  
  const geom_id & geom_info::get_id () const
  {
    return gid_;
  }
 
  const geom_id & geom_info::get_gid () const
  {
    return gid_;
  }
 
  const geom_id & geom_info::get_geom_id () const
  {
    return gid_;
  }
  
  bool geom_info::has_logical () const
  {
    return logical_ != 0;
  }
     
  geom_info::geom_info ()
  {
    gid_.invalidate ();
    world_placement_.set (0., 0., 0., 0., 0., 0.);
    logical_ = 0;
    return;
  }
   
  geom_info::geom_info (const geom_id & a_id)
  {
    gid_ = a_id;
    world_placement_.set (0., 0., 0., 0., 0., 0.);
    logical_ = 0;
    return;
  }
    
  geom_info::geom_info (const geom_id & a_id, 
			const placement & a_world_placement,
			const logical_volume & a_logical_volume)
  {
    gid_ = a_id;
    world_placement_ = a_world_placement;
    logical_ = &a_logical_volume;
    return;
  }


  ostream & operator<< (ostream & a_out, const geom_info & a_geom_info)
  {
    a_out << "<" << a_geom_info.gid_ << " @ "
	 << "{" << a_geom_info.world_placement_ << "}"
	 << " : " << a_geom_info.logical_ << " >";
    return a_out;
  }

} // end of namespace geomtools

// end of geom_info.cc
