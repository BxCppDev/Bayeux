// test_simple_polygon.cxx

// Ourselves:
#include <geomtools/simple_polygon.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux/datatools:
#include <datatools/temporary_files.h>
#include <datatools/properties.h>
#include <datatools/utils.h>

// This project:
#include <geomtools/geomtools_config.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'geomtools::simple_polygon'!" << std::endl;

    bool draw = false;
    bool do_identity= false;
    bool do_surface = true;
    bool do_intercept = true;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-D" || arg == "--draw") draw = true;
      if (arg == "-I" || arg == "--identity") do_identity = true;
      if (arg == "-S" || arg == "--no-surface") do_surface = false;
      if (arg == "-T" || arg == "--no-intercept") do_intercept = false;
      iarg++;
    }

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_simple_polygon_");

    std::cerr << "Polygon #1 :" << std::endl;
    geomtools::simple_polygon my_simple_polygon;
    my_simple_polygon.set_build_mode(geomtools::simple_polygon::BUILD_MODE_BY_VERTICES);
    my_simple_polygon.add_vertex(0.0, 1.0);
    my_simple_polygon.add_vertex(1.0, 3.0);
    my_simple_polygon.add_vertex(3.0, 3.0);
    my_simple_polygon.add_vertex(4.0, 1.0);

    my_simple_polygon.add_vertex(2.5, 1.0); // good
    // my_simple_polygon.add_vertex(1.0, 4.0); // bad
    // my_simple_polygon.add_vertex(2.0, 3.0); // bad
    // my_simple_polygon.add_vertex(2.1, 2.9); // good

    my_simple_polygon.add_vertex(2.5,-2.0);

    my_simple_polygon.add_vertex(0.0,-2.0); // good
    // my_simple_polygon.add_vertex(7.0, 2.5); // bad

    my_simple_polygon.initialize_simple();
    my_simple_polygon.tree_dump(std::clog, "my_simple_polygon");

    std::cerr << "Polygon #2 :" << std::endl;
    geomtools::simple_polygon my_simple_polygon2;
    datatools::properties msp2_config;
    std::string msp2_config_file = "${GEOMTOOLS_TESTING_DIR}/config/test_simple_polygon.conf";
    datatools::fetch_path_with_env(msp2_config_file);
    datatools::properties::read_config(msp2_config_file, msp2_config);
    my_simple_polygon2.initialize(msp2_config);
    my_simple_polygon2.tree_dump(std::clog, "my_simple_polygon2");

    std::cerr << "Polygon #3 :" << std::endl;
    geomtools::simple_polygon my_simple_polygon3;
    my_simple_polygon3.set_build_mode(geomtools::simple_polygon::BUILD_MODE_BY_SEGMENTS);
    std::cerr << "  Adding the first wall segment..." << std::endl;
    my_simple_polygon3.add_wall_segment(geomtools::vector_2d(0.0, 0.0),
                                        geomtools::vector_2d(1.0, 2.0),
                                        0.5, 0.25,
                                        -45.0 * CLHEP::degree);
    std::cerr << "  Adding the second wall segment..." << std::endl;
    my_simple_polygon3.add_wall_segment(geomtools::vector_2d(0.0, 2.5),
                                        0.25, 0.50);
    std::cerr << "  Adding the third wall segment..." << std::endl;
    my_simple_polygon3.add_wall_segment(geomtools::vector_2d(-1.0, 2.0),
                                        0.25, 0.15);
    std::cerr << "  Adding the fourth wall segment..." << std::endl;
    my_simple_polygon3.add_wall_segment(geomtools::vector_2d(-1.0, 1.0),
                                        0.50, 0.25);
    std::cerr << "  Adding the fifth and last wall segment..." << std::endl;
    my_simple_polygon3.add_wall_segment(geomtools::vector_2d(-3.0, 1.0),
                                        0.15, 0.15,
                                        30.0 * CLHEP::degree);
    std::cerr << "  Initializing..." << std::endl;
    my_simple_polygon3.initialize_simple();
    std::cerr << "  Done." << std::endl;
    my_simple_polygon3.tree_dump(std::clog, "my_simple_polygon3");

    geomtools::placement sp_placement(-1.0, -1.0, +1.0,
                                      M_PI / 7.0, M_PI / 4.0, M_PI / 4.0);
    geomtools::placement sp2_placement(0.0, 0.0, +2.0,
                                       30.0 * CLHEP::degree, 0.0, 0.0);
    geomtools::placement sp3_placement(0.0, 0.0, +4.0,
                                       0.0 * CLHEP::degree, 0.0, 0.0);
    if (do_identity) {
      sp_placement.set_identity();
    }

     {
      // Draw polygon (index 0):
      tmp_file.out() << "# simple polygon " << std::endl;
      geomtools::wires_type sp_wires;
      my_simple_polygon.generate_wires(sp_wires,
                                       sp_placement,
                                       geomtools::i_wires_3d_rendering::WR_NONE
                                       | geomtools::i_wires_3d_rendering::WR_BASE_GRID
                                       // | geomtools::i_wires_3d_rendering::WR_BASE_GRID_HIGH_DENSITY
                                       // | geomtools::simple_polygon::WR_SP_INTERNAL_EDGES
                                       );
      geomtools::gnuplot_draw::basic_draw_wires(tmp_file.out(), sp_wires);
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw vertexes (index 1):
      tmp_file.out() << "# vertexes " << std::endl;
      for (size_t i = 0; i < my_simple_polygon.get_number_of_vertices(); i++) {
        geomtools::vector_3d vi = my_simple_polygon.get_vertex(i);
        geomtools::gnuplot_draw::draw_vertex(tmp_file.out(),
                                             sp_placement,
                                             vi);
      }
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw on surface (index 2):
      tmp_file.out() << "# on surface: " << std::endl;
      size_t  nshoots = 100000;
      int counts = 0;
      int counts2 = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d pos((-4 + 8.* drand48()) * CLHEP::mm,
                                 (-4 + 8.* drand48()) * CLHEP::mm,
                                 (-0.2 + 0.4 * drand48()) * CLHEP::mm);
        geomtools::vector_3d pos_c;
        sp_placement.mother_to_child(pos, pos_c);
        double tolerance = 0.20 * CLHEP::mm;
        if (do_surface) {
          if (my_simple_polygon.is_on_surface(pos_c, tolerance)) {
            geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), pos, 4.0);
            counts++;
          } else {
            // geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), pos, 1.0);
            // counts2++;
          }
        }
      }
      if (counts == 0 && counts2 == 0) {
        tmp_file.out() << "0 0 0 0.0" << std::endl;
      }
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw intercepts (index 3):
      size_t nshoots = 10000;
      int counts = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d pos(0.* drand48() * CLHEP::mm,
                                 0.* drand48() * CLHEP::mm,
                                 (4 + 0.* drand48()) * CLHEP::mm);
        geomtools::vector_3d dir;
        geomtools::randomize_direction(drand48, dir);

        geomtools::vector_3d pos_c;
        sp_placement.mother_to_child(pos, pos_c);
        geomtools::vector_3d dir_c;
        sp_placement.mother_to_child_direction(dir, dir_c);

        geomtools::face_intercept_info intercept_c;
        double tolerance = 0.10 * CLHEP::mm;
        if (do_intercept) {
          if (my_simple_polygon.find_intercept (pos_c, dir_c, intercept_c, tolerance)) {
            counts++;
            geomtools::vector_3d impact;
            sp_placement.child_to_mother(intercept_c.get_impact(), impact);
            geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), impact);
          }
        }
      }
      if (counts == 0) {
        tmp_file.out() << "0 0 0 -1" << std::endl;
      }
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw polygon (index 4):
      tmp_file.out() << "# simple polygon 2 " << std::endl;
      geomtools::wires_type sp2_wires;
      my_simple_polygon2.generate_wires(sp2_wires,
                                       sp2_placement,
                                       geomtools::i_wires_3d_rendering::WR_NONE
                                        // | geomtools::i_wires_3d_rendering::WR_BASE_GRID
                                        // | geomtools::i_wires_3d_rendering::WR_BASE_GRID_HIGH_DENSITY
                                        | geomtools::simple_polygon::WR_SP_INTERNAL_EDGES
                                       );
      geomtools::gnuplot_draw::basic_draw_wires(tmp_file.out(), sp2_wires);
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw vertexes (index 5):
      tmp_file.out() << "# vertexes 2" << std::endl;
      for (size_t i = 0; i < my_simple_polygon2.get_number_of_vertices(); i++) {
        geomtools::vector_3d vi = my_simple_polygon2.get_vertex(i);
        geomtools::gnuplot_draw::draw_vertex(tmp_file.out(),
                                             sp2_placement,
                                             vi);
      }
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw intercepts 2 (index 6):
      size_t nshoots = 10000;
      int counts = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d pos(0.* drand48() * CLHEP::mm,
                                 0.* drand48() * CLHEP::mm,
                                 (4 + 0.* drand48()) * CLHEP::mm);
        geomtools::vector_3d dir;
        geomtools::randomize_direction(drand48, dir);

        geomtools::vector_3d pos_c;
        sp2_placement.mother_to_child(pos, pos_c);
        geomtools::vector_3d dir_c;
        sp2_placement.mother_to_child_direction(dir, dir_c);

        geomtools::face_intercept_info intercept_c;
        double tolerance = 0.10 * CLHEP::mm;
        if (do_intercept) {
          if (my_simple_polygon2.find_intercept (pos_c, dir_c, intercept_c, tolerance)) {
            counts++;
            geomtools::vector_3d impact;
            sp2_placement.child_to_mother(intercept_c.get_impact(), impact);
            geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), impact);
          }
        }
      }
      if (counts == 0) {
        tmp_file.out() << "0 0 0 -1" << std::endl;
      }
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw polygon 3 (index 7):
      tmp_file.out() << "# simple polygon 2 " << std::endl;
      geomtools::wires_type sp3_wires;
      my_simple_polygon3.generate_wires(sp3_wires,
                                        sp3_placement,
                                        geomtools::i_wires_3d_rendering::WR_NONE
                                        // | geomtools::i_wires_3d_rendering::WR_BASE_GRID
                                        // | geomtools::i_wires_3d_rendering::WR_BASE_GRID_HIGH_DENSITY
                                        | geomtools::simple_polygon::WR_SP_INTERNAL_EDGES
                                        );
      geomtools::gnuplot_draw::basic_draw_wires(tmp_file.out(), sp3_wires);
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw polygon 3 (index 8):
      tmp_file.out() << "# simple polygon 3 " << std::endl;
      geomtools::gnuplot_draw::draw_simple_polygon(tmp_file.out(),
                                                   // sp3_placement,
                                                   my_simple_polygon3,
                                                   geomtools::i_wires_3d_rendering::WR_BASE_GRID
                                                   | geomtools::i_wires_3d_rendering::WR_BASE_GRID_VERY_HIGH_DENSITY
                                                   );
      tmp_file.out() << std::endl << std::endl;
    }


    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test geomtools::simple_polygon' ");
      g1.cmd("set grid");
      g1.cmd("set size ratio -1");
      g1.cmd("set view equal xyz");
      g1.cmd("set xrange [-4:+4]");
      g1.cmd("set yrange [-4:+4]");
      g1.cmd("set zrange [-4:+4]");
      g1.cmd("set xyplane at -4");
      g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 0 title 'Simple polygon' with lines lt 1";
        plot_cmd << " , '' index 1 title 'Vertexes' with points lt 2 pt 6 ps 1 lw 2";
        plot_cmd << " , '' index 2 using 1:2:3:4 title 'On surface' with points pt 6 ps 0.5 linecolor variable";
        plot_cmd << " , '' index 3 title 'Intercepts' with points lt 3 pt 6 ps 0.5 ";
        plot_cmd << " , '' index 4 title 'Simple polygon 2' with lines lt 1";
        plot_cmd << " , '' index 5 title 'Vertexes 2' with points lt 2 pt 6 ps 1 lw 2";
        plot_cmd << " , '' index 6 title 'Intercepts' with points lt 3 pt 6 ps 0.5 ";
        plot_cmd << " , '' index 7 title 'Simple polygon 3' with lines lt 1";
        plot_cmd << " , '' index 8 title 'Simple polygon 3 (bis)' with lines lt 2";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    }

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
