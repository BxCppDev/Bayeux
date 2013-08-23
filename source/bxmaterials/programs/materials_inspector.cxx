// -*- mode: c++ ; -*-
// materials_inspector.cxx

// Standard Library
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Datatools
#include <datatools/logger.h>
#include <datatools/exception.h>

// Materials
#include <materials/materials_driver.h>

void print_help(std::ostream & out_);

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  materials::materials_driver_params MDP;
  try {

    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];
      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-h") || (option == "--help")) {
          print_help(std::cout);
          return 0;
        } else if (option == "--load-dll") {
          MDP.LL_dlls.push_back (argv_[++iarg]);
        } else if (option == "--dll-config") {
          MDP.LL_config = argv_[++iarg];
        } else if ((option == "-L") || (option == "--logging-priority")) {
          std::string lp_name = argv_[++iarg];
          logging = datatools::logger::get_priority(lp_name);
          DT_THROW_IF(logging == datatools::logger::PRIO_UNDEFINED,
                      std::logic_error, "Undefined logging priority '"
                      << lp_name << "' !");
        } else if ((option == "-c") || (option == "--manager-config")) {
          MDP.MaterialsMgrConfigFile = argv_[++iarg];
        } else if ((option == "-I") || (option == "--list-isotopes")) {
          MDP.action |= materials::materials_driver_params::ACTION_LIST_ISOTOPES;
        } else if ((option == "-E") || (option == "--list-elements")) {
          MDP.action |= materials::materials_driver_params::ACTION_LIST_ELEMENTS;
        } else if ((option == "-M") || (option == "--list-materials")) {
          MDP.action |= materials::materials_driver_params::ACTION_LIST_MATERIALS;
        } else if ((option == "-A") || (option == "--list-aliases")) {
          MDP.action |= materials::materials_driver_params::ACTION_LIST_ALIASES;
        } else if ((option == "-i") || (option == "--show-isotope")) {
          MDP.action |= materials::materials_driver_params::ACTION_SHOW_ISOTOPE;
          MDP.show_target = argv_[++iarg];
        } else if ((option == "-e") || (option == "--show-element")) {
          MDP.action |= materials::materials_driver_params::ACTION_SHOW_ELEMENT;
          MDP.show_target = argv_[++iarg];
        } else if ((option == "-m") || (option == "--show-material")) {
          MDP.action |= materials::materials_driver_params::ACTION_SHOW_MATERIAL;
          MDP.show_target = argv_[++iarg];
        } else if (option == "--with-decoration") {
          MDP.with_decoration = true;
        } else if (option == "--without-decoration") {
          MDP.with_decoration = false;
        } else {
          MDP.action_options.push_back(option);
        }
      } else {
        std::string argument = token;
        if (MDP.MaterialsMgrConfigFile.empty()) {
          MDP.MaterialsMgrConfigFile = argument;
        } else {
          DT_LOG_WARNING(logging, "Ignoring argument '" << argument << "' !");
        }
      }
      iarg++;
    }

    MDP.logging = logging;
    materials::materials_driver MD;
    MD.initialize(MDP);
    MD.run_action(std::cout, std::clog);
    MD.reset();

  }
  catch (std::exception & x) {
    DT_LOG_FATAL(logging, x.what());
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    DT_LOG_FATAL(logging, "Unexpected error !");
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

void print_help(std::ostream & out)
{
  out << "Inspect materials from a isotopes/elements/materials manager.               \n";
  out << "                                                                            \n";
  out << "Usage::                                                                     \n";
  out << "                                                                            \n";
  out << "   materials_inspector [OPTION]...                                          \n";
  out << "                                                                            \n";
  out << "Options::                                                                   \n";
  out << "                                                                            \n";
  out << "  -h, --help :                                                              \n";
  out << "     print this help then exit.                                             \n";
  out << "  --load-dll LIBRARYNAME :                                                  \n";
  out << "     load the LIBRARYNAME dynamic library.                                  \n";
  out << "  --dll-config DLLCONFIGFILE :                                              \n";
  out << "     use the DLLCONFIGFILE dynamic library loader configuration file.       \n";
  out << "  -L, --logging-priority PRIORITY :                                         \n";
  out << "     set the logging priority (default=\"fatal\").                          \n";
  out << "  -c, --manager-config CONFIGFILE :                                         \n";
  out << "     set the configuration file for the materials manager.                  \n";
  out << "  -I, --list-isotopes :                                                     \n";
  out << "     list the available isotopes from the materials manager.                \n";
  out << "  -E, --list-elements :                                                     \n";
  out << "     list the available elements from the materials manager.                \n";
  out << "  -M, --list-materials :                                                    \n";
  out << "     list the available materials from the materials manager.               \n";
  out << "  -A, --list-aliases :                                                      \n";
  out << "     list the available materials aliases from the materials manager.       \n";
  out << "  --with-decoration : print fancy headers, titles and decoration            \n";
  out << "                      (only for -I, -E, -M and -A)                          \n";
  out << "  --without-decoration : do not print fancy headers, titles and decoration  \n";
  out << "                         (only for -I, -E, -M and -A)                       \n";
  out << "  -i, --show-isotope ISONAME  : show isotope ISONAME.                       \n";
  out << "  -e, --show-element ELENAME  : show element ELENAME.                       \n";
  out << "  -m, --show-material MATNAME : show material MATNAME.                      \n";
  out << "                                                                            \n";
  out << "Example:                                                                    \n";
  out << "                                                                            \n";
  out << "  List the available isotopes::                                             \n";
  out << "                                                                            \n";
  out << "     materials_inspector \\                                                 \n";
  out << "       --materials-manager \"${CONFIG_DIR}/materials/manager.conf\" \\      \n";
  out << "       --list-isotopes                                                      \n";
  out << "                                                                            \n";
  return;
}

// end of materials_inspector.cxx
