/// \file g4_production.cxx
/* Description :
 *
 *   The main Bayeux/mctools Geant4 simulation program.
 *
 * Licence :
 *
 * Copyright (C) 2011-2017 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 */

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <sstream>
#include <exception>
#include <vector>

// Third party:
// - Boost:
#include <boost/program_options.hpp>

// - Bayeux
#include <bayeux/bayeux.h>
#include <bayeux/version.h>
// - Bayeux/datatools:
#include <datatools/datatools_config.h>
#include <datatools/utils.h>
#include <datatools/ioutils.h>
#include <datatools/multi_properties.h>
#include <datatools/library_loader.h>
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/kernel.h>
#include <datatools/configuration/io.h>
#include <datatools/configuration/variant_service.h>
// - Bayeux/mygsl:
#include <mygsl/random_utils.h>
// - Bayeux/geomtools:
#include <geomtools/geomtools.h>
// - Bayeux/mctools:
#include <mctools/version.h>
#include <mctools/g4/manager.h>
#include <mctools/g4/manager_parameters.h>

#if DATATOOLS_WITH_QT_GUI == 1
#include <QStyleFactory>
#include <QApplication>
#endif // DATATOOLS_WITH_QT_GUI == 1


/// \brief User interface of the (bx)g4_production application:
struct ui {

  static
  std::string app_name();

  static
  void splash(std::ostream & out_ = std::clog);

  static
  void print_usage(const boost::program_options::options_description & opts_,
                   std::ostream & out_ = std::clog);

  static
  void build_opts(boost::program_options::options_description & opts_,
                  mctools::g4::manager_parameters & params_);

  static
  void process_opts(const boost::program_options::variables_map & vm_,
                    const boost::program_options::options_description & opts_,
                    mctools::g4::manager_parameters & params_);

  static
  void print_examples(std::ostream      & out_   = std::clog,
                      const std::string & name_  = "bxg4_production",
                      const std::string & title_ = "");

};

namespace dtc = datatools::configuration;

/// Return kernel initialization flags for this application
uint32_t app_kernel_init_flags();

int main(int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  bayeux::initialize(argc_, argv_, app_kernel_init_flags());
  try {
    bool run = true;

    // Configuration parameters for the G4 manager:
    mctools::g4::manager_parameters params;
    params.set_defaults();
    dtc::variant_service::config variants_params;

    // Shortcut for Boost/program_options namespace :
    namespace po = boost::program_options;

    // Variables map:
    po::variables_map vm;
    po::options_description optPublic;
    po::options_description optVariant("Variant support");

    try {

      // Declare options for variant support:
      uint32_t po_init_flags = 0;
      po_init_flags |= dtc::variant_service::NO_LABEL;
      po_init_flags |= dtc::variant_service::NO_LOGGING;
      // po_init_flags |= dtc::variant_service::PROFILE_LOAD_DONT_IGNORE_UNKNOWN;
      po_init_flags |= dtc::variant_service::NO_TUI;
      dtc::variant_service::init_options(optVariant,
                                         variants_params,
                                         po_init_flags);

      // Describe command line switches :
      po::options_description opts("Allowed options");
      ui::build_opts(opts, params);

      // Describe Bayeux/datatools kernel options:
      po::options_description optKernel("Kernel options");
      datatools::kernel::param_type kparams;
      datatools::kernel::build_opt_desc(optKernel, kparams, app_kernel_init_flags());

      // Preprocessor for command line arguments:
      unsigned int vpp_flags = 0;
      // vpp_flags |= datatools::configuration::variant_preprocessor::FLAG_TRACE;
      vpp_flags |= datatools::configuration::variant_preprocessor::FLAG_REMOVE_QUOTES;
      datatools::configuration::variant_preprocessor vpp(vpp_flags);
      std::vector<std::string> preprocessed_arguments;
      vpp.preprocess_args_options(argc_, argv_, preprocessed_arguments);

      // Collect all supported options in one container:
      optPublic.add(opts);
      optPublic.add(optVariant);
      optPublic.add(optKernel);

      // Configure the parser:
      po::command_line_parser cl_parser(argc_, argv_);
      cl_parser.options(optPublic);
      // cl_parser.positional(args);
      // cl_parser.allow_unregistered();

      // Parse:
      po::parsed_options parsed = cl_parser.run();

      // Fill and notify a variable map:
      po::store(parsed, vm);
      po::notify(vm);
    } catch (std::exception & po_error) {
      ui::print_usage(optPublic, std::cerr);
      throw;
    }

    /*
    // Collect all other options & args for the driver session:
    std::vector<std::string> unrecognized_opts;
    unrecognized_opts = po::collect_unrecognized(parsed.options,
    po::include_positional);
    */

    // Parse specific options:
    bool do_splash = true;
    if (vm.count("splash")) {
      if (vm["splash"].as<bool>()) {
        do_splash = true;
      } else {
        do_splash = false;
      }
    }

    if (vm.count("no-splash")) {
      if (vm["no-splash"].as<bool>()) {
        do_splash = false;
      } else {
        do_splash = true;
      }
    }

    if (do_splash) {
      ui::splash(std::clog);
    }

    if (vm.count("help")) {
      if (vm["help"].as<bool>()) {
        ui::print_usage(optPublic, std::cout);
        run = false;
      }
    }

    if (run) {
      // Fill the configuration data structure from program options:
      ui::process_opts(vm, optPublic, params);

      // DLL loading:
      datatools::library_loader dll_loader(params.dll_loader_config);
      if (params.g4_visu) {
        std::string g4_vis_dll = "G4visXXX";
        if (std::find(params.dlls.begin(), params.dlls.end(), g4_vis_dll) == params.dlls.end()) {
          DT_LOG_NOTICE(logging, "Force the loading of the DLL '" << g4_vis_dll << "' DLL...");
          params.dlls.push_back(g4_vis_dll);
        }
        std::string g4_opengl_dll = "G4OpenGL";
        if (std::find(params.dlls.begin(), params.dlls.end(), g4_opengl_dll) == params.dlls.end()) {
          DT_LOG_NOTICE(logging, "Force the loading of the DLL '" << g4_opengl_dll << "' DLL...");
          params.dlls.push_back(g4_opengl_dll);
        }
      }
      for (const std::string & dll_name :  params.dlls) {
        DT_LOG_NOTICE(logging, "Loading DLL '" << dll_name << "'...");
        DT_THROW_IF (dll_loader.load (dll_name) != EXIT_SUCCESS,
                     std::logic_error,
                     "Loading DLL '" << dll_name << "' failed !");
      }

      // Variant service:
      std::unique_ptr<dtc::variant_service> vserv;
      if (variants_params.is_active()) {
        // Create and start the variant service:
        vserv.reset(new dtc::variant_service);
        vserv->configure(variants_params);
        vserv->start();
        // vserv->get_repository().tree_dump(std::cerr, "Repository:");
      }

      {
        // Declare the simulation manager:
        DT_LOG_NOTICE(logging, "Instantiate the simulation manager...");
        mctools::g4::manager sim_manager;

        // Configure the simulation manager:
        DT_LOG_NOTICE(logging, "Setup the simulation manager...");
        mctools::g4::manager_parameters::setup(params, sim_manager);

        // Run the simulation session :
        DT_LOG_NOTICE(logging, "Simulation session starts...");
        sim_manager.run_simulation();
        DT_LOG_NOTICE(logging, "Simulation session is stopped.");

        // Explicitely terminate the simulation manager:
        DT_LOG_NOTICE(logging, "Terminate the simulation manager...");
        if (sim_manager.is_initialized()) {
          sim_manager.reset();
        }

        DT_LOG_NOTICE(logging, "Simulation manager is terminated.");
      } // Destructor is invoked here.

      if (vserv) {
        // Stop the variant service:
        vserv->stop();
        vserv.reset();
      }
    }

    DT_LOG_TRACE(logging, "The end.");
  } catch (std::exception & x) {
    DT_LOG_FATAL(logging, x.what());
    error_code = EXIT_FAILURE;
  } catch (...) {
    DT_LOG_FATAL(logging, "Unexpected error !");
    error_code = EXIT_FAILURE;
  }
  DT_LOG_TRACE(logging, "g4_production ends here.");

  bayeux::terminate();
  return error_code;
}

std::string ui::app_name()
{
  return "bxg4_production";
}

void ui::splash(std::ostream & out_)
{
  out_ << "\n"
       << "     B A Y E U X  - G 4   P R O D U C T I O N";
  out_ << "\n";
  out_ << "     Version " << bayeux::version::get_version() << "\n";
  out_ << "     " << app_name() << "\n";
  out_ << "\n"
       << "     Copyright (C) 2011-2017, the BxCppDev group\n"
       << "     Francois Mauger, Xavier Garrido and Arnaud Chapon\n"
       << "\n";
  return;
}

void ui::print_usage(const boost::program_options::options_description & opts_,
                     std::ostream & out_)
{
  out_ << app_name() << " -- A generic GEANT4 simulation wrapper" << std::endl;
  out_ << std::endl;
  out_ << "Usage : " << std::endl;
  out_ << std::endl;
  out_ << "  " << app_name() << " [OPTIONS] [ARGUMENTS] "
       << std::endl;
  out_ << std::endl;
  out_ << opts_ << std::endl;
  ui::print_examples(out_, app_name(), "Examples : ");
  out_ << std::endl;
  return;
}

void ui::build_opts(boost::program_options::options_description & opts_,
                    mctools::g4::manager_parameters & params_)
{
  namespace po = boost::program_options;
  opts_.add_options()

    ("help,h",
     po::value<bool>()
     ->zero_tokens()
     ->default_value(false),
     "Produce help message")

    ("splash",
     po::value<bool>()
     ->zero_tokens(),
     "Produce a splash screen")

    ("no-splash",
     po::value<bool>()
     ->zero_tokens(),
     "Do not produce a splash screen")

    ("logging-priority,g",
     po::value<std::string>(&params_.logging)
     ->default_value("fatal")
     ->value_name("level"),
     "Set the logging priority threshold")

    ("number-of-events,n",
     po::value<uint32_t>(&params_.number_of_events)
     ->default_value(mctools::g4::manager::NO_LIMIT)
     ->value_name("integer"),
     "Set the number of events to be simulated")

    ("number-of-events-modulo,m",
     po::value<uint32_t>(&params_.number_of_events_modulo)
     ->default_value(0)
     ->value_name("integer"),
     "Set the event print period (0 means no print)")

    ("interactive,i",
     po::value<bool>(&params_.interactive)
     ->zero_tokens()
     ->default_value(false),
     "Run simulation in interactive mode")

    ("batch,b",
     po::value<bool>()
     ->zero_tokens()
     ->default_value(false),
     "Run simulation in batch mode")

    ("config,c",
     po::value<std::string>()
     ->value_name("filename"),
     "Set the manager configuration file")

    ("vertex-generator-name",
     po::value<std::string>()
     ->value_name("name"),
     "Set the name of the vertex generator")

    ("vertex-generator-seed",
     po::value<int>(&params_.vg_seed)
     ->default_value(0)
     ->value_name("integer"),
     "Set the seed of the random number generator associated to the vertex generator")

    ("event-generator-name",
     po::value<std::string>()
     ->value_name("name"),
     "Set the name of the event generator")

    ("event-generator-seed",
     po::value<int>(&params_.eg_seed)
     ->default_value(0)
     ->value_name("integer"),
     "Set the seed of the random number generator associated to the event generator")

    ("shpf-seed",
     po::value<int>(&params_.shpf_seed)
     ->default_value(0)
     ->value_name("integer"),
     "Set the seed of the random number generator associated to the step hit processors")

    ("g4-manager-seed",
     po::value<int>(&params_.mgr_seed)
     ->default_value(0)
     ->value_name("integer"),
     "Set the seed of the random number generator associated to the G4 manager")

    ("input-prng-seeds-file",
     po::value<std::string>()
     ->value_name("filename"),
     "Set the file from which to load the seeds of the random number generators")

    ("output-prng-seeds-file",
     po::value<std::string>()
     ->value_name("filename"),
     "Set the file to store the seeds of the random number generators")

    ("input-prng-states-file",
     po::value<std::string>()
     ->value_name("filename"),
     "Set the file from which to load the backuped internal states of the random number generators(PRNG)")

    ("output-prng-states-file",
     po::value<std::string>()
     ->value_name("filename"),
     "Set the file to store/backup the internal states of the random number generators(PRNG)")

    ("output-data-file",
     po::value<std::string>()
     ->value_name("filename"),
     "Set the file to store the generated simulated data")

    ("output-data-format",
     po::value<std::string>()
     ->value_name("format"),
     "Set the data format used for the generated simulated data\n"
     "Supported formats are:\n"
     " \"bank\" : \tsimulated events are recorded within 'datatools::things' objects\n"
     " \"plain\" : \tsimulated events are recorded as 'mctools::simulated_data' objects"
     )

    ("output-data-bank",
     po::value<std::string>()
     ->default_value("SD")
     ->value_name("name"),
     "Set the data bank label used to host the generated simulated data (\"bank\" format only")

    ("g4-visu",
     po::value<bool>(&params_.g4_visu)
     ->zero_tokens()
     ->default_value(false),
     "G4 visualization(only in interactive mode)")

    ("no-g4-visu",
     po::value<bool>()
     ->zero_tokens()
     ->default_value(false),
     "No G4 visualization (only in interactive mode)")

    ("g4-macro",
     po::value<std::string>()
     ->value_name("filename"),
     "Set the G4 macro file (only if interactive G4 visualization)")

    ("using-time-statistics",
     po::value<bool>(&params_.using_time_stat)
     ->zero_tokens()
     ->default_value(false),
     "Print time statistics at the end of the run")

    ("forbid-private-hits",
     po::value<bool>(&params_.forbid_private_hits)
     ->zero_tokens()
     ->default_value(false),
     "Do not allow private (un-official) MC hits")

    ("dont-save-no-sensitive-hit-events",
     po::value<bool>(&params_.dont_save_no_sensitive_hit_events)
     ->zero_tokens()
     ->default_value(false),
     "Do not save events without sensitive MC hits")

    ("run-header-footer",
     po::value<bool>(&params_.use_run_header_footer)
     ->zero_tokens()
     ->default_value(false),
     "store the run header and footer info together with the output data file")

    ("output-profiles",
     po::value<std::string>()
     ->value_name("name"),
     "Set the name for activated output profiles.\n"
     "Example :\n"
     "  --output-profiles \"calo_details\""
     )

    ("dlls-config,L",
     po::value<std::string> (&params_.dll_loader_config)
     ->value_name("filename"),
     "Set the DLL loader configuration file.\n"
     "Example :\n"
     "  --dlls-config \"dlls.conf\""
     )

    ("load-dll,l",
     po::value<std::vector<std::string> >(&params_.dlls)
     ->value_name("libname@path"),
     "Set a DLL to be loaded.\n"
     "Example :\n"
     "  --load-dll \"G4VisXXX\""
     )

    ; // end of options' description
  return;
}

void ui::process_opts( const boost::program_options::variables_map & vm_,
                       const boost::program_options::options_description & /*opts_*/,
                       mctools::g4::manager_parameters & params_)
{

  // Fetch the opts/args :

  if (vm_.count("batch")) {
    if (vm_["batch"].as<bool>()) {
      params_.interactive = false;
    }
  }

  if (params_.interactive) {
    if (vm_.count("g4-visu")) {
      if (vm_["g4-visu"].as<bool>()) {
        params_.g4_visu = true;
      }
    }

    if (vm_.count("no-g4-visu")) {
      if (vm_["no-g4-visu"].as<bool>()) {
        params_.g4_visu = false;
      }
    }
  } else {
    params_.g4_visu = false;
  }

  if (vm_.count("config")) {
    params_.manager_config_filename = vm_["config"].as<std::string>();
  }

  if (vm_.count("vertex-generator-name")) {
    params_.vg_name = vm_["vertex-generator-name"].as<std::string>();
  }

  if (vm_.count("event-generator-name")) {
    params_.eg_name = vm_["event-generator-name"].as<std::string>();
  }

  if (vm_.count("input-prng-seeds-file")) {
    params_.input_prng_seeds_file
      = vm_["input-prng-seeds-file"].as<std::string>();
  }

  if (vm_.count("output-prng-seeds-file")) {
    params_.output_prng_seeds_file
      = vm_["output-prng-seeds-file"].as<std::string>();
  }

  if (vm_.count("input-prng-states-file")) {
    params_.input_prng_states_file
      = vm_["input-prng-states-file"].as<std::string>();
  }

  if (vm_.count("output-prng-states-file")) {
    params_.output_prng_states_file
      = vm_["output-prng-states-file"].as<std::string>();
  }

  if (vm_.count("output-data-file")) {
    params_.output_data_file
      = vm_["output-data-file"].as<std::string>();
  }

  if (vm_.count("output-data-format")) {
    params_.output_data_format
      = vm_["output-data-format"].as<std::string>();
  }

  if (vm_.count("output-data-bank")) {
    params_.output_data_bank_label
      = vm_["output-data-bank"].as<std::string>();
  }

  if (vm_.count("output-profiles")) {
    params_.output_profiles_activation_rule
      = vm_["output-profiles"].as<std::string>();
  }

  if (vm_.count("g4-macro")) {
    params_.g4_macro
      = vm_["g4-macro"].as<std::string>();
  }

  return;
}

void ui::print_examples (std::ostream & a_out,
                         const std::string & a_name,
                         const std::string & a_title)
{
  if (! a_title.empty ()) {
    a_out << a_title << std::endl;
  }
  a_out << std::endl;
  a_out << " 1) Interactive test:" << std::endl;
  a_out << "    " << a_name << " \\" << std::endl;
  a_out << "         --interactive \\" << std::endl;
  a_out << "         --g4-visu \\" << std::endl;
  a_out << "         --vertex-generator-name \"source_bulk.vg\"  \\" << std::endl;
  a_out << "         --vertex-generator-seed 0 \\" << std::endl;
  a_out << "         --event-generator-name \"K40\" \\" << std::endl;
  a_out << "         --event-generator-seed 0  \\" << std::endl;
  a_out << "         --g4-manager-seed 0       \\" << std::endl;
  a_out << "         --shpf-seed 0             \\" << std::endl;
  a_out << "         --output-prng-seeds-file mc_g4_production.seeds   \\" << std::endl;
  a_out << "         --output-prng-states-file mc_g4_production.states \\" << std::endl;
  a_out << "         --config ${CONFIG_DIR}/simulation/manager.conf \\" << std::endl;
  a_out << "                  ${CONFIG_DIR}/simulation/geant4_visualization.macro" << std::endl;
  a_out << std::endl;
  a_out << " 2) Run the simulation in batch mode:" << std::endl;
  a_out << "    " << a_name << " \\" << std::endl;
  a_out << "         --batch                  \\" << std::endl;
  a_out << "         --using-time-statistics  \\" << std::endl;
  a_out << "         --vertex-generator-name \"source_bulk.vg\"  \\" << std::endl;
  a_out << "         --vertex-generator-seed 0 \\" << std::endl;
  a_out << "         --event-generator-name \"K40\"  \\" << std::endl;
  a_out << "         --event-generator-seed 0 \\" << std::endl;
  a_out << "         --g4-manager-seed 0      \\" << std::endl;
  a_out << "         --shpf-seed 0            \\" << std::endl;
  a_out << "         --output-prng-seeds-file mc_g4_production.seeds   \\" << std::endl;
  a_out << "         --output-prng-states-file mc_g4_production.states \\" << std::endl;
  a_out << "         --number-of-events 1     \\" << std::endl;
  a_out << "         --config ${CONFIG_DIR}/simulation/manager.conf \\" << std::endl;
  a_out << "         --output-data-file mc_g4_sample_0.xml \\" << std::endl;
  a_out << "         > g4_production.log 2>&1" << std::endl;
  a_out << std::endl;
  a_out << std::endl;

  return;
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
  return kernel_init_flags;
}
