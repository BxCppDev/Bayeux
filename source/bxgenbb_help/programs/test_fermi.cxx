// -*- mode: c++; -*- 
// test_fermi.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <CLHEP/Units/SystemOfUnits.h>
#include <genbb_help/fermi.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
  {
    clog << "Hello, this is a sample program for class 'genbb_mgr'!" 
	      << endl; 
  
    bool debug = false;

    int iarg = 1;
    while (iarg < argc_)
    {
      string arg = argv_[iarg];

      if (arg == "-d" || arg == "--debug") debug = true;

      iarg++;
    }
    
    double z = 44.;
    double a = a_from_z (z);
    clog << "z=" << z << " a=" << a << endl; 
    for (double e = 0.0 * CLHEP::MeV; 
	 e < 5. * CLHEP::MeV; 
	 e += 0.01 * CLHEP::MeV)
      {
	double e_MeV = e / CLHEP::MeV;
	double f1 = fermi_func_nr_approx (z, e_MeV);
	double f2 = fermi_func_shape_only (z, e_MeV);
	double f3 = fermi_func (z, e_MeV);
	cout << e_MeV 
	     << ' ' << f1 
	     << ' ' << f2 
	     << ' ' << f3 << endl;
      }
   
  }
  catch (exception & x)
  {
    cerr << "error: " << x.what() << endl; 
    error_code=EXIT_FAILURE;
  }
  catch (...)
  {
    cerr << "error: " << "unexpected error!" << endl; 
    error_code=EXIT_FAILURE;
  }
  return error_code;
}

// end of test_fermi.cxx
