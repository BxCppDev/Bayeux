// -*- mode: c++ ; -*-
/* element.cc
 */

#include <geomtools/materials_plugin.h>

#include <stdexcept>
#include <sstream>
#include <fstream>

#include <datatools/exception.h>

namespace geomtools {

  GEOMTOOLS_PLUGIN_REGISTRATION_IMPLEMENT(materials_plugin,"geomtools::materials_plugin");

  const materials::manager & materials_plugin::get_manager () const
  {
    return _manager_;
  }

  materials_plugin::materials_plugin()
  {
    _initialized_ = false;
    return;
  }

  materials_plugin::~materials_plugin()
  {
    if (is_initialized()) {
      reset ();
    }
    return;
  }

  bool materials_plugin::is_initialized() const
  {
    return _initialized_;
  }

  int materials_plugin::initialize(const datatools::properties & config_,
                                   const geomtools::manager::plugins_dict_type & /*plugins_*/,
                                   const datatools::service_dict_type & /*services_*/)
  {
    DT_THROW_IF (is_initialized(), std::logic_error,
                 "Plugin is already initialized !");

    manager::base_plugin::_basic_initialize(config_);

    {
      DT_LOG_NOTICE(get_logging_priority(),
                    "Building the embeded materials manager...");
      //config_.tree_dump (std::cerr, "Materials driver plugin setup parameters: " , "DEBUG: ");
      _build_manager(config_);
    }

    _initialized_ = true;
    return 0;
  }

  void materials_plugin::_build_manager (const datatools::properties & manager_config_)
  {
    DT_LOG_TRACE(get_logging_priority(),"Entering...");
    datatools::properties materials_config;
    manager_config_.export_and_rename_starting_with(materials_config,
                                                    "materials.", "");
    _manager_.initialize(materials_config);

    DT_LOG_TRACE(get_logging_priority(),"Exiting.");
    return;
  }

  int materials_plugin::reset()
  {
    _manager_.unlock ();
    _manager_.reset();
    _initialized_ = false;
    return 0;
  }

} // end of namespace geomtools

// end of materials_plugin.h
