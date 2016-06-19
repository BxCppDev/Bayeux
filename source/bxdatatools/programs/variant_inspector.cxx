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

// Third Party:
// - Boost++:
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
// namespace bpo = boost::program_options;

// This Project:
// - Bayeux:
#include <bayeux/bayeux.h>
// - Bayeux/datatools:
//#include <datatools/kernel.h>
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/utils.h>
#include <datatools/version.h>
#include <datatools/configuration/variant_service.h>
#include <datatools/configuration/variant_repository.h>
#include <datatools/configuration/variant_registry.h>
#include <datatools/configuration/io.h>
#include <datatools/ioutils.h>

namespace bpo = boost::program_options;
namespace dtc = datatools::configuration;

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
  dtc::variant_service::config variants; //!< Variant support
};

void debug_app_dump(std::ostream & out_, const dtc::variant_repository & vrep_);

void app_print_rst(std::ostream & out_, const dtc::variant_repository & vrep_);

void app_print_current_profile(std::ostream & out_, const dtc::variant_repository & vrep_);

  int main(int argc_, char * argv_[])
{
  bayeux::initialize(argc_, argv_);
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
      ("logging",
       bpo::value<std::string>(&params.logging)
       ->default_value("fatal")
       ->value_name("[level]"),
       "a value")
      ("version,v",
       "print version")
      ("action",
       bpo::value<std::string>(&params.action)
       // ->default_value("doc")
       ->value_name("[name]"),
       "action name:\n"
       " - 'doc' : print ReSt formatted documentation\n"
       " - 'profile' : print the current variant profile\n")
      ;

    // Declare options for variant support:
    bpo::options_description optVariant("Variant support");
    uint32_t po_init_flags = 0;
    po_init_flags |= dtc::variant_service::NO_LABEL;
    po_init_flags |= dtc::variant_service::NO_LOGGING;
    // po_init_flags |= dtc::variant_service::PROFILE_LOAD_DONT_IGNORE_UNKNOWN;
    po_init_flags |= dtc::variant_service::NO_TUI;
    dtc::variant_service::init_options(optVariant,
                                       params.variants,
                                       po_init_flags);

    // Aggregate options:
    bpo::options_description optPublic;
    optPublic.add(optDesc).add(optVariant);

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
      return -1;
    }

    if (vMap.count("version")) {
      app_version(std::cout);
      return -1;
    }

    if (vMap.count("logging")) {
      datatools::logger::priority prio = datatools::logger::get_priority(params.logging);
      if (prio != datatools::logger::PRIO_UNDEFINED) {
        logging = prio;
        params.logging = params.logging;
      }
    }

    // Set action:
    bool print_doc = false;
    bool print_profile = false;

    if (params.action.empty()) {
      params.action = "doc";
    }

    if (params.action == "doc") {
      print_doc = true;
    } else if (params.action == "profile") {
      print_profile = true;
    }

    // Variant support:
    try {
      if (datatools::logger::is_debug(logging)) {
        params.variants.print(std::cerr, "[debug] Variant parameters: ");
      }
      if (params.variants.is_active()) {
        dtc::variant_service vserv;
        vserv.configure(params.variants);

        // Action:
        if (print_doc) {
          app_print_rst(std::cout, vserv.get_repository());
        } else if (print_profile) {
          app_print_current_profile(std::cout, vserv.get_repository());
        }

        if (datatools::logger::is_debug(logging)) {
          debug_app_dump(std::cerr, vserv.get_repository());
        }

        // Start the variant service:
        vserv.start();

        // Nothing special here...

        // Stop the variant service:
        vserv.stop();
      }

    } catch (std::exception& e) {
      std::cerr << "[error] " << e.what()
                << std::endl;
      error_code = EXIT_FAILURE;
      throw std::logic_error(e.what());
    }

  }
  catch (const std::exception & error) {
    DT_LOG_ERROR(datatools::logger::PRIO_ALWAYS, error.what());
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    DT_LOG_ERROR(datatools::logger::PRIO_ALWAYS, "Unexpected Exception !");
    error_code = EXIT_FAILURE;
  }

  bayeux::terminate();
  return error_code;
}

// Definitions:
void app_version(std::ostream & out_)
{
  out_ << "bxvariant_inspector " << datatools::version::get_version() << std::endl;
  return;
}

void app_usage(std::ostream & out_, const bpo::options_description & opts_)
{
  out_ << "bxvariant_inspector - Browse/edit a variant repository" << std::endl;
  out_ << "Usage : " << std::endl;
  out_ << "  bxvariant_inspector [options]..." << std::endl;
  out_ << opts_ << std::endl;
  return;
}

app_config_params::app_config_params()
{
  logging = "";
  action = "";
  return;
}

void debug_app_dump(std::ostream & out_, const dtc::variant_repository & vrep_)
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
  for (std::size_t ivreg = 0;
       ivreg < vreg_keys.size();
       ivreg++) {
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

void app_print_rst(std::ostream & out_, const dtc::variant_repository & vrep_)
{
  vrep_.print_rst(out_, 0);

  // out_ << "Profile" << std::endl;
  // out_ << "=======" << std::endl;
  // out_ << std::endl;

  // out_ << "Current variant profile is: " << std::endl;
  // out_ << std::endl;
  // std::ostringstream profile_oss;
  // dtc::ascii_io rep_io(dtc::ascii_io::IO_DEFAULT);
  // rep_io.store_repository(profile_oss, vrep_);
  // out_ << "  ::" << std::endl;
  // out_ << std::endl;
  // datatools::print_multi_lines(out_, profile_oss.str(), "    ");
  // out_ << std::endl;
  // out_ << ".." << std::endl;
  // out_ << std::endl;

  return;
}
