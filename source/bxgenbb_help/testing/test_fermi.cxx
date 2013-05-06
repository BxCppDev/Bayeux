// -*- mode: c++; -*-
// test_fermi.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <CLHEP/Units/SystemOfUnits.h>
#include <genbb_help/decay0_fortran/fermi.h>

int main (int argc_, char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    clog << "Hello, this is a sample program for class 'genbb_mgr'!"
         << endl;

    bool debug = false;

    int iarg = 1;
    while (iarg < argc_) {
      string arg = argv_[iarg];

      if (arg == "-d" || arg == "--debug") debug = true;

      iarg++;
    }

    double z = 34.;
    double a = a_from_z (z);
    clog << "z=" << z << " a=" << a << endl;
    // clog << "TEST: fermi @ 1.5 MeV" << fermi_func_decay0 (z, 1.5, true) << endl;
    for (double e = 0.0 * CLHEP::MeV;
         e < 5. * CLHEP::MeV;
         e += 0.01 * CLHEP::MeV);
    for (int i = 1; i <= 121; i++) {
      double e_MeV = 0.025 * i; //e / CLHEP::MeV;
      double f1 = fermi_func_nr_approx (z, e_MeV);
      double f2 = fermi_func_shape_only (z, e_MeV);
      double f3 = fermi_func_decay0 (z, e_MeV);
      double f4 = fermi_func (z, e_MeV);
      // cout << e_MeV
      //      << ' ' << f1
      //      << ' ' << f2
      //      << ' ' << f3
      //      << ' ' << f4
      //      << endl;
      cout << e_MeV
           << ' ' << f3
           << endl;
    }

  }
  catch (exception & x) {
    cerr << "error: " << x.what() << endl;
    error_code=EXIT_FAILURE;
  }
  catch (...) {
    cerr << "error: " << "unexpected error!" << endl;
    error_code=EXIT_FAILURE;
  }
  return error_code;
}

// end of test_fermi.cxx
