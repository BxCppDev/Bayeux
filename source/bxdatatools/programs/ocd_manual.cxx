// -*- mode: c++ ; -*-
// ocd_manual.cxx

// Standard Library
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

// Third Party
#include <boost/program_options.hpp>

// Datatools
#include <datatools/datatools.h>
#include <datatools/ocd_driver.h>

/****************
 * Main program *
 ****************/
int main(int argc_, char ** argv_)
{
  DATATOOLS_INIT_MAIN(argc_,argv_);
  int error_code = EXIT_SUCCESS;
  datatools::ocd_driver_params params;
  try {
    namespace po = boost::program_options;
    po::options_description opts ("Allowed options ");

    opts.add_options ()

      ("help,h", "produce help message")

      ("debug,d",
       po::value<bool>(&params.debug)
       ->zero_tokens()
       ->default_value (false),
       "produce debug logging")

      // ("interactive,I",
      //  po::value<bool>(&params.interactive)
      //  ->zero_tokens()
      //  ->default_value (false),
      //  "run in interactive mode (not implemented)")

      ("dlls-config,L",
       po::value<std::string> (&params.dll_loader_config),
       "set the DLL loader configuration file.      \n"
       "Example :                                   \n"
       " --dlls-config dlls.conf                    "
       )

      ("load-dll,l",
       po::value<std::vector<std::string> >(&params.dlls),
       "set a DLL to be loaded.                     \n"
       "Example :                                   \n"
       " --load-dll genvtx                          "
       )

      ("class-id,c",
       po::value<std::string> (&params.class_id),
       "set the ID of the class to be investigated. \n"
       "Example :                                   \n"
       " --class-id genvtx::manager                 "
       )

      ("action,a",
       po::value<std::string> (&params.action),
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
       po::value<std::string> (&params.input_path),
       "set the name of an input filename.")

      ("output-file,o",
       po::value<std::string> (&params.output_path),
       "set the name of an output filename.")

      ; // end of options' description


    // Describe command line arguments :
    po::positional_options_description args;
    args.add ("class-id", 1);

    po::variables_map vm;
    po::parsed_options parsed =
      po::command_line_parser(argc_, argv_)
      .options (opts)
      .positional (args)
      .allow_unregistered()
      .run ();
    params.action_options =
      po::collect_unrecognized(parsed.options,
                               po::include_positional);
    // po::store (po::command_line_parser (argc_, argv_)
    //            .options (opts)
    //            .positional (args)
    //            .allow_unregistered()
    //            .run (), vm);
    po::store (parsed, vm);
    po::notify (vm);

    // Fetch the opts/args :
    if (vm.count ("help")) {
      std::cout << "\nocd_manual -- Object Configuration Description Manual" << std::endl;
      std::cout << "                     (datatools OCD support)" << std::endl;
      std::cout << std::endl;
      std::cout << "Usage : " << std::endl;
      std::cout << std::endl;
      std::cout << "  ocd_manual [OPTIONS] [ARGUMENTS] " << std::endl << std::endl;
      std::cout << opts << std::endl;
      std::cout << "Examples : " << std::endl;
      std::cout << "                                                                  " << std::endl;
      std::cout << "  ocd_manual --help                                               " << std::endl;
      std::cout << "                                                                  " << std::endl;
      std::cout << "  ocd_manual --action list                                        " << std::endl;
      std::cout << "                                                                  " << std::endl;
      std::cout << "  ocd_manual --load-dll genvtx --action list                      " << std::endl;
      std::cout << "                                                                  " << std::endl;
      std::cout << "  ocd_manual --class-id datatools::service_manager    \\          " << std::endl
                << "             --action show [--no-configuration-infos] \\          " << std::endl
                << "                           [--no-title]               \\          " << std::endl
                << "             | pandoc -r rst -w plain                 \\          " << std::endl
                << "             | less                                               " << std::endl;
      std::cout << "                                                                  " << std::endl;
      std::cout << "  ocd_manual --class-id datatools::service_manager    \\          " << std::endl
                << "             --action show [--no-configuration-infos] \\          " << std::endl
                << "                           [--no-title]               \\          " << std::endl
                << "             | pandoc -T=\"datatools::service_manager\" \\        " << std::endl
                << "                      -r rst -w html                  \\          " << std::endl
                << "                      -o \"datatools__service_manager.html\"      " << std::endl;
      std::cout << "                                                                  " << std::endl;
      std::cout << "  ocd_manual --class-id datatools::service_manager \\             " << std::endl
                << "             --action skeleton                     \\             " << std::endl
                << "             [--no-additional-infos]               \\             " << std::endl
                << "             [--no-configuration-hints]            \\             " << std::endl
                << "             --output-file sm.conf                                " << std::endl;
      std::cout << "                                                                  " << std::endl;
      std::cout << "  ocd_manual --class-id datatools::service_manager \\             " << std::endl
                << "             --action validate --input-file sm.conf               " << std::endl;
      std::cout << "                                                                  " << std::endl;
      std::cout << std::endl;
      return (1);
    }

    if (params.debug) params.print(std::cerr);

    datatools::ocd_driver ocdd;
    ocdd.initialize(params);
    ocdd.run();
    ocdd.reset();

  }
  catch (std::exception & x) {
    std::cerr << "error: ocd_manual:" << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: ocd_manual: " << "unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  DATATOOLS_FINI();
  return (error_code);
}

// end of ocd_manual.cxx
