/* service_manager.cc
 *
 * Copyright (C) 2011-2017 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

// Ourselves:
#include <datatools/service_manager.h>

// Standard Library:
#include <stdexcept>
#include <sstream>
#include <memory>

// This Project:
#include <datatools/base_service.h>
#include <datatools/ioutils.h>
#include <datatools/utils.h>
#include <datatools/properties.h>
#include <datatools/multi_properties.h>
#include <datatools/exception.h>
#include <datatools/logger.h>

namespace datatools {

  //----------------------------------------------------------------------
  // Public Interface Definitions
  //

  /*
  service_manager::dependee_manager_handle::dependee_manager_handle()
  {
    return;
  }

  service_manager::dependee_manager_handle::~dependee_manager_handle()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  void service_manager::dependee_manager_handle::initialize(const service_manager & dependee_mgr_)
  {
    DT_THROW_IF(dependee_mgr_.is_initialized(),
                std::logic_error, "Dependee manager is not initialized!");
    _manager_ = &dependee_mgr_;
    _owned_   = false;
    return;
  }

  void service_manager::dependee_manager_handle::initialize(const datatools::properties & config_,
                                                            const std::string & name_,
                                                            const std::string & description_,
                                                            const uint32_t flags_)
  {
    service_manager * depmgr = nullptr;
    try {
      depmgr = new service_manager(name_, description_, flags_);
      depmgr->initialize(config_);
      _manager_ = depmgr;
      _owned_   = true;
    } catch (std::exception & error) {
      if (depmgr != nullptr) {
        delete depmgr;
        depmgr = nullptr;
      }
      throw error;
    }
    return;
  }

  bool service_manager::dependee_manager_handle::is_initialized() const
  {
    return _manager_ != nullptr;
  }

  const service_manager & service_manager::dependee_manager_handle::get_manager() const
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error, "Dependee manager handle is not initialized!");
    return *_manager_;
  }

  bool service_manager::dependee_manager_handle::is_owned() const
  {
    return _owned_;
  }

  void service_manager::dependee_manager_handle::reset()
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error, "Dependee manager handle is not initialized!");
    if (_owned_) {
      if (_manager_->is_initialized()) {
        service_manager * mutable_mgr = const_cast<service_manager *>(_manager_);
        mutable_mgr->reset();
      }
      delete _manager_;
    }
    _manager_ = nullptr;
    return;
  }
  */

  // Logging features:
  void service_manager::set_logging_priority(::datatools::logger::priority p_)
  {
    _logging_priority_ = p_;
    return;
  }

  datatools::logger::priority service_manager::get_logging_priority() const
  {
    return _logging_priority_;
  }

  void service_manager::set_name(const std::string& name_)
  {
    _name_ = name_;
    return;
  }


  void service_manager::set_description(const std::string& description_)
  {
    _description_ = description_;
    return;
  }


  const std::string& service_manager::get_name() const
  {
    return _name_;
  }


  const std::string& service_manager::get_description() const
  {
    return _description_;
  }


  bool service_manager::is_debug() const
  {
    return get_logging_priority() >= datatools::logger::PRIO_DEBUG;
  }

  void service_manager::set_debug(bool debug)
  {
    if (debug) {
      set_logging_priority(datatools::logger::PRIO_DEBUG);
    } else {
      set_logging_priority(datatools::logger::PRIO_WARNING);
    }
    return;
  }

  bool service_manager::is_initialized() const
  {
    return _initialized_;
  }

  // bool service_manager::has_mounted_dependee_managers() const
  // {
  //   return dependee_managers_.size();
  // }

  // bool service_manager::can_mount_dependee_manager() const
  // {
  //   return _local_services_.size() == 0;
  // }

  // void service_manager::mount_dependee_manager(const service_manager & mgr_)
  // {
  //   DT_THROW_IF(_local_services_.size(), std::logic_error, "Cannot mount external dependee manager!");
  //   {
  //     dependee_manager_handle new_dmh;
  //     dependee_managers_[mgr_.get_name()] = new_dmh;
  //   }
  //   dependee_manager_handle & dmh = dependee_managers_.find(mgr_.get_name())->second;
  //   dmh.initialize(mgr_);
  //   return;
  // }

  // void service_manager::unmount_dependee_managers()
  // {
  //   DT_THROW_IF(_local_services_.size(), std::logic_error, "Cannot unmount external dependee managers!");
  //   dependee_managers_.clear();
  //   return;
  // }

  void service_manager::load(const std::string& name,
                             const std::string& id,
                             const datatools::properties& config)
  {
    DT_THROW_IF(!is_allow_dynamic_services() && this->is_initialized(),
                std::logic_error, "Service manager is already initialized and does not allow service '" << name << "' to be loaded dynamically !");
    this->load_service(name, id, config);
    return;
  }

  base_service & service_manager::load_no_init(const std::string& name,
                                               const std::string& id)
  {
    DT_THROW_IF(!is_allow_dynamic_services() && this->is_initialized(),
                std::logic_error, "Service manager is already initialized and does not allow service '" << name << "' to be loaded dynamically !");
    this->load_service(name, id, nullptr);
    service_dict_type::iterator found = _local_services_.find(name);
    DT_THROW_IF (found == _local_services_.end(),
                 std::logic_error,
                 "No service named '" << name << "' !");
    service_entry& sentry = *found->second.get();
    return sentry.grab_service_handle().grab();
  }

  void service_manager::configure_no_init(const std::string& name, const datatools::properties& config)
  {
    DT_THROW_IF(!is_allow_dynamic_services() && this->is_initialized(),
                std::logic_error, "Service manager is already initialized and does not allow service '" << name << "' to be configured dynamically !");
    service_dict_type::iterator found = _local_services_.find(name);
    DT_THROW_IF (found == _local_services_.end(),
                 std::logic_error,
                 "No service named '" << name << "' !");
    service_entry& sentry = *found->second.get();
    // Copy the config container in the uninitialized service entry for further initialization:
    sentry.set_service_config(config);
    return;
  }

  void service_manager::load(const datatools::multi_properties& config)
  {
    DT_LOG_TRACE(get_logging_priority(), "Entering...");
    DT_THROW_IF(!is_allow_dynamic_services() && this->is_initialized(),
                std::logic_error, "Service manager is already initialized and does not allow dynamic load of services !");
    for (multi_properties::entries_ordered_col_type::const_iterator i = config.ordered_entries().begin();
         i != config.ordered_entries().end();
         ++i) {
      const multi_properties::entry* mpe = *i;
      const std::string& service_name = mpe->get_key();
      const std::string& service_id = mpe->get_meta();
      DT_LOG_DEBUG (get_logging_priority(), "Load a new service named '" << service_name << "' "
                    << " with type ID '" << service_id << "'...");
      this->load_service(service_name, service_id, mpe->get_properties());
    }
    return;
  }

  const base_service & service_manager::get_service(const std::string& name) const
  {
    DT_LOG_DEBUG(get_logging_priority(), "name='" << name << "'");
    service_manager * mutable_this = const_cast<service_manager*>(this);
    return const_cast<base_service&>(mutable_this->grab_service(name));
  }

  base_service & service_manager::grab_service(const std::string& name)
  {
    DT_LOG_DEBUG(get_logging_priority(), "name='" << name << "'");
    service_dict_type::iterator found = _local_services_.find(name);
    DT_THROW_IF (found == _local_services_.end(),
                 std::logic_error,
                 "No service named '" << name << "' !");
    service_entry& sentry = *found->second.get();
    if (!sentry.is_initialized()) {
      this->initialize_service(sentry);
    }
    return sentry.grab_service_handle().grab();
  }

  void service_manager::initialize(const datatools::properties& config)
  {
    DT_THROW_IF(this->is_initialized(), std::logic_error, "Service manager is already initialized !");

    datatools::logger::priority p
      = datatools::logger::extract_logging_configuration(config,
                                                         get_logging_priority(),
                                                         true);
    if (p != datatools::logger::PRIO_UNDEFINED) {
      set_logging_priority(p);
    }

    if (_name_.empty()) {
      if (config.has_key("name")) {
        _name_ = config.fetch_string("name");
      }
    }

    if (_description_.empty()) {
      if (config.has_key("description")) {
        _description_ = config.fetch_string("description");
      }
    }

    if (!_allow_dynamic_services_) {
      if (config.has_key("allow_dynamic_services")) {
        _allow_dynamic_services_ = config.fetch_boolean("allow_dynamic_services");
      }
    }

    if (!_force_initialization_at_load_) {
      if (config.has_key("force_initialization_at_load")) {
        _force_initialization_at_load_ = config.fetch_boolean("force_initialization_at_load");
      }
    }

    /*
    // Import services from another manager configuration :
    {
      typedef std::vector<std::string> ImportList;
      typedef ImportList::iterator ImportListIterator;
      std::string import_file_key("services.import");

      ImportList import_file_list;
      if (config.has_key(import_file_key)) {
        config.fetch(import_file_key, import_file_list);
      }

      for (ImportListIterator i = import_file_list.begin();
           i < import_file_list.end();
           ++i) {
        datatools::fetch_path_with_env(*i);
        datatools::multi_properties mconfig;
        mconfig.read(*i);
        this->load(mconfig);
      }
    }
    */

    // services :
    {
      typedef std::vector<std::string> CFList;
      typedef CFList::iterator CFListIterator;
      std::string conf_file_key("services.configuration_files");

      CFList conf_file_list;

      if (config.has_key(conf_file_key)) {
        config.fetch(conf_file_key, conf_file_list);
      }

      for (CFListIterator i = conf_file_list.begin();
           i < conf_file_list.end();
           ++i) {
        datatools::fetch_path_with_env(*i);
        datatools::multi_properties mconfig;
        datatools::multi_properties::read_config(*i, mconfig);
        this->load(mconfig);
      }
    }

    sync();
    _initialized_ = true;
    return;
  }

  void service_manager::initialize()
  {
    datatools::properties dummy_config;
    this->initialize(dummy_config);
    return;
  }

  void service_manager::reset()
  {
    DT_LOG_TRACE(get_logging_priority(),"Entering...");
    DT_THROW_IF(!_initialized_,std::logic_error,"Manager is not initialized !");
    _initialized_ = false;
    _service_bus_.clear();
    size_t count = _local_services_.size();
    size_t initial_size = _local_services_.size();
    while (_local_services_.size() > 0) {
      for (service_dict_type::iterator it = _local_services_.begin();
           it != _local_services_.end();
           ++it) {
        service_entry& entry = *it->second.get();
        if (entry.can_be_dropped()) {
          DT_LOG_DEBUG(get_logging_priority(), "Removing service '" << entry.get_service_name ()  << "'...");
          this->destroy_service(entry);
          --count;
          break;
        }
      }
      if (count == initial_size) {
        break;
      }
    }
    if (_local_services_.size() > 0) {
      DT_LOG_WARNING(get_logging_priority(),"There are some left services !");
    }
    _local_services_.clear();
    _factory_register_.reset();
    _allow_dynamic_services_ = false;
    _force_initialization_at_load_ = false;
    _preload_ = true;
    DT_LOG_TRACE(get_logging_priority(),"Exiting.");
    return;
  }

  // Constructor :
  service_manager::service_manager(const std::string& name,
                                   const std::string& description,
                                   uint32_t flag)
    : _factory_register_("datatools::base_service/service_factory",
                        flag & FACTORY_VERBOSE ?
                        base_service::factory_register_type::verbose : 0)
  {
    _initialized_ = false;
    if (!name.empty()) {
      this->set_name(name);
    }
    if (!description.empty()) {
      this->set_description(description);
    }
    set_logging_priority(datatools::logger::PRIO_FATAL);
    _force_initialization_at_load_ = false;
    _allow_dynamic_services_ = false;

    if (flag & FORCE_INITIALIZATION_AT_LOAD) {
      _force_initialization_at_load_ = true;
    }

    if (flag & ALLOW_DYNAMIC_SERVICES) {
      _allow_dynamic_services_ = true;
    }

    bool preload = true;
    if (flag & NO_PRELOAD) {
      preload = false;
    }

    this->set_preload(preload);
    return;
  }

  service_manager::~service_manager ()
  {
    if (_initialized_) this->reset();
    return;
  }

  void service_manager::sync()
  {
    update_service_bus();
    return;
  }

  void service_manager::update_service_bus()
  {
    _service_bus_.clear();
    // Imported services:
    // not supported yet

    // Add locally managed services:
    for (service_dict_type::iterator iservice = _local_services_.begin();
         iservice != _local_services_.end();
         iservice++) {
      const std::string & service_name = iservice->first;
      _service_bus_[service_name] = iservice->second;
    }
    return;
  }

  /****************
   *   SERVICES   *
   ****************/

  void service_manager::build_list_of_services(std::vector<std::string> & list_, uint32_t flags_)
  {
    bool clear = true;
    bool select_initialized = true;
    bool select_uninitialized = true;
    bool select_bus = false;
    if (flags_ & FILTER_NO_CLEAR) {
      clear = false;
    }
    if (flags_ & FILTER_NO_INITIALIZED) {
      select_initialized = false;
    }
    if (flags_ & FILTER_NO_UNINITIALIZED) {
      select_uninitialized = false;
    }
    if (flags_ & FILTER_FROM_BUS) {
      select_bus = true;
    }
    if (clear) {
      list_.clear();
    }
    const service_dict_type * service_coll = &_local_services_;
    if (select_bus) {
      service_coll = &_service_bus_;
    }
    for (service_dict_type::const_iterator iserv = service_coll->begin();
         iserv != service_coll->end();
         iserv++) {
      const service_entry & serventry = *iserv->second.get();
      bool selected = true;
      if (serventry.is_initialized()) {
        if (!select_initialized) selected = false;
      } else {
        if (!select_uninitialized) selected = false;
      }
      if (selected) {
        list_.push_back(iserv->first);
      }
    }
    return;
  }

  bool service_manager::has(const std::string& name) const
  {
    return _local_services_.find(name) != _local_services_.end();
  }

  bool service_manager::is_initialized(const std::string& name) const
  {
    service_dict_type::const_iterator found = _local_services_.find(name);
    return found != _local_services_.end() && found->second->is_initialized();
  }

  const service_dict_type& service_manager::get_local_services() const
  {
    return _local_services_;
  }

  const service_dict_type& service_manager::get_bus_of_services(const bool update_) const
  {
    if (update_) {
      service_manager * mutable_this = const_cast<service_manager*>(this);
      mutable_this->update_service_bus();
    }
    return _service_bus_;
  }

  bool service_manager::can_drop(const std::string& name) const
  {
    if (!is_allow_dynamic_services() && this->is_initialized()) {
      return false;
    }
    service_dict_type::const_iterator found = _local_services_.find(name);
    DT_THROW_IF (found == _local_services_.end(),
                 std::logic_error,
                 "Service '" << name << "' does not exist !");
    return found->second->can_be_dropped();
  }

  void service_manager::drop(const std::string& name)
  {
    DT_THROW_IF(!is_allow_dynamic_services() && this->is_initialized(),
                std::logic_error, "Service manager is already initialized and does not allow service '" << name << "' to be dropped dynamically!");
    service_dict_type::iterator found = _local_services_.find(name);
    DT_THROW_IF (found == _local_services_.end(),
                 std::logic_error,
                 "Service '" << name << "' does not exist !");
    service_entry& entry = *found->second.get();
    DT_THROW_IF (!found->second->can_be_dropped(),
                 std::logic_error,
                 "Service '" << name << "' cannot be dropped because of existing dependent services !");
    DT_LOG_DEBUG(get_logging_priority(), "Reset & remove service '" << name << "' !");
    this->reset_service(entry);
    this->destroy_service(entry);
    return;
  }

  void service_manager::dump_services(std::ostream& out,
                                      const std::string& title,
                                      const std::string& an_indent) const
  {
    std::string indent;
    if (!an_indent.empty()) indent = an_indent;

    if (!title.empty()) out << title << ":" << std::endl;

    // Services:
    {
      size_t sz = _local_services_.size();
      size_t count = 0;
      for (service_dict_type::const_iterator it = _local_services_.begin();
           it != _local_services_.end();
           ++it) {
        count++;
        out << indent;
        if (count == sz) {
          out << "`-- ";
        } else {
          out << "|-- ";
        }
        const std::string& service_name = it->first;
        const service_entry& service_record = *it->second.get();

        out << "Name : '" << service_name << "' "
            << "Type : '" << service_record.get_service_id () << "' ";
        out << '(';
        int local_count = 0;
        if (service_record.is_initialized()) {
          out << "initialized";
          local_count++;
        }

        if (service_record.get_service_status () & service_entry::STATUS_BROKEN_DEPENDENCY) {
          if (local_count > 0) out << '/',
                                 out << "broken";
          local_count++;
        }
        out << ')';
        out << std::endl;
      }
    }
    return;
  }

  bool service_manager::has_service_type(const std::string& id) const
  {
    return _factory_register_.has(id);
  }

  void service_manager::unregister_service_type(const std::string& id)
  {
    _factory_register_.unregistration(id);
    return;
  }

  void service_manager::set_force_initialization_at_load(bool f_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Service manager is already initialized!");
    _force_initialization_at_load_ = f_;
    return;
  }

  bool service_manager::is_force_initialization_at_load() const
  {
    return _force_initialization_at_load_;
  }

  void service_manager::set_allow_dynamic_services(bool a_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Service manager is already initialized!");
    _allow_dynamic_services_ = a_;
    return;
  }

  bool service_manager::is_allow_dynamic_services() const
  {
    return _allow_dynamic_services_;
  }

  void service_manager::tree_dump(std::ostream& out,
                                  const std::string& title,
                                  const std::string& a_indent,
                                  bool inherit) const
  {
    std::string indent;
    if (!a_indent.empty()) indent = a_indent;

    if (!title.empty()) out << indent << title << std::endl;

    out << indent << i_tree_dumpable::tag
        << "Name           : '"
        << _name_
        << "'" << std::endl;

    out << indent << i_tree_dumpable::tag
        << "Description    : '"
        << _description_
        << "'" << std::endl;

    out << indent << i_tree_dumpable::tag
        << "Logging priority          : "
        << get_logging_priority() << std::endl;

    out << indent << i_tree_dumpable::tag
        << "Preload        : "
        << _preload_
        << std::endl;

    out << indent << i_tree_dumpable::tag
        << "Force initialization : "
        << _force_initialization_at_load_
        << std::endl;

    out << indent << i_tree_dumpable::tag
        << "Allow dynamic services : "
        << _allow_dynamic_services_
        << std::endl;

    out << indent << i_tree_dumpable::tag
        << "List of registered services : " << std::endl;
    {
      std::ostringstream indent_oss;
      indent_oss << indent << i_tree_dumpable::skip_tag;

      _factory_register_.print(out, indent_oss.str());
    }

    {
      out << indent << i_tree_dumpable::tag
          << "Services       : ";
      size_t sz = _local_services_.size();
      if (sz == 0) {
        out << "<none>";
      }

      out << std::endl;
      for (service_dict_type::const_iterator i = _local_services_.begin();
           i != _local_services_.end();
           ++i) {
        const std::string& service_name = i->first;
        const service_entry& service_entry = *i->second.get();
        out << indent << i_tree_dumpable::skip_tag;

        std::ostringstream indent_oss;
        indent_oss << indent << i_tree_dumpable::skip_tag;
        service_dict_type::const_iterator j = i;
        j++;
        if (j == _local_services_.end()) {
          out << i_tree_dumpable::last_tag;
          indent_oss << i_tree_dumpable::last_skip_tag;
        } else {
          out << i_tree_dumpable::tag;
          indent_oss << i_tree_dumpable::skip_tag;
        }
        out << "Service : '" << service_name << "'" << std::endl;
        service_entry.tree_dump(out, "", indent_oss.str());
      }
    }

    out << indent << i_tree_dumpable::inherit_tag(inherit)
        << "Initialized    : "
        << std::boolalpha
        << this->is_initialized()
        << std::endl;
    return;
  }

  void service_manager::at_service_creation(const std::string & service_name_,
                                            const std::string & service_type_id_)
  {
    DT_LOG_DEBUG(_logging_priority_,
                 "Creation of new service '" << service_name_ << "' of type '" <<  service_type_id_ << "'.");
    return;
  }

  void service_manager::at_service_initialization(const std::string & service_name_)
  {
    DT_LOG_DEBUG(_logging_priority_,
                 "Initialization of service '" << service_name_ << "'.");
    return;
  }

  void service_manager::at_service_reset(const std::string & service_name_)
  {
    DT_LOG_DEBUG(_logging_priority_,
                 "Reset of service '" << service_name_ << "'.");
    return;
  }

  void service_manager::at_service_drop(const std::string & service_name_)
  {
    DT_LOG_DEBUG(_logging_priority_,
                 "Dropped service '" << service_name_ << "'.");
    return;
  }

  //----------------------------------------------------------------------
  // Protected Interface Definitions
  //

  void service_manager::load_service(const std::string& name,
                                     const std::string& id,
                                     const datatools::properties* config_ptr)
  {
    DT_LOG_TRACE_ENTERING(get_logging_priority());
    DT_THROW_IF (this->has(name),
                 std::logic_error,
                 "Service '" << name << "' already exists !");
    {
      // Add a new entry :
      service_entry_ptr p(new service_entry(name, *this));
      // service_entry & tmp_entry = *p.get();
      DT_LOG_DEBUG(get_logging_priority(), "Add an entry for service '" << name << "'...");
      _local_services_[name] = p;
    }
    // Fetch a reference on it and update :
    service_entry& new_entry = *_local_services_.find(name)->second.get();
    new_entry.set_service_id(id);
    if (config_ptr != nullptr) {
      new_entry.set_service_config(*config_ptr);
    }
    this->create_service(new_entry);
    if (config_ptr != nullptr) {
      std::vector<std::string> strict_dependencies;
      if (config_ptr->has_key("dependencies.strict")) {
        config_ptr->fetch("dependencies.strict", strict_dependencies);
      }
      for (size_t i = 0; i < strict_dependencies.size(); ++i) {
        // XXX the_service_entry.service_masters;
      }
      for (service_dependency_dict_type::const_iterator j
             = new_entry.service_masters.begin();
           j != new_entry.service_masters.end();
           ++j) {
        const std::string& master_service_name = j->first;
        DT_LOG_DEBUG(get_logging_priority(),
                     "Master '"<< master_service_name << "!");
        continue;
      }
      if (_force_initialization_at_load_) {
        this->initialize_service(new_entry);
      }
    }
    DT_LOG_TRACE_EXITING(get_logging_priority());
    return;
  }

  void service_manager::load_service(const std::string& name,
                                     const std::string& id,
                                     const datatools::properties& config)
  {
    DT_LOG_TRACE_ENTERING(get_logging_priority());
    load_service(name, id, &config);
    DT_LOG_TRACE_EXITING(get_logging_priority());
    return;
  }


  void service_manager::preload_global_dict()
  {
    DT_LOG_TRACE_ENTERING(get_logging_priority());
    _factory_register_.import(DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(datatools::base_service));
    DT_LOG_TRACE_EXITING(get_logging_priority());
    return;
  }


  void service_manager::create_service(service_entry& entry)
  {
    if (!entry.is_created()) {
      DT_LOG_DEBUG(get_logging_priority(),
                   "Creating service named '" <<  entry.get_service_name()
                   << "'...");
      // search for the service's label in the factory dictionary:
      DT_THROW_IF (!_factory_register_.has(entry.get_service_id()),
                   std::logic_error,
                   "Cannot find service factory with ID '"
                   << entry.get_service_id()
                   << "' for service named '"
                   << entry.get_service_name() << "' !");

      // You don't think this might be a *little* too deeply nested?
      // Answer: Well, my deepest apologies for that, dear Ben!
      typedef datatools::base_service::factory_register_type::factory_type FactoryType;
      const FactoryType& the_factory = _factory_register_.get(entry.get_service_id());
      base_service* ptr = the_factory();
      entry.grab_service_handle().reset(ptr);
      ptr->set_name(entry.get_service_name());
      entry.update_service_status(service_entry::STATUS_CREATED);
      DT_LOG_DEBUG(get_logging_priority(),
                   "Service named '" <<  entry.get_service_name() << "' has been created !");
    }
    return;
  }

  void service_manager::initialize_service(service_entry& entry)
  {
    // If not created, first do it :
    if (!entry.is_created()) {
      this->create_service(entry);
    }

    // If not initialized, do it :
    if (!entry.is_initialized()) {
      DT_LOG_DEBUG(get_logging_priority(),
                   "Initializing service named '"
                   << entry.get_service_name()
                   << "'...");
      base_service& the_service = entry.grab_service_handle().grab();
      // the_service.set_name(entry.get_service_name());
      the_service.initialize(entry.get_service_config(), _local_services_);
      entry.update_service_status(service_entry::STATUS_INITIALIZED);
    }
    return;
  }

  void service_manager::reset_service(service_entry& entry)
  {
    if (entry.is_initialized()) {
      base_service& the_service = entry.grab_service_handle().grab();
      entry.reset_service_status(service_entry::STATUS_INITIALIZED);
      the_service.reset();
      for (service_dependency_dict_type::iterator i = entry.service_masters.begin();
           i != entry.service_masters.end();
           ++i) {
        const std::string& master_name = i->first;
        service_dict_type::iterator found = _local_services_.find(master_name);
        if (found != _local_services_.end()) {
          // service_entry& the_master_entry = found->second;
          // the_master_entry.remove_slave(entry.get_service_name());
          // DT_LOG_DEBUG(get_logging_priority(),
          //              "Remove slave '"
          //              << entry.get_service_name()
          //              << "' from master '"
          //              << the_master_entry.get_service_name()
          //              << "' !");
        }
      }
    }
    return;
  }

  void service_manager::destroy_service(service_entry& entry)
  {
    if (entry.is_created()) {
      if (entry.is_initialized()) {
        reset_service(entry);
      }
      entry.reset_service_status(service_entry::STATUS_CREATED);
      entry.grab_service_handle().reset();
    }
    service_dict_type::iterator found = _local_services_.find(entry.get_service_name());
    _local_services_.erase(found);
    return;
  }

  //----------------------------------------------------------------------
  // Private Interface Definitions

  void service_manager::set_preload(bool preload_)
  {
    _preload_ = preload_;
    if (_preload_) {
      this->preload_global_dict();
    }
    return;
  }

}  // end of namespace datatools

/***************
 * OCD support *
 ***************/

#include <datatools/ocd_macros.h>

// OCD support for class '::datatools::service_manager' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::datatools::service_manager,ocd_)
{
  ocd_.set_class_name ("datatools::service_manager");
  ocd_.set_class_description ("A generic manager for service objects");
  ocd_.set_class_library ("datatools");
  ocd_.set_class_documentation ("A *service manager* is responsible of the instantiation,           \n"
                                "initialization, management and destruction of various *service*    \n"
                                "objects.                                                           \n"
                                "A service is a special object that fulfils the generic             \n"
                                "``datatools::base_service`` interface. A service is generally      \n"
                                "dedicated to some special task : description of the geometry,      \n"
                                "runtime access to a database, management of a dynamic pool of      \n"
                                "histograms for data analysis... Such service objects are hosted    \n"
                                "by the *service manager* which provides an unique counter for      \n"
                                "end-users and client applications to configure, initialize and     \n"
                                "access to the functionalities offered by the on-board services.    \n"
                                "A service manager can be configured *manually* from plain C++ code.\n"
                                "Is is also possible to initialize it through a set of ASCII        \n"
                                "configuration files, thus dynamically instantiating a list of      \n"
                                "services with various embedded functionalities.                     \n"
                                )
    ;

  datatools::logger::declare_ocd_logging_configuration(ocd_, "warning");

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("name")
      .set_terse_description("The name of the service manager object")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description(
                            "Superseded by the previous method call :                 \n"
                            "  ``datatools::service_manager::set_name(...)``          \n"
                            "                                                         \n"
                            )
      .add_example("The name of the manager: ::                              \n"
                   "                                                         \n"
                   "   name : string = \"DASM\"                              \n"
                   "                                                         \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("description")
      .set_terse_description("The embeded description string")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description(
                            "Superseded by the previous method call :                 \n"
                            "  ``datatools::service_manager::set_description(...)``   \n"
                            "                                                         \n"
                            )
      .add_example("The manager's description string: ::                                \n"
                   "                                                                    \n"
                   "   description : string = \"The service manager for data analysis\" \n"
                   "                                                                    \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("services.configuration_files")
      .set_terse_description("A list of configuration filenames for embeded services")
      .set_traits(datatools::TYPE_STRING,
                  configuration_property_description::ARRAY)
      .set_mandatory(false)
      .set_path(true)
      .set_long_description(
                            "A list of filenames from where the manager               \n"
                            "loads the directives to dynamically instantiate          \n"
                            "new embedded service objects. The filenames may contains \n"
                            "some environment variables.                              \n"
                            "                                                         \n"
                            "The target files must use the format of the              \n"
                            "``datatools::multi_properties`` class.                   \n"
                            "The loading order of the files is critical               \n"
                            "because some services may depend on other ones           \n"
                            "which should thus be defined **before** their            \n"
                            "dependers.                                               \n"
                            "                                                         \n"
                            "Extends the instantiation of services triggered by       \n"
                            "previous method calls :                                  \n"
                            "  ``datatools::service_manager::load(...)``              \n"
                            )
      .add_example(
                   "A service manager with two sets of services: ::          \n"
                   "                                                         \n"
                   "   services.configuration_files : string[2] as path = \\ \n"
                   "     \"${CONFIG_REPOSITORY_DIR}/services.conf\" \\       \n"
                   "     \"${CONFIG_REPOSITORY_DIR}/other_services.conf\"    \n"
                   )
      ;
  }

  ocd_.set_configuration_hints ("The configuration of a ``datatools::service_manager`` object  \n"
                                "can use the ``datatools::properties`` format. The services    \n"
                                "objects handled by the manager are themselves defined in a    \n"
                                "list of using the ``datatools::multi_properties`` format.     \n"
                                );
  ocd_.add_example ("Configuration example for a ``datatools::service_manager``       \n"
                    "object:: \n"
                    "                                                                 \n"
                    "   logging.priority : string = \"warning\"                       \n"
                    "   name  : string = \"CoreServices\"                             \n"
                    "   description  : string = \\                                    \n"
                    "     \"Core services manager for data processing\"               \n"
                    "   services.configuration_files : string[3] as path = \\         \n"
                    "     \"${MY_CONFIG_DIR}/services/database_services.conf\" \\     \n"
                    "     \"${MY_CONFIG_DIR}/services/io_services.conf\" \\           \n"
                    "     \"${MY_CONFIG_DIR}/services/processing_services.conf\"      \n"
                    "                                                                 \n"
                    "where the ``${MY_CONFIG_DIR}/services/database_services.conf``,  \n"
                    "``${MY_CONFIG_DIR}/services/io_services.conf`` and               \n"
                    "``${MY_CONFIG_DIR}/services/processing_services.conf`` files     \n"
                    "use the ``datatools::multi_properties`` ASCII format.            \n"
                    );
  ocd_.add_example("This is the definition sample list of some services in the      \n"
                   "``datatools::multi_properties`` format: ::                      \n"
                   "                                                                \n"
                   "  #@description A list of services                              \n"
                   "  #@key_label   \"name\"                                        \n"
                   "  #@meta_label  \"type\"                                        \n"
                   "                                                                \n"
                   "  [name=\"service_0\" type=\"service_class_ID_0\"]              \n"
                   "                                                                \n"
                   "  #@config A service object                                     \n"
                   "                                                                \n"
                   "  #@description A configuration parameter                       \n"
                   "  logging.priority : string =  \"warning\"                      \n"
                   "                                                                \n"
                   "  #@description Another configuration parameter                 \n"
                   "  mode : string = \"remote\"                                    \n"
                   "                                                                \n"
                   "  ...                                                           \n"
                   "                                                                \n"
                   "  [name=\"service_1\" type=\"service_class_ID_1\"]              \n"
                   "                                                                \n"
                   "  #@config Another service object                               \n"
                   "                                                                \n"
                   "  #@description A configuration parameter                       \n"
                   "  logging.priority : string =  \"information\"                  \n"
                   "                                                                \n"
                   "  #@description Another configuration parameter                 \n"
                   "  algorithm : string = \"experimental\"                         \n"
                   "                                                                \n"
                   "  ...                                                           \n"
                   "                                                                \n"
                   );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()

DOCD_CLASS_SYSTEM_REGISTRATION(datatools::service_manager,"datatools::service_manager")
