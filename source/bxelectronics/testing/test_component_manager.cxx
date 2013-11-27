#include <iostream>

// Third party
// - Bayeux/datatools
#include <datatools/logger.h>
#include <datatools/exception.h>

// This project
#include <electronics/component_manager.h>
#include <electronics/rack_model_base.h>
#include <electronics/crate_model_base.h>
#include <electronics/board_model_base.h>
#include <electronics/component_types.h>

int main( int argc_, char * argv_[])
{
  int error_code = EXIT_SUCCESS;

  try {

    electronics::component_manager comp_mgr;
    datatools::properties comp_mgr_config;
    std::string config_file = "${ELECTRONICS_TESTING_DIR}/config/manager.conf";
    datatools::fetch_path_with_env(config_file);
    std::clog << "NOTICE: config file='" << config_file << "'" << std::endl;
    datatools::properties::read_config(config_file, comp_mgr_config);
    comp_mgr.initialize(comp_mgr_config);
    comp_mgr.tree_dump(std::clog, "The electronics components manager: ");

    if (comp_mgr.is_a<electronics::component_model_base>("setup")) {
      const electronics::component_model_base & setup
        = comp_mgr.get<electronics::component_model_base>("setup");
      setup.tree_dump(std::clog, "The electronics setup: ");
    }

    std::cerr << "DEVEL: The end." << std::endl;

  }
  catch (std::exception& error) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL,
                 error.what());
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL,
                 "Unexpected error!");
    error_code = EXIT_FAILURE;
  }
  return error_code;
}
