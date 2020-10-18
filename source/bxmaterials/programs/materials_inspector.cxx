// (bx)materials_inspector.cxx
//
// Copyright (c) 2007-2017 by Francois Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2007-2017 by Université de Caen
//
// This file is part of Bayeux.
//
// Bayeux is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux. If not, see <http://www.gnu.org/licenses/>.

// Standard Library
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <memory>

// Third Party
// - Boost
#include <boost/program_options.hpp>
// - Bayeux:
#include <bayeux/bayeux.h>
#include <bayeux/version.h>
// - Bayeux/datatools
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/kernel.h>
#include <datatools/configuration/variant_service.h>
// - Bayeux/materials
#include <materials/materials_driver.h>

namespace dtc = datatools::configuration;
namespace po = boost::program_options;

//! \brief Application configuration/running parameters
struct app_config_params {
  /// Logging priority
  std::string                  logging_label = "fatal";
  datatools::logger::priority  logging = datatools::logger::PRIO_FATAL;
  dtc::variant_service::config variants; ///< Parameters specific to variant support
  bool        help           = false;
  bool        no_splash      = false;
  bool        dump_manager   = false;
  bool        list_isotopes  = false;
  bool        list_elements  = false;
  bool        list_materials = false;
  bool        list_aliases   = false;
  std::string show_isotope;
  std::string show_element;
  std::string show_material;
  bool        with_decoration    = false;
  bool        without_decoration = false;
  materials::materials_driver_params driver; ///< Parameters to the driver
};

/// Return the application name
std::string app_name();

//! Print application splash screen
void app_print_splash(std::ostream & out_ = std::clog);

/// Build the list of general options description
void app_build_general_opts(boost::program_options::options_description & opts_,
                            app_config_params & params_);

/// Print online help
void app_print_help(boost::program_options::options_description & opts_,
                    std::ostream & out_ = std::clog);

/// Return kernel initialization flags for this application
uint32_t app_kernel_init_flags();

/// Build genvtx driver initialization options
void app_driver_build_initialization_opts(boost::program_options::options_description & opts_,
                                          materials::materials_driver_params & params_);

/// Build genvtx driver action options
void app_driver_build_action_opts(boost::program_options::options_description & opts_,
                                  app_config_params & params_);

//! Main program
int main(int argc_, char ** argv_)
{
  bayeux::initialize(argc_, argv_, app_kernel_init_flags());

  int error_code = EXIT_SUCCESS;
  app_config_params params;
  po::options_description optPublic;

  try {

    // Describe general command line options:
    po::options_description optDesc("General options");
    app_build_general_opts(optDesc, params);

    // Declare options for the driver:
    po::options_description optDriverInit("Driver initialization parameters");
    po::options_description optDriverAction("Driver action options");
    app_driver_build_initialization_opts(optDriverInit, params.driver);
    app_driver_build_action_opts(optDriverAction, params);

    // Declare options for variant support:
    po::options_description optVariant("Variant support");
    uint32_t po_init_flags = 0;
    po_init_flags |= dtc::variant_service::NO_LABEL;
    po_init_flags |= dtc::variant_service::NO_LOGGING;
    // po_init_flags |= dtc::variant_service::PROFILE_LOAD_DONT_IGNORE_UNKNOWN;
    po_init_flags |= dtc::variant_service::NO_TUI;
    dtc::variant_service::init_options(optVariant,
                                       params.variants,
                                       po_init_flags);

    // Declare options for kernel:
    po::options_description optKernel("Kernel options");
    datatools::kernel::param_type paramsKernel;
    datatools::kernel::build_opt_desc(optKernel, paramsKernel, app_kernel_init_flags());

    // Aggregate options:
    optPublic
      .add(optDesc)
      .add(optDriverInit)
      .add(optDriverAction)
      .add(optVariant)
      .add(optKernel);

    // Describe command line arguments :
    po::positional_options_description args;
    args.add("manager-config", -1);

    // Parse command line options:
    po::variables_map vMap;
    po::parsed_options parsed =
      po::command_line_parser(argc_, argv_)
      .options(optPublic)
      .positional(args)
      .allow_unregistered()
      .run();

    // Collect all other options & args for the driver session:
    params.driver.action_options =
      po::collect_unrecognized(parsed.options,
                               po::include_positional);
    po::store(parsed, vMap);
    po::notify(vMap);

    bool run_session = true;
    // Fetch the opts/args :
    if (params.help) {
      app_print_help(optPublic, std::cout);
      run_session = false;
    }

    {
      datatools::logger::priority prio
        = datatools::logger::get_priority(params.logging_label);
      if (prio != datatools::logger::PRIO_UNDEFINED) {
        params.logging = prio;
      } else {
        DT_THROW(std::logic_error,
                 "Invalid logging priority '" << params.logging_label << "' !");
      }
    }

    // Run the driver session:
    if (run_session) {
      DT_LOG_DEBUG(params.logging, "Running session...");
      if (! params.no_splash) {
        app_print_splash(std::cerr);
      }

      // Variant service:
      std::unique_ptr<dtc::variant_service> vserv;
      if (params.variants.is_active()) {
        // Create and start the variant service:
        vserv.reset(new dtc::variant_service);
        vserv->configure(params.variants);
        vserv->start();
        if (datatools::logger::is_debug(params.logging)) {
          vserv->get_repository().tree_dump(std::cerr, "Variant repository:", "[debug] ");
        }
      }

      params.driver.action = 0;

      if (params.dump_manager) {
        params.driver.action |= materials::materials_driver_params::ACTION_DUMP_MANAGER;
      }

      if (params.list_isotopes) {
        params.driver.action |= materials::materials_driver_params::ACTION_LIST_ISOTOPES;
      }

      if (params.list_elements) {
        params.driver.action |= materials::materials_driver_params::ACTION_LIST_ELEMENTS;
      }

      if (params.list_materials) {
        params.driver.action |= materials::materials_driver_params::ACTION_LIST_MATERIALS;
      }

      if (params.list_aliases) {
        params.driver.action |= materials::materials_driver_params::ACTION_LIST_ALIASES;
      }

      if (! params.show_isotope.empty()) {
        params.driver.show_target = params.show_isotope;
        params.driver.action |= materials::materials_driver_params::ACTION_SHOW_ISOTOPE;
      }

      if (! params.show_element.empty()) {
        params.driver.show_target = params.show_element;
        params.driver.action |= materials::materials_driver_params::ACTION_SHOW_ELEMENT;
      }

      if (! params.show_material.empty()) {
        params.driver.show_target = params.show_material;
        params.driver.action |= materials::materials_driver_params::ACTION_SHOW_MATERIAL;
      }

      if (params.with_decoration) {
        params.driver.with_decoration = true;
      }

      if (params.without_decoration) {
        params.driver.with_decoration = false;
      }

      if (datatools::logger::is_debug(params.logging)) {
        params.driver.dump(std::cerr);
      }

      // Run the driver:
      materials::materials_driver MD;

      // Initialize the driver:
      MD.initialize(params.driver);

      // Run a one-shot session:
      MD.run_action(std::cout, std::clog);

      // Terminate the driver:
      MD.reset();

      if (vserv) {
        if (vserv->is_started()) {
          // Stop the variant service:
          vserv->stop();
        }
        vserv.reset();
      }

      DT_LOG_DEBUG(params.logging, "End of session.");
    } // end of runs session

    // The end.
  } catch (std::exception & x) {
    DT_LOG_FATAL(params.logging, x.what());
    error_code = EXIT_FAILURE;
  } catch (...) {
    DT_LOG_FATAL(params.logging, "Unexpected error !");
    error_code = EXIT_FAILURE;
  }

  bayeux::terminate();
  return error_code;
}

std::string app_name()
{
  return "bxmaterials_inspector";
}

uint32_t app_kernel_init_flags()
{
  uint32_t kernel_init_flags = 0;
  kernel_init_flags |= ::datatools::init_no_help;
  kernel_init_flags |= ::datatools::init_no_splash;
  kernel_init_flags |= ::datatools::init_no_inhibit_libinfo;
  kernel_init_flags |= ::datatools::init_no_libinfo_logging;
  kernel_init_flags |= ::datatools::init_no_variant;
  kernel_init_flags |= ::datatools::init_no_inhibit_variant;
  kernel_init_flags |= ::datatools::init_no_locale_category;
  kernel_init_flags |= ::datatools::init_no_inhibit_qt_gui;
  kernel_init_flags |= ::datatools::init_no_inhibit_urnquery;
  return kernel_init_flags;
}

void app_build_general_opts(boost::program_options::options_description & opts_,
                            app_config_params & params_)
{
  namespace po = boost::program_options;
  opts_.add_options()
    ("help,h",
     po::value<bool>(&params_.help)
     ->zero_tokens()
     ->default_value(false)
     ,
     "Print help then exit.   \n"
     "Example :               \n"
     "  --help                  "
     )
    ("logging",
     po::value<std::string>(&params_.logging_label)
     ->default_value("fatal")
     ,
     "Set the logging priority threshold.\n"
     "Example :                          \n"
     "  --logging \"warning\"              "
     )
    ("no-splash,S",
     po::value<bool>(&params_.no_splash)
     ->zero_tokens()
     ->default_value(false)
     ,
     "Do not print the splash screen.\n"
     "Example : \n"
     "  --no-splash "
     )
    ;
  return;
}

void app_driver_build_initialization_opts(boost::program_options::options_description & opts_,
                                          materials::materials_driver_params & params_)
{
  opts_.add_options()
    ("dll-config",
     po::value<std::string>(&params_.LL_config),
     "Set the dynamic library loader configuration file.\n"
     "Example :\n"
     "  --dll-config \"dlls.lis\""
     )
    ("load-dll",
     po::value<std::vector<std::string> >(&params_.LL_dlls),
     "Load a given dynamic library (DLL).      \n"
     "Example :                                \n"
     "  --load-dll \"foo\" --load-dll \"bar\" "
     )
    ("manager-config,c",
     po::value<std::string>(&params_.MaterialsMgrConfigFile),
     "Set the materials manager configuration file.   \n"
     "Example :                                       \n"
     "  --manager-config config/materials/manager.conf   "
     )
    ;
  return;
}

void app_driver_build_action_opts(boost::program_options::options_description & opts_,
                                  app_config_params & params_)
{
  opts_.add_options()
    ("dump-manager,D",
     po::value<bool>(&params_.dump_manager)
     ->zero_tokens()
     ->default_value(false),
     "Dump the material manager's internals.")
    ("list-isotopes,I",
     po::value<bool>(&params_.list_isotopes)
     ->zero_tokens()
     ->default_value(false),
     "Prints the list of registered isotopes.")
    ("list-elements,E",
     po::value<bool>(&params_.list_elements)
     ->zero_tokens()
     ->default_value (false),
     "Prints the list of registered elements.")
    ("list-materials,M",
     po::value<bool>(&params_.list_materials)
     ->zero_tokens()
     ->default_value (false),
     "Prints the list of registered materials.")
    ("list-aliases,A",
     po::value<bool>(&params_.list_aliases)
     ->zero_tokens()
     ->default_value (false),
     "Prints the list of registered material aliases.")
    ("show-isotope,i",
     po::value<std::string>(&params_.show_isotope),
     "Show a registered isotope.                  \n"
     "Example :                                   \n"
     " --show-isotope \"He-6\"                      ")
    ("show-element,e",
     po::value<std::string>(&params_.show_element),
     "Show a registered element.                  \n"
     "Example :                                   \n"
     " --show-element \"Al\"                        ")
    ("show-material,m",
     po::value<std::string>(&params_.show_material),
     "Show a registered material.                  \n"
     "Example :                                    \n"
     " --show-material \"ShieldingMetal\"            ")
    ("with-decoration,o",
     po::value<bool>(&params_.with_decoration)
     ->zero_tokens()
     ->default_value(true),
     "produce list output with fancy headers, titles and decoration.")
    ("without-decoration,O",
     po::value<bool>(&params_.without_decoration)
     ->zero_tokens()
     ->default_value(false),
     "produce list output without fancy headers, titles and decoration.")
    ;
  return;
}

void app_print_splash(std::ostream & out_)
{
  out_ << "                                                    \n"
       << "\tB A Y E U X  -  M A T E R I A L S    P R O D U C T I O N\n"
       << "\tVersion " << bayeux::version::get_version() << "\n"
       << "                                                    \n"
       << "\tCopyright (C) 2009-2017, the BxCppDev group       \n"
       << "\tFrancois Mauger, Xavier Garrido, Arnaud Chapon    \n"
       << "                                                    \n";
  out_ << "                                                    \n";
  return;
}

void app_print_help(boost::program_options::options_description & opts_,
                    std::ostream & out_)
{
  out_ << "Usage : " << app_name() << " [OPTION]...                                    \n";
  out_ << "                                                                            \n";
  out_ << "Inspect a manager of registered isotopes, elements and materials.           \n";
  out_ << "                                                                            \n";
  out_ << "Usage:                                                                      \n";
  out_ << "                                                                            \n";
  out_ << "  " << app_name() << " [[option]... ] [manager config file]  \n";
  out_ << "                                                                            \n";
  out_ << opts_;
  out_ << "                                                                            \n";
  out_ << "Examples:                                                                   \n";
  out_ << "                                                                            \n";
  out_ << "  List the list of registered isotopes:                                     \n";
  out_ << "                                                                            \n";
  out_ << "   $ " << app_name() << " \\                                                 \n";
  out_ << "       --manager-config \"${CONFIG_DIR}/materials/manager.conf\" \\         \n";
  out_ << "       --list-isotopes --with-decoration                                    \n";
  out_ << "                                                                            \n";
  out_ << "  Print a registered material:                                              \n";
  out_ << "                                                                            \n";
  out_ << "   $ " << app_name() << " \\                                                 \n";
  out_ << "       --manager-config \"${CONFIG_DIR}/materials/manager.conf\" \\         \n";
  out_ << "       --show-material \"Steel100Cr6\"                                      \n";
  out_ << "                                                                            \n";
  return;
}

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
