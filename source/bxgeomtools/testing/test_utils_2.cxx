// -*- mode: c++; -*-
// test_utils_2.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <geomtools/geomtools_config.h>
#include <geomtools/utils.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
#include <geomtools/box.h>

#include <datatools/temporary_files.h>

using namespace std;

int main (int  /* argc_ */, char **  /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try
    {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      bool draw = false;
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
      long seed = 314159;
      srand48 (seed);
      bool interactive = false;

      using namespace geomtools;

      int rotation_axis = ROTATION_AXIS_INVALID;
      clog << "Enter rotation axis (x, y, z): ";
      string word;
      {
        string s;
        if (interactive) {
          getline (cin, s);
          istringstream iss (s);
          iss >> word;
          if (! iss)
            {
              throw runtime_error ("Invalid format for rotation axis!");
            }
        } else {
          word = "z";
        }
      }
      if (word == "x") rotation_axis = ROTATION_AXIS_X;
      else if (word == "y") rotation_axis = ROTATION_AXIS_Y;
      else if (word == "z") rotation_axis = ROTATION_AXIS_Z;
      else
        {
          throw runtime_error ("Invalid rotation axis !");
        }

      double rotation_angle = 0.0;
      clog << "Enter rotation angle (degree): ";
      {
        string s;
        if (interactive) {
          getline (cin, s);
          istringstream iss (s);
          iss >> rotation_angle;
          if (! iss)
            {
              throw runtime_error ("Invalid format for rotation angle !");
            }
        } else {
          rotation_angle = 90.0;
        }
      }
      rotation_angle *= CLHEP::degree;

      vector_3d pos;
      pos.set (0., 0., 0.);
      clog << "Vector: " << pos << endl;

      rotation rot;
      create_rotation (rot, rotation_axis, rotation_angle);
      tree_dump (rot, clog, "Rotation matrix:");

      box b (8.0, 4.0, 1.5);

      datatools::temp_file tmp_file;
      tmp_file.set_remove_at_destroy (true);
      tmp_file.create ("/tmp", ".test_utils_2_");

      gnuplot_draw::draw_box (tmp_file.out (), pos, rot, b);
      tmp_file.close ();
      usleep (200);


      {
        double start_phi = 15.0 * CLHEP::degree;
        double delta_phi = 85.0 * CLHEP::degree;
        for (double phi = -500 * CLHEP::degree;
             phi < +500 * CLHEP::degree;
             phi += 5 * CLHEP::degree) {
          bool test = angle_is_in(ceil(phi), start_phi, delta_phi);
          std::clog << "phi=" << phi / CLHEP::degree
                    << " is ";
          if (! test) {
            std::clog << "not ";
          }
          std::clog << "in [" << start_phi/CLHEP::degree
                    << ':' << (start_phi + delta_phi)/CLHEP::degree << "]"
                    << std::endl;
        }
      }

#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      if (draw)
        {
          Gnuplot g1 ("lines");
          g1.set_xrange (-10, +10).set_yrange (-10, +10).set_zrange (-10, +10);
          g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
          g1.plotfile_xyz (tmp_file.get_filename (), 1, 2, 3, "3D view");
          g1.showonscreen (); // window output
          gnuplot_drawer::wait_for_key ();
          usleep (200);
        }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
    }
  catch (exception & x)
    {
      cerr << "error: " << x.what() << endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error!" << endl;
      error_code = EXIT_FAILURE;
    }
  return error_code;
}

// end of test_utils_2.cxx
