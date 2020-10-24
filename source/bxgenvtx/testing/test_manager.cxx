// test_manager.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/clhep_units.h>
// - Bayeux/geomtools:
#include <geomtools/manager.h>
#include <geomtools/geomtools_config.h>
#include <geomtools/display_data.h>
#include <geomtools/box.h>
#include <geomtools/blur_spot.h>
#include <geomtools/gnuplot_drawer.h>
#include <geomtools/gnuplot_draw.h>

// This project:
#include <genvtx/version.h>
#include <genvtx/manager.h>

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for the 'genvtx::manager' class." << std::endl;

    // bool debug = false;
    int nshoots = 1;
    std::string vg_name;
    bool visu = false;
    bool dump = false;
    std::string visu_object;
    bool dump_gids = false;
    double tiny = 1.0;
    int test_id = 1;

    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];

      if (token[0] == '-')  {
        std::string option = token;
        if ((option == "-v") || (option == "--visu")) {
          visu = true;
        }
        // else if ((option == "-d") || (option == "--debug")) {
        //   debug = true;
        // }
        else if ((option == "-m") || (option == "--many")) {
          nshoots *= 10;
        }
        else if ((option == "-D") || (option == "--dump")) {
          dump = true;
        }
        else if ((option == "-G") || (option == "--dump-gids")) {
          dump_gids = true;
        }
        else if ((option == "-g") || (option == "--vertex-generator")) {
          vg_name = argv_[++iarg];
        }
        else if ((option == "-V") || (option == "--visu-object")) {
          visu = true;
          visu_object = argv_[++iarg];
        }
        else if ((option == "-t") || (option == "--tiny")) {
          tiny *= 0.5;
        }
        else if ((option == "-T1") || (option == "--test-1")) {
          test_id = 1;
        }
        else if ((option == "-T2") || (option == "--test-2")){
          test_id = 2;
        }
        else {
          std::clog << "warning: ignoring option '" << option << "'!" << std::endl;
        }
      }
      else{
        std::string argument = token;
        {
          std::clog << "warning: ignoring argument '" << argument << "'!" << std::endl;
        }
      }
      iarg++;
    }

    std::clog << "genvtx version : " << GENVTX_LIB_VERSION << std::endl;

    // Configuration files :
    std::string GeoMgrConfigFile = "${GENVTX_TESTING_DIR}/config/test-1.0/geometry/manager.conf";
    std::string VGMgrConfigFile = "${GENVTX_TESTING_DIR}/config/test-1.0/vg_mgr.conf";
    if (test_id == 2) {
      GeoMgrConfigFile = "${GENVTX_TESTING_DIR}/config/test-1.0/test_manager.conf";
      VGMgrConfigFile = "${GENVTX_TESTING_DIR}/config/test-2.0/vg_mgr.conf";
    }

    // Geometry manager :
    geomtools::manager GeoMgr;
    datatools::fetch_path_with_env (GeoMgrConfigFile);
    datatools::properties GeoMgrConfig;
    datatools::read_config (GeoMgrConfigFile, GeoMgrConfig);
    GeoMgr.initialize (GeoMgrConfig);

    if (dump_gids) {
      std::clog << "The embeded GID mapping : " << std::endl;
      GeoMgr.get_mapping ().dump_dictionnary (std::clog);
    }
    // Vertex generator manager :
    genvtx::manager VGMgr;
    VGMgr.set_geometry_manager(GeoMgr);
    VGMgr.set_rng_id("taus2");
    VGMgr.set_random_seed(314159);
    datatools::fetch_path_with_env (VGMgrConfigFile);
    datatools::properties VGMgrConfig;
    datatools::read_config (VGMgrConfigFile, VGMgrConfig);
    VGMgr.initialize (VGMgrConfig);
    VGMgr.tree_dump(std::clog, "The vertex generator manager: ");

    if (! vg_name.empty()) {
      VGMgr.activate_current_vg(vg_name);
    }

    // A display object that contains all vertex rendering infos:
    geomtools::display_data dd;

    // The 3D representation of a vertex :
    double vertex_dim = 1.0*CLHEP::mm;
    vertex_dim *= tiny;
    geomtools::blur_spot vertex_spot(3, 1 *CLHEP::mm);
    vertex_spot.set_errors(vertex_dim,vertex_dim,vertex_dim);

    int vtx_counter = 0;
    while (vtx_counter < nshoots && VGMgr.can_shoot_vertex()) {
      geomtools::vector_3d vertex_pos;
      VGMgr.shoot_vertex(vertex_pos);
      //cerr << vertex_pos << std::endl;
      if (dump) {
        geomtools::gnuplot_draw::basic_draw_point(std::cout, vertex_pos);
      }
      if (visu) {
        std::ostringstream vertex_name_oss;
        vertex_name_oss << "vertex_" << vtx_counter;
        geomtools::display_data::display_item & vertex_spot_DI
          = dd.add_static_item (vertex_name_oss.str(),
                                "group::vertex",
                                "magenta");
        geomtools::placement vertex_plcmt;
        vertex_plcmt.set_translation(vertex_pos);
        vertex_spot.generate_wires (vertex_spot_DI.wires, vertex_plcmt);
      }
      vtx_counter++;
    }

    // Now the rendering :
    if (visu) {
      if(visu_object.empty()) {
        visu_object = GeoMgr.get_world_name ();
      }
      std::cerr << "Visu object label : '" << visu_object << "'" << std::endl;
      geomtools::gnuplot_drawer GPD;
      GPD.grab_properties().store(geomtools::gnuplot_drawer::world_name_key(),
                                  GeoMgr.get_world_name ());
      geomtools::placement dd_pl;
      dd_pl.set_translation (0.0, 0.0*CLHEP::cm, 0.0);
      GPD.add_display_data (dd, dd_pl);
      GPD.set_view (geomtools::gnuplot_drawer::view_3d());
      GPD.set_mode (geomtools::gnuplot_drawer::mode_wired());
      int view_code = GPD.draw (GeoMgr,
                                visu_object,
                                100);
      if (view_code != 0) {
        std::cerr << "ERROR: " << "Cannot display the object with label '"
                  << GeoMgr.get_world_name () << "' !" << std::endl;
      }
    }

    {
      datatools::object_configuration_description OCD;
      datatools::load_ocd<genvtx::manager>(OCD);
      std::ofstream fscf ("test_OCD_manager.sample.conf");
      OCD.generate_sample_configuration(fscf,
                                        "the configuration of a 'genvtx::manager' test object");
      OCD.print(std::clog);
    }
    std::clog << "The end." << std::endl;
  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "Unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}
