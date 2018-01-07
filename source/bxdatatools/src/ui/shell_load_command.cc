// datatools/ui/shell_load_command.cc
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
#include <datatools/ui/shell_load_command.h>

// This project:
#include <datatools/ui/utils.h>
#include <datatools/detail/command_macros.h>

namespace datatools {

  namespace ui {

    shell_load_command::shell_load_command(basic_shell & shell_)
      : datatools::ui::target_command<basic_shell>(shell_, "load", "Load a shell macro file")
    {
      return;
    }

    shell_load_command::~shell_load_command()
    {
      if (is_initialized()) {
        reset();
      }
      return;
    }

    void shell_load_command::_init(const datatools::properties & config_)
    {
      this->base_command::_init(config_);

      // Arguments description:
      _grab_opts().add_options()

        ("macro",
         boost::program_options::value<std::string>()
         ->value_name("macro"),
         "Macro to be executed\n"
         "Examples: \n"
         "  load --macro \"${HOME}/config/commands.mac\" \n"
         "  load ~/config/commands.mac"
         )

        ("exit-on-error,x",
         "Exit the macro on first error"
         )

        ; // end of options description

      // Positional options:
      _grab_args().add("macro", 1);

      return;
    }

    void shell_load_command::_run(datatools::command::returned_info & cri_, uint32_t /*flags_*/)
    {
      cri_.reset();
      cri_.set_error_code(datatools::command::CEC_SUCCESS);
      DT_LOG_TRACE(get_logging_priority(), "Running command '" << get_name() << "'...");
      try {
        // Options:
        std::string macro;
        bool exit_on_error = false;
        basic_shell & shell = _grab_target();
        if (_grab_vmap().count("macro")) {
          macro = _grab_vmap()["macro"].as<std::string>();
        }
        if (_grab_vmap().count("exit-on-error")) {
          exit_on_error = true;
        }
        if (macro.empty()) {
          DT_COMMAND_RETURNED_ERROR(cri_,
                                    datatools::command::CEC_COMMAND_INVALID_NUMBER_OF_ARGUMENTS,
                                    get_name() + ": " + "Missing macro filename!");
        } else {
          uint32_t flags = basic_shell::RC_NONE;
          if (exit_on_error) {
            flags |= basic_shell::RC_EXIT_ON_ERROR;
          }
          shell.load_macro(macro, flags);
        }
      } catch (std::exception & error) {
        DT_COMMAND_RETURNED_ERROR(cri_,
                                  datatools::command::CEC_FAILURE,
                                  get_name() + ": " + error.what());
      }

      DT_LOG_TRACE(get_logging_priority(), "End of command '" << get_name() << "'.");
      return;
    }

  } // namespace ui

} // namespace datatools
