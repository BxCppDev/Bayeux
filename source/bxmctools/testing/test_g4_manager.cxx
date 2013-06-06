// -*- mode: c++ ; -*-
// test_g4_manager.cxx
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

#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/multi_properties.h>

#include <mctools/g4/manager.h>
#include <mctools/g4/manager_parameters.h>

int main (int argc_, char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << datatools::io::notice
           << "Test program for class 'mctools::g4::manager' !" << endl;

      bool debug = false;
      string manager_config_filename;
      string g4_macro;
      bool g4_visu = false;
      bool interactive = false;
      size_t noevents = 0;

      int iarg = 1;
      while (iarg < argc_) {
          string token = argv_[iarg];
          if (token[0] == '-') {
               string option = token;
               if ((option == "-d") || (option == "--debug")) {
                   debug = true;
                 }
               else if ((option == "-c") || (option == "--config")) {
                   iarg++;
                   manager_config_filename = argv_[iarg];
                 }
               else if ((option == "-v") || (option == "--visu")) {
                   g4_visu = true;
               }
               else if ((option == "-i") || (option == "--interactive")) {
                   interactive = true;
                 }
               else if ((option == "-b") || (option == "--batch")) {
                   interactive = false;
                 }
               else if (option == "-1") {
                   noevents = 1;
                 }
               else if (option == "-2") {
                   noevents = 2;
                 }
               else if (option == "-10") {
                   noevents = 10;
                 }
               else if (option == "-100") {
                   noevents = 100;
                 }
               else {
                 clog << datatools::io::warning
                      << "Ignoring option '" << option << "'!" << endl;
               }
            }
          else {
              string argument = token;
              if (g4_macro.empty ()) {
                  g4_macro = argument;
                }
              else {
                clog << datatools::io::warning
                     << "Ignoring argument '" << argument << "'!" << endl;
              }
          }
          iarg++;
      }


      if (noevents < 1) noevents = 3;

      if (manager_config_filename.empty ()) {
          manager_config_filename = "${MCTOOLS_DATA_DIR}/testing/config/g4/test-1.0/manager.conf";
          clog << datatools::io::warning
               << "Using default configuration file '"
               << manager_config_filename << "' !" << endl;
        }
      // datatools::fetch_path_with_env (manager_config_filename);
      // datatools::multi_properties configuration ("name",
      //                                            "",
      //                                            "Configuration for testing the mctools::g4::manager class");
      // configuration.read (manager_config_filename);
      // if (debug) configuration.tree_dump (clog,
      //                                     "Configuration for the G4 simulation manager",
      //                                     "DEBUG: ");

      // Setup parameters of the simulation manager :
      mctools::g4::manager_parameters params;
      params.set_defaults();
      if (debug) params.logging = "debug";
      params.vg_seed = 0;
      params.eg_seed = 0;
      params.shpf_seed = 0;
      params.mgr_seed = 0;
      params.g4_visu = g4_visu;
      params.using_time_stat = true;
      params.interactive = interactive;
      params.g4_macro = g4_macro;
      params.manager_config_filename = manager_config_filename;
      params.noevents = noevents;
      params.vg_name = "all_source_bulk.vg";
      params.eg_name = "bkg_K40";

      // Declare the simulation manager:
      mctools::g4::manager sim_manager;

      // Setup and initialize the simulation session :
      mctools::g4::manager_parameters::setup(params, sim_manager);

      if (debug) sim_manager.dump (clog);

      if (interactive) {
        clog << endl << "Hit [Enter] to continue..." << endl;
        string ans;
        getline (cin, ans);
      }

      // Run the simulation session :
      sim_manager.run_simulation();

      clog << "The end." << endl;
    }
  catch (exception & x) {
    cerr << datatools::io::error
         << x.what () << endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    cerr << datatools::io::error
           <<  "Unexpected error !" << endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

// end of test_g4_manager.cxx
