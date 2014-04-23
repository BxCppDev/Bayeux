// test_sphere_2.cxx
//
// bash$ test_sphere_2 [-D ] [-B] [-i] [-o] [-p1] [-p2] [-t1] [-t2]

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <geomtools/geomtools_config.h>
#include <geomtools/sphere.h>
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
  try
    {
      long seed  = 314159;
      bool testa = true;
      bool draw = false;

      bool outer = false;
      bool inner = false;
      bool phi1 = false;
      bool phi2 = false;
      bool theta1 = false;
      bool theta2 = false;
      bool bulk = false;

      int iarg = 1;
      while (iarg < argc_)
        {
          string arg = argv_[iarg];

          if (arg == "-D" || arg == "--draw") draw = true;
          if (arg == "-ta" || arg == "--testa") testa = false;
          if (arg == "-t1" || arg == "--theta1") theta1 = true;
          if (arg == "-t2" || arg == "--theta2") theta2 = true;
          if (arg == "-p1" || arg == "--phi1") phi1 = true;
          if (arg == "-p2" || arg == "--phi2") phi2 = true;
          if (arg == "-i" || arg == "--inner") inner = true;
          if (arg == "-o" || arg == "--outer") outer = true;
          if (arg == "-B" || arg == "--bulk") bulk = true;

          iarg++;
        }

      srand48 (seed);

      datatools::temp_file tmp_file;
      tmp_file.set_remove_at_destroy (true);
      tmp_file.create ("/tmp", "test_sphere_2_");

      // test (a):
      if (testa)
        {
          geomtools::sphere my_sphere (1.8 * CLHEP::mm, 3.0 * CLHEP::mm);
          my_sphere.set_phi(20.0 * CLHEP::degree, 20.0 * CLHEP::degree );
          my_sphere.set_theta(30.0 * CLHEP::degree, 30.0 * CLHEP::degree );
          clog << "test 2: Sphere #2   = " << my_sphere << " " << endl;

          geomtools::vector_3d sphere_pos;
          geomtools::rotation sphere_rot;
          geomtools::create_rotation (sphere_rot, 0.0, 0.0, 0.0);
          geomtools::gnuplot_draw::draw_sphere (tmp_file.out(),
                                                sphere_pos,
                                                sphere_rot,
                                                my_sphere);
          tmp_file.out() << endl << endl;

          int nshoots = 500000;
          for (int i = 0; i < nshoots; i++) {
            double dim = 3. * CLHEP::mm;
            geomtools::vector_3d position (dim * ( 0 + 1 * drand48 ()),
                                           dim * ( 0 + 1 * drand48 ()),
                                           dim * ( 0 + 1 * drand48 ()));
            double skin = 0.2 * CLHEP::mm;
            if (outer && my_sphere.is_on_surface(position,
                                        geomtools::sphere::FACE_OUTER_SIDE, skin)) {
              tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                   << 3
                   << std::endl;
            } else if (inner && my_sphere.is_on_surface(position,
                                               geomtools::sphere::FACE_INNER_SIDE, skin)) {
              tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                   << 4
                   << std::endl;
            } else if (theta1 && my_sphere.is_on_surface(position,
                                               geomtools::sphere::FACE_START_THETA_SIDE, skin)) {
              tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                   << 5
                   << std::endl;
            } else if (theta2 && my_sphere.is_on_surface(position,
                                               geomtools::sphere::FACE_STOP_THETA_SIDE, skin)) {
              tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                   << 6
                   << std::endl;
            } else if (phi1 && my_sphere.is_on_surface(position,
                                               geomtools::sphere::FACE_START_PHI_SIDE, skin)) {
              tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                   << 7
                   << std::endl;
            } else if (phi2 && my_sphere.is_on_surface(position,
                                               geomtools::sphere::FACE_STOP_PHI_SIDE, skin)) {
              tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                   << 8
                   << std::endl;
            } else if (bulk && my_sphere.is_inside(position, skin)) {
              tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                   << 2
                   << std::endl;
            }
          } // for

       }
      if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
        Gnuplot g1;
        g1.cmd ("set title 'Test geomtools::sphere' ");
        g1.cmd ("set grid");
        g1.cmd ("set size ratio -1");
        g1.cmd ("set view equal xyz");
        //g1.cmd ("set xyplane at -10");
        g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
        {
          std::ostringstream plot_cmd;
          plot_cmd << "splot '" << tmp_file.get_filename () << "' index 0 title 'Sphere' with lines , ";
          plot_cmd << " '' index 1 using 1:2:3:4 notitle with points pt 6 ps 0.3 linecolor variable ";
          g1.cmd (plot_cmd.str ());
          g1.showonscreen (); // window output
          geomtools::gnuplot_drawer::wait_for_key ();
          usleep (200);
        }

#endif
      }

    }
  catch (exception & x)
    {
      cerr << "error: " << x.what () << endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error!" << endl;
      error_code = EXIT_FAILURE;
    }
  return error_code;
}
