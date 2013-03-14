// -*- mode: c++ ; -*- 
/* geom_materials_access.cc
 */

#include <string>
#include <stdexcept>

#include <materials/manager.h>
#include <geomtools/materials_plugin.h>
#include <geomtools/mapping_plugin.h>
#include <geomtools/mapping.h>
#include <geomtools/manager.h>

#include <genvtx/detail/geom_manager_utils.h>

namespace genvtx {
  namespace detail {

    const geomtools::mapping *
    access_geometry_mapping(const geomtools::manager & geo_mgr_,
                            const std::string & mapping_plugin_name_)
    { 
      const geomtools::mapping * mapping_ptr = 0;
      if (mapping_plugin_name_.empty())
        {
          if (geo_mgr_.is_mapping_available ())
            {
              mapping_ptr = &geo_mgr_.get_mapping ();
            }
        }
      else
        {
          if (geo_mgr_.has_plugin(mapping_plugin_name_)
              && geo_mgr_.is_plugin_a<geomtools::mapping_plugin>(mapping_plugin_name_))
            {
              const geomtools::mapping_plugin & MP = 
                geo_mgr_.get_plugin<geomtools::mapping_plugin>(mapping_plugin_name_);
              if (! MP.is_initialized())
                {
                  std::ostringstream message;
                  message << "genvtx::detail::access_geometry_mapping: "
                          << "Geometry mapping plugin named '" << mapping_plugin_name_  
                          << "' is not initialized !";
                  throw std::logic_error (message.str ());                
                }
              mapping_ptr = &(MP.get_mapping ());
            }
          else
            {
              std::ostringstream message;
              message << "genvtx::detail::access_geometry_mapping: "
                      << "No geometry mapping plugin named '" << mapping_plugin_name_  
                      << "' available from the geometry manager !";
              throw std::logic_error (message.str ());
            }
        }
      return mapping_ptr;
    }

    const materials::manager *
    access_materials_manager(const geomtools::manager & geo_mgr_,
                             const std::string & materials_plugin_name_)
    {
      if (geo_mgr_.has_plugin(materials_plugin_name_)
          && geo_mgr_.is_plugin_a<geomtools::materials_plugin>(materials_plugin_name_))
        {
          const geomtools::materials_plugin & MGP = 
            geo_mgr_.get_plugin<geomtools::materials_plugin>(materials_plugin_name_);
          if (! MGP.is_initialized())
            {
              std::ostringstream message;
              message << "genvtx::detail::access_materials_manager: "
                      << "Geometry materials plugin named '" << materials_plugin_name_  
                      << "' is not initialized !";
              throw std::logic_error (message.str ());                
            }
          return &(MGP.get_manager ());
        }
      return 0;
    }

  } // end of namespace detail
} // end of namespace genvtx

// end of geom_materials_access.cc
