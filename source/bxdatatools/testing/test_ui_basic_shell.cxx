// test_ui_basic_shell.h
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
#include <datatools/ui/basic_shell.h>

// Standard Library:
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>

// Third party:
#include <datatools/logger.h>
#include <datatools/command_utils.h>
#include <datatools/ui/utils.h>
#include "ui/foo_command_interface.h"

int main(int argc_, char * argv_[])
{
  try {

    // The objects:
    foo bar;
    foo joe;
    foo james;

    // The command interfaces:
    foo_command_interface barCmdInter(bar, "bar", "The bar object interface");
    barCmdInter.initialize_simple();
    barCmdInter.tree_dump(std::clog, "Bar command interface: ");
    std::clog << std::endl;

    foo_command_interface joeCmdInter(joe, "joe", "The joe object interface");
    joeCmdInter.initialize_simple();
    joeCmdInter.tree_dump(std::clog, "Joe command interface: ");
    std::clog << std::endl;

    foo_command_interface jamesCmdInter(james, "james", "The james object interface");
    jamesCmdInter.initialize_simple();
    jamesCmdInter.tree_dump(std::clog, "James command interface: ");
    std::clog << std::endl;

    // The interface hierarchical system:
    datatools::ui::ihs fooIHS;
    fooIHS.set_name("fooIHS");
    fooIHS.set_display_name("Foo IHS");
    fooIHS.set_terse_description("A foo interface hierarchical system (IHS)");
    // fooIHS.set_logging_priority(datatools::logger::PRIO_TRACE);
    fooIHS.add_interface("/", barCmdInter);
    fooIHS.add_interface("/", "test");
    fooIHS.add_interface("/test", joeCmdInter);
    fooIHS.add_interface("/", "dummy");
    fooIHS.add_interface("/dummy", ".what");
    fooIHS.add_interface("/dummy/.what", jamesCmdInter);
    fooIHS.tree_dump(std::clog, fooIHS.get_display_name());
    std::clog << std::endl;

    // The foo shell:
    datatools::ui::basic_shell fooShell;
    fooShell.set_name("fooShell");
    // fooShell.set_logging(datatools::logger::PRIO_TRACE);
    fooShell.set_version(datatools::version_id(1,0));
    fooShell.set_prompt("%n:%w> ");
    fooShell.set_continuation_prompt("> ");
    fooShell.set_using_splash(true);
    fooShell.set_using_readline(true);
    fooShell.set_using_history(true);
    fooShell.set_history_add_only_on_success(true);
    fooShell.set_history_filename("fooShell.history");
    fooShell.set_history_truncate(100);
    fooShell.set_ihs(fooIHS);
    fooShell.initialize_simple();

    // Run the shell session:
    fooShell.run();

    // Terminate the shell:
    fooShell.reset();

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
