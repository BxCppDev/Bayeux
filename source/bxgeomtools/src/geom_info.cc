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
    return _properties_;
  }

  datatools::utils::properties & 
  geom_info::get_properties ()
  {
    return _properties_;
  }
  
  const placement & geom_info::get_world_placement () const
  {
    return _world_placement_;
  }
  
  const logical_volume & geom_info::get_logical () const
  {
    return *_logical_;
  }
  
  const geom_id & geom_info::get_id () const
  {
    return _gid_;
  }
 
  const geom_id & geom_info::get_gid () const
  {
    return _gid_;
  }
 
  const geom_id & geom_info::get_geom_id () const
  {
    return _gid_;
  }
  
  bool geom_info::has_logical () const
  {
    return _logical_ != 0;
  }
     
  geom_info::geom_info ()
  {
    _gid_.invalidate ();
    _world_placement_.set (0., 0., 0., 0., 0., 0.);
    _logical_ = 0;
    return;
  }
   
  geom_info::geom_info (const geom_id & a_id)
  {
    _gid_ = a_id;
    _world_placement_.set (0., 0., 0., 0., 0., 0.);
    _logical_ = 0;
    return;
  }
    
  geom_info::geom_info (const geom_id & a_id, 
                        const placement & a_world_placement,
                        const logical_volume & a_logical_volume)
  {
    _gid_ = a_id;
    _world_placement_ = a_world_placement;
    _logical_ = &a_logical_volume;
    return;
  }


  ostream & operator<< (ostream & a_out, const geom_info & a_geom_info)
  {
    a_out << "<" << a_geom_info._gid_ << " @ "
          << "{" << a_geom_info._world_placement_ << "}"
          << " : " << a_geom_info._logical_ << " >";
    return a_out;
  }

} // end of namespace geomtools

// end of geom_info.cc
