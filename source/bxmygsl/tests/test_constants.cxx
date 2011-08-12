// test_constants.cxx

#include <cstdlib>
#include <iostream>
#include <cmath>

#include <mygsl/constants.h>

using namespace std;
using namespace mygsl::constants;

int main(int argc_ , char ** argv_)
{
  try 
    {
      double me_keV = 
	mksa::mass_electron 
	* pow (mksa::speed_of_light, 2) 
	/ (num::kilo * mksa::electron_volt);

      cout.precision (10);
      cout << "me.c^2 = "  << me_keV << " keV" << endl; 

    }
  catch (exception & x)
    {
      cerr << "CONSTANTS: " << x.what () << endl;
      return (EXIT_FAILURE);
    }
  return (EXIT_SUCCESS);
}

// end of test_constants.cxx
