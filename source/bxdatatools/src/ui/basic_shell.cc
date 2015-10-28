// datatools/ui/basic_shell.cc - A generic basic shell
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

// Ourselves
#include <datatools/ui/basic_shell.h>

// Standard libraries:
#include <fstream>

// Third Party:
// - Boost:
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
// - Readline:
#include <readline/readline.h>
#include <readline/history.h>

// This project:
#include <datatools/exception.h>
#include <datatools/logger.h>
#include <datatools/utils.h>
#include <datatools/multi_properties.h>
#include <datatools/ui/base_command.h>
#include <datatools/ui/base_command_interface.h>
#include <datatools/ui/shell_command_interface.h>
#include <datatools/ui/ihs.h>
#include <datatools/ui/utils.h>

namespace datatools {

  namespace ui {

    //! \brief Dynamic informations about the current parsing source
    struct parser_context
    {
      //! Default constructor
      parser_context();

      std::string macro_name;   //!< The name of the current macro (empty in parent shell)
      std::size_t line_counter; //!< The line counter in the current parser context
    };

    parser_context::parser_context()
    {
      line_counter = 0;
      return;
    }

    //! \brief Private implementation working data
    struct basic_shell::pimpl_type
    {
      //! Default constructor
      pimpl_type();

      //! Reset
      void reset();

      bool        stop_requested;            //!< Terminate request
      std::string expanded_history_filename; //!< Expanded history filename
      std::list<parser_context> pcontexts;   //!< The list of current nested parser context
    };

    basic_shell::pimpl_type::pimpl_type()
    {
      stop_requested = false;
      return;
    }

    void basic_shell::pimpl_type::reset()
    {
      expanded_history_filename = "";
      stop_requested = false;
      return;
    }

    basic_shell::pimpl_type & basic_shell::_grab_pimpl()
    {
      if (_pimpl_ == 0) {
        _pimpl_.reset(new pimpl_type);
      }
      return *_pimpl_;
    }

    const basic_shell::pimpl_type & basic_shell::_get_pimpl() const
    {
      basic_shell * mutable_this = const_cast<basic_shell *>(this);
      return const_cast<basic_shell::pimpl_type &>(mutable_this->_grab_pimpl());
    }

    // **********************************************
    // *                                            *
    // *         B A S I C     S H E L L            *
    // *                                            *
    // **********************************************

    // static
    const std::string & basic_shell::default_prompt()
    {
      static const std::string _p("%n:%W> ");
      return _p;
    }

    // static
    const std::string & basic_shell::default_continuation_prompt()
    {
      static const std::string _p("> ");
      return _p;
    }

    // static
    const std::string & basic_shell::system_interface_name()
    {
      static const std::string _name("builtins");
      return _name;
    }

    void basic_shell::_set_defaults()
    {
      DT_THROW_IF (is_initialized(), std::logic_error, "Shell is already initialized!");
      _name_.clear();
      _version_.reset();
      _prompt_  = default_prompt();
      _continuation_prompt_ = default_continuation_prompt();
      _default_path_   = ::datatools::ui::path::root_path();
      _exit_on_error_ = false;
      _using_splash_   = true;
      _using_readline_ = true;
      _using_history_  = true;
      _history_add_only_on_success_ = true;
      _history_filename_.clear();
      _history_truncate_ = 0;

      _ihs_ = 0;
      _current_working_path_ = ::datatools::ui::path::root_path();
      return;
    }

    void basic_shell::set_current_path(const std::string & p_)
    {
      DT_THROW_IF(! has_ihs(), std::logic_error, "Cannot change current path for no IHS is set!");
      DT_THROW_IF(! _ihs_->is_interface(p_), std::logic_error, "Path '" << p_ << "' is not an interface path!");
      _current_working_path_ = p_;
      return;
    }

    const std::string & basic_shell::get_current_path() const
    {
      return _current_working_path_;
    }

    basic_shell::basic_shell(const std::string & name_)
    {
      _initialized_ = false;
      _logging_ = datatools::logger::PRIO_FATAL;
      _ihs_ = 0;
      _set_defaults();
      if (! name_.empty()) {
        set_name(name_);
      }
      return;
    }

    basic_shell::~basic_shell()
    {
      if (is_initialized()) {
        reset();
      }
      return;
    }

    datatools::logger::priority basic_shell::get_logging() const
    {
      return _logging_;
    }

    void basic_shell::set_logging(datatools::logger::priority p_)
    {
      _logging_ = p_;
      return;
    }

    void basic_shell::set_name(const std::string & name_)
    {
      DT_THROW_IF (is_initialized(), std::logic_error, "Shell is already initialized!");
      DT_THROW_IF(!datatools::ui::path::is_valid_name(name_),
                  std::logic_error,
                  "Invalid shell name '" << name_ << "'!");
      _name_ = name_;
      return;
    }

    bool basic_shell::has_name() const
    {
      return !_name_.empty();
    }

    const std::string & basic_shell::get_name() const
    {
      return _name_;
    }

    bool basic_shell::has_version() const
    {
      return _version_;
    }

    void basic_shell::set_version(const ::datatools::version_id & version_)
    {
      DT_THROW_IF (is_initialized(), std::logic_error, "Shell is already initialized!");
      _version_ = version_;
      return;
    }

    const ::datatools::version_id & basic_shell::get_version() const
    {
      return _version_.get();
    }

    void basic_shell::set_prompt(const std::string & prompt_)
    {
      DT_THROW_IF (is_initialized(), std::logic_error, "Shell is already initialized!");
      _prompt_ = prompt_;
      if (_prompt_.empty()) {
        _prompt_ = default_prompt();
      } else if (_prompt_[_prompt_.length() - 1] != ' ') {
        _prompt_ += " ";
      }
      return;
    }

    const std::string & basic_shell::get_prompt() const
    {
      return _prompt_;
    }

    void basic_shell::set_continuation_prompt(const std::string & p_)
    {
      DT_THROW_IF (is_initialized(), std::logic_error, "Shell is already initialized!");
      _continuation_prompt_ = p_;
      if (_continuation_prompt_.empty()) {
        _continuation_prompt_ = default_prompt();
      } else if (_continuation_prompt_[_continuation_prompt_.length() - 1] != ' ') {
        _continuation_prompt_ += " ";
      }

      return;
    }

    const std::string & basic_shell::get_continuation_prompt() const
    {
      return _continuation_prompt_;
    }

    bool basic_shell::has_default_path() const
    {
      return !_default_path_.empty();
    }

    void basic_shell::set_default_path(const std::string & default_path_)
    {
      DT_THROW_IF (is_initialized(), std::logic_error, "Shell is already initialized!");
      if (_ihs_ == 0) {
        DT_LOG_WARNING(datatools::logger::PRIO_ALWAYS,
                       "No IHS is defined! Default path maybe not valid!");
        _default_path_ = default_path_;
      } else {
        if (!_ihs_->is_interface(default_path_)) {
          DT_LOG_ERROR(datatools::logger::PRIO_ALWAYS,
                       "IHS has no interface with path '" << default_path_ << "'! Fall back to root path!");
          _default_path_ = ::datatools::ui::path::root_path();
        } else {
          _default_path_ = default_path_;
        }
      }
      return;
    }

    const std::string & basic_shell::get_default_path() const
    {
      return _default_path_;
    }

    std::string basic_shell::get_effective_prompt() const
    {
      std::string expanded;
      _expand_prompt(_prompt_, expanded);
      return expanded;
    }

    std::string basic_shell::get_effective_continuation_prompt() const
    {
      std::string expanded;
      _expand_prompt(_continuation_prompt_, expanded);
      return expanded;
    }

    void basic_shell::_expand_prompt(const std::string & prompt_rule_, std::string & expanded_prompt_) const
    {
      // std::cerr << "DEVEL: basic_shell::_expand_prompt: prompt_rule_ = '" << prompt_rule_ << "'" << std::endl;
      std::string tmp_prompt = boost::algorithm::replace_all_copy(prompt_rule_, "%n", get_name());
      tmp_prompt = boost::algorithm::replace_all_copy(tmp_prompt, "%w", _current_working_path_);
      std::string cwd_basename = datatools::ui::path::basename(_current_working_path_);
      // std::cerr << "DEVEL: basic_shell::_expand_prompt: cwd_basename = '" << cwd_basename << "'" << std::endl;
      tmp_prompt = boost::algorithm::replace_all_copy(tmp_prompt, "%W", cwd_basename);
      // std::cerr << "DEVEL: basic_shell::_expand_prompt: tmp_prompt = '" << tmp_prompt << "'" << std::endl;
      expanded_prompt_ = tmp_prompt;
      return;
    }

    void basic_shell::set_exit_on_error(bool aoe_)
    {
      DT_THROW_IF (is_initialized(), std::logic_error, "Shell is already initialized!");
      _exit_on_error_ = aoe_;
      return;
    }

    bool basic_shell::is_exit_on_error() const
    {
      return _exit_on_error_;
    }

    void basic_shell::set_using_splash(bool s_)
    {
      DT_THROW_IF (is_initialized(), std::logic_error, "Shell is already initialized!");
      _using_splash_ = s_;
      return;
    }

    bool basic_shell::is_using_splash() const
    {
      return _using_splash_;
    }

    void basic_shell::set_using_readline(bool s_)
    {
      DT_THROW_IF (is_initialized(), std::logic_error, "Shell is already initialized!");
      _using_readline_ = s_;
      return;
    }

    bool basic_shell::is_using_readline() const
    {
      return _using_readline_;
    }

    void basic_shell::set_using_history(bool s_)
    {
      DT_THROW_IF (is_initialized(), std::logic_error, "Shell is already initialized!");
      _using_history_ = s_;
      return;
    }

    bool basic_shell::is_using_history() const
    {
      return _using_history_;
    }

    void basic_shell::set_history_filename(const std::string & history_filename_)
    {
      DT_THROW_IF (is_initialized(), std::logic_error, "Shell is already initialized!");
      _history_filename_ = history_filename_;
      return;
    }

    const std::string & basic_shell::get_history_filename() const
    {
      return _history_filename_;
    }

    void basic_shell::set_history_truncate(unsigned int t_)
    {
      DT_THROW_IF (is_initialized(), std::logic_error, "Shell is already initialized!");
      _history_truncate_ = t_;
      return;
    }

    unsigned int basic_shell::get_history_truncate()
    {
      return _history_truncate_;
    }

    void basic_shell::set_history_add_only_on_success(bool f_)
    {
      DT_THROW_IF (is_initialized(), std::logic_error, "Shell is already initialized!");
      _history_add_only_on_success_ = f_;
      return;
    }

    bool basic_shell::is_history_add_only_on_success() const
    {
      return _history_add_only_on_success_;
    }

    bool basic_shell::has_services() const
    {
      return _services_ != 0;
    }

    void basic_shell::set_services(datatools::service_manager & services_)
    {
      DT_THROW_IF (is_initialized(), std::logic_error, "Shell is already initialized!");
      _services_ = &services_;
      return;
    }

    void basic_shell::reset_services()
    {
      _services_ = 0;
      return;
    }

    ::datatools::service_manager & basic_shell::grab_services()
    {
      DT_THROW_IF(!has_services(), std::logic_error, "No service manager is set!");
      return *_services_;
    }

    const ::datatools::service_manager & basic_shell::get_services() const
    {
      DT_THROW_IF(!has_services(), std::logic_error, "No service manager is set!");
      return *_services_;
    }

    bool basic_shell::has_system_interface() const
    {
      return _system_interface_.get() != 0;
    }

    bool basic_shell::has_ihs() const
    {
      return _ihs_ != 0;
    }

    void basic_shell::set_ihs(ihs & ihs_)
    {
      DT_THROW_IF (is_initialized(), std::logic_error, "Shell is already initialized!");
      _ihs_ = &ihs_;
      if (!_ihs_->is_interface(_default_path_)) {
        DT_LOG_ERROR(datatools::logger::PRIO_ALWAYS,
                     "IHS has no interface with path '" << _default_path_ << "'! Fall back to root path!");
        _default_path_ = ::datatools::ui::path::root_path();
      }
      return;
    }

    ihs & basic_shell::grab_ihs()
    {
      DT_THROW_IF(!has_ihs(), std::logic_error, "No IHS is set!");
      return *_ihs_;
    }

    const ihs & basic_shell::get_ihs() const
    {
      DT_THROW_IF(!has_ihs(), std::logic_error, "No IHS is set!");
      return *_ihs_;
    }

    void basic_shell::builtin_command_names(std::vector<std::string> & cmd_names_) const
    {
      DT_THROW_IF(!has_system_interface(), std::logic_error, "Shell '" << get_name() << "' has no system interface!");
      const shell_command_interface & sci = *_system_interface_.get();
      sci.build_command_names(cmd_names_);
      return;
    }

    bool basic_shell::has_builtin_command(const std::string & cmd_name_) const
    {
      DT_THROW_IF(!has_system_interface(), std::logic_error, "Shell '" << get_name() << "' has no system interface!");
      const shell_command_interface & sci = *_system_interface_.get();
      return sci.has_command(cmd_name_);
    }

    const base_command & basic_shell::get_builtin_command(const std::string & cmd_name_) const
    {
      DT_THROW_IF(!has_system_interface(), std::logic_error, "Shell '" << get_name() << "' has no system interface!");
      const shell_command_interface & sci = *_system_interface_.get();
      return sci.get_command(cmd_name_);
    }

    bool basic_shell::is_initialized() const
    {
      return _initialized_;
    }

    void basic_shell::initialize_simple()
    {
      datatools::properties dummy_config;
      initialize(dummy_config);
      return;
    }

    void basic_shell::initialize(const datatools::properties & config_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error, "Shell is already initialized!");

      if (_services_ == 0) {
        DT_LOG_WARNING(_logging_, "Shell has no access to any services!");
      }

      // Fetch configuration parameters:

      datatools::logger::priority p
        = datatools::logger::extract_logging_configuration(config_,
                                                           _logging_,
                                                           true);

      if (p != datatools::logger::PRIO_UNDEFINED) {
        set_logging(p);
      }

      // Shell name is mandatory:
      if (!has_name()) {
        if (config_.has_key("name")) {
          const std::string & name = config_.fetch_string("name");
          set_name(name);
        }
      }
      DT_THROW_IF(!has_name(), std::logic_error, "Shell has no name!");

      // Prompt:
      if (_prompt_.empty()) {
        if (config_.has_key("prompt")) {
          const std::string & prompt = config_.fetch_string("prompt");
          set_prompt(prompt);
        }
      }
      if (_prompt_.empty()) {
        _prompt_ = default_prompt();
      }

      // Continuation prompt:
      if (_continuation_prompt_.empty()) {
        if (config_.has_key("continuation_prompt")) {
          const std::string & continuation_prompt = config_.fetch_string("continuation_prompt");
          set_continuation_prompt(continuation_prompt);
        }
      }
      if (_continuation_prompt_.empty()) {
        _continuation_prompt_ = default_continuation_prompt();
      }

      if (config_.has_key("default_path")) {
        const std::string & defpath = config_.fetch_string("default_path");
        set_default_path(defpath);
      }

      if (config_.has_key("exit_on_error")) {
        set_exit_on_error(config_.fetch_boolean("exit_on_error"));
      }

      if (config_.has_key("using_readline")) {
        set_using_readline(config_.fetch_boolean("using_readline"));
      }

      if (config_.has_key("using_splash")) {
        set_using_splash(config_.fetch_boolean("using_splash"));
      }

      if (config_.has_key("using_history")) {
        set_using_history(config_.fetch_boolean("using_history"));
      }

      if (_using_history_) {

        if (_history_filename_.empty()) {
          if (config_.has_key("history_filename")) {
            const std::string & hf = config_.fetch_string("history_filename");
            set_history_filename(hf);
          }
        }

        if (config_.has_key("history_add_only_on_success")) {
          set_history_add_only_on_success(config_.fetch_boolean("history_add_only_on_success"));
        }

        if (_history_filename_.empty()) {
          std::ostringstream hist_oss;
          hist_oss << "~/." << _name_ << ".history";
          _history_filename_ = hist_oss.str();
        }

        if (_history_truncate_ == 0) {
          if (config_.has_key("history_truncate")) {
            int hft = config_.fetch_integer("history_truncate");
            DT_THROW_IF(hft < 0, std::domain_error, "Invalid history truncation size!");
            set_history_truncate((unsigned int) hft);
          }
        }

        if (_history_truncate_ == 0) {
          _history_truncate_ = 200;
        }

      }

      // Specific initialization:
      _at_init(config_);

      // Set the current path:
      set_current_path(get_default_path());

      _initialized_ = true;
      return;
    }

    bool basic_shell::is_stop_requested() const
    {
      return _get_pimpl().stop_requested;
    }

    void basic_shell::set_stop_requested()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Shell is not initialized!");
      _grab_pimpl().stop_requested = true;
      return;
    }

    void basic_shell::_at_init(const datatools::properties & config_)
    {
      _system_interface_.reset(new shell_command_interface);
      _system_interface_->set_target(*this);
      _system_interface_->set_name(system_interface_name());
      _system_interface_->set_terse_description(std::string("System builtin commands for the '") + get_name() + "' shell");

      datatools::properties sci_config;
      config_.export_and_rename_starting_with(sci_config, "system_interface.", "");
      if (has_services()) {
        _system_interface_->initialize(sci_config, get_services());
      } else {
        _system_interface_->initialize_standalone(sci_config);
      }
      _system_interface_->tree_dump(std::clog, "System builtin command interface: ");
      return;
    }

    void basic_shell::_at_reset()
    {
      if (_system_interface_ != 0 && _system_interface_->is_initialized()) {
        _system_interface_->reset();
      }
      _system_interface_.reset();
      return;
    }

    void basic_shell::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error, "Shell is not initialized!");
      _initialized_ = false;

      _at_reset();

      reset_services();
      _set_defaults();

      return;
    }

    std::string basic_shell::canonical_path(const std::string & path_) const
    {
      std::string full_path;

      if (path_.empty()) {

        // "" -> "/path/to/default/interface"
        full_path = _default_path_;

      } else if (path_ == datatools::ui::path::root_path()) {
        // "/" -> ""
        full_path = path_;

      } else if (path_ == datatools::ui::path::home_interface_path()
                 || path_ == (datatools::ui::path::home_interface_path() + datatools::ui::path::sep())) {

        // "~" | "~/" -> "/path/to/default/interface"
        full_path = _default_path_;

      } else if (boost::starts_with(path_,
                                    datatools::ui::path::home_interface_path() + datatools::ui::path::sep())) {

        // "~/foo/bar"  -> "/path/to/default/interface/foo/bar"
        // "~/foo/bar/" -> "/path/to/default/interface/foo/bar/"
        full_path = boost::algorithm::replace_first_copy(path_,
                                                         datatools::ui::path::home_interface_path(),
                                                         _default_path_);


      } else if (path_ == datatools::ui::path::current_interface_path()
                 || path_ == (datatools::ui::path::current_interface_path() + datatools::ui::path::sep())) {

        // "." | "./" -> "/path/to/cwi"
        full_path = _current_working_path_;

      } else if (boost::starts_with(path_,
                                    datatools::ui::path::current_interface_path() + datatools::ui::path::sep())) {

        // "./foo/bar"  -> "/path/to/cwi/foo/bar"
        // "./foo/bar/" -> "/path/to/cwi/foo/bar/"
        full_path = boost::algorithm::replace_first_copy(path_,
                                                         datatools::ui::path::current_interface_path(),
                                                         _current_working_path_);

      } else if (path_ == datatools::ui::path::current_parent_interface_path()
                 || path_ == (datatools::ui::path::current_parent_interface_path() + datatools::ui::path::sep())) {

        // std::cerr << "DEVEL: canonical_path: path_      = '" << path_ << "'" << std::endl;

        // ".." | "../" -> "/path/to"
        full_path = datatools::ui::path::parent_path(_current_working_path_);
        // std::cerr << "DEVEL: canonical_path:  full_path = '" << full_path << "'" << std::endl;

      } else if (boost::starts_with(path_,
                                    datatools::ui::path::current_parent_interface_path() + datatools::ui::path::sep())) {

        // "../foo/bar"  -> "/path/to/foo/bar"
        // "../foo/bar/" -> "/path/to/foo/bar/"
        full_path = boost::algorithm::replace_first_copy(path_,
                                                         datatools::ui::path::current_parent_interface_path(),
                                                         datatools::ui::path::parent_path(_current_working_path_));

      } else if (datatools::ui::path::is_absolute_path(path_)) {

        // "/foo/bar" -> "/foo/bar"
        full_path = path_;

      } else {

        if (_current_working_path_ != datatools::ui::path::root_path()) {

          // "foo/bar" -> "/path/to/cwi/foo/bar"
          full_path = _current_working_path_ + "/" + path_;

        } else {

          // "foo/bar" -> "/foo/bar"
          full_path = _current_working_path_ + path_;

        }
      }

      // std::cerr << "DEVEL: canonical_path: pre full_path      = '" << full_path << "'" << std::endl;
      if (full_path != datatools::ui::path::root_path()) {
        std::vector<std::string> tokens;
        boost::algorithm::split(tokens, full_path, boost::is_any_of("/"));
        std::list<std::string> tokens2;
        for (int itok = 0; itok < (int) tokens.size(); itok++) {
          if (tokens[itok] == datatools::ui::path::current_interface_path()) {
            // ignore token:
          } else if (tokens[itok] == datatools::ui::path::current_parent_interface_path()) {
            if (tokens2.size() > 0) {
              tokens2.pop_back();
            } else {
              // ignore token:
            }
          } else {
            tokens2.push_back(tokens[itok]);
          }
        }
        std::string expanded_path;
        for (std::list<std::string>::const_iterator itok = tokens2.begin();
             itok != tokens2.end();
             itok++) {
          if (!itok->empty()) {
            expanded_path += '/';
            expanded_path += *itok;
          }
        }
        full_path = expanded_path;
        if (full_path.empty()) {
          full_path = datatools::ui::path::root_path();
        }
      }
      full_path = datatools::ui::path::remove_trailing_sep(full_path);
      // std::cerr << "DEVEL: canonical_path: final full_path    = '" << full_path << "'" << std::endl;
      return full_path;
    }

    // static
    void basic_shell::make_title_upper(const std::string & title_, std::string & upper_)
    {
      std::ostringstream oss;
      for (int i = 0; i < title_.length(); i++) {
        oss << (char) toupper(title_[i]);
        if (i != title_.length() - 1) {
          oss << ' ';
        }
      }
      upper_ = oss.str();
      return;
    }

    void basic_shell::_print_splash(std::ostream & out_)
    {
      out_ << "\n";
      std::string upper;
      make_title_upper(_name_, upper);
      out_ << "\t" << upper << "\n";
      if (has_version()) {
        out_ << "\tVersion " << get_version() << "\n";
      }
      out_ << "\n";
      return;
    }

    void basic_shell::_print_help(std::ostream & out_)
    {
      out_ << "\n";
      out_ << "Sorry, no immediate help is available.\n";
      out_ << "\n";
      return;
    }

    int basic_shell::run()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      DT_THROW_IF(!is_initialized(), std::logic_error, "Shell is not initialized!");
      _at_run_start();
      uint32_t rc_flags = RC_NONE;
      if (is_exit_on_error()) {
        rc_flags |= RC_EXIT_ON_ERROR;
      }
      if (!is_using_readline()) {
        rc_flags |= RC_INHIBIT_READLINE;
      }
      if (!is_using_history()) {
        rc_flags |= RC_INHIBIT_HISTORY;
      }
      {
        parser_context top_context;
        _grab_pimpl().pcontexts.push_back(top_context);
      }
      datatools::command::returned_info cri = _run_core(0, rc_flags);
      {
        _grab_pimpl().pcontexts.pop_back();
      }

      _at_run_stop();
      DT_LOG_TRACE_EXITING(_logging_);
      return cri.get_error_code();
    }

    void basic_shell::_at_run_start()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      DT_LOG_TRACE(_logging_, "Starting run...");
      if (_using_splash_) {
        _print_splash(std::cout);
      }

      if (_using_history_) {
        _grab_pimpl().expanded_history_filename = _history_filename_;
        datatools::fetch_path_with_env(_grab_pimpl().expanded_history_filename);
        using_history();
        if (boost::filesystem::exists(_grab_pimpl().expanded_history_filename)) {
          int error = read_history(_grab_pimpl().expanded_history_filename.c_str());
          if (error) {
            _grab_pimpl().expanded_history_filename.clear();
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                         "Shell " << _name_ << ": "
                         << "Cannot read history file '"
                         << _grab_pimpl().expanded_history_filename << "'!");
          }
        }

      }

      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    void basic_shell::_at_run_stop()
    {
      DT_LOG_TRACE_ENTERING(_logging_);
      DT_LOG_TRACE(_logging_, "Stopping run...");
      if (_using_history_) {
        if (! _grab_pimpl().expanded_history_filename.empty()) {
          int error = write_history(_grab_pimpl().expanded_history_filename.c_str());
          if (error) {
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                         "Shell " << _name_ << ": "
                         << "Cannot write history file '"
                         << _grab_pimpl().expanded_history_filename << "' !");
          }
          history_truncate_file(_grab_pimpl().expanded_history_filename.c_str(),
                                _history_truncate_);
        }
        clear_history();
      }
      DT_LOG_TRACE_EXITING(_logging_);
      return;
    }

    void basic_shell::load_macro(const std::string & macro_, uint32_t flags_)
    {
      DT_LOG_TRACE_ENTERING(get_logging());
      std::string macro = macro_;
      datatools::fetch_path_with_env(macro);
      DT_THROW_IF(!boost::filesystem::exists(macro),
                  std::runtime_error,
                  "File '" << macro << "' does not exist!");
      boost::filesystem::path p(macro);
      DT_THROW_IF(!boost::filesystem::is_regular_file(p),
                  std::runtime_error,
                  "File '" << macro << "' is not a regular file!");
      DT_LOG_TRACE(get_logging(), "Executing macro '" << macro << "...");
      std::ifstream ifmacro;
      ifmacro.open(macro.c_str());
      DT_THROW_IF(!ifmacro,
                  std::runtime_error,
                  "Cannot open macro file '" << macro << "'!");
      uint32_t flags = flags_;
      flags |= RC_INHIBIT_READLINE;
      flags |= RC_INHIBIT_HISTORY;
      {
        parser_context macro_context;
        macro_context.macro_name = macro;
        _grab_pimpl().pcontexts.push_back(macro_context);
      }
      datatools::command::returned_info cri = _run_core(&ifmacro, flags);
      {
        _grab_pimpl().pcontexts.pop_back();
      }
      ifmacro.close();
      DT_THROW_IF(cri.is_failure(),
                  std::runtime_error,
                  "Macro '" << macro << "' failed! " << cri.get_error_message());
      DT_LOG_TRACE_EXITING(get_logging());
      return;
    }

    datatools::command::returned_info basic_shell::_run_core(std::istream * in_, uint32_t flags_)
    {
      DT_LOG_TRACE_ENTERING(get_logging());
      datatools::command::returned_info cri;
      // Run the command line interface loop:
      bool go_on = true;
      std::vector<std::string> to_be_historized;
      bool using_prompt = true;
      bool using_history = _using_history_;
      bool exit_on_error = false;
      if (flags_ & RC_EXIT_ON_ERROR) {
        exit_on_error = true;
      }
      if (flags_ & RC_INHIBIT_HISTORY) {
        using_history = false;
      }
      bool using_readline = _using_readline_;
      if (flags_ & RC_INHIBIT_READLINE) {
        using_readline = false;
      }
      // Default input stream:
      std::istream * input = &std::cin;
      // Using an external input stream:
      if (in_ != 0) {
        // Special input stream: inhibit readline and prompt
        using_readline = false;
        using_prompt = false;
        input = in_;
      }
      if (!using_readline) {
        using_history = false;
      }

      // std::size_t line_counter = 0;
      // Main loop :
      while (go_on) {

        if (! using_readline) {
          if (! *input || input->eof()) {
            std::cerr << std::flush;
            std::cout << std::endl;
            break;
          }
        }
        std::string line;

        // Read a line:
        if (using_readline) {
          char * readline_line = 0;
          go_on = false;
          readline_line = readline(get_effective_prompt().c_str()); // use readline library
          _grab_pimpl().pcontexts.back().line_counter++;
          if (readline_line != 0) {
            go_on = true;
            line = readline_line; // use readline library
            if (! line.empty() && using_history) {
              to_be_historized.push_back(readline_line);
            }
            free(readline_line);
            readline_line = 0;
          }
        } else {
          // Prompt:
          if (using_prompt) {
            std::cerr << get_effective_prompt() << std::flush;
          }
          std::getline(*input, line);
          _grab_pimpl().pcontexts.back().line_counter++;
          if (! *input || input->eof()) {
            go_on = false;
          }
        } // End of read line

        {
          // Skip blank line and lines starting with '#' :
          std::istringstream dummy(line);
          std::string word;
          dummy >> word;
          if (word.empty()) continue;
          if (word[0] == '#') continue;
        }

        // Manage continuation marks :
        if (go_on) {
          while (line[line.length()-1] == '\\') {
            line = line.substr(0, line.length()-1);
            std::string more;
            if (using_readline) {
              char * readline_line = 0;
              go_on = false;
              readline_line = readline(get_effective_continuation_prompt().c_str()); // use readline library
              _grab_pimpl().pcontexts.back().line_counter++;
              if (readline_line != 0) {
                go_on = true;
                more = readline_line; // use readline library
                if (! more.empty() && using_history) {
                  to_be_historized.push_back(readline_line);
                }
                free(readline_line);
                readline_line = 0;
              }
            } else {
              if (using_prompt) {
                std::cerr << get_effective_continuation_prompt() << std::flush;
              }
              std::getline(*input, more);
              _grab_pimpl().pcontexts.back().line_counter++;
              if (! *input || input->eof()) {
                go_on = false;
              }
            }
            // Append to the current line:
            line += more;
          }
        } // End of continuation marks management.

        // Interpreter block:
        bool success = true;
        {
          cri.set_success();
          cri = _run_command(line);
          DT_LOG_TRACE(get_logging(),
                       "Command returned with error code [" << cri.get_error_code() << "]");
          if (cri.is_stop()) {
            DT_LOG_TRACE(get_logging(), "Command requests a shell stop.");
            go_on = false;
          } else if (cri.is_failure()) {
            success = false;
            DT_LOG_TRACE(get_logging(), "Command failed.");
            std::cerr << get_name() << ": error: ";
            const parser_context & pcontext = _grab_pimpl().pcontexts.back();
            if (!pcontext.macro_name.empty()) {
              std::cerr << "in macro '" << pcontext.macro_name << "'";
            }
            std::cerr << "at line [#" << pcontext.line_counter << "]: ";
            if (cri.has_error_message()) {
              std::cerr << cri.get_error_message();
            } else {
              std::cerr << "Command failed !";
            }
            if (exit_on_error || cri.get_error_code() == datatools::command::CEC_ABORT) {
              // Special kind of error occured: we abort the shell:
              go_on = false;
              std::cerr << " Abort!";
            }
            std::cerr << std::endl;
          } else {
            DT_LOG_TRACE(get_logging(), "Command was successful.");
          }
        } // End of interpreter block.

        // Use readline/history library:
        if (using_history) {
          bool historize_it = true;
          if (_history_add_only_on_success_ && ! success) {
            historize_it = false;
          }
          if (historize_it) {
            for (int iline = 0; iline < (int) to_be_historized.size(); iline++) {
              const std::string & line = to_be_historized[iline];
              DT_LOG_TRACE(get_logging(), "Adding history '" << line << "'...");
              add_history(line.c_str());
            }
          }
          to_be_historized.clear();
        }

        if (! using_readline && input->eof()) {
          DT_LOG_TRACE(get_logging(), "EOF.");
          go_on = false;
          break;
        }
        if (_grab_pimpl().stop_requested) {
          DT_LOG_TRACE(get_logging(), "Stop is requested.");
          go_on = false;
        }
        DT_LOG_TRACE(get_logging(), "go_on = " << go_on);
        if (! go_on) {
          break;
        }

      } // End of loop.

      DT_LOG_TRACE_EXITING(get_logging());
      return cri;
    }

    datatools::command::returned_info basic_shell::_run_command(const std::string & command_line_)
    {
      DT_LOG_TRACE_ENTERING(get_logging());
      datatools::command::returned_info cri;
      DT_LOG_TRACE(get_logging(), "Line is '" << command_line_ << "'");

      // Prepare the command line as an Unix-like array of options/arguments:
      std::vector<std::string> argv = boost::program_options::split_unix(command_line_);

      if (argv.size()) {
        DT_LOG_TRACE(get_logging(), "Line is not empty.");
        // Fetch the command name:
        std::string cmd_name = argv[0];

        bool call_done = false;

        // First try to call system/builtin commands (cd/ls/pwd...) from the system interface:
        if (! call_done) {
          if (has_system_interface()) {
            DT_LOG_TRACE(get_logging(), "Trying system interface...");
            if (_system_interface_->has_command(cmd_name)) {
              call_done = true;
              _system_interface_->call(argv, cri);
            } else {
              DT_LOG_TRACE(get_logging(), "Not a system interface command!");
            }
          } else {
            DT_LOG_TRACE(get_logging(), "No system interface is available!");
          }
        }

        // Next try some interface from the IHS:
        if (! call_done) {
          if (has_ihs()) {
            DT_LOG_TRACE(get_logging(), "Trying IHS...");
            // Expand the full path of the command:
            std::string cmd_full_path = this->canonical_path(cmd_name);
            // DT_LOG_TRACE(datatools::logger::PRIO_ALWAYS, "cmd_full_path = '" << cmd_full_path << "'");
            DT_LOG_TRACE(get_logging(), "cmd_full_path = '" << cmd_full_path << "'");
            // Check if the path correspond to an IHS command:
            if (_ihs_->is_command(cmd_full_path)) {
              DT_LOG_TRACE(get_logging(), "Found IHS command = '" << cmd_full_path << "'");
              // Then call the command with its vector of options/argument:
              std::vector<std::string> cmd_argv = argv;
              cmd_argv.erase(cmd_argv.begin());
              base_command & cmd = _ihs_->grab_command(cmd_full_path);
              call_done = true;
              cmd.call(cmd_argv, cri);
            } else {
              DT_LOG_TRACE(get_logging(), "Not a IHS command!");
            }
          } else {
            DT_LOG_TRACE(get_logging(), "No IHS is available!");
          }
        }

        if (! call_done) {
          // Finaly, we found no matching command:
          DT_COMMAND_RETURNED_ERROR(cri,
                                    datatools::command::CEC_COMMAND_INVALID,
                                    "Invalid command '" << cmd_name << "'!");
          DT_LOG_TRACE(get_logging(), "Could not found command '" << cmd_name << "'!");
        }

      } else {
        // Blank line means the shell should continue:
        cri.set_continue();
        DT_LOG_TRACE(get_logging(), "Shell continues...");
      }

      DT_LOG_TRACE_EXITING(get_logging());
      return cri;
    }

    void basic_shell::tree_dump(std::ostream & out_,
                                const std::string & title_,
                                const std::string & indent_,
                                bool inherit_) const
    {
      if (! title_.empty()) {
        out_ << indent_ << title_ << std::endl;
      }

      out_ << indent_ << i_tree_dumpable::tag
           << "Logging : '" << datatools::logger::get_priority_label(_logging_) << "'" << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Name    : '" << _name_ << "'" << std::endl;

      if (has_version()) {
        out_ << indent_ << i_tree_dumpable::tag
             << "Version  : '" << get_version() << "'" << std::endl;
      }

      out_ << indent_ << i_tree_dumpable::tag
           << "Prompt   : '" << _prompt_ << "'"  << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Continuation prompt : '" << _continuation_prompt_ << "'"  << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Default path : '" << _default_path_ << "'"  << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Exit on error : " << _exit_on_error_ << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Using splash : " << _using_splash_ << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Using readline : " << _using_readline_ << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Using history : " << _using_history_ << std::endl;

      if (_using_history_) {

        out_ << indent_ << i_tree_dumpable::tag
             << "History add only on success : " << _history_add_only_on_success_ << std::endl;

        out_ << indent_ << i_tree_dumpable::skip_tag << i_tree_dumpable::tag
             << "History filename : '" << _history_filename_  << "'" << std::endl;

        out_ << indent_ << i_tree_dumpable::skip_tag << i_tree_dumpable::last_tag
             << "History truncate : [#" << _history_truncate_ << ']' << std::endl;

      }

      out_ << indent_ << i_tree_dumpable::tag
           << "System interface : " << (has_system_interface() ? (std::string("'") +_system_interface_->get_name() + "'") : "<none>") << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "IHS : " << "[@" << _ihs_ << "]" << std::endl;
      if (has_ihs()) {
        std::ostringstream indent2;
        indent2 << indent_ << i_tree_dumpable::skip_tag;
        _ihs_->tree_dump(out_, "", indent2.str());
      }

      out_ << indent_ << i_tree_dumpable::tag
           << "Current working path : '" << _current_working_path_ << "'"  << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Services : [@" << _services_ << ']' << std::endl;

      out_ << indent_ << i_tree_dumpable::tag
           << "Stop requested : " << _get_pimpl().stop_requested << std::endl;

      out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
           << "Initialized : " << _initialized_ << std::endl;

      return;
    }

  } // namespace ui

} // namespace datatools
