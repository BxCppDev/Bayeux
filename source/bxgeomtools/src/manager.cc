// -*- mode: c++ ; -*-
/* manager.cc
 *
 * Copyright 2007-2013 F. Mauger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public  License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include <geomtools/manager.h>

#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <list>
#include <map>

#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/units.h>
#include <datatools/version_id.h>

#include <geomtools/visibility.h>
#include <geomtools/sensitive.h>
#include <geomtools/material.h>
#include <geomtools/mapping.h>

namespace geomtools {

  bool manager::g_devel = false;

  DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(manager::base_plugin,
                                                   "geomtools::manager::base_plugin/__system__");

  manager::base_plugin::base_plugin()
  {
    _geo_mgr_ = 0;
    return;
  }

  manager::base_plugin::~base_plugin()
  {
    _geo_mgr_ = 0;
    return;
  }

  void manager::base_plugin::set_geo_manager(manager & geo_mgr_)
  {
    if (is_initialized()) {
      std::ostringstream message;
      message << "manager::base_plugin::set_geo_manager: "
              << "Cannot set the geometry manager ! Plugin is already initialized !"
              << std::endl;
      throw std::logic_error(message.str());
    }
    _geo_mgr_ = &geo_mgr_;
    return;
  }

  const manager & manager::base_plugin::get_geo_manager() const
  {
    return *_geo_mgr_;
  }

  manager & manager::base_plugin::_grab_geo_manager()
  {
    return *_geo_mgr_;
  }

  int manager::base_plugin::initialize_standalone(const datatools::properties& config_)
  {
    plugins_dict_type dummy;
    return initialize_simple(config_, dummy);
  }

  int manager::base_plugin::initialize_simple(const datatools::properties & config_,
                                              const plugins_dict_type & plugins_)
  {
    datatools::service_dict_type dummy;
    return initialize(config_, plugins_, dummy);
  }

  /************************************************/

  bool manager::plugin_entry::is_created() const
  {
    return _status_ & plugin_entry::STATUS_CREATED;
  }

  bool manager::plugin_entry::is_initialized() const
  {
    return _status_ & plugin_entry::STATUS_INITIALIZED;
  }

  void manager::plugin_entry::set_name(const std::string& name_)
  {
    if (is_created()) {
      std::ostringstream message;
      message << "manager::plugin_entry::set_name: "
              << "Plugin is already created !"
              << std::endl;
      throw std::logic_error(message.str());
    }
    if (_name_.empty()) {
      std::ostringstream message;
      message << "manager::plugin_entry::set_name: "
              << "Cannot use an empty name !"
              << std::endl;
      throw std::logic_error(message.str());
    }
    _name_ = name_;
  }

  const std::string & manager::plugin_entry::get_name() const
  {
    return _name_;
  }

  void manager::plugin_entry::set_description(const std::string& description_)
  {
    if (is_created()) {
      std::ostringstream message;
      message << "manager::plugin_entry::set_description: "
              << "Plugin is already created !"
              << std::endl;
      throw std::logic_error(message.str());
    }
    _description_ = description_;
    return;
  }

  bool manager::plugin_entry::has_description() const
  {
    return ! _description_.empty ();
  }

  const std::string & manager::plugin_entry::get_description() const
  {
    return _description_;
  }

  /// Set the configuration
  const datatools::properties & manager::plugin_entry::get_config () const
  {
    return _config_;
  }

  /// Get the configuration
  void manager::plugin_entry::set_config (const datatools::properties & config_)
  {
    if (is_initialized()) {
      std::ostringstream message;
      message << "manager::plugin_entry::set_config: "
              << "Plugin is already initialized !"
              << std::endl;
      throw std::logic_error(message.str());
    }
    _config_ = config_;
    return;
  }

  /// Grab the configuration
  datatools::properties & manager::plugin_entry::grab_config ()
  {
    if (is_initialized()) {
      std::ostringstream message;
      message << "manager::plugin_entry::grab_config: "
              << "Plugin is already initialized !"
              << std::endl;
      throw std::logic_error(message.str());
    }
    return _config_;
  }

  void manager::plugin_entry::_set_status (uint32_t status_)
  {
    _status_ = status_;
    return;
  }

  bool manager::plugin_entry::can_be_dropped () const
  {
    return true;
  }

  manager::plugin_entry::plugin_entry()
  {
    _status_ = 0x0;
    return;
  }

  manager::plugin_entry::~plugin_entry()
  {
    _handle_.reset();
    return;
  }

  manager::base_plugin & manager::plugin_entry::grab ()
  {
    return _handle_.grab();
  }

  const manager::base_plugin & manager::plugin_entry::get () const
  {
    return _handle_.get();
  }

  void manager::plugin_entry::tree_dump(std::ostream& out,
                                        const std::string& title,
                                        const std::string& a_indent,
                                        bool a_inherit) const
  {

    std::string indent;
    if (!a_indent.empty()) indent = a_indent;

    if (!title.empty()) out << indent << title << std::endl;

    out << indent << datatools::i_tree_dumpable::tag
        << "Plugin name     : '"
        << _name_
        << "'" << std::endl;

    out << indent << datatools::i_tree_dumpable::tag
        << "Plugin ID       : '"
        << _id_
        << "'" << std::endl;

    out << indent << datatools::i_tree_dumpable::tag
        << "Plugin description : '"
        << _description_
        << "'" << std::endl;

    out << indent << datatools::i_tree_dumpable::tag
        << "Can be dropped   : "
        << can_be_dropped() << std::endl;

    out << indent << datatools::i_tree_dumpable::inherit_tag (a_inherit)
        << "Plugin status   : "
        << _status_;
    {
      size_t count = 0;
      std::ostringstream status_info;
      if (_status_ & plugin_entry::STATUS_CREATED) {
        if (count) status_info << ',';
        status_info << "created";
        count++;
      }
      if (_status_ & plugin_entry::STATUS_INITIALIZED) {
        if (count) status_info << ',';
        status_info << "initialized";
        count++;
      }
      if (count) {
        out << ' ' << '('
            << status_info.str()
            << ')';
      }
    }
    out << std::endl;
  }

  /*****************************************************/

  bool manager::is_debug () const
  {
    return _debug_;
  }

  void manager::set_setup_label (const std::string & sl_)
  {
    _setup_label_ = sl_;
    return;
  }

  const std::string & manager::get_setup_label () const
  {
    return _setup_label_;
  }

  void manager::fetch_setup_version_id (datatools::version_id & vid_) const
  {
    vid_.reset ();
    if (_setup_version_.empty ()) return;
    std::istringstream iss (_setup_version_);
    iss >> vid_;
    return;
  }

  void manager::set_setup_version (const std::string & sv_)
  {
    // check the versioning format :
    if (sv_ != "")  {
      datatools::version_id geom_mgr_setup_vid;
      std::istringstream iss (sv_);
      iss >> geom_mgr_setup_vid;
      if (! iss) {
        std::ostringstream message;
        message << "geomtools::manager::set_setup_version: "
                << "Invalid version format '" << sv_ << "' !";
        throw std::logic_error (message.str ());
      }
    }
    _setup_version_ = sv_;
    return;
  }

  const std::string & manager::get_setup_version () const
  {
    return _setup_version_;
  }

  void manager::set_setup_description (const std::string & sd_)
  {
    _setup_description_ = sd_;
    return;
  }

  const std::string & manager::get_setup_description () const
  {
    return _setup_description_;
  }

  void manager::set_debug (bool new_value_)
  {
    _debug_ = new_value_;
    return;
  }

  bool manager::is_verbose () const
  {
    return _verbose_;
  }

  void manager::set_verbose (bool new_value_)
  {
    _verbose_ = new_value_;
    return;
  }

  bool manager::is_initialized () const
  {
    return _initialized_;
  }

  const geomtools::model_factory & manager::get_factory () const
  {
    return _factory_;
  }

  geomtools::model_factory & manager::grab_factory ()
  {
    return _factory_;
  }

  const geomtools::id_mgr & manager::get_id_mgr () const
  {
    return _id_manager_;
  }

  geomtools::id_mgr & manager::grab_id_mgr ()
  {
    return _id_manager_;
  }

  const manager::plugins_dict_type & manager::get_plugins () const
  {
    return _plugins_;
  }

  manager::plugins_dict_type & manager::grab_plugins ()
  {
    return _plugins_;
  }

  bool manager::has_plugin (const std::string & a_plugin_name) const
  {
    return _plugins_.find (a_plugin_name) != _plugins_.end ();
  }

  bool manager::has_plugin_type(const std::string& plugin_id) const
  {
    return _plugins_factory_register_.has(plugin_id);
  }

  void manager::unregister_plugin_type(const std::string& plugin_id)
  {
    _plugins_factory_register_.unregistration(plugin_id);
  }

  const geomtools::mapping & manager::get_mapping () const
  {
    return _mapping_;
  }

  geomtools::mapping & manager::grab_mapping ()
  {
    return _mapping_;
  }

  void manager::set_mapping_requested (bool a_)
  {
    _mapping_requested_ = a_;
    return;
  }

  bool manager::is_mapping_requested () const
  {
    return _mapping_requested_;
  }

  bool manager::is_mapping_available () const
  {
    return _mapping_requested_ && _mapping_.get_geom_infos ().size () > 0;
  }

  bool manager::has_services () const
  {
    return _services_;
  }

  void manager::reset_services ()
  {
    _services_ = 0;
    return;
  }

  void manager::set_services (datatools::service_dict_type & services_)
  {
    if (_initialized_) {
      throw std::logic_error ("geomtools::manager::set_services: geometry manager is already initialized !");
    }
    _services_ = &services_;
    return;
  }

  void manager::clear_plugins ()
  {
    _plugins_.clear();
    return;
  }

  // ctor:
  manager::manager ()
  {
    _debug_             = false;
    _verbose_           = false;
    _initialized_       = false;
    _mapping_requested_ = false;
    _world_name_        = geomtools::model_factory::DEFAULT_WORLD_LABEL;
    _services_ = 0;
    _plugins_factory_preload_ = true;
    _plugins_force_initialization_at_load_ = false;
    return;
  }

  // dtor:
  manager::~manager ()
  {
    if (_initialized_)
      {
        reset ();
      }
    clear_plugins ();
    reset_services ();
    return;
  }

  void manager::init (const datatools::properties & config_)
  {
    this->initialize (config_);
    return;
  }

  void manager::initialize (const datatools::properties & config_)
  {
    if (_initialized_) {
      throw std::logic_error ("geomtools::manager::initialize: Already initialized !");
    }
    _pre_init (config_);
    _at_init_ (config_);
    _post_init (config_);
    _initialized_ = true;
    return;
  }

  void manager::reset ()
  {
    if (! _initialized_) {
      throw std::logic_error ("geomtools::manager::reset: Not initialized !");
    }
    _factory_.reset ();
    _id_manager_.reset ();
    _initialized_ = false;
    return;
  }

  void manager::_init_plugins_ (const datatools::properties & config_)
  {
    typedef std::vector<std::string> PFList;
    typedef PFList::iterator PFListIterator;
    std::string conf_file_key("plugins.configuration_files");
    PFList conf_file_list;

    if (config_.has_key(conf_file_key)) {
      config_.fetch(conf_file_key, conf_file_list);
    }

    for (PFListIterator i = conf_file_list.begin();
         i < conf_file_list.end();
         ++i) {
      datatools::fetch_path_with_env(*i);
      datatools::multi_properties mconfig;
      mconfig.read(*i);
      this->load_plugins(mconfig);
    }
    return;
  }

  void manager::_at_init_ (const datatools::properties & config_)
  {
    bool debug         = false;
    bool id_mgr_debug  = false;
    bool factory_debug = false;
    bool factory_devel = false;
    std::string setup_label = _setup_label_;
    std::string setup_description = _setup_description_;
    std::string setup_version = _setup_version_;
    std::string world_name;
    std::string factory_geom_list;
    std::vector<std::string> factory_geom_files;
    std::vector<std::string> factory_preserved_property_prefixes;
    std::string categories_list;

    if (setup_label.empty()) {
      if (config_.has_key ("setup_label")) {
        setup_label = config_.fetch_string ("setup_label");
      } else {
        std::ostringstream message;
        message << "geomtools::manager::_at_init_: "
                << "Missing 'setup_label' property !";
        throw std::logic_error (message.str ());
      }
    }

    if (setup_version.empty()) {
      if (config_.has_key ("setup_version")) {
        setup_version = config_.fetch_string ("setup_version");
      }
    }

    if (setup_description.empty()) {
      if (config_.has_key ("setup_description")) {
        setup_description = config_.fetch_string ("setup_description");
      }
    }

    if (config_.has_flag ("debug")) {
      set_debug (true);
    }

    if (config_.has_flag ("verbose"))  {
      set_verbose (true);
    }

    if (config_.has_flag ("plugins.factory_no_preload")) {
      _plugins_factory_preload_ = false;
    }

    if (config_.has_flag ("plugins.force_initialization_at_load")) {
      _plugins_force_initialization_at_load_ = true;
    }

    if (config_.has_flag ("factory.debug")) {
      factory_debug = true;
    }

    if (config_.has_flag ("factory.devel")) {
      factory_devel = true;
    }

    if (config_.has_flag ("id_mgr.debug")) {
      id_mgr_debug = true;
    }

    if (config_.has_key ("id_mgr.categories_list")) {
      categories_list = config_.fetch_string ("id_mgr.categories_list");
    }
    // else
    //   {
    //     std::ostringstream message;
    //     message << "geomtools::manager::_at_init_: "
    //             << "Missing 'id_mgr.categories_list' property !";
    //     throw std::logic_error (message.str ());
    //   }

    if (config_.has_key ("factory.geom_list")) {
      factory_geom_list = config_.fetch_string ("factory.geom_list");
    }

    if (config_.has_key ("factory.geom_files")) {
      config_.fetch ("factory.geom_files", factory_geom_files);
    }

    if (config_.has_key ("factory.preserved_property_prefixes")) {
      config_.fetch ("factory.preserved_property_prefixes", factory_preserved_property_prefixes);
    }

    if (config_.has_key ("world_name")) {
      world_name = config_.fetch_string ("world_name");
      set_world_name (world_name);
    }

    if (config_.has_flag ("build_mapping")) {
      set_mapping_requested (true);
    }

    if (debug) {
      std::clog << datatools::io::debug
                << "geomtools::manager::_at_init_: Properties are parsed..." << std::endl;
    }

    // initializations:
    set_setup_label (setup_label);
    if (! setup_version.empty ()) {
      set_setup_version (setup_version);
    }
    if (! setup_description.empty ()) {
      set_setup_description (setup_description);
    }


    /************************************************
     * Initialization of the geometry model factory *
     ************************************************/

    _factory_.set_debug (factory_debug);
    if (factory_devel)
      {
        geomtools::model_factory::g_devel = factory_devel;
      }

    /* Property prefixes to be preserved in logical volumes */
    std::vector<std::string> default_factory_preserved_property_prefixes;
    default_factory_preserved_property_prefixes.push_back(visibility::constants::instance().VISIBILITY_PREFIX);
    default_factory_preserved_property_prefixes.push_back(mapping::constants::instance().MAPPING_PREFIX);
    default_factory_preserved_property_prefixes.push_back(material::constants::instance().MATERIAL_PREFIX);
    default_factory_preserved_property_prefixes.push_back(sensitive::constants::instance().SENSITIVE_PREFIX);

    // _factory_.add_property_prefix(visibility::constants::instance().VISIBILITY_PREFIX);
    // _factory_.add_property_prefix(mapping::constants::instance().MAPPING_PREFIX);
    // _factory_.add_property_prefix(material::constants::instance().MATERIAL_PREFIX);
    // _factory_.add_property_prefix(sensitive::constants::instance().SENSITIVE_PREFIX);

    std::vector<std::string> effective_factory_preserved_property_prefixes = default_factory_preserved_property_prefixes;

    for (int i = 0; i < factory_preserved_property_prefixes.size (); i++) {
      const std::string & prefix = factory_preserved_property_prefixes[i];
      if (prefix.empty()) {
        continue;
      }
      if (std::find(effective_factory_preserved_property_prefixes.begin(),
                    effective_factory_preserved_property_prefixes.end(),
                    prefix)
          != effective_factory_preserved_property_prefixes.end()) {
        effective_factory_preserved_property_prefixes.push_back(prefix);
      }
    }

    for (int i = 0; i < effective_factory_preserved_property_prefixes.size (); i++) {
      int level = 1;
      const std::string & prefix = effective_factory_preserved_property_prefixes[i];
      if (prefix.empty ()) {
        std::ostringstream message;
        message << "geomtools::manager::_at_init_: "
                << "Property prefix to be preserved in logical is empty !";
        throw std::logic_error(message.str());
      }
      if (prefix[prefix.length() - 1] != '.') {
        std::ostringstream message;
        message << "geomtools::manager::_at_init_: "
                << "Property prefix to be preserved in logical must end with a dot '.' ("
                << prefix << " ) !";
        throw std::logic_error(message.str());
      }
      _factory_.add_property_prefix(prefix, level);
    }

    if (! factory_geom_list.empty ()) {
      _factory_.load_geom_list (factory_geom_list);
    }

    for (std::vector<std::string>::const_iterator i
           = factory_geom_files.begin ();
         i != factory_geom_files.end ();
         i++) {
      std::string geom_filename = *i;
      datatools::fetch_path_with_env (geom_filename);
      if (is_verbose ())
        {
          std::clog << datatools::io::notice
                    << "geomtools::manager::_at_init_: "
                    << "Loading geometry model configuration file '" << geom_filename << "'... " << std::endl;
        }
      _factory_.load (geom_filename);
      if (is_verbose ()) {
        std::clog << datatools::io::notice
                  << "geomtools::manager::_at_init_: "
                  << "Models were loaded from file '" << geom_filename << "'." << std::endl;
      }
    }

    _factory_.lock ();
    if (_factory_.get_models().size() == 0) {
      std::clog << datatools::io::warning
                << "geomtools::manager::_at_init_: "
                << "No geometry model was constructed for the geometry setup '"
                << _setup_label_ << "' !" << std::endl;

    }
    if (debug) {
      _factory_.tree_dump (std::clog,
                           "Geometry manager's model factory",
                           "DEBUG: geomtools::manager::_at_init_: ");
    }


    /*********************************************
     * Initialization of the geometry ID manager *
     *********************************************/

    if (! categories_list.empty()) {
      datatools::fetch_path_with_env (categories_list);
      _id_manager_.load (categories_list);
    }
    if (id_mgr_debug) {
      _id_manager_.tree_dump (std::clog,
                              "Geometry manager's ID manager:",
                              "DEBUG: geomtools::manager::_at_init_: ");
    }

    // Setup mapping:
    if (is_mapping_requested ()) {
      std::clog << datatools::io::notice << "geomtools::manager::_at_init_: "
                << "The building of the general mapping has been requested..." << std::endl;
      datatools::properties mapping_config;
      geomtools::mapping::extract (config_, mapping_config);
      build_mapping (mapping_config);
    }

    // Plugins initialization :
    std::clog << datatools::io::notice << "geomtools::manager::_at_init_: "
              << "Initializing plugins... please wait..." << std::endl;

    if (_plugins_factory_preload_) {
      std::clog << datatools::io::notice << "geomtools::manager::_at_init_: "
                << "Preloading plugin factory from system register..." << std::endl;
      this->_preload_plugins_global_dict();
    }

    _init_plugins_(config_);

    return;
  }

  const std::string &  manager::get_world_name () const
  {
    return _world_name_;
  }

  void manager::set_world_name (const std::string & a_world_name)
  {
    if (this->is_initialized()) {
      std::ostringstream message;
      message << "geomtools::manager::set_world_name: "
              << "Geometry manager is already initialized !";
      throw std::logic_error(message.str());
    }
    _world_name_ = a_world_name;
    return;
  }

  void manager::build_mapping (const datatools::properties & mapping_config_)
  {
    if (! is_mapping_available ())
      {
        set_mapping_requested (true);
        _mapping_.set_id_manager (_id_manager_);
        std::clog << datatools::io::notice
                  << "geomtools::manager::build_mapping: "
                  << "Configuring mapping..." << std::endl;
        _mapping_.initialize (mapping_config_);
        std::clog << datatools::io::notice
                  << "geomtools::manager::build_mapping: "
                  << "Building general mapping... please wait..." << std::endl;
        _mapping_.build_from (_factory_, _world_name_);
      }
    return;
  }


  bool manager::can_drop_plugin(const std::string& plugin_name_)
  {
    plugins_dict_type::const_iterator found = _plugins_.find(plugin_name_);
    if (found == _plugins_.end()) {
      std::ostringstream message;
      message << "geomtools::manager::can_drop_plugin: "
              << "Plugin '"
              << plugin_name_
              << "' does not exist !";
      throw std::logic_error(message.str());
    }
    return found->second.can_be_dropped();
  }


  void manager::drop_plugin(const std::string& plugin_name_)
  {
    plugins_dict_type::iterator found = _plugins_.find(plugin_name_);
    if (found == _plugins_.end()) {
      std::ostringstream message;
      message << "geomtools::manager::drop_plugin: "
              << "Plugin '"
              << plugin_name_
              << "' does not exist !";
      throw std::logic_error(message.str());
    }

    plugin_entry& entry = found->second;
    if (! found->second.can_be_dropped())
      {
        std::ostringstream message;
        message << "geomtools::manager::drop_plugin: "
                << "Plugin '"
                << plugin_name_
                << "' cannot be dropped because of some external constraints !";
        throw std::logic_error(message.str());
      }

    if (this->is_debug()) {
      std::clog << datatools::io::debug
                << "geomtools::manager::drop: "
                << "Reset & remove plugin '"
                << plugin_name_
                << "' !"
                << std::endl;
    }

    this->_reset_plugin(entry);
    _plugins_.erase(found);
  }

  void manager::print_plugins(std::ostream& out,
                              const std::string& title,
                              const std::string& an_indent) const
  {
    std::string indent;
    if (!an_indent.empty()) indent = an_indent;

    if (!title.empty()) out << title << ":" << std::endl;

    // Plugins:
    {
      size_t sz = _plugins_.size();
      size_t count = 0;
      for (plugins_dict_type::const_iterator it = _plugins_.begin();
           it != _plugins_.end();
           ++it) {
        count++;
        out << indent;
        if (count == sz) {
          out << "`-- ";
        } else {
          out << "|-- ";
        }
        const std::string& plugin_name = it->first;
        const plugin_entry& plugin_record = it->second;

        out << "Name : '" << plugin_name << "' "
            << "Type : '" << plugin_record._id_ << "' ";
        out << '(';
        int count = 0;
        if (plugin_record._status_ & plugin_entry::STATUS_INITIALIZED) {
          out << "initialized";
          count++;
        }
        out << ')';
        out << std::endl;
      }
    }
    return;
  }

  void manager::load_plugin(const std::string & plugin_name_,
                            const std::string & plugin_id_,
                            const datatools::properties& plugin_config_,
                            bool only_lock_)
  {
    if (only_lock_ && this->is_initialized()) {
      std::ostringstream message;
      message << "geomtools::manager::load_plugin: "
              << "Geometry manager is already initialized !";
      throw std::logic_error(message.str());
    }

    this->_load_plugin(plugin_name_, plugin_id_, plugin_config_);
  }

  void manager::load_plugins(const datatools::multi_properties& plugin_mconfig_,
                             bool only_lock_)
  {
    bool debug = this->is_debug();
    if (debug) {
      std::clog << datatools::io::debug
                << "geomtools::manager::load_plugins: "
                << "Entering..."
                << std::endl;
    }

    if (only_lock_ && this->is_initialized()) {
      std::ostringstream message;
      message << "geomtools::manager::load_plugins: "
              << "Geometry manager is already initialized !";
      throw std::logic_error(message.str());
    }


    for (datatools::multi_properties::entries_ordered_col_type::const_iterator i =
           plugin_mconfig_.ordered_entries().begin();
         i != plugin_mconfig_.ordered_entries().end();
         ++i) {
      datatools::multi_properties::entry* mpe = *i;
      const std::string& plugin_name = mpe->get_key();
      const std::string& plugin_id = mpe->get_meta();
      std::clog << datatools::io::notice
                << "geomtools::manager::load_plugins: "
                << "Loading plugin named '"
                << plugin_name << "' "
                << " with ID '"
                << plugin_id << "'..."
                << std::endl;
      this->_load_plugin(plugin_name, plugin_id, mpe->get_properties());
    }
  }


  //----------------------------------------------------------------------
  // Protected Interface Definitions
  //

  void manager::_pre_init (const datatools::properties & config_)
  {
    return;
  }

  void manager::_post_init (const datatools::properties & config_)
  {
    return;
  }


  void manager::_preload_plugins_global_dict()
  {
    bool devel = false;
    if (devel) {
      std::clog << datatools::io::devel
                << "geomtools::manager::_preload_plugins_global_dict: "
                << "Entering..."
                << std::endl;
    }
    _plugins_factory_register_.import(DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(geomtools::manager::base_plugin));
    return;
  }


  void manager::_load_plugin(const std::string& name,
                             const std::string& id,
                             const datatools::properties& config)
  {
    bool debug = this->is_debug();
    if (debug) {
      std::clog << datatools::io::debug
                << "geomtools::manager::_load_plugin: "
                << "Entering..."
                << std::endl;
    }
    if (this->has_plugin(name)) {
      std::ostringstream message;
      message << "geomtools::manager::_load_plugin: "
              << "Plugin '"
              << name
              << "' already exists !";
      throw std::logic_error(message.str());
    }

    {
      // Add a new entry :
      plugin_entry tmp_entry;
      tmp_entry._name_ = name;
      if (debug) {
        std::clog << datatools::io::debug
                  << "geomtools::manager::_load_plugin: "
                  << "Add an entry for plugin '"
                  << name
                  << "'..."
                  << std::endl;
      }
      _plugins_[name] = tmp_entry;
    }
    // fetch a reference on it and update :
    plugin_entry& new_entry = _plugins_.find(name)->second;
    new_entry._id_     = id;
    new_entry._config_ = config;
    new_entry._status_ = plugin_entry::STATUS_BLANK;

    if (debug) {
      std::clog << datatools::io::debug
                << "geomtools::manager::_load_plugin: "
                << "Fetch..."
                << std::endl;
    }

    this->_create_plugin(new_entry);

    if (_plugins_force_initialization_at_load_) {
      this->_initialize_plugin(new_entry);
    }

    if(debug) {
      std::clog << datatools::io::debug
                << "geomtools::manager::_load_plugin: "
                << "Exiting."
                << std::endl;
    }
    return;
  }


  void manager::_create_plugin(plugin_entry& entry) {
    if (!(entry._status_ & plugin_entry::STATUS_CREATED)) {
      if (this->is_debug()) {
        std::clog << datatools::io::debug
                  << "geomtools::manager::_create_plugin: "
                  << "Creating plugin named '"
                  <<  entry._name_
                  << "'..."
                  << std::endl;
      }

      // search for the plugin's label in the factory dictionary:
      if (!_plugins_factory_register_.has(entry._id_)) {
        std::ostringstream message;
        message << "geomtools::manager::_create_plugin: "
                << "Cannot find plugin factory with ID '"
                << entry._id_
                << "' for plugin named '"
                << entry._name_ << "' !";
        throw std::logic_error(message.str());
      }

      typedef geomtools::manager::base_plugin::factory_register_type::factory_type FactoryType;

      const FactoryType& the_factory = _plugins_factory_register_.get(entry._id_);
      base_plugin* ptr = the_factory();
      entry._handle_.reset(ptr);
      entry._status_ |= plugin_entry::STATUS_CREATED;
      if (this->is_debug()) {
        std::clog << datatools::io::debug
                  << "geomtools::manager::_create_plugin: "
                  << "Plugin named '"
                  <<  entry._name_
                  << "' has been created !"
                  << std::endl;
      }
    }
    return;
  }


  void manager::_initialize_plugin(plugin_entry& entry)
  {
    // If not created, first do it :
    if (!(entry._status_ & plugin_entry::STATUS_CREATED)) {
      this->_create_plugin(entry);
    }

    // If not initialized, do it :
    if (!(entry._status_ & plugin_entry::STATUS_INITIALIZED)) {
      if (this->is_debug()) {
        std::clog << datatools::io::debug
                  << "geomtools::manager::_initialize_plugin: "
                  << "Initializing plugin named '"
                  << entry._name_
                  << "'..."
                  << std::endl;
      }
      base_plugin& the_plugin = entry._handle_.grab();
      the_plugin.set_geo_manager(*this);
      if (_services_ == 0) {
        the_plugin.initialize_simple (entry._config_, _plugins_);
      }
      else {
        the_plugin.initialize (entry._config_, _plugins_, *_services_);
      }
      entry._status_ |= plugin_entry::STATUS_INITIALIZED;
      std::clog << datatools::io::notice
                << "geomtools::manager::_initialize_plugin: "
                << "Plugin named '"
                << entry._name_
                << "' has been initialized."
                << std::endl;
    }
    return;
  }


  void manager::_reset_plugin(plugin_entry& entry)
  {
    if (entry._status_ & plugin_entry::STATUS_INITIALIZED) {
      base_plugin& the_plugin = entry._handle_.grab();
      the_plugin.reset();
      entry._status_ ^= plugin_entry::STATUS_INITIALIZED;
    }
    return;
  }

  void manager::tree_dump(std::ostream& out,
                          const std::string& title,
                          const std::string& a_indent,
                          bool a_inherit) const
  {

    std::string indent;
    if (!a_indent.empty()) indent = a_indent;

    if (!title.empty()) out << indent << title << std::endl;

    out << indent << datatools::i_tree_dumpable::tag
        << "Debug     : "
        << _debug_
        << "" << std::endl;

    out << indent << datatools::i_tree_dumpable::tag
        << "Verbose     : "
        << _verbose_
        << "" << std::endl;

    out << indent << datatools::i_tree_dumpable::tag
        << "Initialized     : "
        << _initialized_
        << "" << std::endl;

    out << indent << datatools::i_tree_dumpable::tag
        << "Setup label : '"
        << _setup_label_
        << "'" << std::endl;

    out << indent << datatools::i_tree_dumpable::tag
        << "Setup version : '"
        << _setup_version_
        << "'" << std::endl;

    out << indent << datatools::i_tree_dumpable::tag
        << "Setup description : '"
        << _setup_description_
        << "'" << std::endl;

    out << indent << datatools::i_tree_dumpable::tag
        << "Services dictionnary : "
        << _services_
        << " " << std::endl;

    out << indent << datatools::i_tree_dumpable::tag
        << "Model factory size : "
        << _factory_.get_models().size()
        << " " << std::endl;

    out << indent << datatools::i_tree_dumpable::tag
        << "Mapping requested : "
        << _mapping_requested_
        << " " << std::endl;

    out << indent << datatools::i_tree_dumpable::tag
        << "Mapping size : "
        << _mapping_.get_geom_infos ().size()
        << " " << std::endl;

    out << indent << datatools::i_tree_dumpable::tag
        << "World name : "
        << _world_name_
        << " " << std::endl;

    out << indent << datatools::i_tree_dumpable::tag
        << "Plugins factory preload : "
        << _plugins_factory_preload_
        << " " << std::endl;

    out << indent << datatools::i_tree_dumpable::tag
        << "Plugins force initialization at load : "
        << _plugins_force_initialization_at_load_
        << " " << std::endl;

    // out << indent << datatools::i_tree_dumpable::tag
    //     << "Plugins facorty : "
    //     << _plugins_factory_register_.size()
    //     << " " << std::endl;

    out << indent << datatools::i_tree_dumpable::inherit_tag(a_inherit)
        << "Plugins : "
        << _plugins_.size()
        << " " << std::endl;

    return;
  }

  const manager::base_plugin::factory_register_type &
  manager::get_plugins_factory_register()
  {
    return _plugins_factory_register_;
  }

  //----------------------------------------------------------------------
  // Private Interface Definitions

  /*
    void manager::_set_plugins_factory_preload_(bool preload_)
    {
    _plugins_factory_preload_ = preload_;
    if (preload_) {
    this->_preload_plugins_global_dict();
    }
    return;
    }
  */

} // end of namespace geomtools

/****************************************************************/
// OCD support for class '::geomtools::manager' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::geomtools::manager,ocd_)
{
  ocd_.set_class_name("geomtools::manager");
  ocd_.set_class_description("A virtual geometry manager");
  ocd_.set_class_library("geomtools");
  ocd_.set_class_documentation("The geometry manager is the core class of the 'geomtools'   \n"
                               "framework. It hosts four components :                       \n"
                               "a) A factory of geometry models, which is responsible of the\n"
                               "   construction of the full hierarchy of geometry volumes,  \n"
                               "   compatible with the GDML hierarchical scheme (and thus   \n"
                               "   Geant4).                                                 \n"
                               "b) A manager for geometry IDs (GID), which describes the    \n"
                               "   relationships between some geometry volumes in the geometry\n"
                               "   hierarchy and the addressing scheme of such volumes      \n"
                               "   based on geometry IDs ('geomtools::geom_id' class).      \n"
                               "c) A mapping object which is responsible of a lookup-table  \n"
                               "   that allows to access fundamental informations about some\n"
                               "   volume in the setup, given its unique GID and obeying the\n"
                               "   numbering scheme setup by the GID manager.               \n"
                               "d) A manager for geometry plugins which allows to dynamically\n"
                               "   add new geometry-related functionnalities to the manager.\n"
                               );
{
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("debug")
      .set_terse_description("The debug flag")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("The debug flag activates some debug printing.    \n"
                            "Not recommended for a production run.            \n"
                            "Example :                                        \n"
                            "  |                                              \n"
                            "  | debug : boolean = 0                          \n"
                            "  |                                              \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("verbose")
      .set_terse_description("The verbose flag")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("The verbose flag activates some additionnal printing.\n"
                            "Not recommended for a production run.            \n"
                            "Example :                                        \n"
                            "  |                                              \n"
                            "  | verbose : boolean = 0                        \n"
                            "  |                                              \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("setup_label")
      .set_terse_description("The label that identifies the virtual geometry setup")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_complex_triggering_conditions(true)
      .set_long_description("Superseded by a former call of :                  \n"
                            "  geomtools::manager::set_setup_label(...)        \n"
                            "Example :                                      \n"
                            "  |                                            \n"
                            "  | setup_label : string = \"test_setup\"      \n"
                            "  |                                            \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("setup_version")
      .set_terse_description("The version string of the virtual geometry setup")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("Superseded by a former call of :                  \n"
                            "  geomtools::manager::set_setup_version(...)      \n"
                            "Example :                                      \n"
                            "  |                                            \n"
                            "  | setup_version : string = \"0.1\"           \n"
                            "  |                                            \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("setup_description")
      .set_terse_description("The description string of the virtual geometry setup")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("Superseded by a former call of :                       \n"
                            "  geomtools::manager::set_setup_description(...)       \n"
                            "Example :                                           \n"
                            "  |                                                 \n"
                            "  | setup_description : string = \"My test setup\"  \n"
                            "  |                                                 \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("plugins.factory_no_preload")
      .set_terse_description("The flag to inhibit the preloading of plugins' factories")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("This flag inhibits the preloading of plugins' factories  \n"
                            "from the plugins system register.                        \n"
                            "Default is false.                                        \n"
                            "Not recommended for a production run.                    \n"
                            "Example :                                                \n"
                            "  |                                                      \n"
                            "  | plugins.factory_no_preload : boolean = 0             \n"
                            "  |                                                      \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("plugins.force_initialization_at_load")
      .set_terse_description("The flag to force the initialization of plugins at load stage.")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("This flag triggers the initialization of plugins as soons as\n"
                            "they are loaded in the geometry manager's plugin system.    \n"
                            "Default is false to activate the dedicated resources of a   \n"
                            "given plugin if and only if the plugin is used.             \n"
                            "Not recommended for a production run.                       \n"
                            "Example :                                                   \n"
                            "  |                                                         \n"
                            "  | plugins.force_initialization_at_load : boolean = 0      \n"
                            "  |                                                         \n"
                            )
      ;
  }


  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("factory.debug")
      .set_terse_description("The debug flag of the geometry model factory")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("This flag activates some debug printings from    \n"
                            "the embeded geometry model factory.              \n"
                            "Not recommended for a production run.            \n"
                            "Example :                                        \n"
                            "  |                                              \n"
                            "  | factory.debug : boolean = 0                  \n"
                            "  |                                              \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("factory.devel")
      .set_terse_description("The devel flag of the geometry model factory")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("This flag activates some cryptic printings from\n"
                            "the embeded geometry model factory.              \n"
                            "Not recommended for a production run.            \n"
                            "Example :                                        \n"
                            "  |                                              \n"
                            "  | factory.debug : boolean = 0                  \n"
                            "  |                                              \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("factory.geom_list")
      .set_terse_description("A file that contains a list of geometry definition files")
      .set_traits(datatools::TYPE_STRING)
      .set_path(true)
      .set_mandatory(false)
      .set_long_description("The 'geometry list' file stores a list of filenames.        \n"
                            "Each of these files contains rules that define geometry     \n"
                            "models to be instantiated by the embeded geometry model     \n"
                            "factory.                                                    \n"
                            "Example:                                                    \n"
                            "  |                                                         \n"
                            "  | factory.geom_list : string as path = \\                 \n"
                            "  |   \"${CONFIG_REPOSITORY_DIR}/geom/geom_files.lis\"      \n"
                            "  |                                                         \n"
                            "where the '.../geom/geom_files.lis' file has the following  \n"
                            "format :                                                    \n"
                            "  |                                                         \n"
                            "  |  ${CONFIG_REPOSITORY_DIR}/geom/models/sources.geom      \n"
                            "  |  ${CONFIG_REPOSITORY_DIR}/geom/models/detectors.geom    \n"
                            "  |  ${CONFIG_REPOSITORY_DIR}/geom/models/shielding.geom    \n"
                            "  |  ${CONFIG_REPOSITORY_DIR}/geom/models/vessel.geom       \n"
                            "  |  ${CONFIG_REPOSITORY_DIR}/geom/models/lab.geom          \n"
                            "  |  ${CONFIG_REPOSITORY_DIR}/geom/models/world.geom        \n"
                            "  |                                                         \n"
                            "  |                                                         \n"
                            "The order of the geometry files (*.geom) is critical        \n"
                            "because some geometry models may depend on other ones       \n"
                            "which should thus be defined *before* their dependers.      \n"
                            "See OCD support for 'mygsl::model_factory' for further      \n"
                            "informations about the configuration of geometry models.    \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("factory.geom_files")
      .set_terse_description("A list of files that contains geometry models' definitions")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_path(true)
      .set_mandatory(false)
      .set_long_description("Each of the addressed files contains rules that define geometry  \n"
                            "models to be instantiated by the embeded geometry model          \n"
                            "factory.                                                         \n"
                            "Example:                                                         \n"
                            "  |                                                              \n"
                            "  | factory.geom_files : string[6] as path =                  \\ \n"
                            "  |   \"${CONFIG_REPOSITORY_DIR}/geom/models/sources.geom\"   \\ \n"
                            "  |   \"${CONFIG_REPOSITORY_DIR}/geom/models/detectors.geom\" \\ \n"
                            "  |   \"${CONFIG_REPOSITORY_DIR}/geom/models/shielding.geom\" \\ \n"
                            "  |   \"${CONFIG_REPOSITORY_DIR}/geom/models/vessel.geom\"    \\ \n"
                            "  |   \"${CONFIG_REPOSITORY_DIR}/geom/models/lab.geom\"       \\ \n"
                            "  |   \"${CONFIG_REPOSITORY_DIR}/geom/models/world.geom\"        \n"
                            "  |                                                              \n"
                            "  |                                                              \n"
                            "The order of the geometry files (*.geom) is critical             \n"
                            "because some geometry models may depend on other ones            \n"
                            "which should thus be defined *before* their dependers.           \n"
                            "See OCD support for 'mygsl::model_factory' for further           \n"
                            "informations about the configuration of geometry models.         \n"
                            )
      ;
  }


  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("factory.preserved_property_prefixes")
      .set_terse_description("A list of prefixes for property key that are stored in the geometry logical volumes")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_mandatory(false)
      .set_long_description("All auxiliary properties found in a geometry model definition  \n"
                            "file that starts with one of these prefixes will be preserved  \n"
                            "and stored in the geometry model constructed by the embeded    \n"
                            "model factory. Such properties can then be used by external    \n"
                            "algorithms and/or plugins to extract additional informations   \n"
                            "and/or configuration parameters.                               \n"
                            "A property prefix must ends with the dot  '.' character.       \n"
                            "Default behaviour already preserved the properties with the    \n"
                            "following prefixes :                                           \n"
                            "  * \"visibility\" : Properties for rendering/display          \n"
                            "                     (color, hidden...)                        \n"
                            "  * \"mapping\"    : Properties for automatic computing        \n"
                            "                     of the geometry IDs (GID) associated to   \n"
                            "                     the geometry volumes in the setup.        \n"
                            "  * \"material\"   : Properties related to the materials       \n"
                            "                     the geometry volumes are made of.         \n"
                            "                     Used by the material plugin.              \n"
                            "  * \"sensitive\"  : Properties related to the definition      \n"
                            "                     of 'sensitive' geometry volumes.          \n"
                            "                     Such information can be used to associate \n"
                            "                     a given logical volume (model) to some    \n"
                            "                     sensitive detector within Geant4 (see     \n"
                            "                     the 'mctools' library and its 'mctools_g4'\n"
                            "                     DLL plugin.                               \n"
                            "Example:                                                       \n"
                            "  |                                                            \n"
                            "  | factory.preserved_property_prefixes : string[3] =  \\      \n"
                            "  |   \"display.\"         \\                                  \n"
                            "  |   \"radioactivity.\"   \\                                  \n"
                            "  |   \"magnetization.\"                                       \n"
                            "  |                                                            \n"
                            )
      ;
  }


  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("world_name")
      .set_terse_description("A name of the top level geometry volume")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("This is the name used by the geometry system to identify\n"
                            "the top level geometry logical volume that contains the \n"
                            "full virtual geometry setup.                            \n"
                            "Default value is: \"world\"                             \n"
                            "Example:                                                \n"
                            "  |                                                     \n"
                            "  | world_name : string =  \"world\"                    \n"
                            "  |                                                     \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("id_mgr.debug")
      .set_terse_description("The debug flag of the geometry ID (GID) manager")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("This flag activates some debug printings from    \n"
                            "the embeded geometry ID manager.                 \n"
                            "Not recommended for a production run.            \n"
                            "Example :                                        \n"
                            "  |                                              \n"
                            "  | id_mgr.debug : boolean = 0                   \n"
                            "  |                                              \n"
                            )
      ;
  }


  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("id_mgr.categories_list")
      .set_terse_description("A list of configuration files that define geometry categories")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_path(true)
      .set_mandatory(false)
      .set_long_description(
                            "A list of filenames from where the geometry categories      \n"
                            "used by the geometry ID manager are defined.                \n"
                            "Example:                                                    \n"
                            "  |                                                         \n"
                            "  | id_mgr.categories_list : string[1] as path = \\         \n"
                            "  |   \"${CONFIG_REPOSITORY_DIR}/geom/geom_categories.lis\" \n"
                            "  |                                                         \n"
                            "The target files must use the format of the                 \n"
                            "'datatools::multi_properties' class.                        \n"
                            "The loading order of the files is critical                  \n"
                            "because some geometry categories may depend on other ones   \n"
                            "which should thus be defined *before* their dependers.      \n"
                            "See OCD support for 'mygsl::id_mgr' for further             \n"
                            "informations about the configuration of geometry categories.\n"
                            )
      ;
  }


  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("build_mapping")
      .set_terse_description("This flag triggers the generation of a map of geometry IDs")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("This flag triggers the generation of the map of geometry IDs         \n"
                            "associated to geometry volumes. This operation is performed          \n"
                            "by the embeded geometry mapping object that hosts a dictionnary      \n"
                            "of 'geometry information' objects ('geomtools::geom_info' class).    \n"
                            "Each 'geometry information' object describes an unique geometry      \n"
                            "volume (placement, shape, auxiliary properties...), being            \n"
                            "unambiguously associated to an unique key : the 'geometry ID'        \n"
                            "(GID, 'geomtools::geom_id' class).                                   \n"
                            "Example :                                                            \n"
                            "  |                                                                  \n"
                            "  | build_mapping : boolean = 1                                      \n"
                            "  |                                                                  \n"
                            "All properties starting with the \"mapping.\" prefix are then        \n"
                            "transmitted to initialize the the embedded 'mapping' object.         \n"
                            "Example :                                                            \n"
                            "  |                                                                  \n"
                            "  | mapping.debug : boolean = 0                                      \n"
                            "  | mapping.max_depth : integer = 100                                \n"
                            "  | mapping.no_world_mapping: boolean = 0                            \n"
                            "  | #mapping.only_categories : string [1] = \"scin_block.gc\"        \n"
                            "  | mapping.excluded_categories : string [2] = \"bolt.gc\" \"screw\" \n"
                            "  |                                                                  \n"
                            "See OCD support for 'mygsl::mapping' for further                     \n"
                            "informations about the configuration of geometry categories.         \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("plugins.configuration_files")
      .set_terse_description("A list of files that contains geometry plugins' definitions")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_path(true)
      .set_mandatory(false)
      .set_long_description("Each of the addressed files contains rules that define geometry       \n"
                            "plugins. The files use the format of the 'datatools::multi_properties'\n"
                            "class. The filenames may contain environment variables.               \n"
                            "Example :                                                             \n"
                            "  |                                                                   \n"
                            "  | plugins.configuration_files : string[2] as path =      \\         \n"
                            "  |   \"${CONFIG_REPOSITORY_DIR}/geom/mapping_plugins.lis\"  \\       \n"
                            "  |   \"${CONFIG_REPOSITORY_DIR}/geom/mag_field_plugin.lis\"          \n"
                            "  |                                                                   \n"
                            "Here some plugins with embeded dedicated mappingonbjects are defined  \n"
                            "in the 'mapping_plugins.lis' file :                                   \n"
                            "  |                                                                   \n"
                            "  | #@description List of geometry plugins                            \n"
                            "  | #@key_label   \"name\"                                            \n"
                            "  | #@meta_label  \"type\"                                            \n"
                            "  |                                                                   \n"
                            "  | [name=\"mapping_no_screw\" type=\"geomtools::mapping_plugin\"]    \n"
                            "  | mapping.excluded_categories : string [1] = \"screw.gc\"           \n"
                            "  |                                                                   \n"
                            "The 'geomtools::mapping_plugin' is provided by the 'geomtools'        \n"
                            "library.                                                              \n"
                            )
      ;
  }

  ocd_.set_configuration_hints("A geometry manager is configured through a configuration file that  \n"
                               "obeys the format of 'datatools::properties' setup file.             \n"
                               "Example : to be done.                                               \n"
                               );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::geomtools::manager,"geomtools::manager")

// end of manager.cc
