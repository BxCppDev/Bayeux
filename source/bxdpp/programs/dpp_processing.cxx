/** \file dpp_processing.cxx
 *
 * Creation date : 2011-07-03
 * Last modified : 2017-05-19
 *
 * Copyright (C) 2011-2017 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This file is part of Bayeux.
 *
 * Bayeux is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Bayeux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Bayeux. If not, see <http://www.gnu.org/licenses/>.
 *
 * Description:
 *
 *  A generic program that performs processing of data records along
 *  a pipeline of data processing modules.
 *
 */

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <memory>

// Third party:
// - Boost:
#include <boost/program_options.hpp>

// - Bayeux:
#include <bayeux/bayeux.h>
#include <bayeux/version.h>

// - Bayeux/datatools:
#include <datatools/kernel.h>
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/version.h>
#include <datatools/configuration/variant_service.h>
#include <datatools/configuration/variant_repository.h>
#include <datatools/configuration/variant_registry.h>

// This project:
#include <dpp/dpp_config.h>
#include <dpp/dpp_driver.h>

namespace bpo = boost::program_options;
namespace dtc = datatools::configuration;

struct ui {

  //! \brief Application configuration parameters
  struct app_config_params {
    //! Logging level
    std::string                  logging_label = "fatal";
    datatools::logger::priority  logging = datatools::logger::PRIO_FATAL;
    bool                         no_splash = false; //!< Do not print splash screen
    dtc::variant_service::config variants; //!< Variant support parameters
    dpp::dpp_driver_params       driver;   //!< dpp driver parameters
  };

  /// Return the application name
  static std::string app_name();

  /// Print application usage (supported options and arguments)
  static void app_print_usage(std::ostream & out_,
                              const bpo::options_description & desc_);

  //! Print application splash screen
  static void app_print_splash(std::ostream & out_ = std::clog);

  /// Print application version
  static void app_version(std::ostream & out_);

  /// Build options
  static void app_build_opts(boost::program_options::options_description & desc_,
                             app_config_params & params_);

  /// Build driver options
  static void app_build_driver_opts(boost::program_options::options_description & desc_,
                                    dpp::dpp_driver_params & drv_params_);

  /// Return kernel initialization flags for this application
  static uint32_t app_kernel_init_flags();

  static void app_dump_debug(std::ostream & out_, const dtc::variant_repository & vrep_);

};

int main(int argc_, char ** argv_)
{
  bayeux::initialize(argc_, argv_, ui::app_kernel_init_flags());
  int error_code = EXIT_SUCCESS;
  ui::app_config_params params;

  try {
    // Describe driver's command line options:
    bpo::options_description optDesc("Options");
    ui::app_build_opts(optDesc, params);
    ui::app_build_driver_opts(optDesc, params.driver);

    // Describe driver's command line arguments:
    bpo::positional_options_description args;
    args.add("input-file", -1);

    // Declare options for variant support:
    bpo::options_description optVariant("Variant support");
    uint32_t po_init_flags = 0;
    po_init_flags |= dtc::variant_service::NO_LABEL;
    po_init_flags |= dtc::variant_service::NO_LOGGING;
    po_init_flags |= dtc::variant_service::NO_TUI;
    dtc::variant_service::init_options(optVariant,
                                       params.variants,
                                       po_init_flags);

    // Declare options for kernel:
    bpo::options_description optKernel("Kernel options");
    datatools::kernel::param_type paramsKernel;
    datatools::kernel::build_opt_desc(optKernel,
                                      paramsKernel,
                                      ui::app_kernel_init_flags());

    // Aggregate options:
    bpo::options_description optPublic;
    optPublic
      .add(optDesc)
      .add(optVariant)
      .add(optKernel);

    // Parse options:
    bpo::variables_map vMap;
    bpo::parsed_options parsed =
      bpo::command_line_parser(argc_, argv_)
      .options(optPublic)
      .positional(args)
      // .allow_unregistered()
      .run();
    /*
      std::vector<std::string> unrecognized_options
      = bpo::collect_unrecognized(parsed.options,
      bpo::include_positional);
    */
    bpo::store(parsed, vMap);
    bpo::notify(vMap);

    // Handle any non-bound options:
    if (vMap.count("help") && vMap["help"].as<bool>()) {
      ui::app_print_usage(std::cout, optPublic);
      error_code = -1;
    }

    if (vMap.count("version") && vMap["version"].as<bool>()) {
      ui::app_version(std::cout);
      error_code = -1;
    }

    // Run dpp driver session:
    if (error_code == EXIT_SUCCESS) {

      if (vMap.count("logging")) {
        params.logging_label = vMap["logging"].as<std::string>();
        datatools::logger::priority prio
          = datatools::logger::get_priority(params.logging_label);
        if (prio != datatools::logger::PRIO_UNDEFINED) {
          params.logging = prio;
          params.driver.logging_label = datatools::logger::get_priority_label(prio);
        } else {
          DT_THROW(std::logic_error,
                   "Invalid logging priority '" << params.logging_label << "' !");
        }
      }

      if (! params.no_splash) {
        ui::app_print_splash(std::cerr);
      }

      // Variant service:
      std::unique_ptr<dtc::variant_service> vserv;
      if (params.variants.is_active()) {
        // Create and start the variant service:
        vserv.reset(new dtc::variant_service);
        vserv->configure(params.variants);
        if (datatools::logger::is_debug(params.logging)) {
          ui::app_dump_debug(std::cerr, vserv->get_repository());
        }
        // Start the variant service:
        vserv->start();
      }

      // dpp driver session :
      if (params.logging >= datatools::logger::PRIO_DEBUG) {
        DT_LOG_DEBUG(params.logging, "Configuration setup:");
        params.driver.tree_dump(std::clog, "", "[debug]: ");
      }
      dpp::dpp_driver drv;
      drv.setup(params.driver);
      drv.initialize();
      drv.run();
      drv.reset();

      // Stop the variant service:
      if (vserv) {
        if (vserv->is_started()) {
          // Stop the variant service:
          vserv->stop();
        }
        vserv.reset();
      }
    } // end of session run

  } catch (const std::exception & x) {
    DT_LOG_FATAL(params.logging, ui::app_name() << ": " << x.what ());
    error_code = EXIT_FAILURE;
  } catch (...) {
    DT_LOG_FATAL(params.logging, ui::app_name() << ": " << "Unexpected error !");
    error_code = EXIT_FAILURE;
  }

  bayeux::terminate();
  return error_code;
}

// Definitions:

std::string ui::app_name()
{
  return "bxdpp_processing";
}

void ui::app_version(std::ostream & out_)
{
  out_ << app_name() << " " << BAYEUX_LIB_VERSION << std::endl;
  return;
}

void ui::app_print_splash(std::ostream & out_)
{
  out_ << "                                                   \n"
       << "\tB A Y E U X  -  D P P    P R O C E S S I N G     \n"
       << "\tVersion " << BAYEUX_LIB_VERSION << "             \n"
       << "                                                   \n"
       << "\tCopyright (C) 2009-2017, the BxCppDev group      \n"
       << "\tFrancois Mauger, Xavier Garrido                  \n"
       << "                                                   \n";
  out_ << "                                                   \n";
  return;
}

uint32_t ui::app_kernel_init_flags()
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

void ui::app_print_usage(std::ostream & out_, const bpo::options_description & opts_)
{
  out_ << app_name() << " -- A generic data chain processing program" << std::endl;
  out_ << "Usage : " << std::endl;
  out_ << "  " << app_name() << " [options] [arguments]" << std::endl;
  out_ << opts_ << std::endl;
  out_ << std::endl;
  out_ << "Examples : " << std::endl;
  out_ << std::endl;
  out_ << "  Process the chain of 'my_moduleX' (X=1,2,3) data processing modules from the 'mydllY' libraries (Y=1,2): " << std::endl;
  out_ << std::endl;
  out_ << "  $ " << app_name() << "  \\" << std::endl;
  out_ << "          -c my_module_manager.conf   \\" << std::endl;
  out_ << "          -l my_dll1                  \\" << std::endl;
  out_ << "          -l my_dll2                  \\" << std::endl;
  out_ << "          -i my_input_events_0.txt.gz \\" << std::endl;
  out_ << "          -i my_input_events_1.txt.gz \\" << std::endl;
  out_ << "          -i my_input_events_2.txt.gz \\" << std::endl;
  out_ << "          -m my_module1               \\" << std::endl;
  out_ << "          -m my_module2               \\" << std::endl;
  out_ << "          -m my_module3               \\" << std::endl;
  out_ << "          -o my_output_events.txt.gz    " << std::endl;
  out_ << std::endl;
  out_ << "  Simple use cases : " << std::endl;
  out_ << std::endl;
  out_ << "  $ " << app_name() << "  \\" << std::endl;
  out_ << "          -c ${DPP_TESTING_DIR}/config/test_module_manager.conf \\" << std::endl;
  out_ << "          -i ${DPP_TESTING_DIR}/data/data_0.txt.gz     \\" << std::endl;
  out_ << "          -m clear         \\" << std::endl;
  out_ << "          -m chain1        \\" << std::endl;
  out_ << "          -o processed.xml   " << std::endl;
  out_ << std::endl;
  out_ << "  $ " << app_name() << "   \\" << std::endl;
  out_ << "          -c ${DPP_TESTING_DIR}/config/test_module_manager.conf \\" << std::endl;
  out_ << "          -M 10            \\" << std::endl;
  out_ << "          -m chain1        \\" << std::endl;
  out_ << "          -m dump_in_file    " << std::endl;
  out_ << std::endl;
  out_ << "  $ " << app_name() << "    \\" << std::endl;
  out_ << "          -c ${DPP_TESTING_DIR}/config/test_module_manager.conf \\" << std::endl;
  out_ << "          -M 10            \\" << std::endl;
  out_ << "          -m chain1        \\" << std::endl;
  out_ << "          -o processed.xml   " << std::endl;
  out_ << std::endl;
  return;
}

void ui::app_build_opts(boost::program_options::options_description & opts_,
                        app_config_params & params_)
{
  namespace po = boost::program_options;
  opts_.add_options()
    ("help,h", bpo::value<bool>()
     ->zero_tokens()
     ->default_value(false),
     "produce help message.")
    ("version", bpo::value<bool>()
     ->zero_tokens()
     ->default_value(false),
     "print version number then exit.")
    ("logging,P",
     bpo::value<std::string>()->default_value("fatal"),
     "set the logging priority.")
    ("no-splash",
     po::value<bool>(&params_.no_splash)->zero_tokens()->default_value(false),
     "do not print the splash screen.\n"
     )
    ;
  return;
}

void ui::app_build_driver_opts(boost::program_options::options_description & opts_,
                               dpp::dpp_driver_params & params_)
{
  namespace po = boost::program_options;
  opts_.add_options()
    ("load-dll,l",
     bpo::value<std::vector<std::string> >(&params_.LL_dlls),
     "set a DLL to be loaded.")
    ("dlls-config,L",
     bpo::value<std::string>(&params_.LL_config),
     "set the DLL loader configuration file.")
    ("modulo,%",
     bpo::value<int>(&params_.print_modulo)->default_value(10),
     "set the modulo print period for data record.")
    ("max-records,M",
     bpo::value<int>(&params_.max_records)->default_value(0),
     "set the maximum number of data records to be processed.")
    ("no-max-records,X",
     bpo::value<bool>(&params_.no_max_records)->zero_tokens()->default_value(false),
     "Do not limit the maximum number of data records to be processed.")
    ("module,m",
     bpo::value<std::vector<std::string> >(&params_.module_names),
     "add a module in the pipeline (optional).")
    ("module-manager-config,c",
     bpo::value<std::string>(&params_.module_manager_config_file),
     "set the module manager configuration file.")
    ("input-file,i",
     bpo::value<std::vector<std::string> >(&params_.input_files),
     "set an input file (optional).")
    ("output-file,o",
     bpo::value<std::vector<std::string> >(&params_.output_files),
     "set the output file (optional).")
    ("preserve-existing-files,x",
     bpo::value<bool>(&params_.preserve_existing_files)->zero_tokens()->default_value(false),
     "preserve existing files (recommended).")
    ("max-records-per-output-file,O",
     bpo::value<int>(&params_.max_records_per_output_file)->default_value(0),
     "set the maximum number of data records per output file.")
    ("slice-start,s",
     bpo::value<int>(&params_.slice_start)->default_value(-1),
     "set the index of the first data record to be processed.")
    ("slice-stop,S",
     bpo::value<int>(&params_.slice_stop)->default_value(-1),
     "set the index of the last data record to be processed.")
    ("slice-width,w",
     bpo::value<int>(&params_.slice_width)->default_value(-1),
     "set the width of the slice of data records to be processed.")
    ("slice-store-out,T",
     bpo::value<bool>(&params_.slice_store_out)->zero_tokens()->default_value(false),
     "set the flag to store only the sliced data records.")
    ;
  return;
}

void ui::app_dump_debug(std::ostream & out_, const dtc::variant_repository & vrep_)
{
  out_ << "Variant repository : '" << vrep_.get_name() << "'" << std::endl;
  out_ << "  Organization : '" << vrep_.get_organization() << "'" << std::endl;
  out_ << "  Application  : '" << vrep_.get_application() << "'" << std::endl;
  out_ << "  Description  : '" << vrep_.get_terse_description() << "'" << std::endl;
  out_ << "  Accomplished : " << vrep_.is_accomplished() << "" << std::endl;
  out_ << "  Locked       : " << vrep_.is_locked() << "'" << std::endl;
  out_ << "  Registries   : [" << vrep_.get_registries().size() << "]" << std::endl;

  std::vector<std::string> vreg_keys;
  vrep_.build_ordered_registry_keys(vreg_keys);
  for (std::size_t ivreg = 0; ivreg < vreg_keys.size(); ivreg++) {
    const std::string & vreg_key = vreg_keys[ivreg];
    const dtc::variant_registry & vreg = vrep_.get_registry(vreg_key);
    const std::string & vreg_name = vreg.get_name();
    // const std::string & vreg_display_name = vreg.get_display_name();
    const std::string & vreg_description = vreg.get_terse_description();
    bool vreg_accomplished = vreg.is_accomplished();
    out_ << "  Variant register : '" << vreg_name << "'" << std::endl;
    out_ << "    Description  : '" << vreg_description << "'" << std::endl;
    out_ << "    Accomplished : " << vreg_accomplished << "" << std::endl;
  }

  return;
}

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
