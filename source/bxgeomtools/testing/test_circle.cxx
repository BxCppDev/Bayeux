// test_circle.cxx

// Ourselves:
#include <geomtools/circle.h>

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
    std::clog << "Test program for class 'geomtools::circle'!" << std::endl;

    bool draw = false;
    bool do_identity = false;
    bool do_sector = false;
    bool do_curve = true;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-D" || arg == "--draw") draw = true;
      if (arg == "-I" || arg == "--identity") do_identity = true;
      if (arg == "-S" || arg == "--sector") do_sector = true;

      iarg++;
    }

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_circle_");

    geomtools::circle my_circle;
    double radius = 1.25;
    my_circle.set_r(radius);
    if (do_sector) {
      my_circle.set_start_angle(M_PI / 7);
      my_circle.set_delta_angle(0.25 * M_PI);
    }
    my_circle.tree_dump(std::clog, "my_circle");

    geomtools::placement circle_placement(0.2, 0.15, 0.1, M_PI / 7.0, M_PI / 6.0, 0.0);
    if (do_identity) {
      circle_placement.set_identity();
    }

    {
      // Draw circle (index 0):
      tmp_file.out() << "# circle: " << std::endl;
      geomtools::gnuplot_draw::draw_circle(tmp_file.out(),
                                           circle_placement,
                                           my_circle);
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw circle (index 1):
      tmp_file.out() << "# circle (LAS):" << std::endl;
      geomtools::wires_type circle_wires;
      my_circle.generate_wires(circle_wires,
                               circle_placement,
                               geomtools::circle::WR_NONE
                               | geomtools::circle::WR_BASE_LOW_ANGLE_SAMPLING
                               );
      geomtools::gnuplot_draw::basic_draw_wires(tmp_file.out(), circle_wires);
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw circle (index 2):
      tmp_file.out() << "# circle (HAS):" << std::endl;
      geomtools::wires_type circle_wires;
      my_circle.generate_wires(circle_wires,
                               circle_placement,
                               geomtools::circle::WR_NONE
                               | geomtools::circle::WR_BASE_HIGH_ANGLE_SAMPLING
                               );
      geomtools::gnuplot_draw::basic_draw_wires(tmp_file.out(), circle_wires);
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw on curve (index 3):
      tmp_file.out() << "# on curve: " << std::endl;
      size_t  nshoots = 400000;
      int counts = 0;
      int counts2 = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d pos((-1.5 + 3.* drand48()),
                                 (-1.5 + 3.* drand48()),
                                 (-1 + 2 * drand48()));
        geomtools::vector_3d pos_c;
        circle_placement.mother_to_child(pos, pos_c);
        double tolerance = 0.25;
        if (do_curve) {
          if (my_circle.is_on_curve(pos_c, tolerance)) {
            counts++;
            geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), pos, 2.0);
          }
        }
      }
      if (counts == 0 && counts2 == 0) {
        tmp_file.out() << "0 0 0 0.0" << std::endl;
      }
      tmp_file.out() << std::endl << std::endl;
    }

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test geomtools::circle' ");
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
                 << "' index 0 title 'Circle (gnuplot draw)' with lines ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 1 title 'Circle (low sampling)' with lines ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 2 title 'Circle (high sampling)' with lines ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 0 title 'Circle (gnuplot draw)' with lines ";
        plot_cmd << ", '' index 3 using 1:2:3:4 title 'On curve' with points pt 6 ps 0.5 linecolor variable";
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
