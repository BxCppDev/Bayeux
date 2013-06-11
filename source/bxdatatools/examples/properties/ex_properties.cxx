// -*- mode: c++ ; -*-
/* ex_properties.cxx
 *
 * Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2013-05-10
 * Last modified : 2013-05-10
 *
 * Copyright (C) 2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 * Description :
 * This example illustrates the use of the ``datatools::properties``
 * class.
 *
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <exception>

#include <datatools/datatools_config.h>
#include <datatools/properties.h>
#include <datatools/clhep_units.h>
#include <datatools/logger.h>

int main (int argc_, char ** argv_)
{
  datatools::logger::priority logging = datatools::logger::PRIO_WARNING;
  int error_code = EXIT_SUCCESS;
  try {

    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];

      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-d") || (option == "--debug")) {
          logging = datatools::logger::PRIO_DEBUG;
        } else if ((option == "-v") || (option == "--verbose")) {
          logging = datatools::logger::PRIO_INFORMATION;
        } else  {
          datatools_warning(logging, "Ignoring option '" << option << "' !");
        }
      } else {
        std::string argument = token;
        datatools_warning(logging, "Ignoring argument '" << argument << "' !");
      }
      iarg++;
    }

    datatools_information(logging, "datatools example program : ex_properties");

    // Declare a container of properties :
    datatools_debug(logging, "Declare a container of properties");
    datatools::properties foo_config;
    foo_config.set_description("The configuration parameters for a given task");

    datatools_debug(logging, "Store some properties in it...");

    // Set the boolean property ``debug`` to true :
    foo_config.store_flag("debug", "The debug flag");

    // Set the integer property ``number_of_users`` to some value :
    foo_config.store("number_of_users", 666);  // Undocumented property : the number of users

    // Set the real (double precision) property ``width`` to some value :
    double wthumb = 2.541 * CLHEP::cm;
    foo_config.store("width", wthumb / CLHEP::mm, "The width of my thumb (mm)");

    // Set the string property ``colour`` to some value :
    foo_config.store("colour", "red", "My favorite colour");

    // Set a vector of real property :
    std::vector<double> point;
    point.push_back(22.3 * CLHEP::cm);
    point.push_back(1.7 * CLHEP::m);
    point.push_back(8.5 * CLHEP::mm);
    foo_config.store("point", point, "A position in 3D-space (implicit lenght unit)");

    // Set a vector of bool property :
    std::vector<bool> flags;
    flags.push_back(0);
    flags.push_back(1);
    flags.push_back(0);
    flags.push_back(1);
    flags.push_back(1);
    flags.push_back(1);
    flags.push_back(0);
    flags.push_back(1);
    foo_config.store("flags", flags, "Some binary flags");

    // Set an undocumented vector of string :
    std::vector<std::string> words;
    words.push_back("King");
    words.push_back("of");
    words.push_back("the");
    words.push_back("Britains");
    foo_config.store("words", words);

    // Set some properties of which the 'keys/names' have the same prefix :
    foo_config.store_flag("algo.debug",
                          "The debug flag of the embeded algorithm");
    foo_config.store("algo.number_of_events", 234,
                     "The number of events to be processed by the embeded algorithm");
    foo_config.store("algo.max_time", 1.2,
                     "The computation time limit (in millisecond)");
    foo_config.store("algo.mode", "fast",
                     "The running mode");

    std::vector<int> params;
    params.push_back(3);
    params.push_back(5);
    params.push_back(8);
    params.push_back(2);
    foo_config.store("algo.parameters", params,
                     "The input parameters");

    // Smart print :
    std::clog << std::endl;
    foo_config.tree_dump(std::clog, "The configuration parameters : ");

    // Save the setup in ASCII format :
    datatools_debug(logging, "Save the setup in ASCII format...");
    datatools::properties::write_config("foo.conf", foo_config);

    datatools_debug(logging, "Modify its contents...");
    // Change some existing values:
    foo_config.change("width", 3.244 * CLHEP::cm);
    foo_config.update("colour", "blue");

    // Update some possibly existing values:
    foo_config.update("length", 17.3 * CLHEP::cm);

    // Smart print :
    std::clog << std::endl;
    foo_config.tree_dump(std::clog, "The configuration parameters : ");

    // Declare another container of properties :
    datatools::properties algo_config;

    // Export properties from one container to the other, only if property's name
    // starts with the ``algo.`` prefix :
    datatools_debug(logging, "Export part of its contents to another instance...");
    foo_config.export_starting_with(algo_config, "algo.");

    // Remove all properties if property's name starts with the ``algo.`` prefix :
    foo_config.erase_all_starting_with("algo.");

    // Smart print :
    std::clog << std::endl;
    foo_config.tree_dump(std::clog, "The configuration parameters : ");

    // Smart print :
    std::clog << std::endl;
    algo_config.tree_dump(std::clog, "The exported configuration parameters : ");

    // Load the backup configuration parameters from a file :
    datatools_debug(logging, "Load a container of properties from an ASCII formatted file...");
    std::string foo_config_file = "foo.conf";
    datatools::fetch_path_with_env(foo_config_file);
    datatools::properties::read_config(foo_config_file, foo_config);

    // Smart print :
    std::clog << std::endl;
    foo_config.tree_dump(std::clog, "The backup configuration parameters : ");

    datatools_information(logging, "The end.");
   }
  catch (std::exception & x) {
    datatools_fatal(logging, x.what());
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    datatools_fatal(logging, "unexpected error !");
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

// end of ex_properties.cxx
