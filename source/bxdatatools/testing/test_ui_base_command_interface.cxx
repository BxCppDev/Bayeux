// Ourselves:
#include <datatools/ui/base_command_interface.h>

// Standard Library:
#include <cstdlib>
#include <iostream>
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

int main(int /* argc_ */, char * /* argv_ */[])
{
  // A foo object;
  foo bar;
  std::clog << "bar = " << bar << std::endl;

  // The command interface for the object:
  foo_command_interface cmdInter(bar, "bar", "The bar object interface");
  // cmdInter.set_logging_priority(datatools::logger::PRIO_TRACE);
  cmdInter.initialize_simple();
  cmdInter.tree_dump(std::clog, "Bar command interface: ");

  // A micro shell:
  std::clog << "Starting shell (use 'quit' to exit the shell)..." << std::endl;
  std::string line;
  std::string prompt("test> ");
  std::clog << prompt << std::flush;
  while (std::getline(std::cin, line)) {
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
