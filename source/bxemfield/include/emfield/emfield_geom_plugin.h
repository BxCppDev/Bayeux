/// \file emfield/emfield_geom_plugin.h
/* Author(s):     François Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-02-23
 * Last modified: 2013-02-23
 *
 * License:
 *
 * Description:
 *
 *   emfield plugin for the geomtools geometry manager
 *
 * History:
 *
 */

#ifndef EMFIELD_EMFIELD_GEOM_PLUGIN_H
#define EMFIELD_EMFIELD_GEOM_PLUGIN_H 1

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/utils.h>
// - Bayeux/geomtools:
#include <geomtools/manager.h>
#include <geomtools/manager_macros.h>

// This project:
#include <emfield/electromagnetic_field_manager.h>

namespace emfield {

  /// \brief A plugin for the geomtools' geometry manager that drives some EM fields manager
  class emfield_geom_plugin : public geomtools::manager::base_plugin
  {
  public:

    /// Default constructor
    emfield_geom_plugin ();

    /// Destructor
    ~emfield_geom_plugin () override;

    /// Main initialization method
    int initialize(const datatools::properties & config_,
                           const geomtools::manager::plugins_dict_type & plugins_,
                           const datatools::service_dict_type & services_) override;

    /// Main reset method
    int reset() override;

    /// Check if plugin is initialized
    bool is_initialized() const override;

    /// Return a non-mutable reference on a manager of isotopes, elements and emfield
    const electromagnetic_field_manager & get_manager () const;

  protected:

    void _build_manager (const datatools::properties & mapping_config_);

  private:
    bool                           _initialized_; //!< Initialization flag
    electromagnetic_field_manager  _manager_;     //!< The embeded manager of EM fields

    GEOMTOOLS_PLUGIN_REGISTRATION_INTERFACE(emfield_geom_plugin)

  };

} // end of namespace emfield

#endif // EMFIELD_EMFIELD_GEOM_PLUGIN_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
