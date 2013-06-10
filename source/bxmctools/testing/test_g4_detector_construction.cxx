// -*- mode: c++ ; -*-
// test_g4_detector_construction.cxx
/*
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <datatools/properties.h>
#include <mctools/g4/detector_construction.h>
#include <mctools/g4/manager.h>

int main (int argc_, char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    clog << "Test program for class 'mctools::g4::detector_construction'!" << endl;

    bool debug = false;

    int iarg = 1;
    while (iarg < argc_) {
      string token = argv_[iarg];
      if (token[0] == '-') {
        string option = token;
        if ((option == "-d") || (option == "--debug")) {
          debug = true;
        } else {
          clog << "warning: ignoring option '" << option << "'!" << endl;
        }
      } else {
        string argument = token;
        {
          clog << "warning: ignoring argument '" << argument << "'!" << endl;
        }
      }
      iarg++;
    }

    // Geometry manager :
    geomtools::manager geo_manager;
    if (debug) geo_manager.set_logging_priority (datatools::logger::PRIO_DEBUG);
    datatools::properties gm_setup;
    std::string gm_filename = "${MCTOOLS_DATA_DIR}/testing/config/g4/test-1.0/geometry/manager.conf";
    datatools::fetch_path_with_env(gm_filename);
    datatools::properties::read_config(gm_filename, gm_setup);
    geo_manager.initialize (gm_setup);

    // Additional plugins :
    std::string mag_field_plugin_file="${MCTOOLS_DATA_DIR}/testing/config/fields/test_emfield_geom_plugin.conf";
    datatools::fetch_path_with_env(mag_field_plugin_file);
    datatools::multi_properties mag_field_plugin_config;
    mag_field_plugin_config.read(mag_field_plugin_file);
    geo_manager.load_plugins(mag_field_plugin_config);

    // Geant4 detector construction :
    mctools::g4::manager my_g4_manager;
    mctools::g4::detector_construction my_detector_construction (my_g4_manager);
    my_detector_construction.set_geometry_manager(geo_manager);
    datatools::properties dc_setup;
    dc_setup.store("gdml.tmp_dir", ".");
    dc_setup.store("gdml.no_validation", false);
    dc_setup.store("gdml.schema_location", "local");
    dc_setup.store("materials.plugin_name", "materials_driver");
    dc_setup.store("hit_processor_factory.config",
                   "${MCTOOLS_DATA_DIR}/testing/config/g4/test-1.0/step_hit_processor_factory.conf");
    my_detector_construction.initialize(dc_setup);
    G4VPhysicalVolume * mother = my_detector_construction.Construct ();

  }
  catch (exception & x) {
    cerr << "error: " << x.what () << endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    cerr << "error: " << "unexpected error!" << endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

// end of test_g4_detector_construction.cxx
