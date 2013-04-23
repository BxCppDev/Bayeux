/* chain_module.cc
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

#include <stdexcept>
#include <sstream>

#include <datatools/properties.h>
#include <datatools/ioutils.h>
#include <datatools/things.h>

#include <dpp/chain_module.h>

namespace dpp {

  bool chain_module::has_module (const std::string & a_label) const
  {
    for (module_list_type::const_iterator i = _modules_.begin ();
         i != _modules_.end ();
         ++i) {
      const module_entry & the_entry = *i;
      const std::string & module_label = the_entry.label;
      if (module_label == a_label) {
        return true;
      }
    }
    return false;
  }

  void chain_module::add_module (const std::string & a_label,
                                 const module_handle_type & a_handle_module)
  {
    if (has_module (a_label)) {
      std::ostringstream message;
      message << "dpp::chain_module::add_module: "
              << "Module '" << a_label << "' already exists !";
      throw std::logic_error (message.str ());
    }
    if (! a_handle_module.has_data ()) {
      std::ostringstream message;
      message << "dpp::chain_module::add_module: "
              << "Invalid module handle for label '" << a_label << "' !";
      throw std::logic_error (message.str ());
    }
    module_entry e;
    e.label = a_label;
    e.handle = a_handle_module;
    _modules_.push_back (e);
    return;
  }

  /*** Implementation of the interface ***/

  // Constructor :
  chain_module::chain_module (int a_debug_level)
    : base_module ("dpp::chain_module",
                   "An data record processor chain module",
                   "0.1",
                   a_debug_level)
  {
    return;
  }

  // Destructor :
  chain_module::~chain_module ()
  {
    // Make sure all internal resources are
    // terminated before destruction :
    if (is_initialized ()) reset ();
    return;
  }

  // Initialization :
  void chain_module::initialize (const datatools::properties & a_config,
                                 datatools::service_manager & a_service_manager,
                                 module_handle_dict_type & a_module_dict)
  {
    if (is_initialized ()) {
      std::ostringstream message;
      message << "dpp::chain_module::initialize: "
              << "Chain module '" << get_name () << "' is already initialized ! ";
      throw std::logic_error (message.str ());
    }

    if (! is_debug ()) {
      if (a_config.has_flag ("debug")) {
        set_debug (true);
      }
    }

    std::vector<std::string> modules;
    if (a_config.has_key ("modules")) {
      a_config.fetch ("modules", modules);
    }
    for (int i = 0; i < modules.size (); i++) {
      module_handle_dict_type::iterator found
        = a_module_dict.find (modules[i]);
      if (found == a_module_dict.end ()) {
        std::ostringstream message;
        message << "dpp::chain_module::initialize: "
                << "Can't find any module named '" << modules[i]
                << "' from the external dictionnary ! ";
        throw std::logic_error (message.str ());
      }
      // XXX
      add_module (modules[i], found->second.grab_initialized_module_handle ());
      std::clog << "WARNING: " << "dpp::chain_module::initialize: "
                << "Chain module '" << get_name ()
                << "' has added module '" << modules[i] << "'"
                << std::endl;
    }

    // 2012-04-24 FM+XG : Now allow a chain module without modules :
    if (_modules_.size () > 0) {
      std::clog << "WARNING: " << "dpp::chain_module::initialize: "
                << "Chain module '" << get_name () << "' has no embedded modules !"
                << std::endl;
    }

    _set_initialized (true);

    return;
  }

  // Reset :
  void chain_module::reset ()
  {
    if (! is_initialized ()) {
      std::ostringstream message;
      message << "dpp::chain_module::initialize: "
              << "Module '" << get_name () << "' is not initialized !";
      throw std::logic_error (message.str ());
    }
    _modules_.clear ();
    _set_initialized (false);
    return;
  }

  // Processing :
  int chain_module::process (datatools::things & the_data_record)
  {
    if (! is_initialized ()) {
      std::ostringstream message;
      message << "dpp::chain_module::process: "
              << "Module '" << get_name () << "' is not initialized !";
      throw std::logic_error (message.str ());
    }

    this->reset_last_error_message ();

    // Loop on the chain of processing modules :
    if (is_debug ()) {
      std::ostringstream message;
      message << "dpp::chain_module::process: "
              << "number of chained modules=" << _modules_.size () << " ";
      std::clog << datatools::io::debug << message.str () << std::endl;
    }
    for (module_list_type::iterator i = _modules_.begin ();
         i != _modules_.end ();
         ++i) {
      module_entry & the_entry = *i;
      const std::string & module_name = the_entry.label;

      if (is_debug ())
        {
          std::clog << datatools::io::debug
                    << "dpp::chain_module::process: "
                    << "Processing chained module '" << module_name << "'..." << std::endl;
        }
      module_handle_type & the_handle  = the_entry.handle;
      if (! the_handle.has_data ())
        {
          std::ostringstream message;
          message << "dpp::chain_module::process: "
                  << "Handle has no module '" << module_name << "' !"
                  << std::endl;
          throw std::logic_error (message.str ());
        }
      base_module & a_module = the_handle.get ();
      a_module.reset_last_error_message ();
      try {
        int status = a_module.process (the_data_record);
        if (is_debug ())
          {
            std::ostringstream message;
            message << "dpp::chain_module::process: "
                    << "module='" << module_name << "' "
                    << "status=" << status;
            std::clog << datatools::io::debug << message.str () << std::endl;
          }
        if (status & FATAL || status & ERROR)
          {
            // Ask for the abortion of the full event record processing session
            // if the current chained module meets a fatal error  :
            if (a_module.has_last_error_message ())
              {
                this->append_last_error_message (a_module.get_last_error_message ());
              }
            a_module.reset_last_error_message ();
            if (status & FATAL )
              {
                return FATAL;
              }
          }
        if (status & STOP)
          {
            // Stop the chain loop if the current chained module asks for stop :
            return status;
          }
        // Just pass the status from the current chained module :
        //return status;
      }
      catch (std::exception & x) {
        std::ostringstream errmsg;
        errmsg  << "dpp::chain_module::process: "
                << "Module '" << module_name << "' failed to process event record; message is '"
                << x.what () << "'";
        append_last_error_message (errmsg.str ());
        std::clog << datatools::io::error
                  << "dpp::chain_module::process: "
                  << errmsg.str () << std::endl;
        return FATAL;
      }
    } // for loop on modules
    return SUCCESS;
  }

  void chain_module::tree_dump (std::ostream & a_out ,
                                const std::string & a_title,
                                const std::string & a_indent,
                                bool a_inherit) const
  {
    this->base_module::tree_dump (a_out, a_title, a_indent, true);

    a_out << a_indent << datatools::i_tree_dumpable::inherit_tag (a_inherit)
          << "Chained modules : " << std::endl;
    for (module_list_type::const_iterator i = _modules_.begin ();
         i != _modules_.end ();
         ++i) {
      const module_entry & the_entry   = *i;
      const std::string & module_label = the_entry.label;
      module_handle_type the_handle  = the_entry.handle;
      base_module & a_module         = the_handle.get ();
      std::ostringstream indent_oss;
      indent_oss << a_indent << datatools::i_tree_dumpable::inherit_skip_tag (a_inherit);
      a_out << a_indent << datatools::i_tree_dumpable::inherit_skip_tag (a_inherit);
      module_list_type::const_iterator j = i;
      j++;
      if (j == _modules_.end ()) {
        a_out << datatools::i_tree_dumpable::inherit_tag (a_inherit);
        indent_oss << datatools::i_tree_dumpable::inherit_skip_tag (a_inherit);
      } else {
        a_out << datatools::i_tree_dumpable::tag;
        indent_oss << datatools::i_tree_dumpable::skip_tag;
      }
      a_out << "Label : " << '"' << module_label << '"' << std::endl;
      a_module.tree_dump (a_out, "", indent_oss.str ());
    }
    return;
  }

  // Registration instantiation macro :
  DPP_MODULE_REGISTRATION_IMPLEMENT(chain_module, "dpp::chain_module")

}  // end of namespace dpp

// end of chain_module.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
