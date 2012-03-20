// -*- mode: c++ ; -*- 
/* geom_map.cc
 */

#include <geomtools/geom_map.h>

#include <geomtools/id_mgr.h>

namespace geomtools {

  using namespace std;  

  const geom_info_dict_t & geom_map::_get_geom_infos () const
  {
    return _geom_infos_;
  }
    
  const geom_info_dict_t & geom_map::get_geom_infos () const
  {
    return _geom_infos_;
  }
  
  geom_info_dict_t & geom_map::_get_geom_infos ()
  {
    return _geom_infos_;
  }
    
  const id_mgr & geom_map::get_id_manager () const
  {
    if (_id_manager_ == 0)
      {
        ostringstream message;
        message << "geomtools::geom_map::get_id_manager: "
                << "No manager !";
        throw logic_error (message.str ());
      }
    return *_id_manager_;
  }
     
  const id_mgr & geom_map::_get_id_manager () const
  {
    if (_id_manager_ == 0)
      {
        ostringstream message;
        message << "geomtools::geom_map::_get_id_manager: "
                << "No manager !";
        throw logic_error (message.str ());
      }
    return *_id_manager_;
  }
 
  geom_map::geom_map ()
  {
    _invalid_geom_id_.invalidate ();
    _id_manager_ = 0;
    return;
  }

  geom_map::~geom_map ()
  {
    _geom_infos_.clear ();
    return;
  }
  
  bool geom_map::has_id_manager () const
  {
    return _id_manager_ != 0;
  }

  void geom_map::set_id_manager (const id_mgr & mgr_)
  {
    _id_manager_ = &mgr_;
    return;
  }

  bool geom_map::validate_id (const geom_id & id_) const
  {
    return has_geom_info (id_);
  }

  bool geom_map::has_geom_info (const geom_id & id_) const
  {
    return _geom_infos_.find (id_) != _geom_infos_.end ();
  }
  
  const geom_info & geom_map::get_geom_info (const geom_id & id_) const
  {
    geom_info_dict_t::const_iterator found = _geom_infos_.find (id_);
    if (found == _geom_infos_.end ())
      {
        ostringstream message;
        message << "geomtools::geom_map::get_geom_info: "
                << "No '" << id_ << "' ID !";
        throw logic_error (message.str ());
      }
    return found->second;
  }

  const geom_id & geom_map::get_invalid_geom_id () const
  {
    return _invalid_geom_id_;
  }
  
  void geom_map::get_geom_id (const vector_3d & world_position_, 
                              int type_,
                              geom_id & gid_,
                              double tolerance_) const
  {
    gid_.invalidate ();
    gid_ = geom_map::get_geom_id (world_position_, type_, tolerance_);
    return;
  }
 
  void geom_map::get_geom_id (const vector_3d & world_position_, 
                              const string & category_,
                              geom_id & gid_,
                              double tolerance_) const
  {
    gid_.invalidate ();
    gid_ = geom_map::get_geom_id (world_position_, category_, tolerance_);
    return;
  }
 
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
    for (geom_info_dict_t::const_iterator i = _geom_infos_.begin ();
         i != _geom_infos_.end ();
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
    return _invalid_geom_id_;
  }
   
  const geom_id & geom_map::get_geom_id (const vector_3d & world_position_, 
                                         const string & category_, 
                                         double tolerance_) const
  {
    geom_info_dict_t::const_iterator found = _geom_infos_.end ();
    if (! _id_manager_->has_category_info (category_))
      {
        ostringstream message;
        message << "geomtools::geom_map::get_geom_id: "
                << "No category named '" << category_ << "' !";
        throw logic_error (message.str ());
      }
    const id_mgr::category_info & ci 
      = _id_manager_->get_category_info (category_);
    int requested_type = ci.get_type ();
    return get_geom_id (world_position_, requested_type, tolerance_);
  }

  void geom_map::build_from (const model_factory & factory_,
                             const string & mother_)
  {
    throw logic_error ("geomtools::geom_map::build_from: Not implemented !");
  }

} // end of namespace geomtools

// end of geom_map.cc
