/* ex_OCD.cxx
 *
 * Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2013-05-10
 * Last modified : 2013-10-17
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
 * This example illustrates the use of the Object Configuration Description
 * mechanism (OCD) associated to an arbitrary user class and an official
 * class from the datatools library.
 *
 */

// This project:
#include <datatools/datatools_config.h>

// Standard Library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <exception>

// Third Party:

// - Bayeux:
#include <bayeux/bayeux.h>
// - Bayeux/datatools
#include <datatools/object_configuration_description.h>
#include <datatools/service_manager.h>
#include <datatools/logger.h>

// This example
#include <foo.h>

int main (int argc_, char ** argv_)
{
  bayeux::initialize(argc_, argv_);

  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  int error_code = EXIT_SUCCESS;
  try {
    DT_LOG_INFORMATION(logging, "datatools example program : ex_OCD");

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
          DT_LOG_WARNING(logging, "Ignoring option '" << option << "' !");
        }
      } else {
        std::string argument = token;
        DT_LOG_WARNING(logging, "Ignoring argument '" << argument << "' !");
      }
      iarg++;
    }

    // Print the list of class IDs with embeded datatools/OCD system support :
    DOCD_GET_SYSTEM_REGISTRATION().smart_dump(std::clog,
                                              "The datatools/OCD system registration : ");

    {
      std::clog << std::endl;
      datatools::object_configuration_description OCD;
      if (datatools::load_ocd<foo>(OCD)) {
        DT_LOG_INFORMATION(logging, "Found OCD support for the 'foo' class.");
        if (logging >= datatools::logger::PRIO_DEBUG) OCD.dump(std::clog, "OCD for the foo class : ");
        // Store the documentation about the 'foo' class in a RST file :
        std::ofstream ocd_rst("ex_OCD.foo.doc.rst");
        OCD.print(ocd_rst, "");

        // Load and parse a configuration file :
        std::string foo_config_file = "config/ex_OCD.foo.candidate.conf";
        datatools::fetch_path_with_env(foo_config_file);
        datatools::properties foo_config;
        datatools::read_config(foo_config_file, foo_config);

        // Validate the configuration parameters :
        if (OCD.has_validation_support()) {
          std::string message;
          if (OCD.validate(foo_config, message)) {
            DT_LOG_INFORMATION(logging,
                               "The property file '" << foo_config_file << "' is valid !");
          } else {
            DT_LOG_WARNING(logging,
                           "The property file '" << foo_config_file
                           << "' does not conform the OCD rules ! ");
            DT_LOG_WARNING(logging,
                           "Message is : " << message);
          }
        }

        // Initialize a 'foo' object with the validated configuration parameters :
        foo f;
        // Enforce the priority logging before initialization
        f.set_logging_priority(logging);
        f.initialize(foo_config);
        std::clog << "foo object is : " << std::endl;
        f.dump(std::clog);

        // Generate a skeleton configuration file for a 'foo' object :
        std::ofstream fscf ("ex_OCD.foo.skeleton.conf");
        OCD.generate_sample_configuration(fscf, "the configuration of a 'foo' test object");
      }
    }

    {
      // OCD support for the 'datatools::service_manager' class :
      std::clog << std::endl;
      datatools::object_configuration_description OCD;
      if (datatools::load_ocd<datatools::service_manager>(OCD)) {
        DT_LOG_INFORMATION(logging,
                              "Found OCD support for the 'datatools::service_manager' class.");

        // Print the documentation in a ReST file :
        std::ofstream ocd_rst("ex_OCD.service_manager.doc.rst");
        OCD.print(ocd_rst, "");

        // Generate a skeleton configuration file :
        std::ofstream fscf ("ex_OCD.service_manager.skeleton.conf");
        OCD.generate_sample_configuration(fscf, "the configuration of a 'datatools::service_manager' object");
      }
    }

    DT_LOG_INFORMATION(logging, "The end.");
  } catch (std::exception & x) {
    DT_LOG_FATAL(logging, x.what());
    error_code = EXIT_FAILURE;
  } catch (...) {
    DT_LOG_FATAL(logging, "unexpected error !");
    error_code = EXIT_FAILURE;
  }

  bayeux::terminate();
  return error_code;
}
