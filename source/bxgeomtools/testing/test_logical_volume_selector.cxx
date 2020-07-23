// test_logical_volume_selector.cxx
/*
 * Copyright 2020 F. Mauger
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

// Ourselves:
#include <geomtools/logical_volume_selector.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>


// Third party:
// - Boost:
#include <boost/shared_ptr.hpp>
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/utils.h>
#include <datatools/exception.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>

// This project:
#include <geomtools/gnuplot_drawer.h>
#include <geomtools/manager.h>

/*****************************************************/
int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'geomtools::logical_volume_selector' !" << std::endl;
    bool debug = false;
    // parse command line argument:
    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];
      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-d") || (option == "--debug")) {
          debug = true;
        }
      } else {
      }
      iarg++;
    }

    std::string manager_config_file = "${GEOMTOOLS_TESTING_DIR}/config/test-1.0/test_manager.conf";
    datatools::fetch_path_with_env (manager_config_file);
    datatools::properties manager_config;
    datatools::properties::read_config(manager_config_file, manager_config);
    geomtools::manager geo_mgr;
    geo_mgr.initialize (manager_config);
    if (debug) {
      geo_mgr.get_factory().tree_dump(std::clog, "The embedded geometry model factory : ");
      geo_mgr.get_id_mgr().tree_dump(std::clog, "The embedded GID manager : ");
      std::clog << "The embedded GID mapping : " << std::endl;
      geo_mgr.get_mapping().dump_dictionnary(std::clog);     
    }

    geomtools::logical_volume_selector logVolSelector(geo_mgr);
    logVolSelector.add_by_name("scintillator_block.model.log");
    logVolSelector.add_by_name("brick.model.log");
    logVolSelector.add_by_name_regex("(.*)brick(.*).log");
    logVolSelector.add_by_name_regex("source(.*).log");
    logVolSelector.exclude_by_name("source_chain.model.log");
    std::set<std::string> logVols;
    logVolSelector.resolve(logVols);
    std::clog << "Selected logical volumes:\n";
    for (auto logName : logVols) {
      std::clog << logName << '\n';
    }
 
  } catch (std::exception & x) {
    std::cerr << "ERROR: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "ERROR: " << "Unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}
