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
#include <datatools/kernel.h>
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/utils.h>
#include <datatools/version.h>
#include <datatools/configuration/variant_registry_manager.h>
#include <datatools/configuration/variant_registry.h>
#include <datatools/configuration/variant_repository.h>
#include <datatools/configuration/io.h>
#if DATATOOLS_WITH_QT_GUI == 1
// Third party:
// - Qt:
#include <QApplication>
#include <QDialog>
#include <QWidget>
#include <QVBoxLayout>
#include <QStyleFactory>
// - Bayeux/datatools/configuration/ui:
#include <datatools/qt/interface.h>
#include <datatools/configuration/ui/variant_registry_tree_model.h>
#include <datatools/configuration/ui/variant_registry_dialog.h>
#include <datatools/configuration/ui/variant_repository_viewer.h>
#include <datatools/configuration/ui/variant_repository_dialog.h>
#endif // DATATOOLS_WITH_QT_GUI == 1

//! Print application usage (supported options and arguments)
void app_usage(std::ostream & out_,
               const boost::program_options::options_description & desc_);

//! Print application version
void app_version(std::ostream & out_);

//! \brief Application configuration parameters
struct app_config_params {
  //! Default constructor
  app_config_params();
  datatools::logger::priority logging; //!< Logging priority threshold
  std::string variant_config_filename; //!< Name of the configuration file for variant parameters
  bool do_gui;
};

int main(int argc_, char * argv_[])
{
  BAYEUX_INIT_MAIN(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  try {

    // The configurarion parameter sets:
    app_config_params cfg;

    // Program options variables map:
    namespace bpo = boost::program_options;
    bpo::variables_map vm;
    bpo::options_description all_opts;

    try {
      // Describe command line arguments :
      bpo::options_description opts("Allowed options");
      opts.add_options()
        ("help,h", "produce help message")

        ("version,v", "print version")

        ("logging-priority,L",
         bpo::value<std::string>()
         ->value_name("level"),
         "set the logging priority")

#if DATATOOLS_WITH_QT_GUI == 1
        ("gui,G", "activate the GUI")
#endif // DATATOOLS_WITH_QT_GUI == 1

        ("config-variant,c",
         bpo::value<std::string>(&cfg.variant_config_filename)
         ->value_name("file"),
         "set the name of the configuration file for the device manager")

        ; // end of options description

      bpo::positional_options_description args;
      args.add("config-variant", 1);

      // Describe Bayeux/datatools kernel options:
      bpo::options_description kopts("Bayeux/datatools kernel options");
      datatools::kernel::param_type kparams;
      datatools::kernel::build_opt_desc(kopts, kparams);

      // Collect all supported options in one container:
      all_opts.add(opts);
      all_opts.add(kopts);

      // Configure the parser:
      bpo::command_line_parser cl_parser(argc_, argv_);
      cl_parser.options(all_opts);
      cl_parser.positional(args);
      // cl_parser.allow_unregistered();

      // Parse:
      bpo::parsed_options parsed = cl_parser.run();

      // // Collect all other options & args:
      // std::vector<std::string> unrecognized_opts;
      // unrecognized_opts = bpo::collect_unrecognized(parsed.options,
      //                                              bpo::include_positional);

      // Fill and notify a variable map:
      bpo::store(parsed, vm);
      bpo::notify(vm);
    }
    catch (std::exception & po_error) {
      app_usage(std::cerr, all_opts);
      throw;
    }

    // Use command line arguments :

    if (vm.count("help")) {
      app_usage(std::cout, all_opts);
      return(error_code);
    }

    if (vm.count("version")) {
      app_version(std::cout);
      return(error_code);
    }

#if DATATOOLS_WITH_QT_GUI == 1
    if (vm.count("gui")) {
      cfg.do_gui = true;
    }
#endif // DATATOOLS_WITH_QT_GUI == 1

    // Fetch the verbosity level:
    if (vm.count("logging-priority")) {
      const std::string & logging_label = vm["logging-priority"].as<std::string>();
      cfg.logging = datatools::logger::get_priority(logging_label);
      DT_THROW_IF(cfg.logging == datatools::logger::PRIO_UNDEFINED, std::logic_error,
                  "Invalid logging priority label '" << logging_label << "' !");
    }

    // Checks:
    DT_THROW_IF(cfg.variant_config_filename.empty(), std::logic_error, "Missing variant manager configuration file!");
    DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS,
                  "Variant manager configuration file : '" << cfg.variant_config_filename << "'.");
    datatools::fetch_path_with_env(cfg.variant_config_filename);
    DT_THROW_IF(! boost::filesystem::exists(cfg.variant_config_filename),
                std::runtime_error,
                "Variant manager configuration file '" << cfg.variant_config_filename << "' does not exist!");

    // datatools::configuration::variant_registry_manager vrmgr;
    datatools::configuration::variant_repository vrep;
    std::string vrep_config_filename = cfg.variant_config_filename;
    datatools::fetch_path_with_env(vrep_config_filename);
    datatools::properties vrep_config;
    datatools::properties::read_config(vrep_config_filename, vrep_config);
    vrep.initialize(vrep_config);
    vrep.tree_dump(std::clog, "Variant repository: ");

#if DATATOOLS_WITH_QT_GUI == 1
    if (cfg.do_gui) {
      // Launch a Qt based dialog for the variant repository:
      const datatools::kernel & krnl = datatools::kernel::const_instance();
      datatools::qt::interface & iqt = datatools::qt::interface::instance(krnl.get_argc(),
                                                                          krnl.get_argv(),
                                                                          krnl.get_application_name().c_str());
      if (iqt.is_external_app()) {
        DT_LOG_NOTICE(datatools::logger::PRIO_NOTICE, "QT uses external application.");
      }

      datatools::configuration::ui::variant_repository_dialog vrep_dialog(vrep);
      int ret = vrep_dialog.exec();
      if (ret == QDialog::Rejected) {
        DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "Variant repository dialog was rejected!");
      }

      /*
        int argc = 1;
      const char * argv[] = { "Bayeux - Configuration variant ins" };
      QApplication::setStyle(QStyleFactory::create("plastique"));
      QApplication app(argc, (char**) argv);

      {
        datatools::configuration::ui::variant_registry_tree_model vrtm;
        vrtm.construct(dkvr.grab_registry("geometry"), "Geometry setup");
        vrtm.tree_dump(std::clog, "Settings tree model:");
        {
          datatools::configuration::ui::variant_registry_dialog window(vrtm);
          window.show();
          int ret = app.exec();
        }
        dkvr.get_registry("geometry").tree_dump(std::clog, "Settings:");
      }

      {
        QWidget * window = new QWidget;
        QVBoxLayout * layout = new QVBoxLayout;
        datatools::configuration::ui::variant_repository_viewer * vr_viewer
          = new datatools::configuration::ui::variant_repository_viewer;
        vr_viewer->set_repository(dkvr);
        layout->addWidget(vr_viewer);
        window->setLayout(layout);
        window->show();
        int ret = app.exec();
      }

      {
        datatools::configuration::ui::variant_repository_dialog window(dkvr);
        window.show();
        int ret = app.exec();
      }
      */
    }
#endif // DATATOOLS_WITH_QT_GUI == 1
  }
  catch (const std::exception & error) {
    DT_LOG_ERROR(datatools::logger::PRIO_ALWAYS, error.what());
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    DT_LOG_ERROR(datatools::logger::PRIO_ALWAYS, "Unexpected Exception !");
    error_code = EXIT_FAILURE;
  }

  BAYEUX_FINI();
  return error_code;
}

// Definitions:
void app_version(std::ostream & out_)
{
  out_ << "bxvariant_inspector " << datatools::version::get_version() << std::endl;
  return;
}

void app_usage(std::ostream & out_,
               const boost::program_options::options_description & opts_)
{
  out_ << "Usage : " << std::endl;
  out_ << opts_ << std::endl;
  return;
}

app_config_params::app_config_params()
{
  logging = datatools::logger::PRIO_FATAL;
  do_gui = false;
  return;
}
