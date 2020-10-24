// test_step_hit_processor_factory.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <set>

// Third party:
// - Bayeux/datatools
#include <datatools/multi_properties.h>
// - Bayeux/geomtools
#include <geomtools/manager.h>
// - Bayeux/emfield
#include <emfield/version.h>

// This project:
#include <mctools/step_hit_processor_factory.h>


int main(int argc_, char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'step_hit_processor_factory'!" << std::endl;

    std::string emfver = emfield::version::get_version();

    bool debug = false;
    std::string gmgr_config_file;
    std::string mconfig_filename;

    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];
      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-d") || (option == "--debug"))  {
          debug = true;
        }
        else {
          std::clog << "warning: ignoring option '" << option << "'!" << std::endl;
        }
      }
      else {
        std::string argument = token;
        {
          std::clog << "warning: ignoring argument '" << argument << "'!" << std::endl;
        }
      }
      iarg++;
    }

    // Setup the geometry manager:
    if (gmgr_config_file.empty ()) {
      gmgr_config_file = "${MCTOOLS_TESTING_DIR}/config/g4/test-2.0/geometry/manager.conf";
    }
    datatools::fetch_path_with_env (gmgr_config_file);
    datatools::properties gmgr_config;
    datatools::properties::read_config (gmgr_config_file,
                                        gmgr_config);

    geomtools::manager gmgr;
    if (debug) gmgr.set_logging_priority(datatools::logger::PRIO_DEBUG);
    gmgr.set_mapping_requested (true);
    gmgr.initialize (gmgr_config);

    if (debug) {
      std::clog << "DEBUG: Geometry manager is built." << std::endl;
    }

    // Setup the step hit processor factory:
    if (mconfig_filename.empty ()) {
      mconfig_filename = "${MCTOOLS_TESTING_DIR}/config/g4/test-2.0/simulation/step_hit_processor_factory.conf";
    }
    datatools::fetch_path_with_env (mconfig_filename);

    datatools::multi_properties mconfig ("name", "type");
    datatools::multi_properties::read_config(mconfig_filename, mconfig);
    if (debug) {
      std::clog << "DEBUG: Configuration file is loaded." << std::endl;
    }

    // Output profiles to be activated in addition to the default:
    std::set<std::string> output_profiles;
    output_profiles.insert("source_details");
    // output_profiles.insert("scin_details");

    mctools::step_hit_processor_factory SHPF;
    if (debug) SHPF.set_logging_priority(datatools::logger::PRIO_DEBUG);
    if (debug) {
      std::clog << "DEBUG: Factory is instantiated." << std::endl;
    }
    SHPF.set_output_profiles(output_profiles);
    SHPF.set_description ("A list of MC step hit processors");
    SHPF.set_geometry_manager(gmgr); // pass the address of the geom manager.
    if (debug) {
      std::clog << "DEBUG: Factory is configured." << std::endl;
    }
    SHPF.load (mconfig);
    if (debug) {
      std::clog << "DEBUG: Configuration is setup." << std::endl;
    }
    SHPF.initialize();
    SHPF.tree_dump (clog, "Step hit processor factory : ");

    std::clog << "The end." << std::endl;
  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}
