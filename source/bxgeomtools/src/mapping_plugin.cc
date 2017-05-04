// -*- mode: c++ ; -*-
/* mapping_plugin.cc */

#include <geomtools/mapping_plugin.h>

namespace geomtools {

  GEOMTOOLS_PLUGIN_REGISTRATION_IMPLEMENT(mapping_plugin,"geomtools::mapping_plugin")

  const geomtools::mapping & mapping_plugin::get_mapping () const
  {
    return _mapping_;
  }

  mapping_plugin::mapping_plugin()
  {
    _initialized_ = false;
    return;
  }

  mapping_plugin::~mapping_plugin()
  {
    if (is_initialized())
      {
        reset ();
      }
    return;
  }

  bool mapping_plugin::is_initialized() const
  {
    return _initialized_;
  }

  int mapping_plugin::initialize(const datatools::properties & config_,
                                 const geomtools::manager::plugins_dict_type & /*plugins_*/,
                                 const datatools::service_dict_type & /*services_*/)
  {
    DT_THROW_IF (is_initialized(), std::logic_error, "Plugin is already initialized !");
    _build_mapping(config_);
    _initialized_ = true;
    return 0;
  }

  void mapping_plugin::_build_mapping (const datatools::properties & mapping_config_)
  {
    _mapping_.set_id_manager (get_geo_manager().get_id_mgr ());
    _mapping_.initialize (mapping_config_);
    _mapping_.build_from (get_geo_manager().get_factory (),
                          get_geo_manager().get_world_name ());
    return;
  }

  int mapping_plugin::reset()
  {
    _initialized_ = false;
    return 0;
  }

} // end of namespace geomtools

/* end of mapping_plugin.cc */
