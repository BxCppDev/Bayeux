// -*- mode: c++ ; -*-
// test_logical_volume.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <boost/scoped_ptr.hpp>

#include <datatools/temporary_files.h>
#include <datatools/clhep_units.h>

#include <geomtools/box.h>
#include <geomtools/cylinder.h>
#include <geomtools/logical_volume.h>
#include <geomtools/physical_volume.h>
#include <geomtools/regular_grid_placement.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
#include <geomtools/utils.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    clog << "Test program for class 'geomtools::logical_volume' !" << endl;

    bool debug = false;
    bool draw = false;
    bool ignore_daughters = false;
    unsigned long int seed48 = 314159;

    int iarg = 1;
    while (iarg < argc_) {
      string arg = argv_[iarg];

      if (arg == "-d" || arg == "--debug") debug = true;
      else if (arg == "-D" || arg == "--draw") draw = true;
      else if (arg == "-i" || arg == "--ignore-daughters") ignore_daughters = true;

      iarg++;
    }

    srand48(seed48);

    geomtools::box world_box (1.0 * CLHEP::m, 1.0 * CLHEP::m, 1.0 * CLHEP::m);

    geomtools::logical_volume world_log ("world", world_box);
    world_log.grab_parameters ().store ("material.ref", "vacuum");
    world_log.grab_parameters ().store ("visualization.visible", false);
    world_log.tree_dump (cout, "World");

    geomtools::box vessel_box (0.9 * CLHEP::m, 0.9 * CLHEP::m, 0.9 * CLHEP::m);
    geomtools::logical_volume vessel_log ("vessel.log", vessel_box);
    vessel_log.grab_parameters ().store ("material.ref", "inox");
    vessel_log.grab_parameters ().store ("visualization.visible", true);
    vessel_log.grab_parameters ().store ("visualization.color", "blue");
    vessel_log.tree_dump (cout, "Vessel");
    geomtools::placement vessel_placement;
    geomtools::physical_volume vessel_phys ("vessel.phys",
                                            vessel_log,
                                            world_log,
                                            vessel_placement);
    vessel_phys.tree_dump(cout, "Vessel in world physical: ");

    geomtools::box gas_box (0.89 * CLHEP::m, 0.89 * CLHEP::m, 0.89 * CLHEP::m);
    geomtools::logical_volume gas_log ("gas.log", gas_box);
    gas_log.grab_parameters ().store ("material.ref", "air");
    gas_log.grab_parameters ().store ("visualization.visible", true);
    gas_log.grab_parameters ().store ("visualization.color", "cyan");
    gas_log.tree_dump (cout, "Air");
    geomtools::placement gas_placement;
    geomtools::physical_volume gas_phys ("gas.phys",
                                         gas_log,
                                         vessel_log,
                                         gas_placement);

    double r_source = 0.25 * CLHEP::m;
    double z_source = 0.15 * CLHEP::m;
    geomtools::cylinder source_cyl (r_source, z_source);
    geomtools::logical_volume source_log ("source.log", source_cyl);
    source_log.grab_parameters ().store ("material.ref", "mylar");
    source_log.grab_parameters ().store ("visualization.visible", true);
    source_log.grab_parameters ().store ("visualization.color", "blue");
    geomtools::placement source_placement;
    geomtools::physical_volume source_phys ("source.phys",
                                            source_log,
                                            gas_log,
                                            source_placement);

    geomtools::cylinder source_hole_cyl (0.04 * CLHEP::m, z_source);
    geomtools::logical_volume source_hole_log ("source_hole.log", source_hole_cyl);
    source_hole_log.grab_parameters ().store ("material.ref", "air");
    source_hole_log.grab_parameters ().store ("visualization.visible", true);
    source_hole_log.grab_parameters ().store ("visualization.color", "green");
    source_hole_log.tree_dump (cout, "Source hole");
    geomtools::regular_grid_placement source_hole_placement;
    geomtools::placement source_hole_basic_placement;
    source_hole_placement.set_basic_placement(source_hole_basic_placement);
    source_hole_placement.set_mode(geomtools::regular_grid_placement::MODE_XY);
    source_hole_placement.set_number_of_rows(2);
    source_hole_placement.set_number_of_columns(2);
    source_hole_placement.set_column_step(0.20 * CLHEP::m);
    source_hole_placement.set_row_step(0.15 * CLHEP::m);
    source_hole_placement.set_centered(true);
    source_hole_placement.tree_dump (cout, "Source hole placement : ");
    geomtools::physical_volume holes_phys ("source_holes.phys");
    holes_phys.set_logical(source_hole_log);
    holes_phys.set_placement(source_hole_placement);
    holes_phys.set_mother(source_log, "holes"); // After 'set_placement'
    holes_phys.lock();

    source_log.lock();
    source_log.tree_dump(cout, "Source");

    datatools::temp_file tmp_setup;
    tmp_setup.set_remove_at_destroy(false);
    tmp_setup.create ("/tmp", "geomtools_test_logical_volume_setup_");

    datatools::temp_file tmp_vertexes_1;
    tmp_vertexes_1.set_remove_at_destroy(false);
    tmp_vertexes_1.create ("/tmp", "geomtools_test_logical_volume_vertexes_1_");
    tmp_vertexes_1.out() << "# vertexes outside source : " << std::endl;

    datatools::temp_file tmp_vertexes_2;
    tmp_vertexes_2.set_remove_at_destroy(false);
    tmp_vertexes_2.create ("/tmp", "geomtools_test_logical_volume_vertexes_2_");
    tmp_vertexes_2.out() << "# vertexes inside source : " << std::endl;

    datatools::temp_file tmp_vertexes_3;
    tmp_vertexes_3.set_remove_at_destroy(false);
    tmp_vertexes_3.create ("/tmp", "geomtools_test_logical_volume_vertexes_3_");
    tmp_vertexes_3.out() << "# vertexes on source surface: " << std::endl;

    datatools::temp_file tmp_vertexes_4;
    tmp_vertexes_4.set_remove_at_destroy(false);
    tmp_vertexes_4.create ("/tmp", "geomtools_test_logical_volume_vertexes_4_");
    tmp_vertexes_4.out() << "# vertexes inside source daughter : " << std::endl;

    datatools::temp_file tmp_vertexes_5;
    tmp_vertexes_5.set_remove_at_destroy(false);
    tmp_vertexes_5.create ("/tmp", "geomtools_test_logical_volume_vertexes_5_");
    tmp_vertexes_5.out() << "# vertexes on source daughter surface: " << std::endl;

    geomtools::vector_3d pos;
    geomtools::rotation rot;

    // World solid drawing infos:
    tmp_setup.out() << "# world : " << std::endl;
    geomtools::gnuplot_draw::draw_box (tmp_setup.out(), pos, rot, world_box);
    tmp_setup.out() << std::endl;
    tmp_setup.out() << std::endl;

    // Source solid drawing infos:
    tmp_setup.out() << "# source : " << std::endl;
    geomtools::gnuplot_draw::draw_cylinder (tmp_setup.out(), pos, rot, source_cyl);
    tmp_setup.out() << std::endl;
    tmp_setup.out() << std::endl;

    // Source holes solids drawing infos:
    for (int item = 0; item < source_hole_placement.get_number_of_items(); item++) {
      tmp_setup.out() << "# source hole " << item << " : " << std::endl;
      geomtools::placement item_placement;
      source_hole_placement.get_placement(item, item_placement);
      if (debug) item_placement.tree_dump(std::cerr, "Source hole placement : ", "DEBUG: ");
      geomtools::gnuplot_draw::draw_cylinder (tmp_setup.out(),
                                              item_placement.get_translation(),
                                              item_placement.get_rotation(),
                                              source_hole_cyl);
    }
    tmp_setup.out() << std::endl;

    int nvertexes = 40000;
    int vertex_counter = 0;
    for (int vertex_counter = 0; vertex_counter < nvertexes; vertex_counter++) {
      int ntries = 0;
      geomtools::vector_3d vertex;
      while (true) {
        double x = source_cyl.get_radius() * (2*drand48()-1);
        double y = source_cyl.get_radius() * (2*drand48()-1);
        double z = 0.5 * source_cyl.get_z() * (2*drand48()-1);
        vertex.set(x, y, z);
        double source_tolerance = 25. * CLHEP::mm;
        double source_hole_tolerance = 25. * CLHEP::mm;
        geomtools::logical_volume::locate_result locres;
        uint32_t locate_result = source_log.locate(vertex,
                                                   ignore_daughters,
                                                   source_tolerance,
                                                   source_hole_tolerance,
                                                   &locres);
        if (debug) {
          std::cerr << "DEBUG: Locate result: " << std::endl;
          locres.dump(std::cerr, "    ");
        }
        //if (debug) std::cerr << "DEBUG: locate_result=" << locate_result << std::endl;
        if (locate_result == geomtools::SHAPE_DOMAIN_OUTSIDE) {
          geomtools::gnuplot_draw::basic_draw_point(tmp_vertexes_1.out(),vertex);
          break;
        } else if (locate_result == geomtools::SHAPE_DOMAIN_INSIDE ) {
          geomtools::gnuplot_draw::basic_draw_point(tmp_vertexes_2.out(),vertex);
          break;
        } else if (locate_result == geomtools::SHAPE_DOMAIN_ON_SURFACE ) {
          geomtools::gnuplot_draw::basic_draw_point(tmp_vertexes_3.out(),vertex);
          break;
        } else if (locate_result == (geomtools::SHAPE_DOMAIN_INSIDE | geomtools::SHAPE_DOMAIN_INSIDE_DAUGHTER) ) {
          geomtools::gnuplot_draw::basic_draw_point(tmp_vertexes_4.out(),vertex);
          break;
        } else if (locate_result == (geomtools::SHAPE_DOMAIN_INSIDE | geomtools::SHAPE_DOMAIN_ON_DAUGHTER_SURFACE)) {
          geomtools::gnuplot_draw::basic_draw_point(tmp_vertexes_5.out(),vertex);
          break;
        }
        ntries++;
        if (ntries > 1000) break;
      }
      //geomtools::gnuplot_draw::basic_draw_point(tmp_vertexes_.out(), vertex);
    }
    /*
    std::cerr << "DEBUG: TEST INSIDE_DAUGHTER =" << (geomtools::SHAPE_DOMAIN_INSIDE_DAUGHTER)
              << std::endl;
    std::cerr << "DEBUG: TEST ON_DAUGHTER_SURFACE =" << (geomtools::SHAPE_DOMAIN_ON_DAUGHTER_SURFACE)
              << std::endl;
    std::cerr << "DEBUG: TEST INSIDE_DAUGHTER =" << (geomtools::SHAPE_DOMAIN_INSIDE | geomtools::SHAPE_DOMAIN_INSIDE_DAUGHTER)
              << std::endl;
    std::cerr << "DEBUG: TEST ON_DAUGHTER_SURFACE=" << (geomtools::SHAPE_DOMAIN_INSIDE | geomtools::SHAPE_DOMAIN_ON_DAUGHTER_SURFACE)
              << std::endl;
    */
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    if (draw) {

      std::string visu_drawer_view = geomtools::gnuplot_drawer::VIEW_3D;
      std::string visu_drawer_mode = geomtools::gnuplot_drawer::MODE_WIRED;
      bool visu_drawer_labels = true;
      geomtools::gnuplot_drawer GPD;
      geomtools::placement world_placement;
      world_placement.set (0, 0, 0, 0 * CLHEP::degree, 0 * CLHEP::degree, 0 * CLHEP::degree);
      GPD.set_mode (visu_drawer_mode);
      GPD.set_view (visu_drawer_view);
      GPD.set_labels (visu_drawer_labels);
      GPD.draw (world_log,
                world_placement,
                geomtools::gnuplot_drawer::DISPLAY_LEVEL_NO_LIMIT,
                "world");



      Gnuplot g1 ("lines");
      g1.cmd ("set title 'Test geomtools::logical_volume' ");
      g1.cmd ("set grid");
      g1.cmd ("set size ratio -1");
      g1.cmd ("set view equal xyz");
      g1.cmd ("set xyplane at -1");
      g1.cmd ("set key outside right");
      g1.set_xrange (-0.6*CLHEP::m, +0.6*CLHEP::m)
        .set_yrange (-0.6*CLHEP::m, +0.6*CLHEP::m)
        .set_zrange (-0.6*CLHEP::m, +0.6*CLHEP::m);
      g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
      // g1.cmd("set pm3d");
      // g1.cmd("unset hidden3d");
      // //g1.cmd("set view map");
      // g1.cmd("set palette color model RGB defined (0 \"red\", 0.999 \"red\", 1 \"green\", 1.999 \"green\", 2 \"blue\", 2.999 \"blue\", 3 \"magenta\", 3.999 \"magenta\", 4 \"yellow\", 4.999 \"yellow\" )");
      // g1.cmd("set cbrange [0:5]");

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_setup.get_filename ()
                 << "'   index 0 title 'World'         with lines lt 4,"
                 << " '' index 1 title 'Source'        with lines lt 3 ,"
                 << " '' index 2 title 'Source holes'  with lines lt 3 ,"
                 << " '" << tmp_vertexes_1.get_filename () << "' using 1:2:3 title 'Vertexes outside source' with dots lt 1, "
                 << " '" << tmp_vertexes_2.get_filename () << "' using 1:2:3 title 'Vertexes inside source' with dots lt 2, "
                 << " '" << tmp_vertexes_3.get_filename () << "' using 1:2:3 title 'Vertexes on source surface' with dots lt 3, "
                 << " '" << tmp_vertexes_4.get_filename () << "' using 1:2:3 title 'Vertexes inside daughter' with dots lt 4, "
                 << " '" << tmp_vertexes_5.get_filename () << "' using 1:2:3 title 'Vertexes on daughter surface' with dots lt 5"
          ;

        //lt 1"; // points lt 1 pt 7 ps 1 "; with dots lt palette
        g1.cmd (plot_cmd.str ());
      }
      g1.showonscreen (); // window output
      geomtools::gnuplot_drawer::wait_for_key ();
      usleep (200);

    }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

  }
  catch (exception & x) {
    cerr << "error: " << x.what () << endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    cerr << "error: " << "unexpected error!" << endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

// end of test_logical_volume.cxx
