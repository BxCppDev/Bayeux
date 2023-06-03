/* module_manager.cc
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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

// Standard library:
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/multi_properties.h>
#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/service_manager.h>
#include <datatools/exception.h>

// This project:
#include <dpp/module_manager.h>
#include <dpp/base_module.h>

namespace dpp {

  datatools::logger::priority
  module_manager::get_logging_priority() const
  {
    return _logging_priority_;
  }

  void module_manager::set_logging_priority(datatools::logger::priority p_)
  {
    _logging_priority_ = p_;
  }

  bool module_manager::is_no_preload () const
  {
    return _flags_ & FACTORY_NOPRELOAD;
  }

  bool module_manager::is_initialization_at_load () const
  {
    return _flags_ & FACTORY_INITIALIZATION_AT_LOAD;
  }

  void module_manager::_preload_global_dict ()
  {
    DT_LOG_TRACE (get_logging_priority (), "Entering...");
    _factory_register_.import (DATATOOLS_FACTORY_GET_SYSTEM_REGISTER (dpp::base_module));
    DT_LOG_TRACE (get_logging_priority (), "Exiting.");
    return;
  }

  bool module_manager::has_module_type (const std::string & module_id_) const
  {
    return _factory_register_.has (module_id_);
  }

  void module_manager::unregister_module_type (const std::string & module_id_)
  {
    _factory_register_.unregistration (module_id_);
    return;
  }

  void module_manager::build_registered_module_types(std::vector<std::string> & ids_) const
  {
    ids_.clear();
    _factory_register_.list_of_factories(ids_);
    return;
  }

  module_manager::module_manager (uint32_t flags_)
  {
    _logging_priority_      = datatools::logger::PRIO_WARNING;
    _initialized_           = false;
    _flags_                 = flags_;
    _service_manager_owner_ = false;
    _service_manager_       = 0;
    return;
  }

  bool module_manager::has_service_manager () const
  {
    return _service_manager_ != 0;
  }

  const datatools::service_manager &
  module_manager::get_service_manager () const
  {
    DT_THROW_IF(! has_service_manager(),
                std::logic_error,
                "Module manager has no service manager !");
    return *_service_manager_;
  }

  datatools::service_manager &
  module_manager::grab_service_manager ()
  {
    DT_THROW_IF(! has_service_manager(),
                std::logic_error,
                "Module manager has no service manager !");
    return *_service_manager_;
  }

  void module_manager::set_service_manager (datatools::service_manager & service_manager_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Module manager is already initialized !");
    _service_manager_owner_ = false;
    _service_manager_ = &service_manager_;
    return;
  }

  module_manager::~module_manager ()
  {
    if (is_initialized ()) {
      module_manager::reset ();
    }
    return;
  }

  void module_manager::install_service_manager (const datatools::properties & service_manager_configuration_)
  {
    DT_LOG_TRACE(get_logging_priority(),"Entering...");
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Module manager is already initialized !");
    DT_THROW_IF(has_service_manager(),
                std::logic_error,
                "Module manager already has an embedded service manager !");
    _service_manager_ = new datatools::service_manager;
    DT_LOG_NOTICE(get_logging_priority(),"Initializing the embedded service manager...");
    _service_manager_->initialize (service_manager_configuration_);
    _service_manager_owner_ = true;
    DT_LOG_TRACE(get_logging_priority(),"Exiting.");
    return;
  }

  bool module_manager::is_initialized () const
  {
    return _initialized_;
  }

  void module_manager::initialize_simple ()
  {
    datatools::properties dummy;
    initialize(dummy);
    return;
  }

  void module_manager::initialize (const datatools::properties & setup_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Module manager is already initialized !");

    datatools::logger::priority p
      = datatools::logger::extract_logging_configuration(setup_,
                                                         datatools::logger::PRIO_WARNING);
    DT_THROW_IF(p == datatools::logger::PRIO_UNDEFINED,
                std::logic_error,
                "Invalid logging priority !");
    set_logging_priority(p);

    if (setup_.has_key("factory.logging.priority")) {
      std::string prio_label = setup_.fetch_string("factory.logging.priority");
      datatools::logger::priority op = datatools::logger::get_priority(prio_label);
      DT_THROW_IF(op == datatools::logger::PRIO_UNDEFINED,
                  std::domain_error,
                  "Unknow logging priority ``" << prio_label << "`` !");
      _factory_register_.set_logging_priority(op);
    }

    if (setup_.has_flag ("factory.no_preload")) {
      _flags_ |= FACTORY_NOPRELOAD;
    }

    if (setup_.has_flag ("factory.initialization_at_load")) {
      _flags_ |= FACTORY_INITIALIZATION_AT_LOAD ;
    }

    // Embeds a service manager :
    if (! has_service_manager ()) {
      std::string service_manager_configuration_file;
      if (setup_.has_key ("service_manager.configuration")) {
        service_manager_configuration_file
          = setup_.fetch_string ("service_manager.configuration");
        DT_LOG_NOTICE (get_logging_priority(),
                       "Embeds a service manager from setup file '"
                       << service_manager_configuration_file << "' !" << std::endl);
        datatools::fetch_path_with_env (service_manager_configuration_file);
        datatools::properties service_manager_configuration;
        datatools::properties::read_config (service_manager_configuration_file,
                                            service_manager_configuration);
        install_service_manager (service_manager_configuration);
      } else {
        DT_LOG_WARNING (get_logging_priority(),
                        "There is no 'service_manager.configuration' configuration property ! So we won't use an embedded service manager. For now, this is not a issue.");
      }
    }

    // Instantiate a module factory :
    {
      _factory_register_.set_label ("dpp::base_module/module_factory");
      if (! is_no_preload ())  {
        _preload_global_dict ();
      }
    }

    // Check if some service manager is available :
    if (! has_service_manager ()) {
        DT_LOG_WARNING (get_logging_priority(),
                        "No service manager is available ! "
                        << "This will forbid the use of some processing modules that need to access to some specific external services. This depends on your pipeline setup. For now, we continue without a service manager.");
    } else {
      DT_LOG_NOTICE (get_logging_priority(),
                     "We now have a service manager available !");
    }

    // Load and create the data processing modules :
    std::vector<std::string> modules_configurations;
    if (setup_.has_key ("modules.configuration_files")) {
      setup_.fetch ("modules.configuration_files", modules_configurations);
    }
    for (size_t i = 0; i < modules_configurations.size (); ++i) {
      std::string filename = modules_configurations[i];
      datatools::fetch_path_with_env (filename);
      datatools::multi_properties configs;
      DT_LOG_NOTICE (get_logging_priority(),
                     "Loading modules from file '"
                     << filename << "'...");
      configs.read (filename);
      _load_modules (configs);
    }

    _initialized_ = true;
    return;
  }

  void module_manager::create_module (module_entry_type & module_entry_)
  {
    if (! module_entry_.is_created ()) {
      this->_create_module (module_entry_);
    }
    return;
  }

  void module_manager::_create_module (module_entry_type & module_entry_)
  {
    if (module_entry_.is_created ()) {
      return;
    }
    DT_LOG_DEBUG (get_logging_priority(),
                  "Creating module named '"
                  <<  module_entry_.get_module_name ()
                  << "'...");
    // search for the cut's label in the factory dictionary:
    DT_THROW_IF(! _factory_register_.has (module_entry_.get_module_id ()),
                std::logic_error,
                "Cannot find module factory with ID '"
                << module_entry_.get_module_id () << "' for module named '"
                << module_entry_.get_module_name () << "' !");
    const base_module::factory_register_type::factory_type & the_factory
      = _factory_register_.get (module_entry_.get_module_id ());

    base_module * ptr = the_factory ();
    module_entry_.set_ptr (ptr);
    module_entry_.set_created ();
    DT_LOG_DEBUG (get_logging_priority(),
                  "Module named '"
                  <<  module_entry_.get_module_name ()
                  << "' has been created !");
    return;
  }

  void module_manager::initialize_module (module_entry_type & module_entry_)
  {
    if (! module_entry_.is_initialized ()) {
      _initialize_module (module_entry_);
    }
    return;
  }

  void module_manager::_initialize_module (module_entry_type & module_entry_)
  {
    // If not created, first do it :
    if (! module_entry_.is_created ()) {
      _create_module (module_entry_);
    }

    // If not initialized, do it :
    if (! module_entry_.is_initialized ()) {
      DT_LOG_DEBUG (get_logging_priority(),
                    "Initializing module named '"
                    <<  module_entry_.get_module_name ()
                    << "'...");
      base_module & the_module = module_entry_.grab_module ();
      the_module.set_name (module_entry_.get_module_name ());
      if (has_service_manager ()) {
        the_module.initialize (module_entry_.get_module_config (),
                               grab_service_manager (),
                               _modules_);
      } else {
        DT_LOG_DEBUG (get_logging_priority(),
                      "Attempt to initializing module named '"
                      <<  module_entry_.get_module_name ()
                      << "' without service support...");
        the_module.initialize_without_service (module_entry_.get_module_config (),
                                               _modules_);
      }
      module_entry_.set_initialized ();
    }
    return;
  }

  void module_manager::_reset_module (module_entry_type & module_entry_)
  {
    if (module_entry_.is_initialized ()) {
      base_module & the_module = module_entry_.grab_module ();
      the_module.reset ();
      module_entry_.set_uninitialized ();
    }
    return;
  }

  void module_manager::_load_module (const std::string & module_name_,
                                     const std::string & module_id_,
                                     const datatools::properties & module_config_)
  {
    DT_LOG_TRACE (get_logging_priority(), "Entering...");
    DT_THROW_IF(has(module_name_),
                std::logic_error,
                "Module '" << module_name_ << "' already exists !");
    {
      // Add a new entry :
      module_entry_type dummy_module_entry;
      DT_LOG_DEBUG (get_logging_priority(),
                    "Add an entry for module '" << module_name_ << "'...");
      _modules_[module_name_] = dummy_module_entry;
    }

    // fetch a reference on it and update its internal values :
    module_entry_type & the_module_entry = _modules_.find (module_name_)->second;
    the_module_entry.set_module_name (module_name_);
    the_module_entry.set_manager (*this);
    the_module_entry.set_module_id (module_id_);
    the_module_entry.set_module_config (module_config_);
    the_module_entry.set_blank ();

    DT_LOG_DEBUG (get_logging_priority(), "Fetch module properties...");
    DT_LOG_DEBUG (get_logging_priority(), "Module: " << module_id_);

    bool force_initialization_at_load = false;

    if (module_config_.has_flag ("force_initialization_at_load")) {
      force_initialization_at_load = true;
    }

    if (force_initialization_at_load || is_initialization_at_load ()) {
      _initialize_module (the_module_entry);
    }

    DT_LOG_TRACE (get_logging_priority(), "Exiting.");
    return;
  }

  void module_manager::load_module (const std::string & module_name_,
                                    const std::string & module_id_,
                                    const datatools::properties & module_config_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Module manager is already initialized !");
    _load_module (module_name_, module_id_, module_config_);
    return;
  }

  void module_manager::load_modules (const datatools::multi_properties & modules_config_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Module manager is already initialized !");
    _load_modules(modules_config_);
    return;
  }

  void module_manager::load_modules (const std::string & modules_defs_)
  {
    datatools::multi_properties modules_defs;
    std::string modules_defs_path = modules_defs_;
    datatools::fetch_path_with_env(modules_defs_path);
    modules_defs.read(modules_defs_path);
    this->load_modules(modules_defs);
    return;
  }


  void module_manager::_load_modules (const datatools::multi_properties & config_)
  {
    using namespace datatools;
    DT_LOG_TRACE (get_logging_priority(), "Loading modules...");
#if DPP_DATATOOLS_LEGACY == 1
    typedef multi_properties::entries_ordered_col_t col_type;
#else
    typedef multi_properties::entries_ordered_col_type col_type;
#endif
    const col_type & oe = config_.ordered_entries ();
    for (col_type::const_iterator i = oe.begin ();
         i != oe.end ();
         ++i) {
      const multi_properties::entry & the_entry = **i;
      const std::string & module_name = the_entry.get_key ();
      DT_LOG_NOTICE (get_logging_priority(), "Loading module '" << module_name << "'...");
      DT_THROW_IF(has(module_name),
                  std::logic_error,
                  "Module manager already has a module named '"
                  << module_name << "' !");
      const std::string & module_id = the_entry.get_meta ();
      const properties & module_config = the_entry.get_properties ();
      _load_module (module_name, module_id, module_config);
    }
    return;
  }

  void module_manager::reset ()
  {
    DT_THROW_IF(! is_initialized(),
                std::logic_error,
                "Module manager is not initialized !");
    _modules_.clear ();
    _factory_register_.reset ();
    if (_service_manager_owner_) {
      if (has_service_manager ()) {
        delete _service_manager_;
      }
    }
    _service_manager_ = 0;
    _service_manager_owner_ = false;
    _flags_ = BLANK;
    _initialized_ = false;
    return;
  }

  bool module_manager::has (const std::string & module_name_) const
  {
    return _modules_.find (module_name_) != _modules_.end ();
  }

  void module_manager::build_modules_infos(std::vector<std::pair<std::string,std::string> > & infos_) const
  {
    // Ensure the empty array is empty (is it questionnable? should we allow 'append' mode ?):
    infos_.clear();
    for (module_handle_dict_type::const_iterator i = _modules_.begin ();
         i != _modules_.end ();
         i++) {
      {
        std::pair<std::string,std::string> dummy;
        infos_.push_back(dummy);
      }
      std::pair<std::string,std::string> & module_desc = infos_.back();
      module_desc.first = i->first;
      module_desc.second = i->second.get_module_id();
    }
    return;
  }

  void module_manager::remove (const std::string & module_name_)
  {
    _modules_.erase (module_name_);
    return;
  }

  base_module &
  module_manager::grab (const std::string & module_name_)
  {
    module_handle_dict_type::iterator found = _modules_.find (module_name_);
    DT_THROW_IF(found == _modules_.end(),
                std::logic_error,
                "No module named '" << module_name_ << "' !");
    if (! found->second.is_initialized ()) {
      _initialize_module (found->second);
    }
    return found->second.grab_module ();
  }

  const base_module &
  module_manager::get (const std::string & module_name_) const
  {
    module_manager * mutable_this = const_cast<module_manager*> (this);
    return const_cast<base_module &> (mutable_this->grab (module_name_));
  }

  module_handle_dict_type & module_manager::grab_modules ()
  {
    return _modules_;
  }

  // Deprecated
  module_handle_dict_type & module_manager::get_modules ()
  {
    return _modules_;
  }

  const module_handle_dict_type & module_manager::get_modules () const
  {
    return _modules_;
  }

  void module_manager::tree_dump (std::ostream      & out_,
                                  const std::string & title_,
                                  const std::string & indent_,
                                  bool inherit_) const
  {
    std::string indent;
    if (! indent_.empty ()) {
      indent = indent_;
    }
    if ( ! title_.empty ()) {
      out_ << indent << title_ << std::endl;
    }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Initialized       : " << is_initialized () << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Flags             : " << _flags_ << std::endl;

    out_ << indent << datatools::i_tree_dumpable::tag
         << "List of registered data processing module factories : " << std::endl;
    {
      std::ostringstream indent_oss;
      indent_oss << indent << datatools::i_tree_dumpable::skip_tag;
      _factory_register_.print (out_, indent_oss.str ());
    }

    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Modules : ";
      size_t sz = _modules_.size ();
      if (sz == 0) {
        out_ << "<none>";
      }
      out_ << std::endl;
      for (module_handle_dict_type::const_iterator i = _modules_.begin ();
           i != _modules_.end ();
           i++) {
        const std::string & the_module_name = i->first;
        const module_entry_type & the_module_entry = i->second;
        out_ << indent << datatools::i_tree_dumpable::skip_tag;

        std::ostringstream indent_oss;
        indent_oss << indent << datatools::i_tree_dumpable::skip_tag;
        module_handle_dict_type::const_iterator j = i;
        j++;
        if (j == _modules_.end ()) {
          out_ << datatools::i_tree_dumpable::last_tag;
          indent_oss << datatools::i_tree_dumpable::last_skip_tag;
        } else {
          out_ << datatools::i_tree_dumpable::tag;
          indent_oss << datatools::i_tree_dumpable::skip_tag;
        }
        out_ << "Module : '" << the_module_name << "'" << std::endl;
        the_module_entry.tree_dump (out_, "", indent_oss.str ());
      }
    }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Service manager   :" ;
    if (has_service_manager ()) {
      out_ << " @ " << _service_manager_ << std::endl;
      std::ostringstream indent_ss;
      indent_ss << indent << datatools::i_tree_dumpable::skip_tag;
      _service_manager_->tree_dump (out_, "", indent_ss.str ());
    } else {
      out_ << " <none>" << std::endl;
    }

    out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
         << "Owns service manager  : " << _service_manager_owner_ << std::endl;

    return;
  }

}  // end of namespace dpp

/***************
 * OCD support *
 ***************/

#include <datatools/ocd_macros.h>


// OCD support for class '::datatools::module_manager' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::dpp::module_manager,ocd_)
{
  ocd_.set_class_name ("dpp::module_manager");
  ocd_.set_class_library ("dpp");
  ocd_.set_class_description ("A generic manager for data processing modules");
  ocd_.set_class_documentation ("The data processing module manager is responsible of the     \n"
                                "instantiation, initialization, usage and destruction of      \n"
                                "an arbitrary collection of 'data processing modules'.        \n"
                                "A data processing module is able to perform a special task   \n"
                                "on a data structure implemented through an object of the     \n"
                                "'datatools::things' class.                                   \n"
                                "Each data processing module fulfills the 'dpp::base_module'  \n"
                                "interface and is defined from a set of parametrization rules.\n"
                                "The modules are stored in a special dictionnary within the   \n"
                                "manager. One can access and use a module from its unique key.\n"
                                "The modules can also be organized and processed along a      \n"
                                "queue, thus providing a full data processing pipeline.       \n"
                                "Several generic high-level modules are also provided within  \n"
                                "the 'dpp' library.                                           \n"
                                );

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("debug")
      .set_terse_description("Flag to activate debugging output")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("This flag activates debugging output.             \n"
                            "It is not recommended for a production run.       \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("verbose")
      .set_terse_description("Flag to activate verbose output")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("Default: 0                                        \n"
                            "This flag activates verbose output.               \n"
                            "It is not recommended for a production run.       \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("factory.debug")
      .set_terse_description("Flag to activate debugging output from factory.")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("Default: 0                                                    \n"
                            "This flag activates debugging output from the data processing \n"
                            "module embeded factory.                                       \n"
                            "It is not recommended for a production run.                   \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("factory.no_preload")
      .set_terse_description("Flag to inhibit the preloading of registered module factories.")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("Default: 0                                                           \n"
                            "When set this flag inhibits the automatic loading of data processing \n"
                            "module factories registered in the 'dpp' library.                    \n"
                            "Module factories have thus to be 'manually' loaded on user request.  \n"
                            "This behaviour is not recommended.                                   \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("factory.initialization_at_load")
      .set_terse_description("Flag to force the initialization of module when loaded.")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("Default: 0                                                           \n"
                            "When set this flag triggers the automatic initialization of all      \n"
                            "data processing modules that are declared and loaded by the manager, \n"
                            "even if such modules are not used at all through the data processing \n"
                            "chain.                                                               \n"
                            "This behaviour is not recommended.                                   \n"
                            "The default behaviour consists in initializing a module, and allocate\n"
                            "its internal resources, only if it is really useful.                 \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("modules.configuration_files")
      .set_terse_description("The list of module configuration files.")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_path(true)
      .set_mandatory(false)
      .set_long_description("Default: empty                                                        \n"
                            "Each file addressed by this property must follow the format of        \n"
                            "the 'datatools::multi_properties' class.                              \n"
                            "The filenames may contain some path environment variable(s).          \n"
                            "                                                                      \n"
                            "Example :                                                             \n"
                            "  |                                                                   \n"
                            "  | modules.configuration_files : string[1] as path = \"modules.conf\"\n"
                            "  |                                                                   \n"
                            "                                                                      \n"
                            "Where the \"modules.conf\" file builds a data processing chain made   \n"
                            "of two modules, with the following syntax :                           \n"
                            "  |                                                                   \n"
                            "  | #@key_label  \"name\"                                             \n"
                            "  | #@meta_label \"type\"                                             \n"
                            "  |                                                                   \n"
                            "  | [name=\"output\" type=\"dpp::io_module\"]                         \n"
                            "  | mode : string = \"output\"                                        \n"
                            "  | output.mode : string = \"incremental\"                            \n"
                            "  | output.max_record_per_file   : integer = 10000                    \n"
                            "  | output.incremental.path      : string as path = \"${DATA}/save\"  \n"
                            "  | output.incremental.prefix    : string = \"run_\"                  \n"
                            "  | output.incremental.extension : string = \".data\"                 \n"
                            "  | output.incremental.increment : integer = 1                        \n"
                            "  | output.incremental.start     : integer = 0                        \n"
                            "  | output.incremental.stop      : integer = 100                      \n"
                            "  |                                                                   \n"
                            "  | [name=\"dump_in_file\" type=\"dpp::dump_module\"]                 \n"
                            "  | title : string = \"Event record\"                                 \n"
                            "  | output : string = \"file\"                                        \n"
                            "  | output.filename : string as path = \"dump.txt\"                   \n"
                            "  |                                                                   \n"
                            "  | [name=\"pipeline\" type=\"dpp::chain_module\"]                    \n"
                            "  | modules : string[2] = \"output\" \"dump_in_file\"                 \n"
                            "  |                                                                   \n"
                            "                                                                      \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("service_manager.configuration")
      .set_terse_description("The configuration file of the service manager.")
      .set_traits(datatools::TYPE_STRING)
      .set_path(true)
      .set_mandatory(false)
      .set_long_description("Default: empty                                                        \n"
                            "This file must follow the format of the 'datatools::properties' class.\n"
                            "See OCD support for the 'datatools::service_manager' class.           \n"
                            "This property is superseded by a former call to :                     \n"
                            "  dpp::module_manager::set_service_manager(...)                       \n"
                            )
      ;
  }

  //ocd_.set_configuration_hints ("Nothing special.");
  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()

DOCD_CLASS_SYSTEM_REGISTRATION(dpp::module_manager,"dpp::module_manager")
