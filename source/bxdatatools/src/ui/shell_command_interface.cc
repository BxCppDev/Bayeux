// datatools/ui/shell_command_interface.cc
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

// Ourselves:
#include <datatools/ui/shell_command_interface.h>

// This project:
#include <datatools/ui/basic_shell.h>
#include <datatools/ui/shell_commands.h>

namespace datatools {

  namespace ui {

    shell_command_interface::shell_command_interface(basic_shell & sh_,
                                                     const std::string & name_,
                                                     const std::string & description_,
                                                     const datatools::version_id & vid_)
      : datatools::ui::target_command_interface<basic_shell>(sh_, name_, description_, vid_)
    {
      _initialized_ = false;
      _inhibit_cd_ = false;
      return;
    }

    shell_command_interface::~shell_command_interface()
    {
      if (is_initialized()) {
        reset();
      }
      return;
    }

    bool shell_command_interface::has_shell() const
    {
      return has_target();
    }

    basic_shell & shell_command_interface::grab_shell()
    {
      return grab_target();
    }

    const basic_shell & shell_command_interface::get_shell() const
    {
      return get_target();
    }

    bool shell_command_interface::is_initialized() const
    {
      return _initialized_;
    }

    void shell_command_interface::initialize(const datatools::properties & config_,
                                             const datatools::service_manager & services_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Command interface is already initialized!");
      DT_THROW_IF(!has_shell(), std::logic_error,
                  "Command interface has no shell!");
      base_command_interface::_base_initialize(config_);

      if (config_.has_key("inhibit_cd")) {
        bool icd = config_.fetch_boolean("inhibit_cd");
        set_inhibit_cd(icd);
      }

      _add_base_commands();
      _initialized_ = true;
      return;
    }

    void shell_command_interface::reset()
    {
      DT_THROW_IF(!is_initialized(), std::logic_error,
                  "Command interface is not initialized!");
      _initialized_ = false;
      _remove_base_commands();
      _inhibit_cd_ = false;
      base_command_interface::_base_reset();
      return;
    }

    bool shell_command_interface::is_inhibit_cd() const
    {
      return _inhibit_cd_;
    }

    void shell_command_interface::set_inhibit_cd(bool icd_)
    {
      DT_THROW_IF(is_initialized(), std::logic_error,
                  "Shell command interface is already initialized!");
      _inhibit_cd_ = icd_;
      return;
    }

    void shell_command_interface::_add_base_commands()
    {
      // Populate the interface with base commands:

      {
        shell_exit_command * cmdExit = new shell_exit_command(grab_shell());
        cmdExit->initialize_simple();
        add_command(cmdExit, "exit");
      }

      {
        shell_man_command * cmdMan = new shell_man_command(grab_shell());
        cmdMan->initialize_simple();
        add_command(cmdMan, "man");
      }

      {
        shell_pwd_command * cmdPwd = new shell_pwd_command(grab_shell());
        cmdPwd->initialize_simple();
        add_command(cmdPwd, "pwd");
      }

      {
        shell_ls_command * cmdLs = new shell_ls_command(grab_shell());
        cmdLs->initialize_simple();
        add_command(cmdLs, "ls");
      }

      if (! is_inhibit_cd()) {
        shell_cd_command * cmdCd = new shell_cd_command(grab_shell());
        cmdCd->initialize_simple();
        add_command(cmdCd, "cd");
      }

      {
        shell_tree_command * cmdTree = new shell_tree_command(grab_shell());
        cmdTree->initialize_simple();
        add_command(cmdTree, "tree");
      }

      return;
    }

    void shell_command_interface::_remove_base_commands()
    {
      remove_command("cd");
      remove_command("exit");
      remove_command("man");
      remove_command("pwd");
      remove_command("ls");
      remove_command("tree");
      return;
    }

    /*
      datatools::command::returned_info shell_command_interface::_cmd_printhelp(const std::string & interface_name_)
      {
      datatools::command::returned_info cri;

      std::string interface_name = interface_name_;
      if (interface_name.empty() || interface_name == shell::system_interface_name()) {
      std::cout << grab_parent_shell().get_name() << " system shell usage: " << std::endl;
      std::cout << std::endl;
      std::cout << "Supported commands are: " << std::endl;
      std::cout << std::endl;
      this->print_usage(std::cout);
      std::cout << std::endl;
      } else if (grab_parent_shell().has_interface(interface_name)) {
      std::cout << interface_name << " usage: " << std::endl << std::endl;
      std::cout << '\t' << interface_name << " {command} [options...] [arguments...]" << std::endl;
      std::cout << std::endl;
      std::cout << "Supported commands are: " << std::endl;
      std::cout << std::endl;
      grab_parent_shell()._grab_interface(interface_name).print_usage(std::cout);
      std::cout << std::endl;
      } else {
      DT_COMMAND_RETURNED_ERROR(cri, datatools::command::CEC_PARAMETER_INVALID,
      "Invalid interface '" << interface_name << "'!");
      }
      return cri;
      }
    */

  } // namespace ui

} // namespace datatools
