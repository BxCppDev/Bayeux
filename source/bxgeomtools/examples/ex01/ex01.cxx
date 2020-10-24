// This project:
#include <geomtools/geomtools.h>

// Standard library:
#include <cstdlib>
#include <exception>
#include <iostream>

// Third party:
// - Bayeux:
#include <bayeux/bayeux.h>
// - Bayeux/datatools:
#include <datatools/datatools.h>
#include <datatools/logger.h>
// - Bayeux/materials:
#include <materials/manager.h>

// This project:
#include <geomtools/manager.h>
#include <geomtools/gnuplot_drawer.h>

int main(int argc_, char ** argv_)
{
  bayeux::initialize(argc_, argv_);
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_NOTICE;
  try {

    if (argc_>1) {
      std::string opt = argv_[1];
      if (opt == "--debug") logging = datatools::logger::PRIO_DEBUG;
      else if (opt == "--warning") logging = datatools::logger::PRIO_WARNING;
    }

    // Declare a geometry manager :
    geomtools::manager GM;
    GM.set_logging_priority(logging);

    // Parse its config file :
    datatools::properties GM_config;
    std::string GM_config_file = "${CONFIG_DIR}/manager.conf";
    datatools::fetch_path_with_env(GM_config_file);
    datatools::read_config(GM_config_file, GM_config);

    // Initialize the geometry manager from the configuration parameters :
    GM.initialize(GM_config);

    if (logging >= datatools::logger::PRIO_NOTICE) {
      // Smart print :
      GM.tree_dump(std::clog, "The geometry manager: ");
    }

    // A Gnuplot based 3D-renderer :
    geomtools::gnuplot_drawer GPD;
    GPD.grab_properties().store(geomtools::gnuplot_drawer::world_name_key(),
                               GM.get_world_name());
    GPD.set_view (geomtools::gnuplot_drawer::view_3d());
    GPD.set_mode (geomtools::gnuplot_drawer::mode_wired());
    int view_code = GPD.draw (GM,
                              "world",
                              geomtools::gnuplot_drawer::display_level_no_limit());
  } catch(std::exception & x) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL, x.what());
    error_code = EXIT_FAILURE;
  } catch(...) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL, "Unexpected error !");
    error_code = EXIT_FAILURE;
  }
  bayeux::terminate();
  return error_code;
}
