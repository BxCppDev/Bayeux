// test_polyline_3d.cxx

// Ourselves:
#include <geomtools/polyline_3d.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

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
    std::clog << "Test program for class 'geomtools::polyline_3d'!" << std::endl;

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

    long seed = 314159;
    srand48 (seed);

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_polyline_3d_");

    geomtools::polyline_3d pl1(geomtools::polyline_3d::open);
    geomtools::polyline_3d pl2(geomtools::polyline_3d::open);
    geomtools::polyline_3d pl3(geomtools::polyline_3d::open);
    for (int i = 0; i < 10; i++) {
      geomtools::vector_3d p1(2.0 * i + geomtools::random_tools::random_flat(),
                              2.0 * geomtools::random_tools::random_flat(),
                              2.0 * geomtools::random_tools::random_flat());
      pl1.add(0.1 * p1);
      geomtools::vector_3d p2(geomtools::random_tools::random_flat(),
                              1.0 * i + geomtools::random_tools::random_flat(),
                              geomtools::random_tools::random_flat());
      pl2.add(0.1 * p2);
      geomtools::vector_3d p3(geomtools::random_tools::random_flat(),
                              geomtools::random_tools::random_flat(),
                              1.0 * i + geomtools::random_tools::random_flat());
      pl3.add(0.1 * p3);
    }

    geomtools::placement pl_placement(0.2, 0.15, 0.1, M_PI / 7.0, M_PI / 6.0, 0.0);
    if (do_identity) {
      pl_placement.set_identity();
    }
    pl_placement.tree_dump(std::clog, "Polyline placement :");

    {
      // Draw polyline (index 0):
      tmp_file.out() << "# polyline: " << std::endl;
      geomtools::gnuplot_draw::draw_polyline(tmp_file.out(),
                                             pl_placement,
                                             pl1);
      tmp_file.out() << std::endl;

      // Draw polyline (index 1):
      geomtools::gnuplot_draw::draw_polyline(tmp_file.out(),
                                             pl_placement,
                                             pl2);
      tmp_file.out() << std::endl;

      // Draw polyline (index 2):
      geomtools::gnuplot_draw::draw_polyline(tmp_file.out(),
                                             pl_placement,
                                             pl3);
      tmp_file.out() << std::endl;
    }

    {
      // Draw on curve (index 3):
      tmp_file.out() << "# on curve: " << std::endl;
      size_t nshoots = 400000;
      int counts = 0;
      int counts2 = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d pos((-2. + 4.* drand48()),
                                 (-2. + 4.* drand48()),
                                 (-2. + 4.* drand48()));
        geomtools::vector_3d pos_c;
        pl_placement.mother_to_child(pos, pos_c);
        double tolerance = 0.25;
        if (do_curve) {
          if (pl1.is_on_curve(pos_c, tolerance)) {
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
      g1.cmd("set title 'Test geomtools::polyline_3d' ");
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
                 << "' index 0 title 'X-track' with lines ";
        plot_cmd << ", '' index 1 title 'Y-track' with lines ";
        plot_cmd << ", '' index 2 title 'Z-track' with lines ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 0 title 'X-track' with lines ";
        plot_cmd << ", '' index 1 title 'Y-track' with lines ";
        plot_cmd << ", '' index 2 title 'Z-track' with lines ";
        plot_cmd << ", '' index 3 title 'On curve' with points pt 6 ps 0.25 ";
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
