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
 *   This example illustrates the use of the configuration variants.
 *
 */

// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <exception>

// Third party:
// - Boost:
#include <boost/program_options.hpp>
// - Bayeux/datatools:
#include <datatools/datatools_config.h>
#include <datatools/datatools_init.h>
#include <datatools/exception.h>
#include <datatools/logger.h>
#include <datatools/properties.h>
#include <datatools/multi_properties.h>
#include <datatools/kernel.h>
#include <datatools/configuration/variant_repository.h>
#include <datatools/configuration/variant_service.h>
// - Bayeux:
#include <bayeux/bayeux.h>
#include <bayeux/bayeux_init.h>
#if DATATOOLS_WITH_QT_GUI == 1
// - Qt:
#include <QStyleFactory>
#include <QApplication>
// - Bayeux/datatools:
#include <datatools/qt/interface.h>
#include <datatools/configuration/ui/variant_repository_dialog.h>
#endif // DATATOOLS_WITH_QT_GUI == 1

// This project:
#include "application.hpp"

void test0();

void test1(const std::string & load_,
           const std::vector<std::string> & settings_,
           const std::string & store_,
           const bool gui_ = false, bool const multi_ = false);

uint32_t bxinitflags();

int main(int argc_, char ** argv_)
{
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  int error_code = EXIT_SUCCESS;
  bayeux::initialize(argc_, argv_, bxinitflags());
  try {

    bool help = false;
    bool use_variants = false;
    int  variants_test = 1;
    std::string variant_load;
    std::vector<std::string> variant_settings;
    std::string variant_store;
    bool variant_gui = false;

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

      ("variants-test",
       po::value<int>(&variants_test)
       ->default_value(1),
       "Select the configuration variant test.                      \n"
       "Example :                                                   \n"
       "  --with-variants --variants-test 2                           "
       )

      ("variant-load",
       po::value<std::string>(&variant_load),
       "Select the variant profile to be loaded                     \n"
       "Example :                                                   \n"
       "  --variant-load=\"in.profile\"                             "
       )

      ("variant-set",
       po::value<std::vector<std::string>>(&variant_settings),
       "Select the configuration variant test.                      \n"
       "Example :                                                   \n"
       "  --variant-set=\"core:gui=true\"                             "
       )

      ("variant-store",
       po::value<std::string>(&variant_store),
       "Select the variant profile to be stored                     \n"
       "Example :                                                   \n"
       "  --variant-store=\"out.profile\"                             "
       )

#if DATATOOLS_WITH_QT_GUI == 1
      ("variant-gui",
       po::value<bool>(&variant_gui)
       ->zero_tokens()
       ->default_value(false),
       "Use variants GUI.                                           \n"
       "Example :                                                   \n"
       "  --variant-gui                                             "
       )
#endif // DATATOOLS_WITH_QT_GUI == 1

      ;

    // Parse command line options:
    po::positional_options_description args;
    po::variables_map vm;
    po::parsed_options parsed =
      po::command_line_parser(argc_, argv_)
      .options(opts) // Only options to be parsed.
      // .allow_unregistered()
      .run();
    po::store(parsed, vm);
    po::notify(vm);

    // Print help:
    if (help) {
      std::cout << opts;
      return error_code;
    }

    bool multi = false;
    // Run:
    if (! use_variants) {
      test0();
    } else {
      if (variants_test == 1) {
        multi = false;
      } else if (variants_test == 2) {
        multi = true;
      }
      test1(variant_load, variant_settings, variant_store, variant_gui, multi);
    }

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

uint32_t bxinitflags()
{
  uint32_t flags = 0;
  flags |= bayeux::init_dtkernel_no_logging;
  //flags |= bayeux::init_dtkernel_no_variant;
  flags |= bayeux::init_dtkernel_no_inhibit_variant;
  return flags;
}

void test0()
{
  std::clog << "\ntest0: Do not use the configuration variants mechanism...\n";

  std::string app_config_filename = "${FOO_CONFIG_DIR}/foo.conf";
  datatools::fetch_path_with_env(app_config_filename);
  datatools::properties app_config;
  app_config.read_configuration(app_config_filename);
  app_config.tree_dump(std::clog, "Configuration properties (without variants):");
  application app;
  app.initialize(app_config);
  app.print();
  app.run();
  std::clog << "test0: End.\n";
  return;
}

void test1(const std::string & load_,
           const std::vector<std::string> & settings_,
           const std::string & store_,
           const bool gui_,
           const bool multi_)
{
  std::clog << "\ntest1: Use the configuration variants mechanism...\n";

  datatools::configuration::variant_service::config variant_config;
  variant_config.config_filename = "${FOO_CONFIG_DIR}/variants/foo_variants.conf";
  variant_config.profile_load = load_;
  variant_config.settings = settings_;
  variant_config.profile_store = store_;
#if DATATOOLS_WITH_QT_GUI == 1
  variant_config.gui = gui_;
#endif // DATATOOLS_WITH_QT_GUI == 1

  datatools::configuration::variant_service vserv;
  if (variant_config.is_active()) {
    vserv.configure(variant_config);
    vserv.start();
  }

  application app;
  if (!multi_) {
    std::clog << "test1: Setup the application with variant parameters fetched from the Bayeux/datatools kernel...\n";
    std::string app_config_filename = "${FOO_CONFIG_DIR}/foo_with_variants.conf";
    datatools::fetch_path_with_env(app_config_filename);
    datatools::properties app_config;
    app_config.read_configuration(app_config_filename);
    app.initialize(app_config);
  } else {
    std::clog << "test1: Setup the application with variant parameters fetched from the Bayeux/datatools' kernel...\n";
    std::string app_mconfig_filename = "${FOO_CONFIG_DIR}/bar_with_variants.conf";
    datatools::fetch_path_with_env(app_mconfig_filename);
    datatools::multi_properties app_mconfig;
    app_mconfig.read(app_mconfig_filename);
    app.initialize_m(app_mconfig);
  }
  app.print();
  app.run();

  // Terminate variant service:
  if (vserv.is_started()) {
    vserv.stop();
  }

  std::clog << "test1: End.\n";
  return;
}
