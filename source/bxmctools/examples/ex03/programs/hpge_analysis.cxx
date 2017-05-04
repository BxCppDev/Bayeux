// hpge_analysis.cxx
//! \author Arnaud Chapon, François Mauger
//! \brief  Process simulated data from a HPGe detector
//
// Copyright (c) 2015 by Arnaud Chapon <achapon@cerap.fr>
//                       and François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of Bayeux/mctools.
//
// Bayeux/mctools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux/mctools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux/mctools. If not, see <http://www.gnu.org/licenses/>.

// Standard library:
#include <cstdlib>
#include <iostream>
#include <exception>
#include <string>

// Third party:
// - Boost:
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
// - Bayeux:
#include <bayeux/bayeux.h>
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/kernel.h>
// - Bayeux/mctools:
#include <mctools/simulated_data.h>
#include <mctools/simulated_data_reader.h>
// - Bayeux/mygsl:
#include <mygsl/histogram.h>
// - Bayeux/geomtools:
#include <geomtools/geomtools_config.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

// This project:
#include <hpge/calorimeter_hit.h>
#include <hpge/calorimetry.h>

//! Print application usage (supported options and arguments)
void app_usage(std::ostream & out_,
               const boost::program_options::options_description & desc_);

//! Print application version
void app_version(std::ostream & out_);

//! \brief Application configuration parameters
struct app_config_params {
  //! Default constructor
  app_config_params();

  datatools::logger::priority logging; //!< Logging priority threshold (default value: FATAL)
  std::string input_filename;          //!< Name of the input simulation data file (default value: empty)
  std::size_t input_nevents;           //!< Force number of input events (default: 0 for unused)
  double energy_resolution;            //!< Energy resolution of the HPGe detector (arbitrary unit, default value: 2.0e-3)
  double energy_resolution_pedestal;   //!< Energy resolution pedestal of the HPGe detector (arbitrary unit, default value: 0.0)
  double energy_threshold;             //!< Low energy threshold (default value: 10.0 keV)
  std::string hit_category;            //!< Hit category (default value: "ge")
  bool only_gamma;                     //!< Flag to process only gamma rays (default value: false)
  int prng_seed;                       //!< PRNG seed (default value: invalid seed)
  double histo_energy_min;             //!< Minimum histogram energy (default value:    0.0 keV)
  double histo_energy_max;             //!< Maximum histogram energy (default value: 3000.0 keV)
  std::size_t histo_energy_nbins;      //!< Number of histogram energy bins (default value: 3000)
  std::string histo_output_filename;   //!< Name of the output histogram file (default value: empty)
  bool histo_draw;                     //!< Flag to draw the energy histogram (default value: false)
};

//! Main program:
int main(int argc_, char * argv_[])
{
  bayeux::initialize(argc_, argv_, 0);
  int error_code = EXIT_SUCCESS;
  try {

    // The application configuration parameter set:
    app_config_params cfg;

    // Default energy unit:
    double defaut_energy_unit = CLHEP::keV;

    // Describe command line arguments :
    namespace po = boost::program_options;

    // Variable map:
    po::variables_map vm;
    po::options_description all_opts;

    try {

      // Describe the allowed options for this application:
      po::options_description opts("Allowed options");
      opts.add_options()
        ("help,h", "produce help message")

        ("version,v", "print version")

        ("logging-priority,L",
         po::value<std::string>(),
         "set the logging priority")

        ("input-file,i",
         po::value<std::string>(&cfg.input_filename),
         "set the name of the input simulated data file from which to load the Monte-Carlo data")

        ("input-nevents,e",
         po::value<std::size_t>(&cfg.input_nevents),
         "set the true number of input simulated events even if the input data file contains less events due to storage optimization")

        ("hit-category,H",
         po::value<std::string>(&cfg.hit_category),
         "set the name of the hit category to be analyzed")

        ("energy-threshold,T",
         po::value<double>(),
         "set the energy threshold of the HPGe detector")

        ("energy-resolution,R",
         po::value<double>(&cfg.energy_resolution)
         ->default_value(2.0e-3),
         "set the energy resolution of the HPGe detector")

        ("energy-resolution-pedestal,P",
         po::value<double>(&cfg.energy_resolution_pedestal)
         ->default_value(0.0),
         "set the energy resolution pedestal of the HPGe detector")

        ("prng-seed,s",
         po::value<int>(&cfg.prng_seed),
         "set the PRNG seed")

        ("histo-energy-min,m",
         po::value<double>(),
         "set the histogram minimum energy")

        ("histo-energy-max,M",
         po::value<double>(),
         "set the histogram maximum energy")

        ("histo-energy-nbins,b",
         po::value<std::size_t>(&cfg.histo_energy_nbins),
         "set the histogram number of bins")

        ("histo-output-file,o",
         po::value<std::string>(&cfg.histo_output_filename),
         "set the name of the output histogram file")

        ("histo-draw,d",
         po::value<bool>(&cfg.histo_draw)
         ->zero_tokens()
         ->default_value(false),
         "set the flag to draw the histogram")

        ; // end of options description

      // Describe positional arguments for this application:
      po::positional_options_description args;
      args.add("input-file", 1);
      args.add("output-file", 1);

      // Describe Bayeux/datatools kernel options:
      po::options_description kopts("Bayeux/datatools kernel options");
      datatools::kernel::param_type kparams;
      datatools::kernel::build_opt_desc(kopts, kparams);

      // Collect all supported options in one container:
      all_opts.add(opts);
      all_opts.add(kopts);

      // Configure the parser:
      po::command_line_parser cl_parser(argc_, argv_);
      cl_parser.options(all_opts);
      cl_parser.positional(args);
      // cl_parser.allow_unregistered();

      // Parse:
      po::parsed_options parsed = cl_parser.run();

      // // Collect all other options & args:
      // std::vector<std::string> unrecognized_opts;
      // unrecognized_opts = po::collect_unrecognized(parsed.options,
      //                                              po::include_positional);

      // Fill and notify a variable map:
      po::store(parsed, vm);
      po::notify(vm);
    }
    catch (std::exception & po_error) {
      app_usage(std::cerr, all_opts);
      throw;
    }

    // Use command line arguments :

    if (vm.count("help")) {
      // Print help message then quit:
      app_usage(std::cout, all_opts);
      return(error_code);
    }

    if (vm.count("version")) {
      // Print version then quit:
      app_version(std::cout);
      return(error_code);
    }

    // Fetch the verbosity level:
    if (vm.count("logging-priority")) {
      const std::string & logging_label = vm["logging-priority"].as<std::string>();
      cfg.logging = datatools::logger::get_priority(logging_label);
      DT_THROW_IF(cfg.logging == datatools::logger::PRIO_UNDEFINED, std::logic_error,
                  "Invalid logging priority label '" << logging_label << "' !");
    }

    // Apply energy unit to some command line options:
    if (vm.count("energy-threshold")) {
      cfg.energy_threshold = vm["energy-threshold"].as<double>() * defaut_energy_unit;
    }

    if (vm.count("histo_energy-min")) {
      cfg.histo_energy_min = vm["histo-energy-min"].as<double>() * defaut_energy_unit;
    }

    if (vm.count("histo_energy-max")) {
      cfg.histo_energy_max = vm["histo-energy-max"].as<double>() * defaut_energy_unit;
    }

    if (vm.count("input-nevents")) {
      cfg.input_nevents = vm["input-nevents"].as<std::size_t>();
    }

    // Check:
    DT_THROW_IF (cfg.input_filename.empty(), std::logic_error,
                 "Missing simulated data input file !");

    // The input simulated data file (from Bayeux/mctools Geant4 simulation):
    DT_THROW_IF (! boost::filesystem::exists(cfg.input_filename),
                 std::runtime_error,
                 "File '" << cfg.input_filename << "' does not exists !");

    // Check the validity of the PRNG seed:
    DT_THROW_IF (cfg.prng_seed == mygsl::random_utils::SEED_INVALID, std::logic_error,
                 "Seed for pseudo random number generator (PRNG) is not explicitely set !");

    // The simulated data reader :
    mctools::simulated_data_reader sd_reader;
    sd_reader.initialize(cfg.input_filename);

    // The calo simulation detection object to be loaded :
    hpge::calorimetry calo;
    calo.set_logging(cfg.logging);
    calo.set_energy_threshold(cfg.energy_threshold);
    calo.set_energy_resolution(cfg.energy_resolution);
    calo.set_energy_resolution_pedestal(cfg.energy_resolution_pedestal);
    calo.set_hit_category(cfg.hit_category);
    calo.set_only_gamma(cfg.only_gamma);
    calo.set_prng_seed(cfg.prng_seed);
    calo.initialize();

    // The energy histogram:
    mygsl::histogram spectroscopy(cfg.histo_energy_nbins, cfg.histo_energy_min, cfg.histo_energy_max);

    // The event loop:
    int hit_counter = 0;
    int calo_hit_counter = 0;
    while (sd_reader.has_next()) {
      // The source Monte-Carlo data object to be loaded :
      mctools::simulated_data SD;

      // Load the simulated data object:
      sd_reader.load_next(SD);

      // The target calorimeter hit to be produced from Monte-Carlo data:
      hpge::calorimeter_hit CH;

      // Process the source Monte-Carlo data and build the HPGe calorimetry hit:
      calo.process_event(SD, CH);

      // Statistics (counters/histograms):
      hit_counter++;
      if (CH.is_valid()) {
        // Increment counter:
        calo_hit_counter++;
        // Histogramming:
        spectroscopy.fill(CH.get_energy_deposit());
      }
    }
    size_t total_event_counter = hit_counter;
    if (cfg.input_nevents != 0) {
      if (hit_counter > cfg.input_nevents) {
        DT_LOG_ERROR(datatools::logger::PRIO_ALWAYS, "Input number of events lower than number of stored events");
      }
      total_event_counter = cfg.input_nevents;
    }
    DT_LOG_NOTICE(cfg.logging, "Number of Monte-Carlo events  : " << total_event_counter);
    DT_LOG_NOTICE(cfg.logging, "Number of HPGe effective hits : " << calo_hit_counter);
    double efficiency = (1.0 * calo_hit_counter)/ total_event_counter;
    DT_LOG_NOTICE(cfg.logging, "Detection efficiency          : " << efficiency * 100 << " %");

    if (! cfg.histo_output_filename.empty()) {
      datatools::fetch_path_with_env(cfg.histo_output_filename);
      std::ofstream histo_file(cfg.histo_output_filename.c_str());
      spectroscopy.print(histo_file);
      DT_LOG_NOTICE(cfg.logging, "Histogram ASCII output file   : '"
                    << cfg.histo_output_filename << "'");
    }

    if (cfg.histo_draw) {
      if (boost::filesystem::exists(cfg.histo_output_filename)) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
        // Online display:
        Gnuplot g1;
        g1.cmd("set title 'HPGe spectroscopy' ");
        // g1.cmd("set terminal wxt");
        g1.cmd("set grid");
        std::ostringstream set_xrange_oss;
        set_xrange_oss << "set xrange" << ' '
                       << '[' << cfg.histo_energy_min / CLHEP::keV
                       << ":" << cfg.histo_energy_max / CLHEP::keV
                       << ']';
        g1.cmd(set_xrange_oss.str());
        g1.cmd("set yrange [0:]");
        double bin_delta_energy =
          (cfg.histo_energy_max - cfg.histo_energy_min) / cfg.histo_energy_nbins;
        g1.set_xlabel("E (keV)");
        std::ostringstream set_ylabel_oss;
        set_ylabel_oss << "dN/dE (counts/" << (bin_delta_energy / CLHEP::keV) << " keV)";
        g1.set_ylabel(set_ylabel_oss.str());
        {
          std::ostringstream plot_cmd;
          plot_cmd << "plot '" << cfg.histo_output_filename << "'"
                   << " using "
                   << "(0.5*(column(1)+column(2))/" << CLHEP::keV << ')'
                   << ":(column(3)) "
                   << " notitle with histeps ";
          g1.cmd(plot_cmd.str());
          g1.showonscreen(); // window output
          geomtools::gnuplot_drawer::wait_for_key();
          usleep(200);
        }
      }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    }

  }
  catch (std::exception & error) {
    DT_LOG_FATAL(datatools::logger::PRIO_ALWAYS, error.what());
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    DT_LOG_FATAL(datatools::logger::PRIO_ALWAYS, "Unexpected error!");
    error_code = EXIT_FAILURE;
  }
  bayeux::terminate();
  return error_code;
}

// Definitions of functions:

void app_version(std::ostream & out_)
{
  out_ << "hpge_analysis 1.0" << std::endl;
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
  input_nevents = 0;
  energy_resolution = 2.0e-3; //  typical value: 2.0e-3
  energy_resolution_pedestal = 0.0; // typical value: 0.50
  energy_threshold  = 10.0 * CLHEP::keV;
  hit_category      = "ge";
  only_gamma        = false;
  prng_seed         = mygsl::random_utils::SEED_INVALID;
  histo_energy_min   =    0.0 * CLHEP::keV;
  histo_energy_max   = 3000.0 * CLHEP::keV;
  histo_energy_nbins = 3000;
  histo_draw = false;
  return;
}
