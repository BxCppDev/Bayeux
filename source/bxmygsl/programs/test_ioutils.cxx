// test_ioutils.cxx

#include <cstdlib>
#include <iostream>
#include <limits>

#include <mygsl/ioutils.h>

int main (int argc_ , char ** argv_)
{
  try  
    {
      double x       = 3.14159;
      double nan     = numeric_limits<double>::quiet_NaN ();
      double inf_pos = numeric_limits<double>::infinity ();
      double inf_neg = -numeric_limits<double>::infinity ();
      cout << "x       == " << mygsl::odouble (x) << endl;
      cout << "nan     == " << mygsl::odouble (nan) << endl;
      cout << "inf_pos == " << mygsl::odouble (inf_pos) << endl;
      cout << "inf_neg == " << mygsl::odouble (inf_neg) << endl;


      double y;
      clog << "Enter a double precision number (examples: 3.14/" 
	   << mygsl::ioutils::NAN_STRING  << '/'
	   << mygsl::ioutils::INF_POS_STRING  << '/'
	   << mygsl::ioutils::INF_NEG_STRING  << ") : ";
      cin >> mygsl::idouble (y);
      if (! cin)
	{
	  cerr << "Format error!" << endl;
	}
      else
	{
	  cout.precision (12);
	  cout << "y       == " << mygsl::odouble (y) << endl;
	}
    }
  catch (std::exception & x)
    {
      std::cerr << "test_ioutils: " << x.what () << std::endl;
      return (EXIT_FAILURE);
    }
  return (EXIT_SUCCESS);
}

// end of test_ioutils.cxx
