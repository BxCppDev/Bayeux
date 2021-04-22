// emfield_geom_plugin.cc

// Ourselves:
#include <emfield/emfield_geom_plugin.h>

// Standard library:
#include <stdexcept>
#include <sstream>
#include <fstream>

namespace emfield {

  GEOMTOOLS_PLUGIN_REGISTRATION_IMPLEMENT(emfield_geom_plugin, "emfield::emfield_geom_plugin")

  const electromagnetic_field_manager & emfield_geom_plugin::get_manager() const
  {
    return _manager_;
  }

  emfield_geom_plugin::emfield_geom_plugin()
  {
    _initialized_ = false;
    return;
  }

  emfield_geom_plugin::~emfield_geom_plugin()
  {
    if (is_initialized ()) {
      reset();
    }
    return;
  }

  bool emfield_geom_plugin::is_initialized() const
  {
    return _initialized_;
  }

  int emfield_geom_plugin::initialize(const datatools::properties & config_,
                                      const geomtools::manager::plugins_dict_type & /*plugins_*/,
                                      const datatools::service_dict_type & /*services_*/)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Plugin is already initialized !");

    geomtools::manager::base_plugin::_basic_initialize(config_);

    datatools::properties mgr_config;
    if (config_.has_key("manager_config")) {
      std::string mgr_config_file = config_.fetch_string("manager_config");
      datatools::fetch_path_with_env(mgr_config_file);
      datatools::read_config(mgr_config_file, mgr_config);
    } else if (config_.has_key("manager.config")) {
      // obsolete:
      std::string mgr_config_file = config_.fetch_string("manager.config");
      datatools::fetch_path_with_env(mgr_config_file);
      datatools::read_config(mgr_config_file, mgr_config);
    } else {
      config_.export_and_rename_starting_with(mgr_config, "manager.", "");
      // std::clog << "NOTICE: "
      //           << "emfield::emfield_geom_plugin::initialize: "
      //           << "Building the embeded EM field manager..." << std::endl;
    }
    DT_THROW_IF(mgr_config.empty(), std::logic_error, "Empty EM field manager configuration!");

    _build_manager(mgr_config);

    _initialized_ = true;
    return 0;
  }

  void emfield_geom_plugin::_build_manager(const datatools::properties & manager_config_)
  {
    DT_LOG_TRACE(get_logging_priority(), "Entering...");

    _manager_.set_geometry_manager(get_geo_manager());
    _manager_.initialize(manager_config_);

    DT_LOG_TRACE(get_logging_priority(), "Exiting.");
    return;
  }

  int emfield_geom_plugin::reset()
  {
    _manager_.reset();
    _initialized_ = false;
    return 0;
  }

} // end of namespace emfield
