// test_interval.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <cmath>

#include <mygsl/interval.h>

using namespace std;

int main (int argc_ , char ** argv_)
{
  try 
    {
      mygsl::interval i1 (2.0,  mygsl::interval::included, 
			  4.25, mygsl::interval::included, 
			  0.001);
      mygsl::interval i2 (M_PI / 3., mygsl::interval::excluded, 
			  3. * M_PI, mygsl::interval::included, 
			  0.001);
      mygsl::interval i3 = mygsl::interval::make_no_limit ();
      mygsl::interval i4 = mygsl::interval::make_min (-1.0);
      mygsl::interval i5 = mygsl::interval::make_max (+1.0, mygsl::interval::excluded);
      mygsl::interval i6 = mygsl::interval::make_min_max_excluded (-1.0, +1.0);
      mygsl::interval i7 = mygsl::interval::make_min_max_included (-M_PI / 2., M_E * 1.25);
      mygsl::interval i8 = mygsl::interval::make_empty ();

      clog.precision (6);
      clog << "i1 == " << i1 << endl;
      clog << "i2 == " << i2 << endl;
      clog << "i3 == " << i3 << endl;
      clog << "i4 == " << i4 << endl;
      clog.precision (12);
      clog << "i5 == " << i5 << endl;
      clog << "i6 == " << i6 << endl;
      clog << "i7 == " << i7 << endl;
      clog << "i8 == " << i8 << endl;

      {
	mygsl::interval i;
	cin >> i;
	if (cin)
	  {
	    clog << "input i == " << i << endl;
	  }
	else
	  {
	    cerr << "format error!" << endl;
	  }
      }

      {
	mygsl::interval ibad = mygsl::interval::make_min_max_included (+M_PI / 2., -M_E * 1.25);
      }

      {
	mygsl::interval ibad = mygsl::interval::make_min_max_included (-M_PI / 2., +M_E * 1.25, 1000.);
      }

    }
  catch (exception & x) 
    {
      cerr << "ERROR: " << x.what () << endl;
      return (EXIT_FAILURE);
    }
  return (EXIT_SUCCESS);
}

// end of test_interval.cxx
