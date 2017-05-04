/* i_cut.cc
 *
 * Copyright (C) 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
#include <cuts/i_cut.h>

// Standard library:
#include <stdexcept>
#include <sstream>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/ioutils.h>
#include <datatools/service_manager.h>
#include <datatools/exception.h>

namespace cuts {

  DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(i_cut, "cuts::i_cut/__system__")

  bool
  i_cut::is_debug() const
  {
    return _logging >= datatools::logger::PRIO_DEBUG;
  }

  void i_cut::set_logging_priority(datatools::logger::priority p)
  {
    _logging = p;
  }

  datatools::logger::priority i_cut::get_logging_priority() const
  {
    return _logging;
  }

  bool i_cut::is_initialized() const
  {
    return _initialized_;
  }

  void i_cut::_set_initialized(bool a_initialized)
  {
    _initialized_ = a_initialized;
  }

  bool i_cut::has_name() const
  {
    return ! _name.empty();
  }

  const std::string & i_cut::get_name() const
  {
    return _name;
  }

  void i_cut::set_name(const std::string & a_new_value)
  {
    DT_THROW_IF(is_initialized(),
                std::logic_error,
                "Cut '" << _name << "' "
                << "is already initialized ! "
                << "Cannot change the name !");
    _set_name(a_new_value);
    return;
  }

  void i_cut::_set_name(const std::string & a_new_value)
  {
    _name = a_new_value;
  }

  bool i_cut::has_description() const
  {
    return ! _description.empty();
  }

  const std::string & i_cut::get_description() const
  {
    return _description;
  }

  void i_cut::set_description(const std::string & a_description)
  {
    _description = a_description;
  }

  bool i_cut::has_version() const
  {
    return ! _version.empty();
  }

  const std::string & i_cut::get_version() const
  {
    return _version;
  }

  void i_cut::set_version(const std::string & a_version)
  {
    _version = a_version;
  }

  size_t i_cut::get_number_of_accepted_entries() const
  {
    return _number_of_accepted_entries_;
  }

  size_t i_cut::get_number_of_rejected_entries() const
  {
    return _number_of_rejected_entries_;
  }

  size_t i_cut::get_number_of_processed_entries() const
  {
    return get_number_of_accepted_entries() + get_number_of_rejected_entries();
  }

  bool i_cut::is_activated_counters() const
  {
    return _activated_counters_;
  }

  void i_cut::set_activated_counters(bool ac_)
  {
    _activated_counters_ = ac_;
  }

  void i_cut::reset_counters()
  {
    _number_of_accepted_entries_ = 0;
    _number_of_rejected_entries_ = 0;
  }

  void i_cut::_set_defaults()
  {
    _activated_counters_ = true;
    _number_of_accepted_entries_ = 0;
    _number_of_rejected_entries_ = 0;
  }

  i_cut::i_cut(datatools::logger::priority p)
  {
    _initialized_ = false;
    _logging = p;
    _set_defaults();
  }

  i_cut::~i_cut()
  {
    DT_LOG_TRACE(_logging, "Destruction.");
    _reset();
    if (is_initialized()) {
      DT_LOG_ERROR(datatools::logger::PRIO_ALWAYS, "Cut '" << _name << "' "
                   << "still has its 'initialized' flag on ! "
                   << "Possible bug !");
    }
    DT_LOG_TRACE(_logging, "Destruction done.");
  }

  void i_cut::print(std::ostream & a_out) const
  {
    this->tree_dump(a_out, "Base cut :");
  }

  void i_cut::tree_dump(std::ostream & a_out ,
                        const std::string & a_title,
                        const std::string & a_indent,
                        bool a_inherit) const
  {
    std::string indent;
    if (! a_indent.empty()) {
      indent = a_indent;
    }
    if ( ! a_title.empty() )  {
      a_out << indent << a_title << std::endl;
    }
    a_out << indent << datatools::i_tree_dumpable::tag
          << "Cut logging priority : '"
          << datatools::logger::get_priority_label(_logging) << "'" << std::endl;
    a_out << indent << datatools::i_tree_dumpable::tag
          << "Cut name        : '" << _name << "'" << std::endl;
    a_out << indent << datatools::i_tree_dumpable::tag
          << "Cut description : '" << _description << "'" << std::endl;
    a_out << indent << datatools::i_tree_dumpable::tag
          << "Cut version     : '" << _version << "'" << std::endl;

    if (_supported_types_.size()) {
      a_out << indent << datatools::i_tree_dumpable::tag
            << "Supported types : " << std::endl;
      for (std::set<const std::type_info *>::const_iterator iter = _supported_types_.begin();
           iter != _supported_types_.end();
           iter++) {
        const std::type_info * ti = *iter;
        std::set<const std::type_info *>::const_iterator jter = iter;
        jter++;
        a_out << a_indent <<  datatools::i_tree_dumpable::skip_tag;
        if (jter == _supported_types_.end()) {
          a_out << datatools::i_tree_dumpable::last_tag;
        } else {
          a_out << datatools::i_tree_dumpable::tag;
        }
        a_out << "Type '" << ti->name() << "'" << std::endl;
      }
    }

    a_out << indent << datatools::i_tree_dumpable::tag
          << "User data : ";
    if (_user_data_.get() != 0) {
      a_out << "yes (type='" << _user_data_.get()->get_typeinfo()->name() << "')";
    } else {
      a_out << "<none>";
    }
    a_out << std::endl;
    a_out << indent << datatools::i_tree_dumpable::tag
          << "Activated counters : " << _activated_counters_
          << std::endl;
    if (_activated_counters_) {
      const size_t npe = _number_of_accepted_entries_ + _number_of_rejected_entries_;
      a_out << indent << datatools::i_tree_dumpable::tag
            << "Number of processed entries : " << npe
            << std::endl;
      a_out << indent << datatools::i_tree_dumpable::tag
            << "Number of selected entries  : " << _number_of_accepted_entries_;
      if (npe > 0) {
        a_out << " ("
              << (100.0*_number_of_accepted_entries_/npe)
              << "%)";
      }
      a_out << std::endl;
      a_out << indent << datatools::i_tree_dumpable::tag
            << "Number of rejected entries  : " << _number_of_rejected_entries_;
      if (npe > 0) {
        a_out << " ("
              << (100.0*_number_of_rejected_entries_/npe)
              << "%)";
      }
      a_out << std::endl;
    }

    a_out << indent << datatools::i_tree_dumpable::inherit_tag(a_inherit)
          << "Cut initialized : " << is_initialized() << std::endl;

    return;
  }

  void i_cut::_common_initialize(const datatools::properties & a_config)
  {
    DT_LOG_TRACE(_logging, "Entering common initialization of the cut...");

    // Logging priority:
    datatools::logger::priority lp
      = datatools::logger::extract_logging_configuration(a_config,
                                                         _logging);
    DT_THROW_IF(lp ==  datatools::logger::PRIO_UNDEFINED,
                std::logic_error,
                "Invalid logging priority !");
    set_logging_priority(lp);

    if (! has_name()) {
      if (a_config.has_key("cut.name")) {
        set_name(a_config.fetch_string("cut.name"));
      }
    }

    if (! has_description()) {
      if (a_config.has_key("cut.description")) {
        set_description(a_config.fetch_string("cut.description"));
      }
    }

    if (! has_version()) {
      if (a_config.has_key("cut.version")) {
        set_version(a_config.fetch_string("cut.version"));
      }
    }
    DT_LOG_TRACE(_logging, "Exiting common initialization of the cut.");
  }

  void i_cut::initialize_simple()
  {
    datatools::properties dummy_config;
    datatools::service_manager dummy_service_manager;
    cut_handle_dict_type dummy_cut_dict;
    initialize(dummy_config, dummy_service_manager, dummy_cut_dict);
    return;
  }

  void i_cut::initialize_standalone(const datatools::properties & a_config)
  {
    datatools::service_manager dummy;
    cut_handle_dict_type dummy2;
    initialize(a_config, dummy, dummy2);
    return;
  }

  void i_cut::initialize_with_service_only(const datatools::properties & a_config,
                                           datatools::service_manager & a_service_manager)
  {
    cut_handle_dict_type dummy;
    initialize(a_config, a_service_manager, dummy);
    return;
  }

  void i_cut::initialize_without_service(const datatools::properties & a_config,
                                         cut_handle_dict_type & a_cut_dictionary)
  {
    datatools::service_manager dummy_service_manager;
    initialize(a_config, dummy_service_manager, a_cut_dictionary);
    return;
  }

  void i_cut::export_to_config(datatools::properties & config_,
                               uint32_t flags_,
                               const std::string & prefix_) const
  {
    DT_THROW_IF(!is_initialized(), std::logic_error,
                "Cut '" << get_name() << "' is not initialized!") ;

    if (flags_ & EXPORT_CONFIG_CLEAR) {
      config_.clear();
    }
    if (flags_ & EXPORT_CONFIG_NAME) {
      config_.store_string(prefix_ + "cut.name", get_name(), "Name");
    }
    if (flags_ & EXPORT_CONFIG_DESCRIPTION) {
      config_.store_string(prefix_ + "cut.description", get_description(), "Description");
    }
    if (flags_ & EXPORT_CONFIG_VERSION) {
      config_.store_string(prefix_ + "cut.version", get_name(), "Version");
    }
    if (flags_ & EXPORT_CONFIG_LOGGING_PRIORITY) {
      config_.store_string(prefix_ + "logging.priority",
                           datatools::logger::get_priority_label(get_logging_priority()),
                           "Logging priority threshold");
    }
    return;
  }

  bool i_cut::has_user_data() const
  {
    return _user_data_.get() != 0;
  }

  void i_cut::reset_user_data()
  {
    if (_user_data_) {
      DT_LOG_TRACE(_logging, "Cut named '" << (has_name()? get_name() : "?") << "' resets user data.");
      _user_data_.reset();
    }
  }

  bool i_cut::is_user_data_type_supported(const std::type_info & tinfo_) const
  {
    if (_supported_types_.size() == 0) {
      return true;
    }
    if (_supported_types_.count(&tinfo_) == 1) {
      DT_LOG_TRACE(_logging, "Supported registered type '" << tinfo_.name() << "'.");
      return true;
    }
    return false;
  }

  void i_cut::_set_user_data(const boost::shared_ptr<i_referenced_data> & rd_)
  {
    DT_LOG_TRACE(_logging,
                 "Cut named '" << (has_name()? get_name() : "?")
                 << "' sets user data of type '" << rd_.get()->get_typeinfo()->name() << "'.");
    DT_THROW_IF(!is_user_data_type_supported(*rd_.get()->get_typeinfo()),
                std::logic_error,
                "User data type '" << rd_.get()->get_typeinfo()->name() << "' is not supported by cut '" << get_name() << "'!");
    _user_data_ = rd_;
    _at_set_user_data();
    DT_LOG_TRACE(_logging, "Exiting.");
  }

  void i_cut::_at_set_user_data()
  {
    DT_LOG_TRACE(_logging,
                 "Cut named '" << (has_name()? get_name() : "?")
                 << "' has user data of type '" << _user_data_.get()->get_typeinfo()->name() << "'.");
  }

  void i_cut::_at_reset_user_data()
  {
    DT_LOG_TRACE(_logging,
                 "Cut named '" << (has_name()? get_name() : "?")
                 << "' has no more referenced user data.");
  }

  void i_cut::_import_user_data_from(const i_cut & a_cut)
  {
    DT_LOG_TRACE(_logging,
                 "Entering: cut named '" << (has_name()? get_name() : "?")
                 << "' imports user data from cut named '"
                 << (a_cut.has_name()? a_cut.get_name() : "?") << "'");
    _set_user_data(a_cut._user_data_);
    DT_LOG_TRACE(_logging, "Exiting.");
  }

  void i_cut::_export_user_data_to(i_cut & a_cut) const
  {
    DT_LOG_TRACE(_logging,
                 "Entering: cut named '" << (has_name()? get_name() : "?")
                 << "' exports user data to cut named '"
                 << (a_cut.has_name()? a_cut.get_name() : "?") << "'");
    a_cut._set_user_data(_user_data_);
    DT_LOG_TRACE(_logging, "Exiting.");
  }

  void i_cut::_prepare_cut()
  {
    DT_LOG_TRACE(_logging, "Entering...");
    DT_THROW_IF(! has_user_data(),
                std::logic_error,
                "Cut named '" << (has_name()? get_name() : "?") << "' references no user data !");
  }

  int i_cut::_finish_cut(int a_selection_status)
  {
    DT_LOG_TRACE(_logging, "Visiting with selection status = " << a_selection_status);
    return a_selection_status;
  }

  void i_cut::_increment_counters(int a_selection_status)
  {
    if (_activated_counters_) {
      if (a_selection_status == SELECTION_ACCEPTED) _number_of_accepted_entries_++;
      else _number_of_rejected_entries_++;
    }
  }

  int i_cut::process()
  {
    DT_LOG_TRACE(_logging,
                 "Entering: processing of the cut named '" << (has_name()? get_name() : "?") << "'");
    int status = SELECTION_INAPPLICABLE;
    _prepare_cut();
    status = _accept();
    status = _finish_cut(status);
    _increment_counters(status);
    DT_LOG_TRACE(_logging, "Exiting with selection status = " << status);
    return status;
  }

  int i_cut::operator()()
  {
    return this->process();
  }

  void i_cut::_reset()
  {
    reset_user_data();
    reset_counters();
  }

  void i_cut::common_ocd(datatools::object_configuration_description & ocd_)
  {
    datatools::logger::declare_ocd_logging_configuration(ocd_, "fatal", "");

    {
      datatools::configuration_property_description & cpd = ocd_.add_property_info();
      cpd.set_name_pattern("cut.description")
        .set_from("cuts::i_cut")
        .set_terse_description("The description of the cut")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_long_description("A description of the cut.")
        .add_example("Example::                                              \n"
                     "                                                       \n"
                     "  cut.description : string = \"Select strange events\" \n"
                     "                                                       \n"
                     )
        ;
    }

    {
      datatools::configuration_property_description & cpd = ocd_.add_property_info();
      cpd.set_name_pattern("cut.name")
        .set_from("cuts::i_cut")
        .set_terse_description("The name of the cut")
        .set_traits(datatools::TYPE_STRING)
        .set_mandatory(false)
        .set_long_description("A name given to the cut.")
        .add_example("Example::                      \n"
                     "                               \n"
                     "  cut.name : string = \"Cut0\" \n"
                     "                               \n"
                     )
        ;
    }
  }

}  // end of namespace cuts

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
