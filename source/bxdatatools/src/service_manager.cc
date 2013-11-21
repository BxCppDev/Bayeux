/* service_manager.cc
 *
 * Copyright (C) 2011-2012 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
// Ourselves
#include <datatools/service_manager.h>

// Standard Library
#include <stdexcept>
#include <sstream>

// Third Party

// Datatools
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

// Default logger interface

// Logging features:
void service_manager::set_logging_priority(::datatools::logger::priority p)
{
  _logging_priority = p;
}

datatools::logger::priority service_manager::get_logging_priority() const
{
  return _logging_priority;
}

void service_manager::set_name(const std::string& name) {
  name_ = name;
}


void service_manager::set_description(const std::string& description) {
  description_ = description;
}


const std::string& service_manager::get_name() const {
  return name_;
}


const std::string& service_manager::get_description() const {
  return description_;
}


bool service_manager::is_debug() const {
  return get_logging_priority() >= datatools::logger::PRIO_DEBUG;
}


void service_manager::set_debug(bool debug) {
  if (debug) {
    set_logging_priority(datatools::logger::PRIO_DEBUG);
  } else {
    set_logging_priority(datatools::logger::PRIO_WARNING);
  }
}


bool service_manager::is_initialized() const {
  return initialized_;
}


void service_manager::load(const std::string& name,
                           const std::string& id,
                           const datatools::properties& config) {
  this->load_service(name, id, config);
}


void service_manager::load(const datatools::multi_properties& config) {
  DT_LOG_TRACE(get_logging_priority(), "Entering...");
  DT_THROW_IF(this->is_initialized(), std::logic_error, "Service manager is already initialized !");
  for (multi_properties::entries_ordered_col_type::const_iterator i = config.ordered_entries().begin();
       i != config.ordered_entries().end();
       ++i) {
    multi_properties::entry* mpe = *i;
    const std::string& service_name = mpe->get_key();
    const std::string& service_id = mpe->get_meta();
    DT_LOG_DEBUG (get_logging_priority(), "Configuration for service named '" << service_name << "' "
                  << " with ID '" << service_id << "'...");
    this->load_service(service_name, service_id, mpe->get_properties());
  }
}


void service_manager::initialize(const datatools::properties& config) {
  DT_THROW_IF(this->is_initialized(), std::logic_error, "Service manager is already initialized !");

  datatools::logger::priority p = datatools::logger::extract_logging_configuration(config);
  set_logging_priority (p);

  if (name_.empty()) {
    if (config.has_key("name")) {
      name_ = config.fetch_string("name");
    }
  }

  if (description_.empty()) {
    if (config.has_key("description")) {
      description_ = config.fetch_string("description");
    }
  }

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
      mconfig.read(*i);
      this->load(mconfig);
    }
  }

  initialized_ = true;
}


void service_manager::initialize() {
  datatools::properties dummy_config;
  this->initialize(dummy_config);
}


void service_manager::reset() {
  DT_LOG_TRACE(get_logging_priority(),"Entering...");
  DT_THROW_IF(!initialized_,std::logic_error,"Manager is not initialized !");
  size_t count = services_.size();
  size_t initial_size = services_.size();
  while (services_.size() > 0) {
    for (service_dict_type::iterator it = services_.begin();
         it != services_.end();
         ++it) {
      service_entry& entry = it->second;

      if (entry.can_be_dropped()) {
        DT_LOG_DEBUG(get_logging_priority(),"Removing service '" << entry.get_service_name ()  << "'...");
        this->reset_service(entry);
        services_.erase(it);
        --count;
        break;
      }
    }
    if (count == initial_size) {
      break;
    }
  }
  if (services_.size() > 0) {
    DT_LOG_WARNING(get_logging_priority(),"There are some left services !");
  }
  services_.clear();
  factory_register_.reset();
  force_initialization_at_load_ = false;
  preload_ = true;
  initialized_ = false;
  DT_LOG_TRACE(get_logging_priority(),"Exiting.");
}


// Constructor :
service_manager::service_manager(const std::string& name,
                                 const std::string& description,
                                 uint32_t flag)
    : factory_register_("datatools::base_service/service_factory",
                        flag & FACTORY_VERBOSE ?
                        base_service::factory_register_type::verbose : 0) {
  initialized_ = false;
  this->set_name(name);
  this->set_description(description);

  set_logging_priority(datatools::logger::PRIO_WARNING);
  force_initialization_at_load_ = false;

  if (flag & FORCE_INITIALIZATION_AT_LOAD) {
    force_initialization_at_load_ = true;
  }

  bool preload = true;
  if (flag & NO_PRELOAD) {
    preload = false;
  }

  this->set_preload(preload);
}


// Destructor :
service_manager::~service_manager () {
  if (initialized_) this->reset();
}


/****************
 *   SERVICES   *
 ****************/

bool service_manager::has(const std::string& name) const {
  return services_.find(name) != services_.end();
}

bool service_manager::is_initialized(const std::string& name) const {
  service_dict_type::const_iterator found = services_.find(name);
  return found != services_.end() && found->second.is_initialized();
}

// Who needs this (services is an implementation detail....)
const service_dict_type& service_manager::get_services() const {
  return services_;
}


service_dict_type& service_manager::get_services() {
  return services_;
}


bool service_manager::can_drop(const std::string& name) {
  service_dict_type::const_iterator found = services_.find(name);
  DT_THROW_IF (found == services_.end(),
               std::logic_error,
               "Service '" << name << "' does not exist !");
  return found->second.can_be_dropped();
}


void service_manager::drop(const std::string& name) {
  service_dict_type::iterator found = services_.find(name);
  DT_THROW_IF (found == services_.end(),
               std::logic_error,
               "Service '" << name << "' does not exist !");
  service_entry& entry = found->second;
  DT_THROW_IF (!found->second.can_be_dropped(),
               std::logic_error,
               "Service '" << name << "' cannot be dropped because of existing dependent services !");
  DT_LOG_DEBUG(get_logging_priority(), "Reset & remove service '" << name << "' !");
  this->reset_service(entry);
  services_.erase(found);
}


void service_manager::dump_services(std::ostream& out,
                                    const std::string& title,
                                    const std::string& an_indent) const {
  std::string indent;
  if (!an_indent.empty()) indent = an_indent;

  if (!title.empty()) out << title << ":" << std::endl;

  // Services:
  {
    size_t sz = services_.size();
    size_t count = 0;
    for (service_dict_type::const_iterator it = services_.begin();
         it != services_.end();
         ++it) {
      count++;
      out << indent;
      if (count == sz) {
        out << "`-- ";
      } else {
        out << "|-- ";
      }
      const std::string& service_name = it->first;
      const service_entry& service_record = it->second;

      out << "Name : '" << service_name << "' "
          << "Type : '" << service_record.get_service_id () << "' ";
      out << '(';
      int count = 0;
      if (service_record.is_initialized()) {
        out << "initialized";
        count++;
      }

      if (service_record.get_service_status () & service_entry::STATUS_BROKEN_DEPENDENCY) {
        if (count > 0) out << '/',
        out << "broken";
        count++;
      }
      out << ')';
      out << std::endl;
    }
  }
}


bool service_manager::has_service_type(const std::string& id) const {
  return factory_register_.has(id);
}


void service_manager::unregister_service_type(const std::string& id) {
  factory_register_.unregistration(id);
}


void service_manager::tree_dump(std::ostream& out,
                                const std::string& title,
                                const std::string& a_indent,
                                bool inherit) const {
  std::string indent;
  if (!a_indent.empty()) indent = a_indent;

  if (!title.empty()) out << indent << title << std::endl;

  out << indent << i_tree_dumpable::tag
      << "Name           : '"
      << name_
      << "'" << std::endl;

  out << indent << i_tree_dumpable::tag
      << "Description    : '"
      << description_
      << "'" << std::endl;

  out << indent << i_tree_dumpable::tag
      << "Logging priority          : "
      << get_logging_priority() << std::endl;

  out << indent << i_tree_dumpable::tag
      << "Preload        : "
      << preload_
      << std::endl;

  out << indent << i_tree_dumpable::tag
      << "Force initialization : "
      << force_initialization_at_load_
      << std::endl;

  out << indent << i_tree_dumpable::tag
      << "List of registered services : " << std::endl;
  {
    std::ostringstream indent_oss;
    indent_oss << indent << i_tree_dumpable::skip_tag;

    factory_register_.print(out, indent_oss.str());
  }

  {
    out << indent << i_tree_dumpable::tag
        << "Services       : ";
    size_t sz = services_.size();
    if (sz == 0) {
      out << "<none>";
    }

    out << std::endl;
    for (service_dict_type::const_iterator i = services_.begin();
         i != services_.end();
         ++i) {
      const std::string& service_name = i->first;
      const service_entry& service_entry = i->second;
      out << indent << i_tree_dumpable::skip_tag;

      std::ostringstream indent_oss;
      indent_oss << indent << i_tree_dumpable::skip_tag;
      service_dict_type::const_iterator j = i;
      j++;
      if (j == services_.end()) {
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
      << this->is_initialized()
      << std::endl;
}


//----------------------------------------------------------------------
// Protected Interface Definitions
//

void service_manager::load_service(const std::string& name,
                                   const std::string& id,
                                   const datatools::properties& config) {
  DT_LOG_TRACE(get_logging_priority(), "Entering...");
  DT_THROW_IF (this->has(name),
               std::logic_error,
               "Service '" << name << "' already exists !");
  {
    // Add a new entry :
    service_entry tmp_entry;
    tmp_entry.set_service_name(name);
    DT_LOG_DEBUG(get_logging_priority(),
                 "Add an entry for service '"<< name << "'...");
    services_[name] = tmp_entry;
  }
  // fetch a reference on it and update :
  service_entry& new_entry = services_.find(name)->second;
  new_entry.set_service_id(id);
  new_entry.set_service_config(config);
  //new_entry.service_status = service_entry::STATUS_BLANK;
  DT_LOG_DEBUG(get_logging_priority(),"Fetch...");
  this->create_service(new_entry);
  std::vector<std::string> strict_dependencies;
  if (config.has_key("dependencies.strict")) {
    config.fetch("dependencies.strict", strict_dependencies);
  }
  for (size_t i = 0; i < strict_dependencies.size(); ++i) {
    // XXX the_service_entry.service_masters;
  }

  // XXX:
  //the_service_entry.get_service_handle().get ().fetch_dependencies (the_service_entry.service_masters);
  for (service_dependency_dict_type::const_iterator j
         = new_entry.service_masters.begin();
       j != new_entry.service_masters.end();
       ++j) {
    const std::string& master_service_name = j->first;
    DT_LOG_DEBUG(get_logging_priority(),
                 "Master '"<< master_service_name << "!");
    continue;
  }
  if (force_initialization_at_load_) {
    this->initialize_service(new_entry);
  }
  DT_LOG_TRACE(get_logging_priority(),"Exiting.");
}


void service_manager::preload_global_dict() {
  DT_LOG_TRACE(get_logging_priority(),"Entering !");
  factory_register_.import(DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(datatools::base_service));
  DT_LOG_TRACE(get_logging_priority(),"Embedded.");
}


void service_manager::create_service(service_entry& entry) {
  if (!entry.is_created()) {
    DT_LOG_DEBUG(get_logging_priority(),
                 "Creating service named '" <<  entry.get_service_name()
                 << "'...");
    // search for the service's label in the factory dictionary:
    DT_THROW_IF (!factory_register_.has(entry.get_service_id()),
                 std::logic_error,
                 "Cannot find service factory with ID '"
                 << entry.get_service_id()
                 << "' for service named '"
                 << entry.get_service_name() << "' !");

    // You don't think this might be a *little* too deeply nested?
    typedef
        datatools::base_service::factory_register_type::factory_type       FactoryType;

    const FactoryType& the_factory = factory_register_.get(entry.get_service_id());
    base_service* ptr = the_factory();
    entry.grab_service_handle().reset(ptr);
    entry.update_service_status(service_entry::STATUS_CREATED);
    DT_LOG_DEBUG(get_logging_priority(),
                 "Service named '" <<  entry.get_service_name() << "' has been created !");
  }
}


void service_manager::initialize_service(service_entry& entry) {
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
    the_service.initialize(entry.get_service_config(), services_);
    entry.update_service_status(service_entry::STATUS_INITIALIZED);
  }
}


void service_manager::reset_service(service_entry& entry) {
  if (entry.is_initialized()) {
    base_service& the_service = entry.grab_service_handle().grab();
    the_service.reset();
    entry.reset_service_status(service_entry::STATUS_INITIALIZED);
    for (service_dependency_dict_type::iterator i = entry.service_masters.begin();
         i != entry.service_masters.end();
         ++i) {
      const std::string& master_name = i->first;
      service_dict_type::iterator found = services_.find(master_name);
      if (found != services_.end()) {
        service_entry& the_master_entry = found->second;
        the_master_entry.remove_slave(entry.get_service_name());
        DT_LOG_DEBUG(get_logging_priority(),
                     "Remove slave '"
                     << entry.get_service_name()
                     << "' from master '"
                     << the_master_entry.get_service_name()
                     << "' !");
      }
    }
  }
}


//----------------------------------------------------------------------
// Private Interface Definitions

void service_manager::set_preload(bool preload) {
  preload_ = preload;
  if (preload_) {
    this->preload_global_dict();
  }
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


  ocd_.set_configuration_hints ("Configuration example for a ``datatools::service_manager`` object:: \n"
                                "                                                              \n"
                                "   logging.priority : string = \"warning\"                    \n"
                                "   name  : string = \"CoreServices\"                          \n"
                                "   description  : string = \\                                 \n"
                                "     \"Core services manager for data processing\"            \n"
                                "   services.configuration_files : string[3] as path = \\      \n"
                                "     \"${CONFIG_DIR}/database_services.conf\" \\              \n"
                                "     \"${CONFIG_DIR}/io_services.conf\" \\                    \n"
                                "     \"${CONFIG_DIR}/processing_services.conf\"               \n"
                                "                                                              \n"
                                "The ``${CONFIG_DIR}/database_services.conf``,                 \n"
                                "``${CONFIG_DIR}/io_services.conf`` and                        \n"
                                "``${CONFIG_DIR}/processing_services.conf`` files use          \n"
                                "the ``datatools::multi_properties`` ASCII format.             \n"
                                "                                                                \n"
                                "***Example :*** ::                                              \n"
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

// end of service_manager.cc
