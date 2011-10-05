/** demo_pba_3.cpp
 *
 * (C) Copyright 2011 François Mauger, Christian Pfligersdorffer
 *
 * Use, modification and distribution is subject to the Boost Software
 * License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 */

/**
 * The intent of this program is to serve as a tutorial for
 * users of the portable binary archive in the framework of 
 * the Boost/Serialization package. 
 *
 * It shows how to store then load floating numbers
 * (including non-finite/special values) from a portable binary archive
 * associated to a file stream.
 *
 */

#include <string>
#include <fstream>
#include <limits>

#include <boost/cstdint.hpp> // Support for portable integer types:

#include <boost/archive/portable_binary_oarchive.hpp>
#include <boost/archive/portable_binary_iarchive.hpp>

void demo_store_double ()
{
  using namespace std;
  
  cout << "\ndemo_pba_3 : demo_store_double: " << endl;
  
  // A normalized double precision number :
  double pi = 3.14159265358979;
  
  // A denormalized double precision number :
  double epsilon = 1.e-310;
  
  // A double precision number with `-Infinity' value :
  double minus_infinity = -numeric_limits<double>::infinity ();
  
  // A double precision number with `+Infinity' value :
  double plus_infinity = numeric_limits<double>::infinity ();
  
  // A double precision `Not-a-Number' (NaN):
  double nan = numeric_limits<double>::quiet_NaN ();
  
  cout.precision (15);
  cout << "\tpi             = " << pi << " (normalized)" << endl;
  cout << "\tepsilon        = " << epsilon << " (denormalized)"  << endl;
  cout << "\tminus_infinity = " << minus_infinity << " (warning: not portable print format for cout)" << endl;
  cout << "\tplus_infinity  = " << plus_infinity << " (warning: not portable print format for cout)" << endl;
  cout << "\tnan            = " << nan << " (warning: not portable print format for cout)" << endl;
  
  {
    // The name for the example data file :  
    string filename = "demo_pba_3.data";

    // Output binary file stream :
    ofstream fout (filename.c_str (), ios_base::binary);

    // An output portable binary archive attached to the output file :
    boost::archive::portable_binary_oarchive opba (fout);

    // Saving numbers...
    opba & pi & epsilon & minus_infinity & plus_infinity & nan;

    cout << "\tDouble values have been stored in '" << filename  << "'." << endl;
  }

  return;
}


void demo_load_double ()
{
  using namespace std;
  
  cout << "\ndemo_pba_3 : demo_load_double: " << endl;
  
  // Double precision numbers :
  double dpf[5];
    
  {
    // The name for the example data file :  
    string filename = "demo_pba_3.data";

    // Input binary file stream :
    ifstream fin (filename.c_str (), ios_base::binary);

    // An input portable binary archive attached to the input file :
    boost::archive::portable_binary_iarchive ipba (fin);

    // Loading numbers...
    for (int i = 0; i < 5; ++i)
      {
	ipba & dpf[i];
      }

    cout << "\tDouble values have been loaded from '" << filename  << "'." << endl;
  }
  
  // Printing numbers...
  for (int i = 0; i < 5; ++i)
    {
      cout.precision (15);
      cout << "\tvalue[" << i << "] = " << dpf[i];
      switch (fp::fpclassify(dpf[i]))
	{
	case FP_NAN: cout << " (NaN; warning: not portable print format for cout)"; break;
	case FP_INFINITE: cout << " (infinite; warning: not portable print format for cout)"; break;
	case FP_SUBNORMAL: cout << " (denormalized)"; break;
	case FP_NORMAL:  cout << " (normalized)"; break;
	}
      cout << endl;
    }

  return;
}


int main (void)
{
  // run the demo function for serialization of double-precision floating numbers :
  demo_store_double ();

  // run the demo function for deserialization of double-precision floating numbers :
  demo_load_double ();
  
  return 0;   
}

// end of demo_pba_3.cpp
