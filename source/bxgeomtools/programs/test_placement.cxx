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
