// logical_volume_selector.cc

// Ourselves:
#include <geomtools/logical_volume_selector.h>

// Standard library:
#include <stdexcept>
#include <iostream>
#include <sstream>

// Third party:
// - Boost:
#include <boost/regex.hpp>
// - Bayeux/datatools:
#include <datatools/exception.h>

// This project:
#include <geomtools/logical_volume.h>
#include <geomtools/manager.h>

namespace geomtools {
  
  logical_volume_selector::logical_volume_selector(const manager & mgr_)
    : _mgr_(mgr_)
  {
    return;
  }

  void logical_volume_selector::set_clear_first(bool cf_)
  {
    _clear_first_ = cf_;
    return;
  }

  void logical_volume_selector::add_all()
  {
    _lv_names_regex_.insert("(.*)");
  }
  
  void logical_volume_selector::add_by_name(const std::string & logical_volume_name_)
  {
    _lv_names_.insert(logical_volume_name_);
    return;
  }
  
  void logical_volume_selector::exclude_by_name(const std::string & logical_volume_name_)
  {
    _lv_names_excluded_.insert(logical_volume_name_);
    return;
  }
       
  void logical_volume_selector::add_by_name_regex(const std::string & logical_volume_name_regex_)
  {
    _lv_names_regex_.insert(logical_volume_name_regex_);
    return;
  }
 
  void logical_volume_selector::exclude_by_name_regex(const std::string & logical_volume_name_regex_)
  {
    _lv_names_regex_excluded_.insert(logical_volume_name_regex_);
    return;
  }
   
  void logical_volume_selector::add_by_material(const std::string & material_name_)
  {
    _lv_materials_.insert(material_name_);
    return;
  }
   
  void logical_volume_selector::exclude_by_material(const std::string & material_name_)
  {
    _lv_materials_excluded_.insert(material_name_);
    return;
  }

  void logical_volume_selector::add_by_material_regex(const std::string & material_name_regex_)
  {
    _lv_materials_regex_.insert(material_name_regex_);
    return;
  }

  void logical_volume_selector::exclude_by_material_regex(const std::string & material_name_regex_)
  {
    _lv_materials_regex_excluded_.insert(material_name_regex_);
    return;
  }

  void logical_volume_selector::initialize(const datatools::properties & config_)
  {
    if (config_.has_flag("all")) {
      add_all();
    }
    if (config_.has_key("names")) {
      std::set<std::string> names;
      config_.fetch("names", names);
      for (const std::string & name : names) {
        add_by_name(name);
      }
    }
    if (config_.has_key("regex")) {
      std::set<std::string> regex;
      config_.fetch("regex", regex);
      for (const std::string & reg : regex) {
        add_by_name_regex(reg);
      }
    }
    if (config_.has_key("with_materials")) {
      std::set<std::string> materials;
      config_.fetch("with_materials", materials);
      for (const std::string & mat : materials) {
        add_by_material(mat);
      }
    }
    if (config_.has_key("with_materials_regex")) {
      std::set<std::string> materials_regex;
      config_.fetch("with_materials_regex", materials_regex);
      for (const std::string & mat_regex : materials_regex) {
        add_by_material_regex(mat_regex);
      }
    }
    if (config_.has_key("excluded")) {
      std::set<std::string> names;
      config_.fetch("excluded", names);
      for (const std::string & name : names) {
        exclude_by_name(name);
      }
    }
    if (config_.has_key("regex_excluded")) {
      std::set<std::string> regex;
      config_.fetch("regex_excluded", regex);
      for (const std::string & reg : regex) {
        exclude_by_name_regex(reg);
      }
    }
    if (config_.has_key("without_materials")) {
      std::set<std::string> materials;
      config_.fetch("without_materials", materials);
      for (const std::string & mat : materials) {
        exclude_by_material(mat);
      }
    }
    if (config_.has_key("without_materials_regex")) {
      std::set<std::string> materials_regex;
      config_.fetch("without_materials_regex", materials_regex);
      for (const std::string & mat_regex : materials_regex) {
        exclude_by_material_regex(mat_regex);
      }
    }
    return;
  }

  void logical_volume_selector::reset()
  {
    _clear_first_ = false;
    _lv_names_.clear();
    _lv_names_regex_.clear();
    _lv_materials_.clear();
    _lv_materials_regex_.clear();
    _lv_names_excluded_.clear();
    _lv_names_regex_excluded_.clear();
    _lv_materials_excluded_.clear();
    _lv_materials_regex_excluded_.clear();
    return;
  }

  void logical_volume_selector::resolve(std::set<std::string> & logical_volumes_names_)
  {
    if (_clear_first_) logical_volumes_names_.clear();
    for (geomtools::logical_volume::dict_type::const_iterator ilogical
           = _mgr_.get_factory().get_logicals().begin();
         ilogical != _mgr_.get_factory().get_logicals().end();
         ++ilogical) {
      
      bool add_lv = false;
      const std::string & log_name = ilogical->first;

      if (!add_lv) {
        for (const std::string & lv_name : _lv_names_) {
          if (lv_name == log_name) {
            add_lv = true;
            break;
          }
        }
      }
      
      if (!add_lv) {
        for (const std::string & log_name_regex :_lv_names_regex_) {
          boost::regex e1(log_name_regex, boost::regex::extended);
          if (boost::regex_match(log_name, e1)) {
            add_lv = true;
            break;
          }
        }
      }
     
      if (!add_lv) {
        const geomtools::logical_volume & log = *(ilogical->second);
        bool checked_material = false;
        std::string mat_name;
        for (const std::string & mat_name : _lv_materials_) {
          if (log.get_material_ref() == mat_name) {
            checked_material = true;
            break;
          }
        }
        if (checked_material) {
          add_lv = true;
        }
      }
     
      if (!add_lv) {
        for (const std::string & mat_name_regex : _lv_materials_regex_) {
          boost::regex e1(mat_name_regex, boost::regex::extended);
          const geomtools::logical_volume & log = *(ilogical->second);
          if (boost::regex_match(log.get_material_ref(), e1)) {
            add_lv = true;
            break;
          }
        }
      }

      // Excluded:
      if (add_lv) {
        for (const std::string & lv_name : _lv_names_excluded_) {
          if (lv_name == log_name) {
            add_lv = false;
            break;
          }
        }
      }
      
      if (add_lv) {
        for (const std::string & log_name_regex :_lv_names_regex_excluded_) {
          boost::regex e1(log_name_regex, boost::regex::extended);
          if (boost::regex_match(log_name, e1)) {
            add_lv = false;
            break;
          }
        }
      }
     
      if (add_lv) {
        const geomtools::logical_volume & log = *(ilogical->second);
        bool checked_material = false;
        std::string mat_name;
        for (const std::string & mat_name : _lv_materials_excluded_) {
          if (log.get_material_ref() == mat_name) {
            checked_material = true;
            break;
          }
        }
        if (checked_material) {
          add_lv = false;
        }
      }
     
      if (add_lv) {
        for (const std::string & mat_name_regex : _lv_materials_regex_excluded_) {
          boost::regex e1(mat_name_regex, boost::regex::extended);
          const geomtools::logical_volume & log = *(ilogical->second);
          if (boost::regex_match(log.get_material_ref(), e1)) {
            add_lv = false;
            break;
          }
        }
      }
    
      // Final test:
      if (add_lv) {
        logical_volumes_names_.insert(log_name);
      }
    }
    return;
  }

} // end of namespace geomtools
