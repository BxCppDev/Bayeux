// -*- mode: c++ ; -*-
/* ex_variants.cxx
 *
 * Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2014-10-30
 * Last modified : 2014-10-30
 *
 * Copyright (C) 2014 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 * This example illustrates the use of the configuration variants.
 *
 */

// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <exception>

// Third party:
// - Boost:
#include <boost/program_options.hpp>

// Bayeux/datatools:
#include <datatools/datatools_config.h>
#include <datatools/exception.h>
#include <datatools/logger.h>
#include <datatools/properties.h>
#include <datatools/kernel.h>
// #include <datatools/clhep_units.h>
#include <datatools/configuration/variant_repository.h>
#if DATATOOLS_STANDALONE == 1
#include <datatools/datatools.h>
#else
#include <bayeux/bayeux.h>
#endif // DATATOOLS_STANDALONE == 1
#if DATATOOLS_WITH_QT_GUI == 1
#include <QStyleFactory>
#include <QApplication>
#include <datatools/configuration/ui/variant_repository_dialog.h>
#endif // DATATOOLS_WITH_QT_GUI == 1


#include "application.hpp"

void test1();

void test2();

void test3();

int main(int argc_, char ** argv_)
{
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  int error_code = EXIT_SUCCESS;
#if DATATOOLS_STANDALONE == 1
  DATATOOLS_INIT_MAIN(argc_, argv_);
#else
  BAYEUX_INIT_MAIN(argc_, argv_);
#endif // DATATOOLS_STANDALONE == 1
  try {

    bool help = false;
    bool use_variants = false;

    // Define command line options:
    namespace po = boost::program_options;
    po::options_description opts("datatools' ex_variants ");

    opts.add_options()

      ("help",
       po::value<bool>(&help)
       ->zero_tokens()
       ->default_value(false),
       "Print help for the datatools kernel configuration options.  \n"
       "Example :                                                   \n"
       "  --help                                                      "
       )

      ("with-variants",
       po::value<bool>(&use_variants)
       ->zero_tokens()
       ->default_value(false),
       "Use configuration variants.                                 \n"
       "Example :                                                   \n"
       "  --with-variants                                             "
       )
      ;

    // Parse command line options:
    po::positional_options_description args;
    po::variables_map vm;
    po::parsed_options parsed =
      po::command_line_parser(argc_, argv_)
      .options(opts) // Only options to be parsed.
      .allow_unregistered()
      .run();
    // std::vector<std::string> unrecognized_args = po::collect_unrecognized(parsed.options, po::include_positional);
    po::store(parsed, vm);
    po::notify(vm);

    // Print help:
    if (help) {
      std::cout << opts;
      return error_code;
    }

    // Run:

    if (!use_variants) {
      test1();
    } else {
      test2();
      // test2();
    }

  }
  catch (std::exception & x) {
    datatools_fatal(logging, x.what());
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    datatools_fatal(logging, "unexpected error !");
    error_code = EXIT_FAILURE;
  }
#if DATATOOLS_STANDALONE == 1
  DATATOOLS_FINI();
#else
  BAYEUX_FINI();
#endif // DATATOOLS_STANDALONE == 1
  return (error_code);
}

void test1()
{
  std::clog << "\ntest1:\n";
  std::string app_config_filename = "${FOO_CONFIG_DIR}/foo.conf";
  datatools::fetch_path_with_env(app_config_filename);
  datatools::properties app_config;
  app_config.read_configuration(app_config_filename);
  app_config.tree_dump(std::clog, "Configuration properties (without variants):");
  application app;
  app.initialize(app_config);
  app.print();
  app.run();
  return;
}

void test2()
{
  std::clog << "\ntest2:\n";

  // Declare a variant repository:
  datatools::configuration::variant_repository foo_rep;

  {
    // Process the configuration of the variant repository:
    std::string foo_rep_config_filename = "${FOO_CONFIG_DIR}/variants/foo_variance.conf";
    datatools::fetch_path_with_env(foo_rep_config_filename);
    datatools::properties foo_rep_config;
    foo_rep_config.read_configuration(foo_rep_config_filename);

    // Initialize from a configuration file:
    foo_rep.initialize(foo_rep_config);
    foo_rep.tree_dump(std::clog, "Variant repository:");

#if DATATOOLS_WITH_QT_GUI == 1
    {
      // Launch a Qt based dialog for the variant repository:
      // QApplication::setStyle(QStyleFactory::create(QString::fromStdString("plastique")));
      int argc = 1;
      const char * argv[] = { "Bayeux - Configuration Variant Repository Dialog" };
      QApplication qapp(argc, (char**) argv);
      datatools::configuration::ui::variant_repository_dialog vrep_dialog(foo_rep);
      vrep_dialog.show();
      int ret = qapp.exec();
    }
#endif // DATATOOLS_WITH_QT_GUI == 1

    // Import all registries in the kernel system configuration repository
    // and make it globaly visible :
    datatools::kernel::instance().import_configuration_repository(foo_rep);
  }

  {
    std::string app_config_filename = "${FOO_CONFIG_DIR}/foo_with_variants.conf";
    datatools::fetch_path_with_env(app_config_filename);
    datatools::properties app_config;
    app_config.read_configuration(app_config_filename);

    application app;
    app.initialize(app_config);
    app.print();
    app.run();
  }

  return;
}

void test3()
{
  std::clog << "\ntest3:\n";
  std::string app_config_filename = "${FOO_CONFIG_DIR}/foo_with_variants.conf";
  datatools::fetch_path_with_env(app_config_filename);
  datatools::properties app_config;
  app_config.read_configuration(app_config_filename);
  app_config.tree_dump(std::clog, "Configuration properties (with variants):");
  application app;
  app.initialize(app_config);
  app.print();
  app.run();
  return;
}

// Include classes' definitions:
#include "application.cpp"
