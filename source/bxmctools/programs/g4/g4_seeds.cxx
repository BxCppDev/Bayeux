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

// Third party:
// - Boost:
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/scoped_ptr.hpp>
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
class g4_seed_generator
{
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
    std::string logging;
    uint32_t    number_of_runs;
    uint32_t    run_start;
    std::string base_dir;
    std::string pattern;
    std::string list_file;
  };

  /// Default constructor
  g4_seed_generator();

  /// Destructors
  ~g4_seed_generator();

  /// Initialize
  void initialize(const parameters & params_);

  /// Run
  void run();

  /// Terminate
  void terminate();

protected:

  void _run_generate();

private:

  datatools::logger::priority _logging_; ///< Logging proprity threshold
  parameters _params_; ///< Parameters

};

int main(int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;

  bayeux::initialize(argc_, argv_, 0xFFFF);
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
      // cl_parser.positional(args);
      // cl_parser.allow_unregistered();

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

    g4_seed_generator app;
    app.initialize(params);
    app.run();
    app.terminate();

  } catch (std::exception & x) {
    DT_LOG_FATAL(logging, x.what());
    error_code = EXIT_FAILURE;
  } catch (...) {
    DT_LOG_FATAL(logging, "Unexpected error !");
    error_code = EXIT_FAILURE;
  }
  DT_LOG_TRACE(logging, "g4_production ends here.");

  bayeux::terminate();
  return error_code;
}

g4_seed_generator::parameters::parameters()
{
  set_defaults();
  return;
}

void g4_seed_generator::parameters::set_defaults()
{
  logging        = "fatal";
  number_of_runs = 1;
  run_start      = 0;
  base_dir       = ".";
  pattern        = "seeds_%n.conf";
  list_file      = "runs.lis";
  return;
}

void g4_seed_generator::parameters::print_usage(const boost::program_options::options_description & opts_,
                                                std::ostream & out_)
{
  const std::string APP_NAME = "bxg4_seeds";
  out_ << APP_NAME << " -- generator of seeds for the GEANT4 simulation wrapper" << std::endl;
  out_ << std::endl;
  out_ << "Usage : " << std::endl;
  out_ << std::endl;
  out_ << "  " << APP_NAME << " [OPTIONS] [ARGUMENTS] "
       << std::endl;
  out_ << std::endl;
  out_ << opts_ << std::endl;
  out_ << std::endl;
  return;
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

    ("logging-priority,l",
     po::value<std::string>(&params_.logging)
     ->default_value("fatal")
     ->value_name("level"),
     "Set the logging priority threshold")

    ("number-of-runs,n",
     po::value<uint32_t>(&params_.number_of_runs)
     ->default_value(1)
     ->value_name("integer"),
     "Set the number of runs for which seeds are generated")

    ("run-start,s",
     po::value<uint32_t>(&params_.run_start)
     ->default_value(0)
     ->value_name("integer"),
     "Set the number of the first run")

    ("base-dir,d",
     po::value<std::string>(&params_.base_dir)
     ->default_value(".")
     ->value_name("dirname"),
     "Set the base directory where seed file is generated")

    ("pattern,p",
     po::value<std::string>(&params_.pattern)
     ->default_value("seeds_%n.conf")
     ->value_name("pattern"),
     "Set the pattern of the generated incremental seed file")

    ("list,t",
     po::value<std::string>(&params_.list_file)
     ->default_value("runs.lis")
     ->value_name("name"),
     "Set the name of the run list file")

    ;
  return;
}

g4_seed_generator::g4_seed_generator()
{
  _logging_ = datatools::logger::PRIO_FATAL;
  return;
}

g4_seed_generator::~g4_seed_generator()
{
  return;
}

void g4_seed_generator::initialize(const parameters & params_)
{
  {
    datatools::logger::priority prio = datatools::logger::get_priority(params_.logging);
    if (prio != datatools::logger::PRIO_UNDEFINED) {
      _logging_ = prio;
    }
  }
  {
    bool exists = params_.base_dir.find("%n") != std::string::npos;
    DT_THROW_IF(exists, std::logic_error,
                "Base directory has a forbidden '%n' directive!");
  }
  {
    bool exists = params_.pattern.find("%n") != std::string::npos;
    DT_THROW_IF(!exists, std::logic_error,
                "Missing '%n' directive in the output file pattern!");
  }
  _params_ = params_;
  return;
}

void g4_seed_generator::run()
{
  _run_generate();
  return;
}

void g4_seed_generator::_run_generate()
{
  std::map<uint32_t, std::string> seeds_per_run;
  std::set<std::string> check_signatures;
  // std::string last;
  std::size_t max_count = 3;
  for (std::size_t irun = _params_.run_start;
       irun != _params_.run_start + _params_.number_of_runs;
       irun++) {
    DT_LOG_DEBUG(_logging_, "Generate seeds for run #" << irun << "...");
    std::size_t count = 0;
    // DT_LOG_NOTICE(_logging_, "  Last = '" << last << "'");
    // for (std::set<std::string>::const_iterator icsig = check_signatures.begin();
    //      icsig != check_signatures.end();
    //      icsig++) {
    //   DT_LOG_NOTICE(_logging_, "  Check signature = '" << *icsig << "'");
    // }
    while (true) {
      int mgr_seed  = mygsl::random_utils::SEED_AUTO;
      int vg_seed   = mygsl::random_utils::SEED_AUTO;
      int eg_seed   = mygsl::random_utils::SEED_AUTO;
      int shpf_seed = mygsl::random_utils::SEED_AUTO;
      mygsl::seed_manager smgr;
      smgr.set_init_seed_flags(mygsl::seed_manager::INIT_SEED_FROM_RANDOM_DEVICE);
      smgr.add_seed(mctools::g4::manager::constants::instance().G4_MANAGER_LABEL,
                    mgr_seed);
      smgr.add_seed(mctools::g4::manager::constants::instance().VERTEX_GENERATOR_LABEL,
                    vg_seed);
      smgr.add_seed(mctools::g4::manager::constants::instance().EVENT_GENERATOR_LABEL,
                    eg_seed);
      smgr.add_seed(mctools::g4::manager::constants::instance().SHPF_LABEL,
                    shpf_seed);
      smgr.ensure_different_seeds();
      if (datatools::logger::is_debug(_logging_)) {
        smgr.dump(std::cerr);
      }
      std::ostringstream seeds_repr;
      seeds_repr << smgr;
      // if (!last.empty()) {
      //   DT_LOG_NOTICE(_logging_, "  Using last seed set = '" << seeds_repr.str() << "'...");
      //   seeds_repr << last;
      // } else {
      //   seeds_repr << smgr;
      // }
      DT_LOG_DEBUG(_logging_, "  Seed set = '" << seeds_repr.str() << "'");
      std::string signature = seeds_repr.str();
      if (check_signatures.count(signature) == 0) {
        seeds_per_run[irun] = signature;
        DT_LOG_DEBUG(_logging_, "  Registering signature = '" << signature << "'...");
        check_signatures.insert(signature);
        // last = signature;
        break;
      } else {
        DT_LOG_WARNING(_logging_,
                       "  Seed set '" << signature << "' is already used: "
                       << "Generating a new one...");
        count++;
      }
      if (count > max_count) {
        DT_THROW(std::runtime_error, "Could not generate an independant seed set after "
                 << count << " tries!");
      }
    }
  }

  std::map<uint32_t, std::string> seedfiles_per_run;
  std::ostringstream list_out;
  for (std::map<uint32_t, std::string>::const_iterator iseed = seeds_per_run.begin();
       iseed != seeds_per_run.end();
       iseed++) {
    uint32_t irun = iseed->first;
    std::string path_pattern = _params_.base_dir + "/" + _params_.pattern;
    DT_LOG_DEBUG(_logging_, "  Original path for run #" << irun << " is '" << path_pattern << "'.");
    boost::replace_all(path_pattern, "//", "/");
    boost::replace_all(path_pattern, "%n", boost::lexical_cast<std::string>(irun));
    DT_LOG_DEBUG(_logging_, "  Path for run #" << irun << " is '" << path_pattern << "'.");
    boost::filesystem::path base_dir_path = boost::filesystem::path(path_pattern).parent_path();
    if (!boost::filesystem::exists(base_dir_path)) {
      DT_LOG_DEBUG(_logging_, "  Directory '" << base_dir_path.string() << "' does not exist!");
      bool base_path_created = boost::filesystem::create_directories(base_dir_path);
      DT_THROW_IF(!base_path_created, std::runtime_error, "Cannot create directory " << base_dir_path << "!");
    } else {
      DT_LOG_DEBUG(_logging_, "  Directory '" << base_dir_path.string() << "' already exists!");
    }
    {
      std::string test_path = path_pattern;
      datatools::fetch_path_with_env(test_path);
      if (boost::filesystem::exists(test_path)) {
        DT_THROW(std::logic_error, "File '" << path_pattern << "' already exists!");
      }
    }
    std::ofstream fout(path_pattern.c_str());
    DT_THROW_IF(!fout, std::runtime_error, "Cannot open file '" << path_pattern << "'!");
    fout << iseed->second << std::endl;
    seedfiles_per_run[irun] = path_pattern;
  }
  DT_LOG_NOTICE(_logging_, "Generated run seeding files from directory: '" << _params_.base_dir << "'");

  boost::scoped_ptr<std::ofstream> flist;
  std::string list_file_path;
  if (!_params_.list_file.empty()) {
    bool full_list_path = false;
    {
      std::string test_lis = _params_.list_file;
      datatools::fetch_path_with_env(test_lis);
      if (test_lis.size() > 0) {
        if (test_lis[0] == '/') full_list_path = true;
      }
    }
    if (! full_list_path) {
      list_file_path = _params_.base_dir + "/" + _params_.list_file;
    } else {
      list_file_path = _params_.list_file;
    }
    boost::replace_all(list_file_path, "//", "/");
    datatools::fetch_path_with_env(list_file_path);
    if (boost::filesystem::exists(list_file_path)) {
      DT_THROW(std::runtime_error, "File '" << list_file_path << "' already exists!");
    }
    flist.reset(new std::ofstream);
    flist->open(list_file_path.c_str());
    if (!(*flist.get())) {
      flist.reset();
      DT_THROW(std::runtime_error, "Cannot open file '" << list_file_path << "'!");
    }
  }

  if (flist.get() != nullptr) {
    for (std::map<uint32_t, std::string>::const_iterator iseedfile = seedfiles_per_run.begin();
       iseedfile != seedfiles_per_run.end();
       iseedfile++) {
      *flist.get() << iseedfile->first << ' ' << iseedfile->second << std::endl;
    }
    DT_LOG_NOTICE(_logging_, "Generated run seeding list file: '" << list_file_path << "'");
  }

  return;
}

void g4_seed_generator::terminate()
{
  return;
}
