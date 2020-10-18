//! \file  bxvariant_inspector.cxx
//! \brief Variant parameters inspector program

// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>
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

// Standard Library:
#include <cstdlib>
#include <string>
#include <iostream>
#include <exception>
#include <vector>

// Third Party:
// - Boost++:
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

// This Project:
// - Bayeux:
#include <bayeux/bayeux.h>
// - Bayeux/datatools:
#include <datatools/kernel.h>
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/utils.h>
#include <datatools/version.h>
#include <datatools/configuration/variant_service.h>
#include <datatools/configuration/variant_repository.h>
#include <datatools/configuration/variant_registry.h>
#include <datatools/configuration/io.h>
#include <datatools/ioutils.h>
#include <datatools/properties.h>
#include <datatools/multi_properties.h>
#include <datatools/library_loader.h>

namespace bpo = boost::program_options;
namespace dtc = datatools::configuration;

//! Return the application name
std::string app_name();

//! Print application usage (supported options and arguments)
void app_usage(std::ostream & out_,
               const bpo::options_description & desc_);

//! Print application version
void app_version(std::ostream & out_);

//! \brief Application configuration parameters
struct app_config_params {
  //! Default constructor
  app_config_params();
  std::string logging; //!< Logging label
  std::string action; //!< Action
  std::string input_filename; //!< Input file for test action
  dtc::variant_service::config variants; //!< Variant support
  std::string LL_config;
  std::vector<std::string> LL_dlls;
};

void app_dump_debug(std::ostream & out_, const dtc::variant_repository & vrep_);

void app_print_doc_rst(std::ostream & out_, const dtc::variant_repository & vrep_);

void app_print_current_profile(std::ostream & out_, const dtc::variant_repository & vrep_);

void app_test_config(std::ostream & out_, const std::string & config_file_);

void app_test_multi_config(std::ostream & out_, const std::string & mconfig_file_);

// Return kernel initialization flags for this application:
uint32_t app_kernel_init_flags();

int main(int argc_, char * argv_[])
{
  bayeux::initialize(argc_, argv_, app_kernel_init_flags());
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_WARNING;
  app_config_params params;

  namespace bpo = boost::program_options;
  try {

    // Declare options:
    bpo::options_description optDesc("Options");
    optDesc.add_options()
      ("help,h",
       "print this help message")
      ("version,v",
       "print version")
      ("logging,l",
       bpo::value<std::string>(&params.logging)
       ->default_value("fatal")
       ->value_name("[level]"),
       "a value\n"
       "Example: \n"
       "  --logging \"debug\""
       )
      ("load-dll,L",
       bpo::value<std::vector<std::string>>(&params.LL_dlls)
       ->value_name("[library]"),
       "a library to be loaded\n"
       "Example: \n"
       "  --load-dll \"my@library/installation/path\""
       )
      ("dll-config",
       bpo::value<std::string>(&params.LL_config)
       ->value_name("[file]"),
       "a configuration file for the library loader\n"
       "Example: \n"
       "  --dll-config \"${HOME}/config/mydlls.conf\""
       )
      ("action,a",
       bpo::value<std::string>(&params.action)
       // ->default_value("doc")
       ->value_name("[name]"),
       "action name:\n"
       " - 'doc' : print ReSt formatted documentation\n"
       " - 'profile' : print the current variant profile\n"
       " - 'testcfg' : test a properties input file\n"
       " - 'testmcfg' : test a multi-properties input file\n"
       "Example:\n"
       "  --action \"doc\""
       )
      ("input-file,i",
       bpo::value<std::string>(&params.input_filename)
       ->value_name("[filename]"),
       "input file to be parsed/tested.\n"
       "Needed for action='testcfg' or action='testmcfg'.\n"
       "Example:\n"
       "  --action \"testcfg\" --input-file \"my_parameters.conf\""
       )
      ;

    // Declare options for variant support:
    bpo::options_description optVariant("Variant support");
    uint32_t po_init_flags = 0;
    po_init_flags |= dtc::variant_service::NO_LABEL;
    // po_init_flags |= dtc::variant_service::NO_LOGGING;
    // po_init_flags |= dtc::variant_service::PROFILE_LOAD_DONT_IGNORE_UNKNOWN;
    po_init_flags |= dtc::variant_service::NO_TUI;
    dtc::variant_service::init_options(optVariant,
                                       params.variants,
                                       po_init_flags);

    // Declare options for kernel:
    bpo::options_description optKernel("Kernel options");
    datatools::kernel::param_type paramsKernel;
    datatools::kernel::build_opt_desc(optKernel,
                                      paramsKernel,
                                      app_kernel_init_flags());

    // Aggregate options:
    bpo::options_description optPublic;
    optPublic.add(optDesc).add(optVariant).add(optKernel);

    // Parse options:
    bpo::variables_map vMap;
    try {
      bpo::store(bpo::parse_command_line(argc_, argv_, optPublic), vMap);
      bpo::notify(vMap);
    } catch (const bpo::required_option& e) {
      // We need to handle help even if required_option thrown
      if (!vMap.count("help") && !vMap.count("version")) {
        std::cerr << "[OptionsException] " << e.what() << std::endl;
        throw std::logic_error(e.what());
      }
    } catch (const std::exception& e) {
      std::cerr << "[OptionsException] " << e.what() << std::endl;
      throw std::logic_error(e.what());
    }

    // Handle any non-bound options:
    if (vMap.count("help")) {
      app_usage(std::cout, optPublic);
      error_code = -1;
    }

    if (vMap.count("version")) {
      app_version(std::cout);
      error_code = -1;
    }

    if (error_code == EXIT_SUCCESS) {
      if (vMap.count("logging")) {
        datatools::logger::priority prio = datatools::logger::get_priority(params.logging);
        if (prio != datatools::logger::PRIO_UNDEFINED) {
          logging = prio;
        }
      }

      // Load dynamic libraries, if any are requested:
      datatools::library_loader LL(params.LL_config);
      for (auto dll_name : params.LL_dlls) {
        DT_LOG_NOTICE(logging, "Loading DLL '" << dll_name << "'...");
        DT_THROW_IF (LL.load(dll_name) != EXIT_SUCCESS,
                     std::runtime_error,
                     "Loading DLL '" << dll_name << "' failed !");
      }

      // Set action:
      bool print_doc = false;
      bool print_profile = false;
      bool test_config = false;
      bool test_mconfig = false;

      // if (params.action.empty()) {
      //   params.action = "doc";
      // }

      if (params.action == "doc") {
        print_doc = true;
      } else if (params.action == "profile") {
        print_profile = true;
      } else if (params.action == "testcfg") {
        test_config = true;
      } else if (params.action == "testmcfg") {
        test_mconfig = true;
      }

      // Variant support:
      try {
        if (datatools::logger::is_debug(logging)) {
          params.variants.print(std::cerr, "[debug] Variant parameters: ");
        }
        if (params.variants.is_active()) {
          dtc::variant_service vserv;
          vserv.configure(params.variants);

          if (datatools::logger::is_debug(logging)) {
            app_dump_debug(std::cerr, vserv.get_repository());
          }

          // Start the variant service:
          vserv.start();

          // Action:
          if (print_doc) {
            app_print_doc_rst(std::cout, vserv.get_repository());
          } else if (print_profile) {
            app_print_current_profile(std::cout, vserv.get_repository());
          } else if (test_config) {
            DT_THROW_IF(params.input_filename.empty(),
                        std::logic_error,
                        "Missing '--input-file' for testing a 'datatools::properties' configuration file!");
            app_test_config(std::cout, params.input_filename);
          } else if (test_mconfig) {
            DT_THROW_IF(params.input_filename.empty(),
                        std::logic_error,
                        "Missing '--input-file' for testing a 'datatools::multi_properties' configuration file!");
            app_test_multi_config(std::cout, params.input_filename);
          }

          // Stop the variant service:
          if (vserv.is_started()) {
            vserv.stop();
          }
        }

      } catch (std::exception & error) {
        std::cerr << "[error] " << error.what()
                  << std::endl;
        error_code = EXIT_FAILURE;
        throw std::logic_error(error.what());
      }
    }
  } catch (const std::exception & error) {
    DT_LOG_ERROR(datatools::logger::PRIO_ALWAYS, error.what());
    error_code = EXIT_FAILURE;
  } catch (...) {
    DT_LOG_ERROR(datatools::logger::PRIO_ALWAYS, "Unexpected Exception !");
    error_code = EXIT_FAILURE;
  }

  bayeux::terminate();
  return error_code;
}

// Definitions:
std::string app_name()
{
  return "bxvariant_inspector";
}

void app_version(std::ostream & out_)
{
  out_ << app_name() << " " << datatools::version::get_version() << std::endl;
  return;
}

void app_usage(std::ostream & out_, const bpo::options_description & opts_)
{
  out_ << app_name() << " - Browse/edit a variant repository" << std::endl;
  out_ << "Usage : " << std::endl;
  out_ << "  " << app_name() << " [options]..." << std::endl;
  out_ << opts_ << std::endl;
  return;
}

app_config_params::app_config_params()
{
  logging = "";
  action = "";
  return;
}

void app_dump_debug(std::ostream & out_, const dtc::variant_repository & vrep_)
{
  out_ << "Variant repository : '" << vrep_.get_name() << "'" << std::endl;
  out_ << "  Organization : '" << vrep_.get_organization() << "'" << std::endl;
  out_ << "  Application  : '" << vrep_.get_application() << "'" << std::endl;
  out_ << "  Description  : '" << vrep_.get_terse_description() << "'" << std::endl;
  out_ << "  Accomplished : " << vrep_.is_accomplished() << "" << std::endl;
  out_ << "  Locked       : " << vrep_.is_locked() << "'" << std::endl;
  out_ << "  Registries   : [" << vrep_.get_registries().size() << "]" << std::endl;

  std::vector<std::string> vreg_keys;
  vrep_.build_ordered_registry_keys(vreg_keys);
  for (std::size_t ivreg = 0; ivreg < vreg_keys.size(); ivreg++) {
    const std::string & vreg_key = vreg_keys[ivreg];
    const dtc::variant_registry & vreg = vrep_.get_registry(vreg_key);
    const std::string & vreg_name = vreg.get_name();
    // const std::string & vreg_display_name = vreg.get_display_name();
    const std::string & vreg_description = vreg.get_terse_description();
    bool vreg_accomplished = vreg.is_accomplished();
    out_ << "  Variant register : '" << vreg_name << "'" << std::endl;
    out_ << "    Description  : '" << vreg_description << "'" << std::endl;
    out_ << "    Accomplished : " << vreg_accomplished << "" << std::endl;
  }

  return;
}

void app_print_current_profile(std::ostream & out_, const dtc::variant_repository & vrep_)
{

  std::clog << "Current variant profile is: " << std::endl;
  std::clog << std::endl;
  dtc::ascii_io rep_io(dtc::ascii_io::IO_DEFAULT);
  rep_io.store_repository(out_, vrep_);
  out_ << std::endl;
  return;
}

void app_print_doc_rst(std::ostream & out_, const dtc::variant_repository & vrep_)
{
  vrep_.print_rst(out_, 0);
  return;
}

void app_test_config(std::ostream & out_, const std::string & config_file_)
{
  datatools::properties cfg;
  {
    uint32_t reader_options = 0;
    datatools::properties::config reader(reader_options);
    reader.read(config_file_, cfg);
  }
  {
    uint32_t writer_options = 0;
    datatools::properties::config writer(writer_options);
    writer.write(out_, cfg);
  }
  return;
}

void app_test_multi_config(std::ostream & out_, const std::string & mconfig_file_)
{
  datatools::multi_properties mcfg;
  {
    uint32_t reader_options = 0;
    datatools::multi_properties::config reader(reader_options);
    reader.read(mconfig_file_, mcfg);
  }
  {
    uint32_t writer_options = 0;
    datatools::multi_properties::config writer(writer_options);
    writer.write(out_, mcfg);
  }
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
