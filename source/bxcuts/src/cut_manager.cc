/* cut_manager.cc
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

// Ourselves:
#include <cuts/cut_manager.h>

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
#include <cuts/i_cut.h>

namespace cuts {

  bool cut_manager::is_no_preload() const
  {
    return _flags_ & FACTORY_NOPRELOAD;
  }

  bool cut_manager::is_initialization_at_load() const
  {
    return _flags_ & FACTORY_INITIALIZATION_AT_LOAD;
  }

  bool cut_manager::has_print_report() const
  {
    return _print_report_;
  }

  void cut_manager::_preload_global_dict()
  {
    DT_LOG_TRACE(_logging_, "Entering...");
    _factory_register_.import(DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(cuts::i_cut));
    return;
  }

  bool cut_manager::has_cut_type(const std::string & cut_id_) const
  {
    return _factory_register_.has(cut_id_);
  }

  void cut_manager::unregister_cut_type(const std::string & cut_id_)
  {
    _factory_register_.unregistration(cut_id_);
    return;
  }

  void cut_manager::set_logging_priority(datatools::logger::priority p)
  {
    _logging_ = p;
    return;
  }

  datatools::logger::priority cut_manager::get_logging_priority() const
  {
    return _logging_;
  }

  cut_manager::cut_manager(uint32_t flags_)
  {
    _initialized_           = false;
    _logging_               = datatools::logger::PRIO_FATAL;
    _flags_                 = flags_;
    _service_manager_owner_ = false;
    _service_manager_       = 0;
    _print_report_          = false;
    return;
  }

  bool cut_manager::has_service_manager() const
  {
    return _service_manager_ != 0;
  }

  const datatools::service_manager &
  cut_manager::get_service_manager() const
  {
    DT_THROW_IF(! has_service_manager(),
                std::logic_error,
                "Cut manager has no service manager !");
    return *_service_manager_;
  }

  datatools::service_manager &
  cut_manager::grab_service_manager()
  {
    DT_THROW_IF(! has_service_manager(),
                std::logic_error,
                "Cut manager has no service manager !");
    return *_service_manager_;
  }

  void cut_manager::set_service_manager(datatools::service_manager & service_manager_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Cut manager is already initialized !");
    _service_manager_owner_ = false;
    _service_manager_ = &service_manager_;
    return;
  }

  cut_manager::~cut_manager()
  {
    if (is_initialized()) {
      cut_manager::reset();
    }
    return;
  }

  void cut_manager::install_service_manager(const datatools::properties & service_manager_configuration_)
  {
    DT_LOG_DEBUG(_logging_,"Entering...");
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Cut manager is already initialized !");
    DT_THROW_IF(has_service_manager(),
                std::logic_error,
                "Cut manager has already an embedded service manager !");
    _service_manager_ = new datatools::service_manager;
    DT_LOG_NOTICE(_logging_,"Initializing the embedded service manager...");
    _service_manager_->initialize(service_manager_configuration_);
    _service_manager_owner_ = true;
    DT_LOG_DEBUG(_logging_,"Exiting.");
    return;
  }

  bool cut_manager::is_initialized() const
  {
    return _initialized_;
  }

  void cut_manager::initialize(const datatools::properties & a_config)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Cut manager is already initialized !");

    DT_LOG_DEBUG(_logging_,"Parsing configuration parameters...");

    if (a_config.has_key("logging.priority")) {
      std::string prio_label = a_config.fetch_string("logging.priority");
      datatools::logger::priority p = datatools::logger::get_priority(prio_label);
      DT_THROW_IF(p == datatools::logger::PRIO_UNDEFINED,
                  std::domain_error,
                  "Unknow logging priority ``" << prio_label << "`` !");
      set_logging_priority(p);
    }

    if (a_config.has_key("factory.logging.priority")) {
      std::string prio_label = a_config.fetch_string("factory.logging.priority");
      datatools::logger::priority p = datatools::logger::get_priority(prio_label);
      DT_THROW_IF(p == datatools::logger::PRIO_UNDEFINED,
                  std::domain_error,
                  "Unknow logging priority ``" << prio_label << "`` !");
      _factory_register_.set_logging_priority(p);
    }

    if (a_config.has_flag("factory.no_preload")) {
      _flags_ |= FACTORY_NOPRELOAD;
    }

    if (a_config.has_flag("factory.initialization_at_load")) {
      _flags_ |= FACTORY_INITIALIZATION_AT_LOAD;
    }

    if (a_config.has_flag("print_report")) {
      _print_report_ = true;
    }

    // Embed a  service manager :
    if (! has_service_manager()) {
      std::string service_manager_configuration_file;
      if (a_config.has_key("service_manager.configuration")) {
        service_manager_configuration_file
          = a_config.fetch_string("service_manager.configuration");
        DT_LOG_NOTICE(_logging_,
                      "Embedes a service manager setup from file '"
                      << service_manager_configuration_file << "' !");
        datatools::fetch_path_with_env(service_manager_configuration_file);
        datatools::properties service_manager_configuration;
        datatools::properties::read_config(service_manager_configuration_file,
                                           service_manager_configuration);
        install_service_manager(service_manager_configuration);
      } else {
        DT_LOG_WARNING(_logging_,
                       "No service manager configuration property !");
      }
    }

    // Instantiate a cut factory :
    {
      _factory_register_.set_label("cuts::i_cut/cut_factory");
      if (! is_no_preload()) {
        _preload_global_dict();
      }
    }

    // Check if some service manager is available :
    if (! has_service_manager()) {
      DT_LOG_WARNING(_logging_,
                     "No service manager is available ! "
                     << "This could forbid the use of some cuts !");
    } else {
      DT_LOG_NOTICE(_logging_, "We now have a service manager available !");
    }

    // Load and create the data processing cuts :
    std::vector<std::string> cuts_configurations;
    if (a_config.has_key("cuts.configuration_files")){
      a_config.fetch("cuts.configuration_files", cuts_configurations);
    }
    for (size_t i = 0; i < cuts_configurations.size(); ++i) {
      std::string filename = cuts_configurations[i];
      datatools::fetch_path_with_env(filename);
      datatools::multi_properties configs;
      DT_LOG_NOTICE(_logging_, "Loading cuts from file '"
                    << filename << "'...");
      datatools::multi_properties::read_config(filename, configs);
      _load_cuts(configs);
    }

    _initialized_ = true;
    return;
  }

  void cut_manager::_create_cut(cut_entry_type & cut_entry_)
  {
    if (cut_entry_.is_created()) {
      return;
    }
    DT_LOG_DEBUG(_logging_,
                 "Creating cut named '"
                 <<  cut_entry_.get_cut_name()
                 << "'...");
    // search for the cut's label in the factory dictionary:
    DT_THROW_IF(! _factory_register_.has(cut_entry_.get_cut_id()),
                std::logic_error,
                "Cannot find cut factory with ID '"
                << cut_entry_.get_cut_id() << "' for cut named '"
                << cut_entry_.get_cut_name() << "' !");
    const i_cut::factory_register_type::factory_type & the_factory
      = _factory_register_.get(cut_entry_.get_cut_id());

    i_cut * ptr = the_factory();
    cut_entry_.set_ptr(ptr);
    cut_entry_.set_created();
    // Propagate the cut name :
    ptr->set_name(cut_entry_.get_cut_name());
    _ordered_cuts_.push_back(cut_entry_.get_cut_name());
    DT_LOG_NOTICE(_logging_, "Cut named '" << cut_entry_.get_cut_name() << "' has been created !");
    return;
  }

  // void cut_manager::initialize_cut(cut_entry_type & cut_entry_)
  // {
  //   if (! cut_entry_.is_initialized()) {
  //     _initialize_cut(cut_entry_);
  //   }
  //   return;
  // }

  void cut_manager::_initialize_cut(cut_entry_type & cut_entry_)
  {
    // If not created, first do it :
    if (! cut_entry_.is_created()) {
      _create_cut(cut_entry_);
    }

    // If not initialized, do it :
    if (! cut_entry_.is_initialized()) {
      DT_LOG_DEBUG(_logging_,
                   "Initializing cut named '"
                   <<  cut_entry_.get_cut_name()
                   << "'...");
      i_cut & the_cut = cut_entry_.grab_cut();
      if (has_service_manager()) {
        the_cut.initialize(cut_entry_.get_cut_config(),
                            grab_service_manager(),
                            _cuts_);
      } else {
        DT_LOG_NOTICE(_logging_, "Attempt to initializing cut named '"
                      <<  cut_entry_.get_cut_name()
                      << "' without service support...");
        the_cut.initialize_without_service(cut_entry_.get_cut_config(), _cuts_);
      }
      cut_entry_.set_initialized();
    }
    return;
  }

  void cut_manager::_reset_cut(cut_entry_type & cut_entry_)
  {
    if (cut_entry_.is_initialized()) {
      i_cut & the_cut = cut_entry_.grab_cut();
      the_cut.reset();
      cut_entry_.set_uninitialized();
    }
    return;
  }

  void cut_manager::_load_cut(const std::string & cut_name_,
                              const std::string & cut_id_,
                              const datatools::properties & cut_config_)
  {
    DT_LOG_TRACE(_logging_, "Entering...");
    DT_THROW_IF(has(cut_name_),
                std::logic_error,
                "Cut '" << cut_name_ << "' already exists !");
    {
      // Add a new entry :
      cut_entry_type dummy_cut_entry;
      DT_LOG_DEBUG(_logging_, "Add an entry for cut '" << cut_name_ << "'...");
      _cuts_[cut_name_] = dummy_cut_entry;
    }

    // fetch a reference on it and update its internal values :
    cut_entry_type & the_cut_entry = _cuts_.find(cut_name_)->second;
    the_cut_entry.set_cut_name(cut_name_);
    the_cut_entry.set_manager(*this);
    the_cut_entry.set_cut_id(cut_id_);
    the_cut_entry.set_cut_config(cut_config_);
    the_cut_entry.set_blank();
    bool force_initialization_at_load = false;
    if (cut_config_.has_flag("force_initialization_at_load")) {
      force_initialization_at_load = true;
    }
    if (force_initialization_at_load || is_initialization_at_load()) {
      _initialize_cut(the_cut_entry);
    }

    DT_LOG_TRACE(_logging_, "Exiting.");
    return;
  }

  void cut_manager::load_cut(const std::string & cut_name_,
                             const std::string & cut_id_,
                             const datatools::properties & cut_config_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Cut manager is already initialized !");
    _load_cut(cut_name_, cut_id_, cut_config_);
    return;
  }


  void cut_manager::_load_cuts(const datatools::multi_properties & config_)
  {
    if (_logging_ >= datatools::logger::PRIO_DEBUG) {
      DT_LOG_DEBUG(_logging_, "Cut manager configuration parameters : ");
      config_.tree_dump(std::clog, "", "[debug]: ");
    }
    const datatools::multi_properties::entries_ordered_col_type & oe = config_.ordered_entries();
    for (datatools::multi_properties::entries_ordered_col_type::const_iterator
           i = oe.begin(); i != oe.end(); ++i) {
      const datatools::multi_properties::entry & the_entry = **i;
      const std::string & cut_name = the_entry.get_key();
      DT_LOG_NOTICE(_logging_, "Loading cuts '" << cut_name << "'...");
      DT_THROW_IF(has(cut_name),
                  std::logic_error,
                  "Cut manager already has a cut named '" << cut_name << "' !");
      const std::string & cut_id = the_entry.get_meta();
      const datatools::properties & cut_config = the_entry.get_properties();
      _load_cut(cut_name, cut_id, cut_config);
    }
    return;
  }

  void cut_manager::reset()
  {
    DT_THROW_IF(! is_initialized(),
                std::logic_error,
                "Cut manager is not initialized !");

    if (has_print_report())
      {
        print_report(std::clog);
      }
    _cuts_.clear();
    _ordered_cuts_.clear();
    _factory_register_.reset();

    if (_service_manager_owner_) {
      if (has_service_manager()) {
        delete _service_manager_;
      }
    }
    _service_manager_ = 0;
    _service_manager_owner_ = false;
    _flags_ = BLANK;
    _logging_ = datatools::logger::PRIO_FATAL;
    _initialized_ = false;
    return;
  }

  bool cut_manager::has(const std::string & cut_name_) const
  {
    return _cuts_.find(cut_name_) != _cuts_.end();
  }

  void cut_manager::remove(const std::string & cut_name_)
  {
    _cuts_.erase(cut_name_);
    return;
  }

  i_cut &
  cut_manager::grab(const std::string & cut_name_)
  {
    cut_handle_dict_type::iterator found = _cuts_.find(cut_name_);
    DT_THROW_IF(found == _cuts_.end(),
                std::domain_error,
                "No cut named '" << cut_name_ << "' !");
    if (! found->second.is_initialized()) {
      _initialize_cut(found->second);
    }
    return found->second.grab_cut();
  }

  const i_cut &
  cut_manager::get(const std::string & cut_name_) const
  {
    cut_manager * mutable_this = const_cast<cut_manager*>(this);
    return const_cast<i_cut &>(mutable_this->grab(cut_name_));
  }

  cut_handle_dict_type & cut_manager::get_cuts()
  {
    return _cuts_;
  }

  const cut_handle_dict_type & cut_manager::get_cuts() const
  {
    return _cuts_;
  }

  void cut_manager::tree_dump(std::ostream      & out_,
                              const std::string & title_,
                              const std::string & indent_,
                              bool inherit_) const
  {
    std::string indent;
    if (! indent.empty()) {
      indent = indent_;
    }
    if ( ! title_.empty()) {
      out_ << indent << title_ << std::endl;
    }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Initialized       : " << is_initialized() << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Flags             : " << _flags_ << std::endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "List of registered services : " << std::endl;
    {
      std::ostringstream indent_oss;
      indent_oss << indent << datatools::i_tree_dumpable::skip_tag;
      _factory_register_.print(out_, indent_oss.str());
    }

    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Cuts : ";
      size_t sz = _cuts_.size();
      if (sz == 0) {
        out_ << "<none>";
      }
      out_ << std::endl;
      for (cut_handle_dict_type::const_iterator i = _cuts_.begin();
           i != _cuts_.end();
           i++) {
        const std::string & the_cut_name = i->first;
        const cut_entry_type & the_cut_entry = i->second;
        out_ << indent << datatools::i_tree_dumpable::skip_tag;

        std::ostringstream indent_oss;
        indent_oss << indent << datatools::i_tree_dumpable::skip_tag;
        cut_handle_dict_type::const_iterator j = i;
        j++;
        if (j == _cuts_.end()) {
          out_ << datatools::i_tree_dumpable::last_tag;
          indent_oss << datatools::i_tree_dumpable::last_skip_tag;
        } else {
          out_ << datatools::i_tree_dumpable::tag;
          indent_oss << datatools::i_tree_dumpable::skip_tag;
        }
        out_ << "Cut : '" << the_cut_name << "'" << std::endl;
        the_cut_entry.tree_dump(out_, "", indent_oss.str());
      }
    }
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Service manager   :" ;
    if (has_service_manager()) {
      out_ << " @ " << _service_manager_ << std::endl;
      std::ostringstream indent_ss;
      indent_ss << indent << datatools::i_tree_dumpable::skip_tag;
      _service_manager_->tree_dump(std::clog, "", indent_ss.str());
    } else {
      out_ << " <none>" << std::endl;
    }

    out_ << indent << datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Owns service manager  : " << _service_manager_owner_ << std::endl;

    return;
  }

  void cut_manager::print_report(std::ostream & out_) const
  {
    out_ << "List of created cuts (by order of appeareance) :" << std::endl;
    for (ordered_cut_list_type::const_iterator
           i = _ordered_cuts_.begin();
         i != _ordered_cuts_.end();
         i++) {
      const std::string & the_cut_name = *i;
      std::ostringstream indent_oss;
      ordered_cut_list_type::const_iterator j = i;
      if (++j == _ordered_cuts_.end()) {
        out_ << datatools::i_tree_dumpable::last_tag;
        indent_oss << datatools::i_tree_dumpable::last_skip_tag;
      } else {
        out_ << datatools::i_tree_dumpable::tag;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
      }
      out_ << "Cut '" << the_cut_name << "' status report : " << std::endl;
      _cuts_.at(the_cut_name).get_cut().tree_dump(out_, "", indent_oss.str());
    }
    return;
  }
}  // end of namespace cuts

/***************
 * OCD support *
 ***************/

#include <datatools/ocd_macros.h>

// OCD support for class '::cuts::cut_manager' :
DOCD_CLASS_IMPLEMENT_LOAD_BEGIN(::cuts::cut_manager,ocd_)
{
  ocd_.set_class_name("cuts::cut_manager");
  ocd_.set_class_description("A cut manager");
  ocd_.set_class_library("cuts");
  ocd_.set_class_documentation("The ``cut_manager`` object is responsible of a repository      \n"
                               "of cuts on arbitrary data. It embeds a factory of cuts that    \n"
                               "allows it to dynamically instantiate cut objects on user       \n"
                               "request. The manager is also responsible of the initialization \n"
                               "of such cut objects as well as their destruction at the end of \n"
                               "their life cycle. It makes possible to combined cut objets     \n"
                               "within cut objects of higher level (AND, OR...) and more       \n"
                               "generally to build complex cut objects that use more basic     \n"
                               "cut objects.                                                   \n"
                               "The cut classes that can be used within a cut manager must     \n"
                               "inherit the ``cuts::i_cut`` mother interface and use the       \n"
                               "cut factory registration system provided by the library.       \n"
                               );

  datatools::logger::declare_ocd_logging_configuration(ocd_, "fatal", "");

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("print_report")
      .set_terse_description("Flag to activate print report")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .set_long_description("This flag activates the report of cut efficiencies showing       \n"
                            "how much events have been selected/rejected for each active cut. \n"
                            )
      .add_example("Activate the printing of the report: :: \n"
                   "                                        \n"
                   "  print_report : boolean = 1            \n"
                   "                                        \n"
                   )
      ;

  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("factory.verbose")
      .set_terse_description("Verbosity flag dedicated to the embeded cut factory")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .set_long_description("This flag activates verbose printing for the embedded \n"
                            "*cut factory*.                                        \n"
                            )
      .add_example("Activate the factory verbosity: ::      \n"
                   "                                        \n"
                   "  factory.verbose : boolean = 1         \n"
                   "                                        \n"
                   )
      ;

  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("factory.no_preload")
      .set_terse_description("Flag to inhibit the preloading of system cut factory")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .set_long_description("When set, this flag inhibits the automatic loading of    \n"
                            "cut factories registered in the system cut registration. \n"
                            "Not recommanded for standard usage.                      \n"
                            )
      .add_example("Do not preload registered cut factories \n"
                   "from the system repository: ::          \n"
                   "                                        \n"
                   "  factory.no_preload : boolean = 1      \n"
                   "                                        \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("factory.initialization_at_load")
      .set_terse_description("Flag to automaticaly initialize cut objects at load")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_default_value_boolean(false)
      .set_long_description("When set, this flag triggers the automatic initialization of a   \n"
                            "cut while it is loaded. The default behaviour consists in        \n"
                            "initializing a cut if and only if it is effectively used.        \n"
                            "Not recommanded for standard usage.                              \n"
                            )
      .add_example("Do not initialized cut objects at load time: :: \n"
                   "                                                \n"
                   "  factory.initialization_at_load : boolean = 0  \n"
                   "                                                \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("service_manager.configuration")
      .set_terse_description("The configuration file of the embeded service manager")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_path(true)
      .set_complex_triggering_conditions(true)
      .set_long_description("This property indicates the name of the configuration file     \n"
                            "for the service manager made accessible to the cuts objects.   \n"
                            "It is only used if no external service manager has been        \n"
                            "defined through the ``cuts::cut_manager::set_service_manager`` \n"
                            "method.                                                        \n"
                            "The filename may contains some environment variables.          \n"
                            "The service manager is implemented as a ``datatools::service_manager`` \n"
                            "object.                                                        \n"
                            )
      .add_example("Specify the configuration file of the service manager: :: \n"
                   "                                                          \n"
                   "  service_manager.configuration : string as path = \\     \n"
                   "       \"${MY_CONFIG_DIR}/services/manager.conf\"         \n"
                   "                                                          \n"
                   )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("cuts.configuration_files")
      .set_terse_description("The list of configuration files for embeded cuts")
      .set_traits(datatools::TYPE_STRING,
                  datatools::configuration_property_description::ARRAY)
      .set_mandatory(false)
      .set_path(true)
      .set_complex_triggering_conditions(true)
      .set_long_description("This property indicates the names of the configuration files      \n"
                            "that stored the rules to instantiate cuts objects.                \n"
                            "The filenames may contain some environment variables.             \n"
                            "Each file must use the format of the ``datatools::multi_properties``\n"
                            "class.                                                            \n"
                            )
      .add_example("Use 3 different cut definition files : ::                         \n"
                    "                                                                  \n"
                    "   cuts.configuration_files : string[3] a path = \\               \n"
                    "     \"${MY_CONFIG_DIR}/cuts/energy_cuts.conf\" \\                \n"
                    "     \"${MY_CONFIG_DIR}/cuts/track_cuts.conf\"  \\                \n"
                    "     \"${MY_CONFIG_DIR}/cuts/topology_cuts.conf\"                 \n"
                    "                                                                  \n"
                    );
  }

  ocd_.set_configuration_hints("Here is a typical configuration::                                 \n"
                                "                                                                  \n"
                                "   logging.priority   : string = \"warning\"                      \n"
                                "   factory.verbose    : boolean = 0                               \n"
                                "   factory.no_preload : boolean = 0                               \n"
                                "   factory.initialization_at_load : boolean = 0                   \n"
                                "   service_manager.configuration : string as path = \\            \n"
                                "     \"${MY_CONFIG_DIR}/services/srvcmgr.conf\"                   \n"
                                "   cuts.configuration_files : string[3] a path = \\               \n"
                                "     \"${MY_CONFIG_DIR}/cuts/energy_cuts.conf\" \\                \n"
                                "     \"${MY_CONFIG_DIR}/cuts/track_cuts.conf\"  \\                \n"
                                "     \"${MY_CONFIG_DIR}/cuts/topology_cuts.conf\"                 \n"
                                "                                                                  \n"
                                );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()

DOCD_CLASS_SYSTEM_REGISTRATION(::cuts::cut_manager,"cuts::cut_manager")

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
