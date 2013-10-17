// -*- mode: c++ ; -*-
// materials_inspector.cxx

// This project
#include <materials/materials_config.h>

// Standard Library
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third Party
// - Boost
#include <boost/program_options.hpp>

#if DATATOOLS_STANDALONE == 0
// - bayeux:
#include <bayeux/bayeux.h>
#endif // DATATOOLS_STANDALONE == 1

// - datatools
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/datatools.h>

// - materials
#include <materials/materials.h>
#include <materials/materials_driver.h>

namespace materials {
  struct ui {

    static
    void print_usage(std::ostream & out_,
                     const boost::program_options::options_description & opts_);

  };
}

int main (int argc_, char ** argv_)
{
  MATERIALS_INIT_MAIN(argc_,argv_);
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  materials::materials_driver_params MDP;
  namespace po = boost::program_options;
  po::options_description opts("Allowed options ");

  try {
    // Temporary variables for command line parsing:
    std::string logging_label = "warning";
    bool help           = false;
    bool dump_manager   = false;
    bool list_isotopes  = false;
    bool list_elements  = false;
    bool list_materials = false;
    bool list_aliases   = false;
    std::string show_isotope;
    std::string show_element;
    std::string show_material;
    bool with_decoration    = false;
    bool without_decoration = false;

    opts.add_options ()

      ("help,h",
       po::value<bool>(&help)
       ->zero_tokens()
       ->default_value(false),
       "Print help then exit.")

      ("logging,G",
       po::value<std::string> (&logging_label),
       "Set the logging priority threshold.         \n"
       "Example :                                   \n"
       " --logging \"trace\"                          "
       )

      ("dlls-config,L",
       po::value<std::string> (&MDP.LL_config),
       "Set the DLL loader configuration file.      \n"
       "Example :                                   \n"
       " --dlls-config dlls.conf                    "
       )

      ("load-dll,l",
       po::value<std::vector<std::string> >(&MDP.LL_dlls),
       "Set a DLL to be loaded.                     \n"
       "Example :                                   \n"
       " --load-dll stdmat                            "
       )

      ("manager-config,c",
       po::value<std::string>(&MDP.MaterialsMgrConfigFile),
       "Set the materials manager configuration file.   \n"
       "Example :                                       \n"
       " --manager-config config/materials/manager.conf   "
       )

      ("dump-manager,D",
       po::value<bool>(&dump_manager)
       ->zero_tokens()
       ->default_value(false),
       "Dump the material manager's internals.")

      ("list-isotopes,I",
       po::value<bool>(&list_isotopes)
       ->zero_tokens()
       ->default_value(false),
       "Prints the list of registered isotopes.")

      ("list-elements,E",
       po::value<bool>(&list_elements)
       ->zero_tokens()
       ->default_value (false),
       "Prints the list of registered elements.")

      ("list-materials,M",
       po::value<bool>(&list_materials)
       ->zero_tokens()
       ->default_value (false),
       "Prints the list of registered materials.")

      ("list-aliases,A",
       po::value<bool>(&list_aliases)
       ->zero_tokens()
       ->default_value (false),
       "Prints the list of registered material aliases.")

      ("show-isotope,i",
       po::value<std::string>(&show_isotope),
       "Show a registered isotope.                  \n"
       "Example :                                   \n"
       " --show-isotope \"He-6\"                      ")

      ("show-element,e",
       po::value<std::string>(&show_element),
       "Show a registered element.                  \n"
       "Example :                                   \n"
       " --show-element \"Al\"                        ")

      ("show-material,m",
       po::value<std::string>(&show_material),
       "Show a registered material.                  \n"
       "Example :                                    \n"
       " --show-material \"ShieldingMetal\"            ")

      ("with-decoration,o",
       po::value<bool>(&with_decoration)
       ->zero_tokens()
       ->default_value(true),
       "produce list output with fancy headers, titles and decoration.")

      ("without-decoration,O",
       po::value<bool>(&without_decoration)
       ->zero_tokens()
       ->default_value(false),
       "produce list output without fancy headers, titles and decoration.")

      ; // end of options' description

    // Describe command line arguments :
    po::positional_options_description args;
    args.add("manager-config", -1);

    po::variables_map vm;
    po::parsed_options parsed =
      po::command_line_parser(argc_, argv_)
      .options(opts)
      .positional(args)
      .allow_unregistered()
      .run();
    MDP.action_options =
      po::collect_unrecognized(parsed.options,
                               po::include_positional);
    po::store(parsed, vm);
    po::notify(vm);

    bool run = true;
    // Fetch the opts/args :
    if (help) {
      materials::ui::print_usage(std::cout, opts);
      run = false;
    }

    if (run) {

      if (vm.count("logging")) {
        logging = datatools::logger::get_priority(vm["logging"].as<std::string>());
        MDP.logging = logging;
      }

      if (dump_manager) {
        MDP.action |= materials::materials_driver_params::ACTION_DUMP_MANAGER;
      }

      if (list_isotopes) {
        MDP.action |= materials::materials_driver_params::ACTION_LIST_ISOTOPES;
      }

      if (list_elements) {
        MDP.action |= materials::materials_driver_params::ACTION_LIST_ELEMENTS;
      }

      if (list_materials) {
        MDP.action |= materials::materials_driver_params::ACTION_LIST_MATERIALS;
      }

      if (list_aliases) {
        MDP.action |= materials::materials_driver_params::ACTION_LIST_ALIASES;
      }

      if (vm.count("show-isotope")) {
        MDP.show_target = vm["show-isotope"].as<std::string>() ;
        MDP.action |= materials::materials_driver_params::ACTION_SHOW_ISOTOPE;
      }

      if (vm.count("show-element")) {
        MDP.show_target = vm["show-element"].as<std::string>() ;
        MDP.action |= materials::materials_driver_params::ACTION_SHOW_ELEMENT;
      }

      if (vm.count("show-material")) {
        MDP.show_target = vm["show-material"].as<std::string>() ;
        MDP.action |= materials::materials_driver_params::ACTION_SHOW_MATERIAL;
      }

      if (with_decoration) {
        MDP.with_decoration = true;
      }

      if (without_decoration) {
        MDP.with_decoration = false;
      }

      if (logging >= datatools::logger::PRIO_DEBUG) {
        MDP.dump(std::cerr);
      }

      // Run the driver:
      materials::materials_driver MD;
      MD.initialize(MDP);
      MD.run_action(std::cout, std::clog);
      MD.reset();
    } // end of run

  }
  catch (std::exception & x) {
    DT_LOG_FATAL(logging, x.what());
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    DT_LOG_FATAL(logging, "Unexpected error !");
    error_code = EXIT_FAILURE;
  }
  if (error_code != EXIT_SUCCESS) {
    materials::ui::print_usage(std::cerr, opts);
  }
  MATERIALS_FINI();
  return (error_code);
}

namespace materials {

  void ui::print_usage(std::ostream & out_,
                       const boost::program_options::options_description & opts_)
  {
    out_ << "Inspect a manager of registered isotopes, elements and materials.           \n";
    out_ << "                                                                            \n";
    out_ << "Usage:                                                                      \n";
    out_ << "                                                                            \n";
    out_ << "  materials_inspector [[option]... ] [manager config file]                  \n";
    out_ << "                                                                            \n";
    out_ << opts_;
    out_ << "                                                                            \n";
    out_ << "Examples:                                                                   \n";
    out_ << "                                                                            \n";
    out_ << "  List the list of registered isotopes:                                     \n";
    out_ << "                                                                            \n";
    out_ << "   $ materials_inspector \\                                                 \n";
    out_ << "       --manager-config \"${CONFIG_DIR}/materials/manager.conf\" \\         \n";
    out_ << "       --list-isotopes --with-decoration                                    \n";
    out_ << "                                                                            \n";
    out_ << "  Print a registered material:                                              \n";
    out_ << "                                                                            \n";
    out_ << "   $ materials_inspector \\                                                 \n";
    out_ << "       --manager-config \"${CONFIG_DIR}/materials/manager.conf\" \\         \n";
    out_ << "       --show-material \"Steel100Cr6\"                                      \n";
    out_ << "                                                                            \n";
    return;
  }

} // end of materials

// end of materials_inspector.cxx
