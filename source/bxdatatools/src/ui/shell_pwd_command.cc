// datatools/ui/shell_pwd_command.cc
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
#include <datatools/ui/shell_pwd_command.h>

// This project:
#include <datatools/ui/utils.h>
#include <datatools/detail/command_macros.h>

namespace datatools {

  namespace ui {

    shell_pwd_command::shell_pwd_command(const basic_shell & shell_)
      : datatools::ui::const_target_command<basic_shell>(shell_, "pwd", "Print current working interface")
    {
      return;
    }

    shell_pwd_command::~shell_pwd_command()
    {
      if (is_initialized()) {
        reset();
      }
      return;
    }

    void shell_pwd_command::_run(datatools::command::returned_info & cri_, uint32_t /*flags_*/)
    {
      cri_.reset();
      cri_.set_error_code(datatools::command::CEC_SUCCESS);
      DT_LOG_TRACE(get_logging_priority(), "Running command '" << get_name() << "'...");
      try {
        DT_LOG_TRACE(get_logging_priority(), "Pwd...");
        std::cout << _get_target().get_current_path() << std::endl;
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
