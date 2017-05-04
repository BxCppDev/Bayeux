// -*- mode: c++; -*-
// test_rotation_3d.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <geomtools/utils.h>

using namespace std;

int main (int /* argc_ */, char ** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try
  {

    clog << "Hello, this is a sample program for class 'rotation_3d'!" << endl;

    // bool debug = false;

    // int iarg = 1;
    // while (iarg < argc_)
    // {

    //   string arg = argv_[iarg];

    //   if (arg == "-d" || arg == "--debug") debug = true;

    //   iarg++;
    // }

    geomtools::rotation_3d r;
    double phi = 30. * CLHEP::degree;
    double theta = 45. * CLHEP::degree;
    double delta = 0. * CLHEP::degree;
    geomtools::create_rotation_3d (r, phi, theta, delta);

    clog << "phi   = " << r.phi () / CLHEP::degree << " °" << endl;
    clog << "theta = " << r.theta () /CLHEP::degree << " °" << endl;
    clog << "delta = " << r.delta () /CLHEP::degree << " °" << endl;

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

// end of test_dummy_geomtools.cxx
