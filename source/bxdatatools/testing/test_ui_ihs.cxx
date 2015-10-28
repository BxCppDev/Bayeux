// test_ui_ihs.h
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
#include <datatools/ui/ihs.h>

// Standard Library:
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>

// Third Party:
// - Boost:
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/replace.hpp>

// This project:
#include <datatools/command_utils.h>
#include <datatools/ui/utils.h>
#include "ui/foo_command_interface.h"

//! \brief A dummy shell
class foo_shell
{
public:

  foo_shell(datatools::ui::ihs & ihs_) : _ihs_(&ihs_)
  {
    _cwi_ = ::datatools::ui::path::root_path();
    return;
  }

  std::string resolve_path(const std::string & name_) const
  {
    std::string full_path;
    if (name_.empty()) {
      // "" -> "/path/to/cwi"
      full_path = _cwi_;

    } else if (name_ == datatools::ui::path::root_path()) {
      // "/" -> ""
      full_path = name_;

    } else if (name_ == datatools::ui::path::current_interface_path()
               || name_ == (datatools::ui::path::current_interface_path() + datatools::ui::path::sep())) {

      // "." | "./" -> "/path/to/cwi"
      full_path = _cwi_;

    } else if (boost::starts_with(name_,
                                  datatools::ui::path::current_interface_path() + datatools::ui::path::sep())) {

      // "./foo/bar" -> "/path/to/cwi/foo/bar"
      full_path = boost::algorithm::replace_first_copy(name_,
                                                       datatools::ui::path::current_interface_path(),
                                                       _cwi_);

    } else if (name_ == datatools::ui::path::current_parent_interface_path()
               || name_ == (datatools::ui::path::current_parent_interface_path() + datatools::ui::path::sep())) {

      // ".." | "../" -> "/path/to"
      full_path = datatools::ui::path::parent_path(_cwi_);

    } else if (boost::starts_with(name_,
                                  datatools::ui::path::current_parent_interface_path() + datatools::ui::path::sep())) {

      // "../foo/bar" -> "/path/to/foo/bar"
      full_path = boost::algorithm::replace_first_copy(name_,
                                                       datatools::ui::path::current_parent_interface_path(),
                                                       datatools::ui::path::parent_path(_cwi_));

    } else if (datatools::ui::path::is_absolute_path(name_)) {

      // "/foo/bar" -> "/foo/bar"
      full_path = name_;

    } else {

      if (_cwi_ != datatools::ui::path::root_path()) {

        // "foo/bar" -> "/path/to/cwi/foo/bar"
        full_path = _cwi_ + "/" + name_;

      } else {

        // "foo/bar" -> "/foo/bar"
        full_path = _cwi_ + name_;

      }
    }
    full_path = datatools::ui::path::remove_trailing_sep(full_path);
    return full_path;
  }

  int cd(const std::string & path_)
  {
    std::string full_path = resolve_path(path_);
    full_path = datatools::ui::path::remove_trailing_sep(full_path);
    std::clog << "DEVEL: foo_shell::cd: : Full path = '" << full_path << "'" << std::endl;
    if (!_ihs_->exists(full_path)) {
      std::cerr << "foo_shell: error: '" << path_ << "' does not exists!" << std::endl;
      return 1;
    }
    if (!_ihs_->is_interface(full_path)) {
      std::cerr << "foo_shell: error: '" << path_ << "' is not an interface!" << std::endl;
      return 1;
    }
    _cwi_ = full_path;
    return 0;
  }

  int ls(const std::string & path_ = "")
  {
    std::string full_path = resolve_path(path_);
    std::vector<std::string> children;
    _ihs_->build_children_paths(full_path, children);
    for (int i = 0; i < (int) children.size(); i++) {
      std::string child_path = datatools::ui::path::basename(children[i]);
      std::cout << child_path;
      if (_ihs_->is_interface(children[i])) std::cout << datatools::ui::path::sep();
      std::cout << std::endl;
    }
    return 0;
  }

  int call(const std::string & path_, const std::vector<std::string> & args_)
  {
    std::string cmd_full_path = resolve_path(path_);
    if (!_ihs_->exists(cmd_full_path)) {
      std::cerr << "foo_shell: error: '" << path_ << "' does not exists!" << std::endl;
      return 1;
    }
    if (!_ihs_->is_command(cmd_full_path)) {
      std::cerr << "foo_shell: error: '" << path_ << "' is not a command!" << std::endl;
      return 1;
    }
    datatools::ui::base_command & cmd = _ihs_->grab_command(cmd_full_path);
    datatools::command::returned_info cri = cmd(args_);
    if (cri.is_failure()) {
      std::cerr << "foo_shell: error: Command '" << path_ << "' failed: "
                << cri.get_error_message()
                << std::endl;
      return cri.get_error_code();
    }
    return 0;
  }

  void run();

private:

  std::string          _cwi_; //!< Current working interface
  datatools::ui::ihs * _ihs_; //!< Handle to the interface hierarchical system

};

void foo_shell::run()
{
  std::string line;
  std::string prompt("test> ");
  std::clog << prompt << std::flush;
  while (std::getline(std::cin, line)) {
    std::vector<std::string> argv = boost::program_options::split_unix(line);
    if (argv.size()) {
      std::string cmd_name = argv[0];
      if (cmd_name == "quit") {
        break;
      } else if (cmd_name == "ls") {
        std::string path;
        if (argv.size() == 2) {
          path = argv[1];
        }
        int error = ls(path);
        if (error) {
          std::cerr << "error: Cannot list the current interface from path '" << path << "'!" << std::endl;
        }
      } else if (cmd_name == "pwd") {
        std::cout << _cwi_ << std::endl;
      } else if (cmd_name == "cd") {
        if (argv.size() != 2) {
          std::cerr << "error: Missing path!" << std::endl;
        } else {
          std::string path = argv[1];
          int error = cd(path);
          if (error) {
            std::cerr << "error: Cannot change interface!" << std::endl;
          }
        }
      } else {
        std::vector<std::string> args;
        args = argv;
        args.erase(args.begin());
        int error = call(cmd_name, args);
        if (error) {
          std::cerr << "error: Failed command '" << cmd_name << "'!" << std::endl;
        }
      }
    }
    std::clog << prompt << std::flush;
  }
  std::clog << "Bye!" << std::endl << std::endl;

  return;
}

int main(int /* argc_ */, char * /* argv_ */[])
{
  try {

    // The objects:
    foo bar;
    foo joe;

    // The command interfaces:
    foo_command_interface barCmdInter(bar, "bar", "The bar object interface");
    barCmdInter.initialize_simple();
    barCmdInter.tree_dump(std::clog, "Bar command interface: ");

    foo_command_interface joeCmdInter(joe, "joe", "The joe object interface");
    joeCmdInter.initialize_simple();
    joeCmdInter.tree_dump(std::clog, "Joe command interface: ");
    std::clog << std::endl;

    // The interface hierarchical system:
    datatools::ui::ihs testIHS;
    testIHS.set_name("test");
    testIHS.set_display_name("Test");
    testIHS.set_terse_description("A test interface hierarchical system (IHS)");
    testIHS.set_logging_priority(datatools::logger::PRIO_TRACE);
    testIHS.tree_dump(std::clog, testIHS.get_display_name());
    std::clog << std::endl;

    testIHS.add_interface("/", "test");
    testIHS.add_interface("/test", "dummy");
    testIHS.add_interface("/test", "bidule");
    testIHS.add_interface("/test/bidule", "machin");
    testIHS.add_interface("/", barCmdInter);
    testIHS.add_interface("/", joeCmdInter);
    testIHS.add_interface("/bar", "truc");
    testIHS.add_interface("/bar", "chose");
    testIHS.tree_dump(std::clog, testIHS.get_display_name());
    testIHS.remove_command("/joe/test");
    testIHS.tree_dump(std::clog, testIHS.get_display_name());
    std::clog << std::endl;

    testIHS.remove_interface("/joe", true);
    testIHS.tree_dump(std::clog, testIHS.get_display_name());
    std::clog << std::endl;

    testIHS.add_interface("/test/bidule", joeCmdInter);
    testIHS.tree_dump(std::clog, testIHS.get_display_name());
    std::clog << std::endl;

    foo_shell testShell(testIHS);
    testShell.run();

  }
  catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
