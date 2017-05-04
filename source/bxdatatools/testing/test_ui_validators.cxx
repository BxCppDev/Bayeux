// test_ui_validators.h
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>
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
#include <datatools/ui/validators.h>

// Standard Library:
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>

// Third party:
#include <boost/program_options.hpp>
#include <datatools/logger.h>
// #include <datatools/utils.h>
#include <datatools/ioutils.h>
// #include <datatools/units.h>

// Doc:
// http://stackoverflow.com/questions/4107087/accepting-negative-doubles-with-boostprogram-options
// http://stackoverflow.com/questions/2539077/boost-program-options-parsing-multiple-argument-list

void test_1(int argc_, char * argv_[]);

int main(int argc_, char * argv_[])
{
  try {
    test_1(argc_, argv_);
  } catch (std::exception & error) {
    DT_LOG_ERROR(datatools::logger::PRIO_ALWAYS, error.what());
    return EXIT_FAILURE;
  } catch (...) {
    DT_LOG_ERROR(datatools::logger::PRIO_ALWAYS, "Unexpected error!");
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

void test_1(int argc_, char * argv_[])
{
  namespace po = boost::program_options;
  try {
    po::options_description opts("Allowed options");
    opts.add_options()
      ("help", "produce a help screen")
      ("version", "print the version number")
      ("boolean", po::value<datatools::ui::wrapped_boolean>(),
       "boolean value (extended format)")
      ("integer", po::value<datatools::ui::wrapped_integer>(),
       "integer value (extended format)")
      ("real-without-unit", po::value<datatools::ui::wrapped_real_without_unit>(),
       "real value (extended format)")
      ("real-with-unit", po::value<datatools::ui::wrapped_real_with_unit>(),
       "real value with unit (extended format)")
      ;

    po::positional_options_description args;
    args.add("boolean", 1);
    args.add("integer", 1);
    args.add("real-without-unit", 1);
    args.add("real-with-unit", 1);

    po::variables_map vm;
    po::parsed_options parsed =
      po::command_line_parser(argc_, argv_)
      .options(opts)
      .positional(args)
      .style(po::command_line_style::unix_style ^ po::command_line_style::allow_short)
      // .allow_unregistered()
      .run();
    po::store(parsed, vm);
    po::notify(vm);

    if (vm.count("help")) {
      std::cout << "Usage: test_ui_validators [options]\n";
      std::cout << opts;
      return;
    }

    if (vm.count("version")) {
      std::cout << "Version 1.\n";
      return;
    }

    if (vm.count("boolean")) {
      bool b = vm["boolean"].as<datatools::ui::wrapped_boolean>().value;
      std::clog << "The boolean is [" << b << "]\n";
    }

    if (vm.count("integer")) {
      int i = vm["integer"].as<datatools::ui::wrapped_integer>().value;
      std::clog << "The integer is [" << i << "]\n";
    }

    if (vm.count("real-without-unit")) {
      double rwou = vm["real-without-unit"].as<datatools::ui::wrapped_real_without_unit>().value;
      std::clog << "The real without unit is [" << rwou << "]\n";
    }

    if (vm.count("real-with-unit")) {
      double rwu =
        vm["real-with-unit"].as<datatools::ui::wrapped_real_with_unit>().value;
      std::string unit_symbol =
        vm["real-with-unit"].as<datatools::ui::wrapped_real_with_unit>().unit_symbol;
      std::string unit_label =
        vm["real-with-unit"].as<datatools::ui::wrapped_real_with_unit>().unit_label;
      std::clog << "The real with unit is [" << rwu << "] and unit='" << unit_symbol << "' "
                << "of dimension '" << unit_label << "'\n";
    }

  } catch(std::exception& e) {
    std::cerr << e.what() << "\n";
  }

  return;
}
