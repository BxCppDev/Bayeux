/// mapping_plugin.h
/* Author(s) :    \file geomtools/Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2013-02-24
 * Last modified: 2013-02-24
 *
 * License:
 *
 * Description:
 *
 *   A geometry manager plugin with embeded mapping.
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_MAPPING_PLUGIN_H
#define GEOMTOOLS_MAPPING_PLUGIN_H 1

// This project:
#include <geomtools/manager.h>
#include <geomtools/manager_macros.h>
#include <geomtools/mapping.h>

namespace geomtools {

  /// \brief A geometry manager plugin with embeded mapping with its specific mapping rules.
  class mapping_plugin : public geomtools::manager::base_plugin
  {
  public:

    /// Default constructor
    mapping_plugin ();

    /// Destructor
    ~mapping_plugin () override;

    /// Main plugin initialization method
    int initialize(const datatools::properties & config_,
                           const geomtools::manager::plugins_dict_type & plugins_,
                           const datatools::service_dict_type & services_) override;

    /// Plugin reset method
    int reset() override;

    /// Check if plugin is initialized
    bool is_initialized() const override;

    /// Returns a non-mutable reference to the embeded mapping instance
    const geomtools::mapping & get_mapping () const;

  protected:

    /// Internal mapping build method
    void _build_mapping (const datatools::properties & mapping_config_);

  private:

    bool                _initialized_; //<! Initialization flag
    geomtools::mapping  _mapping_;  //<! Embeded mapping instance

    GEOMTOOLS_PLUGIN_REGISTRATION_INTERFACE(mapping_plugin)

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_MAPPING_PLUGIN_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
