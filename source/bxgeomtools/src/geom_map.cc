// -*- mode: c++ ; -*- 
/* geom_map.cc
 */

#include <geomtools/geom_map.h>

namespace geomtools {

  const geom_info_dict_t & geom_map::_get_geom_infos () const
  {
    return __geom_infos;
  }
    
  const geom_info_dict_t & geom_map::get_geom_infos () const
  {
    return __geom_infos;
  }
  
  geom_info_dict_t & geom_map::_get_geom_infos ()
  {
    return __geom_infos;
  }
    
  const id_mgr & geom_map::get_id_manager () const
  {
    if (__id_manager == 0)
      {
	ostringstream message;
	message << "geom_map::get__manager: "
		<< "No manager !";
	throw runtime_error (message.str ());
      }
    return *__id_manager;
  }
     
  const id_mgr & geom_map::_get_id_manager () const
  {
    if (__id_manager == 0)
      {
	ostringstream message;
	message << "geom_map::_get__manager: "
		<< "No manager !";
	throw runtime_error (message.str ());
      }
    return *__id_manager;
  }
 
  geom_map::geom_map ()
  {
    __invalid_geom_id.invalidate ();
    __id_manager = 0;
  }

  geom_map::~geom_map ()
  {
    __geom_infos.clear ();
  }
  
  bool geom_map::has_id_manager () const
  {
    return __id_manager != 0;
  }

  void geom_map::set_id_manager (const id_mgr & mgr_)
  {
    __id_manager = &mgr_;
  }

  bool geom_map::validate_id (const geom_id & id_) const
  {
    return has_geom_info (id_);
  }

  bool geom_map::has_geom_info (const geom_id & id_) const
  {
    return __geom_infos.find (id_) != __geom_infos.end ();
  }
  
  const geom_info & geom_map::get_geom_info (const geom_id & id_) const
  {
    geom_info_dict_t::const_iterator found = __geom_infos.find (id_);
    if (found == __geom_infos.end ())
      {
	ostringstream message;
	message << "geom_map::get_geom_info: "
		<< "No '" << id_ << "' ID !";
	throw runtime_error (message.str ());
      }
    return found->second;
  }

  const geom_id & geom_map::get_invalid_geom_id () const
  {
    return __invalid_geom_id;
  }
  
  void geom_map::get_geom_id (const vector_3d & world_position_, 
			      int type_,
			      geom_id & gid_,
			      double tolerance_) const
  {
    gid_.invalidate ();
    gid_ = geom_map::get_geom_id (world_position_, type_, tolerance_);
  };
 
  void geom_map::get_geom_id (const vector_3d & world_position_, 
			      const string & category_,
			      geom_id & gid_,
			      double tolerance_) const
  {
    gid_.invalidate ();
    gid_ = geom_map::get_geom_id (world_position_, category_, tolerance_);
  };
 
  bool geom_map::check_inside (const geom_info & ginfo_,
			       const vector_3d & world_position_,
			       double tolerance_)
  {
    const geom_info & ginfo = ginfo_;
    vector_3d local_position;
    const placement & pl = ginfo.get_world_placement ();
    pl.mother_to_child (world_position_, local_position);
    const logical_volume & log = ginfo.get_logical ();
    const i_shape_3d & shape = log.get_shape ();
    if (shape.is_inside (local_position, tolerance_))
      {
	return true;
      }
    return false;
  }

  const geom_id & geom_map::get_geom_id (const vector_3d & world_position_, 
					 int type_, 
					 double tolerance_) const
  {
    int requested_type = type_;
    for (geom_info_dict_t::const_iterator i = __geom_infos.begin ();
	 i != __geom_infos.end ();
	 i++)
      {
	const geom_info & ginfo = i->second;
	const geom_id & gid = ginfo.get_id ();
	if (gid.get_type () != requested_type) continue;

	if (geom_map::check_inside (ginfo, world_position_, tolerance_))
	  {
	    return gid;
	  }

	/*
	vector_3d local_position;
	const placement & pl = ginfo.get_world_placement ();
	pl.mother_to_child (world_position_, local_position);
	const logical_volume & log = ginfo.get_logical ();
	const i_shape_3d & shape = log.get_shape ();
	if (shape.is_inside (local_position, tolerance_))
	  {
	    return gid;
	  }
	*/
      }
     return __invalid_geom_id;
  }
   
  const geom_id & geom_map::get_geom_id (const vector_3d & world_position_, 
					 const string & category_, 
					 double tolerance_) const
  {
    geom_info_dict_t::const_iterator found = __geom_infos.end ();
    if (! __id_manager->has_category_info (category_))
      {
	ostringstream message;
	message << "geom_map::get_geom_id: "
		<< "No category named '" << category_ << "' !";
	throw runtime_error (message.str ());
      }
    const id_mgr::category_info & ci 
      = __id_manager->get_category_info (category_);
    int requested_type = ci.get_type ();
    return get_geom_id (world_position_, requested_type, tolerance_);
  }

  void geom_map::build_from (const model_factory & factory_,
			     const string & mother_)
  {
    throw runtime_error ("geom_map::build_from: Not implemented !");
  }

} // end of namespace geomtools

// end of geom_map.cc
