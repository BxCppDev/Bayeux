/* dpp_processing.cxx
 * Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-07-03
 * Last modified : 2013-05-12
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * Description:
 *
 *  A generic program that performs processing of data records along
 *  a pipeline of data processing modules.
 *
 */

// Standard library
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party
// - Boost
#include <boost/program_options.hpp>

// - datatools
#include <datatools/datatools.h>
#include <datatools/kernel.h>
#include <datatools/logger.h>
#include <datatools/exception.h>

// - Boost I/O guard
#include <datatools/bio_guard.h>
#include <mygsl/bio_guard.h>

// This project
#include <dpp/dpp_driver.h>

/// Print usage
void print_usage(std::ostream & out_);

int main (int argc_, char ** argv_)
{
  DATATOOLS_INIT_MAIN(argc_, argv_);

  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_WARNING;
  const std::string APP_NAME_PREFIX = "dpp_processing: ";
  namespace po = boost::program_options;
  po::options_description opts ("Allowed options");

  try {
    // The dpp_processing data record processing program.
    dpp::dpp_driver_params DP;
    bool run = true;
    dpp::dpp_driver_params::build_opts(opts, DP);

    // Describe command line arguments :
    po::positional_options_description args;
    args.add ("input-file", -1);

    po::variables_map vm;
    po::parsed_options parsed =
      po::command_line_parser(argc_, argv_)
      .options(opts)
      .positional(args)
      .allow_unregistered()
      .run();
    /*
    std::vector<std::string> unrecognized_options
      = po::collect_unrecognized(parsed.options,
                                 po::include_positional);
    */
    po::store(parsed, vm);
    po::notify(vm);

    // Fetch general opts :
    logging = datatools::logger::get_priority(DP.logging_label);
    DT_THROW_IF(logging == datatools::logger::PRIO_UNDEFINED,
                std::logic_error,
                "Invalid logging priority '" << DP.logging_label << "' !");

    if (DP.help) {
      print_usage(std::cout);
      run = false;
    }

    // dpp driver session :
    if (run) {
      if (logging >= datatools::logger::PRIO_DEBUG) {
        DP.dump(std::cerr);
      }
      dpp::dpp_driver drv;
      drv.setup(DP);
      drv.initialize();
      drv.run();
      drv.reset();
    }

  }
  catch (std::exception & x) {
    DT_LOG_FATAL(logging, APP_NAME_PREFIX << x.what ());
    print_usage(std::cerr);
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    DT_LOG_FATAL(logging, APP_NAME_PREFIX << "Unexpected error !");
    print_usage(std::cerr);
    error_code = EXIT_FAILURE;
  }

  DATATOOLS_FINI();
  return (error_code);
}


void print_usage(std::ostream & out_)
{
  out_ << "dpp_processing -- A generic data chain processing program" << std::endl;
  out_ << std::endl;
  out_ << "Usage : " << std::endl;
  out_ << std::endl;
  out_ << "  dpp_processing [OPTIONS] [ARGUMENTS] " << std::endl;
  out_ << std::endl;
  boost::program_options::options_description opts;
  dpp::dpp_driver_params dummy;
  dpp::dpp_driver_params::build_opts(opts, dummy);
  out_ << opts << std::endl;
  out_ << std::endl;
  out_ << "Examples : " << std::endl;
  out_ << std::endl;
  out_ << "  Process the chain of 'my_moduleX' (X=1,2,3) data processing modules from the 'mydllY' libraries (Y=1,2): " << std::endl;
  out_ << std::endl;
  out_ << "  bash$ dpp_processing              \\" << std::endl;
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
  out_ << "  bash$ dpp_processing   \\" << std::endl;
  out_ << "          -c ${DPP_DATA_DIR}/testing/config/test_module_manager.conf \\" << std::endl;
  out_ << "          -i ${DPP_DATA_DIR}/testing/data/data_0.txt.gz     \\" << std::endl;
  out_ << "          -m clear         \\" << std::endl;
  out_ << "          -m chain1        \\" << std::endl;
  out_ << "          -o processed.xml   " << std::endl;
  out_ << std::endl;
  out_ << "  bash$ dpp_processing   \\" << std::endl;
  out_ << "          -c ${DPP_DATA_DIR}/testing/config/test_module_manager.conf \\" << std::endl;
  out_ << "          -M 10            \\" << std::endl;
  out_ << "          -m chain1        \\" << std::endl;
  out_ << "          -m dump_in_file    " << std::endl;
  out_ << std::endl;
  out_ << "  bash$ dpp_processing   \\" << std::endl;
  out_ << "          -c ${DPP_DATA_DIR}/testing/config/test_module_manager.conf \\" << std::endl;
  out_ << "          -M 10            \\" << std::endl;
  out_ << "          -m chain1        \\" << std::endl;
  out_ << "          -o processed.xml   " << std::endl;
  out_ << std::endl;
  return;
}

// end of dpp_processing.cxx
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
