// -*- mode: c++ ; -*-
// ocd_manual.cxx

// This project
#include <datatools/datatools.h>

// Standard Library
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

// Third Party

// - Boost
#include <boost/program_options.hpp>

#if DATATOOLS_STANDALONE == 0
// - bayeux:
#include <bayeux/bayeux.h>
#endif // DATATOOLS_STANDALONE == 0

// - datatools
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/ocd_driver.h>

namespace datatools {

  // User interface for this app:
  struct ui {

    static
    void build_options(boost::program_options::options_description & opts_,
                       datatools::ocd_driver_params & params_);

    static
    void print_usage(boost::program_options::options_description & opts_,
                     std::ostream & out_);
  };

}

/****************
 * Main program *
 ****************/
int main(int argc_, char ** argv_)
{
#if DATATOOLS_STANDALONE == 1
  DATATOOLS_INIT_MAIN(argc_, argv_);
#else
  BAYEUX_INIT_MAIN(argc_, argv_);
#endif // DATATOOLS_STANDALONE == 1

  int error_code = EXIT_SUCCESS;
  datatools::ocd_driver_params params;
  try {
    namespace po = boost::program_options;
    po::options_description opts ("Allowed options ");

    datatools::ui::build_options(opts, params);

    // Describe command line arguments :
    po::positional_options_description args;
    args.add("class-id", 1);

    po::variables_map vm;
    po::parsed_options parsed =
      po::command_line_parser(argc_, argv_)
      .options(opts)
      .positional(args)
      .allow_unregistered()
      .run();
    params.action_options =
      po::collect_unrecognized(parsed.options,
                               po::include_positional);
    po::store(parsed, vm);
    po::notify(vm);

    // Fetch the opts/args :
    // Fetch the opts/args :
    if (vm.count("help")) {
      if (vm["help"].as<bool>()) {
        datatools::ui::print_usage(opts, std::cout);
        return (1);
      }
    }

    if (vm.count("logging-priority")) {
      const std::string logging_label = vm["logging-priority"].as<std::string>();
      params.logging = datatools::logger::get_priority(logging_label);
      DT_THROW_IF(params.logging == datatools::logger::PRIO_UNDEFINED,
                  std::logic_error,
                  "Invalid logging priority label '" << logging_label << "' !");
    }

    if (params.logging >= datatools::logger::PRIO_DEBUG) params.print(std::clog);

    datatools::ocd_driver ocdd;
    ocdd.initialize(params);
    ocdd.run();
    ocdd.reset();

  }
  catch (std::exception & x) {
    DT_LOG_FATAL(params.logging, x.what ());
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    DT_LOG_FATAL(params.logging, "Unexpected error !");
    error_code = EXIT_FAILURE;
  }

#if DATATOOLS_STANDALONE == 1
  DATATOOLS_FINI();
#else
  BAYEUX_FINI();
#endif // DATATOOLS_STANDALONE == 1
  return error_code;
}

namespace datatools {

  void ui::build_options(boost::program_options::options_description & opts_,
                         datatools::ocd_driver_params & params_)
  {
    namespace po = boost::program_options;
    opts_.add_options()

      ("help,h",
       po::value<bool>()
       ->zero_tokens()
       ->default_value(false),
       "produce help message")

      ("logging-priority,P",
       po::value<std::string>()->default_value("notice"),
       "set the logging priority threshold")

      ("dlls-config,L",
       po::value<std::string>(&params_.dll_loader_config),
       "set the DLL loader configuration file.      \n"
       "Example :                                   \n"
       " --dlls-config dlls.conf                    "
       )

      ("load-dll,l",
       po::value<std::vector<std::string> >(&params_.dlls),
       "set a DLL to be loaded.                     \n"
       "Example :                                   \n"
       " --load-dll \"foo\"                         \n"
       " --load-dll \"foo@/usr/local/foo/1.0/lib\"  \n"
       )

      ("class-id,c",
       po::value<std::string>(&params_.class_id),
       "set the ID of the class to be investigated. \n"
       "Example :                                   \n"
       " --class-id genvtx::manager                 "
       )

      ("action,a",
       po::value<std::string>(&params_.action),
       "define the action to be performed.                \n"
       "Actions:                                          \n"
       " list     : list the registered classes by ID     \n"
       "            May use '--load-dll DLL_NAME' option. \n"
       " show     : print some description                \n"
       "            of the configuration parameters       \n"
       "            for class CLASS_ID.                   \n"
       "            Needs '--class-id CLASS_ID' option.   \n"
       " skeleton : print a sample skeleton configuration \n"
       "            file for class CLASS_ID.              \n"
       "            Needs '--class-id CLASS_ID' option.   \n"
       "            May use '--output-file OUTPUT_FILE'   \n"
       "            option.                               \n"
       " validate : validate an input configuration       \n"
       "            file for class CLASS_ID.              \n"
       "            Needs '--class-id CLASS_ID' option.   \n"
       "            Needs '--input-file INPUT_FILE'       \n"
       "            option.                               "
       )

      ("input-file,i",
       po::value<std::string>(&params_.input_path),
       "set the name of an input filename.")

      ("output-file,o",
       po::value<std::string>(&params_.output_path),
       "set the name of an output filename.")

      ; // end of options' description
    return;
  }

  void ui::print_usage(boost::program_options::options_description & opts_,
                       std::ostream & out_)
  {
#if DATATOOLS_STANDALONE == 1
    const std::string APP_NAME = "ocd_manual";
#else
    const std::string APP_NAME = "bxocd_manual";
#endif // DATATOOLS_STANDALONE == 1
    out_ << "\n" << APP_NAME << " -- Object Configuration Description Manual" << std::endl;
    out_ << "                     (datatools OCD support)" << std::endl;
    out_ << std::endl;
    out_ << "Usage : " << std::endl;
    out_ << std::endl;
    out_ << "  " << APP_NAME << " [OPTIONS] [ARGUMENTS] " << std::endl << std::endl;
    out_ << opts_ << std::endl;
    out_ << "Examples : " << std::endl;
    out_ << "                                                        " << std::endl;
    out_ << "  " << APP_NAME << " --help                             " << std::endl;
    out_ << "                                                        " << std::endl;
    out_ << "  " << APP_NAME << " --action list                      " << std::endl;
    out_ << "                                                        " << std::endl;
    out_ << "  " << APP_NAME << " --action list | grep ^datatools::  " << std::endl;
    out_ << "                                                        " << std::endl;
#if DATATOOLS_STANDALONE == 1
    out_ << "  " << APP_NAME << " --load-dll genvtx --action list    " << std::endl;
#else
    out_ << "  " << APP_NAME << " --load-dll foo --action list       " << std::endl;
#endif // DATATOOLS_STANDALONE == 1
    out_ << "                                                        " << std::endl;
    out_ << "  " << APP_NAME << " \\" << std::endl
         << "             --class-id datatools::service_manager    \\" << std::endl
         << "             --action show [--no-configuration-infos] \\" << std::endl
         << "                           [--no-title]               \\" << std::endl
         << "             | pandoc -r rst -w plain                 \\" << std::endl
         << "             | less                                     " << std::endl;
    out_ << "                                                        " << std::endl;
    out_ << "  " << APP_NAME << " \\" << std::endl
         << "             --class-id datatools::service_manager    \\" << std::endl
         << "             --action show [--no-configuration-infos] \\" << std::endl
         << "                           [--no-title]               \\" << std::endl
         << "             | pandoc -T=\"datatools::service_manager\" \\" << std::endl
         << "                      -r rst -w html                    \\" << std::endl
         << "                      -o \"datatools__service_manager.html\" " << std::endl;
    out_ << "                                                             " << std::endl;
    out_ << "  " << APP_NAME << " \\" << std::endl
         << "             --class-id datatools::service_manager \\" << std::endl
         << "             --action skeleton                     \\" << std::endl
         << "             [--no-additional-infos]               \\" << std::endl
         << "             [--no-configuration-hints]            \\" << std::endl
         << "             --output-file sm.conf                   " << std::endl;
    out_ << "                                                     " << std::endl;
    out_ << "  " << APP_NAME << " \\" << std::endl
         << "             --class-id datatools::service_manager \\" << std::endl
         << "             --action validate --input-file sm.conf          " << std::endl;
    out_ << "                                                             " << std::endl;
    out_ << std::endl;
    return;
  }

}

// end of ocd_manual.cxx
