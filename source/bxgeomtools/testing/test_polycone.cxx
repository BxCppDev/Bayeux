// test_polycone.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <geomtools/geomtools_config.h>
#include <geomtools/polycone.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
#include <datatools/temporary_files.h>
#include <datatools/utils.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    clog << "Test program for class 'polycone'!" << endl;

    bool draw = false;
    bool outer = false;
    bool inner = false;
    bool top = false;
    bool bottom = false;
    bool bulk = false;

    int iarg = 1;
    while (iarg < argc_) {
      string token = argv_[iarg];

      if (token[0] == '-') {
        string arg = token;
        if (arg == "-D" || arg == "--no-draw") draw = true;
        if (arg == "-i" || arg == "--inner") inner = true;
        if (arg == "-o" || arg == "--outer") outer = true;
        if (arg == "-b" || arg == "--bottom") bottom = true;
        if (arg == "-t" || arg == "--top") top = true;
        if (arg == "-B" || arg == "--bulk") bulk = true;
      } else {
        string argument = token;
        {
          clog << "warning: ignoring argument '" << argument << "'!" << endl;
        }
      }
      iarg++;
    }

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy (true);
    tmp_file.set_remove_at_destroy (false);
    tmp_file.create ("/tmp", "test_polycone_");

    {
      double pmt_radius  = 100. * CLHEP::mm;
      double pmt_front_a = 74. * CLHEP::mm;
      double pmt_back_a  = 80. * CLHEP::mm;
      double pmt_base_radius = 50. * CLHEP::mm;
      double pmt_length  = 240. * CLHEP::mm;

      geomtools::polycone my_polycone;

      double a = pmt_front_a;
      double b = pmt_radius;
      double dtheta = M_PI / 50.;
      my_polycone.add (a, 0.0);
      for (double theta = dtheta;
           theta < 0.5 * M_PI - 0.1 * dtheta;
           theta += dtheta) {
        double z = b / sqrt (pow (tan (theta), 2) + pow (b / a, 2));
        double r = b * sqrt (1.0 - (z / a) * (z / a));
        my_polycone.add (z, r);
      }
      double a2 = pmt_back_a;
      for (double theta = 0.5 * M_PI;
           theta > 0.2 * M_PI;
           theta -= dtheta) {
        double z = b / sqrt (pow (tan (theta), 2) + pow (b / a2, 2));
        double r = b * sqrt (1.0 - (z / a2) * (z / a2));
        my_polycone.add (- z, r);
      }
      my_polycone.add (-pmt_front_a, pmt_base_radius);
      my_polycone.add ((pmt_front_a - pmt_length), pmt_base_radius);

      my_polycone.tree_dump (clog, "Polycone");
      clog << "Polycone: " << my_polycone << endl;

      geomtools::vector_3d polycone_pos;
      geomtools::rotation polycone_rot;
      geomtools::create_rotation (polycone_rot, 0.0, 0.0, 0.0);
      geomtools::gnuplot_draw::draw_polycone (tmp_file.out(),
                                              polycone_pos,
                                              polycone_rot,
                                              my_polycone);
      tmp_file.out() << endl << endl;
    }

    {
      geomtools::polycone my_polycone;
      my_polycone.add(100.0, 30.0);
      my_polycone.add(150.0, 50.0);
      my_polycone.add(175.0, 20.0, 50.0);
      my_polycone.add(200.0, 20.0, 35.0);
      my_polycone.add(250.0, 15.0, 35.0);
      my_polycone.add(300.0, 15.0, 20.0);
      my_polycone.tree_dump (clog, "Polycone 2");
      clog << "Polycone 2: " << my_polycone << endl;
      clog << "Number of frustra: " << my_polycone.number_of_frustra() << endl;
      geomtools::vector_3d polycone_pos;
      geomtools::rotation polycone_rot;
      geomtools::create_rotation (polycone_rot, 0.0, 0.0, 0.0);
      geomtools::gnuplot_draw::draw_polycone (tmp_file.out(),
                                              polycone_pos,
                                              polycone_rot,
                                              my_polycone);
      tmp_file.out() << endl << endl;

      geomtools::polycone::frustrum_data fd;
      my_polycone.get_frustrum(2, fd);
      geomtools::vector_3d frustrum_pos(120.0, 0.0, 0.0);
      geomtools::gnuplot_draw::draw_right_circular_conical_frustrum(tmp_file.out(),
                                                                    frustrum_pos,
                                                                    polycone_rot,
                                                                    fd.z1, fd.a1, fd.b1,
                                                                    fd.z2, fd.a2, fd.b2,
                                                                    0.0, 2 * M_PI,
                                                                    36,
                                                                    0xB); // default=0xF
      tmp_file.out() << endl << endl;


      size_t nshoots = 1000000;
      for (int i = 0; i < (int) nshoots; i++) {
        double dim = 60. * CLHEP::mm;
        geomtools::vector_3d position (dim * ( -1 + 2 * drand48 ()),
                                       dim * ( -1 + 2 * drand48 ()),
                                       dim * (  0 + 6 * drand48 ()));

        double skin = 2.0 * CLHEP::mm;

        if (outer && my_polycone.is_on_surface(position,
                                               geomtools::polycone::FACE_OUTER_SIDE, skin)) {
          if (position.x() > 0 && position.y() < 0) {
            tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                           << 3
                           << std::endl;
          }
        } else if (inner && my_polycone.is_on_surface(position,
                                                      geomtools::polycone::FACE_INNER_SIDE, skin)) {
          if (position.x() > 0 && position.y() > 0) {
            tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                           << 4
                           << std::endl;
          }
        } else if (top && my_polycone.is_on_surface(position,
                                                    geomtools::polycone::FACE_TOP, skin)) {
          tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                         << 5
                         << std::endl;
        } else if (bottom && my_polycone.is_on_surface(position,
                                                       geomtools::polycone::FACE_BOTTOM, skin)) {
          tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                         << 6
                         << std::endl;
        } else if (bulk && my_polycone.is_inside(position, skin)) {
          if (position.x() < 0 && position.y() > 0) {
            tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                           << 2
                           << std::endl;
          }
        }
      }

      tmp_file.out() << endl << endl;

    }


    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd ("set title 'Test geomtools::polycone' ");
      g1.cmd ("set grid");
      g1.cmd ("set key out");
      g1.cmd ("set size ratio -1");
      g1.cmd ("set view equal xyz");
      g1.cmd ("set xyplane at -200");
      g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 0 notitle with lines ";
        g1.cmd (plot_cmd.str ());
        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (200);
      }

      {
        g1.cmd ("set xyplane at 100");
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 1 notitle with lines ";
         // plot_cmd << ", '' index 1 using 1:2:3:4 notitle with points pt 6 ps 0.3 linecolor variable ";
        g1.cmd (plot_cmd.str ());
        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 2 notitle with lines ";
         // plot_cmd << ", '' index 1 using 1:2:3:4 notitle with points pt 6 ps 0.3 linecolor variable ";
        g1.cmd (plot_cmd.str ());
        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (200);
      }

      {
        g1.cmd ("set zrange [50:500]");
        g1.cmd ("set xyplane at 75");
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 1 notitle with lines ";
        plot_cmd << ", '' index 3 using 1:2:3:4 notitle with points pt 6 ps 0.3 linecolor variable ";
        g1.cmd (plot_cmd.str ());
        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (200);
      }

#endif
    }

  }
  catch (exception & x) {
    cerr << "error: " << x.what () << endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    cerr << "error: " << "unexpected error!" << endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}
