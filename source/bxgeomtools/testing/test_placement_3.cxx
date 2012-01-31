// -*- mode: c++; -*- 
// test_placement_3.cxx

#include <cstdlib>
#include <cmath>

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

      vector_3d O1 (2, 1, 0);
      vector_3d O2 (3, 4, 0);
      placement p1 (O1, M_PI / 4, 0.0, 0.0);
      placement p2 (O2, M_PI / 2, 0.0, 0.0);
      clog << "O1O2= " << (O2 - O1) << endl;
      
      p1.tree_dump (clog, "Placement 1");
      p2.tree_dump (clog, "Placement 2");

      placement p2_1;
      p1.relocate (p2, p2_1);
      p2_1.tree_dump (clog, "Placement 2/1");

      vector_3d M2_2 (1, 0, 0);
      vector_3d N2_2 (0, 1, 0);
      vector_3d K2_2 (0, 0, 1);
      vector_3d M2_1;
      vector_3d N2_1;
      vector_3d K2_1;
      p2_1.child_to_mother (M2_2, M2_1);
      p2_1.child_to_mother (N2_2, N2_1);
      p2_1.child_to_mother (K2_2, K2_1);
      clog << "M2_1= " << M2_1 << endl;
      clog << "N2_1= " << N2_1 << endl;
      clog << "K2_1= " << K2_1 << endl;

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

// end of test_placement_3.cxx
