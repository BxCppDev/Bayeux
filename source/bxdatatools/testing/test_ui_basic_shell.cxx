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
#include <sstream>
#include <vector>
#include <string>

// Third party:
#include <datatools/logger.h>
#include <datatools/command_utils.h>
#include <datatools/ui/utils.h>
#include <datatools/ui/traits.h>
#include "ui/foo_command_interface.h"

int main(int argc_, char * argv_[])
{
  int error_code = EXIT_SUCCESS;

  bool interactive = false;
  if (argc_ > 1 && std::string(argv_[1]) == "-i") {
    interactive = true;
  }

  try {

    // The objects:
    foo bar;
    foo joe;
    foo james;
    foo gnu;

    // The command interfaces:
    foo_command_interface barCmdInter(bar, "bar", "The bar object interface");
    barCmdInter.initialize_simple();
    barCmdInter.tree_dump(std::clog, "Bar command interface: ");
    std::clog << std::endl;

    foo_command_interface joeCmdInter(joe, "joe", "The joe object interface");
    joeCmdInter.initialize_simple();
    joeCmdInter.tree_dump(std::clog, "Joe command interface: ");
    std::clog << std::endl;

    foo_command_interface gnuCmdInter(gnu, "gnu", "The gnu object interface");
    gnuCmdInter.initialize_simple();
    gnuCmdInter.tree_dump(std::clog, "Gnu command interface: ");
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
    fooIHS.add_interface("/", "baz");
    fooIHS.add_interface("/baz", "gnat");
    fooIHS.add_interface("/baz/gnat", gnuCmdInter);
    fooIHS.add_interface("/", "void");
    fooIHS.add_interface("/dummy", ".what");
    fooIHS.add_interface("/dummy/.what", jamesCmdInter);
    fooIHS.tree_dump(std::clog, fooIHS.get_display_name());
    std::clog << std::endl;

    // Add specific traits:
    fooIHS.set_trait("/dummy/.what/james/set_value", datatools::ui::traits::disabled_label());
    fooIHS.set_trait("/bar/set_sum", datatools::ui::traits::broken_label());
    fooIHS.set_trait("/baz/gnat/gnu", datatools::ui::traits::broken_label());
    fooIHS.set_trait("/test/joe", datatools::ui::traits::hidden_label());
    fooIHS.set_trait("/void", datatools::ui::traits::disabled_label());

    // The foo shell:
    datatools::ui::basic_shell fooShell;
    fooShell.set_name("fooShell"); // Mandatory
    fooShell.set_title("foo shell"); // Mandatory
    fooShell.set_logging(datatools::logger::PRIO_NOTICE);
    fooShell.set_version(datatools::version_id(1,0));
    fooShell.set_prompt("%n:%W> ");
    fooShell.set_continuation_prompt("> ");
    fooShell.set_exit_on_error(true);
    fooShell.set_using_splash(true);
    fooShell.set_using_readline(false);
    if (interactive) {
      fooShell.set_exit_on_error(false);
      fooShell.set_using_readline(true);
      fooShell.set_using_history(true);
      fooShell.set_history_add_only_on_success(true);
      fooShell.set_history_filename("fooShell.history");
      fooShell.set_history_truncate(100);
    }
    fooShell.set_ihs(fooIHS);
    fooShell.set_default_path("/dummy");
    fooShell.initialize_simple();
    {
      boost::property_tree::ptree popts;
      popts.put("title", "Shell: ");
      fooShell.print_tree(std::cerr, popts);
    }

    // Batch mode uses a dedicated input stream:
    std::ostringstream omacro;
    omacro << "pwd" << std::endl;
    omacro << "ls" << std::endl;
    omacro << "tree --all --color /" << std::endl;
    omacro << "cd /bar" << std::endl;
    omacro << "pwd" << std::endl;
    omacro << "ls --long" << std::endl;
    omacro << "set_value 2" << std::endl;
    omacro << "get_value" << std::endl;
    omacro << "cd /test/joe" << std::endl;
    omacro << "pwd" << std::endl;
    omacro << "set_sum 1 2 3 4 5" << std::endl;
    omacro << "get_value" << std::endl;
    omacro << "/bar/get_value" << std::endl;
    omacro << "exit" << std::endl;
    std::string macro = omacro.str();
    std::istringstream imacro(macro);

    std::istringstream * in = &imacro;
    if (interactive) {
      std::clog << "NOTICE: Interactive mode..." << std::endl;
      in = 0;
    } else {
      std::clog << "NOTICE: Batch mode..." << std::endl;
    }

    // Run the shell session:
    error_code = fooShell.run(in);

    // Terminate the shell:
    fooShell.reset();

  } catch (std::exception & error) {
    DT_LOG_ERROR(datatools::logger::PRIO_ALWAYS, error.what());
    error_code = EXIT_FAILURE;
  } catch (...) {
    DT_LOG_ERROR(datatools::logger::PRIO_ALWAYS, "Unexpected error!");
    error_code = EXIT_FAILURE;
  }

  return error_code;
}
