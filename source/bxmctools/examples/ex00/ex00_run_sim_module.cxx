// ex00_run_sim_module.cxx
/* Copyright (C) 2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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

// Standard library:
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

// Third party:
// - Boost:
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/foreach.hpp>
#include <boost/scoped_ptr.hpp>
// - Bayeux:
#include <bayeux/bayeux.h>
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/things.h>
#include <datatools/library_loader.h>
// - Bayeux/geomtools:
#include <geomtools/manager.h>
// - Bayeux/dpp:
#include <dpp/output_module.h>
// - Bayeux/mctools:
#include <mctools/g4/simulation_module.h>

int main(int argc_, char **argv_)
{
  bayeux::initialize(argc_, argv_, 0);

  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  int error_code = EXIT_SUCCESS;
  try {
    std::string  geometry_config_filename;
    std::string  simulation_config_filename;
    std::string  output_filename;
    unsigned int number_of_events = 10;

    // Parse options:
    namespace po = boost::program_options;
    po::options_description opts("Allowed options");
    opts.add_options()
      ("help,h", "produce help message")
      ("geometry-config,g",
       po::value<std::string>(&geometry_config_filename),
       "set the main configuration file of the geometry manager")
      ("simulation-config,g",
       po::value<std::string>(&simulation_config_filename),
       "set the main configuration file of the simulation manager")
      ("output-file,o",
       po::value<std::string>(&output_filename),
       "set the output filename")
      ("number-of-events,n",
       po::value<unsigned int>(&number_of_events),
       "set the number of events")
      ; // end of options description

    // Describe command line arguments :
    po::variables_map vm;
    po::store(po::command_line_parser(argc_, argv_)
              .options(opts)
              .run(), vm);
    po::notify(vm);

    // Use command line arguments :
    if (vm.count("help")) {
      std::cout << "Usage : " << std::endl;
      std::cout << opts << std::endl;
      return(1);
    }

    if (geometry_config_filename.empty()) {
      geometry_config_filename = "${CONFIG_DIR}/geometry/manager.conf";
    }

    if (simulation_config_filename.empty()) {
      simulation_config_filename = "${CONFIG_DIR}/simulation/manager.conf";
    }

    // Configure the geometry manager:
    geomtools::manager geo_mgr;
    datatools::fetch_path_with_env(geometry_config_filename);
    datatools::properties geo_mgr_config;
    datatools::read_config(geometry_config_filename, geo_mgr_config);
    geo_mgr.initialize(geo_mgr_config);

    // Setup the simulation module:
    mctools::g4::manager_parameters sim_mod_params;
    sim_mod_params.logging = "warning";
    sim_mod_params.manager_config_filename = simulation_config_filename;
    sim_mod_params.mgr_seed  = 11111; // Geant4 PRNG seed
    sim_mod_params.vg_seed   = 22222; // Vertex generator PRNG seed
    sim_mod_params.eg_seed   = 33333; // Event generator PRNG seed
    sim_mod_params.shpf_seed = 44444; // Hit post-processing PRNG seed
    sim_mod_params.eg_name   = "electron_1MeV_cone";
    sim_mod_params.vg_name   = "source_bulk.vg";
    // sim_mod_params.input_prng_seeds_file   = "prng_seeds.conf";
    // sim_mod_params.input_prng_states_file  = "prng_states.conf";
    sim_mod_params.output_prng_seeds_file  = "prng_seeds.save";
    sim_mod_params.output_prng_states_file = "prng_states.save";
    sim_mod_params.number_of_events_modulo = 1;
    mctools::g4::simulation_module sim_mod;
    sim_mod.set_name("SimMod"); // For convenience
    sim_mod.set_sd_label("SD");
    // sim_mod.set_geo_label("Geo"); // Not used here because of the Geometry manager above
    sim_mod.set_geometry_manager(geo_mgr);
    sim_mod.set_geant4_parameters(sim_mod_params);
    sim_mod.initialize_simple();

    // Setup the output module:
    boost::scoped_ptr<dpp::output_module> out_mod;
    if (! output_filename.empty()) {
      out_mod.reset(new dpp::output_module);
      out_mod->set_name("OutMod"); // For convenience
      out_mod->set_single_output_file(output_filename);
      out_mod->initialize_simple();
    }

    // Current work event record:
    datatools::things event_record;

    // Main loop:
    unsigned int event_count = 0;
    while (event_count++ < number_of_events) {
      // Clear the event at each new loop:
      event_record.clear();
      // Generate the simulated data in the event record:
      dpp::base_module::process_status status = sim_mod.process(event_record);
      if (status != dpp::base_module::PROCESS_OK) {
        DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Simulation module failed !");
        break;
      }
      // Print the event record:
      // event_record.tree_dump(std::clog, "Event: ", "DEBUG: ");
      if (out_mod) {
        // Save the event record:
        status = out_mod->process(event_record);
        if (status != dpp::base_module::PROCESS_OK) {
          DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Output module failed !");
          break;
        }
      }
    }

    if (out_mod) {
      // Terminate the output module:
      out_mod->reset();
    }
    // Terminate the simulation module:
    sim_mod.reset();
  }
  catch (std::exception& x) {
    DT_LOG_FATAL(logging, x.what());
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    DT_LOG_FATAL(logging, "Unexpected error !");
    error_code = EXIT_FAILURE;
  }

  bayeux::terminate();
  return error_code;
}
