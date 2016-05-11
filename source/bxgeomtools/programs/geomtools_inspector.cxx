// geomtools_inspector.cxx

// Ourselves:
#include <geomtools/geomtools.h>

// Standard libraries:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <vector>

// Third Party:
// - Boost:
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#if GEOMTOOLS_STANDALONE == 0
// - Bayeux:
#include <bayeux/bayeux.h>
#endif
#if GEOMTOOLS_WITH_READLINE == 1
#include <readline/readline.h>
#include <readline/history.h>
// #include <curses.h>
// #include <term.h>
#endif // GEOMTOOLS_WITH_READLINE
// - Bayeux/datatools:
#include <datatools/datatools.h>
#include <datatools/exception.h>
#include <datatools/logger.h>
#include <datatools/kernel.h>
#include <datatools/configuration/io.h>
// - Bayeux/materials:
#include <materials/materials.h>

// This project:
#include <geomtools/version.h>
#include <geomtools/geomtools_driver.h>

void print_splash(std::ostream & out_ = std::clog);

void print_help(const boost::program_options::options_description & opts_,
                std::ostream & out_ = std::clog);

void print_shell_help(geomtools::geomtools_driver & gd_,
                      std::ostream & out_ = std::clog);

void build_argv(std::istringstream & command_in_, std::vector<std::string> & argv_);

int main(int argc_, char ** argv_)
{
#if MATERIALS_STANDALONE == 1
  MATERIALS_INIT_MAIN(argc_, argv_);
#else
  BAYEUX_INIT_MAIN(argc_, argv_);
#endif
#if GEOMTOOLS_STANDALONE == 1
  const std::string APP_NAME = "geomtools_inspector";
#else
  const std::string APP_NAME = "bxgeomtools_inspector";
#endif // GEOMTOOLS_STANDALONE == 1

  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_WARNING;
  namespace po = boost::program_options;
  boost::program_options::options_description opts("geomtools inspector configuration parameters");
#if GEOMTOOLS_WITH_READLINE == 1
  bool without_readline = false;
  std::string history_filename = "~/.bxgeomtools_inspector.history";
  datatools::fetch_path_with_env(history_filename);
#endif // GEOMTOOLS_WITH_READLINE
  try {
    std::string prompt = "geomtools> ";
    std::string prompt_continue = "> ";
    bool run = true;
    bool help = false;
    bool no_splash = false;
    std::string logging_label = "warning";
    std::vector<std::string> GDP_argv;
    //std::string manager_config;

    opts.add_options()
      ("help,h",
       po::value<bool>(&help)
       ->zero_tokens()
       ->default_value(false),
       "Print help then exit.  \n"
       )
#if GEOMTOOLS_WITH_READLINE == 1
      ("without-readline",
       po::value<bool>(&without_readline)
       ->zero_tokens()
       ->default_value(false),
       "Deactivate readline.\n"
       )
#endif // GEOMTOOLS_WITH_READLINE
      ("logging,G",
       po::value<std::string>(&logging_label)
       ->default_value("warning"),
       "Set the logging priority threshold.\n"
       "Example :                          \n"
       "  --logging \"warning\"           "
       )
      ("prompt,P",
       po::value<std::string>(&prompt)
       ->default_value("geomtools> "),
       "Set the prompt.          \n"
       "Example :                \n"
       "  --prompt \"Geo$ \"  "
       )
      ("no-splash,S",
       po::value<bool>(&no_splash)
       ->zero_tokens()
       ->default_value(false),
       "Do not print the geomtools splash screen.\n"
       "Example :                                \n"
       "  --no-splash                          "
       )
      ;

    // Preprocessor for command line arguments:
    unsigned int vpp_flags = 0;
    // vpp_flags |= datatools::configuration::variant_preprocessor::FLAG_TRACE;
    vpp_flags |= datatools::configuration::variant_preprocessor::FLAG_REMOVE_QUOTES;
    datatools::configuration::variant_preprocessor vpp(vpp_flags);
    std::vector<std::string> preprocessed_arguments;
    vpp.preprocess_args_options(argc_, argv_, preprocessed_arguments);

    po::positional_options_description args;

    po::variables_map vm;
    po::parsed_options parsed =
      po::command_line_parser(preprocessed_arguments)
      .options(opts)
      .allow_unregistered()
      .run();
    GDP_argv = po::collect_unrecognized(parsed.options,
                                        po::include_positional);
    po::store(parsed, vm);
    po::notify(vm);

    GDP_argv.push_back("--logging");
    GDP_argv.push_back(logging_label);

#if GEOMTOOLS_WITH_READLINE == 1
    //if (! without_readline) {
      using_history(); // use readline library
      if (boost::filesystem::exists(history_filename)) {
        int error = read_history(history_filename.c_str());
        if (error) {
          DT_LOG_ERROR(datatools::logger::PRIO_ERROR, APP_NAME << ": "
                       << "Cannot read history file '" << history_filename << "'!");
        }
      }
      //}
#endif // GEOMTOOLS_WITH_READLINE

    if (! no_splash) {
      print_splash(std::cerr);
    }

    if (help) {
      print_help(opts, std::cout);
      run = false;
    }

    // The geomtools driver :
    geomtools::geomtools_driver GD;
    {
      GDP_argv.push_back("--mute");
      int error = GD.command_initialize(GDP_argv, std::clog);
      if (error > 0) {
        DT_LOG_DEBUG(logging, "Geometry driver has not been initialized ! ");
        if (error == 1) {
          DT_LOG_ERROR(logging, "Geometry driver could not be initialized ! ");
        }
      } else {
        DT_LOG_DEBUG(logging, "Geometry driver has been initialized ! ");
      }
    }

    // Run the geomtools inspector shell:
    bool go_on = run;
    /// Browser main loop :
    while (go_on) {
#if GEOMTOOLS_WITH_READLINE == 0
      if (!std::cin || std::cin.eof()) {
        std::cerr << std::flush;
        std::cout << std::endl;
        break;
      }
#endif
      std::string line;
#if GEOMTOOLS_WITH_READLINE == 1
      char * readline_line = 0;
      go_on = false;
      readline_line = readline (prompt.c_str ()); // use readline library
      if (readline_line != 0) {
        go_on = true;
        line = readline_line; // use readline library
        if (! line.empty()) {
          add_history(readline_line); // use readline/history library
        }
        free(readline_line);
        readline_line = 0;
      }
#else // GEOMTOOLS_WITH_READLINE
      // Prompt:
      std::cerr << prompt << std::flush;
      std::getline (std::cin, line);
      if (! std::cin || std::cin.eof()) {
        go_on = false;
      }
#endif // GEOMTOOLS_WITH_READLINE
      {
        // Skip blank line and lines starting with '#' :
        std::istringstream dummy(line);
        std::string word;
        dummy >> word;
        if (word.empty()) continue;
        if (word[0] == '#') continue;
      }
      // Manage continuation marks :
      if (go_on) {
        while (line[line.length()-1] == '\\') {
          line = line.substr(0, line.length()-1);
          std::string more;
#if GEOMTOOLS_WITH_READLINE == 1
          {
            char * local_readline_line = 0;
            go_on = false;
            local_readline_line = readline (prompt_continue.c_str ()); // use readline library
            if (local_readline_line != 0) {
              go_on = true;
              more = local_readline_line; // use readline library
              if (! more.empty()) {
                add_history(local_readline_line); // use readline/history library
              }
              free(local_readline_line);
              local_readline_line = 0;
            }
          }
#else // GEOMTOOLS_WITH_READLINE
          std::cerr << prompt_continue << std::flush;
          std::getline (std::cin, more);
          if (! std::cin || std::cin.eof()) {
            go_on = false;
          }
#endif // GEOMTOOLS_WITH_READLINE
          line += more;
        }
      }

      // Interpreter block:
      {
        // Parse the line :
        std::istringstream command_iss (line);
        std::string command;
        command_iss >> command >> std::ws;
        if (command == "h" || command == "help") {
          print_shell_help(GD, std::cerr);
        } else if (command == "q" || command == "quit") {
          go_on = false;
        } else if (command == "i" || command == "initialize") {
          std::vector<std::string> argv;
          build_argv(command_iss, argv);
          int error = GD.command_initialize(argv, std::clog);
          if (error > 0) {
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Cannot initialize the geometry driver ! ");
          }
        } else if (command == "r" || command == "reset") {
          int error = GD.command_reset();
          if (error > 0) {
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Cannot terminate the geometry driver !");
          }
        } else if (command == "s" || command == "status") {
          GD.command_status(std::cerr);
        } else if (command == "P" || command == "print_manager") {
          GD.command_print_manager(std::cout);
        } else if (command == "F" || command == "print_factory") {
          GD.command_print_factory(std::cout);
        } else if (command == "A" || command == "print_mapping") {
          std::string mapping_name;
          command_iss >> mapping_name >> std::ws;
          GD.command_print_mapping(std::cout, mapping_name);
          mapping_name.clear();
        } else if (command == "m" || command == "model") {
          std::string model_name;
          command_iss >> model_name >> std::ws;
          GD.command_print_model(model_name, std::cout);
          model_name.clear();
        } else if (command == "l" || command == "logical") {
          std::string logical_name;
          command_iss >> logical_name >> std::ws;
          GD.command_print_logical(logical_name, std::cout);
          logical_name.clear();
        // } else if (command == "R" || command == "rendering_options") {
        //   std::vector<std::string> argv;
        //   while (command_iss) {
        //     std::string token;
        //     command_iss >> token >> std::ws;
        //     if (token.empty()) break;
        //     argv.push_back(token);
        //     if (command_iss.eof()) break;
        //   } // end of parsing
        //   int error = GD.command_set_rendering_options(argv);
        //   if (error > 0) {
        //     DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Cannot set rendering options !");
        //   }
        } else if (command == "M" || command == "list_of_models") {
          std::string print_models_options;
          std::getline(command_iss, print_models_options);
          int error = GD.command_print_list_of_models(std::cout, print_models_options);
          if (error > 0) {
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                         "Invalid options '" << print_models_options << "' !");
          }
        } else if (command == "L" || command == "list_of_logicals") {
          std::string print_logical_options;
          std::getline(command_iss, print_logical_options);
          int error = GD.command_print_list_of_logicals(std::cout, print_logical_options);
          if (error > 0) {
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                         "Invalid options '" << print_logical_options << "' !");
          }
        } else if (command == "G" || command == "list_of_gids") {
          std::string print_gids_options;
          std::getline(command_iss, print_gids_options);
          if (GD.has_manager()) {
            int error = GD.command_print_list_of_gids(std::cout, print_gids_options);
            if (error > 0) {
              DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Invalid options '" << print_gids_options << "' !");
            }
          } else {
            DT_LOG_WARNING(logging, "Sorry ! Mapping is only supported by a geometry manager !");
          }
        } else if (command == "x" || command == "export_gdml") {
          if (GD.has_manager()) {
            std::vector<std::string> argv;
            build_argv(command_iss, argv);
            int error = GD.command_export_gdml(argv);
            if (error > 0) {
              DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Cannot export GDML file !");
            }
          } else {
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "A plain geometry model factory cannot export GDML file !");
          }
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
        } else if (command == "d" || command == "display") {
          std::vector<std::string> argv;
          build_argv(command_iss, argv);
          int error = GD.command_gnuplot_display(argv);
          if (error > 0) {
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Cannot display the geometry setup !");
          }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
        } else if (command == "C" || command == "list_of_categories") {
          std::vector<std::string> argv;
          build_argv(command_iss, argv);
          int error = GD.command_print_list_of_categories(std::cout, argv);
          if (error > 0) {
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Cannot print the list of geometry categories !");
          }
        } else if (command == "lsdd" || command == "list_of_display_data") {
          std::vector<std::string> argv;
          build_argv(command_iss, argv);
          int error = GD.command_print_list_of_display_data(std::cout, argv);
          if (error > 0) {
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Cannot print the list of display data !");
          }
        } else if (command == "ldd" || command == "load_display_data") {
          std::vector<std::string> argv;
          build_argv(command_iss, argv);
          int error = GD.command_load_display_data(argv);
          if (error > 0) {
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Cannot load display data !");
          }
        } else if (command == "udd" || command == "unload_display_data") {
          std::vector<std::string> argv;
          build_argv(command_iss, argv);
          int error = GD.command_unload_display_data(argv);
          if (error > 0) {
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Cannot unload display data !");
          }
        } else if (command == "cdd" || command == "clear_display_data") {
          std::vector<std::string> argv;
          build_argv(command_iss, argv);
          int error = GD.command_clear_display_data(argv);
          if (error > 0) {
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Cannot clear display data !");
          }
        } else {
          // Invalid command:
          DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Invalid command '" << command << "' !");
        }
      } // End of interpreter block.
      if (std::cin.eof()) {
        go_on = false;
        break;
      }
      if (! go_on) {
        break;
      }
    } // End of browser main loop.

#if GEOMTOOLS_WITH_READLINE == 1
    DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, APP_NAME << ": "
                  << "Saving history file '" << history_filename << "'...");
    int error = write_history(history_filename.c_str());
    if (error) {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                   APP_NAME << ": " << "Cannot write history file '" << history_filename << "' !");
    }
    history_truncate_file(history_filename.c_str(), 200);
    clear_history();
#endif // GEOMTOOLS_WITH_READLINE

  }
  catch (std::exception & x) {
    print_help(opts);
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL, APP_NAME << ": " << x.what());
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL, APP_NAME << ": " << "Unexpected error !");
    error_code = EXIT_FAILURE;
  }

#if MATERIALS_STANDALONE == 1
  MATERIALS_FINI();
#else
  BAYEUX_FINI();
#endif
  return (error_code);
}

void print_splash(std::ostream & out_)
{
  out_ << "                                                 \n"
       << "\tG E O M T O O L S    I N S P E C T O R         \n"
       << "\tVersion " << GEOMTOOLS_LIB_VERSION << "        \n"
       << "                                                 \n"
       << "\tCopyright (C) 2009-2015                        \n"
       << "\tFrancois Mauger, Xavier Garrido, Benoit Guillon, \n"
       << "\tBen Morgan and Arnaud Chapon                   \n"
       << "                                                 \n"
       << "\timmediate help: type \"help\"                  \n"
       << "\tquit:           type \"quit\"                  \n";
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
  out_ << "\tsupport:        Gnuplot display                \n";
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
#if GEOMTOOLS_WITH_ROOT_DISPLAY == 1
  out_ << "\tsupport:        Root display from GDML         \n";
#endif // GEOMTOOLS_WITH_ROOT_DISPLAY
  out_ << "                                                 \n";
  return;
}

void print_help (const boost::program_options::options_description & opts_,
                 std::ostream & out_)
{
#if GEOMTOOLS_STANDALONE == 1
  const std::string APP_NAME = "geomtools_inspector";
#else
  const std::string APP_NAME = "bxgeomtools_inspector";
#endif // GEOMTOOLS_STANDALONE == 1
  out_ << APP_NAME << " -- Inspect and display a virtual geometry" << std::endl;
  out_ << "\n";
  out_ << "Usage: \n\n";

  out_ << "  " << APP_NAME << " [OPTIONS...] [MGRCFG_FILE]\n\n";
  out_ << "Options: \n\n";
  out_ << opts_;
  {
    out_ << "\nDatatools kernel options: \n";
    boost::program_options::options_description kopts;
    datatools::kernel::param_type kparams;
    datatools::kernel::build_opt_desc(kopts, kparams);
    datatools::kernel::print_opt_desc(kopts, out_);
  }
  {
    out_ << "\nDefault inspector options: \n";
    geomtools::geomtools_driver_params GDP;
    std::vector<std::string> argv;
    argv.push_back("--help-short");
    GDP.parse(argv, out_);
  }
  out_ << "\n";
  out_ << "Examples:\n\n";
  out_ << "  " << APP_NAME << " --manager-config config/geometry/setup-1.0/manager.conf \n\n";
  return;
}

void print_shell_help (geomtools::geomtools_driver & /*gd_*/, std::ostream & out_)
{
  out_ <<  "  h | help                       : Print this help                                  \n";
  out_ <<  "  s | status                     : Print the status of the geometry driver          \n";
  out_ <<  "  i | initialize [OPTIONS] FILE  : Initialize the geometry driver                   \n";
  out_ <<  "                                   Use 'initialize --help' for the online help.     \n";
  out_ <<  "  r | reset                      : Terminate the geometry driver                    \n";
  out_ <<  "  P | print_manager              : Print the embedded geometry manager              \n";
  out_ <<  "  F | print_factory              : Print the embedded geometry model factory        \n";
  out_ <<  "  C | list_of_categories [OPTIONS] : Print the list of geometry categories (GID).          \n";
  out_ <<  "                                   Use 'list_of_categories --help' for the list of options.\n";
  out_ <<  "  M | list_of_models [OPTIONS]   : Print the list of geometry models.                      \n";
  out_ <<  "                                   Use 'list_of_models --help' for the list of options.    \n";
  out_ <<  "  L | list_of_logicals [OPTIONS] : Print the list of logical volumes.                      \n";
  out_ <<  "                                   Use 'list_of_logicals --help' for the list of options.  \n";
  out_ <<  "  A | print_mapping [NAME]       : Print the embedded mapping object                       \n";
  out_ <<  "  G | list_of_gids  [OPTIONS]    : Print the list of geometry identifiers (GID).           \n";
  out_ <<  "                                   Use 'list_of_gids --help' for the list of options.      \n";
  out_ <<  "  m | model MODEL_NAME           : Print the geometry model named 'MODEL_NAME'             \n";
  out_ <<  "  l | logical LOGICAL_NAME       : Print the logical volume named 'LOGICAL_NAME'           \n";
  out_ <<  "  ldd | load_display_data [OPTIONS] [NAME] [INFILE] : \n";
  out_ << "                                    Load a display data object  \n";
  out_ <<  "  udd | unload_display_data [OPTIONS] [NAME] : \n";
  out_ << "                                    Unload a display data object \n";
  out_ <<  "  cdd | clear_display_data :       Clear display data objects \n";
  out_ <<  "  pdd | print_display_data :       Print the list of display data objects \n";
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
  out_ <<  "  d | display [OPTIONS] [NAME]   : Gnuplot display of the geometry setup         \n";
  out_ <<  "                                   Use 'display --help' for the online help.     \n";
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
  out_ <<  "  x | export_gdml  [OPTIONS] [GDML_FILENAME] :                                   \n"
       << "                                    Export the geometry setup to a GDML file      \n";
  out_ <<  "                                   Use 'export_gdml --help' for the online help. \n";
  out_ <<  "  q | quit                       : Quit                                          \n";
  return;
}

void build_argv(std::istringstream & command_in_, std::vector<std::string> & argv_)
{
  while (command_in_) {
    std::string token;
    command_in_ >> token >> std::ws;
    if (token.empty()) break;
    argv_.push_back(token);
    if (command_in_.eof()) break;
  } // end of parsing
  return;
}
