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
#include <memory>

// Third Party:
// - Boost:
#include <boost/optional.hpp>
#include <boost/utility.hpp>

// This project:
#include <datatools/logger.h>
#include <datatools/command_utils.h>
#include <datatools/service_manager.h>
#include <datatools/properties.h>
#include <datatools/version_id.h>
#include <datatools/handle.h>
#include <datatools/multi_properties.h>
#include <datatools/i_tree_dump.h>
#include <datatools/bit_mask.h>
#include <datatools/ui/ihs.h>
#include <datatools/ui/target_command_interface.h>

namespace datatools {

  namespace ui {

    class shell_command_interface;

    //! \brief Command line shell interface
    class basic_shell
      : public datatools::i_tree_dumpable
      , private boost::noncopyable
    {
    public:

      typedef target_command_interface<basic_shell> shell_command_interface_type;

      //! Special flags for run core
      enum run_core_flags {
        RC_NONE             = 0, //!< Normal run
        RC_INHIBIT_HISTORY  = datatools::bit_mask::bit00, //!< Inhibit the history
        RC_INHIBIT_READLINE = datatools::bit_mask::bit01, //!< Inhibit readline (only interactive session)
        RC_EXIT_ON_ERROR    = datatools::bit_mask::bit02  //!< Force the shell to exit when an error is met
      };

      static const unsigned int DEFAULT_HISTORY_TRUNCATE = 200;

      //! Retour the name of the builtin system interface
      static const std::string & system_interface_name();

      //! Retour the default prompt
      static const std::string & default_prompt();

      //! Retour the default continuation prompt
      static const std::string & default_continuation_prompt();

      //! Default constructor
      basic_shell(const std::string & name_ = "");

      //! Destructor
      ~basic_shell() override;

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

      //! Check is the title is set
      bool has_title() const;

      //! Set the title:
      void set_title(const std::string &);

      //! Return the title
      const std::string & get_title() const;

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

      //! Return the prompt
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

      //! Check if the user identifier is set
      bool has_user() const;

      //! Set the user identifier
      void set_user(const std::string &);

      //! Return the user identifier
      const std::string & get_user() const;

      //! Check if the host identifier is set
      bool has_host() const;

      //! Set the host identifier
      void set_host(const std::string &);

      //! Return the host identifier
      const std::string & get_host() const;

      //! Set the abort-on-error flag
      void set_exit_on_error(bool);

      //! Return the abort-on-error flag
      bool is_exit_on_error() const;

      //! Set the using splash
      void set_using_splash(bool);

      //! Return the using splash flag
      bool is_using_splash() const;

      //! Set the using readline
      void set_using_readline(bool);

      //! Return the using readline flag
      bool is_using_readline() const;

      //! Set the using history
      void set_using_history(bool);

      //! Return the using history flag
      bool is_using_history() const;

      //! Set the history filename
      void set_history_filename(const std::string &);

      //! Return the history filename
      const std::string & get_history_filename() const;

      //! Set the history truncate
      void set_history_truncate(unsigned int);

      //! Return the history truncate
      unsigned int get_history_truncate();

      //! Set the flag to add only valid command line in history
      void set_history_add_only_on_success(bool);

      //! Return the flag to add only valid command line in history
      bool is_history_add_only_on_success() const;

      //! Check if the default path is set
      bool has_default_path() const;

      //! Set the default path
      void set_default_path(const std::string &);

      //! Return the default path
      const std::string & get_default_path() const;

      //! Check if the current path is set
      bool has_current_path() const;

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

      //! Build the list of builtin commands
      void builtin_command_names(std::vector<std::string> & cmd_names_) const;

      //! Check if a builtin command is available
      bool has_builtin_command(const std::string & cmd_name_) const;

      //! Return a const reference to a builtin command
      const base_command & get_builtin_command(const std::string & cmd_name_) const;

      //! Check initialization
      bool is_initialized() const;

      //! Initialization
      void initialize_simple();

      //! Initialization
      void initialize(const datatools::properties &);

      //! Reset
      void reset();

      //! Main run loop
      int run(std::istream * in_ = nullptr);

      //! Build a upper and spaced title
      static void make_title_upper(const std::string & title_,
                                   std::string & upper_);

      //! Build the canonical full path associated to a given relative or absolute path
      std::string canonical_path(const std::string & path_) const;

      //! Check if a path is an absolute existing path
      bool is_absolute_path(const std::string & path_) const;

      //! Check if stop is requested
      bool is_stop_requested() const;

      //! Set the stop requested flag
      void set_stop_requested();

      //! Load a macro file
      void load_macro(const std::string & macro_, uint32_t flags_ = 0);

      //! Build list of interfaces
      void build_current_list_of_interfaces(std::vector<std::string> & names_,
                                            uint32_t flags_ = 0,
                                            std::size_t depth_ = 0) const;

      //! Check if system interface is set
      bool has_system_interface() const;

      //! Set an external system interface
      void set_system_interface(shell_command_interface_type &);

      //! Smart print
      //!
      //! Supported options:
      //! \code
      //! {
      //!   "title"    : "My title: ",
      //!   "indent"   : "[debug] ",
      //!   "inherit"  : true,
      //!   "ihs"      : false
      //! }
      //! \endcode
      void print_tree(std::ostream & out_ = std::clog,
                              const boost::property_tree::ptree & options_ = datatools::i_tree_dumpable::empty_options()) const override;

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
      virtual void _at_init(const datatools::properties & config_);

      //! At reset
      virtual void _at_reset();

      //! At run start
      virtual void _at_run_start();

      //! At run loop
      datatools::command::returned_info _run_core(std::istream * in_ = nullptr,
                                                  uint32_t flags_ = RC_NONE);

      //! Run command
      datatools::command::returned_info _run_command(const std::string & command_line_);

      //! At run stop
      virtual void _at_run_stop();

      //! Return the system interface
      shell_command_interface_type & _grab_system_interface();

      //! Return the system interface
      const shell_command_interface_type & _get_system_interface() const;

      //! Compute continuation condition
      //! @return false if the shell should stop at the end of a loop
      virtual bool _compute_continue_condition();

    private:

      // Control/management:
      bool _initialized_;                    //!< Initialization flag
      datatools::logger::priority _logging_; //!< Logging priority threshold

      // Configuration:
      std::string _name_;                    //!< Name of the shell (%n)
      std::string _title_;                   //!< Title of the shell
      boost::optional<version_id> _version_; //!< Version identifier of the shell (optional)
      std::string _prompt_;                  //!< Prompt string (PS1)
      std::string _continuation_prompt_;     //!< Continuation prompt string (PS2)
      std::string _default_path_;            //!< Default path
      std::string _user_;                    //!< User identifier (%u)
      std::string _host_;                    //!< Host identifier (%H)
      bool        _exit_on_error_;           //!< Flag to exit/abort the shell on first error
      bool        _using_splash_;            //!< Flag to print splash at start
      bool        _using_readline_;          //!< Flag to use the readline library
      bool        _using_history_;           //!< Flag to use command history
      bool        _history_add_only_on_success_; //!< Flag to use command history only for valid command
      std::string _history_filename_;            //!< History filename
      unsigned int _history_truncate_;           //!< History truncation size
      datatools::service_manager * _services_;   //!< handle to the service manager

      // Working data:
      ihs * _ihs_; //!< Handle to the IHS
      std::string _current_working_path_; //!< Current working interface path (%w, %W)
      std::unique_ptr<shell_command_interface_type> _system_interface_; //!< Embedded system interface for this shell
      shell_command_interface_type * _external_system_interface_; //! External system interface for this shell

      // Private data:
      struct pimpl_type;
      std::unique_ptr<pimpl_type> _pimpl_;

      //! Create and return a mutable reference to the private internal data
      pimpl_type & _grab_pimpl();

      //! Create and return a non mutable reference to the private internal data
      const pimpl_type & _get_pimpl() const;

      // Shell command interface has special priviledeges:
      // friend class shell_command_interface;

    };

  } // namespace ui

} // namespace datatools

#endif // DATATOOLS_UI_BASIC_SHELL_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
