// test_ui_shell_command_interface.h
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

// Standard Library:
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>

// Third party:
#include <datatools/logger.h>
#include <datatools/command_utils.h>
#include <datatools/ui/utils.h>
#include <datatools/ui/basic_shell.h>
#include "ui/foo_command_interface.h"

int main(/*int argc_, char * argv_[]*/)
{
  try {

    // A shell (not initialized):
    datatools::ui::basic_shell aShell;

    // The shell command interface:
    datatools::ui::shell_command_interface shellCmdInter(aShell, "shellCmdInter");
    shellCmdInter.tree_dump(std::clog, shellCmdInter.get_name());
    std::clog << std::endl;

  }
  catch (std::exception & error) {
    DT_LOG_ERROR(datatools::logger::PRIO_ALWAYS, error.what());
    return EXIT_FAILURE;
  }
  catch (...) {
    DT_LOG_ERROR(datatools::logger::PRIO_ALWAYS, "Unexpected error!");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
