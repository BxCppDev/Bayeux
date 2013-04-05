// -*- mode: c++ ; -*- 
// test_manager.cxx 
/* 
 * Copyright (C) 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
  
#include <datatools/utils/utils.h>
#include <datatools/utils/ioutils.h>
#include <datatools/utils/multi_properties.h>
 
#include <sng4/manager.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << datatools::utils::io::notice 
           << "Test program for class 'snemo::g4::manager' !" << endl; 
  
      bool debug = false;
      bool verbose = false;
      string manager_config_filename;
      string g4_macro;
      bool g4_visu = false;
      bool interactive = false;
      size_t noevents = 0;

      int iarg = 1;
      while (iarg < argc_)
        {
          string token = argv_[iarg];

          if (token[0] == '-')
            {
               string option = token; 
               if ((option == "-d") || (option == "--debug")) 
                 {
                   debug = true;
                 }
               else if ((option == "-c") || (option == "--config")) 
                 {
                   iarg++; 
                   manager_config_filename = argv_[iarg];
                 }
               else if ((option == "-V") || (option == "--verbose")) 
                 {
                   verbose = true;
                 }
               else if ((option == "-v") || (option == "--visu")) 
                 {
                   g4_visu = true;
                 }
               else if ((option == "-i") || (option == "--interactive")) 
                 {
                   interactive = true;
                 }
               else if ((option == "-b") || (option == "--batch")) 
                 {
                   interactive = false;
                 }
               else if (option == "-1") 
                 {
                   noevents = 1;
                 }
               else if (option == "-2") 
                 {
                   noevents = 2;
                 }
               else if (option == "-10") 
                 {
                   noevents = 10;
                 }
               else if (option == "-100") 
                 {
                   noevents = 100;
                 }
               else 
                 { 
                    clog << datatools::utils::io::warning 
                         << "Ignoring option '" << option << "'!" << endl; 
                 }
            }
          else
            {
              string argument = token; 
              if (g4_macro.empty ())
                {
                  g4_macro = argument;
                }
              else
                { 
                  clog << datatools::utils::io::warning 
                       << "Ignoring argument '" << argument << "'!" << endl;
                }
            }
          iarg++;
      }

      if (noevents < 1) noevents = 3;

      if (manager_config_filename.empty ())
        {
          manager_config_filename = "${SNG4_DATA_DIR}/resources/setups/snemo/config_1.0/manager.conf";
          clog << datatools::utils::io::warning
               << "Using default configuration file '" 
               << manager_config_filename << "' !" << endl; 
        }
      datatools::utils::fetch_path_with_env (manager_config_filename);

      datatools::utils::multi_properties configuration ("name", 
                                                        "", 
                                                        "Configuration for the SuperNEMO simulation program");
      configuration.read (manager_config_filename);
      if (debug) configuration.tree_dump (clog, 
                                          "Configuration for the SuperNEMO simulation program", 
                                          "DEBUG: ");

      // Simulation manager:
      snemo::g4::manager sim_manager;
      sim_manager.set_debug (debug);
      sim_manager.set_verbose (verbose);
      if (! g4_macro.empty ())
        {
          clog << datatools::utils::io::notice
               << "Using G4 macro '" 
               << g4_macro << "'..." << endl; 
          sim_manager.set_g4_macro (g4_macro);
        }
      sim_manager.set_interactive (interactive);
      sim_manager.set_g4_visualization (g4_visu);
      sim_manager.set_number_of_events (noevents);
      if (debug) sim_manager.dump (clog);
      sim_manager.initialize (configuration);
      if (debug) sim_manager.dump (clog);

      if (interactive)
        {
          clog << endl << "Hit [Enter] to continue..." << endl;
          string ans;
          getline (cin, ans);
        }

      sim_manager.run_simulation ();

      clog << "The end." << endl;
    }
  catch (exception & x)
    {
      cerr << datatools::utils::io::error
           << x.what () << endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << datatools::utils::io::error
           <<  "unexpected error !" << endl; 
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}

// end of test_manager.cxx
