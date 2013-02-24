// -*- mode: c++ ; -*-
/* element.cc
 */

#include <materials/materials_geom_plugin.h>

#include <stdexcept>
#include <sstream>
#include <fstream>

namespace materials {

  GEOMTOOLS_PLUGIN_REGISTRATION_IMPLEMENT(materials_geom_plugin,"materials::materials_geom_plugin");

  const manager & materials_geom_plugin::get_manager () const
  {
    return _manager_;
  }

  materials_geom_plugin::materials_geom_plugin()
  {
    _initialized_ = false;
    return;
  }

  materials_geom_plugin::~materials_geom_plugin()
  {
    if (is_initialized())
      {
        reset ();
      }
    return;
  }

  bool materials_geom_plugin::is_initialized() const 
  {
    return _initialized_;
  }

  int materials_geom_plugin::initialize(const datatools::properties & config_,
                                        const geomtools::manager::plugins_dict_type & plugins_,
                                        const datatools::service_dict_type & services_)
  {
    if (is_initialized())
      {
        std::ostringstream message;
        message << "materials_geom_plugin::initialize: "
                << "Plugin is already initialized !"
                << std::endl;
        throw std::logic_error(message.str());
      }

    {
      std::clog << "NOTICE: " 
                << "materials::materials_geom_plugin::initialize: "
                << "Building the embeded materials manager..." << std::endl;
      //config_.tree_dump (std::cerr, "Materials driver plugin setup parameters: " , "DEBUG: ");
      _build_manager(config_);
    }

    _initialized_ = true;
    return 0;
  }

  void materials_geom_plugin::_build_manager (const datatools::properties & manager_config_)
  {
    bool devel = false;
    //devel = true;
    if (devel)
      {
        std::clog << "DEVEL: " 
                  << "materials::materials_geom_plugin::_build_manager: "
                  << "Entering..." << std::endl;
      }
    if (manager_config_.has_flag ("materials.debug"))
      {
        _manager_.set_debug (true);
      }
    
    std::vector<std::string> input_files;
    if (manager_config_.has_key ("materials.files"))
      {
        manager_config_.fetch("materials.files", input_files);
      }
    for (std::vector<std::string>::const_iterator i = input_files.begin ();
         i != input_files.end ();
         i++)
      {
        datatools::multi_properties config ("name", "type");
        std::string filename = *i;
        datatools::fetch_path_with_env (filename);
        config.read (filename);
        if (_manager_.is_debug() || devel)
          {
            std::clog << "DEBUG: materials::materials_geom_plugin::_build_manager: Config file '" << filename << "' : " << std::endl;
            config.tree_dump (std::clog, "", "DEBUG: ");
          }
        _manager_.load (config);
      }

    _manager_.lock ();

    if (devel)
      {
        std::clog << "DEVEL: " 
                  << "materials::materials_geom_plugin::_build_manager: "
                  << "Exiting." << std::endl;
      }

    return;
  }

  int materials_geom_plugin::reset()
  {
    _manager_.unlock ();
    _manager_.reset();
    _initialized_ = false;
    return 0;
  }

} // end of namespace materials 

// end of materials.h
