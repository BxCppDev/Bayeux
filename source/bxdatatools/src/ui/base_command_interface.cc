// datatools/ui/base_command_interface.cc
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
#include <datatools/ui/base_command_interface.h>

// Standard library:
#include <map>

// This project:
#include <datatools/ui/base_command.h>
#include <datatools/ui/basic_shell.h>
#include <datatools/service_manager.h>
#include <datatools/properties.h>
#include <datatools/detail/command_macros.h>

namespace datatools {

  namespace ui {

    DATATOOLS_FACTORY_SYSTEM_REGISTER_IMPLEMENTATION(base_command_interface,
                                                     "datatools::ui::base_command_interface/__system__")

    //! \brief Command entry
    struct command_entry_type {
      command_entry_type();
      void set_command(base_command & command_);
      void set_command(base_command * command_);
      base_command & grab_command();
      const base_command & get_command() const;
      bool is_external() const;
    private:
      boost::shared_ptr<base_command> _shared_cmd_; //!< Shared pointer to a command
      base_command *                  _ref_cmd_;    //!< Reference to an external command
    };

    command_entry_type::command_entry_type()
    {
      _ref_cmd_ = 0;
      return;
    }

    bool command_entry_type::is_external() const
    {
      return _ref_cmd_ != 0;
    }

    void command_entry_type::set_command(base_command * command_)
    {
      if (_ref_cmd_ != 0) _ref_cmd_ = 0;
      _shared_cmd_.reset(command_);
      return;
    }

    void command_entry_type::set_command(base_command & command_)
    {
      _shared_cmd_.reset();
      _ref_cmd_ = &command_;
      return;
    }

    base_command & command_entry_type::grab_command()
    {
      if (_ref_cmd_ != 0) {
        return *_ref_cmd_;
      }
      return *_shared_cmd_.get();
    }

    const base_command & command_entry_type::get_command() const
    {
      if (_ref_cmd_ != 0) {
        return *_ref_cmd_;
      }
      return *_shared_cmd_.get();
    }

    //! \brief Dictionary of command entries
    typedef std::map<std::string, command_entry_type> command_dict_type;

    //! \brief Private implementation data
    struct base_command_interface::pimpl_type
    {
      command_dict_type commands; //!< Dictionary of commands
    };

    base_command_interface::pimpl_type &
    base_command_interface::_grab_pimpl()
    {
      if (_pimpl_ == 0) {
        _pimpl_.reset(new pimpl_type);
      }
      return *_pimpl_;
    }

    const base_command_interface::pimpl_type &
    base_command_interface::_get_pimpl() const
    {
      base_command_interface * mutable_this = const_cast<base_command_interface *>(this);
      return const_cast<pimpl_type&>(mutable_this->_grab_pimpl());
    }


    base_command_interface::base_command_interface()
    {
      return;
    }

    base_command_interface::base_command_interface(const std::string & name_,
                                                   const std::string & description_,
                                                   const version_id & vid_)
    {
      _base_setup(name_, description_, vid_);
      return;
    }

    base_command_interface::~base_command_interface()
    {
      return;
    }

    bool base_command_interface::has_version() const
    {
      return _version_ != boost::none;
    }

    void base_command_interface::set_version(const datatools::version_id & version_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Command interface '" << get_name() << "' is initialized!");
      _set_version(version_);
      return;
    }

    const datatools::version_id & base_command_interface::get_version() const
    {
      DT_THROW_IF(!has_version(),
                  std::logic_error,
                  "No version identifier is available for parser '" << get_name() << "'!");
      return _version_.get();
    }

    bool base_command_interface::is_active() const
    {
      if (!is_valid()) return false;
      base_command_interface * mutable_this = const_cast<base_command_interface *>(this);
      return mutable_this->_is_active();
    }

    // virtual
    bool base_command_interface::_is_active() const
    {
      return true;
    }

    bool base_command_interface::is_valid() const
    {
      return _is_valid();
    }

    // virtual
    bool base_command_interface::_is_valid() const
    {
      return has_name() && is_initialized();
    }

    std::size_t base_command_interface::size() const
    {
      return _get_pimpl().commands.size();
    }

    void base_command_interface::build_command_names(std::vector<std::string> & names_) const
    {
      names_.clear();
      names_.reserve(_get_pimpl().commands.size());
      for (command_dict_type::const_iterator i = _get_pimpl().commands.begin();
           i != _get_pimpl().commands.end();
           i++) {
        names_.push_back(i->first);
      }
      return;
    }

    bool base_command_interface::has_command(const std::string & command_name_) const
    {
      return _get_pimpl().commands.find(command_name_) != _get_pimpl().commands.end();
    }

    base_command & base_command_interface::grab_command(const std::string & command_name_)
    {
      command_dict_type::iterator found = _grab_pimpl().commands.find(command_name_);
      DT_THROW_IF(found == _grab_pimpl().commands.end(),
                  std::logic_error,
                  "Command interface '" << get_name() << "' has no command named '"
                  << command_name_ << "'!");
      return found->second.grab_command();
    }

    const base_command & base_command_interface::get_command(const std::string & command_name_) const
    {
      command_dict_type::const_iterator found = _get_pimpl().commands.find(command_name_);
      DT_THROW_IF(found == _get_pimpl().commands.end(),
                  std::logic_error,
                  "Command interface '" << get_name() << "' has no command named '"
                  << command_name_ << "'!");
      return found->second.get_command();
    }

    base_command_interface & base_command_interface::add_command(base_command & command_,
                                                                 const std::string & command_name_)
    {
      std::string cmd_name = command_name_;
      if (cmd_name.empty()) {
        cmd_name = command_.get_name();
      }
      DT_THROW_IF(!command_.is_valid(),
                  std::logic_error,
                  "Command is not valid!");
      DT_THROW_IF(_get_pimpl().commands.find(cmd_name) != _get_pimpl().commands.end(),
                  std::logic_error,
                  "Command interface '" << get_name() << "' already has a command named '"
                  << cmd_name << "'!");
      {
        command_entry_type tmp;
        _grab_pimpl().commands[cmd_name] = tmp;
      }
      command_entry_type & cmd_entry
        = _grab_pimpl().commands.find(cmd_name)->second;
      cmd_entry.set_command(command_);
      return *this;
    }

    base_command_interface & base_command_interface::add_command(base_command * command_,
                                                                 const std::string & command_name_)
    {
      std::string cmd_name = command_name_;
      if (cmd_name.empty()) {
        cmd_name = command_->get_name();
      }
      DT_THROW_IF(! command_->is_valid(),
                  std::logic_error,
                  "Command is not valid!");
      DT_THROW_IF(_grab_pimpl().commands.find(cmd_name) != _grab_pimpl().commands.end(),
                  std::logic_error,
                  "Command interface '" << get_name() << "' already has a command named '"
                  << cmd_name << "'!");
      {
        command_entry_type tmp;
        _grab_pimpl().commands[cmd_name] = tmp;
      }
      command_entry_type & cmd_entry
        = _grab_pimpl().commands.find(cmd_name)->second;
      cmd_entry.set_command(command_);
      return *this;
    }

    void base_command_interface::remove_all_commands()
    {
      _grab_pimpl().commands.clear();
      return;
    }

    void base_command_interface::remove_command(const std::string & command_name_)
    {
      command_dict_type::iterator found = _grab_pimpl().commands.find(command_name_);
      DT_THROW_IF(found == _grab_pimpl().commands.end(),
                  std::logic_error,
                  "Command interface '" << get_name() << "' has no command named '"
                  << command_name_ << "'!");
      _grab_pimpl().commands.erase(found);
      return;
    }

    void base_command_interface::initialize_simple()
    {
      datatools::properties dummy_config;
      initialize_standalone(dummy_config);
      return;
    }

    void base_command_interface::initialize_standalone(const datatools::properties & config_)
    {
      datatools::service_manager dummy_services;
      initialize(config_, dummy_services);
      return;
    }

    void base_command_interface::_base_setup(const std::string & name_,
                                             const std::string & description_,
                                             const version_id & vid_)
    {
      if (! name_.empty()) {
        set_name(name_);
      }
      if (! description_.empty()) {
        set_terse_description(description_);
      }
      if (vid_.is_valid()) {
        _set_version(vid_);
      }
      return;
    }

    void base_command_interface::_base_initialize(const datatools::properties & config_)
    {
      this->enriched_base::initialize(config_, false);
      return;
    }

    void base_command_interface::_base_reset()
    {
      remove_all_commands();
      this->enriched_base::reset();
      return;
    }

    void base_command_interface::_set_version(const datatools::version_id & version_)
    {
      DT_THROW_IF(! version_.is_valid(), std::logic_error,
                  "Version ID '" << get_name() << "' is not valid!");
      _version_ = version_;
      return;
    }

    void base_command_interface::call(const std::vector<std::string> & argv_,
                                      datatools::command::returned_info & cri_,
                                      uint32_t flags_)
    {
      this->operator()(argv_, cri_, flags_);
      return;
    }

    int base_command_interface::operator()(const std::vector<std::string> & argv_,
                                           datatools::command::returned_info & cri_,
                                           uint32_t /*flags_*/)
    {
      DT_LOG_TRACE_ENTERING(get_logging_priority());
      DT_THROW_IF(!is_active(), std::logic_error, "Command interface '" << get_name() << "' is not active!");
      cri_.reset();
      cri_.set_error_code(datatools::command::CEC_SUCCESS);

      if (! argv_.size()) {
        DT_THROW_IF(!is_active(), std::logic_error, "Missing command name in command interface '" << get_name() << "'!");
      } else {
        std::string cmd_name = argv_[0];
        DT_LOG_TRACE(get_logging_priority(), "About to call command '" << cmd_name << "'...");
        if (! has_command(cmd_name)) {
          DT_COMMAND_RETURNED_ERROR(cri_,
                                    datatools::command::CEC_COMMAND_INVALID,
                                    "Invalid command name '" << cmd_name << "' in command interface '" << get_name() << "'!");
        } else {
          command_entry_type & cmd_entry
            = _grab_pimpl().commands.find(cmd_name)->second;
          base_command & cmd = cmd_entry.grab_command();
          if (!cmd.is_active()) {
            DT_COMMAND_RETURNED_ERROR(cri_,
                                      datatools::command::CEC_COMMAND_INVALID_CONTEXT,
                                      "Command '" << cmd_name << "' is not active in command interface '" << get_name() << "'!");
          } else {
            // Extract the list of arguments:
            std::vector<std::string> argv = argv_;
            argv.erase(argv.begin());
            // Command call:
            cmd(argv, cri_);
          }
        }
      }

      DT_LOG_TRACE_EXITING(get_logging_priority());
      return cri_.get_error_code();
    }

    datatools::command::returned_info
    base_command_interface::operator()(const std::vector<std::string> & argv_,
                                       uint32_t flags_)
    {
      datatools::command::returned_info cri;
      this->operator()(argv_, cri, flags_);
      return cri;
    }

    void base_command_interface::tree_dump(std::ostream & out_,
                                           const std::string & title_,
                                           const std::string & indent_,
                                           bool inherit_) const
    {
      this->enriched_base::tree_dump(out_, title_, indent_, true);

      if (has_version()) {
        out_ << indent_ << i_tree_dumpable::tag
             << "Version     : '" << _version_.get() << "'" << std::endl;
      }

      if (_pimpl_ != 0) {
        out_ << indent_ << i_tree_dumpable::tag
             << "Commands : [" << _pimpl_->commands.size() << "]" << std::endl;
        std::size_t counter = 0;
        for (command_dict_type::const_iterator icmd = _pimpl_->commands.begin();
             icmd != _pimpl_->commands.end();
             icmd++) {
          out_ << indent_ << i_tree_dumpable::skip_tag;
          if (++counter == _pimpl_->commands.size()) {
            out_ << i_tree_dumpable::last_tag;
          } else {
            out_ << i_tree_dumpable::tag;
          }
          // const command_entry_type & ce = icmd->second;
          // const base_command & cmd = ce.get_command();
          out_ << "Command : '" << icmd->first << "'" << std::endl;
        }
      }

      out_ << indent_ << i_tree_dumpable::inherit_tag(inherit_)
           << "Initialized : " << is_initialized() << std::endl;

      return;
    }

  } // namespace ui

} // namespace datatools
