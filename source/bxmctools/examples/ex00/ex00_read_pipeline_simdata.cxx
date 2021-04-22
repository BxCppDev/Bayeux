// ex00_read_pipeline_simdata.cxx
/* Copyright (C) 2013-2015 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

// Standard library:
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

// Third party:
// - Boost:
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/foreach.hpp>
#include <boost/scoped_ptr.hpp>
// - Bayeux:
#include <bayeux/bayeux.h>
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/library_loader.h>
#include <datatools/things.h>
// - Bayeux/dpp:
#include <dpp/input_module.h>
// - Bayeux/geomtools:
#include <geomtools/manager.h>
// - Bayeux/mctools:
#include <mctools/simulated_data.h>

// This project:
#include <ex00_inspector.h>

int main(int argc_, char **argv_)
{
  bayeux::initialize(argc_, argv_, 0);

  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  int error_code = EXIT_SUCCESS;
  try {

    std::string pipeline_simulated_data_filename;
    std::string geometry_config_filename;
    bool interactive = false;
    bool visualization = false;
    bool dump_data_record = false;
    bool dump_simulated_data = false;
    bool dump_hits = false;
    std::vector<std::string> LL_dlls;
    std::string              LL_config;
    std::string              SD_bank_label = "SD";

    namespace po = boost::program_options;
    po::options_description opts("Allowed options");
    opts.add_options()
      ("help,h", "produce help message")

      ("logging-priority,g",
       po::value<std::string>()
       ->default_value("warning"),
       "set the logging priority threshold")

      ("load-dll,l",
       po::value<std::vector<std::string> > (),
       "load a dynamic library"
       )

      ("dll-config",
       po::value<std::vector<std::string> > (),
       "load a configuration file for dynamic library loading"
       )

      ("input-file,i",
       po::value<std::string>(&pipeline_simulated_data_filename),
       "set the input pipeline simulated data file from which to load the Monte-Carlo data")

      ("geometry-config,g",
       po::value<std::string>(&geometry_config_filename),
       "set the main configuration file of the geometry manager")

      ("interactive,I",
       po::value<bool>(&interactive)
       ->zero_tokens()
       ->default_value(false),
       "run in interactive mode")

      ("with-visualization,V",
       po::value<bool>(&visualization)
       ->zero_tokens()
       ->default_value(false),
       "activate visualization")

      ("dump-data-record,D",
       po::value<bool>(&dump_data_record)
       ->zero_tokens()
       ->default_value(false),
       "dump each data record")

      ("dump-simulated-data,S",
       po::value<bool>(&dump_simulated_data)
       ->zero_tokens()
       ->default_value(false),
       "dump each simulated data record")

      ("dump-hits,H",
       po::value<bool>(&dump_hits)
       ->zero_tokens()
       ->default_value(false),
       "dump each truth step hit")

      ; // end of options description

    // Describe command line arguments :
    po::positional_options_description args;
    args.add("input-file", 1);
    po::variables_map vm;
    po::store(po::command_line_parser(argc_, argv_)
      .options(opts)
              .positional(args)
              .run(), vm);
    po::notify(vm);
    // Use command line arguments :
    if (vm.count("help")) {
      std::cout << "Usage : " << std::endl;
      std::cout << opts << std::endl;
      return(1);
    }
    if (vm.count("logging-priority")) {
      std::string logging_label = vm["logging-priority"].as<std::string>();
      logging = datatools::logger::get_priority(logging_label);
      DT_THROW_IF(logging == datatools::logger::PRIO_UNDEFINED, std::logic_error,
                  "Invalid logging priority label '" << logging_label << "' !");
    }

    if (vm.count ("load-dll")) {
      LL_dlls = vm["load-dll"].as<std::vector<std::string> > ();
    }

    if (vm.count ("dll-config")) {
      LL_config = vm["dll-config"].as<std::string> ();
    }


    datatools::library_loader LL(LL_config);
    BOOST_FOREACH (const std::string & dll_name, LL_dlls) {
      DT_LOG_NOTICE(logging,"Loading DLL '" << dll_name << "'...");
      DT_THROW_IF (LL.load(dll_name) != EXIT_SUCCESS,
                   std::runtime_error,
                   "Loading DLL '" << dll_name << "' failed !");
    }

    DT_THROW_IF (pipeline_simulated_data_filename.empty(), std::logic_error,
                 "Missing pipeline simulated data input file !");

    // The input pipeline simulated data file :
    DT_THROW_IF (! boost::filesystem::exists(pipeline_simulated_data_filename),
                 std::runtime_error,
                 "File '" << pipeline_simulated_data_filename << "' does not exists !");

    // The pipeline simulated data reader :
    dpp::input_module reader;

    // Configuration:
    datatools::properties reader_config;
    reader_config.store ("logging.priority", "debug");
    reader_config.store ("max_files", 100);
    reader_config.store ("max_record_total", 10000);
    reader_config.store ("max_record_per_file", 10000);
    reader_config.store ("files.mode", "single");
    reader_config.store ("files.single.filename", pipeline_simulated_data_filename);
    reader.initialize_standalone (reader_config);
    reader.tree_dump (std::clog, "Simulated data reader module");

    // Geometry manager :
    DT_LOG_DEBUG(logging, "Initializing the geometry manager...");
    if (geometry_config_filename.empty()) {
      geometry_config_filename = "config/geometry/manager.conf";
    }
    geomtools::manager geo_mgr;
    datatools::fetch_path_with_env(geometry_config_filename);
    datatools::properties geo_mgr_config;
    datatools::read_config(geometry_config_filename, geo_mgr_config);
    geo_mgr.initialize(geo_mgr_config);

    // The simulated data inspector :
    mctools::ex00::simulated_data_inspector SDI;
    SDI.set_interactive(interactive);
    SDI.set_with_visualization(visualization);
    SDI.set_dump_simulated_data(dump_simulated_data);
    SDI.set_dump_hits(dump_hits);
    SDI.set_geometry_manager(geo_mgr);

    // The event record with embedded simulated data bank to be loaded :
    datatools::things ER;

    // The simulated data loading loop :
    int edr_count = 0;
    while (! reader.is_terminated()) {
      // Load the event data record :
      reader.process(ER);
      DT_LOG_NOTICE(logging, "Event data record #" << edr_count);
      if (dump_data_record) {
        ER.tree_dump(std::clog, "Event data record : ", " ");
        std::clog << std::endl;
      }
      edr_count++;
      if (ER.has(SD_bank_label) && ER.is_a<mctools::simulated_data>(SD_bank_label)) {
        // Access to the "SD" bank with a stored `mctools::simulated_data' :
        const mctools::simulated_data & SD = ER.grab<mctools::simulated_data>(SD_bank_label);
        bool goon = SDI.inspect(SD);
        if (! goon) {
          break;
        }
      }
      ER.clear();
    }
    std::clog << "Number of processed event data records: " << edr_count << std::endl;

  } catch(std::exception& x) {
    DT_LOG_FATAL(logging, x.what());
    error_code = EXIT_FAILURE;
  } catch(...) {
    DT_LOG_FATAL(logging, "Unexpected error !");
    error_code = EXIT_FAILURE;
  }
  bayeux::terminate();
  return error_code;
}
