//! \file  datatools/ui/basic_shell.h
//! \brief A basic generic shell
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of datatools.
//
// datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with datatools. If not, see <http://www.gnu.org/licenses/>.

#ifndef DATATOOLS_UI_BASIC_SHELL_H
#define DATATOOLS_UI_BASIC_SHELL_H

// Standard libraries:
#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <exception>
#include <vector>

// Third Party:
// - Boost:
#include <boost/optional.hpp>

// This project:
#include <datatools/logger.h>
#include <datatools/command_utils.h>
#include <datatools/service_manager.h>
#include <datatools/properties.h>
#include <datatools/version_id.h>
#include <datatools/handle.h>
#include <datatools/multi_properties.h>
#include <datatools/i_tree_dump.h>
#include <datatools/ui/base_command.h>
#include <datatools/ui/base_command_interface.h>
#include <datatools/ui/ihs.h>

namespace datatools {

  namespace ui {

    class base_command;
    class base_command_interface;

    //! \brief Command line shell interface
    class basic_shell : public datatools::i_tree_dumpable
    {
    public:

      //! Retour the name of the system interface
      static const std::string & system_interface_name();

      //! Retour the default prompt
      static const std::string & default_prompt();

      //! Retour the default continuation prompt
      static const std::string & default_continuation_prompt();

      //! Default constructor
      basic_shell();

      //! Destructor
      virtual ~basic_shell();

      //! Return the logging priority threshold
      datatools::logger::priority get_logging() const;

      //! Set the logging priority threshold
      void set_logging(datatools::logger::priority);

      //! Check is the name is set
      bool has_name() const;

      //! Set the name:
      void set_name(const std::string &);

      //! Return the name
      const std::string & get_name() const;

      //! Check if version identifier is set
      bool has_version() const;

      //! Set the version
      void set_version(const ::datatools::version_id &);

      //! Return the version
      const ::datatools::version_id & get_version() const;

      //! Set the prompt
      //!
      //! Supported syntax:
      //! \a %n is replaced by the name of the shell
      //! \a %w is replaced by the current working interface path
      //! \a %W is replaced by the basename of the current working interface path
      //!
      //! Example:
      //! \code
      //! datatools::ui::basic_shell sh;
      //! sh.set_prompt("%n:%W> ");
      //! \endcode
      void set_prompt(const std::string &);

      //! Return the  prompt
      const std::string & get_prompt() const;

      //! Set the continuation prompt
      //! \see set_prompt
      void set_continuation_prompt(const std::string &);

      //! Return the continuation prompt
      const std::string & get_continuation_prompt() const;

      //! Return the current effective prompt
      std::string get_effective_prompt() const;

      //! Return the current effective prompt
      std::string get_effective_continuation_prompt() const;

      //! Set the using splash
      void set_using_splash(bool);

      //! Set the using readline
      void set_using_readline(bool);

      //! Set the using history
      void set_using_history(bool);

      //! Set the history filename
      void set_history_filename(const std::string &);

      //! Set the history truncate
      void set_history_truncate(unsigned int);

      //! Set the flag to add only valid command line in history
      void set_history_add_only_on_success(bool);

      //! Set the default path
      void set_default_path(const std::string &);

      //! Return the default path
      const std::string & get_default_path() const;

      //! Set the current path
      void set_current_path(const std::string & p_);

      //! Return the current path
      const std::string & get_current_path() const;

      //! Check if services are defined
      bool has_services() const;

      //! Set the handle to an external service manager
      void set_services(::datatools::service_manager & services_);

      //! Return a mutable reference to the service manager
      ::datatools::service_manager & grab_services();

      //! Return a non mutable reference to the service manager
      const ::datatools::service_manager & get_services() const;

      //! Reset the handle
      void reset_services();

      //! Check if IHS is set
      bool has_ihs() const;

      //! Set the handle to an external IHS
      void set_ihs(ihs & ihs_);

      //! Return a mutable reference to the IHS
      ihs & grab_ihs();

      //! Return a non mutable reference to the IHS
      const ihs & get_ihs() const;

      // //! Mount an external command interface with explicit name
      // void mount_interface(const std::string & interface_name_,
      //                      base_command_interface & external_interface_);

      // //! Mount an external command interface with default name
      // void mount_interface(base_command_interface & external_interface_);

      // //! Load a command interface
      // void load_interface(const std::string & interface_name_,
      //                     const std::string & interface_type_id_,
      //                     const datatools::properties & interface_config_);

      // //! Load interfaces
      // void load_interfaces(const datatools::multi_properties & interfaces_config_);

      // //! Check if interface with given name exists
      // bool has_interface(const std::string & interface_name_);

      // //! Build the list of interface names
      // void build_interface_names(std::vector<std::string> & names_) const;

      //! Check initialization
      bool is_initialized() const;

      //! Initialization
      void initialize_simple();

      //! Initialization
      void initialize(const datatools::properties &);

      //! Reset
      void reset();

      //! Main run loop
      void run();

      //! Build a upper and spaced title
      static void make_title_upper(const std::string & title_,
                                   std::string & upper_);

      //! Build the canonical full path associated to a given relative or absolute path
      std::string canonical_path(const std::string & path_) const;

      //! Check if stop is requested
      bool is_stop_requested() const;

      //! Set the stop requested flag
      void set_stop_requested();

      //! Build list of interfaces
      void build_current_list_of_interfaces(std::vector<std::string> & names_,
                                            uint32_t flags_ = 0,
                                            std::size_t depth_ = 0) const;

      //! Check if system interface is set
      bool has_system_interface() const;

      // Shell command interface has special priviledeges:
      friend class shell_command_interface;

      //! Smart print
      virtual void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_  = "",
                             const std::string & indent_ = "",
                             bool inherit = false) const;

    protected:

      //! Set default values to attributes
      void _set_defaults();

      //! Update the effective prompt
      void _expand_prompt(const std::string &, std::string &) const;

      //! Print the splash screen
      void _print_splash(std::ostream & out_ = std::cout);

      //! Print online help
      void _print_help(std::ostream & out_ = std::cout);

      //! At initialization
      void _at_init(const datatools::properties & config_);

      //! At reset
      void _at_reset();

      //! At run start
      void _at_run_start();

      //! At run loop
      void _run_core();

      //! Run command
      datatools::command::returned_info _run_command(const std::string & command_line_);

      //! At run stop
      void _at_run_stop();

    private:

      // Control/management:
      bool _initialized_; //!< Initialization flag
      datatools::logger::priority _logging_; //!< Logging priority threshold

      // Configuration:
      std::string _name_;    //!< Name of the shell
      boost::optional<version_id> _version_; //!< Version identifier of the shell (optional)
      std::string _prompt_;  //!< Prompt string
      std::string _continuation_prompt_; //!< Continuation prompt string
      std::string _default_path_;        //!< Default path
      bool        _using_splash_;        //!< Flag to print splash at start
      bool        _using_readline_;      //!< Flag to use the readline library
      bool        _using_history_;       //!< Flag to use command history
      bool        _history_add_only_on_success_; //!< Flag to use command history only for valid command
      std::string _history_filename_;  //!< History filename
      unsigned int _history_truncate_; //!< History truncation size
      datatools::service_manager * _services_; //!< handle to the service manager

      // Working data:
      ihs * _ihs_; //!< Handle to the IHS
      std::string _current_working_path_; //!< Current working interface path
      boost::shared_ptr<base_command_interface> _system_interface_; //!< System interface for this shell

      // Private data:
      struct pimpl_type;
      boost::scoped_ptr<pimpl_type> _pimpl_;

      //! Create and return a mutable reference to the private internal data
      pimpl_type & _grab_pimpl();

      //! Create and return a non mutable reference to the private internal data
      const pimpl_type & _get_pimpl() const;

    };

  } // namespace ui

} // namespace datatools

#endif // DATATOOLS_UI_BASIC_SHELL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
