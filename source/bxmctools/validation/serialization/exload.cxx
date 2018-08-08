// exload.cxx
/* Copyright (C) 2018 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
// - Bayeux:
#include <bayeux/bayeux.h>
// - Bayeux/datatools:
#include <bayeux/datatools/io_factory.h>
// - Bayeux/geomtools:
#include <bayeux/geomtools/geom_id.h>
#include <bayeux/geomtools/material.h>
#include <bayeux/geomtools/sensitive.h>
// - Bayeux/mctools:
#include <bayeux/mctools/base_step_hit.h>
#include <bayeux/mctools/simulated_data.h>
#include <bayeux/mctools/utils.h>

int main(int argc_, char **argv_)
{
  bayeux::initialize(argc_, argv_, 0);

  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  int error_code = EXIT_SUCCESS;
  try {

    std::string infile;
    std::string outfile;

    // Parse options:
    namespace po = boost::program_options;
    po::options_description opts("Allowed options");
    opts.add_options()
      ("input-file,i",
       po::value<std::string>(&infile),
       "set the input filename")
      ("output-file,o",
       po::value<std::string>(&outfile),
       "set the output filename")
      ; // end of options description

    // Describe command line arguments :
    po::variables_map vm;
    po::store(po::command_line_parser(argc_, argv_)
              .options(opts)
              .run(), vm);
    po::notify(vm);
    
    int hit_lib_version = boost::serialization::version<mctools::base_step_hit>::value;
    int sd_lib_version = boost::serialization::version<mctools::simulated_data>::value;
    std::clog << "[info] Base step hit class library version  = " << hit_lib_version << std::endl;
    std::clog << "[info] Simulated data class library version = " << sd_lib_version << std::endl;

    if (infile.empty()) {
      infile = "out-" + std::to_string(hit_lib_version) + ".xml";
      std::clog << "[info] infile = '" << infile << "'" << std::endl;
    }
 
    datatools::data_reader reader(infile, datatools::using_multi_archives);
    
    if (outfile.empty()) {
      outfile = "check-out-" + std::to_string(hit_lib_version) + ".xml";
      std::clog << "[info] outfile = '" << outfile << "'" << std::endl;      
    }
    datatools::data_writer writer(outfile, datatools::using_multi_archives);

    // Step loops:
    for (int i = 0; i < 2; i++) {
      // MC hit:
      mctools::base_step_hit mchit;
      reader.load("MChit", mchit);
      mchit.tree_dump(std::cout, "Loaded mock base step hit:");
      writer.store("MChit", mchit);
    }
    
  } catch (std::exception& x) {
    DT_LOG_FATAL(logging, x.what());
    error_code = EXIT_FAILURE;
  } catch (...) {
    DT_LOG_FATAL(logging, "Unexpected error !");
    error_code = EXIT_FAILURE;
  }

  bayeux::terminate();
  return error_code;
}
