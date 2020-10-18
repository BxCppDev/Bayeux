// geomtools_inspector.cxx

// Ourselves:
#include <geomtools/geomtools.h>

// Standard libraries:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <vector>
#include <memory>

// Third Party:
// - Boost:
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

// - Bayeux:
#include <bayeux/bayeux.h>
#include <bayeux/version.h>
#if GEOMTOOLS_WITH_READLINE == 1
#include <readline/readline.h>
#include <readline/history.h>
// #include <curses.h>
// #include <term.h>
#endif // GEOMTOOLS_WITH_READLINE
// - Bayeux/datatools:
#include <datatools/exception.h>
#include <datatools/logger.h>
#include <datatools/kernel.h>
#include <datatools/configuration/io.h>
#include <datatools/configuration/variant_service.h>

// This project:
#include <geomtools/geomtools_driver.h>

namespace dtc = datatools::configuration;

//! Return the application name
std::string app_name();

//! Print application splash screen
void app_print_splash(std::ostream & out_ = std::clog);

//! Print application usage (supported options and arguments)
void app_print_help(const boost::program_options::options_description & opts_,
                    std::ostream & out_ = std::clog);

//! Print application shell usage (supported commands)
void app_print_shell_help(geomtools::geomtools_driver & gd_,
                          std::ostream & out_ = std::clog);

//! Build a list of args/opts from an input string stream
void app_build_argv(std::istringstream & command_in_, std::vector<std::string> & argv_);

// Return kernel initialization flags for this application:
uint32_t app_kernel_init_flags();

//! \brief Application configuration parameters
struct app_config_params {
  //! Default constructor
  app_config_params();
  std::string logging_label = "fatal";
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL; //!< Logging label
#if GEOMTOOLS_WITH_READLINE == 1
  bool without_readline = false;
  std::string history_filename = "~/.bxgeomtools_inspector.history";
#endif // GEOMTOOLS_WITH_READLINE
  bool no_splash = false;
  std::string prompt = "geomtools> ";
  std::string prompt_continue = "> ";
  std::vector<std::string> GDP_argv;
  dtc::variant_service::config variants; //!< Variant support
};

int main(int argc_, char ** argv_)
{
  bayeux::initialize(argc_, argv_, app_kernel_init_flags());

  int error_code = EXIT_SUCCESS;
  namespace dtc = datatools::configuration;
  app_config_params params;

  namespace po  = boost::program_options;
  po::options_description optPublic;
  po::options_description optDesc("General configuration parameters");
  try {
    bool help = false;
    bool run = true;

    optDesc.add_options()

      ("help,h",
       po::value<bool>(&help)
       ->zero_tokens()
       ->default_value(false),
       "Print help then exit."
       )

#if GEOMTOOLS_WITH_READLINE == 1
      ("without-readline",
       po::value<bool>(&params.without_readline)
       ->zero_tokens()
       ->default_value(false),
       "Deactivate readline."
       )
#endif // GEOMTOOLS_WITH_READLINE

      ("logging,G",
       po::value<std::string>(&params.logging_label)
       ->default_value("fatal"),
       "Set the logging priority threshold.\n"
       "Example : \n"
       "  --logging \"debug\" "
       )

      ("prompt,P",
       po::value<std::string>(&params.prompt)
       ->default_value("geomtools> "),
       "Set the prompt. \n"
       "Example : \n"
       "  --prompt \"Geo$ \"  "
       )

      ("no-splash,S",
       po::value<bool>(&params.no_splash)
       ->zero_tokens()
       ->default_value(false),
       "Do not print the geomtools splash screen.\n"
       "Example : \n"
       "  --no-splash "
       )
      ;

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

    po::options_description optKernel("Kernel options");
    datatools::kernel::param_type paramsKernel;
    datatools::kernel::build_opt_desc(optKernel, paramsKernel, app_kernel_init_flags());

    // Aggregate options:
    optPublic.add(optDesc).add(optVariant).add(optKernel);

    // Preprocessor for command line arguments:
    unsigned int vpp_flags = 0;
    // vpp_flags |= datatools::configuration::variant_preprocessor::FLAG_TRACE;
    vpp_flags |= datatools::configuration::variant_preprocessor::FLAG_REMOVE_QUOTES;
    datatools::configuration::variant_preprocessor vpp(vpp_flags);
    std::vector<std::string> preprocessed_arguments;
    vpp.preprocess_args_options(argc_, argv_, preprocessed_arguments);

    po::positional_options_description args;

    // Parse options:
    po::variables_map vm;
    po::parsed_options parsed =
      po::command_line_parser(preprocessed_arguments)
      .options(optPublic)
      .allow_unregistered()
      .run();
    params.GDP_argv = po::collect_unrecognized(parsed.options,
                                               po::include_positional);
    po::store(parsed, vm);
    po::notify(vm);

    if (help) {
      app_print_help(optPublic, std::cout);
      run = false;
      return (1);
    }

    params.GDP_argv.push_back("--logging");
    params.GDP_argv.push_back(params.logging_label);

#if GEOMTOOLS_WITH_READLINE == 1
    //if (! params.without_readline) {
    using_history(); // use readline library
    datatools::fetch_path_with_env(params.history_filename);
    if (boost::filesystem::exists(params.history_filename)) {
      int error = read_history(params.history_filename.c_str());
      if (error) {
        DT_LOG_ERROR(datatools::logger::PRIO_ERROR, app_name() << ": "
                     << "Cannot read history file '" << params.history_filename << "'!");
      }
    }
    //}
#endif // GEOMTOOLS_WITH_READLINE

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
    }

    // The geomtools driver :
    geomtools::geomtools_driver GD;
    {
      params.GDP_argv.push_back("--mute");
      for (const auto & argopt : params.GDP_argv) {
        DT_LOG_DEBUG(params.logging, "GDP arg/opt = '" << argopt << "'");
      }
      int error = GD.command_initialize(params.GDP_argv, std::clog);
      if (error > 0) {
        DT_LOG_DEBUG(params.logging, "Geometry driver has not been initialized ! ");
        if (error == 1) {
          DT_LOG_ERROR(params.logging, "Geometry driver could not be initialized ! ");
        }
      } else {
        DT_LOG_DEBUG(params.logging, "Geometry driver has been initialized ! ");
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
      readline_line = readline(params.prompt.c_str()); // use readline library
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
      std::cerr << params.prompt << std::flush;
      std::getline(std::cin, line);
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
            local_readline_line = readline(params.prompt_continue.c_str()); // use readline library
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
          std::cerr << params.prompt_continue << std::flush;
          std::getline(std::cin, more);
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
        std::istringstream command_iss(line);
        std::string command;
        command_iss >> command >> std::ws;
        if (command == "h" || command == "help") {
          app_print_shell_help(GD, std::cerr);
        } else if (command == "q" || command == "quit") {
          go_on = false;
        } else if (command == "i" || command == "initialize") {
          std::vector<std::string> argv;
          app_build_argv(command_iss, argv);
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
            DT_LOG_WARNING(params.logging, "Sorry ! Mapping is only supported by a geometry manager !");
          }
        } else if (command == "x" || command == "export_gdml") {
          if (GD.has_manager()) {
            std::vector<std::string> argv;
            app_build_argv(command_iss, argv);
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
          app_build_argv(command_iss, argv);
          int error = GD.command_gnuplot_display(argv);
          if (error > 0) {
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Cannot display the geometry setup !");
          }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
        } else if (command == "C" || command == "list_of_categories") {
          std::vector<std::string> argv;
          app_build_argv(command_iss, argv);
          int error = GD.command_print_list_of_categories(std::cout, argv);
          if (error > 0) {
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Cannot print the list of geometry categories !");
          }
        } else if (command == "lsdd" || command == "list_of_display_data") {
          std::vector<std::string> argv;
          app_build_argv(command_iss, argv);
          int error = GD.command_print_list_of_display_data(std::cout, argv);
          if (error > 0) {
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Cannot print the list of display data !");
          }
        } else if (command == "ldd" || command == "load_display_data") {
          std::vector<std::string> argv;
          app_build_argv(command_iss, argv);
          int error = GD.command_load_display_data(argv);
          if (error > 0) {
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Cannot load display data !");
          }
        } else if (command == "udd" || command == "unload_display_data") {
          std::vector<std::string> argv;
          app_build_argv(command_iss, argv);
          int error = GD.command_unload_display_data(argv);
          if (error > 0) {
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Cannot unload display data !");
          }
        } else if (command == "cdd" || command == "clear_display_data") {
          std::vector<std::string> argv;
          app_build_argv(command_iss, argv);
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
    DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, app_name() << ": "
                  << "Saving history file '" << params.history_filename << "'...");
    int error = write_history(params.history_filename.c_str());
    if (error) {
      DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                   app_name() << ": " << "Cannot write history file '" << params.history_filename << "' !");
    }
    history_truncate_file(params.history_filename.c_str(), 200);
    clear_history();
#endif // GEOMTOOLS_WITH_READLINE

    if (vserv) {
      // Stop the variant service:
      vserv->stop();
      vserv.reset();
    }

  } catch (std::exception & x) {
    app_print_help(optPublic);
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL, app_name() << ": " << x.what());
    error_code = EXIT_FAILURE;
  } catch (...) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL, app_name() << ": " << "Unexpected error !");
    error_code = EXIT_FAILURE;
  }

  bayeux::terminate();
  return (error_code);
}

//! Return the application name
std::string app_name()
{
  return "bxgeomtools_inspector";
}

void app_print_splash(std::ostream & out_)
{
  out_ << "                                                   \n"
       << "\tB A Y E U X  -  G E O M T O O L S    I N S P E C T O R \n"
       << "\tVersion " << bayeux::version::get_version() << "\n"
       << "                                                   \n"
       << "\tCopyright (C) 2009-2017                          \n"
       << "\tFrancois Mauger, Xavier Garrido, Benoit Guillon, \n"
       << "\tBen Morgan and Arnaud Chapon                     \n"
       << "                                                   \n"
       << "\timmediate help: type \"help\"                    \n"
       << "\tquit:           type \"quit\"                    \n";
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
  out_ << "\tsupport:        Gnuplot display                  \n";
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
#if GEOMTOOLS_WITH_ROOT_DISPLAY == 1
  out_ << "\tsupport:        Root display from GDML           \n";
#endif // GEOMTOOLS_WITH_ROOT_DISPLAY
  out_ << "                                                   \n";
  return;
}

void app_print_help(const boost::program_options::options_description & opts_,
                std::ostream & out_)
{
  out_ << app_name() << " -- Inspect and display a virtual geometry" << std::endl;
  out_ << "\n";
  out_ << "Usage: \n\n";
  out_ << "  " << app_name() << " [OPTIONS...] [FILE]\n";
  out_ << opts_;
  {
    out_ << "\nGeometry inspector options: \n";
    geomtools::geomtools_driver_params GDP;
    std::vector<std::string> options;
    options.push_back("--help-short");
    GDP.parse(options, out_);
  }
  out_ << "\n";
  out_ << "Example:\n\n";
  out_ << "  " << app_name() << " --manager-config=\"config/geometry/setup-1.0/manager.conf\" \n\n";
  return;
}

void app_print_shell_help(geomtools::geomtools_driver & gd_, std::ostream & out_)
{
  out_ <<  "  h [ help ]                         : Print this help                                  \n";
  out_ <<  "  s [ status ]                       : Print the status of the geometry driver          \n";
  if (!gd_.is_initialized()) {
    out_ <<  "  i [ initialize ] [OPTIONS] FILE  : Initialize the geometry driver                   \n";
    out_ <<  "                                     Use 'initialize --help' for the online help.     \n";
  } else {
    out_ <<  "  r [ reset ]                      : Reset the geometry driver                        \n";
  }
  out_ <<  "  P [ print_manager ]                : Print the embedded geometry manager              \n";
  out_ <<  "  F [ print_factory ]                : Print the embedded geometry model factory        \n";
  out_ <<  "  C [ list_of_categories ] [OPTIONS] : Print the list of geometry categories.           \n";
  out_ <<  "                                       Use 'list_of_categories --help' for the list of options.\n";
  out_ <<  "  M [ list_of_models ] [OPTIONS]     : Print the list of geometry models.                      \n";
  out_ <<  "                                       Use 'list_of_models --help' for the list of options.    \n";
  out_ <<  "  L [ list_of_logicals ] [OPTIONS]   : Print the list of logical volumes.                      \n";
  out_ <<  "                                       Use 'list_of_logicals --help' for the list of options.  \n";
  out_ <<  "  A [ print_mapping ] [NAME]         : Print the embedded mapping object                       \n";
  out_ <<  "  G [ list_of_gids ] [OPTIONS]       : Print the list of geometry identifiers (GID).           \n";
  out_ <<  "                                       Use 'list_of_gids --help' for the list of options.      \n";
  out_ <<  "  m [ model ] MODEL_NAME             : Print the geometry model named 'MODEL_NAME'             \n";
  out_ <<  "  l [ logical ] LOGICAL_NAME         : Print the logical volume named 'LOGICAL_NAME'           \n";
  out_ <<  "  ldd [ load_display_data ] [OPTIONS] [NAME] [FILE] :                             \n";
  out_ <<  "                                       Load a display data object                 \n";
  out_ <<  "  udd [ unload_display_data ] [OPTIONS] [NAME] :                                  \n";
  out_ <<  "                                       Unload a display data object               \n";
  out_ <<  "  cdd [ clear_display_data ]         : Clear display data objects                 \n";
  out_ <<  "  pdd [ print_display_data ]         : Print the list of display data objects     \n";
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
  out_ <<  "  d [ display ] [OPTIONS] [NAME]     : Gnuplot display of the geometry setup      \n";
  out_ <<  "                                       Use 'display --help' for the online help.  \n";
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
  out_ <<  "  x [ export_gdml ] [OPTIONS] [FILE] :                                            \n"
       <<  "                                     Export the geometry setup to a GDML file     \n";
  out_ <<  "                                     Use 'export_gdml --help' for the online help.\n";
  out_ <<  "  q [ quit ]                         : Quit                                       \n";
  return;
}

void app_build_argv(std::istringstream & command_in_, std::vector<std::string> & argv_)
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

app_config_params::app_config_params()
{
  logging_label = "fatal";
  logging = datatools::logger::PRIO_FATAL;
#if GEOMTOOLS_WITH_READLINE == 1
  without_readline = false;
  history_filename = "~/.bxgeomtools_inspector.history";
#endif // GEOMTOOLS_WITH_READLINE
  no_splash = false;
  prompt = "geomtools> ";
  prompt_continue = "> ";
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
