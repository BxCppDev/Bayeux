// -*- mode: c++ ; -*-
// materials_inspector.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <boost/foreach.hpp>

#include <datatools/properties.h>
#include <datatools/clhep_units.h>
#include <datatools/library_loader.h>

#include <materials/materials_config.h>
#include <materials/manager.h>

void print_help(std::ostream & out_);

enum action_flag_type {
  ACTION_UNDEFINED      = -1,
  ACTION_LIST_ISOTOPES  =  0x1,
  ACTION_LIST_ELEMENTS  =  0x2,
  ACTION_LIST_MATERIALS =  0x4,
  ACTION_LIST_ALIASES   =  0x8,
  ACTION_SHOW_ISOTOPE   =  0x10,
  ACTION_SHOW_ELEMENT   =  0x20,
  ACTION_SHOW_MATERIAL  =  0x40,
};

int main (int argc_, char ** argv_)
{
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  int error_code = EXIT_SUCCESS;
  try {
    int         nshoots = 0;
    int         action = 0;
    std::string show_target;
    std::vector<std::string> LL_dlls;
    std::string LL_config;
    std::string MaterialsMgrConfigFile;

    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];
      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-h") || (option == "--help")) {
          print_help(std::cout);
          return 0;
        } else if (option == "--load-dll") {
          LL_dlls.push_back (argv_[++iarg]);
        } else if (option == "--dll-config") {
          LL_config = argv_[++iarg];
        } else if ((option == "-L") || (option == "--logging-priority")) {
          std::string lp_name = argv_[++iarg];
          logging = datatools::logger::get_priority(lp_name);
          DT_THROW_IF(logging == datatools::logger::PRIO_UNDEFINED,
                      std::logic_error, "Undefined logging priority '"
                      << lp_name << "' !");
        } else if ((option == "-c") || (option == "--manager-config")) {
          MaterialsMgrConfigFile = argv_[++iarg];
        } else if ((option == "-I") || (option == "--list-isotopes")) {
          action |= ACTION_LIST_ISOTOPES;
        } else if ((option == "-E") || (option == "--list-elements")) {
          action |= ACTION_LIST_ELEMENTS;
        } else if ((option == "-M") || (option == "--list-materials")) {
          action |= ACTION_LIST_MATERIALS;
        } else if ((option == "-A") || (option == "--list-aliases")) {
          action |= ACTION_LIST_ALIASES;
        } else if ((option == "-i") || (option == "--show-isotope")) {
          action |= ACTION_SHOW_ISOTOPE;
          show_target = argv_[++iarg];
        } else if ((option == "-e") || (option == "--show-element")) {
          action |= ACTION_SHOW_ELEMENT;
          show_target = argv_[++iarg];
        } else if ((option == "-m") || (option == "--show-material")) {
          action |= ACTION_SHOW_MATERIAL;
          show_target = argv_[++iarg];
        } else {
          DT_LOG_WARNING(logging, "Ignoring option '" << option << "' !");
        }
      } else {
        std::string argument = token;
        if (MaterialsMgrConfigFile.empty()) {
          MaterialsMgrConfigFile = argument;
        } else {
          DT_LOG_WARNING(logging, "Ignoring argument '" << argument << "' !");
        }
      }
      iarg++;
    }

    uint32_t LL_flags = datatools::library_loader::allow_unregistered;
    datatools::library_loader LL(LL_flags, LL_config);
    BOOST_FOREACH (const std::string & dll_name, LL_dlls) {
      DT_LOG_NOTICE(logging,"Loading DLL '" << dll_name << "'...");
      DT_THROW_IF (LL.load(dll_name) != EXIT_SUCCESS,
                   std::runtime_error,
                   "Loading DLL '" << dll_name << "' failed !");
    }

    if (action == 0) {
      DT_LOG_NOTICE(logging, "Perform default action : list materials...");
      action = ACTION_LIST_MATERIALS;
    }

    // Configuration files :
    DT_THROW_IF(MaterialsMgrConfigFile.empty(),
                std::logic_error,
                "Missing materials manager configuration file !");

    // Materials manager :
    materials::manager MatMgr;
    datatools::fetch_path_with_env(MaterialsMgrConfigFile);
    datatools::properties MaterialsMgrConfig;
    datatools::properties::read_config (MaterialsMgrConfigFile, MaterialsMgrConfig);
    MatMgr.initialize(MaterialsMgrConfig);
    if (logging >= datatools::logger::PRIO_DEBUG) {
      DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "The materials manager : ");
      MatMgr.tree_dump(std::clog);
    }

    if (action & ACTION_LIST_ISOTOPES) {
      std::cerr << "List of isotopes: " << std::endl;
      const materials::isotope_dict_type & isotopes = MatMgr.get_isotopes ();
      for (materials::isotope_dict_type::const_iterator i
             = isotopes.begin();
           i != isotopes.end();
           i++) {
        materials::isotope_dict_type::const_iterator j = i;
        j++;
        if (j == isotopes.end()) {
          std::cerr << "`-- ";
        } else {
          std::cerr << "|-- ";
        }
        std::cerr << "'" << std::flush;
        std::cout << i->first;
        std::cerr << "'" << std::flush;
        std::cout << std::endl;
      }
    }

    if (action & ACTION_LIST_ELEMENTS) {
      std::cerr << "List of elements: " << std::endl;
      const materials::element_dict_type & elements = MatMgr.get_elements ();
      for (materials::element_dict_type::const_iterator i
             = elements.begin();
           i != elements.end();
           i++) {
        materials::element_dict_type::const_iterator j = i;
        j++;
        if (j == elements.end()) {
          std::cerr << "`-- ";
        } else {
          std::cerr << "|-- ";
        }
        std::cerr << "'" << std::flush;
        std::cout << i->first;
        std::cerr << "'" << std::flush;
        std::cout << std::endl;
      }
    }

    if (action & ACTION_LIST_MATERIALS) {
      std::cerr << "List of materials: " << std::endl;
      const materials::material_dict_type & materials = MatMgr.get_materials ();
      for (materials::material_dict_type::const_iterator i
             = materials.begin();
           i != materials.end();
           i++) {
        materials::material_dict_type::const_iterator j = i;
        j++;
        if (j == materials.end()) {
          std::cerr << "`-- ";
        } else {
          std::cerr << "|-- ";
        }
        std::cerr << std::flush;
        std::cerr << "'" << std::flush;
        std::cout << i->first << std::flush;
        std::cerr << "'" << std::flush;
        if (i->second.is_alias()) {
          std::cerr << " alias of '" << i->second.get_alias_of() << "' " << std::flush;;
        }
        std::cout << std::endl;
      }
    }

    if (action & ACTION_LIST_ALIASES) {
      std::map<std::string,std::string> aliases;
      std::cerr << "List of material aliases: " << std::endl;
      const materials::material_dict_type & materials = MatMgr.get_materials ();
      for (materials::material_dict_type::const_iterator i
             = materials.begin();
           i != materials.end();
           i++) {
        if (i->second.is_alias()) {
          aliases[i->first] = i->second.get_alias_of();
        }
      }

      for (std::map<std::string,std::string>::const_iterator i
             = aliases.begin();
           i != aliases.end();
           i++) {
        std::map<std::string,std::string>::const_iterator j = i;
        j++;
        if (j == aliases.end()) {
          std::cerr << "`-- ";
        } else {
          std::cerr << "|-- ";
        }
        std::cerr << std::flush;
        std::cerr << "'" << std::flush;
        std::cout << i->first << std::flush;
        std::cerr << "'" << std::flush;
        std::cerr << " alias of '" << i->second << "' " << std::flush;;
        std::cout << std::endl;
      }
    }

    if (action & ACTION_SHOW_ISOTOPE) {
      const materials::isotope_dict_type & isotopes = MatMgr.get_isotopes ();
      materials::isotope_dict_type::const_iterator found = isotopes.find(show_target);
      DT_THROW_IF (found == isotopes.end(), std::logic_error,
                   "Unknown isotope '" << show_target << "' !");
      found->second.get_ref().tree_dump(std::cout, "Isotope: ");
    }

    if (action & ACTION_SHOW_ELEMENT) {
      const materials::element_dict_type & elements = MatMgr.get_elements ();
      materials::element_dict_type::const_iterator found = elements.find(show_target);
      DT_THROW_IF (found == elements.end(), std::logic_error,
                   "Unknown element '" << show_target << "' !");
      found->second.get_ref().tree_dump(std::cout, "Element: ");
    }

    if (action & ACTION_SHOW_MATERIAL) {
      const materials::material_dict_type & materials = MatMgr.get_materials ();
      materials::material_dict_type::const_iterator found = materials.find(show_target);
      DT_THROW_IF (found == materials.end(), std::logic_error,
                   "Unknown material '" << show_target << "' !");
      found->second.get_ref().tree_dump(std::cout, "Material: ");
    }

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
  out << "Inspect vertices from a vertex generator managed                            \n";
  out << "by a vertex generator manager and coupled to a                              \n";
  out << "geometry manager.                                                           \n";
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
