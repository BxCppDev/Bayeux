// -*- mode: c++ ; -*-
// test_conical_frustrum_random_tools.cxx

#include <cstdlib>
#include <fstream>
#include <string>
#include <exception>

#include <genvtx/genvtx_config.h>
#include <genvtx/conical_frustrum_random_tools.h>
#include <geomtools/utils.h>
#include <geomtools/polyline_3d.h>
#include <mygsl/histogram_1d.h>

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
    clog << "Test program for the 'genvtx' conical_frustrum_random_tools library." << endl;

    // bool debug = false;
    // bool verbose = false;
    bool draw = false;
    int iarg = 1;
    while (iarg < argc_) {
      string token = argv_[iarg];
      if (token[0] == '-') {
        string option = token;
        // if ((option == "-d") || (option == "--debug")) {
        //   debug = true;
        // } else if ((option == "-v") || (option == "--verbose")) {
        //   verbose = true;
        // } else
        if ((option == "-D") || (option == "--draw")) {
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
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create ("/tmp", "test_trt_");

    {
      double r1 = 1.2;
      double r2 = 2.3;
      double h  = 3.5;
      double thickness = 0.05;
      double th1 = M_PI / 5;
      double th2 = 1.5 * M_PI;
      int nshoots = 10000;
      for (int i = 0; i < nshoots; i++) {
        geomtools::vector_3d vertex
          = genvtx::right_circular_conical_frustrum::random_surface(random,
                                                                    r1, r2, h,
                                                                    thickness,
                                                                    th1, th2);
        geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(),
                                                  vertex,
                                                  true);
      }
      tmp_file.out() << endl << endl;
    }

   {
      double r1 = 0.7;
      double r2 = 1.2;
      double h  = 5.0;
      double thickness = 0.05;
      double th1 = 0.0;
      double th2 = 0.75 * M_PI;
      int nshoots = 100000;
      for (int i = 0; i < nshoots; i++) {
        geomtools::vector_3d vertex
          = genvtx::right_circular_conical_frustrum::random_surface(random,
                                                                    r1, r2, h,
                                                                    thickness,
                                                                    th1, th2);
          geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(),
                                                    vertex,
                                                    true);
      }
      tmp_file.out() << endl << endl;
    }

    {
      int nbins = 70;
      double zmin = -1.0;
      double zmax = 6.0;
      double dz = (zmax-zmin)/nbins;
      mygsl::histogram_1d hz(nbins, zmin, zmax);
      double a1 = 0.4;
      double b1 = 0.5;
      double a2 = 0.2;
      double b2 = 1.0;
      double h  = 5.0;
      double thickness = 0.0;
      double th1 = 0.35 * M_PI;
      double th2 = 1.55 * M_PI;

      {
        geomtools::vector_3d pos;
        geomtools::rotation rot;
        geomtools::create_rotation (rot, 0.0, 0.0, 0.0);
        geomtools::gnuplot_draw::draw_right_circular_conical_frustrum(tmp_file.out(),
                                                                      pos, rot,
                                                                      0.0, a1, b1,
                                                                      h, a2, b2,
                                                                      th1, th2);
        tmp_file.out() << endl << endl;
      }

      int nshoots = 10000;
      for (int i = 0; i < nshoots; i++) {
        geomtools::vector_3d vertex
          = genvtx::right_circular_conical_frustrum::random_volume(random,
                                                                   a1, b1, a2, b2, h,
                                                                   thickness,
                                                                   th1, th2);
        hz.fill(vertex.z());
        geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(),
                                                  vertex,
                                                  true);
      }
      tmp_file.out() << endl << endl;
      hz.print(tmp_file.out(), 9);
      tmp_file.out() << endl << endl;

      // Plot the PDF for z :
      double A1 = b1 + a1;
      double A2 = b2 + a2;
      double B1 = b1 - a1;
      double B2 = b2 - a2;
      double u = 2 * (A2 - A1) * (B2 - B1);
      double v = 3 * (A1 * B2 + A2 * B1 - 2 * A1 * B1);
      double w = 6 * A1 * B1;
      double T = u + v + w;
      for (double z = zmin; z < zmax; z += 0.1*dz) {
        double e = z / h;
        double pdf_z = 0.0;
        if (e >= 0 && e <= 1) {
          pdf_z = M_PI * (A1 + (A2-A1) * e) * (B1 + (B2-B1) * e);
        }
        pdf_z /= (M_PI *  h * T/6);
        tmp_file.out() << z << ' ' << pdf_z * nshoots * dz << std::endl;
      }
      tmp_file.out() << endl << endl;
    }

    if (draw) {
      std::cerr << "NOTICE: Display..." << std::endl;
      {
        Gnuplot g1 ("dots");
        g1.cmd ("set grid");
        g1.cmd ("set title 'test genvtx::conical_frustrum_random_tools'");
        g1.cmd ("set size ratio -1");
        g1.cmd ("set view equal xyz");
        g1.cmd ("set xyplane at -1");
        g1.set_xrange (-3, +3).set_yrange (-3, +3).set_zrange (-3, +8);
        g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
        std::ostringstream plot_cmd;
        plot_cmd << "splot ";
        plot_cmd << "'" << tmp_file.get_filename ()
                 << "' index 0 title 'Random vertices (surface)' with dots ";
        plot_cmd << ", '" << tmp_file.get_filename ()
                 << "' index 1 title 'Random vertices (surface)' with dots ";
        plot_cmd << ", '" << tmp_file.get_filename ()
                 << "' index 3 title 'Random vertices (bulk)' with dots ";
        plot_cmd << ", '" << tmp_file.get_filename ()
                 << "' index 2 title 'Frustum' with lines ";
        g1.cmd (plot_cmd.str ());
        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (200);
      }

      {
         Gnuplot g2;
         g2.cmd ("set grid");
         g2.cmd ("set title 'test genvtx::conical_frustrum_random_tools'");
         //g2.set_xrange (-2, +8).set_yrange (-20, 500);
         std::ostringstream plot_cmd;
         plot_cmd << "plot [-1:6][-20:*]";
         plot_cmd << "'" << tmp_file.get_filename ()
                  << "' index 4 using (0.5*(($1)+($2))):3 title 'Random vertex Z' with histeps ";
         plot_cmd << ", '" << tmp_file.get_filename ()
                  << "' index 5 title 'Theoretical PDF(z)' with lines ";
          g2.cmd (plot_cmd.str ());
         g2.showonscreen (); // window output
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
