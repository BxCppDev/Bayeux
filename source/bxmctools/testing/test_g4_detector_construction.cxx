// test_g4_detector_construction.cxx
/*
 * Copyright (C) 2011-2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>

// This project:
#include <mctools/g4/detector_construction.h>

void test_1(bool debug_);

int main (int argc_, char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'mctools::g4::detector_construction'!" << std::endl;

    bool debug = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];
      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-d") || (option == "--debug")) {
          debug = true;
        } else {
          std::clog << "warning: ignoring option '" << option << "'!" << std::endl;
        }
      } else {
        std::string argument = token;
        {
          std::clog << "warning: ignoring argument '" << argument << "'!" << std::endl;
        }
      }
      iarg++;
    }

    test_1(debug);

    std::clog << "The end." << std::endl;

  } catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

void test_1(bool debug_)
{
  std::clog << "\ntest_1: Entering..." << std::endl;

  // Geometry manager :
  geomtools::manager geo_manager;
  if (debug_) geo_manager.set_logging_priority(datatools::logger::PRIO_DEBUG);
  datatools::properties gm_setup;
  std::string gm_filename = "${MCTOOLS_TESTING_DIR}/config/g4/test-2.0/geometry/manager.conf";
  datatools::fetch_path_with_env(gm_filename);
  datatools::read_config(gm_filename, gm_setup);
  geo_manager.initialize (gm_setup);

  // Additional plugins :
  // std::string mag_field_plugin_file="${MCTOOLS_TESTING_DIR}/config/fields/test_emfield_geom_plugin.conf";
  // datatools::fetch_path_with_env(mag_field_plugin_file);
  // datatools::multi_properties mag_field_plugin_config;
  // mag_field_plugin_config.read(mag_field_plugin_file);
  // geo_manager.load_plugins(mag_field_plugin_config);

  {
    std::clog << "\ntest_1: Entering Geant4 detector construction block..." << std::endl;

    // Geant4 detector construction :

    mctools::g4::detector_construction my_detector_construction;
    my_detector_construction.set_geometry_manager(geo_manager);
    datatools::properties dc_setup;
    dc_setup.store("logging.priority", "trace");
    dc_setup.store("gdml.tmp_dir", ".");
    dc_setup.store("gdml.no_validation", false);
    dc_setup.store("gdml.schema_location", "local");
    dc_setup.store("materials.plugin_name", "materials_driver");
    dc_setup.store("hit_processor_factory.config",
                   "${MCTOOLS_TESTING_DIR}/config/g4/test-2.0/simulation/step_hit_processor_factory.conf");
    std::cerr << std::endl << "******************* INITIALIZE *************" << std::endl;
    my_detector_construction.initialize(dc_setup);
    my_detector_construction.Construct();
    my_detector_construction.reset();

    std::clog << "\ntest_1: Exiting Geant4 detector construction block...." << std::endl;
  }
  std::clog << "\ntest_1: Geant4 detector construction block is closed." << std::endl;

  std::clog << "\ntest_1: Exiting." << std::endl;
  return;
}
