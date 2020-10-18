// (bx)genvtx_production.cxx
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

// Standard libraries:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <memory>

// Third Party:
// - Boost:
#include <boost/program_options.hpp>
// - Bayeux:
#include <bayeux/bayeux.h>
#include <bayeux/version.h>
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/kernel.h>
#include <datatools/ioutils.h>
#include <datatools/units.h>
#include <datatools/configuration/io.h>
#include <datatools/configuration/variant_service.h>
// - Bayeux/geomtools:
#include <geomtools/color.h>
// - Bayeux/genvtx:
#include <genvtx/genvtx_driver.h>

namespace dtc = datatools::configuration;
namespace po = boost::program_options;

//! \brief Application configuration/running parameters
struct app_config_params {
  std::string logging_label = "fatal";
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL; ///< Logging priority threshold
  bool help        = false;                   ///< Help flag
  bool no_splash   = false;                   ///< Do not print splash screen
  genvtx::genvtx_driver_params genvtx_params; ///< Parameters specific to the genvtx driver
  std::vector<std::string> GD_argv;           ///< ???
  dtc::variant_service::config variants;      ///< Parameters specific to variant support
};

/// Return the application name
std::string app_name();

/// Return kernel initialization flags for this application
uint32_t app_kernel_init_flags();

//! Print application splash screen
void app_print_splash(std::ostream & out_ = std::clog);

/// Build the list of general options description
void app_build_general_opts(boost::program_options::options_description &,
                            app_config_params &);

/// Print online help
void app_print_help(boost::program_options::options_description &,
                    std::ostream & out_ = std::clog);

/// Build genvtx driver initialization options
void app_driver_build_initialization_opts(boost::program_options::options_description &,
                                          genvtx::genvtx_driver_params &);

/// Build genvtx driver action options
void app_driver_build_action_opts(boost::program_options::options_description &,
                                  genvtx::genvtx_driver_params &);

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

    // Declare options for the driver:
    po::options_description optDriverInit("Driver initialization parameters");
    po::options_description optDriverAction("Driver action parameters");
    app_driver_build_initialization_opts(optDriverInit, params.genvtx_params);
    app_driver_build_action_opts(optDriverAction, params.genvtx_params);

    // Aggregate options:
    optPublic
      .add(optDesc)
      .add(optDriverInit)
      .add(optDriverAction)
      .add(optVariant)
      .add(optKernel);

    // Preprocessor for command line arguments:
    unsigned int vpp_flags = 0;
    // vpp_flags |= datatools::configuration::variant_preprocessor::FLAG_TRACE;
    vpp_flags |= datatools::configuration::variant_preprocessor::FLAG_REMOVE_QUOTES;
    datatools::configuration::variant_preprocessor vpp(vpp_flags);
    std::vector<std::string> preprocessed_arguments;
    vpp.preprocess_args_options(argc_, argv_, preprocessed_arguments);

    // Parse command line options:
    po::positional_options_description args;
    po::variables_map vm;
    po::parsed_options parsed =
      po::command_line_parser(preprocessed_arguments)
      .options(optPublic)
      .allow_unregistered()
      .run();

    // Collect all other options & args for the driver session:
    params.GD_argv = po::collect_unrecognized(parsed.options,
                                              po::include_positional);
    po::store(parsed, vm);
    po::notify(vm);

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

    bool run_session = true;
    if (params.help) {
      app_print_help(optPublic, std::cout);
      run_session = false;
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
          vserv->get_repository().tree_dump(std::cerr, "Repository:");
        }
      }

      // Processing of special options:
      if (vm.count("visu-spot-size")) {
        std::string spot_size_str = vm["visu-spot-size"].as<std::string>();
        std::istringstream iss(spot_size_str);
        double spot_size;
        std::string unit_symbol;
        std::string unit_label;
        DT_THROW_IF(!datatools::units::parse_value_with_unit(spot_size_str,
                                                             spot_size,
                                                             unit_symbol,
                                                             unit_label),
                    std::logic_error,
                    "Invalid format for vertex spot size!");
        DT_THROW_IF(unit_label != "length",  std::logic_error,
                    "Not a length unit for vertex spot size!");
        params.genvtx_params.visu_spot_size = spot_size;
      }
      params.genvtx_params.logging_label = params.logging_label;
      if (datatools::logger::is_debug(params.logging)) {
        params.genvtx_params.dump();
      }

      // The genvtx driver :
      genvtx::genvtx_driver GD;

      // Initialize the driver:
      GD.initialize(params.genvtx_params);

      // Run a one-shot session:
      GD.run();

      // Terminate the driver:
      GD.reset();

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
  return "bxgenvtx_production";
}

uint32_t app_kernel_init_flags()
{
  uint32_t kernel_init_flags = 0;
  kernel_init_flags |= datatools::init_no_help;
  kernel_init_flags |= datatools::init_no_splash;
  kernel_init_flags |= datatools::init_no_inhibit_libinfo;
  kernel_init_flags |= datatools::init_no_libinfo_logging;
  kernel_init_flags |= datatools::init_no_variant;
  kernel_init_flags |= datatools::init_no_inhibit_variant;
  kernel_init_flags |= datatools::init_no_locale_category;
  kernel_init_flags |= datatools::init_no_inhibit_qt_gui;
  kernel_init_flags |= datatools::init_no_inhibit_urnquery;
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

void app_print_splash(std::ostream & out_)
{
  out_ << "                                                    \n"
       << "\tB A Y E U X  -  G E N V T X    P R O D U C T I O N\n"
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
  out_ << "  Inspect and generate vertices from a vertex generator                     \n";
  out_ << "  provided by a vertex generator manager associated to a                    \n";
  out_ << "  geometry manager.                                                         \n";
  out_ << "                                                                            \n";
  out_ << opts_ << std::endl;
  out_ << "                                                                            \n";
  out_ << "Example:                                                                    \n";
  out_ << "                                                                            \n";
  out_ << "  List the available vertex generators:                                     \n";
  out_ << "                                                                            \n";
  out_ << "     " << app_name() << " \\                                                \n";
  out_ << "       --geometry-manager \"${CONFIG_DIR}/geometry/manager.conf\" \\        \n";
  out_ << "       --vertex-generator-manager \"${CONFIG_DIR}/vertex/manager.conf\" \\  \n";
  out_ << "       --list                                                               \n";
  out_ << "                                                                            \n";
  out_ << "  Show a given vertex generator:                                            \n";
  out_ << "                                                                            \n";
  out_ << "     " << app_name() << " \\                                                \n";
  out_ << "       --geometry-manager \"${CONFIG_DIR}/geometry/manager.conf\" \\        \n";
  out_ << "       --vertex-generator-manager \"${CONFIG_DIR}/vertex/manager.conf\" \\  \n";
  out_ << "       --show --vertex-generator \"scintillator_block.vg\"                  \n";
  out_ << "                                                                            \n";
  out_ << "  Generate some vertices, store them in a file, display them and            \n";
  out_ << "  serialize a display data object:                                          \n";
  out_ << "                                                                            \n";
  out_ << "     " << app_name() << " \\                                                \n";
  out_ << "       --geometry-manager \"${CONFIG_DIR}/geometry/manager.conf\" \\        \n";
  out_ << "       --vertex-generator-manager \"${CONFIG_DIR}/vertex/manager.conf\" \\  \n";
  out_ << "       --shoot \\                                                           \n";
  out_ << "       --prng-seed 314159 \\                                                \n";
  out_ << "       --number-of-vertices 10000 \\                                        \n";
  out_ << "       --vertex-generator \"scintillator_block.vg\" \\                      \n";
  out_ << "       --output-file \"vertices.data\"    \\                                \n";
  out_ << "       --visu \\                                                            \n";
  out_ << "       --visu-output-file \"vertices-visu-dd.data.gz\"                      \n";
  out_ << "                                                                            \n";
  out_ << "                                                                            \n";
  return;
}


void app_driver_build_initialization_opts(boost::program_options::options_description & opts_,
                                          genvtx::genvtx_driver_params & params_)
{
  namespace po = boost::program_options;
  opts_.add_options()
    ("dll-config",
     po::value<std::string>(&params_.LL_config),
     "Set the dynamic library loader configuration file.\n"
     "Example :\n"
     "  --dll-config \"dlls.lis\""
     )
    ("load-dll",
     po::value<std::vector<std::string> >(&params_.LL_dlls),
     "Load a dynamic library (DLL).      \n"
     "Example :                          \n"
     "  --load-dll \"foo\" --load-dll \"bar\" "
     )
    ("geometry-manager",
     po::value<std::string>(&params_.GeoMgrConfigFile),
     "Set the geometry manager configuration file.\n"
     "Example :                                   \n"
     "  --geometry-manager \"geo_mgr.conf\" "
     )
    ("vertex-generator-manager",
     po::value<std::string>(&params_.VGMgrConfigFile),
     "Set the vertex generator manager configuration file.\n"
     "Example :                                           \n"
     "  --vertex-generator-manager \"vg_mgr.conf\" "
     )
    ;
  return;
}

void app_driver_build_action_opts(boost::program_options::options_description & opts_,
                                  genvtx::genvtx_driver_params & params_)
{
  namespace po = boost::program_options;
  opts_.add_options()
    ("list,l",
     po::value<bool>(&params_.action_list)
     ->zero_tokens()
     ->default_value(false)
     ,
     "List the available vertex generators defined\n"
     "in the vertex generator manager.            \n"
     "Example :                                   \n"
     "  --list                                      "
     )
    ("vertex-generator,g",
     po::value<std::string>(&params_.generator_name),
     "Set the name of the vertex generator to be \n"
     "be shown or shot.                          \n"
     "Example :                                  \n"
     "  --vertex-generator \"calib_source_bulk.conf\""
     )
    ("show,w",
     po::value<bool>(&params_.action_show)
     ->zero_tokens()
     ->default_value(false)
     ,
     "Print a vertex generator defined\n"
     "in the vertex generator manager.\n"
     "Example :                       \n"
     "  --show                        \n"
     "  --vertex-generator \"calib_source_bulk.conf\""
     )
    ("shoot,s",
     po::value<bool>(&params_.action_shoot)
     ->zero_tokens()
     //->default_value(false)
     ,
     "Generate random vertices from a given \n"
     "vertex generator defined in the       \n"
     "vertex generator manager.             \n"
     "Example :                             \n"
     "  --shoot                             \n"
     "  --vertex-generator \"calib_source_bulk.conf\""
     )
    ("prng-type",
     po::value<std::string>(&params_.prng_type)
     //->default_value("taus2")
     ,
     "Set the type of the random number generator.\n"
     "Example :                                   \n"
     "  --prng-type \"taus2\"   "
     )
    ("prng-seed",
     po::value<int>(&params_.prng_seed),
     "Set the seed for the random number generator.\n"
     "Example :                                    \n"
     "  --prng-seed 314159   "
     )
    ("number-of-vertices,n",
     po::value<int>(&params_.nshoots),
     "Set the number of vertices to be generated.\n"
     "Example :                                  \n"
     "  --number-of-vertices 1000   "
     )
    ("vertex-modulo,m",
     po::value<int>(&params_.shoot_modulo),
     "Set the modulo on number of generated\n"
     "vertices.                            \n"
     "Example :                            \n"
     "  --vertex-modulo 100 "
     )
    ("output-file,o",
     po::value<std::string>(&params_.VtxOutputFile),
     "Set the output file name where to store\n"
     "generated vertices.                    \n"
     "Example :                              \n"
     "  --output-file \"vertexes.data\" "
     )
    ("visu",
     po::value<bool>(&params_.action_visu)
     ->zero_tokens()
     ->default_value(false)
     ,
     "Activate the visualization of randomly\n"
     "generated vertices.                   \n"
     "Example :                             \n"
     "  --visu  "
     )
    ("visu-max-counts",
     po::value<int>(&params_.visu_max_counts)
     //->default_value(10000)
     ,
     "Set the maximum number of randomly \n"
     "generated vertices to be displayed.\n"
     "Example :                          \n"
     "  --visu-max-counts 100  "
     )
    ("visu-spot-zoom",
     po::value<double>(&params_.visu_spot_zoom)
     //->default_value(1.0)
     ,
     "Set the zoom factor of the randomly\n"
     "generated vertices to be displayed.\n"
     "Example :                          \n"
     "  --visu-spot-zoom 3.0 "
     )
    ("visu-spot-size",
     po::value<std::string>()
     ,
     "Set the size (in mm) of the randomly\n"
     "generated vertices to be displayed. \n"
     "Example :                           \n"
     "  --visu-spot-size \"3.0 mm\"      "
     )
    ("visu-object",
     po::value<std::string>(&params_.visu_object)
     ,
     "Set the identifier of the object  \n"
     "to be displayed.                  \n"
     "Example :                         \n"
     "  --visu-object \"calorimeter.model\""
     )
    ("visu-spot-color",
     po::value<std::string>(&params_.visu_spot_color)
     ->default_value(geomtools::color::magenta())
     ,
     "Set the color of the randomly      \n"
     "generated vertices to be displayed.\n"
     "Example :                          \n"
     "  --visu-spot-color \"magenta\"    "
     )
    ("visu-output",
     po::value<bool>(&params_.action_visu_store_dd)
     ->zero_tokens()
     //->default_value(false)
     ,
     "Activate display data storage.\n"
     "Example :                     \n"
     "  --visu-output               "
     )
    ("visu-output-file",
     po::value<std::string>(&params_.visu_store_dd_out),
     "Set the output file name where to  \n"
     "store generated vertices converted \n"
     " in some display data object.      \n"
     "Example :                          \n"
     "  --visu-output-file \"vertexes-dd.data\""
     )
    ("visu-view",
     po::value<std::string>(&params_.visu_view),
     "Set the display view ('xyz', 'xy', 'xz', 'yz') \n"
     "Example :                          \n"
     "  --visu-view \"xy\""
     )
    ;
  return;
}

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
