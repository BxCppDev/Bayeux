// ex01.cxx

// Ourselves
#include <genvtx/genvtx_config.h>

// Standard libraries:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third Party
// - Bayeux:
#include <bayeux/bayeux.h>
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/clhep_units.h>
#include <datatools/properties.h>
// - Bayeux/geomtools:
#include <geomtools/manager.h>
#include <geomtools/gnuplot_draw.h>
// - Bayeux/genvtx:
#include <genvtx/manager.h>

int main(int argc_, char ** argv_)
{
  bayeux::initialize(argc_, argv_);

  datatools::logger::priority logging = datatools::logger::PRIO_WARNING;
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "genvtx example program : ex01" << std::endl;

    int prng_seed = 314159;

    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];
      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-d") || (option == "--debug")) {
          logging = datatools::logger::PRIO_DEBUG;
        } else {
          DT_LOG_WARNING(logging, "Ignoring option '" << option << "' !");
        }
      } else {
        std::string argument = token;
        DT_LOG_WARNING(logging, "Ignoring argument '" << argument << "' !");
      }
      iarg++;
    }

    // Geometry manager :
    DT_LOG_DEBUG(logging, "Initializing the geometry manager...");
    std::string GeoMgrConfigFile = "config/geometry/manager.conf";
    geomtools::manager GeoMgr;
    datatools::fetch_path_with_env (GeoMgrConfigFile);
    datatools::properties GeoMgrConfig;
    datatools::read_config (GeoMgrConfigFile, GeoMgrConfig);
    GeoMgr.initialize (GeoMgrConfig);

    // Vertex generator manager :
    DT_LOG_DEBUG(logging, "Initializing the vertex generator manager...");
    std::string VGMgrConfigFile = "config/vertex/manager.conf";
    genvtx::manager VGMgr(logging);
    VGMgr.set_geometry_manager(GeoMgr);
    VGMgr.set_rng_id("taus2");
    VGMgr.set_random_seed(prng_seed);
    datatools::fetch_path_with_env (VGMgrConfigFile);
    datatools::properties VGMgrConfig;
    datatools::read_config (VGMgrConfigFile, VGMgrConfig);
    VGMgr.initialize (VGMgrConfig);
    DT_LOG_DEBUG(logging, "The vertex generator manager: ");
    if (logging >= datatools::logger::PRIO_DEBUG) {
      VGMgr.tree_dump(std::clog);
    }

    // Set the name of the vertex generator to be activated :
    DT_LOG_DEBUG(logging, "Choosing the vertex generator...");
    std::string vg_name = "all_mezzanine_pcb_bulk.vg";
    //vg_name = "all_coil_bulk.vg";
    if (! vg_name.empty()) {
      VGMgr.activate_current_vg(vg_name);
    }

    // Vertex generation loop :
    DT_LOG_DEBUG(logging, "Generating random vertices...");
    int number_of_vertices = 10000;
    int vtx_counter = 0;
    while (vtx_counter < number_of_vertices && VGMgr.can_shoot_vertex()) {
      geomtools::vector_3d vertex_pos;
      VGMgr.shoot_vertex(vertex_pos);
      geomtools::gnuplot_draw::basic_draw_point(std::cout,
                                                (vertex_pos / CLHEP::mm));
      vtx_counter++;
    }

    DT_LOG_DEBUG(logging, "This is the end.");
  }
  catch(std::exception & x) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL, x.what());
    error_code = EXIT_FAILURE;
  }
  catch(...) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL, "Unexpected error !");
    error_code = EXIT_FAILURE;
  }
  bayeux::terminate();
  return error_code;
}
