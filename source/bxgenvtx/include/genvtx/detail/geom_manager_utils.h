// -*- mode: c++ ; -*-
/// \file genvtx/detail/geom_manager_utils.h
/* Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-03-11
 * Last modified:
 */

#ifndef GENVTX_DETAIL_GEOM_MANAGER_UTILS_H
#define GENVTX_DETAIL_GEOM_MANAGER_UTILS_H 1

// Standard library:
#include <string>

namespace materials {
  class manager;
}

namespace geomtools {
  class manager;
  class mapping;
}

namespace genvtx {
  /// Nested private namespace of the Bayeux/genvtx module library (detail)
  namespace detail {

    const materials::manager *
    access_materials_manager(const geomtools::manager & geo_mgr_,
                             const std::string & materials_plugin_name_ = "materials_driver");

    const geomtools::mapping *
    access_geometry_mapping(const geomtools::manager & geo_mgr_,
                            const std::string & mapping_plugin_name_ = "");

  } // end of namespace detail
} // end of namespace genvtx

#endif // GENVTX_DETAIL_GEOM_MANAGER_UTILS_H
