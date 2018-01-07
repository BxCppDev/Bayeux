// datatools/ui/shell_ls_command.cc
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
#include <datatools/ui/shell_ls_command.h>

// This project:
#include <datatools/ui/utils.h>
#include <datatools/ui/traits.h>
#include <datatools/detail/command_macros.h>
#include <datatools/ui/ansi_colors.h>

namespace datatools {

  namespace ui {

    shell_ls_command::shell_ls_command(const basic_shell & shell_)
      : datatools::ui::const_target_command<basic_shell>(shell_, "ls", "List interfaces and commands")
    {
      return;
    }

    shell_ls_command::~shell_ls_command()
    {
      if (is_initialized()) {
        reset();
      }
      return;
    }

    void shell_ls_command::_init(const datatools::properties & config_)
    {
      this->base_command::_init(config_);

      // Arguments description:
      _grab_opts().add_options()

        ("path,p",
         boost::program_options::value<std::string>()
         ->value_name("path"),
         "Path to be listed\n"
         "Examples: \n"
         "  ls --path / \n"
         "  ls ~ \n"
         "  ls ./ \n"
         "  ls /"
         )

        ("all,a",
         "Do not ignore entries starting with ."
         )

        ("long,l",
         "Use a long listing format"
         )

        ("color",
         "Use colored output"
         )

        ("classify,F",
         "Append indicator (one of /*) to entries"
         )

        ; // end of options description

      // Positional options:
      _grab_args().add("path", 1);

      return;
    }

    void shell_ls_command::_run(datatools::command::returned_info & cri_, uint32_t /*flags_*/)
    {
      cri_.reset();
      cri_.set_error_code(datatools::command::CEC_SUCCESS);
      DT_LOG_TRACE(get_logging_priority(), "Running command '" << get_name() << "'...");
      try {
        // Options:
        std::string path;
        bool all      = false;
        bool classify = false;
        bool longf    = false;
        bool colored  = false;
        const basic_shell & shell = _get_target();
        if (_grab_vmap().count("path")) {
          path = _grab_vmap()["path"].as<std::string>();
        }
        if (path.empty()) {
          path = datatools::ui::path::current_interface_path();
        }
        if (_grab_vmap().count("all")) {
          all = true;
        }
        if (_grab_vmap().count("classify")) {
          classify = true;
        }
        if (_grab_vmap().count("long")) {
          longf = true;
        }
        if (_grab_vmap().count("color")) {
          colored = true;
        }
        std::string full_path = shell.canonical_path(path);

        std::vector<std::string> children;
        shell.get_ihs().build_children_paths(full_path, children);
        for (int i = 0; i < (int) children.size(); i++) {
          bool list_it = true;
          std::string child_full_path = shell.canonical_path(children[i]);
          std::string child_path = datatools::ui::path::basename(children[i]);
          bool child_interface = false;
          bool child_command   = false;
          bool child_disabled = false;
          bool child_broken = false;
          bool child_hidden = false;
          if (shell.get_ihs().is_trait(child_full_path, traits::hidden_label())) {
            child_hidden = true;
          }
          if (child_hidden && !all) {
            list_it = false;
          }
          if (list_it) {
            if (shell.get_ihs().is_trait(child_full_path, traits::disabled_label())) {
              child_disabled = true;
            }
            if (shell.get_ihs().is_trait(child_full_path, traits::broken_label())) {
              child_broken = true;
            }
            if (shell.get_ihs().is_interface(child_full_path)) {
              child_interface = true;

            } else {
              child_command = true;
            }
            if (colored) {
              if (child_interface) {
                if (child_broken) {
                  std::cout << ::datatools::ui::ansi_colors::color_8bits(5,0,0);
                } else if (child_disabled) {
                  std::cout << ::datatools::ui::ansi_colors::color_8bits(datatools::ui::ansi_colors::color_8bits::MODE_GRAYSCALE, 12);
                } else {
                  std::cout << ::datatools::ui::ansi_colors::color_8bits(0,0,5);
                }
              } else if (child_command) {
                if (child_broken) {
                  std::cout << ::datatools::ui::ansi_colors::color_8bits(5,0,0);
                } else if (child_disabled) {
                  std::cout << ::datatools::ui::ansi_colors::color_8bits(datatools::ui::ansi_colors::color_8bits::MODE_GRAYSCALE, 12);
                } else {
                  std::cout << ::datatools::ui::ansi_colors::color_8bits(0,5,0);
                }
              }
            }
            std::cout << child_path;
            if (colored) {
              std::cout << datatools::ui::ansi_colors::reset();
            }
            if (classify) {
              if (child_interface) {
               std::cout << ::datatools::ui::path::sep();
              } else if (child_command) {
               std::cout << ::datatools::ui::path::exec();
              }
            }
            if (longf) {
              if (shell.get_ihs().is_command(child_full_path) &&
                  shell.get_ihs().has_command(child_full_path)) {
                const base_command & cmd = shell.get_ihs().get_command(child_full_path);
                if (cmd.has_terse_description()) {
                  std::cout << " - " << cmd.get_terse_description();
                }
              }
              if (shell.get_ihs().is_interface(child_full_path) &&
                  shell.get_ihs().has_interface(child_full_path)) {
                const base_command_interface & cmdInter = shell.get_ihs().get_interface(child_full_path);
                if (cmdInter.has_terse_description()) {
                  std::cout << " - " << cmdInter.get_terse_description();
                }
              }
            }
            std::cout << std::endl;
          } // list_it
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
