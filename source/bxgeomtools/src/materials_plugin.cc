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
                                   const geomtools::manager::plugins_dict_type & plugins_,
                                   const datatools::service_dict_type & services_)
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
    if (manager_config_.has_flag ("materials.debug")) {
        _manager_.set_debug (true);
      }

    std::vector<std::string> input_files;
    if (manager_config_.has_key ("materials.files")) {
        manager_config_.fetch("materials.files", input_files);
      }
    for (std::vector<std::string>::const_iterator i = input_files.begin ();
         i != input_files.end ();
         i++) {
        datatools::multi_properties config ("name", "type");
        std::string filename = *i;
        datatools::fetch_path_with_env (filename);
        config.read (filename);
        if (_manager_.is_debug()) {
          DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG,
                       "Config file '" << filename << "' : ");
          config.tree_dump (std::clog);
        }
        _manager_.load (config);
      }

    _manager_.lock ();

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
