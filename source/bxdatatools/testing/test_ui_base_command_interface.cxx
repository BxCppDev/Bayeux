// Ourselves:
#include <datatools/ui/base_command_interface.h>

// Standard Library:
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

// Third party:
// - Boost:
#include <boost/program_options.hpp>

// This project:
#include <datatools/command_utils.h>
#include <datatools/ui/base_command.h>
#include "ui/foo.h"
#include "ui/foo_commands.h"
#include "ui/foo_command_interface.h"

int main(int argc_, char * argv_[])
{
  bool interactive = false;
  if (argc_ > 1 && std::string(argv_[1]) == "-i") {
    interactive = true;
  }

  // A foo object;
  foo bar;
  std::clog << "bar = " << bar << std::endl;

  // The command interface for the object:
  foo_command_interface cmdInter(bar, "bar", "The bar object interface");
  // cmdInter.set_logging_priority(datatools::logger::PRIO_TRACE);
  cmdInter.initialize_simple();
  cmdInter.tree_dump(std::clog, "Bar command interface: ");

  std::ostringstream omacro;
  omacro << "set_value 23" << std::endl;
  omacro << "get_value" << std::endl;
  omacro << "zero" << std::endl;
  omacro << "get_value" << std::endl;
  omacro << "set_sum 1 3 4 12" << std::endl;
  omacro << "get_value" << std::endl;
  omacro << "quit" << std::endl;
  std::string macro = omacro.str();
  std::istringstream imacro(macro);

  std::istream * in = &imacro;
  if (interactive) {
    in = &std::cin;
  } else {
    std::clog << "Macro: " << std::endl;
    std::clog << macro;
  }

  // A micro shell:
  std::clog << "Starting shell (use 'quit' to exit the shell)..." << std::endl;
  std::string line;
  std::string prompt("test> ");
  std::clog << prompt << std::flush;
  while (std::getline(*in, line)) {
    std::vector<std::string> argv = boost::program_options::split_unix(line);
    if (argv.size()) {
      std::string cmd_name = argv[0];
      if (cmd_name == "quit") {
        break;
      } else if (cmdInter.has_command(cmd_name)) {
        std::clog << "notice: " << "Calling command '" << cmd_name << "'..." << std::endl;
        datatools::command::returned_info cri;
        cmdInter.call(argv, cri);
        if (! cri.is_success()) {
          std::cerr << "error: command '" << cmd_name << "' failed: "
                    << cri.get_error_message()
                    << std::endl;
        }
      } else {
        std::cerr << "error: Invalid command '" << cmd_name << "'" << std::endl;
      }
    }
    std::clog << prompt << std::flush;
  }
  std::clog << "Bye!" << std::endl << std::endl;

  cmdInter.reset();

  std::clog << "final bar = " << bar << std::endl;

  return EXIT_SUCCESS;
}
