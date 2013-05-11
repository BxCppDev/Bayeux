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

namespace cuts {

  using namespace std;

  bool cut_manager::is_debug () const
  {
    return _flags_ & DEBUG;
  }

  bool cut_manager::is_verbose () const
  {
    return _flags_ & VERBOSE;
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
    bool devel = false;
    if (devel)
      {
        clog << datatools::io::devel
             << "cuts::cut_manager::_preload_global_dict: "
             << "Entering..."
             << endl;
      }
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

  void cut_manager::set_debug (bool debug_)
  {
    if (debug_)
      {
        _flags_ |= DEBUG;
      }
    else
      {
        _flags_ &= DEBUG;
      }
    return;
  }

  void cut_manager::set_verbose (bool verbose_)
  {
    if (verbose_)
      {
        _flags_ |= VERBOSE;
      }
    else
      {
        _flags_ &= VERBOSE;
      }
    return;
  }

  // ctor:
  cut_manager::cut_manager (uint32_t flags_)
  {
    _initialized_           = false;
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
    if (! has_service_manager ())
      {
        ostringstream message;
        message << "cuts::cut_manager::get_service_manager: "
                << "Cut manager has no service manager !";
        throw logic_error (message.str ());
      }
    return *_service_manager_;
  }

  datatools::service_manager &
  cut_manager::grab_service_manager ()
  {
    if (! has_service_manager ())
      {
        ostringstream message;
        message << "cuts::cut_manager::grab_service_manager: "
                << "Cut manager has no service manager !";
        throw logic_error (message.str ());
      }
    return *_service_manager_;
  }

  void cut_manager::set_service_manager (datatools::service_manager & service_manager_)
  {
    if (is_initialized ())
      {
        ostringstream message;
        message << "cuts::cut_manager::set_service_manager: "
                << "Cut manager is already initialized !";
        throw logic_error (message.str ());
      }
    _service_manager_owner_ = false;
    _service_manager_ = &service_manager_;
    return;
  }

  // dtor:
  cut_manager::~cut_manager ()
  {
    if (is_initialized ())
      {
        cut_manager::reset ();
      }
    return;
  }

  void cut_manager::install_service_manager (const datatools::properties & service_manager_configuration_)
  {
    if (is_debug ())
      {
        clog << datatools::io::debug
             << "cuts::cut_manager::install_service_manager: "
             << "Entering..." << endl;
      }
    if (is_initialized ())
      {
        ostringstream message;
        message << "cuts::cut_manager::install_service_manager: "
                << "Cut manager is already initialized !";
        throw logic_error (message.str ());
      }
    if (has_service_manager ())
      {
        ostringstream message;
        message << "cuts::cut_manager::install_service_manager: "
                << "Cut manager has already an embedded service manager !";
        throw logic_error (message.str ());
      }
    _service_manager_ = new datatools::service_manager;
    if (is_verbose ())
      {
        clog << datatools::io::notice
             << "cuts::cut_manager::install_service_manager: "
             << "Initializing the embedded service manager..." << endl;
      }
    _service_manager_->initialize (service_manager_configuration_);
    _service_manager_owner_ = true;
    if (is_debug ())
      {
        clog << datatools::io::debug
             << "cuts::cut_manager::install_service_manager: "
             << "Exiting." << endl;
      }
    return;
  }

  bool cut_manager::is_initialized () const
  {
    return _initialized_;
  }

  void cut_manager::initialize (const datatools::properties & setup_)
  {
    if (is_initialized ())
      {
        ostringstream message;
        message << "cuts::cut_manager::initialize: "
                << "Cut manager is already initialized !";
        throw logic_error (message.str ());
      }

    if (setup_.has_flag ("debug"))
      {
        set_debug (true);
      }

    if (setup_.has_flag ("verbose"))
      {
        set_verbose (true);
      }

    if (setup_.has_flag ("factory.debug"))
      {
        _flags_ |= FACTORY_DEBUG;
      }

    if (setup_.has_flag ("factory.no_preload"))
      {
        _flags_ |= FACTORY_NOPRELOAD;
      }

    if (setup_.has_flag ("factory.initialization_at_load"))
      {
        _flags_ |= FACTORY_INITIALIZATION_AT_LOAD ;
      }

    // Embed a  service manager :
    if (! has_service_manager ())
      {
        string service_manager_configuration_file;
        if (setup_.has_key ("service_manager.configuration"))
          {
            service_manager_configuration_file
              = setup_.fetch_string ("service_manager.configuration");
            if (is_verbose ())
              {
                clog << datatools::io::notice
                     << "cuts::cut_manager::initialize: "
                     << "Embeds a service manager setup from file '"
                     << service_manager_configuration_file << "' !" << endl;
              }
            datatools::fetch_path_with_env (service_manager_configuration_file);
            datatools::properties service_manager_configuration;
            datatools::properties::read_config (service_manager_configuration_file,
                                                       service_manager_configuration);
            install_service_manager (service_manager_configuration);
          }
        else
          {
            clog << datatools::io::warning
                 << "cuts::cut_manager::initialize: "
                 << "No service manager configuration property !" << endl;
          }
      }

    // Instantiate a cut factory :
    {
      _factory_register_.set_label ("cuts::i_cut/cut_factory");
      _factory_register_.set_verbose (_flags_ & FACTORY_DEBUG);
      if (! is_no_preload ())
        {
          _preload_global_dict ();
        }
    }

    // Check if some service manager is available :
    if (! has_service_manager ())
      {
        clog << datatools::io::warning
             << "cuts::cut_manager::initialize: "
             << "No service manager is available ! "
             << "This could forbid the use of some cuts !" << endl;
      }
    else
      {
        clog << datatools::io::notice
             << "cuts::cut_manager::initialize: "
             << "We now have a service manager available !" << endl;
      }

    // Load and create the data processing cuts :
    vector<string> cuts_configurations;
    if (setup_.has_key ("cuts.configuration_files"))
      {
        setup_.fetch ("cuts.configuration_files", cuts_configurations);
      }
    for (size_t i = 0; i < cuts_configurations.size (); ++i)
      {
        string filename = cuts_configurations[i];
        datatools::fetch_path_with_env (filename);
        datatools::multi_properties configs;
        clog << datatools::io::notice
             << "cuts::cut_manager::initialize: "
             << "Loading cuts from file '"
             << filename << "'..." << endl;
        configs.read (filename);
        _load_cuts (configs);
      }

    _initialized_ = true;
    return;
  }

  void cut_manager::create_cut (cut_entry_type & cut_entry_)
  {
    if (! cut_entry_.is_created ())
      {
        this->_create_cut (cut_entry_);
      }
    return;
  }

  void cut_manager::_create_cut (cut_entry_type & cut_entry_)
  {
    if (cut_entry_.is_created ())
      {
        return;
      }

    if (is_debug ())
      {
        clog << datatools::io::debug
             << "cuts::cut_manager::_create_cut: "
             << "Creating cut named '"
             <<  cut_entry_.get_cut_name ()
             << "'..."
             << endl;
      }

    // search for the cut's label in the factory dictionary:
    if (! _factory_register_.has (cut_entry_.get_cut_id ()))
      {
        ostringstream message;
        message << "cuts::cut_manager::_create_cut: "
                << "Cannot find cut factory with ID '"
                << cut_entry_.get_cut_id () << "' for cut named '"
                << cut_entry_.get_cut_name () << "' !";
        throw logic_error (message.str ());
      }
    const i_cut::factory_register_type::factory_type & the_factory
      = _factory_register_.get (cut_entry_.get_cut_id ());

    i_cut * ptr = the_factory ();
    cut_entry_.set_ptr (ptr);
    cut_entry_.set_created ();

    if (is_debug ())
      {
        clog << datatools::io::debug
             << "cuts::cut_manager::_create_cut: "
             << "Cut named '"
             <<  cut_entry_.get_cut_name ()
             << "' has been created !"
             << endl;
      }

    return;
  }

  void cut_manager::initialize_cut (cut_entry_type & cut_entry_)
  {
    if (! cut_entry_.is_initialized ())
      {
        _initialize_cut (cut_entry_);
      }
    return;
  }

  void cut_manager::_initialize_cut (cut_entry_type & cut_entry_)
  {
    // If not created, first do it :
    if (! cut_entry_.is_created ())
      {
        _create_cut (cut_entry_);
      }

    // If not initialized, do it :
    if (! cut_entry_.is_initialized ())
      {
        if (is_debug ())
          {
            clog << datatools::io::debug
                 << "cuts::cut_manager::_initialize_cut: "
                 << "Initializing cut named '"
                 <<  cut_entry_.get_cut_name ()
                 << "'..."
                 << endl;
          }
        i_cut & the_cut = cut_entry_.grab_cut ();
        if (has_service_manager ())
          {
            the_cut.initialize (cut_entry_.get_cut_config (),
                                grab_service_manager (),
                                _cuts_);
          }
        else
          {
            clog << datatools::io::notice
                 << "cuts::cut_manager::_initialize_cut: "
                 << "Attempt to initializing cut named '"
                 <<  cut_entry_.get_cut_name ()
                 << "' without service support..."
                 << endl;
            the_cut.initialize_without_service (cut_entry_.get_cut_config (),
                                                _cuts_);
          }
        cut_entry_.set_initialized ();
      }
    return;
  }

  void cut_manager::_reset_cut (cut_entry_type & cut_entry_)
  {
    if (cut_entry_.is_initialized ())
      {
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
    bool debug = is_debug ();
    if (debug)
      {
        clog << datatools::io::debug
             << "cuts::cut_manager::_load_cut: "
             << "Entering..."
             << endl;
      }

    if (has (cut_name_))
      {
        ostringstream message;
        message << "cuts::cut_manager::_load_cut: "
                << "Cut '" << cut_name_ << "' already exists !";
        throw logic_error (message.str());
      }

    {
      // Add a new entry :
      cut_entry_type dummy_cut_entry;
      //dummy_cut_entry.set_cut_name (cut_name_);
      if (debug)
        {
          clog << datatools::io::debug
               << "cuts::cut_manager::_load_cut: "
               << "Add an entry for cut '" << cut_name_ << "'..."
               << endl;
        }
      _cuts_[cut_name_] = dummy_cut_entry;
      //_cuts_[cut_name_] = dummy_cut_entry.cut_handle;
    }

    // fetch a reference on it and update its internal values :
    cut_entry_type & the_cut_entry = _cuts_.find (cut_name_)->second;
    the_cut_entry.set_cut_name (cut_name_);
    the_cut_entry.set_manager (*this);
    the_cut_entry.set_cut_id (cut_id_);
    the_cut_entry.set_cut_config (cut_config_);
    the_cut_entry.set_blank ();
    if (debug)
      {
        clog << datatools::io::debug
             << "cuts::cut_manager::_load_cut: "
             << "Fetch..."
             << endl;
      }
    //_create_cut (the_cut_entry);

    bool force_initialization_at_load = false;

    if (cut_config_.has_flag ("force_initialization_at_load"))
      {
        force_initialization_at_load = true;
      }

    if (force_initialization_at_load || is_initialization_at_load ())
      {
        _initialize_cut (the_cut_entry);
      }

    if (debug)
      {
        clog << datatools::io::debug
             << "cuts::cut_manager::_load_cut: "
             << "Exiting."
             << endl;
      }
    return;
  }

  void cut_manager::load_cut (const std::string & cut_name_,
                              const std::string & cut_id_,
                              const datatools::properties & cut_config_)
  {
    if (is_initialized ())
      {
        ostringstream message;
        message << "cuts::cut_manager::load_cut: "
                << "Cut manager is already initialized !";
        throw logic_error (message.str ());
      }
    _load_cut (cut_name_, cut_id_, cut_config_);
    return;
  }


  void cut_manager::_load_cuts (const datatools::multi_properties & config_)
  {
    using namespace datatools;
    if (is_debug())
      {
        config_.tree_dump (std::clog,
                           "cuts::cut_manager::_load_cuts: ",
                           "DEBUG: ");
      }
    const multi_properties::entries_ordered_col_type & oe = config_.ordered_entries ();
    for (multi_properties::entries_ordered_col_type::const_iterator i =
           oe.begin ();
         i != oe.end ();
         ++i)
      {
        const multi_properties::entry & the_entry = **i;
        const std::string & cut_name = the_entry.get_key ();
        clog << datatools::io::notice
             << "cuts::cut_manager::_load_cuts: "
             << "Loading cuts '" << cut_name << "'..." << endl;
        if (has (cut_name))
          {
            std::ostringstream message;
            message << "cuts::cut_manager::load_cuts_: "
                    << "Cut manager already has a cut named '"
                    << cut_name << "' !";
            throw std::logic_error (message.str ());
          }
        const std::string & cut_id = the_entry.get_meta ();
        const properties & cut_config = the_entry.get_properties ();
        _load_cut (cut_name, cut_id, cut_config);
      }
    return;
  }

  void cut_manager::reset ()
  {
    if (! is_initialized ())
      {
        std::ostringstream message;
        message << "cuts::cut_manager::reset: "
                << "Cut manager is not initialized !";
        throw std::logic_error (message.str ());
      }

    _cuts_.clear ();
    _factory_register_.reset ();

    if (_service_manager_owner_)
      {
        if (has_service_manager ())
          {
            delete _service_manager_;
          }
      }
    _service_manager_ = 0;
    _service_manager_owner_ = false;
    _flags_ = BLANK;
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
    if (found == _cuts_.end ())
      {
        std::ostringstream message;
        message << "cuts::cut_manager::grab: "
                << "No cut named '" << cut_name_ << "' !";
        throw std::logic_error (message.str ());
      }
    if (! found->second.is_initialized ())
      {
        _initialize_cut (found->second);
      }
    return found->second.grab_cut ();
  }

  const i_cut &
  cut_manager::get (const string & cut_name_) const
  {
    cut_manager * mutable_this = const_cast<cut_manager*> (this);
    return const_cast<i_cut &> (mutable_this->grab (cut_name_));
    /*
      cut_handle_dict_type::const_iterator found
      = _cuts_.find (cut_name_);
      if (found == _cuts_.end ())
      {
      ostringstream message;
      message << "cuts::cut_manager::get: "
      << "No cut named '" << cut_name_ << "' !";
      throw logic_error (message.str ());
      }
      if (found->second.is_initialized ())
      {
      cut_manager * mutable_this = const_cast<cut_manager*> (this);
      mutable_this->_initialize_cut (found->second);
      }
      return found->second.get_cut ();
    */
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
    if (! indent.empty ())
      {
        indent = indent_;
      }
    if ( ! title_.empty () )
      {
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
      if (sz == 0)
        {
          out_ << "<none>";
        }
      out_ << endl;
      for (cut_handle_dict_type::const_iterator i = _cuts_.begin ();
           i != _cuts_.end ();
           i++)
        {
          const string & the_cut_name = i->first;
          const cut_entry_type & the_cut_entry = i->second;
          out_ << indent << datatools::i_tree_dumpable::skip_tag;

          ostringstream indent_oss;
          indent_oss << indent << datatools::i_tree_dumpable::skip_tag;
          cut_handle_dict_type::const_iterator j = i;
          j++;
          if (j == _cuts_.end ())
            {
              out_ << datatools::i_tree_dumpable::last_tag;
              indent_oss << datatools::i_tree_dumpable::last_skip_tag;
            }
          else
            {
              out_ << datatools::i_tree_dumpable::tag;
              indent_oss << datatools::i_tree_dumpable::skip_tag;
            }
          out_ << "Cut : '" << the_cut_name << "'" << endl;
          the_cut_entry.tree_dump (out_, "", indent_oss.str ());
        }
    }

    /**
       if (is_initialized ())
       {
       size_t sz = _cuts_.size ();
       out_ << indent << datatools::i_tree_dumpable::tag
       << "Cuts           : " << sz << " element" << (sz < 2? "": "s")  << endl;
       ostringstream indent_ss;
       int count = 0;
       for (cut_handle_dict_type::const_iterator it = _cuts_.begin ();
       it != _cuts_.end ();
       it++)
       {
       count++;
       out_ << indent;
       out_ << datatools::i_tree_dumpable::skip_tag;
       if (count == sz)
       {
       out_ << datatools::i_tree_dumpable::last_tag;
       }
       else
       {
       out_ << datatools::i_tree_dumpable::tag;
       }
       out_ << "Name='" << it->first
       << "' : type='" << it->second.get ().get_name () << "'"
       << endl;
       }
       }
    **/

    out_ << indent << datatools::i_tree_dumpable::tag
         << "Service manager   :" ;
    if (has_service_manager ())
      {
        out_ << " @ " << _service_manager_ << endl;
        ostringstream indent_ss;
        indent_ss << indent << datatools::i_tree_dumpable::skip_tag;
        _service_manager_->tree_dump (clog, "", indent_ss.str ());
      }
    else
      {
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
    cpd.set_name_pattern("debug")
      .set_terse_description("Debug flag")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("This flag activates debug printing.               \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("verbose")
      .set_terse_description("verbose flag")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("This flag activates verbosity printing.               \n"
                            )
      ;
  }

  {
    configuration_property_description & cpd = ocd_.add_configuration_property_info();
    cpd.set_name_pattern("factory.debug")
      .set_terse_description("Debug flag dedicated to the embeded cut factory")
      .set_traits(datatools::TYPE_BOOLEAN)
      .set_mandatory(false)
      .set_long_description("This flag activates debug printing for the embeded  \n"
                            "*cut factory*.                                      \n"
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

  ocd_.set_configuration_hints ("Configuration example::                                           \n"
                                "                                                                  \n"
                                "   debug : boolean = 0                                            \n"
                                "   verbose : boolean = 0                                          \n"
                                "   factory.debug : boolean = 0                                    \n"
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
