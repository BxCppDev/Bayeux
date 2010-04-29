// -*- mode: c++; -*- 
// test_placement.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <geomtools/placement.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
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
