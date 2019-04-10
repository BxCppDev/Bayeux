// -*- mode: c++ ; -*-
// test_g4_manager.cxx
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

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <signal.h>

#include <bayeux.h>
#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/multi_properties.h>

#include <mctools/g4/manager.h>
#include <mctools/g4/manager_parameters.h>

// #ifdef G4VIS_USE
// #pragma message("NOTE: G4VIS_USE is set.)
// #else
// #pragma message("NOTE: G4VIS_USE is NOT set'.")
// #endif // G4VIS_USE

void signal_callback_handler(int signum)
{
  std::cerr << "signal: Caught signal " << signum << std::endl;

  // Cleanup and close up stuff here...

  // Terminate program
  exit(signum);
}

struct params {
  bool debug = false;
  std::string manager_config_filename;
  std::string g4_macro;
  bool g4_visu = false;
  bool interactive = false;
  size_t number_of_events = 0;
};

void test_1(const params & pars_);

int main (int argc_, char ** argv_)
{
  bayeux::initialize(argc_, argv_);
  // Register signal and signal handler
  // signal(SIGABRT, signal_callback_handler);

  //using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << datatools::io::notice
         << "Test program for class 'mctools::g4::manager' !" << std::endl;

    params pars;

    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];
      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-d") || (option == "--debug")) {
          pars.debug = true;
        } else if ((option == "-c") || (option == "--config")) {
          iarg++;
          pars.manager_config_filename = argv_[iarg];
        } else if ((option == "-v") || (option == "--visu")) {
          pars.g4_visu = true;
        } else if ((option == "-i") || (option == "--interactive")) {
          pars.interactive = true;
        } else if ((option == "-b") || (option == "--batch")) {
          pars.interactive = false;
        } else if (option == "-1") {
          pars.number_of_events = 1;
        } else if (option == "-2") {
          pars.number_of_events = 2;
        } else if (option == "-10") {
          pars.number_of_events = 10;
        } else if (option == "-100") {
          pars.number_of_events = 100;
        } else {
          std::clog << datatools::io::warning
               << "Ignoring option '" << option << "'!" << std::endl;
        }
      } else {
        std::string argument = token;
        if (pars.g4_macro.empty ()) {
          pars.g4_macro = argument;
        } else {
          std::clog << datatools::io::warning
               << "Ignoring argument '" << argument << "'!" << std::endl;
        }
      }
      iarg++;
    }

    if (pars.number_of_events < 1) pars.number_of_events = 3;

    if (pars.manager_config_filename.empty()) {
      pars.manager_config_filename = "${MCTOOLS_TESTING_DIR}/config/g4/test-2.0/simulation/manager.conf";
      std::clog << datatools::io::warning
           << "Using default configuration file '"
           << pars.manager_config_filename << "' !" << std::endl;
    }
    if (pars.g4_macro.empty()) {
      pars.g4_macro = "${MCTOOLS_TESTING_DIR}/config/g4/test-2.0/simulation/geant4_visualization.mac";
    }

    test_1(pars);

    std::clog << "The end." << std::endl;
  } catch (std::exception & x) {
    std::cerr << datatools::io::error
         << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << datatools::io::error
         <<  "Unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  bayeux::terminate();
  return (error_code);
}

void test_1(const params & pars_)
{
  std::clog << "\ntest_1: Entering...." << std::endl;

  // Declare the simulation manager:
  mctools::g4::manager sim_manager;

  // Setup parameters of the simulation manager :
  mctools::g4::manager_parameters params;
  params.set_defaults();
  if (pars_.debug) params.logging =  "debug";
  params.vg_seed = 0;
  params.eg_seed = 0;
  params.shpf_seed = 0;
  params.mgr_seed = 0;
  params.g4_visu = pars_.g4_visu;
  params.using_time_stat = true;
  params.interactive = pars_.interactive;
  params.g4_macro = pars_.g4_macro;
  params.manager_config_filename = pars_.manager_config_filename;
  params.number_of_events = pars_.number_of_events;
  params.vg_name = "source_bulk.vg";
  params.eg_name = "electron_1MeV";

  // Setup and initialize the simulation session :
  mctools::g4::manager_parameters::setup(params, sim_manager);

  if (pars_.debug) sim_manager.dump (std::clog);

  if (pars_.interactive) {
    std::clog << std::endl << "Hit [Enter] to continue..." << std::endl;
    std::string ans;
    std::getline(std::cin, ans);
  }

  // Run the simulation session :
  sim_manager.run_simulation();
  sim_manager.reset();

  if (sim_manager.is_initialized()) {
    sim_manager.reset();
  }
  std::clog << "test_1: Exiting...." << std::endl;
  return;
}
