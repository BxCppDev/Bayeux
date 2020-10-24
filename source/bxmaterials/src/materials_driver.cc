// -*- mode: c++ ; -*-
/* materials_driver.cc
 */

// Ourselves
#include <materials/materials_driver.h>

// Standard Library
#include <stdexcept>
#include <sstream>

// Third Party
#include <boost/foreach.hpp>

// Datatools
#include <datatools/multi_properties.h>
#include <datatools/utils.h>
#include <datatools/library_loader.h>

// Materials
#include <materials/materials_config.h>
#include <materials/isotope.h>
#include <materials/element.h>
#include <materials/material.h>
#include <materials/manager.h>

namespace materials {

  materials_driver_params::materials_driver_params()
  {
    reset();
    return;
  }

  materials_driver_params::~materials_driver_params()
  {
    reset();
    return;
  }

  void materials_driver_params::reset()
  {
    action = 0;
    with_decoration = true;
    action_options.clear();
    show_target.clear();
    LL_dlls.clear();
    LL_config.clear();
    MaterialsMgrConfigFile.clear();
    logging = datatools::logger::PRIO_WARNING;
    return;
  }

  void materials_driver_params::dump(std::ostream & out_) const
  {
    out_ << "materials driver configuration parameters: " << std::endl;
    out_ << "|-- " << "Manager config file : '" << MaterialsMgrConfigFile << "'\n";
    out_ << "|-- " << "Logging          : '" << datatools::logger::get_priority_label(logging) << "'\n";
    out_ << "|-- " << "DLLs             : " << LL_dlls.size() << "\n";
    out_ << "|-- " << "DLL config       : '" << LL_config << "'\n";
    out_ << "|-- " << "Action           : " << action << "\n";
    out_ << "|-- " << "Show target      : '" << show_target << "'\n";
    out_ << "|-- " << "With decoration  : " << with_decoration << "\n";
    out_ << "`-- " << "Action options   : " << action_options.size() << "\n";
    return;
  }

  materials_driver::materials_driver()
  {
    _initialized_ = false;
    return;
  }

  materials_driver::~materials_driver()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  bool materials_driver::is_initialized() const
  {
    return _initialized_;
  }

  void materials_driver::initialize(const materials_driver_params & mdp_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");

    _params_ = mdp_;

    datatools::library_loader LL(_params_.LL_config);
    BOOST_FOREACH (const std::string & dll_name,_params_. LL_dlls) {
      DT_LOG_NOTICE(_params_.logging,"Loading DLL '" << dll_name << "'...");
      DT_THROW_IF (LL.load(dll_name) != EXIT_SUCCESS,
                   std::runtime_error,
                   "Loading DLL '" << dll_name << "' failed !");
    }

    if (_params_.action == 0) {
      DT_LOG_NOTICE(_params_.logging, "Perform default action : list materials...");
      _params_.action = materials_driver_params::ACTION_LIST_MATERIALS;
    }

    // Configuration files :
    DT_THROW_IF(_params_.MaterialsMgrConfigFile.empty(),
                std::logic_error,
                "Missing materials manager configuration file !");

    _mgr_.reset(new materials::manager);
    datatools::fetch_path_with_env(_params_.MaterialsMgrConfigFile);
    datatools::properties MaterialsMgrConfig;
    datatools::read_config (_params_.MaterialsMgrConfigFile,
                                        MaterialsMgrConfig);
    _mgr_.get()->initialize(MaterialsMgrConfig);
    if (_params_.logging >= datatools::logger::PRIO_DEBUG) {
      DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "The materials manager : ");
      _mgr_.get()->tree_dump(std::clog);
    }

    _initialized_ = true;
    return;
  }

  void materials_driver::reset()
  {
    DT_THROW_IF(! is_initialized(), std::logic_error, "Not initialized !");
    _initialized_ = false;
    _params_.reset();
    if (_mgr_ && _mgr_.get()->is_initialized()) {
      _mgr_.get()->reset();
    }
    return;
  }

  int materials_driver::run_action(std::ostream & out_, std::ostream & log_)
  {
    // bool XXX = true;
    // if (std::find(_params_.action_options.begin(),
    //               _params_.action_options.end(),
    //               "--XXX") != _params_.action_options.end()) {
    //   XXX = false;
    // }

    if (_params_.action & materials_driver_params::ACTION_DUMP_MANAGER) {
      std::ostream & dump_out = out_;
      _mgr_.get()->tree_dump(dump_out, "Material manager : ");
    }

    if (_params_.action & materials_driver_params::ACTION_LIST_ISOTOPES) {
      if (_params_.with_decoration) log_ << "List of isotopes: " << std::endl;
      const materials::isotope_dict_type & isotopes = _mgr_.get()->get_isotopes ();
      for (materials::isotope_dict_type::const_iterator i
             = isotopes.begin();
           i != isotopes.end();
           i++) {
        materials::isotope_dict_type::const_iterator j = i;
        j++;
        if (_params_.with_decoration) {
          if (j == isotopes.end()) {
            log_ << "`-- ";
          } else {
            log_ << "|-- ";
          }
          log_ << "'" << std::flush;
        }
        out_ << i->first << std::flush;
        if (_params_.with_decoration) {
          log_ << "'" << std::flush;
        }
        out_ << std::endl;
      }
    }

    if (_params_.action & materials_driver_params::ACTION_LIST_ELEMENTS) {
      if (_params_.with_decoration) {
        log_ << "List of elements: " << std::endl;
      }
      const materials::element_dict_type & elements = _mgr_.get()->get_elements ();
      for (materials::element_dict_type::const_iterator i
             = elements.begin();
           i != elements.end();
           i++) {
        materials::element_dict_type::const_iterator j = i;
        j++;
        if (_params_.with_decoration) {
          if (j == elements.end()) {
            log_ << "`-- ";
          } else {
            log_ << "|-- ";
          }
          log_ << "'" << std::flush;
        }
        out_ << i->first << std::flush;
        if (_params_.with_decoration) {
          log_ << "'" << std::flush;
        }
        out_ << std::endl;
      }
    }

    if (_params_.action & materials_driver_params::ACTION_LIST_MATERIALS) {
      if (_params_.with_decoration) {
        log_ << "List of materials: " << std::endl;
      }
      const materials::material_dict_type & materials = _mgr_.get()->get_materials ();
      for (materials::material_dict_type::const_iterator i
             = materials.begin();
           i != materials.end();
           i++) {
        materials::material_dict_type::const_iterator j = i;
        j++;
        if (_params_.with_decoration) {
          if (j == materials.end()) {
            log_ << "`-- ";
          } else {
            log_ << "|-- ";
          }
          log_ << std::flush;
          log_ << "'" << std::flush;
        }
        out_ << i->first << std::flush;
        if (_params_.with_decoration) {
          log_ << "'" << std::flush;
          if (i->second.is_alias()) {
            log_ << " alias of '" << i->second.get_alias_of() << "' " << std::flush;;
          }
        }
        out_ << std::endl;
      }
    }

    if (_params_.action & materials_driver_params::ACTION_LIST_ALIASES) {
      std::map<std::string,std::string> aliases;
      if (_params_.with_decoration) {
        log_ << "List of material aliases: " << std::endl;
      }
      const materials::material_dict_type & materials = _mgr_.get()->get_materials ();
      for (materials::material_dict_type::const_iterator i
             = materials.begin();
           i != materials.end();
           i++) {
        if (i->second.is_alias()) {
          aliases[i->first] = i->second.get_alias_of();
        }
      }

      for (std::map<std::string,std::string>::const_iterator i
             = aliases.begin();
           i != aliases.end();
           i++) {
        std::map<std::string,std::string>::const_iterator j = i;
        j++;
        if (_params_.with_decoration) {
          if (j == aliases.end()) {
            log_ << "`-- ";
          } else {
            log_ << "|-- ";
          }
          log_ << std::flush;
          log_ << "'" << std::flush;
        }
        out_ << i->first << std::flush;
        if (_params_.with_decoration) {
          log_ << "'" << std::flush;
          log_ << " alias of '" << i->second << "' " << std::flush;;
        }
        out_ << std::endl;
      }
    }

    if (_params_.action & materials_driver_params::ACTION_SHOW_ISOTOPE) {
      const materials::isotope_dict_type & isotopes = _mgr_.get()->get_isotopes ();
      materials::isotope_dict_type::const_iterator found = isotopes.find(_params_.show_target);
      DT_THROW_IF (found == isotopes.end(), std::logic_error,
                   "Unknown isotope '" << _params_.show_target << "' !");
      found->second.get_ref().tree_dump(out_, "Isotope: ");
    }

    if (_params_.action & materials_driver_params::ACTION_SHOW_ELEMENT) {
      const materials::element_dict_type & elements = _mgr_.get()->get_elements ();
      materials::element_dict_type::const_iterator found = elements.find(_params_.show_target);
      DT_THROW_IF (found == elements.end(), std::logic_error,
                   "Unknown element '" << _params_.show_target << "' !");
      found->second.get_ref().tree_dump(out_, "Element: ");
    }

    if (_params_.action & materials_driver_params::ACTION_SHOW_MATERIAL) {
      const materials::material_dict_type & materials = _mgr_.get()->get_materials ();
      materials::material_dict_type::const_iterator found = materials.find(_params_.show_target);
      DT_THROW_IF (found == materials.end(), std::logic_error,
                   "Unknown material '" << _params_.show_target << "' !");
      found->second.get_ref().tree_dump(out_, "Material: ");
    }
    return 0;
  }

} // end of namespace materials

// end of materials_driver.cc
