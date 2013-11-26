/* component_manager.cc
 *
 * Copyright (C) 2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
#include <electronics/component_manager.h>

// Standard Library
#include <stdexcept>
#include <sstream>

// Third Party
// - Bayeux/datatools
#include <datatools/ioutils.h>
#include <datatools/utils.h>
#include <datatools/properties.h>
#include <datatools/multi_properties.h>
#include <datatools/exception.h>
#include <datatools/logger.h>

// This project
#include <electronics/component_model_base.h>

namespace electronics {

  //----------------------------------------------------------------------
  // Public Interface Definitions
  //

  // Default logger interface

  // Logging features:
  void component_manager::set_logging_priority(::datatools::logger::priority p_)
  {
    _logging_priority_ = p_;
    return;
  }

  datatools::logger::priority component_manager::get_logging_priority() const
  {
    return _logging_priority_;
  }

  void component_manager::set_setup_label(const std::string& label_) {
    _setup_label_ = label_;
    return;
  }

  void component_manager::set_setup_version(const std::string& version_) {
    _setup_version_ = version_;
    return;
  }

  void component_manager::set_setup_description(const std::string& description_) {
    _setup_description_ = description_;
    return;
  }

  const std::string& component_manager::get_setup_label() const {
    return _setup_label_;
  }

  const std::string& component_manager::get_setup_version() const {
    return _setup_version_;
  }

  const std::string& component_manager::get_setup_description() const {
    return _setup_description_;
  }

  bool component_manager::is_debug() const {
    return get_logging_priority() >= datatools::logger::PRIO_DEBUG;
  }

  void component_manager::set_debug(bool debug_) {
    if (debug_) {
      set_logging_priority(datatools::logger::PRIO_DEBUG);
    } else {
      set_logging_priority(datatools::logger::PRIO_WARNING);
    }
    return;
  }

  bool component_manager::is_initialized() const {
    return _initialized_;
  }

  void component_manager::load(const std::string& name_,
                               const std::string& id_,
                               const datatools::properties& config_) {
    this->load_component(name_, id_, config_);
    return;
  }


  void component_manager::load(const datatools::multi_properties& config_) {
    DT_LOG_TRACE(get_logging_priority(), "Entering...");
    DT_THROW_IF(this->is_initialized(), std::logic_error,
                "Component manager is already initialized !");
    for (datatools::multi_properties::entries_ordered_col_type::const_iterator i
           = config_.ordered_entries().begin();
         i != config_.ordered_entries().end();
         ++i) {
      datatools::multi_properties::entry* mpe = *i;
      const std::string& component_name = mpe->get_key();
      const std::string& component_id = mpe->get_meta();
      DT_LOG_DEBUG (get_logging_priority(), "Configuration for component named '"
                    << component_name << "' "
                    << " with ID '" << component_id << "'...");
      this->load_component(component_name, component_id, mpe->get_properties());
    }
    return;
  }


  void component_manager::initialize(const datatools::properties& config_) {
    DT_THROW_IF(this->is_initialized(), std::logic_error, "Component manager is already initialized !");

    datatools::logger::priority p = datatools::logger::extract_logging_configuration(config_);
    set_logging_priority (p);

    if (_setup_label_.empty()) {
      if (config_.has_key("setup_label")) {
        _setup_label_ = config_.fetch_string("setup_label");
      }
    }

    if (_setup_version_.empty()) {
      if (config_.has_key("setup_version")) {
        _setup_version_ = config_.fetch_string("setup_version");
      }
    }

    if (_setup_description_.empty()) {
      if (config_.has_key("setup_description")) {
        _setup_description_ = config_.fetch_string("setup_description");
      }
    }

    // components :
    {
      typedef std::vector<std::string> CFList;
      typedef CFList::iterator CFListIterator;
      std::string conf_file_key("components.configuration_files");

      CFList conf_file_list;

      if (config_.has_key(conf_file_key)) {
        config_.fetch(conf_file_key, conf_file_list);
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

    // EID manager:
    std::vector<std::string> eid_mgr_defs;
    {
      if (config_.has_key("eid_manager.categories")) {
        config_.fetch("eid_manager.categories", eid_mgr_defs);
      }
    }

    for (int i = 0; i < eid_mgr_defs.size(); i++) {
      load_eid_categories(eid_mgr_defs[i]);
    }

    _initialized_ = true;
    return;
  }

  void component_manager::load_eid_categories(const std::string & file_)
  {
    std::string ftmp = file_;
    datatools::fetch_path_with_env(ftmp);
    _eid_manager_.load(ftmp);
    return;
  }

  void component_manager::initialize() {
    datatools::properties dummy_config;
    this->initialize(dummy_config);
    return;
  }


  void component_manager::reset() {
    DT_LOG_TRACE(get_logging_priority(),"Entering...");
    DT_THROW_IF(!_initialized_,std::logic_error,"Manager is not initialized !");
    size_t count = _components_.size();
    size_t initial_size = _components_.size();
    while (_components_.size() > 0) {
      for (component_pool_type::iterator it = _components_.begin();
           it != _components_.end();
           ++it) {
        component_entry& entry = it->second;
        DT_LOG_DEBUG(get_logging_priority(), "Removing component '" << entry.get_component_name ()  << "'...");
        this->reset_component(entry);
        _components_.erase(it);
        --count;
        break;
      }
      if (count == initial_size) {
        break;
      }
    }
    if (_components_.size() > 0) {
      DT_LOG_WARNING(get_logging_priority(), "There are some left components !");
    }
    _eid_manager_.reset();
    _components_.clear();
    _factory_register_.reset();
    _force_initialization_at_load_ = false;
    _preload_ = true;
    _initialized_ = false;
    DT_LOG_TRACE(get_logging_priority(), "Exiting.");
    return;
  }


  // Constructor :
  component_manager::component_manager(uint32_t flag)
    : _factory_register_("electronics::component_model_base/component_factory",
                        flag & FACTORY_VERBOSE ?
                        component_model_base::factory_register_type::verbose : 0) {
    _initialized_ = false;

    set_logging_priority(datatools::logger::PRIO_WARNING);
    _force_initialization_at_load_ = false;

    if (flag & FORCE_INITIALIZATION_AT_LOAD) {
      _force_initialization_at_load_ = true;
    }

    bool preload = true;
    if (flag & NO_PRELOAD) {
      preload = false;
    }

    this->_set_preload_(preload);
    return;
  }


  // Destructor :
  component_manager::~component_manager () {
    if (_initialized_) this->reset();
    return;
  }

  const geomtools::id_mgr & component_manager::get_eid_manager() const
  {
    return _eid_manager_;
  }

  /****************
   *   COMPONENTS   *
   ****************/

  bool component_manager::has(const std::string& name) const {
    return _components_.find(name) != _components_.end();
  }

  bool component_manager::is_initialized(const std::string& name) const {
    component_pool_type::const_iterator found = _components_.find(name);
    return found != _components_.end() && found->second.is_initialized();
  }

  // Who needs this (components is an implementation detail....)
  const component_pool_type& component_manager::get_components() const {
    return _components_;
  }


  component_pool_type& component_manager::grab_components() {
    return _components_;
  }

  void component_manager::dump_components(std::ostream& out,
                                          const std::string& title,
                                          const std::string& an_indent) const {
    std::string indent;
    if (!an_indent.empty()) indent = an_indent;

    if (!title.empty()) out << title << ":" << std::endl;

    // Components:
    {
      size_t sz = _components_.size();
      size_t count = 0;
      for (component_pool_type::const_iterator it = _components_.begin();
           it != _components_.end();
           ++it) {
        count++;
        out << indent;
        if (count == sz) {
          out << "`-- ";
        } else {
          out << "|-- ";
        }
        const std::string& component_name = it->first;
        const component_entry& component_record = it->second;

        out << "Name : '" << component_name << "' "
            << "Type : '" << component_record.get_component_id () << "' ";
        out << '(';
        int count = 0;
        if (component_record.is_initialized()) {
          out << "initialized";
          count++;
        }
        out << std::endl;
      }
    }
    return;
  }


  bool component_manager::has_component_type(const std::string& id) const {
    return _factory_register_.has(id);
  }


  void component_manager::unregister_component_type(const std::string& id) {
    _factory_register_.unregistration(id);
    return;
  }


  void component_manager::tree_dump(std::ostream& out,
                                    const std::string& title,
                                    const std::string& a_indent,
                                    bool inherit) const {
    std::string indent;
    if (!a_indent.empty()) indent = a_indent;

    if (!title.empty()) out << indent << title << std::endl;

    out << indent << i_tree_dumpable::tag
        << "Setup label       : '"
        << _setup_label_
        << "'" << std::endl;

    out << indent << i_tree_dumpable::tag
        << "Setup version     : '"
        << _setup_version_
        << "'" << std::endl;

    out << indent << i_tree_dumpable::tag
        << "Setup description : '"
        << _setup_description_
        << "'" << std::endl;

    out << indent << i_tree_dumpable::tag
        << "Logging priority : '"
        << datatools::logger::get_priority_label(get_logging_priority())
        << "'" << std::endl;

    out << indent << i_tree_dumpable::tag
        << "Preload        : "
        << _preload_
        << std::endl;

    out << indent << i_tree_dumpable::tag
        << "Force initialization : "
        << _force_initialization_at_load_
        << std::endl;

    out << indent << i_tree_dumpable::tag
        << "List of registered components : " << std::endl;
    {
      std::ostringstream indent_oss;
      indent_oss << indent << i_tree_dumpable::skip_tag;

      _factory_register_.print(out, indent_oss.str());
    }

    {
      out << indent << i_tree_dumpable::tag
          << "Components       : ";
      size_t sz = _components_.size();
      if (sz == 0) {
        out << "<none>";
      }

      out << std::endl;
      for (component_pool_type::const_iterator i = _components_.begin();
           i != _components_.end();
           ++i) {
        const std::string& component_name = i->first;
        const component_entry& component_entry = i->second;
        out << indent << i_tree_dumpable::skip_tag;

        std::ostringstream indent_oss;
        indent_oss << indent << i_tree_dumpable::skip_tag;
        component_pool_type::const_iterator j = i;
        j++;
        if (j == _components_.end()) {
          out << i_tree_dumpable::last_tag;
          indent_oss << i_tree_dumpable::last_skip_tag;
        } else {
          out << i_tree_dumpable::tag;
          indent_oss << i_tree_dumpable::skip_tag;
        }
        out << "Component : '" << component_name << "'" << std::endl;
        component_entry.tree_dump(out, "", indent_oss.str());
      }
    }

    {
      out << indent << i_tree_dumpable::tag
          << "EID manager    : "
          << std::endl;
      std::ostringstream indent_oss;
      indent_oss << indent << i_tree_dumpable::skip_tag;
      _eid_manager_.tree_dump(std::clog, "", indent_oss.str());
    }

    {
      out << indent << i_tree_dumpable::inherit_tag(inherit)
          << "Initialized    : "
          << this->is_initialized()
          << std::endl;
    }

    return;
  }


  //----------------------------------------------------------------------
  // Protected Interface Definitions
  //

  void component_manager::load_component(const std::string& name,
                                         const std::string& id,
                                         const datatools::properties& config) {
    DT_LOG_TRACE(get_logging_priority(), "Entering...");
    DT_THROW_IF (this->has(name),
                 std::logic_error,
                 "Component '" << name << "' already exists !");
    {
      // Add a new entry :
      component_entry tmp_entry;
      tmp_entry.set_component_name(name);
      DT_LOG_DEBUG(get_logging_priority(),
                   "Add an entry for component '"<< name << "'...");
      _components_[name] = tmp_entry;
    }
    // fetch a reference on it and update :
    component_entry& new_entry = _components_.find(name)->second;
    new_entry.set_component_manager(*this);
    new_entry.set_component_id(id);
    new_entry.set_component_config(config);
    //new_entry.component_status = component_entry::STATUS_BLANK;
    DT_LOG_DEBUG(get_logging_priority(),"Fetch...");
    this->create_component(new_entry);
    if (_force_initialization_at_load_) {
      this->initialize_component(new_entry);
    }
    DT_LOG_TRACE(get_logging_priority(),"Exiting.");
    return;
  }


  void component_manager::preload_global_dict() {
    DT_LOG_TRACE(get_logging_priority(),"Entering !");
    _factory_register_.import(DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(component_model_base));
    DT_LOG_TRACE(get_logging_priority(),"Embeded.");
    return;
  }


  void component_manager::create_component(component_entry& entry) {
    if (!entry.is_created()) {
      DT_LOG_DEBUG(get_logging_priority(),
                   "Creating component named '" <<  entry.get_component_name()
                   << "'...");
      // search for the component's label in the factory dictionary:
      DT_THROW_IF (!_factory_register_.has(entry.get_component_id()),
                   std::logic_error,
                   "Cannot find component factory with ID '"
                   << entry.get_component_id()
                   << "' for component named '"
                   << entry.get_component_name() << "' !");

      typedef component_model_base::factory_register_type::factory_type FactoryType;
      const FactoryType& the_factory = _factory_register_.get(entry.get_component_id());
      component_model_base* ptr = the_factory();
      ptr->set_name(entry.get_component_name());
      entry.grab_component_handle().reset(ptr);
      entry.update_component_status(component_entry::STATUS_CREATED);
      DT_LOG_DEBUG(get_logging_priority(),
                   "Component named '" <<  entry.get_component_name() << "' has been created !");
    }
    return;
  }


  void component_manager::initialize_component(component_entry& entry) {
    // If not created, first do it :
    if (!entry.is_created()) {
      this->create_component(entry);
    }

    // If not initialized, do it :
    if (!entry.is_initialized()) {
      DT_LOG_DEBUG(get_logging_priority(),
                   "Initializing component named '"
                   << entry.get_component_name()
                   << "'...");
      component_model_base& the_component = entry.grab_component_handle().grab();
      the_component.initialize(entry.get_component_config(), _components_);
      entry.update_component_status(component_entry::STATUS_INITIALIZED);
    }
    return;
  }


  void component_manager::reset_component(component_entry& entry) {
    if (entry.is_initialized()) {
      component_model_base& the_component = entry.grab_component_handle().grab();
      the_component.reset();
      entry.reset_component_status(component_entry::STATUS_INITIALIZED);
    }
    return;
  }


  //----------------------------------------------------------------------
  // Private Interface Definitions

  void component_manager::_set_preload_(bool preload) {
    _preload_ = preload;
    if (_preload_) {
      this->preload_global_dict();
    }
    return;
  }

} // end of namespace electronics


/***************
 * OCD support *
 ***************/

#include <datatools/ocd_macros.h>

// OCD support for class '::electronics::component_manager' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::electronics::component_manager,ocd_)
{
  ocd_.set_class_name ("::electronics::component_manager");
  ocd_.set_class_description ("A generic manager for electronics component models");
  ocd_.set_class_library ("Bayeux/electronics");
  ocd_.set_class_documentation ("A *component manager* is responsible of the instantiation,           \n"
                                "initialization, management and destruction of various electronics    \n"
                                "*component* models.                                                  \n"
                                "A component model is a special object that fulfils the generic       \n"
                                "``electronics::component_model_base`` interface. A component model is\n"
                                "generally dedicated to some special task : description of a crate,   \n"
                                "a board, a chip... Such component models are hosted by the           \n"
                                "*component manager* which provides an unique counter for             \n"
                                "end-users and client applications to configure, initialize and       \n"
                                "access to the functionalities offered by the component models.       \n"
                                "A component manager can be configured *manually* from plain C++ code.\n"
                                "Is is also possible to initialize it through a set of ASCII          \n"
                                "configuration files, thus dynamically instantiating a list of        \n"
                                "component models with various embedded functionalities.              \n"
                                )
    ;

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("logging.priority")
      .set_terse_description("Set the logging priority threshold")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("Example::                                 \n"
                            "                                          \n"
                            "  logging.priority : string = \"notice\"  \n"
                            "                                          \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("setup_label")
      .set_terse_description("The label of the electronics virtual setup")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description(
                            "Superseded by the previous method call :                   \n"
                            "  ``electronics::component_manager::set_setup_label(...)`` \n"
                            "                                                           \n")
      .add_example("::                                 \n"
                   "                                   \n"
                   "  setup_label : string = \"ECM\"   \n"
                   "                                   \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("setup_version")
      .set_terse_description("The version of the electronics virtual setup")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(true)
      .set_long_description(
                            "Superseded by the previous method call :                     \n"
                            "  ``electronics::component_manager::set_setup_version(...)`` \n"
                            "                                                             \n")
      .add_example("::                                 \n"
                   "                                   \n"
                   "  setup_version : string = \"1.0\" \n"
                   "                                   \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("setup_description")
      .set_terse_description("The description of the electronics virtual setup")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("Superseded by the previous method call :                         \n"
                            "  ``electronics::component_manager::set_setup_description(...)`` \n"
                            "                                                                 \n")
      .add_example("::                                                               \n"
                   "                                                                 \n"
                   "  setup_description : string = \"The electronics virtual setup\" \n"
                   "                                                                 \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("components.configuration_files")
      .set_terse_description("A list of configuration filenames for embeded components")
      .set_traits(datatools::TYPE_STRING,
                  configuration_property_description::ARRAY)
      .set_mandatory(false)
      .set_path(true)
      .set_long_description(
                            "A list of filenames from where the manager loads the     \n"
                            "directives to dynamically instantiate new embedded       \n"
                            "component model objects. The filenames may contains      \n"
                            "some environment variables.                              \n"
                            "The target files must use the format of the              \n"
                            "``datatools::multi_properties`` class.                   \n"
                            "The loading order of the files is critical               \n"
                            "because some components may depend on other ones         \n"
                            "which should thus be defined **before** their            \n"
                            "dependers.                                               \n"
                            "                                                         \n"
                            "Extends the instantiation of components triggered by     \n"
                            "previous method calls :                                  \n"
                            "  ``electronics::component_manager::load(...)``          \n"
                            "                                                         \n")
      .add_example("A setup with two files::                                   \n"
                   "                                                           \n"
                   "   components.configuration_files : string[2] as path = \\ \n"
                   "     \"${CONFIG_REPOSITORY_DIR}/components.conf\" \\       \n"
                   "     \"${CONFIG_REPOSITORY_DIR}/other_components.conf\"    \n"
                   "                                                           \n"
                   )
      ;
  }


  ocd_.set_configuration_hints ("Configuration example for a ``electronics::component_manager`` object:: \n"
                                "                                                                \n"
                                "   setup_label : string = \"MyExperiment\"                      \n"
                                "   setup_version : string = \"1.0\"                             \n"
                                "   setup_description  : string = \\                             \n"
                                "     \"Core components manager for MyExperiment\"               \n"
                                "   components.configuration_files : string[2] as path = \\      \n"
                                "     \"${CONFIG_DIR}/frontend_components.conf\" \\              \n"
                                "     \"${CONFIG_DIR}/trigger_components.conf\"                  \n"
                                "                                                                \n"
                                "The ``${CONFIG_DIR}/database_components.conf``, ``${CONFIG_DIR}/io_components.conf``\n"
                                "and ``${CONFIG_DIR}/processing_components.conf`` files use      \n"
                                "the ``datatools::multi_properties`` ASCII format.               \n"
                                "                                                                \n"
                                "Example::                                                       \n"
                                "                                                                \n"
                                "  #@description A list of components                            \n"
                                "  #@key_label   \"name\"                                        \n"
                                "  #@meta_label  \"type\"                                        \n"
                                "                                                                \n"
                                "  [name=\"component_0\" type=\"component_model_0\"]             \n"
                                "                                                                \n"
                                "  #@config A component object                                   \n"
                                "                                                                \n"
                                "  #@description A configuration parameter                       \n"
                                "  debug : boolean = 1                                           \n"
                                "                                                                \n"
                                "  #@description Another configuration parameter                 \n"
                                "  name : string = \"value\"                                     \n"
                                "                                                                \n"
                                "  ...                                                           \n"
                                "                                                                \n"
                                "  [name=\"component_1\" type=\"component_model_1\"]             \n"
                                "                                                                \n"
                                "  #@config Another component object                             \n"
                                "                                                                \n"
                                "  #@description A configuration parameter                       \n"
                                "  debug : boolean = 0                                           \n"
                                "                                                                \n"
                                "  #@description Another configuration parameter                 \n"
                                "  name : string = \"value\"                                     \n"
                                "                                                                \n"
                                "  ...                                                           \n"
                                "                                                                \n"
                                );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()

DOCD_CLASS_SYSTEM_REGISTRATION(electronics::component_manager,"electronics::component_manager")

// end of component_manager.cc
