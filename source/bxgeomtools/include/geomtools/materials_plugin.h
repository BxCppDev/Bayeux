/// \file geomtools/materials_plugin.h
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

#ifndef GEOMTOOLS_MATERIALS_PLUGIN_H
#define GEOMTOOLS_MATERIALS_PLUGIN_H 1

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/utils.h>
// - Bayeux/materials:
#include <materials/manager.h>

// This project:
#include <geomtools/manager.h>
#include <geomtools/manager_macros.h>

namespace geomtools {

  /// \brief A plugin for the geomtools' geometry manager that embeds an isotope/element/material manager
  class materials_plugin : public geomtools::manager::base_plugin
  {
  public:

    /// Default constructor
    materials_plugin ();

    /// Destructor
    ~materials_plugin () override;

    /// Main initialization method
    int initialize(const datatools::properties & config_,
                           const geomtools::manager::plugins_dict_type & plugins_,
                           const datatools::service_dict_type & services_) override;

    /// Main reset method
    int reset() override;

    /// Check if plugin is initialized
    bool is_initialized() const override;

    /// Return a non-mutable reference on a manager of isotopes, elements and materials
    const materials::manager & get_manager () const;

  protected:

    void _build_manager (const datatools::properties & mapping_config_);

  private:

    bool               _initialized_; //!< Initialization flag
    materials::manager _manager_;     //!< The embeded manager of isotopes, elements and materials

    GEOMTOOLS_PLUGIN_REGISTRATION_INTERFACE(materials_plugin)

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_MATERIALS_PLUGIN_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
