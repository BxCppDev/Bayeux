// -*- mode: c++; -*-
// test_placement.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <geomtools/placement.h>

using namespace std;

int main (int /* argc_ */, char ** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      {
        clog << endl << "Test 0: \n";
        geomtools::vector_3d pos;
        pos.set (1., 2., 3.);
        cout << "Vector: " << pos << endl;

        double phi   = 0.0 * CLHEP::degree;
        double theta = 90.0 * CLHEP::degree;
        double delta = 0.0 * CLHEP::degree;

        geomtools::placement p (pos, phi, theta, delta);
        p.tree_dump (clog, "Placement", ">>> ");


        string s1 = " 3 4 5 (mm) @ 90 30 0 (deg)";
        geomtools::placement p1;
        geomtools::placement::from_string (s1, p1);
        p1.tree_dump (clog, "P1 = ", ">>> ");

        string s2 = " 3 4 5 (m)";
        geomtools::placement p2;
        geomtools::placement::from_string (s2, p2);
        p2.tree_dump (clog, "P2 = ", ">>> ");
      }

      {
        clog << endl << "Test 1(a): \n";
        // 2011-04-29 FM : new test :
        geomtools::placement p;
        p.set_translation (0, 0, 0);
        p.set_orientation (geomtools::ROTATION_AXIS_Y,
                           get_special_rotation_angle (geomtools::ROTATION_ANGLE_180));

        //p.set_orientation (geomtools::ROTATION_AXIS_Y, 30 * CLHEP::degree);
        //p.set_orientation (geomtools::ROTATION_AXIS_X, 30 * CLHEP::degree);
        //p.set_orientation (geomtools::ROTATION_AXIS_Z, 30 * CLHEP::degree);
        p.tree_dump (clog, "P = ", ">>> ");
        clog << "phi   = " << p.get_phi ()   / CLHEP::degree << " °\n";
        clog << "theta = " << p.get_theta () / CLHEP::degree << " °\n";
        clog << "delta = " << p.get_delta () / CLHEP::degree << " °\n";

        if (p.is_simple_rotation ())
          {
            clog << "Extracting ZYZ Euler angles..." << endl;
            double phi, theta, delta;
            geomtools::extract_zyz_euler_angle_from_rotation (p.get_rotation (),
                                                              phi, theta, delta);
            clog << "phi   = " << phi   / CLHEP::degree << " °\n";
            clog << "theta = " << theta / CLHEP::degree << " °\n";
            clog << "delta = " << delta / CLHEP::degree << " °\n";

            geomtools::placement p2;
            p2.set_translation (0, 0, 0);
            p2.set_orientation (phi, theta, delta);
            p2.tree_dump (clog, "P2 = ", ">>> ");
          }
      }

      {
        clog << endl << "Test 1(b): \n";
        // 2011-04-29 FM : new test :
        geomtools::placement p;
        p.set_translation (0, 0, 0);
        p.set_orientation (geomtools::ROTATION_AXIS_Z, 30 * CLHEP::degree);
        p.tree_dump (clog, "P = ", ">>> ");
        clog << "phi   = " << p.get_phi ()   / CLHEP::degree << " °\n";
        clog << "theta = " << p.get_theta () / CLHEP::degree << " °\n";
        clog << "delta = " << p.get_delta () / CLHEP::degree << " °\n";

        if (p.is_simple_rotation ())
          {
            clog << "Extracting ZYZ Euler angles..." << endl;
            double phi, theta, delta;
            geomtools::extract_zyz_euler_angle_from_rotation (p.get_rotation (),
                                                              phi, theta, delta);
            clog << "phi   = " << phi   / CLHEP::degree << " °\n";
            clog << "theta = " << theta / CLHEP::degree << " °\n";
            clog << "delta = " << delta / CLHEP::degree << " °\n";

            geomtools::placement p2;
            p2.set_translation (0, 0, 0);
            p2.set_orientation (phi, theta, delta);
            p2.tree_dump (clog, "P2 = ", ">>> ");
          }
      }

      {
        clog << endl << "Test 1(c): \n";
        // 2011-04-29 FM : new test :
        geomtools::placement p;
        p.set_translation (0, 0, 0);
        p.set_orientation (geomtools::ROTATION_AXIS_X, 67 * CLHEP::degree);
        p.tree_dump (clog, "P = ", ">>> ");
        clog << "phi   = " << p.get_phi ()   / CLHEP::degree << " °\n";
        clog << "theta = " << p.get_theta () / CLHEP::degree << " °\n";
        clog << "delta = " << p.get_delta () / CLHEP::degree << " °\n";

        if (p.is_simple_rotation ())
          {
            clog << "Extracting ZYZ Euler angles..." << endl;
            double phi, theta, delta;
            geomtools::extract_zyz_euler_angle_from_rotation (p.get_rotation (),
                                                              phi, theta, delta);
            clog << "phi   = " << phi   / CLHEP::degree << " °\n";
            clog << "theta = " << theta / CLHEP::degree << " °\n";
            clog << "delta = " << delta / CLHEP::degree << " °\n";

            geomtools::placement p2;
            p2.set_translation (0, 0, 0);
            p2.set_orientation (phi, theta, delta);
            p2.tree_dump (clog, "P2 = ", ">>> ");
          }
      }

      {
        clog << endl << "Test 2: \n";
        // 2011-04-29 FM : new test :
        geomtools::placement p;
        p.set_translation (0, 0, 0);

        double phi   = 30.0 * CLHEP::degree;
        double theta = 65.0 * CLHEP::degree;
        double delta = 50.0 * CLHEP::degree;
        clog << "phi   = " << phi   / CLHEP::degree << " °\n";
        clog << "theta = " << theta / CLHEP::degree << " °\n";
        clog << "delta = " << delta / CLHEP::degree << " °\n";
        p.set_orientation (phi, theta, delta);
        p.tree_dump (clog, "P = ", ">>> ");
        clog << "phi   = " << p.get_phi ()   / CLHEP::degree << " °\n";
        clog << "theta = " << p.get_theta () / CLHEP::degree << " °\n";
        clog << "delta = " << p.get_delta () / CLHEP::degree << " °\n";

        clog << "Extracting ZYZ Euler angles..." << endl;
        geomtools::extract_zyz_euler_angle_from_rotation (p.get_rotation (),
                                                          phi, theta, delta);
        clog << "phi   = " << phi   / CLHEP::degree << " °\n";
        clog << "theta = " << theta / CLHEP::degree << " °\n";
        clog << "delta = " << delta / CLHEP::degree << " °\n";

        geomtools::placement p2;
        p2.set_translation (0, 0, 0);
        p2.set_orientation (phi, theta, delta);
        p2.tree_dump (clog, "P2 = ", ">>> ");

        clog << "Extracting XYZ Euler angles..." << endl;
        double a, b, c;
        geomtools::extract_xyz_euler_angle_from_rotation (p.get_rotation (),
                                                          a, b, c);
        clog << "a = " << a   / CLHEP::degree << " °\n";
        clog << "b = " << b / CLHEP::degree << " °\n";
        clog << "c = " << c / CLHEP::degree << " °\n";

        geomtools::placement p3;
        p3.set_translation (0, 0, 0);
        p3.set_orientation_xyz (a, b, c);
        p3.tree_dump (clog, "P3 = ", ">>> ");

      }

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

// end of test_placement.cxx
