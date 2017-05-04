// test_triangle.cxx

// Ourselves:
#include <geomtools/triangle.h>

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
  try{
    std::clog << "Test program for class 'geomtools::triangle'!" << std::endl;

    bool draw = false;
    bool with_placement = true;
    bool do_intercept = true;
    bool do_surface = true;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-D" || arg == "--draw") draw = true;
      if (arg == "-I" || arg == "--no-placement") with_placement = false;
      if (arg == "-i" || arg == "--no-intercept") do_intercept = false;
      if (arg == "-s" || arg == "--no-surface") do_surface = false;
     iarg++;
    }

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_triangle_");


    geomtools::triangle my_triangle;
    my_triangle.set_vertexes(geomtools::vector_3d(1.0, 0.0, 0.0),
                             geomtools::vector_3d(0.5, 1.5, 0.0),
                             geomtools::vector_3d(-1.0, 0.0, 0.0));
    my_triangle.tree_dump(std::clog, "my_triangle");

    geomtools::placement triangle_placement(0.2, 0.15, 0.1, M_PI / 7.0, M_PI / 6.0, 0.0);
    if (! with_placement) {
      triangle_placement.set_identity();
    }

    {
      // Draw triangle (index 0):
      tmp_file.out() << "# triangle: " << std::endl;
      geomtools::gnuplot_draw::draw_triangle(tmp_file.out(),
                                             triangle_placement,
                                             my_triangle);
      tmp_file.out() << std::endl << std::endl;
      std::clog << "INFO: Triangle is drawn..." << std::endl;
    }

    {
      // Draw on surface (index 1):
      tmp_file.out() << "# Points on surface: " << std::endl;
      size_t  nshoots = 10000;
      int counts = 0;
      int counts2 = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d pos((-2 + 4.* drand48()) * CLHEP::mm,
                                 (-2 + 4.* drand48()) * CLHEP::mm,
                                 (-0.1 + 0.2 * drand48()) * CLHEP::mm);
        geomtools::vector_3d pos_c;
        triangle_placement.mother_to_child(pos, pos_c);
        double tolerance = 0.15 * CLHEP::mm;
        if (do_surface) {
          if (my_triangle.is_on_surface(pos_c, tolerance)) {
            counts++;
            geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), pos, 1.0);
          } else {
            geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), pos, 2.0);
            counts2++;
          }
        }
      }
      if (counts == 0 && counts2 == 0) {
        tmp_file.out() << "0 0 0 0.0" << std::endl;
      }
      tmp_file.out() << std::endl << std::endl;
      std::clog << "INFO: Points on surface are drawn..." << std::endl;
    }

    {
      // Draw intercepts (index 2):
      size_t nshoots = 100000;
      int counts = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d pos(2.* drand48() * CLHEP::mm,
                                 2.* drand48() * CLHEP::mm,
                                 (-2 + 4.* drand48()) * CLHEP::mm);
        geomtools::vector_3d dir;
        geomtools::randomize_direction (drand48, dir);

        geomtools::vector_3d pos_c;
        triangle_placement.mother_to_child(pos, pos_c);
        geomtools::vector_3d dir_c;
        triangle_placement.mother_to_child_direction(dir, dir_c);

        geomtools::face_intercept_info intercept_c;
        if (do_intercept) {
          if (my_triangle.find_intercept (pos_c, dir_c, intercept_c)) {
            counts++;
            geomtools::vector_3d impact;
            triangle_placement.child_to_mother(intercept_c.get_impact(), impact);
            geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), impact);
          }
        }
      }
      if (counts == 0) {
        tmp_file.out() << "0 0 0 -1" << std::endl;
      }
      tmp_file.out() << std::endl << std::endl;
      std::clog << "INFO: Intercepts points are drawn..." << std::endl;
    }

    {
      // Draw intercepts (index 3):
      tmp_file.out() << "# intercepts:" << std::endl;
      size_t nshoots = 1000;
      int counts = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d pos(2. * drand48() * CLHEP::mm,
                                 2. * drand48() * CLHEP::mm,
                                 0.2 * CLHEP::mm);
        geomtools::vector_3d dir;
        geomtools::randomize_direction(drand48, dir);
        dir.setZ(0.0);

        geomtools::vector_3d pos_c;
        triangle_placement.mother_to_child(pos, pos_c);
        geomtools::vector_3d dir_c;
        triangle_placement.mother_to_child_direction(dir, dir_c);
        geomtools::face_intercept_info intercept_c;
        if (do_intercept) {
          if (my_triangle.find_intercept(pos_c, dir_c, intercept_c)) {
            counts++;
            geomtools::vector_3d impact;
            triangle_placement.child_to_mother(intercept_c.get_impact(), impact);
            geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), impact);
          }
        }
      }
      if (counts == 0) {
        tmp_file.out() << "0 0 0 -1" << std::endl;
      }
      tmp_file.out() << std::endl << std::endl;
      std::clog << "INFO: More intercepts points are drawn..." << std::endl;
    }

    {
      // Draw triangle (index 4):
      tmp_file.out() << "# triangle (grid, low):" << std::endl;
      geomtools::wires_type triangle_wires;
      my_triangle.generate_wires(triangle_wires,
                                 triangle_placement,
                                 geomtools::i_wires_3d_rendering::WR_NONE
                                 | geomtools::i_wires_3d_rendering::WR_BASE_GRID
                                 | geomtools::i_wires_3d_rendering::WR_BASE_GRID_LOW_DENSITY
                                 | geomtools::i_wires_3d_rendering::WR_BASE_LOW_ANGLE_SAMPLING
                                 );
      geomtools::gnuplot_draw::basic_draw_wires(tmp_file.out(), triangle_wires);
      tmp_file.out() << std::endl << std::endl;
      std::clog << "INFO: Triangle is drawn again..." << std::endl;
    }

    {
      // Draw triangle (index 5):
      tmp_file.out() << "# triangle (grid, normal):" << std::endl;
      geomtools::wires_type triangle_wires;
      my_triangle.generate_wires(triangle_wires,
                                 triangle_placement,
                                 geomtools::i_wires_3d_rendering::WR_NONE
                                 | geomtools::i_wires_3d_rendering::WR_BASE_GRID
                                 );
      geomtools::gnuplot_draw::draw_wires(tmp_file.out(), triangle_wires);
      tmp_file.out() << std::endl << std::endl;
      std::clog << "INFO: Triangle is drawn again (2)..." << std::endl;
    }

    {
      // Draw triangle (index 6):
      tmp_file.out() << "# triangle (grid, high):" << std::endl;
      geomtools::wires_type triangle_wires;
      my_triangle.generate_wires(triangle_wires,
                                 triangle_placement,
                                 geomtools::i_wires_3d_rendering::WR_NONE
                                 | geomtools::i_wires_3d_rendering::WR_BASE_GRID
                                 | geomtools::i_wires_3d_rendering::WR_BASE_GRID_HIGH_DENSITY
                                 | geomtools::triangle::WR_TRIANGLE_NO_FIRST_EDGE
                                 );
      geomtools::gnuplot_draw::draw_wires(tmp_file.out(), triangle_wires);
      tmp_file.out() << std::endl << std::endl;
      std::clog << "INFO: Triangle is drawn again (3)..." << std::endl;
    }

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test geomtools::triangle' ");
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
                 << "' index 0 title 'Triangle (gnuplot)' with lines ";
        plot_cmd << ", '' index 1 using 1:2:3:4 title 'On surface' with points pt 6 ps 0.5 linecolor variable";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 0 title 'Triangle (gnuplot)' with lines ";
        plot_cmd << ", '' index 2 title 'Intercepts 1' with points pt 6 ps 0.5 ";
        plot_cmd << ", '' index 3 title 'Horizontal intercepts 2' with points pt 6 ps 0.5 ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 4 title 'Triangle (grid, low)' with lines lw 2";
        plot_cmd << ", '' index 5 title 'Triangle (grid, normal)' with lines lw 1 ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 6 title 'Triangle (grid, high)' with lines lw 1";
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
