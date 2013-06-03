// -*- mode: c++; -*-
//! \file emfield/emfield_geom_plugin.h
/* emfield.h
 * Author(s):  François Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef EMFIELD_EMFIELD_GEOM_PLUGIN_H_
#define EMFIELD_EMFIELD_GEOM_PLUGIN_H_ 1

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <datatools/properties.h>
#include <datatools/utils.h>

#include <geomtools/manager.h>
#include <geomtools/manager_macros.h>

#include <emfield/electromagnetic_field_manager.h>

namespace emfield {

  /// \brief A plugin for the geomtools' geometry manager that drives some EM fields manager
  class emfield_geom_plugin : public geomtools::manager::base_plugin
  {
  public:

    /// Default constructor
    emfield_geom_plugin ();

    /// Destructor
    virtual ~emfield_geom_plugin ();

    /// Main initialization method
    virtual int initialize(const datatools::properties & config_,
                           const geomtools::manager::plugins_dict_type & plugins_,
                           const datatools::service_dict_type & services_);

    /// Main reset method
    virtual int reset();

    /// Check if plugin is initialized
    virtual bool is_initialized() const;

    /// Return a non-mutable reference on a manager of isotopes, elements and emfield
    const electromagnetic_field_manager & get_manager () const;

  protected:

    void _build_manager (const datatools::properties & mapping_config_);

  private:
    bool                           _initialized_; //!< Initialization flag
    electromagnetic_field_manager  _manager_;     //!< The embeded manager of EM fields

    GEOMTOOLS_PLUGIN_REGISTRATION_INTERFACE(emfield_geom_plugin);

  };

} // end of namespace emfield

#endif // EMFIELD_EMFIELD_GEOM_PLUGIN_H_

// end of emfield_geom_plugin.h
