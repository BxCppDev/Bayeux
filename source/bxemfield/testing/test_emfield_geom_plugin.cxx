// test_emfield_geom_plugin.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/ioutils.h>
#include <datatools/service_manager.h>
// - Bayeux/geomtools:
#include <geomtools/manager.h>
#include <geomtools/gnuplot_drawer.h>
#include <geomtools/display_data.h>
#include <geomtools/blur_spot.h>

// This project:
#include <emfield/emfield_config.h>
#include <emfield/electromagnetic_field_manager.h>
#include <emfield/emfield_geom_plugin.h>
#include <emfield/geom_map.h>

int main (int argc_, char ** argv_)
{
  using namespace std;

  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for the 'emfield::emfield_geom_plugin' class." << std::endl;

    bool   debug   = false;
    bool   plot    = false;
    double b_scale = 1.0;

    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];
      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-d") || (option == "--debug")) {
          debug = true;
        }
        else if ((option == "-p") || (option == "--plot")) {
          plot = true;
        }
        else if ((option == "-P") || (option == "--no-plot")) {
          plot = false;
        }
        else if ((option == "-B:10")) {
          b_scale /= 10;
        }
        else if ((option == "-Bx10")) {
          b_scale *= 10;
        }
        else {
          std::clog << "warning: ignoring option '" << option << "' !"
                    << std::endl;
        }
      }
      else {
        std::string argument = token;
        {
          std::clog << "warning: ignoring argument '" << argument << "' !"
                    << std::endl;
        }
      }
      iarg++;
    }

    DATATOOLS_FACTORY_GET_SYSTEM_REGISTER(geomtools::manager::base_plugin).tree_dump (std::clog,
                                                                                      "System geometry plugin register: ",
                                                                                      "INFO: ");

    // This is the geometry manager :
    geomtools::manager geo_manager;
    geo_manager.tree_dump(std::clog, "The geometry manager (uninitialized) : ");
    if (debug) geo_manager.set_logging_priority (datatools::logger::PRIO_DEBUG);

    std::string mag_field_plugin_file="${EMFIELD_TESTING_DIR}/config/test_emfield_geom_plugin.conf";
    datatools::fetch_path_with_env(mag_field_plugin_file);
    datatools::multi_properties mag_field_plugins_setup;
    mag_field_plugins_setup.read(mag_field_plugin_file);

    datatools::properties gm_setup;
    std::string gm_filename = "${EMFIELD_TESTING_DIR}/config/geometry/test_manager.conf";
    datatools::fetch_path_with_env(gm_filename);
    datatools::read_config(gm_filename, gm_setup);
    geo_manager.initialize (gm_setup);
    // Load additional plugins :
    geo_manager.get_plugins_factory_register().tree_dump (std::clog,
                                                          "Embeded geometry plugin register: ",
                                                          "INFO: ");
    geo_manager.load_plugins(mag_field_plugins_setup);
    geo_manager.tree_dump(std::clog, "The geometry manager with EM fields plugin: ");

    const emfield::electromagnetic_field_manager * EMFmgr_ref = 0;
    std::string emfield_plugin_name = "fields_driver";
    if (geo_manager.has_plugin (emfield_plugin_name)
        && geo_manager.is_plugin_a<emfield::emfield_geom_plugin>(emfield_plugin_name)) {
      std::clog << "NOTICE: " << "Found plugin named '"
                << emfield_plugin_name
                << "'" << std::endl;
      const emfield::emfield_geom_plugin & egp
        = geo_manager.get_plugin<emfield::emfield_geom_plugin>(emfield_plugin_name);
      const emfield::electromagnetic_field_manager & field_mgr = egp.get_manager();
      field_mgr.tree_dump(std::clog, "The EM field manager : ");
      EMFmgr_ref = &field_mgr;
    }

    if (EMFmgr_ref == 0) {
      throw std::logic_error("No field manager was extracted from the geometry manager's collection of plugins !");
    }

    // This is the EM fields manager :
    const emfield::electromagnetic_field_manager & EMFmgr = *EMFmgr_ref;

    geomtools::display_data DD;

    if (EMFmgr.has_geom_map()) {
      const emfield::geom_map & EMFgeomap = EMFmgr.get_geom_map();
      int count = 0;
      for (emfield::geom_map::association_dict_type::const_iterator i
             = EMFgeomap.get_associations().begin();
           i != EMFgeomap.get_associations().end();
           i++) {
        std::string association_name = i->first;
        //std::cerr << "DEVEL: association = '" << association_name << "'  ";
        const emfield::geom_map::association_entry & e = i->second;
        //const geomtools::logical_volume & logvol = e.get_logvol();
        const emfield::base_electromagnetic_field & field = e.get_field();
        if (! field.is_magnetic_field()) continue;
        if ( association_name != "vessel" ) continue;
        for (double x = -1 * CLHEP::m; x < +1 * CLHEP::m; x += 25.*CLHEP::cm) {
          for (double y = -1 * CLHEP::m; y < +1 * CLHEP::m; y += 25.*CLHEP::cm) {
            for (double z = -1 * CLHEP::m; z < +1 * CLHEP::m; z += 25.*CLHEP::cm) {
              geomtools::vector_3d position (x, y, z);
              geomtools::vector_3d magfield;
              field.compute_magnetic_field(position, 0.0, magfield);
              std::ostringstream B_label;
              B_label << "B_" << count++;
              geomtools::display_data::display_item & DI
                = DD.add_static_item (B_label.str(),
                                      "group::magfield",
                                      "blue");
              geomtools::blur_spot arrow(geomtools::blur_spot::dimension_one, 0.5 * CLHEP::mm);
              //std::cerr << "|magfield| = " << magfield.mag()/ CLHEP::gauss << " gauss\n";
              arrow.set_errors(magfield.mag() * b_scale * 2.0 * CLHEP::cm / CLHEP::gauss );
              // Compute placement of the arrow :
              geomtools::placement plcmt;
              plcmt.set_translation(x, y, z);
              plcmt.set_orientation(magfield.getPhi(),
                                    magfield.getTheta(),
                                    0.0);
              arrow.generate_wires(DI.paths, plcmt);
              // New Bayeux 2.0.0 will use:
              // arrow.generate_wires(DI.wires, plcmt);
            } // for z
          } // for y
        } // for x
      }
    }
    if (plot) {
      int         visu_depth  = geomtools::gnuplot_drawer::display_level_no_limit();
      std::string drawer_view = geomtools::gnuplot_drawer::view_3d();
      std::string visu_object_name = "world";

      geomtools::gnuplot_drawer GPD;
      GPD.grab_properties().store(geomtools::gnuplot_drawer::world_name_key(),
                                  geo_manager.get_world_name ());
      GPD.set_view(drawer_view);
      GPD.set_mode(geomtools::gnuplot_drawer::mode_wired());
      geomtools::placement DD_pl;
      DD_pl.set_translation(0,0,0);
      GPD.add_display_data(DD, DD_pl);
      /* int view_code = */ GPD.draw(geo_manager,
                                     visu_object_name,
                                     visu_depth);
    }

    std::clog << "The end." << std::endl;
  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}
