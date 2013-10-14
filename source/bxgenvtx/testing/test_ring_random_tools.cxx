// -*- mode: c++ ; -*-
// test_ring_random_tools.cxx

#include <cstdlib>
#include <fstream>
#include <string>
#include <exception>

#include <genvtx/genvtx_config.h>
#include <genvtx/ring_random_tools.h>
#include <geomtools/utils.h>
#include <geomtools/polyline_3d.h>

#include <geomtools/geomtools_config.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_draw.h>
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#include <datatools/temporary_files.h>
#include <datatools/utils.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    clog << "Test program for the 'genvtx' program library." << endl;

    bool debug = false;
    bool verbose = false;
    bool draw = false;
    int iarg = 1;
    while (iarg < argc_) {
      string token = argv_[iarg];

      if (token[0] == '-') {
        string option = token;
        if ((option == "-d") || (option == "--debug")) {
          debug = true;
        } else if ((option == "-v") || (option == "--verbose")) {
          verbose = true;
        } else if ((option == "-D") || (option == "--draw")) {
          draw = true;
        } else {
          clog << "warning: ignoring option '" << option << "'!" << endl;
        }
      } else  {
        string argument = token;
        {
          clog << "warning: ignoring argument '" << argument << "'!" << endl;
        }
      }
      iarg++;
    }

    std::string       rng_id = "mt19937";
    unsigned long int rng_seed = 12345;
    mygsl::rng random (rng_id, rng_seed);

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy (true);
    tmp_file.create ("/tmp", "test_trt_");

    {
      double r1 = 1.2;
      double r2 = 2.3;
      double thickness = 0.5;
      double th1 = M_PI/3;
      double th2 = M_PI;
      int nshoots = 10000;
      for (int i = 0; i < nshoots; i++) {
        geomtools::vector_3d vertex
          = genvtx::ring_random_surface (random, r2, r1, thickness, th1, th2);
        geomtools::gnuplot_draw::basic_draw_point (tmp_file.out (),
                                                   vertex,
                                                   true);
      }
      tmp_file.out () << endl << endl;
    }

    {
      double r1 = 1.5;
      double r2 = 2.8;
      double thickness = 0.25;
      double th1 = M_PI;
      double th2 = 1.7 * M_PI;
      int nshoots = 10000;
      for (int i = 0; i < nshoots; i++) {
        geomtools::vector_3d vertex
          = genvtx::ring_random_surface (random, r2, r1, thickness, th1, th2);
        geomtools::gnuplot_draw::basic_draw_point (tmp_file.out (),
                                                   vertex,
                                                   true);
      }
      tmp_file.out () << endl << endl;
    }

    {
      double r1 = 0.0;
      double r2 = 2.5;
      double thickness = 0.0;
      double th1 = 1.9 * M_PI;
      double th2 = 2.2 * M_PI;
      int nshoots = 10000;
      for (int i = 0; i < nshoots; i++) {
        geomtools::vector_3d vertex
          = genvtx::ring_random_surface (random, r2, r1, thickness, th1, th2);
        geomtools::gnuplot_draw::basic_draw_point (tmp_file.out (),
                                                   vertex,
                                                   true);
      }
      tmp_file.out () << endl << endl;
    }

    if (draw) {
      Gnuplot g1 ("dots");
      {
        g1.cmd ("set grid");
        g1.cmd ("set title 'test ring_random_tools'");
        g1.cmd ("set size ratio -1");
        g1.cmd ("set view equal xyz");
        g1.cmd ("set xyplane at -3");
        g1.set_xrange (-3, +3).set_yrange (-3, +3).set_zrange (-3, +3);
        g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
        std::ostringstream plot_cmd;
        plot_cmd << "splot ";
        plot_cmd << "'" << tmp_file.get_filename ()
                 << "' index 0 title 'Random vertices' with dots ";
        plot_cmd << ", '" << tmp_file.get_filename ()
                 << "' index 1 title 'Random vertices' with dots ";
        plot_cmd << ", '" << tmp_file.get_filename ()
                 << "' index 2 title 'Random vertices' with dots ";
        g1.cmd (plot_cmd.str ());

        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (200);

      }
    }

    clog << "The end." << endl;
  }
  catch (exception & x) {
    cerr << "error: " << x.what () << endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    cerr << "error: " << "unexpected error !" << endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

// end of test_ring_random_tools.cxx
