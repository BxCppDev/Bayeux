/* geom_materials_access.cc
 */

#include <genvtx/detail/geom_manager_utils.h>

// Standard library:
#include <string>
#include <stdexcept>

// Third party:
// - Bayeux/datatools
#include <datatools/exception.h>
// - Bayeux/materials
#include <materials/manager.h>
// - Bayeux/geomtools
#include <geomtools/materials_plugin.h>
#include <geomtools/mapping_plugin.h>
#include <geomtools/mapping.h>
#include <geomtools/manager.h>

namespace genvtx {
  namespace detail {

    const geomtools::mapping *
    access_geometry_mapping(const geomtools::manager & geo_mgr_,
                            const std::string & mapping_plugin_name_)
    {
      const geomtools::mapping * mapping_ptr = 0;
      if (mapping_plugin_name_.empty()) {
        if (geo_mgr_.is_mapping_available ()) {
          mapping_ptr = &geo_mgr_.get_mapping ();
        }
      } else {
        if (geo_mgr_.has_plugin(mapping_plugin_name_)
            && geo_mgr_.is_plugin_a<geomtools::mapping_plugin>(mapping_plugin_name_)) {
          const geomtools::mapping_plugin & MP =
            geo_mgr_.get_plugin<geomtools::mapping_plugin>(mapping_plugin_name_);
          DT_THROW_IF (! MP.is_initialized(), std::logic_error,
                       "Geometry mapping plugin named '" << mapping_plugin_name_
                       << "' is not initialized !");
          mapping_ptr = &(MP.get_mapping ());
        } else {
          DT_THROW_IF(true, std::logic_error,
                      "No geometry mapping plugin named '" << mapping_plugin_name_
                      << "' available from the geometry manager !");
        }
      }
      return mapping_ptr;
    }

    const materials::manager *
    access_materials_manager(const geomtools::manager & geo_mgr_,
                             const std::string & materials_plugin_name_)
    {
      std::string materials_plugin_name = materials_plugin_name_;
      if (materials_plugin_name.empty()) {
          // We try to find a material plugin associated to the manager :
          typedef geomtools::manager::plugins_dict_type dict_type;
          const dict_type & plugins = geo_mgr_.get_plugins ();
          for (dict_type::const_iterator ip = plugins.begin();
               ip != plugins.end();
               ip++) {
            const std::string & plugin_name = ip->first;
            if (geo_mgr_.is_plugin_a<geomtools::materials_plugin>(plugin_name)) {
              materials_plugin_name = plugin_name;
              break;
            }
          }
        }
      if (geo_mgr_.has_plugin(materials_plugin_name)
          && geo_mgr_.is_plugin_a<geomtools::materials_plugin>(materials_plugin_name)) {
        const geomtools::materials_plugin & MGP =
          geo_mgr_.get_plugin<geomtools::materials_plugin>(materials_plugin_name);
        DT_THROW_IF (! MGP.is_initialized(), std::logic_error,
                     "Geometry materials plugin named '" << materials_plugin_name
                     << "' is not initialized !");
        return &(MGP.get_manager ());
      }
      return 0;
    }

  } // end of namespace detail
} // end of namespace genvtx
