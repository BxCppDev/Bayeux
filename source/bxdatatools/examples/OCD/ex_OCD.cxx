// -*- mode: c++ ; -*-
/* ex_OCD.cxx
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
 * This example illustrates the use of the Object Configuration Description
 * mechanism (OCD) associated to an arbitrary user class and an official
 * class from the datatools library.
 *
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <exception>

#include <datatools/datatools_config.h>
#include <datatools/object_configuration_description.h>
#include <datatools/service_manager.h>

#include <foo.h>

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "datatools example program : ex_OCD" << std::endl;

    bool debug = false;
    bool verbose = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];

      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-d") || (option == "--debug")) {
          debug = true;
        } else if ((option == "-v") || (option == "--verbose")) {
          verbose = true;
        } else  {
          std::clog << "warning: ignoring option '" << option << "'!" << std::endl;
        }
      } else {
        std::string argument = token;
        {
          std::clog << "warning: ignoring argument '" << argument << "'!" << std::endl;
        }
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
        std::clog << "Found OCD support for the 'foo' class." << std::endl;
        if (debug) OCD.dump(std::clog, "OCD for the foo class : ");

        // Store the documentation about the 'foo' class in a RST file :
        std::ofstream ocd_rst("ex_OCD.foo.doc.rst");
        OCD.print(ocd_rst, "");

        // Load and parse a configuration file :
        std::string foo_config_file
          = DATATOOLS_DATA_INSTALL_DIR
          "/examples/OCD/config/ex_OCD.foo.candidate.conf";
        datatools::fetch_path_with_env(foo_config_file);
        datatools::properties foo_config;
        datatools::properties::read_config(foo_config_file, foo_config);

        // Validate the configuration parameters :
        if (OCD.has_validation_support()) {
          std::string message;
          if (OCD.validate(foo_config, message)) {
            std::clog << "The property file '" << foo_config_file << "' is valid !\n";
          } else {
            std::clog << "The property file '" << foo_config_file
                      << "' does not conform the OCD rules ! ";
            std::clog << "Message is : " << message << "\n";
          }
        }

        // Initialize a 'foo' object with the validated configuration parameters :
        foo f;
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
        std::clog << "Found OCD support for the 'datatools::service_manager' class." << std::endl;

        // Print the documentaiton in a RST file :
        std::ofstream ocd_rst("ex_OCD.service_manager.doc.rst");
        OCD.print(ocd_rst, "");

        // Generate a skeleton configuration file :
        std::ofstream fscf ("ex_OCD.service_manager.skeleton.conf");
        OCD.generate_sample_configuration(fscf, "the configuration of a 'datatools::service_manager' object");
      }
    }

    std::clog << "The end." << std::endl;
  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

// end of ex_OCD.cxx
