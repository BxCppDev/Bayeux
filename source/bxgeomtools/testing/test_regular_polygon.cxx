// test_regular_polygon.cxx

// Ourselves:
#include <geomtools/regular_polygon.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux/datatools:
#include <datatools/temporary_files.h>
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
    std::clog << "Test program for class 'geomtools::regular_polygon'!" << std::endl;

    bool draw = false;
    bool do_identity= false;
    size_t n_sides = 6;
    bool do_surface = true;
    bool do_intercept = true;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-D" || arg == "--draw") draw = true;
      if (arg == "-I" || arg == "--identity") do_identity = true;
      if (arg == "-3") n_sides = 3;
      if (arg == "-4") n_sides = 4;
      if (arg == "-5") n_sides = 5;
      if (arg == "-6") n_sides = 6;
      if (arg == "-7") n_sides = 7;
      if (arg == "-S" || arg == "--no-surface") do_surface = false;
      if (arg == "-T" || arg == "--no-intercept") do_intercept = false;
      iarg++;
    }

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_regular_polygon_");

    geomtools::regular_polygon my_regular_polygon;
    double radius = 1.25;
    my_regular_polygon.set_n_sides(n_sides);
    my_regular_polygon.set_r(radius);
    my_regular_polygon.tree_dump(std::clog, "my_regular_polygon");

    geomtools::placement rp_placement(0.2, 0.15, 0.1, M_PI / 7.0, M_PI / 4.0, 0.0);
    if (do_identity) {
      rp_placement.set_identity();
    }
    {
      // Draw polygon (index 0):
      tmp_file.out() << "# regular polygon: " << std::endl;
      geomtools::gnuplot_draw::draw_regular_polygon(tmp_file.out(),
                                                    rp_placement,
                                                    my_regular_polygon);
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw polygon (index 1):
      tmp_file.out() << "# regular polygon " << std::endl;
      geomtools::wires_type rp_wires;
      my_regular_polygon.generate_wires(rp_wires,
                                        rp_placement,
                                        geomtools::i_wires_3d_rendering::WR_BASE_GRID
                                        // | geomtools::i_wires_3d_rendering::WR_BASE_GRID_HIGH_DENSITY
                                        | geomtools::regular_polygon::WR_RP_NO_EXTERNAL_EDGES
                                        );
      geomtools::gnuplot_draw::basic_draw_wires(tmp_file.out(), rp_wires);
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw polygon (index 2):
      tmp_file.out() << "# regular polygon " << std::endl;
      geomtools::wires_type rp_wires;
      my_regular_polygon.generate_wires(rp_wires,
                                        rp_placement,
                                        0
                                        );
      geomtools::gnuplot_draw::basic_draw_wires(tmp_file.out(), rp_wires);
      tmp_file.out() << std::endl << std::endl;
    }


    {
      // Draw vertexes (index 3):
      tmp_file.out() << "# vertexes " << std::endl;
      for (size_t i = 0; i < my_regular_polygon.get_n_sides(); i++) {
        geomtools::vector_3d vi;
        my_regular_polygon.get_vertex(i, vi);
        geomtools::gnuplot_draw::draw_vertex(tmp_file.out(),
                                             rp_placement,
                                             vi);
      }
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw on surface (index 4):
      tmp_file.out() << "# on surface: " << std::endl;
      size_t  nshoots = 10000;
      int counts = 0;
      int counts2 = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d pos((-2 + 4.* drand48()) * CLHEP::mm,
                                 (-2 + 4.* drand48()) * CLHEP::mm,
                                 (-0.1 + 0.2 * drand48()) * CLHEP::mm);
        geomtools::vector_3d pos_c;
        rp_placement.mother_to_child(pos, pos_c);
        double tolerance = 0.10 * CLHEP::mm;
        if (do_surface) {
          if (my_regular_polygon.is_on_surface(pos_c, tolerance)) {
            counts++;
            geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), pos, 4.0);
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
      // Draw intercepts (index 5):
      size_t nshoots = 10000;
      int counts = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d pos(2.* drand48() * CLHEP::mm,
                                 2.* drand48() * CLHEP::mm,
                                 (-2 + 4.* drand48()) * CLHEP::mm);
        geomtools::vector_3d dir;
        geomtools::randomize_direction (drand48, dir);

        geomtools::vector_3d pos_c;
        rp_placement.mother_to_child(pos, pos_c);
        geomtools::vector_3d dir_c;
        rp_placement.mother_to_child_direction(dir, dir_c);

        geomtools::face_intercept_info intercept_c;
        double tolerance = 0.10 * CLHEP::mm;
        if (do_intercept) {
          if (my_regular_polygon.find_intercept (pos_c, dir_c, intercept_c, tolerance)) {
            counts++;
            geomtools::vector_3d impact;
            rp_placement.child_to_mother(intercept_c.get_impact(), impact);
            geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), impact);
          }
        }
      }
      if (counts == 0) {
        tmp_file.out() << "0 0 0 -1" << std::endl;
      }
      tmp_file.out() << std::endl << std::endl;
    }

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test geomtools::regular_polygon' ");
      g1.cmd("set grid");
      g1.cmd("set size ratio -1");
      g1.cmd("set view equal xyz");
      g1.cmd("set xrange [-2:+2]");
      g1.cmd("set yrange [-2:+2]");
      g1.cmd("set zrange [-2:+2]");
      g1.cmd("set xyplane at -2");
      g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 0 title 'Regular polygon' with lines ";
        plot_cmd << ", '' index 5 title 'Intercepts' with points pt 6 ps 0.5 ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 1 title 'Regular polygon' with lines lw 1";
        plot_cmd << " , '' index 2 title 'External edges' with lines lw 3";
        plot_cmd << " , '' index 3 title 'Vertexes' with points pt 6 ps 1 lw 2";
        plot_cmd << " , '' index 4 using 1:2:3:4 title 'On surface' with points pt 6 ps 0.5 linecolor variable";
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
