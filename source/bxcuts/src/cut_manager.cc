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

#include <stdexcept>
#include <sstream>

#include <cuts/cut_manager.h>
#include <cuts/i_cut.h>

#include <datatools/properties.h>
#include <datatools/multi_properties.h>
#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/service_manager.h>
#include <datatools/exception.h>

namespace cuts {

  using namespace std;

  bool cut_manager::is_debug () const
  {
    return _logging_ >= datatools::logger::PRIO_DEBUG;
  }

  bool cut_manager::is_verbose () const
  {
    return _logging_ >= datatools::logger::PRIO_NOTICE;
  }

  bool cut_manager::is_no_preload () const
  {
    return _flags_ & FACTORY_NOPRELOAD;
  }

  bool cut_manager::is_initialization_at_load () const
  {
    return _flags_ & FACTORY_INITIALIZATION_AT_LOAD;
  }

  void cut_manager::_preload_global_dict ()
  {
    DT_LOG_TRACE(_logging_, "Entering...");
    _factory_register_.import (DATATOOLS_FACTORY_GET_SYSTEM_REGISTER (cuts::i_cut));
    return;
  }

  bool cut_manager::has_cut_type (const string & cut_id_) const
  {
    return _factory_register_.has (cut_id_);
  }

  void cut_manager::unregister_cut_type (const string & cut_id_)
  {
    _factory_register_.unregistration (cut_id_);
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

  // ctor:
  cut_manager::cut_manager(uint32_t flags_)
  {
    _initialized_           = false;
    _logging_               = datatools::logger::PRIO_FATAL;
    _flags_                 = flags_;
    _service_manager_owner_ = false;
    _service_manager_       = 0;
    return;
  }

  bool cut_manager::has_service_manager () const
  {
    return _service_manager_ != 0;
  }

  const datatools::service_manager &
  cut_manager::get_service_manager () const
  {
    DT_THROW_IF(! has_service_manager(),
                std::logic_error,
                "Cut manager has no service manager !");
    return *_service_manager_;
  }

  datatools::service_manager &
  cut_manager::grab_service_manager ()
  {
    DT_THROW_IF(! has_service_manager(),
                std::logic_error,
                "Cut manager has no service manager !");
    return *_service_manager_;
  }

  void cut_manager::set_service_manager (datatools::service_manager & service_manager_)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Cut manager is already initialized !");
    _service_manager_owner_ = false;
    _service_manager_ = &service_manager_;
    return;
  }

  // dtor:
  cut_manager::~cut_manager ()
  {
    if (is_initialized()) {
      cut_manager::reset();
    }
    return;
  }

  void cut_manager::install_service_manager (const datatools::properties & service_manager_configuration_)
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
    _service_manager_->initialize (service_manager_configuration_);
    _service_manager_owner_ = true;
    DT_LOG_DEBUG(_logging_,"Exiting.");
    return;
  }

  bool cut_manager::is_initialized () const
  {
    return _initialized_;
  }

  void cut_manager::initialize (const datatools::properties & a_config)
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

    if (a_config.has_flag ("factory.verbose")) {
     _flags_ |= FACTORY_VERBOSE;
    }

    if (a_config.has_flag ("factory.no_preload")) {
      _flags_ |= FACTORY_NOPRELOAD;
    }

    if (a_config.has_flag ("factory.initialization_at_load")) {
      _flags_ |= FACTORY_INITIALIZATION_AT_LOAD ;
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
      _factory_register_.set_verbose(_flags_ & FACTORY_VERBOSE);
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
    vector<string> cuts_configurations;
    if (a_config.has_key ("cuts.configuration_files")){
      a_config.fetch ("cuts.configuration_files", cuts_configurations);
    }
    for (size_t i = 0; i < cuts_configurations.size (); ++i) {
        string filename = cuts_configurations[i];
        datatools::fetch_path_with_env (filename);
        datatools::multi_properties configs;
        DT_LOG_NOTICE(_logging_, "Loading cuts from file '"
                      << filename << "'...");
        configs.read (filename);
        _load_cuts (configs);
      }

    _initialized_ = true;
    return;
  }

  // void cut_manager::create_cut (cut_entry_type & cut_entry_)
  // {
  //   if (! cut_entry_.is_created ()) {
  //     this->_create_cut (cut_entry_);
  //   }
  //   return;
  // }

  void cut_manager::_create_cut (cut_entry_type & cut_entry_)
  {
    if (cut_entry_.is_created ()) {
      return;
    }
    DT_LOG_DEBUG(_logging_,
                 "Creating cut named '"
                 <<  cut_entry_.get_cut_name ()
                 << "'...");
    // search for the cut's label in the factory dictionary:
    DT_THROW_IF(! _factory_register_.has(cut_entry_.get_cut_id ()),
                std::logic_error,
                "Cannot find cut factory with ID '"
                << cut_entry_.get_cut_id () << "' for cut named '"
                << cut_entry_.get_cut_name () << "' !");
    const i_cut::factory_register_type::factory_type & the_factory
      = _factory_register_.get (cut_entry_.get_cut_id ());

    i_cut * ptr = the_factory ();
    cut_entry_.set_ptr (ptr);
    cut_entry_.set_created ();
    // Propagate the cut name :
    ptr->set_name(cut_entry_.get_cut_name ());
    DT_LOG_NOTICE(_logging_, "Cut named '" << cut_entry_.get_cut_name() << "' has been created !");
    return;
  }

  // void cut_manager::initialize_cut (cut_entry_type & cut_entry_)
  // {
  //   if (! cut_entry_.is_initialized ()) {
  //     _initialize_cut (cut_entry_);
  //   }
  //   return;
  // }

  void cut_manager::_initialize_cut (cut_entry_type & cut_entry_)
  {
    // If not created, first do it :
    if (! cut_entry_.is_created ()) {
      _create_cut (cut_entry_);
    }

    // If not initialized, do it :
    if (! cut_entry_.is_initialized ()) {
      DT_LOG_DEBUG(_logging_,
                   "Initializing cut named '"
                   <<  cut_entry_.get_cut_name ()
                   << "'...");
      i_cut & the_cut = cut_entry_.grab_cut ();
      if (has_service_manager ()) {
        the_cut.initialize (cut_entry_.get_cut_config (),
                            grab_service_manager (),
                            _cuts_);
      } else {
        DT_LOG_NOTICE(_logging_, "Attempt to initializing cut named '"
                      <<  cut_entry_.get_cut_name ()
                      << "' without service support...");
        the_cut.initialize_without_service (cut_entry_.get_cut_config (), _cuts_);
      }
      cut_entry_.set_initialized ();
    }
    return;
  }

  void cut_manager::_reset_cut (cut_entry_type & cut_entry_)
  {
    if (cut_entry_.is_initialized ()) {
      i_cut & the_cut = cut_entry_.grab_cut ();
      the_cut.reset ();
      cut_entry_.set_uninitialized ();
    }
    return;
  }

  void cut_manager::_load_cut (const std::string & cut_name_,
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
    the_cut_entry.set_blank ();
    bool force_initialization_at_load = false;
    if (cut_config_.has_flag ("force_initialization_at_load")) {
      force_initialization_at_load = true;
    }
    if (force_initialization_at_load || is_initialization_at_load ()) {
      _initialize_cut (the_cut_entry);
    }

    DT_LOG_TRACE(_logging_, "Exiting.");
    return;
  }

  void cut_manager::load_cut (const std::string & cut_name_,
                              const std::string & cut_id_,
                              const datatools::properties & cut_config_)
  {
    DT_THROW_IF(is_initialized (),
                std::logic_error,
                "Cut manager is already initialized !");
    _load_cut (cut_name_, cut_id_, cut_config_);
    return;
  }


  void cut_manager::_load_cuts (const datatools::multi_properties & config_)
  {
    using namespace datatools;
    DT_LOG_DEBUG(_logging_, "Cut manager configuration parameters : ");
    if (is_debug()) {
      config_.tree_dump (std::clog, "");
    }
    const multi_properties::entries_ordered_col_type & oe = config_.ordered_entries ();
    for (multi_properties::entries_ordered_col_type::const_iterator i = oe.begin ();
         i != oe.end ();
         ++i) {
      const multi_properties::entry & the_entry = **i;
      const std::string & cut_name = the_entry.get_key ();
      DT_LOG_NOTICE(_logging_, "Loading cuts '" << cut_name << "'...");
      DT_THROW_IF(has (cut_name),
                  std::logic_error,
                  "Cut manager already has a cut named '" << cut_name << "' !");
      const std::string & cut_id = the_entry.get_meta ();
      const properties & cut_config = the_entry.get_properties ();
      _load_cut (cut_name, cut_id, cut_config);
    }
    return;
  }

  void cut_manager::reset ()
  {
    DT_THROW_IF(! is_initialized (),
                std::logic_error,
                "Cut manager is not initialized !");
    _cuts_.clear ();
    _factory_register_.reset ();

    if (_service_manager_owner_) {
      if (has_service_manager ()) {
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

  bool cut_manager::has (const string & cut_name_) const
  {
    return _cuts_.find (cut_name_) != _cuts_.end ();
  }

  void cut_manager::remove (const string & cut_name_)
  {
    _cuts_.erase (cut_name_);
    return;
  }

  i_cut &
  cut_manager::grab (const string & cut_name_)
  {
    cut_handle_dict_type::iterator found = _cuts_.find (cut_name_);
    DT_THROW_IF(found == _cuts_.end (),
                std::domain_error,
                "No cut named '" << cut_name_ << "' !");
    if (! found->second.is_initialized ()) {
      _initialize_cut (found->second);
    }
    return found->second.grab_cut ();
  }

  const i_cut &
  cut_manager::get (const string & cut_name_) const
  {
    cut_manager * mutable_this = const_cast<cut_manager*> (this);
    return const_cast<i_cut &> (mutable_this->grab (cut_name_));
  }

  cut_handle_dict_type & cut_manager::get_cuts ()
  {
    return _cuts_;
  }

  const cut_handle_dict_type & cut_manager::get_cuts () const
  {
    return _cuts_;
  }

  void cut_manager::tree_dump (ostream      & out_,
                               const string & title_,
                               const string & indent_,
                               bool inherit_) const
  {
    std::string indent;
    if (! indent.empty()) {
      indent = indent_;
    }
    if ( ! title_.empty()) {
      out_ << indent << title_ << endl;
    }

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Initialized       : " << is_initialized () << endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Flags             : " << _flags_ << endl;
    out_ << indent << datatools::i_tree_dumpable::tag
         << "List of registered services : " << endl;
    {
      ostringstream indent_oss;
      indent_oss << indent << datatools::i_tree_dumpable::skip_tag;
      _factory_register_.print (out_, indent_oss.str ());
    }

    {
      out_ << indent << datatools::i_tree_dumpable::tag
           << "Cuts : ";
      size_t sz = _cuts_.size ();
      if (sz == 0) {
        out_ << "<none>";
      }
      out_ << endl;
      for (cut_handle_dict_type::const_iterator i = _cuts_.begin ();
           i != _cuts_.end ();
           i++) {
        const string & the_cut_name = i->first;
        const cut_entry_type & the_cut_entry = i->second;
        out_ << indent << datatools::i_tree_dumpable::skip_tag;

        ostringstream indent_oss;
        indent_oss << indent << datatools::i_tree_dumpable::skip_tag;
        cut_handle_dict_type::const_iterator j = i;
        j++;
        if (j == _cuts_.end ()) {
          out_ << datatools::i_tree_dumpable::last_tag;
          indent_oss << datatools::i_tree_dumpable::last_skip_tag;
        } else {
          out_ << datatools::i_tree_dumpable::tag;
          indent_oss << datatools::i_tree_dumpable::skip_tag;
        }
        out_ << "Cut : '" << the_cut_name << "'" << endl;
        the_cut_entry.tree_dump (out_, "", indent_oss.str ());
      }
    }
    out_ << indent << datatools::i_tree_dumpable::tag
         << "Service manager   :" ;
    if (has_service_manager ()) {
      out_ << " @ " << _service_manager_ << endl;
      ostringstream indent_ss;
      indent_ss << indent << datatools::i_tree_dumpable::skip_tag;
      _service_manager_->tree_dump (clog, "", indent_ss.str ());
    } else {
      out_ << " <none>" << endl;
    }

    out_ << indent << datatools::i_tree_dumpable::inherit_tag (inherit_)
         << "Owns service manager  : " << _service_manager_owner_ << endl;

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
  ocd_.set_class_name ("cuts::cut_manager");
  ocd_.set_class_description ("A cut manager");
  ocd_.set_class_library ("cuts");
  ocd_.set_class_documentation ("not documented yet");

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("logging.priority")
      .set_terse_description("Logging priority threshold")
      .set_traits(datatools::TYPE_STRING)
      .set_mandatory(false)
      .set_long_description("This parameters sets the logging level.               \n"
                            "Default value is ``\"fatal\"``                        \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("factory.verbose")
      .set_terse_description("Verbosity flag dedicated to the embeded cut factory")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("This flag activates verbose printing for the embeded  \n"
                            "*cut factory*.                                        \n"
                            )
      ;

  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("factory.no_preload")
      .set_terse_description("Flag to inhibit the preloading of system cut factory")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("When set, this flag inhibits the automatic loading of    \n"
                            "cut factories registered in the system cut registration. \n"
                            "Not recommanded for standard usage.                      \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("factory.initialization_at_load")
      .set_terse_description("Flag to automaticaly initialize cut objects at load")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("When set, this flag triggers the automatic initialization of a   \n"
                            "cut while it is loaded. The default behaviour consists in        \n"
                            "initializing a cut if and only if it is effectively used.        \n"
                            "Not recommanded for standard usage.                              \n"
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
      .set_long_description("This property indicated the name of the configuration file   \n"
                            "for the service manager made accessible to the cuts objects. \n"
                            "It is only used if no external service manager has been      \n"
                            "defined through the                                          \n"
                            "``cuts::cut_manager::set_service_manager(...)`` method.      \n"
                            );
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
      .set_long_description("This property indicated the name of the configuration files       \n"
                            "that stored the rules to instantiate cuts objects.                \n"
                            "The filenames may contains some environment variables.            \n"
                            "Each file must use the format of the ``datatools::multi_properties``\n"
                            "class.                                                            \n"
                            "                                                                  \n"
                            "Example::                                                         \n"
                            "                                                                  \n"
                            "   cuts.configuration_files : string[3] a path = \\               \n"
                            "     \"${CUTS_CONFIG_DIR}/energy_cuts.conf\" \\                   \n"
                            "     \"${CUTS_CONFIG_DIR}/track_cuts.conf\"  \\                   \n"
                            "     \"${CUTS_CONFIG_DIR}/topology_cuts.conf\"                    \n"
                            "                                                                  \n"
                           );
   }

  ocd_.set_configuration_hints ("Here is a typical configuration::                                 \n"
                                "                                                                  \n"
                                "   logging.priority   : string = \"warning\"                      \n"
                                "   factory.verbose    : boolean = 0                               \n"
                                "   factory.no_preload : boolean = 0                               \n"
                                "   factory.initialization_at_load : boolean = 0                   \n"
                                "   service_manager.configuration : string as path = \\            \n"
                                "     \"${CUTS_CONFIG_DIR}/srvcmgr.conf\"                          \n"
                                "   cuts.configuration_files : string[3] a path = \\               \n"
                                "     \"${CUTS_CONFIG_DIR}/energy_cuts.conf\" \\                   \n"
                                "     \"${CUTS_CONFIG_DIR}/track_cuts.conf\"  \\                   \n"
                                "     \"${CUTS_CONFIG_DIR}/topology_cuts.conf\"                    \n"
                                "                                                                  \n"
                               );

  ocd_.set_validation_support(true);
  ocd_.lock();
  return;
}
DOCD_CLASS_IMPLEMENT_LOAD_END()

DOCD_CLASS_SYSTEM_REGISTRATION(::cuts::cut_manager,"cuts::cut_manager")

// end of cut_manager.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
