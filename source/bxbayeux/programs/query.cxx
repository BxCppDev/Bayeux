//! \file  query.cxx
//! \brief Query information about Bayeux
//! \details
//
// Copyright (c) 2013-2020 by Francois Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2013-2020 by Université de Caen
//
// This file is part of Bayeux.
//
// Bayeux is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux. If not, see <http://www.gnu.org/licenses/>.

// Standard library
#include <iostream>
#include <cstdlib>
#include <exception>

// Third Party
// - Boost
#include <boost/program_options.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

// Bayeux
#include <bayeux/bayeux.h>
#include <bayeux/bayeux_config.h>
#include <bayeux/version.h>
#include <bayeux/reloc.h>
#include <bayeux/resource.h>
// - datatools
#include <datatools/kernel.h>
#include <datatools/library_info.h>

std::string app_name();

int main(int argc_, char * argv_[])
{
  bayeux::initialize(argc_, argv_, 0); //datatools::kernel::init_mask);
  int error_code = EXIT_SUCCESS;
  try {
    namespace bpo = boost::program_options;
    bpo::options_description opts("Options");
    opts.add_options()
      ("help,h",
       bpo::value<bool>()
       ->zero_tokens(),
       "Print help. \n"
       )
      ("version",
       bpo::value<bool>()
       ->zero_tokens(),
       "Print version. \n"
       )
      ("prefix",
       bpo::value<bool>()
       ->zero_tokens(),
       "Print installation prefix directory. \n"
       )
      ("libdir",
       bpo::value<bool>()
       ->zero_tokens(),
       "Print library directory. \n"
       )
      ("bindir",
       bpo::value<bool>()
       ->zero_tokens(),
       "Print binary directory. \n"
       )
      ("incdir",
       bpo::value<bool>()
       ->zero_tokens(),
       "Print include base directory. \n"
       )
      ("datadir",
       bpo::value<bool>()
       ->zero_tokens(),
       "Print data base directory. \n"
       )
      ("resourcedir",
       bpo::value<bool>()
       ->zero_tokens(),
       "Print resource base directory. \n"
       )
      ("bxresourcedir",
       bpo::value<bool>()
       ->zero_tokens(),
       "Print Bayeux resource base directory. \n"
       )
      ("exampledir",
       bpo::value<bool>()
       ->zero_tokens(),
       "Print example base directory. \n"
       )
      ("cmakedir",
       bpo::value<bool>()
       ->zero_tokens(),
       "Print CMake config directory. \n"
       )
      ("modules",
       bpo::value<bool>()
       ->zero_tokens(),
       "Print the list of Bayeux's modules. \n"
       )
      ("description",
       bpo::value<std::string>()
       ->value_name("[module]"),
       "Print the description of a Bayeux's module. \n"
       )
      ("has-geant4-module",
       bpo::value<bool>()
       ->zero_tokens(),
       "Print 1 if Bayeux was built with Geant4 \n"
       "extension module, else print 0.         \n"
       )
      ("with-bxdecay0",
       bpo::value<bool>()
       ->zero_tokens(),
       "Print 1 if Bayeux is built and linked with bxdecay0, \n"
       "else print 0.             \n"
       )
      ("with-qt",
       bpo::value<bool>()
       ->zero_tokens(),
       "Print 1 if Bayeux is built and linked with Qt5, \n"
       "else print 0.             \n"
       )
      ;
    bpo::positional_options_description args;
    bpo::variables_map vm;
    bpo::parsed_options parsed =
      bpo::command_line_parser(argc_, argv_)
      .options(opts)
      .allow_unregistered()
      .run();
    std::vector<std::string> unrecognized
      = bpo::collect_unrecognized(parsed.options,
                                  bpo::include_positional);
    bpo::store(parsed, vm);
    bpo::notify(vm);

    if (vm.count("help")) {
      std::cout << "Usage: " << app_name() << " [options]" << std::endl;
      std::cout << opts << std::endl;
    } else if (vm.count("version")) {
      bool version = vm["version"].as<bool>();
      if (version) {
        std::cout << BAYEUX_LIB_VERSION << std::endl;
      }
    } else if (vm.count("prefix")) {
      bool prefix = vm["prefix"].as<bool>();
      if (prefix) {
        std::cout << bayeux::get_prefix_dir() << std::endl;
      }
    } else if (vm.count("libdir")) {
      bool libdir = vm["libdir"].as<bool>();
      if (libdir) {
        std::cout << bayeux::get_library_dir() << std::endl;
      }
    } else if (vm.count("bindir")) {
      bool bindir = vm["bindir"].as<bool>();
      if (bindir) {
        std::cout << bayeux::get_binary_dir() << std::endl;
      }
    } else if (vm.count("datadir")) {
      bool datadir = vm["datadir"].as<bool>();
      if (datadir) {
        std::cout << bayeux::get_data_dir() << std::endl;
      }
    } else if (vm.count("incdir")) {
      bool incdir = vm["incdir"].as<bool>();
      if (incdir) {
        std::cout << bayeux::get_prefix_dir() << '/' << "include" << std::endl;
      }
    } else if (vm.count("resourcedir")) {
      bool incdir = vm["resourcedir"].as<bool>();
      if (incdir) {
        std::cout << bayeux::get_resource_dir() << std::endl;
      }
     } else if (vm.count("bxresourcedir")) {
      bool incdir = vm["bxresourcedir"].as<bool>();
      if (incdir) {
        std::cout << bayeux::get_bayeux_resource_dir() << std::endl;
      }
    } else if (vm.count("exampledir")) {
      bool incdir = vm["exampledir"].as<bool>();
      if (incdir) {
        std::cout << bayeux::get_data_dir() << "/../examples" << std::endl;
      }
    } else if (vm.count("cmakedir")) {
      bool cmakedir = vm["cmakedir"].as<bool>();
      if (cmakedir) {
        std::cout << bayeux::get_library_dir() << '/' << "cmake/Bayeux-" << BAYEUX_LIB_VERSION << std::endl;
      }
    } else if (vm.count("modules")) {
      bool mod = vm["modules"].as<bool>();
      if (mod) {
        std::vector<std::string> modules;
        boost::split(modules, BAYEUX_SUBMODULES, boost::is_any_of(";"));
        for (size_t i = 0; i < modules.size(); i++) {
          std::cout << modules[i] << std::endl;
        }
      }
    } else if (vm.count("has-geant4-module")) {
      bool has_geant4 = vm["has-geant4-module"].as<bool>();
      if (has_geant4) {
        std::cout << BAYEUX_WITH_GEANT4_MODULE << std::endl;
      }
    } else if (vm.count("with-bxdecay0")) {
      bool with_bxdecay0 = vm["with-bxdecay0"].as<bool>();
      if (with_bxdecay0) {
        std::cout << BAYEUX_WITH_BXDECAY0 << std::endl;
      }
    } else if (vm.count("with-qt")) {
      bool with_qt = vm["with-qt"].as<bool>();
      if (with_qt) {
        std::cout << BAYEUX_WITH_QT << std::endl;
      }
    } else if (vm.count("description")) {
      std::string module_name = vm["description"].as<std::string>();
      DT_THROW_IF (module_name.empty(),
                   std::logic_error,
                   "Missing module name !");
      std::vector<std::string> modules;
      boost::split(modules, BAYEUX_SUBMODULES, boost::is_any_of(";"));
      modules.push_back("bayeux");
      DT_THROW_IF (std::find(modules.begin(), modules.end(), module_name) == modules.end(),
                   std::domain_error,
                   "No module with name '" << module_name << "'!");
      datatools::kernel & krnl = datatools::kernel::instance();
      if (krnl.has_library_info_register()) {
        const datatools::library_info & lib_info_reg = krnl.get_library_info_register();
        if (lib_info_reg.has(module_name)) {
          const datatools::properties & module_lib_infos = lib_info_reg.get(module_name);
          std::cout << "module=" << module_name<< "" << std::endl;
          // if (module_lib_infos.has_key(datatools::library_info::keys::version())) {
          //   std::cout << "version=" << module_lib_infos.fetch_string(datatools::library_info::keys::version()) << std::endl;
          // }
          if (module_lib_infos.has_key(datatools::library_info::keys::description())) {
            std::cout << "description=" << module_lib_infos.fetch_string(datatools::library_info::keys::description()) << std::endl;
          }
          if (module_lib_infos.has_key(datatools::library_info::keys::install_resource_dir())) {
            std::cout << "install_resource_dir=" << module_lib_infos.fetch_string(datatools::library_info::keys::install_resource_dir()) << std::endl;
          }
          if (module_lib_infos.has_key(datatools::library_info::keys::env_resource_dir())) {
            std::cout << "env_resource_dir=" << module_lib_infos.fetch_string(datatools::library_info::keys::env_resource_dir()) << std::endl;
          }
        }
      }
    } else {
      std::ostringstream message;
      message << "Missing option !\n" << opts;
      throw std::logic_error(message.str());
    }

    if (unrecognized.size()) {
      std::ostringstream message;
      message << "Unrecognized option '" << unrecognized[0] << "' !";
      throw std::logic_error(message.str());
    }

  } catch (std::exception & error) {
    std::cerr << "[error] " << app_name() << ": " << error.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "[error] " << app_name() << ": " << "Unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  bayeux::terminate();
  return error_code;
}

std::string app_name()
{
  return "bxquery";
}
