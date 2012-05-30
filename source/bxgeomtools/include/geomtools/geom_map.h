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

#include <geomtools/utils.h>
#include <boost/cstdint.hpp>

#include <geomtools/i_locator.h>
#include <geomtools/geom_id.h>
#include <geomtools/geom_info.h>

namespace geomtools {

  class id_mgr;
  class model_factory;

  class geom_map : public i_locator
  {
  public:

    typedef std::vector<const geom_info*> ginfo_ptr_collection_type;
    typedef std::map<uint32_t, ginfo_ptr_collection_type> ginfo_collections_with_type_dict_type;

  protected:

    const id_mgr & _get_id_manager () const;

    const geom_info_dict_t & _get_geom_infos () const;

    geom_info_dict_t & _get_geom_infos ();

  public:

    const id_mgr & get_id_manager () const;

    const geom_info_dict_t & get_geom_infos () const;

    geom_map ();

    virtual ~geom_map ();

    bool has_id_manager () const;

    void set_id_manager (const id_mgr & mgr_);

    const geom_id & get_invalid_geom_id () const;

    /***  i_locator interface  ***/

    virtual bool validate_id (const geom_id & id_) const;

    virtual const geom_info & get_geom_info (const geom_id &) const;

    virtual const geom_id & get_geom_id (const vector_3d & world_position_,
                                         int type_,
                                         double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual bool has_geom_info (const geom_id &) const;

    void get_geom_id (const vector_3d & world_position_,
                      int type_,
                      geom_id & gid_,
                      double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    void get_geom_id (const vector_3d & world_position_,
                      const std::string & category_,
                      geom_id & gid_,
                      double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual const geom_id & get_geom_id (const vector_3d & world_position_,
                                         const std::string & category_,
                                         double tolerance_ = GEOMTOOLS_PROPER_TOLERANCE) const;

    virtual void build_from (const model_factory & factory_,
                             const std::string & mother_ = "world");

    static bool check_inside (const geom_info & ginfo_,
                              const vector_3d & world_position_,
                              double tolerance_);

    bool has_ginfo_collection_with_type (uint32_t type_) const;

    const ginfo_ptr_collection_type & get_ginfo_collection_with_type (uint32_t type_) const;

  private:

    /// Used by the 'get_typed_ginfo_collection' public interface
    // Note: instantiate at first use technique
    const ginfo_ptr_collection_type & _compute_ginfo_collection_with_type_ (uint32_t type_);
                                                                                       
  protected:

    const ginfo_collections_with_type_dict_type & get_geom_infos_with_type_map () const;

  private:

    geom_id          _invalid_geom_id_; //!< value of a invalid geometry ID
    const id_mgr *   _id_manager_;      //!< the ID manager that knows about geometry categories and their relationship
    geom_info_dict_t _geom_infos_;      //!< the main dictionary of geometry informations addressed through IDs

    ginfo_collections_with_type_dict_type _geom_infos_with_type_map_;

  };

} // end of namespace geomtools

#endif // __geomtools__geom_map_h

// end of geom_map.h
