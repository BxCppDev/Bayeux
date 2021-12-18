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

// Ourselves:
#include <geomtools/manager.h>

// Standard library:
#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <list>
#include <map>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/units.h>
#include <datatools/version_id.h>
#include <datatools/version_check.h>
#include <datatools/exception.h>

// This project:
#include <geomtools/visibility.h>
#include <geomtools/sensitive.h>
#include <geomtools/material.h>
#include <geomtools/mapping.h>
#include <geomtools/mapping_plugin.h>

namespace geomtools {

  DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(manager::base_plugin,
                                                   "geomtools::manager::base_plugin/__system__")

  datatools::logger::priority manager::base_plugin::get_logging_priority() const
  {
    return _logging;
  }

  void manager::base_plugin::set_logging_priority(datatools::logger::priority lp_)
  {
    _logging = lp_;
    return;
  }

  manager::base_plugin::base_plugin()
  {
    _logging = datatools::logger::PRIO_WARNING;
    _geo_mgr_ = 0;
    return;
  }

  manager::base_plugin::~base_plugin()
  {
    _geo_mgr_ = 0;
    _geom_setup_requirements_.clear();
    return;
  }

  void manager::base_plugin::set_geo_manager(manager & geo_mgr_)
  {
    DT_THROW_IF (is_initialized(),
                 std::logic_error,
                 "Cannot set the geometry manager ! Plugin is already initialized !");
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

  bool manager::base_plugin::_has_geom_setup_requirement() const
  {
    return ! _geom_setup_requirements_.empty();
  }

  void manager::base_plugin::_check_geom_setup_requirement() const
  {
    DT_THROW_IF(_geo_mgr_ == 0, std::logic_error,
                "The geometry manager has not been set !");
    if (_has_geom_setup_requirement()) {
      const std::string & geom_mgr_setup_label = _geo_mgr_->get_setup_label ();
      const std::string & geom_mgr_setup_version = _geo_mgr_->get_setup_version ();
      datatools::version_id geom_mgr_setup_vid;
      {
        std::istringstream iss (geom_mgr_setup_version);
        iss >> geom_mgr_setup_vid;
      }
      for (size_t i = 0; i < _geom_setup_requirements_.size(); i++)
        {
          if (datatools::validate_version (geom_mgr_setup_label,
                                           geom_mgr_setup_vid,
                                           _geom_setup_requirements_[i]))
            {
              DT_LOG_DEBUG(get_logging_priority(), "Geometry setup is validated");
              return;
            }
        }
      DT_THROW_IF(true, std::logic_error,
                  "Geometry manager setup label '" << _geo_mgr_->get_setup_label ()
                  << "' with version '" << geom_mgr_setup_vid
                  << "' does not match one of the requested setup requirement");
    }
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

  void manager::base_plugin::_basic_initialize(const datatools::properties & config_)
  {
    set_logging_priority(datatools::logger::extract_logging_configuration(config_,datatools::logger::PRIO_WARNING));

    if (config_.has_key("geometry.setup_requirement"))
      {
        DT_LOG_NOTICE(get_logging_priority(),
                      "Loading 'geometry.setup_requirement' rules...");
        config_.fetch("geometry.setup_requirement", _geom_setup_requirements_);
        _check_geom_setup_requirement();
      }
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
    DT_THROW_IF (is_created(),
                 std::logic_error,
                 "Geometry plugin '" << name_ << "' is already created !");
    DT_THROW_IF (name_.empty(),
                 std::logic_error,
                 "Geometry plugin cannot have an empty name !");
    _name_ = name_;
  }

  const std::string & manager::plugin_entry::get_name() const
  {
    return _name_;
  }

  const std::string & manager::plugin_entry::get_id() const
  {
    return _id_;
  }

  void manager::plugin_entry::set_description(const std::string& description_)
  {
    DT_THROW_IF (is_created(),
                 std::logic_error,
                 "Plugin is already created !");
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
    DT_THROW_IF (is_initialized(),
                 std::logic_error,
                 "Plugin is already initialized !");
    _config_ = config_;
    return;
  }

  /// Grab the configuration
  datatools::properties & manager::plugin_entry::grab_config ()
  {
    DT_THROW_IF (is_initialized(),
                 std::logic_error,
                 "Plugin is already initialized !");
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

  void manager::plugin_entry::tree_dump(std::ostream& out_,
                                        const std::string& title_,
                                        const std::string& indent_,
                                        bool inherit_) const
  {

    std::string indent;
    if (!indent_.empty()) indent = indent_;

    if (!title_.empty()) out_ << indent << title_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
        << "Plugin name     : '"
        << _name_
        << "'" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
        << "Plugin ID       : '"
        << _id_
        << "'" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
        << "Plugin description : '"
        << _description_
        << "'" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
        << "Can be dropped   : "
        << can_be_dropped() << std::endl;

    out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
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
        out_ << ' ' << '('
            << status_info.str()
            << ')';
      }
    }
    out_ << std::endl;
  }

  /*****************************************************/

  void manager::set_logging_priority(datatools::logger::priority logging_priority_)
  {
    _logging = logging_priority_;
    return;
  }

  datatools::logger::priority manager::get_logging_priority() const
  {
    return _logging;
  }

  bool manager::is_debug () const
  {
    return _logging >= datatools::logger::PRIO_DEBUG;
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
    if (! sv_.empty())  {
      datatools::version_id geom_mgr_setup_vid;
      std::istringstream iss (sv_);
      iss >> geom_mgr_setup_vid;
      DT_THROW_IF (! iss,
                   std::logic_error,
                   "Invalid version format '" << sv_ << "' !");
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

  bool manager::is_verbose () const
  {
    return _logging >= datatools::logger::PRIO_NOTICE;
  }

  bool manager::is_initialized () const
  {
    return _initialized_;
  }

  const geomtools::shape_factory & manager::get_shape_factory () const
  {
    return _shape_factory_;
  }

  geomtools::shape_factory & manager::grab_shape_factory ()
  {
    return _shape_factory_;
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

  bool manager::has_plugin (const std::string & plugin_name_) const
  {
    return _plugins_.find(plugin_name_) != _plugins_.end ();
  }

  bool manager::has_plugin_type(const std::string& plugin_id_) const
  {
    return _plugins_factory_register_.has(plugin_id_);
  }

  void manager::unregister_plugin_type(const std::string& plugin_id_)
  {
    _plugins_factory_register_.unregistration(plugin_id_);
    return;
  }

  const geomtools::mapping & manager::get_mapping(const std::string & mapping_name_) const
  {
    if (! mapping_name_.empty()) {
      DT_THROW_IF(! is_plugin_a<mapping_plugin>(mapping_name_),
                  std::logic_error,
                  "There is no mapping plugin named '" << mapping_name_ << "' !");
      const mapping_plugin & mp = get_plugin<mapping_plugin>(mapping_name_);
      return mp.get_mapping();
    }
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

  void manager::set_services(datatools::service_dict_type & services_)
  {
    DT_THROW_IF (_initialized_,
                 std::logic_error,
                 "Geometry manager is already initialized !");
    _services_ = &services_;
    return;
  }

  void manager::clear_plugins ()
  {
    _plugins_.clear();
    return;
  }

  manager::manager ()
  {
    _logging            = datatools::logger::PRIO_WARNING;
    _initialized_       = false;
    _mapping_requested_ = false;
    _world_name_        = geomtools::model_factory::default_world_label();
    _services_ = 0;
    _plugins_factory_preload_ = true;
    _plugins_force_initialization_at_load_ = false;
    return;
  }

  manager::~manager()
  {
    if (_initialized_) {
      reset();
    }
    clear_plugins();
    reset_services();
    return;
  }

  void manager::init (const datatools::properties & config_)
  {
    this->initialize(config_);
    return;
  }

  void manager::initialize (const datatools::properties & config_)
  {
    DT_LOG_TRACE_ENTERING(_logging);
    DT_THROW_IF(_initialized_,
                std::logic_error,
                "Geometry manager is already initialized !");
    DT_LOG_TRACE(_logging, " Pre-init...");
    _pre_init(config_);
    DT_LOG_TRACE(_logging, " At-init...");
    _at_init_(config_);
    DT_LOG_TRACE(_logging, " Post-init...");
    _post_init(config_);
    _initialized_ = true;
    DT_LOG_TRACE_EXITING(_logging);
    return;
  }

  void manager::reset ()
  {
    DT_LOG_TRACE_ENTERING(_logging);
    DT_THROW_IF (! _initialized_,
                 std::logic_error,
                 "Geometry manager is not initialized ! Cannot reset !");
    _factory_.reset();
    _shape_factory_.reset();
    _id_manager_.reset();
    _initialized_ = false;
    DT_LOG_TRACE_EXITING(_logging);
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
    std::string setup_label = _setup_label_;
    std::string setup_description = _setup_description_;
    std::string setup_version = _setup_version_;
    std::string world_name;
    // std::string factory_geom_list;
    std::vector<std::string> factory_geom_lists;
    std::vector<std::string> factory_geom_files;
    std::vector<std::string> factory_preserved_property_prefixes;
    std::vector<std::string> categories_lists;

    if (setup_label.empty()) {
      DT_THROW_IF (! config_.has_key ("setup_label"),
                   std::logic_error,
                   "Missing 'setup_label' property !");
      setup_label = config_.fetch_string ("setup_label");
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

    {
      // Logging priority:
      datatools::logger::priority lp
        = datatools::logger::extract_logging_configuration (config_,
                                                            datatools::logger::PRIO_WARNING);
      DT_THROW_IF(lp == datatools::logger::PRIO_UNDEFINED,
                  std::logic_error,
                  "Invalid logging priority level for geometry manager !");
      set_logging_priority (lp);
    }

    if (config_.has_flag ("plugins.factory_no_preload")) {
      _plugins_factory_preload_ = false;
    }

    if (config_.has_flag ("plugins.force_initialization_at_load")) {
      _plugins_force_initialization_at_load_ = true;
    }

    datatools::properties id_mgr_config;
    config_.export_and_rename_starting_with(id_mgr_config, "id_mgr.", "");

    // if (config_.has_flag ("id_mgr.logging.priority")) {
    //   const std::string factory_priority = config_.fetch_string ("id_mgr.logging.priority");
    //   datatools::logger::priority lp = datatools::logger::get_priority (factory_priority);
    //   DT_THROW_IF (lp == datatools::logger::PRIO_UNDEFINED,
    //                std::logic_error,
    //                "Invalid logging priority label '" << factory_priority << "' !");
    //   _id_manager_.set_logging_priority (lp);
    // }

    // if (config_.has_key ("id_mgr.categories_list")) {
    //   std::string categories_list = config_.fetch_string("id_mgr.categories_list");
    //   categories_lists.push_back(categories_list);
    // } else if (config_.has_key ("id_mgr.categories_lists")) {
    //   config_.fetch("id_mgr.categories_lists", categories_lists);
    // }

    // Initialize the shape factory:
    datatools::properties shape_factory_config;
    config_.export_and_rename_starting_with(shape_factory_config,
                                            "shape_factory.",
                                            "");
    _shape_factory_.initialize(shape_factory_config);

    if (config_.has_key ("factory.logging.priority")) {
      const std::string factory_priority = config_.fetch_string ("factory.logging.priority");
      datatools::logger::priority lp = datatools::logger::get_priority (factory_priority);
      DT_THROW_IF (lp == datatools::logger::PRIO_UNDEFINED,
                   std::logic_error,
                   "Invalid logging priority label '" << factory_priority << "' !");
      _factory_.set_logging_priority (lp);
    }

    // Pass the shape factory to the model factory:
    _factory_.set_shape_factory(_shape_factory_);

    // Load on list file of geometry models definition files:
    if (config_.has_key ("factory.geom_list")) {
      std::string factory_geom_list = config_.fetch_string ("factory.geom_list");
      factory_geom_lists.push_back(factory_geom_list);
    } else {
      // Load several list files of geometry models definition files:
      if (config_.has_key ("factory.geom_lists")) {
        config_.fetch("factory.geom_lists", factory_geom_lists);
      }
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

    if (is_mapping_requested()) {
      // List of files where to find external mapping rules associated to geometry models:
      std::vector<std::string> mapping_rules_files;
      if (config_.has_key ("mapping.external_rules")) {
        config_.fetch("mapping.external_rules", mapping_rules_files);
      } else if (config_.has_key ("external_mapping_rules")) {
        DT_LOG_WARNING(datatools::logger::PRIO_WARNING,
                       "Property '" << "external_mapping_rules"
                       << "' is deprecated. Please use '" << "mapping.external_rules" << "'!");
        config_.fetch("external_mapping_rules", mapping_rules_files);
      }
      if (mapping_rules_files.size()) {
        _external_mapping_rules_.set_key_label("model");
        _external_mapping_rules_.set_meta_label("policy");
        for (size_t i = 0; i < mapping_rules_files.size(); i++) {
          std::string mrfile = mapping_rules_files[i];
          datatools::fetch_path_with_env(mrfile);
          _external_mapping_rules_.read(mrfile);
        }
        // _external_mapping_rules_.tree_dump(std::cerr, "Mapping rules: ", "DEVEL: ");
      }
    }

    DT_LOG_DEBUG(_logging, "Properties are parsed...");

    // initializations:
    set_setup_label(setup_label);
    if (! setup_version.empty()) {
      set_setup_version(setup_version);
    }
    if (! setup_description.empty()) {
      set_setup_description(setup_description);
    }

    /************************************************
     * Initialization of the geometry model factory *
     ************************************************/
    DT_LOG_NOTICE(_logging, "Initialization of the geometry model factory...");

    /* Property prefixes to be preserved in logical volumes */
    std::vector<std::string> default_factory_preserved_property_prefixes;
    default_factory_preserved_property_prefixes.push_back(visibility::constants::instance().VISIBILITY_PREFIX);
    default_factory_preserved_property_prefixes.push_back(mapping::constants::instance().MAPPING_PREFIX);
    default_factory_preserved_property_prefixes.push_back(material::material_prefix());
    default_factory_preserved_property_prefixes.push_back(sensitive::constants::instance().SENSITIVE_PREFIX);

    std::vector<std::string> effective_factory_preserved_property_prefixes
      = default_factory_preserved_property_prefixes;

    for (size_t i = 0; i < factory_preserved_property_prefixes.size(); i++) {
      const std::string & prefix = factory_preserved_property_prefixes[i];
      if (prefix.empty()) {
        continue;
      }
      if (std::find(effective_factory_preserved_property_prefixes.begin(),
                    effective_factory_preserved_property_prefixes.end(),
                    prefix) == effective_factory_preserved_property_prefixes.end()) {
        effective_factory_preserved_property_prefixes.push_back(prefix);
      }
    }

    for (size_t i = 0; i < effective_factory_preserved_property_prefixes.size(); i++) {
      const std::string & prefix = effective_factory_preserved_property_prefixes[i];
      DT_THROW_IF(prefix.empty(),
                  std::logic_error,
                  "Property prefix to be preserved in logical is empty !");
      DT_THROW_IF(prefix[prefix.length() - 1] != '.',
                  std::logic_error,
                  "Property prefix to be preserved in logical must end with a dot '.' ("
                  << prefix << " ) !");
      DT_LOG_NOTICE(_logging,
                   "Property prefix '" << prefix << "' will be exported by the model factory");
      _factory_.add_property_prefix(prefix);
    }

    for (std::vector<std::string>::const_iterator i
           = factory_geom_lists.begin();
         i != factory_geom_lists.end();
         i++) {
      std::string geom_list = *i;
      datatools::fetch_path_with_env (geom_list);
      DT_LOG_NOTICE(_logging,
                    "Loading list of geometry model configuration files '" << geom_list << "'... ");
      _factory_.load_geom_list(geom_list);
      DT_LOG_NOTICE(_logging,
                    "Lists were loaded from file '" << geom_list << "'.");
    }

    for (std::vector<std::string>::const_iterator i
           = factory_geom_files.begin();
         i != factory_geom_files.end();
         i++) {
      std::string geom_filename = *i;
      datatools::fetch_path_with_env (geom_filename);
      DT_LOG_NOTICE(_logging,
                    "Loading geometry model configuration file '" << geom_filename << "'... ");
      _factory_.load (geom_filename);
      DT_LOG_NOTICE(_logging,
                    "Models were loaded from file '" << geom_filename << "'.");
    }

    if (is_mapping_requested()) {
      // Push external mapping rules in geometry models configuration section:
      if (! _external_mapping_rules_.empty()) {
        DT_LOG_NOTICE(_logging, "Loading external mapping rules...");
        std::vector<std::string> model_names;
        _external_mapping_rules_.keys(model_names);
        datatools::multi_properties & factory_mp = _factory_.grab_mp();
        for (int i = 0; i < (int) model_names.size(); i++) {
          const std::string & model_name = model_names[i];
          const datatools::multi_properties::entry & model_mapping_entry = _external_mapping_rules_.get(model_name);
          std::string model_mapping_policy = model_mapping_entry.get_meta();
          if (model_mapping_policy == "merge") {
             DT_THROW(std::logic_error, "Policy ('" << model_mapping_policy << "') for external mapping rules is not supported yet!");
          } else if (model_mapping_policy == "ignore") {
             DT_THROW(std::logic_error, "Policy ('" << model_mapping_policy << "') for external mapping rules is not supported yet!");
          } else if (model_mapping_policy == "supersede") {
          } else if (model_mapping_policy.empty()) {
            // Default value:
            model_mapping_policy = "supersede";
          } else {
            DT_THROW(std::logic_error, "Unknown policy ('" << model_mapping_policy << "') for external mapping rules!");
          }
          const datatools::properties & model_mapping_rules = _external_mapping_rules_.get_section(model_name);
          // model_mapping_rules.tree_dump(std::cerr,
          //                               std::string("External mapping rules for geometry model '") + model_name + "': ",
          //                               "DEVEL: ");
          if (factory_mp.has_section(model_name)) {
            datatools::properties & factory_model_section = factory_mp.grab_section(model_name);
            if (model_mapping_policy == "supersede") {
              // First delete former 'mapping' properties:
              factory_model_section.erase_all_starting_with(mapping::constants::instance().MAPPING_PREFIX);
            }
            // Then install the ones for external mapping rules:
            model_mapping_rules.export_starting_with(factory_model_section,
                                                     mapping::constants::instance().MAPPING_PREFIX);
            /*
            // What to do with "ignore" and "merge" policy:
            if (model_mapping_policy == "ignore") {
              model_mapping_rules.export_starting_with(factory_model_section,
              mapping::constants::instance().MAPPING_PREFIX);
            }
            if (model_mapping_policy == "merge") {
            }
            */
            DT_LOG_NOTICE(_logging, "Geometry model '" << model_name << "' is now enriched with external mapping rules...");
          } else {
            DT_LOG_WARNING(_logging, "Ignoring external mapping rule for non-existing model '" << model_name << "'!");
          }
        }
      }
    }

    {
      // Build the geometry models:
      _factory_.lock();
      DT_THROW_IF(_factory_.get_models().size() == 0,
                  std::logic_error,
                  "No geometry model was constructed for the geometry setup '"
                  << _setup_label_ << "' !");
      DT_LOG_DEBUG(_logging,"Geometry manager's model factory : ");
      if (_factory_.get_logging_priority () == datatools::logger::PRIO_DEBUG) {
        DT_LOG_DEBUG (datatools::logger::PRIO_DEBUG,
                      "Geometry model factory:");
        _factory_.tree_dump(std::clog);
      }
    }

    /*********************************************
     * Initialization of the geometry ID manager *
     *********************************************/
    DT_LOG_NOTICE(_logging, "Initialization of the geometry ID manager...");
    _id_manager_.initialize(id_mgr_config);
    // for (size_t i(0); i < categories_lists.size(); ++i) {
    //   std::string categories_list = categories_lists[i];
    //   datatools::fetch_path_with_env(categories_list);
    //   _id_manager_.load(categories_list);
    // }
    if (datatools::logger::is_debug(_id_manager_.get_logging_priority())) {
      DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "Geometry manager's ID manager:");
      _id_manager_.tree_dump (std::clog);
    }

    DT_LOG_NOTICE(_logging, "Initialization of the geometry mapping...");
    // Setup mapping:
    if (is_mapping_requested ()) {
      DT_LOG_NOTICE(_logging, "The building of the general mapping has been requested...");
      datatools::properties mapping_config;
      config_.export_and_rename_starting_with(mapping_config, "mapping.", "");
      // geomtools::mapping::extract(config_, mapping_config);
      build_mapping(mapping_config);
    }

    // Plugins initialization :
    DT_LOG_NOTICE(_logging, "Initialization of the geometry plugins... please wait...");
    if (_plugins_factory_preload_) {
      DT_LOG_NOTICE(_logging, "Preloading plugin factory from system register...");
      this->_preload_plugins_global_dict();
    }
    _init_plugins_(config_);

    DT_LOG_NOTICE(_logging, "Exiting.");
    return;
  }

  const std::string & manager::get_world_name () const
  {
    return _world_name_;
  }

  void manager::set_world_name (const std::string & world_name_)
  {
    DT_THROW_IF(this->is_initialized(),
                std::logic_error,
                "Geometry manager is already initialized !");
    _world_name_ = world_name_;
    return;
  }

  void manager::build_mapping (const datatools::properties & mapping_config_)
  {
    if (! is_mapping_available ()) {
      set_mapping_requested (true);
      _mapping_.set_id_manager(_id_manager_);
      DT_LOG_NOTICE(_logging, "Configuring mapping...");
      _mapping_.initialize(mapping_config_);
      DT_LOG_NOTICE(_logging, "Building general mapping... please wait...");
      _mapping_.build_from(_factory_, _world_name_);
      DT_LOG_NOTICE(_logging, "General mapping has been built.");
    }
    return;
  }


  bool manager::can_drop_plugin (const std::string& plugin_name_)
  {
    plugins_dict_type::const_iterator found = _plugins_.find(plugin_name_);
    DT_THROW_IF(found == _plugins_.end(),
                std::logic_error,
                "Geometry plugin '" << plugin_name_ << "' does not exist !");
    return found->second.can_be_dropped();
  }


  void manager::drop_plugin (const std::string& plugin_name_)
  {
    plugins_dict_type::iterator found = _plugins_.find(plugin_name_);
    DT_THROW_IF(found == _plugins_.end(),
                std::logic_error,
                "Geometry plugin '" << plugin_name_ << "' does not exist !");
    plugin_entry& entry = found->second;
    DT_THROW_IF(! found->second.can_be_dropped(),
                std::logic_error,
                "Plugin '" << plugin_name_
                << "' cannot be dropped because of some external constraints !");
    DT_LOG_DEBUG(_logging, "Reset & remove plugin '" << plugin_name_ << "' !");
    this->_reset_plugin(entry);
    _plugins_.erase(found);
  }

  void manager::print_plugins (std::ostream& out_,
                               const std::string& title_,
                               const std::string& indent_) const
  {
    std::string indent;
    if (!indent_.empty()) indent = indent_;

    if (!title_.empty()) out_ << title_ << ":" << std::endl;

    // Plugins:
    {
      size_t sz = _plugins_.size();
      size_t count = 0;
      for (plugins_dict_type::const_iterator it = _plugins_.begin();
           it != _plugins_.end();
           ++it) {
        count++;
        out_ << indent;
        if (count == sz) {
          out_ << "`-- ";
        } else {
          out_ << "|-- ";
        }
        const std::string& plugin_name = it->first;
        const plugin_entry& plugin_record = it->second;

        out_ << "Name : '" << plugin_name << "' "
             << "Type : '" << plugin_record._id_ << "' ";
        out_ << '(';
        int local_count = 0;
        if (plugin_record._status_ & plugin_entry::STATUS_INITIALIZED) {
          out_ << "initialized";
          local_count++;
        }
        out_ << ')';
        out_ << std::endl;
      }
    }
    return;
  }

  void manager::load_plugin(const std::string & plugin_name_,
                            const std::string & plugin_id_,
                            const datatools::properties & plugin_config_,
                            bool only_lock_)
  {
    DT_THROW_IF (only_lock_ && this->is_initialized(),
                 std::logic_error,
                 "Geometry manager is already initialized !");
    this->_load_plugin(plugin_name_, plugin_id_, plugin_config_);
  }

  void manager::load_plugins(const datatools::multi_properties & plugin_mconfig_,
                             bool only_lock_)
  {
    DT_LOG_TRACE(_logging, "Entering...");
    DT_THROW_IF(only_lock_ && this->is_initialized(),
                std::logic_error,
                "Geometry manager is already initialized !");
    for (datatools::multi_properties::entries_ordered_col_type::const_iterator i =
           plugin_mconfig_.ordered_entries().begin();
         i != plugin_mconfig_.ordered_entries().end();
         ++i) {
      datatools::multi_properties::entry* mpe = *i;
      const std::string& plugin_name = mpe->get_key();
      const std::string& plugin_id = mpe->get_meta();
      DT_LOG_NOTICE(_logging,
                    "Loading plugin named '"
                    << plugin_name << "' "
                    << " with ID '"
                    << plugin_id << "'...");
      this->_load_plugin(plugin_name, plugin_id, mpe->get_properties());
    }
  }


  //----------------------------------------------------------------------
  // Protected Interface Definitions
  //

  void manager::_pre_init (const datatools::properties & /*config_*/) {
  }

  void manager::_post_init (const datatools::properties & /*config_*/) {
  }


  void manager::_preload_plugins_global_dict()
  {
    DT_LOG_TRACE(_logging, "Entering...");
    _plugins_factory_register_.import(DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(geomtools::manager::base_plugin));
    return;
  }


  void manager::_load_plugin(const std::string& name_,
                             const std::string& id_,
                             const datatools::properties& config_)
  {
    DT_LOG_TRACE(_logging, "Entering...");
    DT_THROW_IF (this->has_plugin(name_),
                 std::logic_error,
                 "Plugin '" << name_ << "' already exists !");

    {
      // Add a new entry :
      plugin_entry tmp_entry;
      tmp_entry._name_ = name_;
      DT_LOG_DEBUG(_logging, "Add an entry for plugin '" << name_ << "'...");
      _plugins_[name_] = tmp_entry;
    }
    // fetch a reference on it and update :
    plugin_entry& new_entry = _plugins_.find(name_)->second;
    new_entry._id_     = id_;
    new_entry._config_ = config_;
    new_entry._status_ = plugin_entry::STATUS_BLANK;
    //DT_LOG_DEBUG(_logging, "Fetch...");
    this->_create_plugin(new_entry);

    if (_plugins_force_initialization_at_load_) {
      this->_initialize_plugin(new_entry);
    }
    DT_LOG_TRACE(_logging, "Exiting.");
    return;
  }


  void manager::_create_plugin(plugin_entry& entry_)
  {
    if (!(entry_._status_ & plugin_entry::STATUS_CREATED)) {
      DT_LOG_DEBUG(_logging, "Creating plugin named '" <<  entry_._name_ << "'...");
      // search for the plugin's label in the factory dictionary:
      DT_THROW_IF(!_plugins_factory_register_.has(entry_._id_),
                  std::logic_error,
                  "Cannot find plugin factory with ID '" << entry_._id_
                  << "' for plugin named '"
                  << entry_._name_ << "' !");
      typedef geomtools::manager::base_plugin::factory_register_type::factory_type FactoryType;
      const FactoryType& the_factory = _plugins_factory_register_.get(entry_._id_);
      base_plugin* ptr = the_factory();
      entry_._handle_.reset(ptr);
      entry_._status_ |= plugin_entry::STATUS_CREATED;
      DT_LOG_DEBUG(_logging,
                   "Plugin named '"
                   <<  entry_._name_
                   << "' has been created !");
    }
    return;
  }


  void manager::_initialize_plugin(plugin_entry& entry_)
  {
    // If not created, first do it :
    if (!(entry_._status_ & plugin_entry::STATUS_CREATED)) {
      this->_create_plugin(entry_);
    }

    // If not initialized, do it :
    if (!(entry_._status_ & plugin_entry::STATUS_INITIALIZED)) {
      DT_LOG_DEBUG(_logging,
                   "Initializing plugin named '"
                   << entry_._name_
                   << "'...");
      base_plugin& the_plugin = entry_._handle_.grab();
      the_plugin.set_geo_manager(*this);
      if (_services_ == 0) {
        the_plugin.initialize_simple (entry_._config_, _plugins_);
      } else {
        the_plugin.initialize (entry_._config_, _plugins_, *_services_);
      }
      entry_._status_ |= plugin_entry::STATUS_INITIALIZED;
      DT_LOG_NOTICE(_logging,
                    "Plugin named '"
                    << entry_._name_
                    << "' has been initialized.");
    }
    return;
  }


  void manager::_reset_plugin(plugin_entry& entry_)
  {
    if (entry_._status_ & plugin_entry::STATUS_INITIALIZED) {
      base_plugin& the_plugin = entry_._handle_.grab();
      the_plugin.reset();
      entry_._status_ ^= plugin_entry::STATUS_INITIALIZED;
    }
    return;
  }

  void manager::tree_dump(std::ostream& out_,
                          const std::string& title_,
                          const std::string& indent_,
                          bool inherit_) const
  {

    std::string indent;
    if (!indent_.empty()) indent = indent_;

    if (!title_.empty()) out_ << indent << title_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
        << "Logging              : '"
        << datatools::logger::get_priority_label(_logging) << "'"
        << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
        << "Initialized          : "
        << _initialized_
        << "" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
        << "Setup label          : '"
        << _setup_label_
        << "'" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
        << "Setup version        : '"
        << _setup_version_
        << "'" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
        << "Setup description    : '"
        << _setup_description_
        << "'" << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
        << "Services dictionnary : "
        << _services_
        << " " << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Shape factory : "
         << " " << std::endl;

    out_ << indent << datatools::i_tree_dumpable::skip_tag
         << datatools::i_tree_dumpable::last_tag
         << "Number of shapes  : "
         << _shape_factory_.get_shapes().size()
         << " " << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Model factory : "
         << " " << std::endl;

    out_ << indent << datatools::i_tree_dumpable::skip_tag
         << datatools::i_tree_dumpable::tag
         << "Number of geometry models  : "
         << _factory_.get_models().size()
         << " " << std::endl;

   out_ << indent << datatools::i_tree_dumpable::skip_tag
         << datatools::i_tree_dumpable::tag
         << "Number of logical volumes  : "
         << _factory_.get_logicals().size()
         << " " << std::endl;

   out_ << indent << datatools::i_tree_dumpable::skip_tag
         << datatools::i_tree_dumpable::last_tag
         << "Exported property prefixes : "
         << " " << std::endl;
   for (size_t i = 0; i < _factory_.get_property_prefixes().size(); i++) {
     out_ << indent << datatools::i_tree_dumpable::skip_tag
          << datatools::i_tree_dumpable::last_skip_tag;
     if (i != _factory_.get_property_prefixes().size() - 1) {
       out_ << datatools::i_tree_dumpable::tag;
     } else {
       out_ << datatools::i_tree_dumpable::last_tag;
     }
     out_ << "'" << _factory_.get_property_prefixes()[i] << "'" << std::endl;
   }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Mapping requested    : "
         << _mapping_requested_
         << " " << std::endl;
    if (_mapping_requested_) {
      out_ << indent << datatools::i_tree_dumpable::skip_tag
           << datatools::i_tree_dumpable::last_tag << "Mapping size     : "
           << _mapping_.get_geom_infos ().size()
           << " " << std::endl;
    }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "World name           : '" << _world_name_ << "'"
         << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Plugins factory preload              : "
         << _plugins_factory_preload_
         << " " << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Plugins force initialization at load : "
         << _plugins_force_initialization_at_load_
         << " " << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Plugins factories                    : "
         << std::endl;
    {
      std::ostringstream indent2;
      indent2 << indent <<  datatools::i_tree_dumpable::skip_tag;
      std::vector<std::string> plugin_factory_names;
      _plugins_factory_register_.tree_dump(out_,"",indent2.str());
      /*
      std::vector<std::string> plugin_factory_names;
      _plugins_factory_register_.list_of_factories(plugin_factory_names);
      for (int i = 0; i < plugin_factory_names.size(); i++)  {
        int j = i;
        j++;
        out_ << indent;
        out_ << datatools::i_tree_dumpable::skip_tag;
        if (j == plugin_factory_names.size()) {
          out_ << datatools::i_tree_dumpable::last_tag;
        } else {
          out_ << datatools::i_tree_dumpable::tag;
        }
        out_ << plugin_factory_names[i] << std::endl;
      }
      */
    }

    out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
        << "Plugins                              : "
        << _plugins_.size()
        << " " << std::endl;
    for (plugins_dict_type::const_iterator i = _plugins_.begin();
         i != _plugins_.end();
         i++) {
      plugins_dict_type::const_iterator j = i;
      j++;
      out_ << indent;
      out_ << datatools::i_tree_dumpable::inherit_skip_tag(inherit_);
      if (j == _plugins_.end()) {
        out_ << datatools::i_tree_dumpable::last_tag;
      } else {
        out_ << datatools::i_tree_dumpable::tag;
      }
      out_ << "'" << i->first << "' as a '" << i->second.get_id() << "'";
      if (i->second.is_created()) {
        out_ << " (created)";
      } else if  (i->second.is_initialized()) {
        out_ << " (initialized)";
      }
      out_ << std::endl;
    }

    return;
  }

  const manager::base_plugin::factory_register_type & manager::get_plugins_factory_register()
  {
    return _plugins_factory_register_;
  }

  int manager::print_list_of_gids(const geomtools::manager & mgr_,
                                  const std::string & command_,
                                  const std::string & options_,
                                  std::ostream & out_)
  {
    std::vector<std::string> requested_categories;
    std::vector<int> requested_types;

    bool with_title = true;
    bool with_multicolumn = true;
    std::string mapping_plugin;

    // Parse options :
    std::istringstream options_iss(options_);
    options_iss >> std::ws;
    while (options_iss && ! options_iss.eof()) {
      std::string option;
      options_iss >> option >> std::ws;

      if (option=="--help" || option=="-h") {
        out_ << "  Usage: \n";
        out_ << "     " << command_ << " [OPTIONS...]  \n"
             << "\n";
        out_ << "  Options: \n";
        out_ << "    -h | --help                    Print this help\n"
             << "    -t | --with-title              Print a title line\n"
             << "    -T | --without-title           Do not print a title line\n"
             << "    -m | --multicolumn             Print in multicolumn mode\n"
             << "    -1 | --onecolumn               Print in one column mode\n"
             << "    -c | --with-category CATEGORY  Print GIDs with category name CATEGORY\n"
             << "    -p | --with-type     TYPE      Print GIDs with category type TYPE\n"
             << std::endl;
        return -1;
      }
      else if (option=="--with-title" || option=="-t") with_title = true;
      else if (option=="--without-title" || option=="-T") with_title = false;
      else if (option=="--multicolumn" || option=="-m") with_multicolumn = true;
      else if (option=="--onecolumn" || option=="-1") with_multicolumn = false;
      else if (option=="--with-category" || option=="-c") {
        std::string category;
        options_iss >> category >> std::ws;
        datatools::remove_quotes(category);
        if (category.empty()) {
          DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                       "Missing geometry category name (please use: '--with-category CATEGORY' ) !");
          return 1;
        }
        requested_categories.push_back(category);
      }
      else if (option=="--with-type" || option=="-p") {
        int type = -1;
        options_iss >> type >> std::ws;
        if (!options_iss) {
          DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                       "Invalid geometry type format !");
          return 1;
        }
        if (type < 0) {
          DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                       "Missing geometry category type (please use: '--with-type TYPE') !");
          return 1;
        }
        requested_types.push_back(type);
      }
      // else if (option=="--with-mapping-plugin") {
      //   std::string plugin;
      //   options_iss >> plugin;
      //   if (plugin.empty()) {
      //     DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
      //                  "Missing mapping plugin name (please use: '--with-mapping-plugin PLUGIN' ) !");
      //     return 1;
      //   }
      //   mapping_plugin = plugin;
      // }
      else {
        DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Invalid option '" << option << "' !");
        return -1;
      }
      if (options_iss.eof()) break;
    }

    if (mapping_plugin.empty() && ! mgr_.is_mapping_available()) {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                   "No mapping information is available from the geometry manager !");
      return 1;
    }
    const geomtools::mapping & the_mapping = mgr_.get_mapping();
    const geomtools::geom_info_dict_type & ginfo_dict = the_mapping.get_geom_infos ();
    std::vector<const geom_info*> selected_ginfos;
    selected_ginfos.reserve(ginfo_dict.size()/2+1);

    for (geomtools::geom_info_dict_type::const_iterator i = ginfo_dict.begin();
         i != ginfo_dict.end();
         i++) {
      const geomtools::geom_info & ginfo = i->second;
      const geomtools::geom_id & gid = ginfo.get_gid ();

      const geomtools::id_mgr::categories_by_type_col_type & cats
        = the_mapping.get_id_manager ().categories_by_type ();
      const geomtools::id_mgr::category_info * cat_info = cats.find(gid.get_type())->second;
      const std::string & category = cat_info->get_category ();
      bool selected = true;
      if (requested_categories.size() ||  requested_types.size()) {
        selected = false;
      }
      if (! selected && requested_categories.size()) {
        if (std::find(requested_categories.begin(),
                      requested_categories.end(),
                      category) != requested_categories.end()) {
           selected = true;
        }
      }
      if (! selected && requested_types.size()) {
        if (std::find(requested_types.begin(),
                      requested_types.end(),
                      gid.get_type()) != requested_types.end()) {
          selected = true;
        }
      }
      if (selected) {
        selected_ginfos.push_back(&ginfo);
      }
    }

    if (with_title) {
      out_ << std::flush << "List of available GIDs : " << std::endl;
    }
    int count = 0;
    size_t max_width = 36;
    for (std::vector<const geom_info*>::const_iterator i = selected_ginfos.begin();
         i != selected_ginfos.end();
         i++) {
      const geomtools::geom_info & ginfo =**i;
      const geomtools::geom_id & gid = ginfo.get_gid ();

      const geomtools::id_mgr::categories_by_type_col_type & cats
        = the_mapping.get_id_manager ().categories_by_type ();
      const geomtools::id_mgr::category_info * cat_info = cats.find(gid.get_type())->second;
      const std::string & category = cat_info->get_category ();

      if (with_multicolumn) {
        bool long_name = false;
        std::ostringstream oss;
        oss << gid << " as '" << category << "'";
        if (oss.str().size () > max_width) {
          long_name = true;
        }
        if (count != 0 && (count % 2) == 0) {
          out_ << std::endl;
        }
        out_  << "  " << std::setw (max_width)
              << std::setiosflags(std::ios::left)
              << std::resetiosflags(std::ios::right)
              << oss.str() << "  ";
        if (long_name) {
          out_ << std::endl;
          count = 0;
        }
        count++;
      } else {
        out_ << gid << " as '" << category << "' with addresses: ";
        for (size_t j = 0; j < cat_info->get_depth(); j++) {
          out_ << ' ' << cat_info->get_addresses()[j];
          if (j < cat_info->get_depth()-1) out_ << ',';
        }
        out_ << std::endl;
      }
    }
    if (with_multicolumn) {
      if ((count % 2) == 1) {
        out_ << std::endl;
      }
      out_ << std::endl;
    }
    return 0;
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
  ocd_.set_class_documentation("The geometry manager is the core class of the ``geomtools``     \n"
                               "framework. It hosts four components :                           \n"
                               "                                                                \n"
                               " a) A factory of geometry models, which is responsible of the   \n"
                               "    construction of the full hierarchy of geometry volumes,     \n"
                               "    compatible with the GDML hierarchical scheme (and thus      \n"
                               "    Geant4).                                                    \n"
                               " b) A manager for geometry IDs (GID), which describes the       \n"
                               "    relationships between some geometry volumes in the geometry \n"
                               "    hierarchy and the addressing scheme of such volumes         \n"
                               "    based on geometry IDs (``geomtools::geom_id`` class).       \n"
                               " c) A mapping object which is responsible of a lookup-table     \n"
                               "    that allows to access fundamental informations about some   \n"
                               "    volume in the setup, given its unique GID and obeying the   \n"
                               "    numbering scheme setup by the GID manager.                  \n"
                               " d) A manager for geometry plugins which allows to dynamically  \n"
                               "    add new geometry-related functionnalities to the manager.   \n"
                               );


  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("logging.priority")
      .set_terse_description("The logging priority threshold")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .add_example("Activate the warning logging level::        \n"
                   "                                            \n"
                   "    logging.priority : string = \"warning\" \n"
                   "                                            \n"
                   )
      ;
  }

  // {
  //   configuration_property_description & cpd = ocd_.add_configuration_property_info();
  //   cpd.set_name_pattern("debug")
  //     .set_terse_description("The debug flag")
  //     .set_traits(datatools::TYPE_BOOLEAN)
  //     .set_mandatory(false)
  //     .set_long_description("The debug flag activates some debug printing.    \n"
  //                           "Not recommended for a production run.            \n"
  //                           "Example :                                        \n"
  //                           "                                                 \n"
  //                           "    debug : boolean = 0                          \n"
  //                           "                                                 \n"
  //                           )
  //     ;
  // }

  // {
  //   configuration_property_description & cpd = ocd_.add_configuration_property_info();
  //   cpd.set_name_pattern("verbose")
  //     .set_terse_description("The verbose flag")
  //     .set_traits(datatools::TYPE_BOOLEAN)
  //     .set_mandatory(false)
  //     .set_long_description("The verbose flag activates some additionnal printing.\n"
  //                           "Not recommended for a production run.            \n"
  //                           "Example :                                        \n"
  //                           "                                                 \n"
  //                           "    verbose : boolean = 0                        \n"
  //                           "                                                 \n"
  //                           )
  //     ;
  // }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("setup_label")
      .set_terse_description("The label that identifies the virtual geometry setup")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_complex_triggering_conditions(true)
      .set_long_description("Superseded by a previous call to :             \n"
                            "  ``geomtools::manager::set_setup_label(...)`` \n")
      .add_example("Set the label of the virtual geometry setup::  \n"
                   "                                               \n"
                   "  setup_label : string = \"test_setup\"        \n"
                   "                                               \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("setup_version")
      .set_terse_description("The version string of the virtual geometry setup")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("Superseded by a previous call to:                \n"
                            "  ``geomtools::manager::set_setup_version(...)`` \n")
      .add_example("Set the version of the virtual geometry setup::  \n"
                   "                                                 \n"
                   "    setup_version : string = \"0.1\"             \n"
                   "                                                 \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("setup_description")
      .set_terse_description("The description string of the virtual geometry setup")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("Superseded by a previous call to :                   \n"
                            "  ``geomtools::manager::set_setup_description(...)`` \n")
      .add_example("Set the description of the virtual geometry setup::  \n"
                   "                                                     \n"
                   "    setup_description : string = \"My test setup\"   \n"
                   "                                                     \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("plugins.factory_no_preload")
      .set_terse_description("The flag inhibits the preloading of plugins' factories")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .set_long_description("This flag inhibits the preloading of plugins' factories  \n"
                            "from the plugins system register.                        \n"
                            "Not recommended for a production run.                    \n")
      .add_example("Use the plugin factory preload::                         \n"
                   "                                                         \n"
                   "    plugins.factory_no_preload : boolean = 0             \n"
                   "                                                         \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("plugins.force_initialization_at_load")
      .set_terse_description("The flag to force the initialization of plugins at load stage.")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("This flag triggers the initialization of plugins as soons as \n"
                            "they are loaded in the geometry manager's plugin system.     \n"
                            "Default is false to activate the dedicated resources of a    \n"
                            "given plugin if and only if the plugin is used.              \n"
                            "Not recommended for a production run.                        \n")
      .set_default_value_boolean(false)
      .add_example("Do not force initialization at load step::           \n"
                   "                                                     \n"
                   "  plugins.force_initialization_at_load : boolean = 0 \n"
                   "                                                     \n"
                   )
      ;
  }


  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("factory.debug")
      .set_terse_description("The debug flag of the geometry model factory")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .set_long_description("This flag activates some debug printings from \n"
                            "the embedded geometry model factory.          \n"
                            "Not recommended for a production run.         \n"
                            )
      .add_example("Do not use debug message for the factory::    \n"
                   "                                              \n"
                   "    factory.debug : boolean = 0               \n"
                   "                                              \n"
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
      .set_long_description("The *geometry list* file stores a list of filenames.         \n"
                            "Each of these files contains rules that define geometry      \n"
                            "models to be instantiated by the embedded geometry model     \n"
                            "factory.                                                     \n"
                            "                                                             \n"
                            "The order of the geometry files (``*.geom``) is critical     \n"
                            "because some geometry models may depend on other ones        \n"
                            "which should thus be defined **before** their dependers.     \n"
                            "See OCD support for ``geomtools::model_factory`` for further \n"
                            "informations about the configuration of geometry models.     \n"
                            )
      .add_example("Load a file which contains the list of geometry files::      \n"
                   "                                                             \n"
                   "    factory.geom_list : string as path = \\                  \n"
                   "      \"${CONFIG_REPOSITORY_DIR}/geom/geom_files.lis\"       \n"
                   "                                                             \n"
                   "where the '.../geom/geom_files.lis' file has the following   \n"
                   "format ::                                                    \n"
                   "                                                             \n"
                   "     ${CONFIG_REPOSITORY_DIR}/geom/models/sources.geom       \n"
                   "     ${CONFIG_REPOSITORY_DIR}/geom/models/detectors.geom     \n"
                   "     ${CONFIG_REPOSITORY_DIR}/geom/models/shielding.geom     \n"
                   "     ${CONFIG_REPOSITORY_DIR}/geom/models/vessel.geom        \n"
                   "     ${CONFIG_REPOSITORY_DIR}/geom/models/lab.geom           \n"
                   "     ${CONFIG_REPOSITORY_DIR}/geom/models/world.geom         \n"
                   "                                                             \n"
                   "Here the ``CONFIG_REPOSITORY_DIR`` is an environment variable\n"
                   "which points to the base configuration directory.            \n"
                   "                                                             \n"
                   "A plural version exist to load several lists of geom files:: \n"
                   "The order of the geometry list files (``*.lis``) is critical \n"
                   "because some geometry models may depend on other ones::      \n"
                   "                                                             \n"
                   "    factory.geom_lists : string[3] as path = \\              \n"
                   "      \"${CONFIG_REPOSITORY_DIR}/geom/calo_models_1.lis\" \\  \n"
                   "      \"${CONFIG_REPOSITORY_DIR}/geom/traker_models_2.lis\" \\\n"
                   "      \"${CONFIG_REPOSITORY_DIR}/geom/detector.lis\"         \n"
                   "                                                             \n"
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
                            "models to be instantiated by the embedded geometry model         \n"
                            "factory.                                                         \n"
                            "                                                                 \n"
                            "The order of the geometry files (``*.geom``) is critical         \n"
                            "because some geometry models may depend on other ones            \n"
                            "which should thus be defined **before** their dependers.         \n"
                            "See OCD support for ``geomtools::model_factory`` for further     \n"
                            "informations about the configuration of geometry models.         \n"
                            )
      .add_example("Use the explicit list of geometry files::                        \n"
                   "                                                                 \n"
                   "    factory.geom_files : string[6] as path =                  \\ \n"
                   "      \"${CONFIG_REPOSITORY_DIR}/geom/models/sources.geom\"   \\ \n"
                   "      \"${CONFIG_REPOSITORY_DIR}/geom/models/detectors.geom\" \\ \n"
                   "      \"${CONFIG_REPOSITORY_DIR}/geom/models/shielding.geom\" \\ \n"
                   "      \"${CONFIG_REPOSITORY_DIR}/geom/models/vessel.geom\"    \\ \n"
                   "      \"${CONFIG_REPOSITORY_DIR}/geom/models/lab.geom\"       \\ \n"
                   "      \"${CONFIG_REPOSITORY_DIR}/geom/models/world.geom\"        \n"
                   "                                                                 \n"
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
      .set_long_description("All auxiliary properties found in a geometry model definition        \n"
                            "file that starts with one of these prefixes will be preserved        \n"
                            "and stored in the geometry model constructed by the embedded         \n"
                            "model factory. Such properties can then be used by external          \n"
                            "algorithms and/or plugins to extract additional informations         \n"
                            "and/or configuration parameters.                                     \n"
                            "A property prefix must ends with the dot ``.`` character.            \n"
                            "Default behaviour already preserves the properties with the          \n"
                            "following prefixes :                                                 \n"
                            "                                                                     \n"
                            "  * \"visibility.\" : Properties for rendering/display               \n"
                            "                      (color, hidden...)                             \n"
                            "  * \"mapping.\"    : Properties for automatic computing             \n"
                            "                      of the geometry IDs (GID) associated to        \n"
                            "                      the geometry volumes in the setup.             \n"
                            "  * \"material.\"   : Properties related to the materials            \n"
                            "                      the geometry volumes are made of.              \n"
                            "                      Used by the material plugin.                   \n"
                            "  * \"sensitive.\"  : Properties related to the definition           \n"
                            "                      of *sensitive* geometry volumes.               \n"
                            "                      Such information can be used to associate      \n"
                            "                      a given logical volume (model) to some         \n"
                            "                      sensitive detector within Geant4 (see          \n"
                            "                      the ``mctools`` library and its ``mctools_g4`` \n"
                            "                      DLL plugin.                                    \n"
                            )
      .add_example("Use a specific list of prefixes for geometry models properties::     \n"
                   "                                                                     \n"
                   "    factory.preserved_property_prefixes : string[3] =  \\            \n"
                   "      \"display.\"         \\                                        \n"
                   "      \"radioactivity.\"   \\                                        \n"
                   "      \"magnetization.\"                                             \n"
                   "                                                                     \n"
                   )
      ;
  }


  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("world_name")
      .set_terse_description("A name of the top level geometry volume")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("This is the name used by the geometry system to identify \n"
                            "the top level geometry logical volume that contains the  \n"
                            "full virtual geometry setup.                             \n")
      .set_default_value_string("world")
      .add_example("Use the default name:                                    \n"
                   "                                                         \n"
                   "    world_name : string =  \"world\"                     \n"
                   "                                                         \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("id_mgr.debug")
      .set_terse_description("The debug flag of the geometry ID (GID) manager")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("This flag activates some debug printings from \n"
                            "the embedded geometry ID manager.             \n"
                            "Not recommended for a production run.         \n")
      .add_example("Do not activate the debug message for the ID manager:: \n"
                   "                                                       \n"
                   "    id_mgr.debug : boolean = 0                         \n"
                   "                                                       \n"
                   )
      ;
  }


  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("id_mgr.categories_list")
      .set_terse_description("A unique configuration file that defines geometry categories")
      .set_traits(datatools::TYPE_STRING)
      .set_path(true)
      .set_mandatory(false)
      .set_deprecated(false, "See the 'id_mgr.categories_lists' configuration property")
      .set_long_description(
                            "A unique filename from where the geometry categories         \n"
                            "used by the geometry ID manager are defined.                 \n"
                            "                                                             \n"
                            "The target file must use the format of the                   \n"
                            "``datatools::multi_properties`` class.                       \n"
                            "See OCD support for ``geomtools::id_mgr`` for further        \n"
                            "informations about the configuration of geometry categories. \n"
                            )
      .add_example("Use a single file to describe the geometry categories::      \n"
                   "                                                             \n"
                   "    id_mgr.categories_list : string as path = \\             \n"
                   "      \"${CONFIG_REPOSITORY_DIR}/geom/geom_categories.lis\"  \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("id_mgr.categories_lists")
      .set_terse_description("A list of configuration files that define geometry categories")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_path(true)
      .set_mandatory(false)
      .set_long_description(
                            "A list of filenames from where the geometry categories       \n"
                            "used by the geometry ID manager are defined.                 \n"
                            "                                                             \n"
                            "The target files must use the format of the                  \n"
                            "``datatools::multi_properties`` class.                       \n"
                            "The loading order of the files is critical                   \n"
                            "because some geometry categories may depend on other ones    \n"
                            "which should thus be defined **before** their dependers.     \n"
                            "See OCD support for ``geomtools::id_mgr`` for further        \n"
                            "informations about the configuration of geometry categories. \n"
                            )
      .add_example("Use two files to describe the geometry categories::                \n"
                   "                                                                   \n"
                   "    id_mgr.categories_lists : string[2] as path = \\               \n"
                   "      \"${CONFIG_REPOSITORY_DIR}/geom/tracker_categories.lis\" \\  \n"
                   "      \"${CONFIG_REPOSITORY_DIR}/geom/calorimeter_categories.lis\" \n"
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
                            "by the embedded geometry mapping object that hosts a dictionnary     \n"
                            "of *geometry information* objects (``geomtools::geom_info`` class).  \n"
                            "Each *geometry information* object describes an unique geometry      \n"
                            "volume (placement, shape, auxiliary properties...), being            \n"
                            "unambiguously associated to an unique key : the *geometry ID*        \n"
                            "(GID, see the ``geomtools::geom_id`` class).                         \n"
                            "                                                                     \n"
                            "All properties starting with the ``\"mapping.\"`` prefix are then    \n"
                            "transmitted to initialize the embedded 'mapping' object.             \n"
                            "                                                                     \n"
                            "See OCD support for ``geomtools::mapping`` for further               \n"
                            "informations about the configuration of geometry categories.         \n"
                            )
      .add_example("Build the default mapping::                            \n"
                   "                                                       \n"
                   "    build_mapping : boolean = 1                        \n"
                   "                                                       \n"
                   )
      .add_example("Configure the mapping in order to map all categories up to level 100,   \n"
                   "including the top-level 'world' category but excluding two geometry     \n"
                   "categories::                                                            \n"
                   "                                                                        \n"
                   "    mapping.debug : boolean = 0                                         \n"
                   "    mapping.max_depth : integer = 100                                   \n"
                   "    mapping.no_world_mapping: boolean = 0                               \n"
                   "    mapping.excluded_categories : string [2] = \"bolt.gc\" \"screw.gc\" \n"
                   )
      .add_example("Configure the mapping in order to map only one category, excluding  \n"
                   "the top-level 'world' category ::                                   \n"
                   "                                                                    \n"
                   "    mapping.no_world_mapping: boolean = 1                           \n"
                   "    mapping.only_categories : string [1] = \"scin_block.gc\"        \n"
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
      .set_long_description("Each of the addressed files contains rules that define geometry          \n"
                            "plugins. The files use the format of the ``datatools::multi_properties`` \n"
                            "class. The filenames may contain environment variables.                  \n")
      .add_example("Activates the geometry plugins described in 2 configuration files::      \n"
                   "                                                                         \n"
                   "    plugins.configuration_files : string[2] as path =      \\            \n"
                   "      \"${CONFIG_REPOSITORY_DIR}/geom/mapping_plugins.conf\"  \\         \n"
                   "      \"${CONFIG_REPOSITORY_DIR}/geom/mag_field_plugin.conf\"            \n"
                   "                                                                         \n"
                   "Here some plugins with embedded dedicated mapping objects are defined    \n"
                   "in the ``mapping_plugins.conf`` file::                                   \n"
                   "                                                                         \n"
                   "    #@description List of geometry plugins                               \n"
                   "    #@key_label   \"name\"                                               \n"
                   "    #@meta_label  \"type\"                                               \n"
                   "                                                                         \n"
                   "    [name=\"mapping_no_screw\" type=\"geomtools::mapping_plugin\"]       \n"
                   "    mapping.excluded_categories : string [1] = \"screw.gc\"              \n"
                   "                                                                         \n"
                   "The ``geomtools::mapping_plugin`` is provided by the ``geomtools``       \n"
                   "library.                                                                 \n"
                   )
      ;
  }

  ocd_.set_configuration_hints("A geometry manager is configured through a configuration file that  \n"
                               "uses the format of 'datatools::properties' setup file.              \n"
                               "Example: ::         .                                               \n"
                               "  logging.priority : string = \"warning\"                           \n"
                               "  setup_label : string = \"Foo\"                                    \n"
                               "  setup_version : string = \"1.2\"                                  \n"
                               "  setup_description : string = \\                                   \n"
                               "     \"The virtual geometry setup of the Foo experiment\"           \n"
                               "  factory.geom_files : string[4] as path = \\                       \n"
                               "    \"@foo:config/models/optical_modules.geom\" \\                 \n"
                               "    \"@foo:config/models/detector.geom\" \\                        \n"
                               "    \"@foo:config/models/support.geom\" \\                         \n"
                               "    \"@foo:config/models/lab.geom\"                                \n"
                               "  id_mgr.categories_lists : string[2] as path = \\                 \n"
                               "    \"@foo:config/om_categories.lis\" \\                           \n"
                               "    \"@foo:config/env_categories.lis\"                             \n"
                               "  build_mapping : boolean = 1                                      \n"
                               "  plugins.configuration_files : string[2] as path = \\             \n"
                               "    \"@foo:config/plugins/material.conf\"  \\                      \n"
                               "    \"@foo:config/plugins/magfields.conf\"                         \n"
                               "                                                                   \n"
                              );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()
DOCD_CLASS_SYSTEM_REGISTRATION(::geomtools::manager,"geomtools::manager")
