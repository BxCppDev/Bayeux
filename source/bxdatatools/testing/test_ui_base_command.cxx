// Ourselves:
#include <datatools/ui/base_command.h>

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
#include "ui/foo.h"
#include "ui/foo_commands.h"

int main(int argc_, char * argv_[])
{
  bool interactive = false;
  if (argc_ > 1 && std::string(argv_[1]) == "-i") {
    interactive = true;
  }

  foo bar;
  std::clog << "bar = " << bar << std::endl;

  foo_set_value cmdSet(bar);
  cmdSet.initialize_simple();

  foo_get_value cmdGet(bar);
  cmdGet.initialize_simple();

  foo_zero cmdZero(bar);
  cmdZero.initialize_simple();

  foo_set_sum cmdSetSum(bar);
  cmdSetSum.initialize_simple();

  foo_test cmdTest(bar);
  cmdTest.initialize_simple();

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
      } else if (cmd_name == cmdSet.get_name()) {
        argv.erase(argv.begin());
        datatools::command::returned_info cri = cmdSet(argv);
        if (! cri.is_success()) {
          std::cerr << "error: command '" << cmdSet.get_name()<< "' failed: "
                    << cri.get_error_message()
                    << std::endl;
        }
      } else if (cmd_name == cmdGet.get_name()) {
        argv.erase(argv.begin());
        datatools::command::returned_info cri = cmdGet(argv);
        if (! cri.is_success()) {
          std::cerr << "error: command '" << cmdGet.get_name()<< "' failed: "
                    << cri.get_error_message()
                    << std::endl;
        }
      } else if (cmd_name == cmdZero.get_name()) {
        argv.erase(argv.begin());
        datatools::command::returned_info cri = cmdZero(argv);
        if (! cri.is_success()) {
          std::cerr << "error: command '" << cmdZero.get_name()<< "' failed: "
                    << cri.get_error_message()
                    << std::endl;
        }
      } else if (cmd_name == cmdSetSum.get_name()) {
        argv.erase(argv.begin());
        datatools::command::returned_info cri = cmdSetSum(argv);
        if (! cri.is_success()) {
          std::cerr << "error: command '" << cmdSetSum.get_name()<< "' failed: "
                    << cri.get_error_message()
                    << std::endl;
        }
      } else if (cmd_name == cmdTest.get_name()) {
        argv.erase(argv.begin());
        datatools::command::returned_info cri = cmdTest(argv);
        if (! cri.is_success()) {
          std::cerr << "error: command '" << cmdTest.get_name()<< "' failed: "
                    << cri.get_error_message()
                    << std::endl;
        }
       } else {
        std::cerr << "error: Unknown command: " << cmd_name << std::endl;
      }
    }
    std::clog << prompt << std::flush;
  }
  std::clog << "Bye!" << std::endl << std::endl;

  cmdSetSum.reset();
  cmdZero.reset();
  cmdGet.reset();
  cmdSet.reset();
  cmdTest.reset();

  std::clog << "final bar = " << bar << std::endl;

  return EXIT_SUCCESS;
}
