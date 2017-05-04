// test_line_3d.cxx

// Ourselves:
#include <geomtools/line_3d.h>

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
    std::clog << "Test program for class 'geomtools::line_3d'!" << std::endl;

    bool draw = false;
    bool do_curve = true;
    bool do_identity = false;


    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-D" || arg == "--draw") draw = true;
      if (arg == "-C" || arg == "--curve") do_curve = false;
      if (arg == "-I" || arg == "--identity") do_identity = true;
      iarg++;
    }

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_line_3d_");

    geomtools::line_3d x_axis;
    x_axis.set_first(0.0, 0.0, 0.0);
    x_axis.set_last(1.0, 0.0, 0.0);
    x_axis.tree_dump(std::clog, "X-axis :");

    geomtools::line_3d y_axis;
    y_axis.set_first(0.0, 0.0, 0.0);
    y_axis.set_last(0.0, 1.0, 0.0);
    y_axis.tree_dump(std::clog, "Y-axis :");

    geomtools::line_3d z_axis;
    z_axis.set_first(0.0, 0.0, 0.0);
    z_axis.set_last(0.0, 0.0, 1.0);
    z_axis.tree_dump(std::clog, "Z-axis :");

    geomtools::placement line_placement(0.2, 0.15, 0.1, M_PI / 7.0, M_PI / 6.0, 0.0);
    if (do_identity) {
      line_placement.set_identity();
    }
    line_placement.tree_dump(std::clog, "Line placement :");

    {
      // Draw line (index 0):
      tmp_file.out() << "# line: " << std::endl;
      geomtools::gnuplot_draw::draw_line(tmp_file.out(),
                                         line_placement,
                                         x_axis);
      tmp_file.out() << std::endl;

      // Draw line (index 1):
      geomtools::gnuplot_draw::draw_line(tmp_file.out(),
                                         line_placement,
                                         y_axis);
      tmp_file.out() << std::endl;

      // Draw line (index 2):
      geomtools::gnuplot_draw::draw_line(tmp_file.out(),
                                         line_placement,
                                         z_axis);
      tmp_file.out() << std::endl;
      tmp_file.out() << std::endl;

      geomtools::vector_3d M(1.0, 1.0, 1.0);

      // Draw M (index 3):
      tmp_file.out() << "# Projection M->H: " << std::endl;
      geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), M, 3.0);
      geomtools::vector_3d M_c;
      line_placement.mother_to_child(M, M_c);
      geomtools::vector_3d H_c;
      geomtools::line_3d::orthogonal_projection(M_c,
                                                x_axis.get_first(),
                                                x_axis.get_last() - x_axis.get_first(),
                                                H_c);
      geomtools::vector_3d H;
      line_placement.child_to_mother(H_c, H);
      geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), H, 4.0);
      tmp_file.out() << std::endl;

      tmp_file.out() << std::endl;
      double dMH = geomtools::line_3d::distance_to_line(M_c,
                                                        x_axis.get_first(),
                                                        x_axis.get_last() - x_axis.get_first());
      std::clog << "Distance MH = " << dMH << std::endl;
    }

    {
      // Draw on line (index 4):
      tmp_file.out() << "# on line: " << std::endl;
      size_t  nshoots = 200000;
      int counts = 0;
      int counts2 = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d pos((-2 + 4.* drand48()),
                                 (-2 + 4.* drand48()),
                                 (-2 + 4 * drand48()));
        geomtools::vector_3d pos_c;
        line_placement.mother_to_child(pos, pos_c);
        double tolerance = 0.25;
        if (do_curve) {
          if (x_axis.is_on_curve(pos_c, tolerance)) {
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

    {
      // Draw on curve (index 5):
      tmp_file.out() << "# on curve: " << std::endl;
      size_t  nshoots = 200000;
      int counts = 0;
      int counts2 = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d pos((-1.5 + 3.* drand48()),
                                 (-1.5 + 3.* drand48()),
                                 (-1.5 + 3.* drand48()));
        geomtools::vector_3d pos_c;
        line_placement.mother_to_child(pos, pos_c);
        double tolerance = 0.25;
        if (do_curve) {
          if (x_axis.is_on_segment(pos_c, tolerance)) {
            counts++;
            geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), pos, 3.0);
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
      g1.cmd("set title 'Test geomtools::line_3d' ");
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
                 << "' index 0 title 'X-axis' with lines ";
        plot_cmd << ", '' index 1 title 'Y-axis' with lines ";
        plot_cmd << ", '' index 2 title 'Z-axis' with lines ";
        plot_cmd << ", '' index 3 using 1:2:3:4 title 'M, H' with points pt 6 ps 1.0 linecolor variable ";
        plot_cmd << ", '' index 3 title 'MH' with lines ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 0 title 'X-axis' with lines lw 2";
        plot_cmd << ", '' index 4 using 1:2:3:4 title 'On curve' with points pt 6 ps 0.5 linecolor variable";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 0 title 'X-axis' with lines lw 2";
        plot_cmd << ", '' index 5 using 1:2:3:4 title 'On segment' with points pt 6 ps 0.5 linecolor variable";
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
