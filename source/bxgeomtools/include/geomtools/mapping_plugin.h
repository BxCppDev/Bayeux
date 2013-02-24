// -*- mode: c++ ; -*- 
/* mapping_plugin.h
 * Author (s) :     Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef GEOMTOOLS_MAPPING_PLUGIN_H_
#define GEOMTOOLS_MAPPING_PLUGIN_H_ 1

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
  virtual ~mapping_plugin ();

  /// Main plugin initialization method
  virtual int initialize(const datatools::properties & config_,
                         const geomtools::manager::plugins_dict_type & plugins_,
                         const datatools::service_dict_type & services_);
  
  /// Plugin reset method
  virtual int reset();
  
  /// Check if plugin is initialized
  virtual bool is_initialized() const;

  /// Returns a non-mutable reference to the embeded mapping instance
  const geomtools::mapping & get_mapping () const;
  
protected:

  /// Internal mapping build method
  void _build_mapping (const datatools::properties & mapping_config_);

private:

  bool                _initialized_; /// Initialization flag
  geomtools::mapping  _mapping_;  /// Embeded mapping instance

  GEOMTOOLS_PLUGIN_REGISTRATION_INTERFACE(mapping_plugin);

};

} // end of namespace geomtools

#endif // GEOMTOOLS_MAPPING_PLUGIN_H_

// end of mapping_plugin.h
