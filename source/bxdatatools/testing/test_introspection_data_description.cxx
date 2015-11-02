//! \file  datatools/introspection/testing/test-introspection-data_description.cxx
//! \brief Datatools introspection data description test
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
#include <datatools/introspection/data_description.h>

void test_data_description_1();
void test_data_description_2();

int main(int /* argc_ */, char ** /* argv_ */)
{
  DATATOOLS_INIT();
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for the 'datatools::introspection::data_description' class."
              << std::endl;

    test_data_description_1();
    test_data_description_2();

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

void test_data_description_1()
{
  std::clog << "\ntest_data_description_1: basics" << std::endl;

  {
    datatools::introspection::data_description dd;
    dd.set_type(datatools::introspection::DATA_TYPE_INT32);
    dd.set_layout(datatools::introspection::DATA_LAYOUT_SCALAR);
    dd.tree_dump(std::clog, "Data description: ");
    std::clog << std::endl;
  }

  datatools::units::registry::const_system_registry().tree_dump(std::clog,
                                                                "Unit registry: ");
  std::clog << std::endl;

  try {
    datatools::introspection::data_description dd;
    dd.set_type(datatools::introspection::DATA_TYPE_FLOAT_WITH_IMPLICIT_UNIT);
    dd.set_layout(datatools::introspection::DATA_LAYOUT_SCALAR);
    dd.set_implicit_unit_symbol("kilopotato");
  } catch (std::exception & error) {
    DT_LOG_ERROR(datatools::logger::PRIO_ALWAYS,
                 "As expected, data description failed due to invalid implicit unit!");
  }
  std::clog << std::endl;

  {
    datatools::introspection::data_description dd;
    dd.set_type(datatools::introspection::DATA_TYPE_FLOAT_WITH_IMPLICIT_UNIT);
    dd.set_layout(datatools::introspection::DATA_LAYOUT_SCALAR);
    dd.set_implicit_unit_symbol("volt");
    dd.tree_dump(std::clog, "Data description: ");
    std::clog << std::endl;
  }

  try {
    datatools::introspection::data_description dd;
    dd.set_type(datatools::introspection::DATA_TYPE_FLOAT_WITH_EXPLICIT_UNIT);
    dd.set_layout(datatools::introspection::DATA_LAYOUT_SCALAR);
    dd.set_explicit_unit_dimension_label("solar_neutrino_unit");
  } catch (std::exception & error) {
    DT_LOG_ERROR(datatools::logger::PRIO_ALWAYS,
                 "As expected, data description failed due to invalid explicit unit dimension!");
  }
  std::clog << std::endl;

  {
    datatools::introspection::data_description dd;
    dd.set_type(datatools::introspection::DATA_TYPE_FLOAT_WITH_EXPLICIT_UNIT);
    dd.set_layout(datatools::introspection::DATA_LAYOUT_VECTOR_WITH_FREE_SIZE);
    // dd.set_explicit_unit_dimension_label("pressure");
    // dd.set_explicit_unit_dimension_label("temperature");
    dd.set_explicit_preferred_unit_symbol("mbar");
    dd.tree_dump(std::clog, "Data description: ");
    std::clog << std::endl;
  }

  return;
}

void test_data_description_2()
{
  std::clog << "\ntest_data_description_2: configuration " << std::endl;

  {
    datatools::introspection::data_description dd;

    datatools::properties dd_config;
    dd_config.store_string("type", "double_with_explicit_unit");
    dd_config.store_string("explicit_unit_dimension", "length");
    dd_config.store_string("layout", "vector_with_fixed_size");
    dd_config.store_integer("vector_fixed_size", 3);

    dd.initialize(dd_config);
    dd.tree_dump(std::clog, "Data description: ");
    std::clog << std::endl;
  }

  return;
}
