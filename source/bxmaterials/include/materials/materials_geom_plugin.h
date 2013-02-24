// -*- mode: c++; -*- 
//! \file materials/materials_geom_plugin.h
/* materials.h
 * Author(s):  François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-02-23
 * Last modified: 2013-02-23
 * 
 * License: 
 * 
 * Description: 
 *
 *   materials plugin for the geomtools geometry manager
 *  
 * History: 
 * 
 */

#ifndef MATERIALS_MATERIALS_GEOM_PLUGIN_H_
#define MATERIALS_MATERIALS_GEOM_PLUGIN_H_ 1

#include <cstdlib>
#include <iostream> 
#include <string>
#include <exception>
 
#include <datatools/properties.h>
#include <datatools/utils.h>

#include <geomtools/manager.h>
#include <geomtools/manager_macros.h>

#include <materials/manager.h>

namespace materials {

  /// \brief A plugin for the geomtools' geometry manager that embeds an isotope/element/material manager
  class materials_geom_plugin : public geomtools::manager::base_plugin
  {
  public:

    /// Default constructor
    materials_geom_plugin ();

    /// Destructor
    virtual ~materials_geom_plugin ();

    /// Main initialization method
    virtual int initialize(const datatools::properties & config_,
                           const geomtools::manager::plugins_dict_type & plugins_,
                           const datatools::service_dict_type & services_);
  
    /// Main reset method
    virtual int reset();
  
    /// Check if plugin is initialized
    virtual bool is_initialized() const;

    /// Return a non-mutable reference on a manager of isotopes, elements and materials
    const manager & get_manager () const;
  
  protected:

    void _build_manager (const datatools::properties & mapping_config_);

  private:
    bool     _initialized_; //!< Initialization flag
    manager  _manager_;  //!< The embeded manager of isotopes, elements and materials

    GEOMTOOLS_PLUGIN_REGISTRATION_INTERFACE(materials_geom_plugin);

  };

} // end of namespace materials 

#endif // MATERIALS_MATERIALS_GEOM_PLUGIN_H_

// end of materials_geom_plugin.h
