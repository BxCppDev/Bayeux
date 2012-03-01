/* test_real_range.cxx */

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <string>

#include <datatools/utils/real_range.h>

using namespace std;
using namespace datatools::utils;

int main (void)
{
  try
    {
      {
	clog << "*** ri ***" << endl;
	datatools::utils::real_range ri;
	ri.dump (clog);
	
	clog << "*** set (4.1, 45.8) ***" << endl;
	ri.set (4.1, 45.8);
	ri.dump (clog);
	
	clog << "*** reset ***" << endl;
	ri.reset ();
	ri.dump (clog);
      }

      {
	clog << endl << "*** ri (3.14) ***" << endl;
	datatools::utils::real_range ri (0, 3.14) ;
	ri.dump (clog);
      }

      {
	clog << endl << "*** ri (0) ***" << endl;
	datatools::utils::real_range ri (0, 0);
	ri.dump (clog);
      }

      {
	clog << endl << "*** ri (0, 5.0) ***" << endl;
	datatools::utils::real_range ri (0, 5.0);
	ri.dump (clog);
      }

      {
	clog << endl << "*** ri (0, 6, excluded) ***" << endl;
	datatools::utils::real_range ri (0, 6.0, datatools::utils::range::excluded);
	ri.dump (clog);
      }

      {
	clog << endl << "*** full_positive ***" << endl;
	datatools::utils::real_range ri;
	ri.make_full_positive ();
	ri.dump (clog);
      }

      {
	clog << endl << "*** full_positive ***" << endl;
	datatools::utils::real_range ri;
	ri.make_full_negative ();
	ri.dump (clog);
      }

      {
	clog << endl << "*** full ***" << endl;
	datatools::utils::real_range ri;
	ri.make_full ();
	ri.dump (clog);
      }

      {
	clog << endl << "*** lower_unbounded ***" << endl;
	datatools::utils::real_range ri;
	ri.make_lower_unbounded (12);
	ri.dump (clog);
      }

      {
	clog << endl << "*** upper_unbounded ***" << endl;
	datatools::utils::real_range ri;
	ri.make_upper_unbounded (42.3);
	ri.dump (clog);
      }

      {
	clog << "*** set (4.1, 15.2) ***" << endl;
	datatools::utils::real_range ri;
	ri.make_bounded (4.1,     /* lower bound */
			 15.2,    /* upper bound */ 
			 false, /* lower bound inclusion */
			 false); /* upper bound inclusion */ 
	ri.dump (clog);
	
	for (double x = 3.9; x < 15.5; x += 0.1)
	  {
	    if (ri.has (x))
	      {
		clog.precision (15);
		clog << x << " is in ";
		clog.precision (15);
		clog << ri << endl;
	      }
	  }
      }
    }
  catch (exception & x)
    {
      cerr << "error: " << x.what () << endl;
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}

// end of test_real_range.cxx

