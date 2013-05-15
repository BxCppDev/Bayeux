// -*- mode: c++ ; -*-
/* ex_things_1.cxx
 *
 * Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2013-05-15
 * Last modified : 2013-05-15
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
 * This example illustrates the use of the ``datatools::things``
 * class.
 *
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <exception>

#include <datatools/datatools_config.h>
#include <datatools/things.h>
#include <datatools/properties.h>
#include <datatools/clhep_units.h>
#include <datatools/logger.h>

int main (int argc_, char ** argv_)
{
  datatools::logger::priority logging = datatools::logger::PRIO_WARNING;
  int error_code = EXIT_SUCCESS;
  try {
    long prng_seed = 314159;

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

    datatools_information(logging, "datatools example program : ex_things_1");

    datatools_debug(logging, "Initialize the PRNG");
    srand48(prng_seed);

    // Declare a ``datatools::things`` object :
    datatools_debug(logging, "Declare a data record");
    datatools::things DR;
    DR.set_name("DR");
    DR.set_description("A data record with data banks in it");

    // Fill the data record with some data banks :
    {
      // Add a bank named ``EH`` of type ``datatools::properties`` :
      datatools_debug(logging, "Add the *event header* bank");
      datatools::properties & EH = DR.add<datatools::properties>("EH", "The event header");
      EH.store("run_number", 4);
      EH.store("event_number", 2);
      EH.store("time", 123456);

      // Add a bank named ``RD`` of type ``datatools::properties`` :
      datatools::properties & RD = DR.add<datatools::properties>("RD", "Raw hits data");
      int number_of_hits = 5;
      RD.store("number_of_hits", number_of_hits);
      // Generate some raw hit data :
      std::vector<int> hit_ids;
      std::vector<std::string> hit_channels;
      std::vector<int> hit_adcs;
      for (int i = 0; i < number_of_hits; i++) {
        hit_ids.push_back(i);
        std::ostringstream channel_s;
        channel_s << "[calo:"
                  << ((int) (4 * drand48()))
                  << '.' << ((int) (8 * drand48()))
                  << ']';
        hit_channels.push_back(channel_s.str());
        hit_adcs.push_back(20 + (int) (2028 * drand48()));
      }
      RD.store("hit.ids", hit_ids);
      RD.store("hit.channels", hit_channels);
      RD.store("hit.adcs", hit_adcs);

      // Add a bank named ``CD`` of type ``datatools::properties`` :
      datatools::properties & CD = DR.add<datatools::properties>("CD", "Calibrated data");
      RD.store("total_energy", 734.3 * CLHEP::keV);
      RD.store("total_energy.sigma", 14.6 * CLHEP::keV);
      RD.store("timestamp", 345.0 * CLHEP::picosecond);
      RD.store("track.length", 45.9 * CLHEP::cm);
      RD.store("track.particle", "e-");
      RD.store("track.quality", 3);

      // Smart print
      DR.tree_dump(std::clog, "The data record : ");
    }

    // Extract and manipulate data banks from the data record :
    {
      if (DR.has("EH") && DR.is_a<datatools::properties>("EH")) {
        datatools_notice(logging, "Found the event header bank.");

        // Obtain a mutable reference :
        datatools::properties & EH = DR.grab<datatools::properties>("EH");
        EH.store_flag("visited"); // modify the contents of the container

        // Obtain a const reference :
        const datatools::properties & const_EH = DR.get<datatools::properties>("EH");
        const_EH.tree_dump(std::clog, "Event header : "); // print it
      }

      if (DR.has("RD") && DR.is_a<datatools::properties>("RD")) {
        datatools_notice(logging, "Found the raw data bank.");

        // Obtain a mutable reference :
        datatools::properties & RD = DR.grab<datatools::properties>("RD");
        RD.store_flag("fake_hits"); // modify the contents of the container

        // Obtain a const reference :
        const datatools::properties & const_RD = DR.get<datatools::properties>("RD");
        const_RD.tree_dump(std::clog, "Raw data : "); // print it

        // Read the raw hits data from the bank :
        int number_of_hits = const_RD.fetch_integer("number_of_hits");
        std::vector<int> hit_ids;
        std::vector<std::string> hit_channels;
        std::vector<int> hit_adcs;
        const_RD.fetch("hit.ids", hit_ids);
        const_RD.fetch("hit.channels", hit_channels);
        const_RD.fetch("hit.adcs", hit_adcs);
        for (int i = 0; i < number_of_hits; i++) {
          std::clog << "Raw hit #" << hit_ids[i] << " :"
                    << " Channel=" <<  hit_channels[i]
                    << " ADC=" <<  hit_adcs[i]
                    << std::endl;
        }
      }

    }


    // Remove a data bank from the data record :
    {
      if (DR.has("RD")) {
        datatools_notice(logging, "Remove the raw data bank.");
        DR.erase("RD");

        // Smart print :
        DR.tree_dump(std::clog, "The data record (without the raw data bank): ");
      }
    }

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

// end of ex_things_1.cxx
