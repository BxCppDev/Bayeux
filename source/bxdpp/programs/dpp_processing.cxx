/* dpp_processing.cxx
 * Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-07-03
 * Last modified : 2013-10-21
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
#include <datatools/kernel.h>

// - Boost I/O guard
#include <datatools/bio_guard.h>
#include <mygsl/bio_guard.h>

// This project
#include <dpp/dpp_driver.h>

/// Print usage
void print_usage(std::ostream &);

// Build options
void build_opts(boost::program_options::options_description &,
                dpp::dpp_driver_params &);

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
    build_opts(opts, DP);

    po::variables_map vm;
    po::parsed_options parsed =
      po::command_line_parser(argc_, argv_)
      .options(opts)
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
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    DT_LOG_FATAL(logging, APP_NAME_PREFIX << "Unexpected error !");
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
  boost::program_options::options_description opts("Allowed options");
  dpp::dpp_driver_params dummy;
  build_opts(opts, dummy);
  out_ << opts << std::endl;
  {
    boost::program_options::options_description kopts("datatools' kernel options");
    datatools::kernel::param_type kparams;
    datatools::kernel::build_opt_desc(kopts, kparams);
    datatools::kernel::print_opt_desc(kopts, out_);
  }
  out_ << std::endl;
  out_ << "Examples : " << std::endl;
  out_ << std::endl;
  out_ << "  Process the chain of 'my_moduleX' (X=1,2,3) data processing modules from the 'mydllY' libraries (Y=1,2): " << std::endl;
  out_ << std::endl;
  out_ << "  $ dpp_processing              \\" << std::endl;
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
  out_ << "  $ dpp_processing   \\" << std::endl;
  out_ << "          -c ${DPP_DATA_DIR}/testing/config/test_module_manager.conf \\" << std::endl;
  out_ << "          -i ${DPP_DATA_DIR}/testing/data/data_0.txt.gz     \\" << std::endl;
  out_ << "          -m clear         \\" << std::endl;
  out_ << "          -m chain1        \\" << std::endl;
  out_ << "          -o processed.xml   " << std::endl;
  out_ << std::endl;
  out_ << "  $ dpp_processing   \\" << std::endl;
  out_ << "          -c ${DPP_DATA_DIR}/testing/config/test_module_manager.conf \\" << std::endl;
  out_ << "          -M 10            \\" << std::endl;
  out_ << "          -m chain1        \\" << std::endl;
  out_ << "          -m dump_in_file    " << std::endl;
  out_ << std::endl;
  out_ << "  $ dpp_processing   \\" << std::endl;
  out_ << "          -c ${DPP_DATA_DIR}/testing/config/test_module_manager.conf \\" << std::endl;
  out_ << "          -M 10            \\" << std::endl;
  out_ << "          -m chain1        \\" << std::endl;
  out_ << "          -o processed.xml   " << std::endl;
  out_ << std::endl;
  return;
}

void build_opts(boost::program_options::options_description & opts_,
                dpp::dpp_driver_params & params_)
{
  namespace po = boost::program_options;
  opts_.add_options ()
    ("help,h", po::value<bool> (&params_.help)
     ->zero_tokens()
     ->default_value(false),
     "produce help message.")
    ("logging-priority,P",
     po::value<std::string>(&params_.logging_label)->default_value ("warning"),
     "set the logging priority.")
    ("load-dll,l",
     po::value<std::vector<std::string> > (&params_.LL_dlls),
     "set a DLL to be loaded.")
    ("dlls-config,L",
     po::value<std::string> (&params_.LL_config),
     "set the DLL loader configuration file.")
    ("modulo,%",
     po::value<int> (&params_.print_modulo)->default_value (10),
     "set the modulo print period for data record.")
    ("max-records,M",
     po::value<int> (&params_.max_records)->default_value (0),
     "set the maximum number of data records to be processed.")
    ("no-max-records,X",
     po::value<bool>(&params_.no_max_records)->zero_tokens()->default_value (false),
     "Do not limit the maximum number of data records to be processed.")
    ("module,m",
     po::value<std::vector<std::string> > (&params_.module_names),
     "add a module in the pipeline (optional).")
    ("module-manager-config,c",
     po::value<std::string> (&params_.module_manager_config_file),
     "set the module manager configuration file.")
    ("input-file,i",
     po::value<std::vector<std::string> > (&params_.input_files),
       "set an input file (optional).")
    ("output-file,o",
     po::value<std::vector<std::string> > (&params_.output_files),
     "set the output file (optional).")
    ("preserve-existing-files,x",
     po::value<bool>(&params_.preserve_existing_files)->zero_tokens()->default_value (false),
     "preserve existing files (recommended).")
    ("max-records-per-output-file,O",
     po::value<int> (&params_.max_records_per_output_file)->default_value (0),
     "set the maximum number of data records per output file.")
    // ("save-stopped-records,s",
    //  po::value<bool>(&save_stopped_data_records)->zero_tokens()->default_value (false),
    //  "Blablabla.")
    ;
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
