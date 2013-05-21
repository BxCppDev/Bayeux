// -*- mode: c++ ; -*-
/* geom_map.cc
 */

#include <geomtools/geom_map.h>

#include <datatools/exception.h>

#include <geomtools/id_mgr.h>

namespace geomtools {

  using namespace std;

  const geom_info_dict_type & geom_map::_get_geom_infos () const
  {
    return _geom_infos_;
  }

  const geom_info_dict_type & geom_map::get_geom_infos () const
  {
    return _geom_infos_;
  }

  geom_info_dict_type & geom_map::_get_geom_infos ()
  {
    return _geom_infos_;
  }

  const id_mgr & geom_map::get_id_manager () const
  {
    DT_THROW_IF (_id_manager_ == 0,logic_error, "No geometry ID manager !");
    return *_id_manager_;
  }

  const id_mgr & geom_map::_get_id_manager () const
  {
    DT_THROW_IF (_id_manager_ == 0, logic_error, "No geometry ID manager !");
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
    geom_info_dict_type::const_iterator found = _geom_infos_.find (id_);
    DT_THROW_IF (found == _geom_infos_.end (), logic_error,  "No '" << id_ << "' geometry ID !");
    return found->second;
  }

  void geom_map::compute_matching_geom_id (const geom_id & gid_pattern_,
                                           std::vector<geom_id> & gids_,
                                           bool append_) const
  {
    // If not using 'append' mode, we clear the vector :
    if (! append_)
      {
        gids_.clear ();
      }
    // Return if no valid pattern :
    if (! gid_pattern_.is_valid ())
      {
        return;
      }
    // Short cut if the GID pattern is complete (no 'ANY' addresses) :
    if (gid_pattern_.is_complete ())
      {
        geom_info_dict_type::const_iterator found = _geom_infos_.find (gid_pattern_);
        if (found != _geom_infos_.end ())
          {
            gids_.push_back (found->first);
          }
        return;
      }
    // Else traverse the full map of ginfos :
    for (geom_info_dict_type::const_iterator it = _geom_infos_.begin ();
         it != _geom_infos_.end ();
         it++)
      {
        const geom_id & gid = it->first;
        if (geom_id::match (gid_pattern_, gid, false))
          {
            gids_.push_back (gid);
          }
      }
    return;
  }

  const geom_info * geom_map::get_geom_info_ptr (const geom_id & id_) const
  {
    geom_info_dict_type::const_iterator found = _geom_infos_.find (id_);
    if (found == _geom_infos_.end ()) return 0;
    return &found->second;
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
    for (geom_info_dict_type::const_iterator i = _geom_infos_.begin ();
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
    geom_info_dict_type::const_iterator found = _geom_infos_.end ();
    DT_THROW_IF (! _id_manager_->has_category_info (category_),
                 logic_error,
                 "No geometry category named '" << category_ << "' !");
    const id_mgr::category_info & ci
      = _id_manager_->get_category_info (category_);
    int requested_type = ci.get_type ();
    return get_geom_id (world_position_, requested_type, tolerance_);
  }

  void geom_map::build_from (const model_factory & factory_,
                             const string & mother_)
  {
    DT_THROW_IF(true, runtime_error, "Not implemented !");
  }

  const geom_map::ginfo_ptr_collection_type &
  geom_map::get_ginfo_collection_with_type (uint32_t type_) const
  {
    geom_map * mutable_this = const_cast<geom_map *>(this);
    return mutable_this->_compute_ginfo_collection_with_type_ (type_);
  }


  bool geom_map::has_ginfo_collection_with_type (uint32_t type_) const
  {
    const ginfo_ptr_collection_type & col = get_ginfo_collection_with_type (type_);
    return col.size () > 0;
  }

  const geom_map::ginfo_ptr_collection_type &
  geom_map::_compute_ginfo_collection_with_type_ (uint32_t type_)
  {
    ginfo_collections_with_type_dict_type::iterator found = _geom_infos_with_type_map_.find (type_);
    if (found != _geom_infos_with_type_map_.end ())
      {
        return (found->second);
      }
    {
      // add a new collection for the requested type :
      ginfo_ptr_collection_type empty_collection;
      _geom_infos_with_type_map_[type_] = empty_collection;
    }
    // then fill it with the addresses of matching geom_infos objects
    // from the main dictionary of 'ginfos' :
    ginfo_ptr_collection_type & the_collection = _geom_infos_with_type_map_[type_];
    for (geom_info_dict_type::const_iterator i = _geom_infos_.begin ();
         i != _geom_infos_.end ();
         i++)
      {
        const geom_info & ginfo = i->second;
        has_geom_type_predicate pred (type_);
        if (pred (ginfo))
          {
            the_collection.push_back (&ginfo);
          }
      }
    return the_collection;
  }

  const geom_map::ginfo_collections_with_type_dict_type &
  geom_map::get_geom_infos_with_type_map () const
  {
    return _geom_infos_with_type_map_;
  }

} // end of namespace geomtools

// end of geom_map.cc
