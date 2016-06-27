// datatools/ui/shell_help_command.cc
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
#include <datatools/ui/shell_help_command.h>

// This project:
#include <datatools/ui/utils.h>
#include <datatools/detail/command_macros.h>

namespace datatools {

  namespace ui {

    shell_help_command::shell_help_command(const basic_shell & shell_)
      : datatools::ui::const_target_command<basic_shell>(shell_, "help", "On-line help about the shell builtin commands")
    {
      return;
    }

    shell_help_command::~shell_help_command()
    {
      if (is_initialized()) {
        reset();
      }
      return;
    }

    void shell_help_command::_init(const datatools::properties & config_)
    {
      this->base_command::_init(config_);
      // Arguments description:
      _grab_opts().add_options()

        ("no-description,D",
         "Do not print description if available"
         )

        ("no-title,T",
         "Do not print title"
         )

        ("color,C",
         "Use colored output"
         )

        ; // end of options description

      return;
    }

    void shell_help_command::_run(datatools::command::returned_info & cri_, uint32_t /*flags_*/)
    {
      cri_.reset();
      cri_.set_error_code(datatools::command::CEC_SUCCESS);
      DT_LOG_TRACE(get_logging_priority(), "Running command '" << get_name() << "'...");
      try {
        const basic_shell & shell = get_target();
        // Options:
        bool no_description = false;
        bool no_title = false;
        bool colored  = false;
        if (_grab_vmap().count("no-title")) {
          no_title = true;
        }
        if (_grab_vmap().count("no-description")) {
          no_description = true;
        }
        if (_grab_vmap().count("color")) {
          colored = true;
        }
        std::vector<std::string> builtin_commands;
        shell.builtin_command_names(builtin_commands);
        if (!no_title) {
          std::cout << shell.get_name() << " builtin commands :";
          std::cout << std::endl;
        }
        for (std::vector<std::string>::const_iterator icmd = builtin_commands.begin();
             icmd != builtin_commands.end();
             icmd++) {
          const std::string & buildin_cmdname = *icmd;
          const base_command & builtin_cmd = shell.get_builtin_command(buildin_cmdname);
          std::string cmd_color_tag;
          std::string desc_color_tag;
          std::string reset_tag;
          if (colored) {
            cmd_color_tag = ::datatools::ui::ansi_colors::green();
            desc_color_tag = ::datatools::ui::ansi_colors::magenta();
            reset_tag = ::datatools::ui::ansi_colors::reset();
          }
          if (colored) {
            std::cout << cmd_color_tag;
          }
          std::cout << buildin_cmdname;
          if (colored) {
            std::cout << reset_tag;
          }
          if (! no_description && builtin_cmd.has_terse_description()) {
            std::cout << " - ";
            if (colored) {
              std::cout << desc_color_tag;
            }
            std::cout << builtin_cmd.get_terse_description();
            if (colored) {
              std::cout << reset_tag;
            }
          }
          std::cout << std::endl;
        }

      }
      catch (std::exception & error) {
        DT_COMMAND_RETURNED_ERROR(cri_,
                                  datatools::command::CEC_FAILURE,
                                  get_name() + ": " + error.what());
      }

      DT_LOG_TRACE(get_logging_priority(), "End of command '" << get_name() << "'.");
      return;
    }

  } // namespace ui

} // namespace datatools
