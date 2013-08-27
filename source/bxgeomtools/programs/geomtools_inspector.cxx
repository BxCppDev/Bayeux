// -*- mode: c++ ; -*-
// geomtools_inspector.cxx

// Standard libraries:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <vector>

// Datatools:
#include <datatools/exception.h>
#include <datatools/logger.h>

// Geomtools:
#include <geomtools/geomtools_config.h>
#include <geomtools/geomtools_driver.h>

void print_splash(std::ostream & out_ = std::clog);

void print_help(std::ostream & out_ = std::clog);

void print_shell_help(geomtools::geomtools_driver & gd_,
                      std::ostream & out_ = std::clog);

int main(int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_WARNING;
  try {
    const std::string prompt = "geomtools> ";
    const std::string prompt_continue = "> ";
    bool splash = true;
    std::vector<std::string> GDP_argv;
    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg++];
      if (token[0] == '-' || token[0] == '+') {
        std::string option = token;
        if ((option == "-h") || (option == "--help"))  {
          print_help(std::clog);
          return 0;
        } else if ((option == "-S") || (option == "--no-splash"))  {
          splash = false;
        } else if ((option == "-G") || (option == "--logging")) {
          GDP_argv.push_back(option);
          std::string logging_label = argv_[iarg++];
          GDP_argv.push_back(logging_label);
          datatools::logger::priority lp = datatools::logger::get_priority(logging_label);
          if (lp == datatools::logger::PRIO_UNDEFINED) {
            DT_LOG_WARNING(logging, "Invalid logging priority '" << logging_label << "' ! Ignore !");
          } else {
            logging = lp;
          }
        } else {
          GDP_argv.push_back(option);
        }
      } else {
        std::string argument = token;
        GDP_argv.push_back(argument);
      }
    }

    if (splash) {
      print_splash(std::cerr);
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

    // The geomtools inspector shell:
    bool go_on = true;
    /// Browser main loop :
    while (go_on) {
      if (!std::cin || std::cin.eof()) {
        std::cerr << std::flush;
        std::cout << std::endl;
        break;
      }
      // Prompt:
      std::cerr << prompt << std::flush;
      std::string line;
      std::getline (std::cin, line);
      if (! std::cin || std::cin.eof()) {
        go_on = false;
      }
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
          std::cerr << prompt_continue << std::flush;
          std::string more;
          std::getline (std::cin, more);
          if (! std::cin || std::cin.eof()) {
            go_on = false;
          }
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
          while (command_iss) {
            std::string token;
            command_iss >> token >> std::ws;
            if (token.empty()) break;
            argv.push_back(token);
            if (command_iss.eof()) break;
          } // end of parsing
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
            while (command_iss) {
              std::string token;
              command_iss >> token >> std::ws;
              if (token.empty()) break;
              argv.push_back(token);
              if (command_iss.eof()) break;
            } // end of parsing
            int error = GD.command_export_gdml(argv);
            if (error != 0) {
              DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Cannot export GDML file !");
            }
          } else {
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "A plain geometry model factory cannot export GDML file !");
          }
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
        } else if (command == "d" || command == "display") {
          std::vector<std::string> argv;
          while (command_iss) {
            std::string token;
            command_iss >> token >> std::ws;
            if (token.empty()) break;
            argv.push_back(token);
            if (command_iss.eof()) break;
          } // end of parsing
          int error = GD.command_gnuplot_display(argv);
          if (error > 0) {
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Cannot display the geometry setup !");
          }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
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
  }
  catch (std::exception & x) {
    print_help ();
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL, "geomtools_inspector: " << x.what ());
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL, "geomtools_inspector: " << "Unexpected error !");
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

void print_splash(std::ostream & out_)
{
  out_ << "                                                 \n"
       << "\tG E O M T O O L S                              \n"
       << "\tVersion " << GEOMTOOLS_LIB_VERSION << "        \n"
       << "                                                 \n"
       << "\tCopyright (C) 2009-2013                        \n"
       << "\tFrancois Mauger, Xavier Garrido and Ben Morgan \n"
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

void print_help (std::ostream & out_)
{
  out_ << "geomtools_inspector -- Inspect and display a virtual geometry" << std::endl;
  out_ << "\n";
  out_ << "Usage: \n\n";

  out_ << "  geomtools_inspector [OPTIONS...] [MGRCFG_FILE]\n\n";
  out_ << "Options: \n\n";
  out_ << "     -h|--help             Print this help            \n";
  out_ << "     -S|--no-splash        Do not print splash screen \n";
  geomtools::geomtools_driver_params GDP;
  std::vector<std::string> argv;
  argv.push_back("--help-short");
  GDP.parse(argv, out_);

  out_ << "\n";
  out_ << "Examples:\n\n";
  out_ << "     geomtools_inspector --manager-config config/geometry/setup-1.0/manager.conf \n\n";
  return;
}

void print_shell_help (geomtools::geomtools_driver & gd_, std::ostream & out_)
{
  out_ <<  "  h | help                       : Print this help                                  \n";
  out_ <<  "  s | status                     : Print the status of the geometry driver          \n";
  out_ <<  "  i | initialize [OPTIONS] FILE  : Initialize the geometry driver                   \n";
  out_ <<  "                                   Use 'initialize --help' for the online help.     \n";
  out_ <<  "  r | reset                      : Terminate the geometry driver                    \n";
  out_ <<  "  P | print_manager              : Print the embeded geometry manager               \n";
  out_ <<  "  F | print_factory              : Print the embeded geometry model factory         \n";
  out_ <<  "  M | list_of_models [OPTIONS]   : Print the list of geometry models.               \n";
  out_ <<  "                                   Use 'list_of_models --help' for the list of options.   \n";
  out_ <<  "  L | list_of_logicals [OPTIONS] : Print the list of logical volumes.                     \n";
  out_ <<  "                                   Use 'list_of_logicals --help' for the list of options. \n";
  out_ <<  "  G | list_of_gids  [OPTIONS]    : Print the list of geometry identifiers (GID).          \n";
  out_ <<  "                                   Use 'list_of_gids --help' for the list of options.     \n";
  out_ <<  "  m | model MODEL_NAME           : Print the geometry model named 'MODEL_NAME'            \n";
  out_ <<  "  l | logical LOGICAL_NAME       : Print the logical volume named 'LOGICAL_NAME'          \n";
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

// end of geomtools_inspector.cxx
