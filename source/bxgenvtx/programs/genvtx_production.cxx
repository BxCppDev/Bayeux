// -*- mode: c++ ; -*-
// genvtx_production.cxx

// Standard libraries:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third Party
// - Boost:
#include <boost/program_options.hpp>

// Bayeux
// - datatools:
#include <datatools/datatools.h>
#include <datatools/logger.h>
#include <datatools/exception.h>

// - genvtx:
#include <genvtx/genvtx_driver.h>

void print_help(boost::program_options::options_description & opts_,
                std::ostream & out_ = std::clog);

int main(int argc_, char ** argv_)
{
  DATATOOLS_INIT_MAIN(argc_, argv_);

  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  namespace po = boost::program_options;
  boost::program_options::options_description opts("General options");
  try {
    bool run_session = true;
    genvtx::genvtx_driver_params params;

    genvtx::genvtx_driver::build_general_opts(opts, params);
    po::positional_options_description args;
    po::variables_map vm;
    po::parsed_options parsed =
      po::command_line_parser(argc_, argv_)
      .options(opts)
      .allow_unregistered()
      .run();

    // Collect all other options & args for the driver session:
    std::vector<std::string> GD_argv;
    GD_argv = po::collect_unrecognized(parsed.options,
                                       po::include_positional);
    po::store(parsed, vm);
    po::notify(vm);

    logging = datatools::logger::get_priority(params.logging_label);
    if (params.help) {
      print_help(opts, std::cout);
      run_session = false;
      std::cerr << "*** DEVEL *** Help! " << std::endl;
    }

    if (run_session) {
      // The genvtx driver :
      genvtx::genvtx_driver GD;

      // Force logging options:
      GD_argv.push_back("--logging");
      GD_argv.push_back(params.logging_label);

      // Initialize the driver:
      int code = GD.command_initialize(GD_argv);
      DT_THROW_IF(code != EXIT_SUCCESS,
                  std::logic_error,
                  "Driver initialization failed !");

      // Run a one-shot session:
      GD.run();

      // Terminate the driver:
      GD.reset();
    }

    // The end.
  }
  catch (std::exception & x) {
    DT_LOG_FATAL(logging, x.what());
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    DT_LOG_FATAL(logging, "Unexpected error !");
    error_code = EXIT_FAILURE;
  }

  DATATOOLS_FINI();
  return (error_code);
}

void print_help(boost::program_options::options_description & opts_,
                std::ostream & out)
{
  genvtx::genvtx_driver_params driver_params;
  //boost::program_options::options_description driver_general_opts;
  boost::program_options::options_description driver_init_opts;
  boost::program_options::options_description driver_action_opts;
  //genvtx::genvtx_driver::build_general_opts(driver_init_opts, driver_params);
  genvtx::genvtx_driver::build_initialization_opts(driver_init_opts, driver_params);
  genvtx::genvtx_driver::build_action_opts(driver_action_opts, driver_params);

  // boost::program_options::options_description all_opts;
  // all_opts.add(driver_init_opts).add(driver_action_opts);

  out << "Usage : genvtx_production [OPTION]...                                       \n";
  out << "                                                                            \n";
  out << "  Inspect and generate vertices from a vertex generator                     \n";
  out << "  managed by a vertex generator manager associated to a                     \n";
  out << "  geometry manager.                                                         \n";
  out << "                                                                            \n";
  out << opts_ << std::endl;
  //out << driver_general_opts << std::endl;
  out << driver_init_opts << std::endl;
  out << driver_action_opts  << std::endl;
  out << "                                                                            \n";
  out << "Example:                                                                    \n";
  out << "                                                                            \n";
  out << "  List the available vertex generators:                                     \n";
  out << "                                                                            \n";
  out << "     genvtx_production \\                                                   \n";
  out << "       --geometry-manager \"${CONFIG_DIR}/geometry/manager.conf\" \\        \n";
  out << "       --vertex-generator-manager \"${CONFIG_DIR}/vertex/manager.conf\" \\  \n";
  out << "       --list                                                               \n";
  out << "                                                                            \n";
  out << "  Generate some vertices, store then in a file and display:                 \n";
  out << "                                                                            \n";
  out << "     genvtx_production \\                                                   \n";
  out << "       --geometry-manager \"${CONFIG_DIR}/geometry/manager.conf\" \\        \n";
  out << "       --vertex-generator-manager \"${CONFIG_DIR}/vertex/manager.conf\" \\  \n";
  out << "       --shoot \\                                                           \n";
  out << "       --prng-seed 314159 \\                                                \n";
  out << "       --number-of-vertices 10000 \\                                        \n";
  out << "       --vertex-generator \"scintillator_block.vg\" \\                      \n";
  out << "       --output-file \"vertices.data\"    \\                                \n";
  out << "       --visu                                                               \n";
  out << "                                                                            \n";
  out << "                                                                            \n";

}

// end of genvtx_production.cxx
