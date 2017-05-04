/// \file g4_seeds.cxx
/* Description :
 *
 *   Generate seeds for the Bayeux/mctools Geant4 simulation program.
 *
 * Licence :
 *
 * Copyright (C) 2016 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 */

// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <exception>
#include <random>
#include <map>
#include <memory>

// Third party:
// - Boost:
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>
// - Bayeux:
#include <bayeux/bayeux.h>
// - Bayeux/datatools:
#include <datatools/datatools_config.h>
#include <datatools/logger.h>
#include <datatools/exception.h>
#include <datatools/utils.h>
// - Bayeux/mygsl:
#include <mygsl/seed_manager.h>
// - Bayeux/mctools:
#include <mctools/g4/manager.h>

/// \brief Seed generator for the G4 simulation program
class g4_seed_generator {
public:
  /// \brief Parameters for the application
  struct parameters
  {
    /// Default constructor
    parameters();

    /// Set default values
    void set_defaults();

    /// Describe command line options
    static void build_opts(boost::program_options::options_description & opts_,
                           parameters & params_);

    static void print_usage(const boost::program_options::options_description & opts_,
                            std::ostream & out_ = std::clog);

    // Attributes:
    uint32_t    number_of_runs;
    uint32_t    run_start;
    std::string base_dir;
    std::string pattern;
    std::string run_list_file;
    std::string list_mount_point;
    bool        no_safe_checks = false;
    bool        no_run_list_file = false;
  };

public:
  /// Default constructor
  g4_seed_generator(const parameters& p);

  /// Destructors
  ~g4_seed_generator();

  /// Run
  void run();

private:
  parameters _params_; ///< Parameters
};

int main(int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;

  try {
    // Configuration parameters for the G4 manager:
    g4_seed_generator::parameters params;
    params.set_defaults();

    // Shortcut for Boost/program_options namespace :
    namespace po = boost::program_options;

    // Variables map:
    po::variables_map vm;
    po::options_description all_opts;

    try {

      // Describe command line switches :
      po::options_description opts("Allowed options");
      g4_seed_generator::parameters::build_opts(opts, params);

      // Collect all supported options in one container:
      all_opts.add(opts);

      // Configure the parser:
      po::command_line_parser cl_parser(argc_, argv_);
      cl_parser.options(all_opts);

      // Parse:
      po::parsed_options parsed = cl_parser.run();

      // Fill and notify a variable map:
      po::store(parsed, vm);
      po::notify(vm);
    } catch (std::exception & po_error) {
      g4_seed_generator::parameters::print_usage(all_opts, std::cerr);
      throw;
    }

    if (vm.count("help")) {
      if (vm["help"].as<bool>()) {
        g4_seed_generator::parameters::print_usage(all_opts, std::cout);
        return(0);
      }
    }

    g4_seed_generator app(params);
    app.run();

  } catch (std::exception & x) {
    std::cerr << "[error] Seed generation failed with exception:\n" << x.what() << "\n";
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "[error] Seed generation failed with unexpected exception." << "\n";
    error_code = EXIT_FAILURE;
  }

  return error_code;
}

g4_seed_generator::parameters::parameters()
{
  set_defaults();
}

void g4_seed_generator::parameters::set_defaults()
{
  number_of_runs   = 1;
  run_start        = 0;
  base_dir         = "__bxg4_seeds-set.d";
  pattern          = "seeds_%n.conf";
  run_list_file    = "__bxg4_seeds-runs.lis";
  list_mount_point = "";
  no_safe_checks   = false;
  no_run_list_file = false;
}

void g4_seed_generator::parameters::print_usage(const boost::program_options::options_description & opts_,
                                                std::ostream & out_)
{
  const std::string APP_NAME = "bxg4_seeds";
  out_ << APP_NAME << " -- generator of seeds for the Bayeux/mctools GEANT4 simulation wrapper" << std::endl;
  out_ << std::endl;
  out_ << "Usage : " << std::endl;
  out_ << std::endl;
  out_ << "  " << APP_NAME << " [OPTIONS] [ARGUMENTS] "
       << std::endl;
  out_ << std::endl;
  out_ << opts_ << std::endl;
  out_ << std::endl;
}


void g4_seed_generator::parameters::build_opts(boost::program_options::options_description & opts_,
                                               g4_seed_generator::parameters & params_)
{
  namespace po = boost::program_options;
  opts_.add_options()

    ("help,h",
     po::value<bool>()
     ->zero_tokens()
     ->default_value(false),
     "Produce help message")

    ("number-of-runs,n",
     po::value<uint32_t>(&params_.number_of_runs)
     ->default_value(1)
     ->value_name("integer"),
     "Set the number of runs for which seeds are generated")

    // ("run-start,s",
    //  po::value<uint32_t>(&params_.run_start)
    //  ->default_value(0)
    //  ->value_name("integer"),
    //  "Set the number of the first run")

    ("base-dir,d",
     po::value<std::string>(&params_.base_dir)
     ->default_value("__bxg4_seeds-set.d")
     ->value_name("dirname"),
     "Set the base directory where seed file is generated")

    ("pattern,p",
     po::value<std::string>(&params_.pattern)
     ->default_value("seeds_%n.conf")
     ->value_name("pattern"),
     "Set the pattern of the generated incremental seed file")

    ("run-list,t",
     po::value<std::string>(&params_.run_list_file)
     ->default_value("__bxg4_seeds-runs.lis")
     ->value_name("name"),
     "Set the name of the run list file")

    ("no-safe-checks,k",
     po::value<bool>(&params_.no_safe_checks)
     ->zero_tokens()
     ->default_value(false),
     "Do not perform safe checks (allow overwriting of existing output directory and files...)")

    ("no-run-list-file,T",
     po::value<bool>(&params_.no_run_list_file)
     ->zero_tokens()
     ->default_value(false),
     "Do not generate the run list file")

    ;
}

g4_seed_generator::g4_seed_generator(const parameters & params_)
{
  {
    bool exists = params_.base_dir.find("%n") != std::string::npos;
    DT_THROW_IF(exists, std::logic_error,
                "Base directory '" << params_.base_dir << "' has a forbidden '%n' directive!");
  }
  {
    DT_THROW_IF(params_.number_of_runs == 0, std::logic_error,
                "Invalid null number of runs!");
  }
  {
    bool exists = params_.pattern.find("/") != std::string::npos;
    DT_THROW_IF(exists, std::logic_error,
                "Forbidden '/' separator in the output file pattern '" << params_.pattern << "'!");
  }
  if (params_.number_of_runs > 1) {
    bool exists = params_.pattern.find("%n") != std::string::npos;
    DT_THROW_IF(!exists, std::logic_error,
                "Missing '%n' directive in the output file pattern '" << params_.pattern << "'!");
  }
  if (!params_.list_mount_point.empty()) {
    const char open = params_.list_mount_point[0];
    if (open == '@') {
      std::string name = params_.list_mount_point.substr(1);
      DT_THROW_IF(!datatools::name_validation(name, datatools::NV_INSTANCE),
                  std::logic_error,
                  "Invalid mounting point '" << name << "' for seed files!");
    } else if (open == '$') {
      std::string envpath = params_.list_mount_point.substr(1);
      if (envpath.length() > 2) {
        if (envpath[0] == '{' && envpath.back() == '}') {
          envpath = envpath.substr(1, envpath.length() - 2);
        }
      }
      DT_THROW_IF(!datatools::name_validation(envpath, datatools::NV_INSTANCE),
                  std::logic_error,
                  "Invalid environment path '" << envpath << "' for seed files!");
    } else {
      DT_THROW(std::logic_error,
               "Invalid list mount point '" << params_.list_mount_point << "' for seed files!");
    }
  }
  _params_ = params_;
}

g4_seed_generator::~g4_seed_generator()
{
}

void g4_seed_generator::run()
{
  // GENERATE
  std::map<uint32_t, std::string> seeds_per_run;
  std::set<std::string> check_signatures;
  std::size_t max_count = 3;
  std::size_t last_run = _params_.run_start + _params_.number_of_runs;
  for (std::size_t irun {_params_.run_start};
       irun != last_run;
       irun++) {
    std::size_t count = 0;
    while (true) {
      int mgr_seed  = mygsl::random_utils::SEED_AUTO;
      int vg_seed   = mygsl::random_utils::SEED_AUTO;
      int eg_seed   = mygsl::random_utils::SEED_AUTO;
      int shpf_seed = mygsl::random_utils::SEED_AUTO;
      mygsl::seed_manager smgr;
      smgr.set_init_seed_flags(mygsl::seed_manager::INIT_SEED_FROM_RANDOM_DEVICE);
      smgr.add_seed(mctools::g4::manager::g4_manager_label(),
                    mgr_seed);
      smgr.add_seed(mctools::g4::manager::vertex_generator_label(),
                    vg_seed);
      smgr.add_seed(mctools::g4::manager::event_generator_label(),
                    eg_seed);
      smgr.add_seed(mctools::g4::manager::shpf_label(),
                    shpf_seed);
      smgr.ensure_different_seeds();
      std::ostringstream seeds_repr;
      seeds_repr << smgr;
      std::string signature = seeds_repr.str();
      if (check_signatures.count(signature) == 0) {
        seeds_per_run[irun] = signature;
        check_signatures.insert(signature);
        break;
      } else {
        count++;
      }
      if (count > max_count) {
        DT_THROW(std::runtime_error, "Could not generate an independant seed set after "
                 << count << " tries!");
      }
    }
  }

  // WRITE
  // Require unique directory
  if (boost::filesystem::exists(_params_.base_dir)) {
    DT_THROW_IF(! _params_.no_safe_checks,
                std::runtime_error,
                "Requested output directory '" << _params_.base_dir << "' already exists");
  } else {
    DT_THROW_IF(!boost::filesystem::create_directories(_params_.base_dir),
                std::runtime_error,
                "Could not create directories '" << _params_.base_dir << "'");
  }

  std::map<uint32_t, std::string> seedfiles_per_run;
  std::ostringstream list_out;
  for (const auto& iseed : seeds_per_run) {
    uint32_t irun = iseed.first;
    std::string seed_path_pattern = _params_.pattern;
    std::string path_pattern = _params_.base_dir + "/" + _params_.pattern;
    boost::replace_all(path_pattern, "//", "/");
    boost::replace_all(path_pattern, "%n", boost::lexical_cast<std::string>(irun));
    boost::replace_all(seed_path_pattern, "%n", boost::lexical_cast<std::string>(irun));
    {
      std::string test_path = path_pattern;
      datatools::fetch_path_with_env(test_path);
      if (boost::filesystem::exists(test_path)) {
        DT_THROW_IF(! _params_.no_safe_checks,
                    std::logic_error,
                    "File '" << path_pattern << "' already exists");
      }
    }
    std::ofstream fout(path_pattern.c_str());
    DT_THROW_IF(!fout, std::runtime_error, "Cannot open file '" << path_pattern << "'");
    fout << iseed.second << std::endl;
    seedfiles_per_run[irun] = seed_path_pattern; //path_pattern;
  }

  std::unique_ptr<std::ofstream> frunlist;
  std::string run_list_file_path;
  if (!_params_.no_run_list_file && !_params_.run_list_file.empty()) {
    bool full_list_path = false;
    {
      std::string test_lis = _params_.run_list_file;
      datatools::fetch_path_with_env(test_lis);
      if (test_lis.size() > 0) {
        if (test_lis[0] == '/') full_list_path = true;
      }
    }
    if (! full_list_path) {
      run_list_file_path = _params_.base_dir + "/" + _params_.run_list_file;
    } else {
      run_list_file_path = _params_.run_list_file;
    }
    boost::replace_all(run_list_file_path, "//", "/");
    datatools::fetch_path_with_env(run_list_file_path);
    if (boost::filesystem::exists(run_list_file_path)) {
      DT_THROW_IF(! _params_.no_safe_checks,
                  std::runtime_error,
                  "File '" << run_list_file_path << "' already exists!");
    }
    frunlist.reset(new std::ofstream);
    frunlist->open(run_list_file_path.c_str());
    if (!(*frunlist.get())) {
      frunlist.reset();
      DT_THROW(std::runtime_error, "Cannot open file '" << run_list_file_path << "'!");
    }
  }

  if (frunlist) {
    for (const auto& iseedfile : seedfiles_per_run) {
      *frunlist.get() << iseedfile.first << ' ';
      if (!_params_.list_mount_point.empty()) {
        *frunlist.get() << _params_.list_mount_point;
        if (_params_.list_mount_point[0] == '@') *frunlist.get() << ':';
        if (_params_.list_mount_point[0] == '$') *frunlist.get() << '/';
      }
      *frunlist.get() << iseedfile.second << std::endl;
    }
  }

}
