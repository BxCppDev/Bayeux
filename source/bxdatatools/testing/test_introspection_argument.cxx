//! \file  datatools/introspection/testing/test-introspection-argument.cxx
//! \brief Datatools introspection method argument test
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of Datatools.
//
// Datatools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Datatools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Datatools. If not, see <http://www.gnu.org/licenses/>.

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux:
#include <bayeux/bayeux.h>
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/units.h>

// This project:
#include <datatools/datatools.h>
#include <datatools/introspection/argument.h>

void test_argument_1();
void test_argument_2();

int main(int /* argc_ */, char ** /* argv_ */)
{
  datatools::initialize();
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for the 'datatools::introspection::argument' class."
              << std::endl;

    test_argument_1();
    test_argument_2();

    std::clog << "The end." << std::endl;
  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  datatools::terminate();
  return (error_code);
}

void test_argument_1()
{
  std::clog << "\ntest_argument_1: basics" << std::endl;

  {
    datatools::introspection::argument arg;
    arg.set_access(datatools::introspection::ACCESS_INPUT);
    datatools::introspection::data_description & arg_dd = arg.grab_data_description();
    arg_dd.set_type(datatools::introspection::DATA_TYPE_INT32);
    arg_dd.set_layout(datatools::introspection::DATA_LAYOUT_SCALAR);
    arg.set_default_value_as<int32_t>(32);
    arg.tree_dump(std::clog, "Argument: ");

    if (arg.is_default_value_a<int32_t>()) {
      std::clog << "Default value is: "
                << std::dec << boost::any_cast<int32_t>(arg.get_default_value())
                << std::endl;
    }

    boost::any value = (int32_t) 17;
    std::clog << "Value is: "
              << std::dec << boost::any_cast<int32_t>(value)
              << std::endl;
    datatools::introspection::make_value(value,
                                         arg_dd.get_type(),
                                         arg_dd.get_layout_label());
    value = arg.get_default_value();
    if (value.type() == arg_dd.native_type()) {
      std::clog << "Value is now : "
                << std::dec << boost::any_cast<int32_t>(value)
                << std::endl;
    }

    datatools::properties arg_config;
    arg.export_to_config(arg_config, datatools::introspection::argument::ARG_XC_DEFAULT, "test.");
    arg_config.tree_dump(std::clog, "Argument configuration: ");
    std::clog << std::endl;

  }

  return;
}

void test_argument_2()
{
  std::clog << "\ntest_argument_2: configuration" << std::endl;

  {
    datatools::introspection::argument arg;

    datatools::properties arg_config;
    arg_config.store_string("name", "position");
    arg_config.store_integer("rank", 0);
    arg_config.store_string("description", "The 3D position of the detector");
    arg_config.store_string("access", "input");
    arg_config.store_string("data.layout", "vector_with_fixed_size");
    arg_config.store_integer("data.vector_fixed_size", 3);
    arg_config.store_string("data.type", "double");
    arg_config.store_string("data.unit.support", "explicit_unit_dimension");
    arg_config.store_string("data.unit.explicit_unit_dimension", "length");
    arg_config.store_string("data.unit.preferred_unit", "mm");
    arg_config.tree_dump(std::clog, "Argument primary configuration: ");
    std::clog << std::endl;

    arg.initialize(arg_config);
    arg.tree_dump(std::clog, "Argument: ");
    std::clog << std::endl;

    datatools::properties xarg_config;
    arg.export_to_config(xarg_config, datatools::introspection::argument::ARG_XC_DEFAULT);
    xarg_config.tree_dump(std::clog, "Argument exported configuration: ");
    std::clog << std::endl;
  }

  return;
}
