// -*- mode: c++; -*- 
// test_placement_2.cxx

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
      using namespace geomtools;
      vector_3d M1 (0.3, 0.2);
      vector_3d M2 (0.4, 0.3);

      vector_3d O1;
      O1.set (1.0, 2.0, 0.);
      cout << "O1: " << O1 << endl;
      double phi1   = 45.0 * CLHEP::degree;
      double theta1 = 0.0 * CLHEP::degree;
      double delta1  = 0.0 * CLHEP::degree;
      
      placement p1 (O1, phi1, theta1, delta1);
      p1.tree_dump (clog, "Placement 1", ">>> ");

      vector_3d O2;
      O2.set (0.5, 1.0, 0.);
      cout << "O2: " << O2 << endl;
      double phi2   = 30.0 * CLHEP::degree;
      double theta2 = 0.0 * CLHEP::degree;
      double delta2 = 0.0 * CLHEP::degree;
      
      placement p2 (O2, phi2, theta2, delta2);
      p2.tree_dump (clog, "Placement 2" , ">>> ");

      placement p2p;
      p1.child_to_mother (p2, p2p);
      p2p.tree_dump (clog, "Placement 2'" , ">>> ");
 
      vector_3d M1p;
      vector_3d M2p;
      p2.child_to_mother (M1, M1p);
      p2.child_to_mother (M2, M2p);
      vector_3d M1q;
      vector_3d M2q;
      p1.child_to_mother (M1p, M1q);
      p1.child_to_mother (M2p, M2q);
      vector_3d M1r;
      vector_3d M2r;
      p2p.child_to_mother (M1, M1r);
      p2p.child_to_mother (M2, M2r);

      cout << "M1q:    " << M1q << endl;
      cout << "-> M1r: " << M1r << endl;
      cout << "M2q:    " << M2q << endl;
      cout << "-> M2r: " << M2r << endl;
      

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

// end of test_placement_2.cxx
