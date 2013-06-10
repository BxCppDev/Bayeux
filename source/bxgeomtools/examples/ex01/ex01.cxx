#include <cstdlib>
#include <exception>
#include <iostream>

#include <datatools/logger.h>

#include <geomtools/manager.h>
#include <geomtools/gnuplot_drawer.h>

int main(int argc_, char ** argv_)
{
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
    std::string GM_config_file = "config/manager.conf";
    datatools::properties::read_config(GM_config_file, GM_config);

    // Initialize the geometry manager from the configuration parameters :
    GM.initialize(GM_config);

    if (logging >= datatools::logger::PRIO_NOTICE) {
      // Smart print :
      GM.tree_dump(std::clog, "The geometry manager: ");
    }

    // A Gnuplot based 3D-renderer :
    geomtools::gnuplot_drawer GPD;
    GPD.grab_properties().store(geomtools::gnuplot_drawer::WORLD_NAME_KEY,
                               GM.get_world_name ());
    GPD.set_view (geomtools::gnuplot_drawer::VIEW_3D);
    GPD.set_mode (geomtools::gnuplot_drawer::MODE_WIRED);
    int view_code = GPD.draw (GM,
                              "world",
                              geomtools::gnuplot_drawer::DISPLAY_LEVEL_NO_LIMIT);

  }
  catch(std::exception & x) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL,x.what());
  }
  catch(...) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL,"Unexpected error !");
  }
  return 0;
}
