//! \file  datatools/introspection/testing/test-introspection-method.cxx
//! \brief Datatools introspection method test
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
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/units.h>

// This project:
#include <datatools/datatools.h>
#include <datatools/introspection/method.h>

void test_method_1();
void test_method_2();
void test_method_3();

int main(int /* argc_ */, char ** /* argv_ */)
{
  DATATOOLS_INIT();
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for the 'datatools::introspection::method' class."
              << std::endl;

    test_method_1();
    test_method_2();
    test_method_3();

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
  DATATOOLS_FINI();
  return (error_code);
}

void test_method_1()
{
  std::clog << "\ntest_method_1: basics" << std::endl;

  {
    datatools::introspection::method meth;
    meth.set_name("getItem");
    meth.set_terse_description("Return the value of item with selection tag at given index");
    meth.set_constness(true);

    {
      datatools::introspection::argument index_arg;
      index_arg.set_name("index");
      index_arg.set_description("Short description of the 'index' input argument");
      index_arg.set_access(datatools::introspection::ACCESS_INPUT);
      datatools::introspection::data_description & index_arg_dd = index_arg.grab_data_description();
      index_arg_dd.set_type(datatools::introspection::DATA_TYPE_INT32);
      index_arg_dd.set_layout(datatools::introspection::DATA_LAYOUT_SCALAR);
      index_arg.tree_dump(std::clog, "Input argument: ");
      meth.add_argument(index_arg);
    }

    {
      datatools::introspection::argument tag_arg;
      tag_arg.set_name("tag");
      tag_arg.set_description("Short description of the 'tag' input argument");
      tag_arg.set_access(datatools::introspection::ACCESS_INPUT);
      datatools::introspection::data_description & tag_arg_dd = tag_arg.grab_data_description();
      tag_arg_dd.set_type(datatools::introspection::DATA_TYPE_STRING);
      tag_arg_dd.set_layout(datatools::introspection::DATA_LAYOUT_SCALAR);
      tag_arg.tree_dump(std::clog, "Input argument: ");
      meth.add_argument(tag_arg);
    }

    {
      datatools::introspection::argument value_arg;
      value_arg.set_name("return");
      value_arg.set_description("Short description of the returned value");
      value_arg.set_access(datatools::introspection::ACCESS_OUTPUT);
      datatools::introspection::data_description & value_arg_dd = value_arg.grab_data_description();
      value_arg_dd.set_type(datatools::introspection::DATA_TYPE_FLOAT);
      value_arg_dd.set_layout(datatools::introspection::DATA_LAYOUT_SCALAR);
      datatools::introspection::unit_info uinfo;
      uinfo.make_implicit_unit("V");
      value_arg_dd.set_unit_info(uinfo);
      value_arg.tree_dump(std::clog, "Returned argument: ");
      meth.add_argument(value_arg);
    }

    meth.tree_dump(std::clog, "Method: ");

    {
      std::vector<std::string> input_arg_names;
      meth.build_list_of_input_argument_names(input_arg_names);
      std::clog << "List of input arguments: " << std::endl;
      for (int i = 0; i < (int) input_arg_names.size(); i++) {
        const std::string & input_arg_name = input_arg_names[i];
        std::clog << "  Argument: '" << input_arg_name << "'" << std::endl;
      }
    }

    {
      std::vector<std::string> output_arg_names;
      meth.build_list_of_output_argument_names(output_arg_names);
      std::clog << "List of output arguments: " << std::endl;
      for (int i = 0; i < (int) output_arg_names.size(); i++) {
        const std::string & output_arg_name = output_arg_names[i];
        std::clog << "  Argument: '" << output_arg_name << "'" << std::endl;
      }
    }

    datatools::properties xmeth_config;
    meth.export_to_config(xmeth_config,
                          datatools::introspection::method::METHOD_XC_DEFAULT);
    xmeth_config.tree_dump(std::clog, "Method exported configuration: ");
    std::clog << std::endl;

    meth.reset();
    meth.tree_dump(std::clog, "Method (after reset): ");
    std::clog << std::endl;

    meth.initialize(xmeth_config);
    meth.tree_dump(std::clog, "Regenerated method: ");
    std::clog << std::endl;

  }

  return;
}

void test_method_2()
{
  std::clog << "\ntest_method_2: configuration" << std::endl;

  {
    datatools::introspection::method meth;

    datatools::properties meth_config;
    meth_config.store_string("name", "getItem");
    meth_config.store_string("terse_description",
                             "Return the value of item with selection tag at given index");
    meth_config.store_boolean("constness", true);
    std::vector<std::string> arg_names;
    arg_names.push_back("index");
    arg_names.push_back("tag");
    arg_names.push_back("return");
    meth_config.store("arguments", arg_names);

    meth_config.store_string("arguments.index.access",      "input");
    meth_config.store_string("arguments.index.data.type",   "int32");
    meth_config.store_string("arguments.index.data.layout", "scalar");
    meth_config.store_string("arguments.index.description", "Index in some array");

    meth_config.store_string("arguments.tag.access",      "input");
    meth_config.store_string("arguments.tag.data.type",   "string");
    meth_config.store_string("arguments.tag.data.layout", "scalar");
    meth_config.store_string("arguments.tag.description", "A tag");

    meth_config.store_string("arguments.return.access",             "output");
    meth_config.store_string("arguments.return.data.type",          "float");
    meth_config.store_string("arguments.return.data.layout",        "scalar");
    meth_config.store_string("arguments.return.data.unit.support",  "implicit_unit");
    meth_config.store_string("arguments.return.data.unit.implicit_unit", "V");
    meth_config.tree_dump(std::clog, "Method primary configuration: ");
    std::clog << std::endl;

    meth.initialize(meth_config);
    meth.tree_dump(std::clog, "Method: ");
    std::clog << std::endl;

    datatools::properties xmeth_config;
    meth.export_to_config(xmeth_config,
                          datatools::introspection::method::METHOD_XC_DEFAULT);
    xmeth_config.tree_dump(std::clog, "Method exported configuration: ");
    std::clog << std::endl;

    meth.reset();
    meth.tree_dump(std::clog, "Method (after reset): ");
    std::clog << std::endl;

    meth.initialize(xmeth_config);
    meth.tree_dump(std::clog, "Regenerated method: ");
    std::clog << std::endl;

  }

  {
    datatools::introspection::method meth;

    datatools::properties meth_config;
    meth_config.store_string("name", "get");
    meth_config.store_string("terse_description", "Return the value");
    meth_config.store_boolean("constness", true);
    meth_config.store_string("type_id", "my::class");
    std::vector<std::string> arg_names;
    arg_names.push_back("return");
    meth_config.store("arguments", arg_names);

    meth_config.store_string("arguments.return.access",            "output");
    meth_config.store_string("arguments.return.description",       "The pressure");
    meth_config.store_string("arguments.return.data.type",         "float");
    meth_config.store_string("arguments.return.data.layout",       "scalar");
    meth_config.store_string("arguments.return.data.unit.support", "implicit_unit");
    meth_config.store_string("arguments.return.data.unit.implicit_unit", "mbar");
    meth_config.tree_dump(std::clog, "Method primary configuration: ");
    std::clog << std::endl;

    meth.initialize(meth_config);
    meth.tree_dump(std::clog, "Method: ");
    std::clog << std::endl;

    datatools::properties xmeth_config;
    meth.export_to_config(xmeth_config, datatools::introspection::argument::ARG_XC_DEFAULT);
    xmeth_config.tree_dump(std::clog, "Method exported configuration: ");
    std::clog << std::endl;
  }

  return;
}

void test_method_3()
{
  std::clog << "\ntest_method_3: makers..." << std::endl;

  {
    datatools::introspection::method meth;
    meth.make_scalar_setter("pressure",
                            datatools::introspection::DATA_TYPE_DOUBLE,
                            "mbar",
                            "mbar",
                            "Set the pressure (in implicit pressure unit)",
                            "set_pressure");
    meth.tree_dump(std::clog, "Setter method: ");
  }

  {
    datatools::introspection::method meth;
    meth.make_scalar_getter("pressure",
                            datatools::introspection::DATA_TYPE_DOUBLE,
                            "pressure",
                            "mbar",
                            "Get the pressure (in explicit pressure unit)",
                            "get_pressure");
    meth.tree_dump(std::clog, "Getter method: ");
  }

  return;
}
