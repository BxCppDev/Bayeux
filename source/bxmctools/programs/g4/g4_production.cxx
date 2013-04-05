// -*- mode: c++ ; -*-
// g4_production.cxx
/*
 * Description :
 *
 *   The main mctools Geant4 simulation program.
 *
 * Licence :
 *
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <exception>

#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/multi_properties.h>

#include <mygsl/random_utils.h>

#include <mctools/g4/manager.h>
#include <mctools/g4/manager_parameters.h>

#include <boost/program_options.hpp>

// For Boost I/O :
// Some pre-processor guard about Boost I/O usage and linkage :
#include <datatools/bio_guard.h>
#include <mygsl/bio_guard.h>
#include <geomtools/bio_guard.h>
#include <genbb_help/bio_guard.h>
#include <mctools/bio_guard.h>

int main(int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << datatools::io::notice
              << "g4_production: "
              << "mctools Geant4 program for simulation production." << std::endl;

    // 2011-07-22 XG: most of these parameters can be overwritten by
    // the default_value method of options_description.
    mctools::g4::manager_parameters params;
    params.set_defaults();

    // Shortcut for Boost/program_options namespace :
    // See: http://www.boost.org/doc/libs/1_46_1/doc/html/program_options.html
    namespace po = boost::program_options;

    // Describe command line switches(-X, --XXX...) :
    po::options_description opts("Allowed options");

    opts.add_options()
      ("help,h", "produce help message")

      ("debug,d",
       po::value<bool>(&params.debug)
       ->zero_tokens()
       ->default_value(false),
       "produce debug logging")

      ("verbose,v",
       po::value<bool>(&params.verbose)
       ->zero_tokens()
       ->default_value(false),
       "produce verbose logging")

      ("number-of-events,n",
       po::value<uint32_t>(&params.noevents)
       ->default_value(mctools::g4::manager::constants::instance().NO_LIMIT),
       "set the number of events to be simulated")

      ("number-of-events-modulo,m",
       po::value<uint32_t>(&params.noevents_modulo)
       ->default_value(0),
       "set the event print period(0 means no print)")

      ("interactive,i",
       po::value<bool>(&params.interactive)
       ->zero_tokens()
       ->default_value(false),   
       "run simulation in interactive mode")

      ("batch,b",
       po::value<bool>()
       ->zero_tokens()
       ->default_value(false),
       "run simulation in batch mode")

      ("config,c",
       po::value<std::string>(),
       "set the manager configuration file")

      ("vertex-generator-name",
       po::value<std::string>(),
       "set the name of the vertex generator")

      ("vertex-generator-seed",
       po::value<int>(&params.vg_seed)
       ->default_value(0),
       "set the seed of the vertex generator random number generator")

      ("event-generator-name",
       po::value<std::string>(),
       "set the name of the event generator")

      ("event-generator-seed",
       po::value<int>(&params.eg_seed)
       ->default_value(0),
       "set the seed of the event generator random number generator")

      ("shpf-seed",
       po::value<int>(&params.shpf_seed)
       ->default_value(0),
       "set the seed of the step hit processors random number generator")

      ("g4-manager-seed",
       po::value<int>(&params.mgr_seed)
       ->default_value(0),
       "set the seed of the G4 manager random number generator")

      ("input-prng-seeds-file",
       po::value<std::string>(),
       "set the file from which to load the seeds of the random number generators")

      ("output-prng-seeds-file",
       po::value<std::string>(),
       "set the file to store the seeds of the random number generators")

      ("input-prng-states-file",
       po::value<std::string>(),
       "set the file from which to load the backuped internal states of the random number generators(PRNG)")

      ("output-prng-states-file",
       po::value<std::string>(),
       "set the file to store/backup the internal states of the random number generators(PRNG)")

      ("output-data-file",
       po::value<std::string>(),
       "set the file to store the generated simulated data")

      ("g4-visu",
       po::value<bool>(&params.g4_visu)
       ->zero_tokens()
       ->default_value(false),
       "G4 visualization(only in interactive mode)")

      ("no-g4-visu",
       po::value<bool>()
       ->zero_tokens()
       ->default_value(false),
       "no G4 visualization(only in interactive mode)")

      ("g4-macro",
       po::value<std::string>(),
       "set the G4 macro file(only if interactive G4 visualization)")

      ("using-time-statistics",
       po::value<bool>(&params.using_time_stat)
       ->zero_tokens()
       ->default_value(false),
       "print time statistics at the end of the run")

      ("forbid-private-hits",
       po::value<bool>(&params.forbid_private_hits)
       ->zero_tokens()
       ->default_value(false),
       "do not allow private(un-official)\nMC hits")

      ("dont-save-no-sensitive-hit-events",
       po::value<bool>(&params.dont_save_no_sensitive_hit_events)
       ->zero_tokens()
       ->default_value(false),
       "do not save events without sensitive\nMC hits")

      ("run-header-footer",
       po::value<bool>(&params.use_run_header_footer)
       ->zero_tokens()
       ->default_value(false),
       "store the run header and footer info\ntogether with the output data file")
      ; // end of options' description

    // Describe command line arguments :
    po::positional_options_description args;
    args.add("g4-macro", 1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc_, argv_)
      .options(opts)
              .positional(args)
              .run(), vm);
    po::notify(vm);

    /*** begin of opts/args parsing ***/

    // Fetch the opts/args :
    if (vm.count("help")) {
      std::cout << "g4_production -- A generic GEANT4 simulation program" << std::endl;
      std::cout << std::endl;
      std::cout << "Usage : " << std::endl;
      std::cout << std::endl;
      std::cout << "  g4_production [OPTIONS] [ARGUMENTS] "
                << std::endl;
      std::cout << std::endl;
      std::cout << opts << std::endl;
      std::cout << std::endl;
      mctools::g4::manager_parameters::print_examples(std::cout,
                                                      "g4_production",
                                                      "Examples : ");
      std::cout << std::endl;
      return(1);
    }

    if (vm.count("batch")) {
      if (vm["batch"].as<bool>()) {
        //std::cerr << std::endl << "********* DEVEL: BATCH !" << std::endl << std::endl;
        params.interactive = false;
      }
    }

    if (params.interactive) {
      if (vm.count("g4-visu")) {
        if (vm["g4-visu"].as<bool>()) {
          params.g4_visu = true;
        }
      }

      if (vm.count("no-g4-visu")) {
        if (vm["no-g4-visu"].as<bool>()) {
          params.g4_visu = false;
        }
      }
    }
    else {
      params.g4_visu = false;
    }

    if (vm.count("config")) {
      params.manager_config_filename = vm["config"].as<std::string>();
    }

    if (vm.count("vertex-generator-name")) {
      params.vg_name = vm["vertex-generator-name"].as<std::string>();
    }

    if (vm.count("event-generator-name")) {
      params.eg_name = vm["event-generator-name"].as<std::string>();
    }

    if (vm.count("input-prng-seeds-file")) {
      params.input_prng_seeds_file
        = vm["input-prng-seeds-file"].as<std::string>();
    }

    if (vm.count("output-prng-seeds-file")) {
      params.output_prng_seeds_file
        = vm["output-prng-seeds-file"].as<std::string>();
    }

    if (vm.count("input-prng-states-file")) {
      params.input_prng_states_file
        = vm["input-prng-states-file"].as<std::string>();
    }

    if (vm.count("output-prng-states-file")) {
      params.output_prng_states_file
        = vm["output-prng-states-file"].as<std::string>();
    }

    if (vm.count("output-data-file")) {
      params.output_data_file
        = vm["output-data-file"].as<std::string>();
    }

    if (vm.count("g4-macro")) {
      params.g4_macro
        = vm["g4-macro"].as<std::string>();
    }

    /*** end of opts/args parsing ***/

    // Declare the simulation manager:
    mctools::g4::manager sim_manager;

    std::clog << datatools::io::notice
              << "g4_production: " << "Setup the simulation manager..." << std::endl;

    mctools::g4::manager_parameters::setup(params, sim_manager);

    std::clog << datatools::io::notice
              << "g4_production: " << "Simulation session starts..." << std::endl;

    // Run the simulation session :
    sim_manager.run_simulation();

    std::clog << datatools::io::notice
              << "g4_production: " << "Simulation session is stopped." << std::endl;
  }
  catch (std::exception & x) {
    std::cerr << datatools::io::error
              << "g4_production: "
              << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << datatools::io::error
              << "g4_production: "
              << "Unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  std::clog << datatools::io::notice
            << "g4_production: g4_production ends here." << std::endl;
  return(error_code);
}

// end of g4_production.cxx
