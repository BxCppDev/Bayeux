// Ourselves:
#include <datatools/ui/reflective_command.h>
#include <datatools/ui/reflective_command_interface.h>

// Standard Library:
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>

// Third party:
// - Boost:
#include <boost/program_options.hpp>
// - Camp:
#include <camp/enum.hpp>
#include <camp/enumget.hpp>
#include <camp/class.hpp>
#include <camp/classget.hpp>

// This project:
#include <datatools/command_utils.h>
#include "ui/foo.h"
#include "ui/foo-reflect.h"
#include "ui/foo_commands.h"

void check()
{
  std::clog << "List of registered metaclasses:\n";
  for (std::size_t i = 0; i < camp::classCount(); i++) {
    const camp::Class & metaclass = camp::classByIndex(i);
    std::clog << "Class: '" << metaclass.name() << "'" << '\n';
  }
  std::clog << "List of registered metaenums:\n";
  for (std::size_t i = 0; i < camp::enumCount(); i++) {
    const camp::Enum & metaenum = camp::enumByIndex(i);
    std::clog << "Enum: '" << metaenum.name() << "'" << '\n';
  }
  return;
}

void test_0(bool interactive_);

void test_1(bool interactive_);

void test_2(bool interactive_);

void test_3(bool interactive_);

int main(int argc_, char * argv_[])
{
  bool interactive = false;
  if (argc_ > 1 && std::string(argv_[1]) == "-i") {
    interactive = true;
  }
  try {
    check();
    // test_0(interactive);
    test_1(interactive);
    test_2(interactive);
    test_3(interactive);
  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    return EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: " << "unexpected error" << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

void test_0(bool /*interactive_*/)
{
  std::clog << "\ntest_0..." << std::endl;

  int value = 42;
  std::clog << "value = " << value << std::endl;

  try {

    {
      std::string repr = "  666  ";
      value = boost::lexical_cast<int>(repr);
      std::clog << "value = " << value << std::endl;
    }

    {
      std::string repr = "Booh!";
      value = boost::lexical_cast<int>(repr);
      std::clog << "value = " << value << std::endl;
    }

  } catch (std::exception & error) {
    std::cerr << "error: As expected: " << error.what() << std::endl;
  }
  return;
}

void test_1(bool interactive_)
{
  std::clog << "\ntest_1..." << std::endl;
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
  if (interactive_) {
    in = &std::cin;
  } else {
    std::clog << "Macro: " << std::endl;
    std::clog << macro;
  }
  // A micro shell:
  std::clog << "Starting shell (use 'quit' to exit the shell)..." << std::endl;
  std::string line;
  std::string prompt("test> ");
  std::clog << "current bar = " << bar << std::endl;
  std::clog << prompt << std::flush;
  while (std::getline(*in, line)) {
    std::vector<std::string> argv = boost::program_options::split_unix(line);
    if (argv.size()) {
      std::string cmd_name = argv[0];
      if (cmd_name == "quit" || cmd_name == "exit") {
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
      std::clog << "current bar = " << bar << std::endl;
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

  return;
}

void test_2(bool interactive_)
{
  std::clog << "\ntest_2..." << std::endl;
  foo bar;
  std::clog << "bar = " << bar << std::endl;

  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  // logging = datatools::logger::PRIO_TRACE;

  std::map<std::string, datatools::ui::base_command *> comset;

  datatools::ui::reflective_command<foo> cmdSetName(bar, "set_name");
  // cmdSetName.set_logging_priority(logging);
  cmdSetName.initialize_simple();
  comset[cmdSetName.get_name()] = &cmdSetName;

  datatools::ui::reflective_command<foo> cmdGetName(bar, "get_name");
  // cmdGetName.set_logging_priority(logging);
  cmdGetName.initialize_simple();
  comset[cmdGetName.get_name()] = &cmdGetName;

  datatools::ui::reflective_command<foo> cmdSetFlag(bar, "set_flag");
  // cmdSetFlag.set_logging_priority(logging);
  cmdSetFlag.initialize_simple();
  comset[cmdSetFlag.get_name()] = &cmdSetFlag;

  datatools::ui::reflective_command<foo> cmdIsFlag(bar, "is_flag");
  // cmdIsFlag.set_logging_priority(logging);
  cmdIsFlag.initialize_simple();
  comset[cmdIsFlag.get_name()] = &cmdIsFlag;

  datatools::ui::reflective_command<foo> cmdSetValue(bar, "set_value");
  // cmdSetValue.set_logging_priority(logging);
  cmdSetValue.initialize_simple();
  comset[cmdSetValue.get_name()] = &cmdSetValue;

  datatools::ui::reflective_command<foo> cmdGetValue(bar, "get_value");
  // cmdGetValue.set_logging_priority(logging);
  cmdGetValue.initialize_simple();
  comset[cmdGetValue.get_name()] = &cmdGetValue;

  datatools::ui::reflective_command<foo> cmdSetX(bar, "set_x");
  // cmdSetX.set_logging_priority(logging);
  cmdSetX.initialize_simple();
  comset[cmdSetX.get_name()] = &cmdSetX;

  datatools::ui::reflective_command<foo> cmdGetX(bar, "get_x");
  // cmdGetX.set_logging_priority(logging);
  cmdGetX.initialize_simple();
  comset[cmdGetX.get_name()] = &cmdGetX;

  datatools::ui::reflective_command<foo> cmdSetMass(bar, "set_mass");
  // cmdSetMass.set_logging_priority(logging);
  cmdSetMass.initialize_simple();
  comset[cmdSetMass.get_name()] = &cmdSetMass;

  datatools::ui::reflective_command<foo> cmdGetMass(bar, "get_mass");
  // cmdGetMass.set_logging_priority(logging);
  cmdGetMass.initialize_simple();
  comset[cmdGetMass.get_name()] = &cmdGetMass;

  datatools::ui::reflective_command<foo> cmdSetTime(bar, "set_time");
  // cmdSetTime.set_logging_priority(logging);
  cmdSetTime.initialize_simple();
  comset[cmdSetTime.get_name()] = &cmdSetTime;

  datatools::ui::reflective_command<foo> cmdGetTime(bar, "get_time");
  // cmdGetTime.set_logging_priority(logging);
  cmdGetTime.initialize_simple();
  comset[cmdGetTime.get_name()] = &cmdGetTime;

  datatools::ui::reflective_command<foo> cmdZero(bar, "zero");
  cmdZero.initialize_simple();
  comset[cmdZero.get_name()] = &cmdZero;

  datatools::ui::reflective_command<foo> cmdIncr(bar, "incr");
  cmdIncr.initialize_simple();
  comset[cmdIncr.get_name()] = &cmdIncr;

  datatools::ui::reflective_command<foo> cmdDecr(bar, "decr");
  cmdDecr.initialize_simple();
  comset[cmdDecr.get_name()] = &cmdDecr;

  datatools::ui::reflective_command<foo> cmdAddCount(bar, "add_count");
  cmdAddCount.initialize_simple();
  comset[cmdAddCount.get_name()] = &cmdAddCount;

  datatools::ui::reflective_command<foo> cmdGetCount(bar, "get_count");
  cmdGetCount.initialize_simple();
  comset[cmdGetCount.get_name()] = &cmdGetCount;

  datatools::ui::reflective_command<foo> cmdReset(bar, "reset");
  cmdReset.initialize_simple();
  comset[cmdReset.get_name()] = &cmdReset;

  datatools::ui::reflective_command<foo> cmdPrint(bar, "print");
  cmdPrint.initialize_simple();
  comset[cmdPrint.get_name()] = &cmdPrint;

  datatools::ui::reflective_command<foo> cmdSetB(bar, "set_b");
  cmdSetB.set_logging_priority(logging);
  cmdSetB.initialize_simple();
  comset[cmdSetB.get_name()] = &cmdSetB;

  datatools::ui::reflective_command<foo> cmdGetB(bar, "get_b");
  cmdGetB.initialize_simple();
  comset[cmdGetB.get_name()] = &cmdGetB;

  datatools::ui::reflective_command<foo> cmdSetLogging(bar, "set_logging");
  cmdSetLogging.set_logging_priority(logging);
  cmdSetLogging.initialize_simple();
  comset[cmdSetLogging.get_name()] = &cmdSetLogging;

  datatools::ui::reflective_command<foo> cmdGetLogging(bar, "get_logging");
  cmdGetLogging.set_logging_priority(logging);
  cmdGetLogging.initialize_simple();
  comset[cmdGetLogging.get_name()] = &cmdGetLogging;

  std::ostringstream omacro;
  omacro << "set_flag 1" << std::endl;
  // omacro << "set_value 23" << std::endl;
  // omacro << "get_value" << std::endl;
  // omacro << "zero" << std::endl;
  // omacro << "get_value" << std::endl;
  // omacro << "set_sum 1 3 4 12" << std::endl;
  // omacro << "get_value" << std::endl;
  omacro << "quit" << std::endl;
  std::string macro = omacro.str();
  std::istringstream imacro(macro);

  std::istream * in = &imacro;
  if (interactive_) {
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
    if (datatools::logger::is_debug(logging)) {
      if (argv.size()) {
        std::cerr << "debug: argv : " << std::endl;
        for (auto arg : argv) {
          std::cerr << "debug: arg = '" << arg << "'" << std::endl;
        }
      }
    }
    if (argv.size()) {
      std::string cmd_name = argv[0];
      argv.erase(argv.begin());
      if (cmd_name == "quit" || cmd_name == "exit") {
        break;
      } else if (cmd_name == "help") {
        std::clog << "List of supported commands: \n";
        for (auto com : comset) {
          std::clog << " - " << com.first;
          if (com.second->has_terse_description()) {
            std::clog << " : " << com.second->get_terse_description();
          }
          std::clog << "\n";
        }
      } else if (comset.count(cmd_name)) {
        datatools::ui::base_command & comm = *(comset.find(cmd_name)->second);
        datatools::command::returned_info cri = comm(argv);
        if (! cri.is_success()) {
          std::cerr << "error: command '" << cmd_name<< "' failed: " << std::endl;
          std::cerr << "error details:\n"
                    << cri.get_error_message()
                    << std::endl;
        }
      } else {
        std::cerr << "error: Unknown command: " << cmd_name << std::endl;
      }
      if (datatools::logger::is_debug(logging)) {
        std::cerr << "debug: current bar = " << bar << std::endl;
      }
    }
    std::clog << prompt << std::flush;
  }
  std::clog << "Bye!" << std::endl << std::endl;

  std::clog << "final bar = " << bar << std::endl;

  return;
}

void test_3(bool interactive_)
{
  std::clog << "\ntest_3...  " << std::endl;
  foo bar;
  std::clog << "bar = " << bar << std::endl;

  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  // logging = datatools::logger::PRIO_TRACE;

  datatools::ui::reflective_command_interface<foo> barInter(bar, "bar", "bar command interface");
  barInter.initialize_simple();
  barInter.tree_dump(std::clog, "Command interface: ");

  std::ostringstream omacro;
  omacro << "set_flag 1" << std::endl;
  // omacro << "set_value 23" << std::endl;
  // omacro << "get_value" << std::endl;
  // omacro << "zero" << std::endl;
  // omacro << "get_value" << std::endl;
  // omacro << "get_value" << std::endl;
  omacro << "quit" << std::endl;
  std::string macro = omacro.str();
  std::istringstream imacro(macro);

  std::istream * in = &imacro;
  if (interactive_) {
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
    if (datatools::logger::is_debug(logging)) {
      if (argv.size()) {
        std::cerr << "debug: argv : " << std::endl;
        for (auto arg : argv) {
          std::cerr << "debug: arg = '" << arg << "'" << std::endl;
        }
      }
    }
    if (argv.size()) {
      std::string cmd_name = argv[0];
      argv.erase(argv.begin());
      if (cmd_name == "quit" || cmd_name == "exit") {
        break;
      } else if (cmd_name == "man") {
        std::clog << "note: Command '" << cmd_name << "'..." << std::endl;
        if (argv.size() != 1) {
          std::cerr << "error: command '" << cmd_name << "' failed: " << std::endl;
          std::cerr << "error details:\n"
                    << "Missing command name!"
                    << std::endl;
        } else {
          std::string comname = argv[0];
          std::clog << "note: man '" << comname << "'" << std::endl;
          if (!barInter.has_command(comname)) {
            std::cerr << "error: command '" << cmd_name << "' failed: " << std::endl;
            std::cerr << "error details:\n"
                      << "No command '" << comname << "'!"
                      << std::endl;
          } else {
            std::cerr << "note: man '" << cmd_name << "' with command '" << comname << "'" << std::endl;
            const datatools::ui::base_command & comm = barInter.get_command(comname);
            comm.tree_dump(std::clog, "Command: ");
          }
        }
      } else if (cmd_name == "help") {
        std::vector<std::string> comnames;
        barInter.build_command_names(comnames);
        std::clog << "List of supported commands: \n";
        for (auto comname : comnames) {
          std::clog << " - " << comname;
          const datatools::ui::base_command & comm = barInter.get_command(comname);
          if (comm.has_terse_description()) {
            std::clog << " : " << comm.get_terse_description();
          }
          std::clog << "\n";
        }
      } else if (barInter.has_command(cmd_name)) {
        datatools::ui::base_command & comm = barInter.grab_command(cmd_name);
        datatools::command::returned_info cri = comm(argv);
        if (! cri.is_success()) {
          std::cerr << "error: command '" << cmd_name<< "' failed: " << std::endl;
          std::cerr << "error details:\n"
                    << cri.get_error_message()
                    << std::endl;
        }
      } else {
        std::cerr << "error: Unknown command: " << cmd_name << std::endl;
      }
      if (datatools::logger::is_debug(logging)) {
        std::cerr << "debug: current bar = " << bar << std::endl;
      }
    }
    std::clog << prompt << std::flush;
  }
  std::clog << "Bye!" << std::endl << std::endl;

  std::clog << "final bar = " << bar << std::endl;

  return;
}
