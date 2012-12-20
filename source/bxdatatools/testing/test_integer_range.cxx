/* test_integer_range.cxx */

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <string>

#include <datatools/integer_range.h>

int main (void)
{
  using namespace std;
  using namespace datatools;
  try
    {
      {
        clog << "*** ri ***" << endl;
        datatools::integer_range ri;
        ri.dump (clog);
        
        clog << "*** set (4, 45) ***" << endl;
        ri.set (4, 45);
        ri.dump (clog);
        
        clog << "*** reset ***" << endl;
        ri.reset ();
        ri.dump (clog);
      }

      {
        clog << endl << "*** ri (3) ***" << endl;
        datatools::integer_range ri (0, 3) ;
        ri.dump (clog);
      }

      {
        clog << endl << "*** ri (0) ***" << endl;
        datatools::integer_range ri (0, 0);
        ri.dump (clog);
      }

      {
        clog << endl << "*** ri (0, 5) ***" << endl;
        datatools::integer_range ri (0, 5);
        ri.dump (clog);
        for (int32_t i = ri.begin ();
             i != ri.end ();
             i++)
          {
            clog << "Index = " << i << endl;
          }
      }

      {
        clog << endl << "*** ri (0, 6, excluded) ***" << endl;
        datatools::integer_range ri (0, 6, datatools::range_bound_excluded);
        ri.dump (clog);
        for (int32_t i = ri.begin ();
             i != ri.end ();
             i++)
          {
            clog << "Index = " << i << endl;
          }
      }

      {
        clog << endl << "*** full_positive ***" << endl;
        datatools::integer_range ri;
        ri.make_full_positive ();
        ri.dump (clog);
      }

      {
        clog << endl << "*** full_positive ***" << endl;
        datatools::integer_range ri;
        ri.make_full_negative ();
        ri.dump (clog);
      }

      {
        clog << endl << "*** full ***" << endl;
        datatools::integer_range ri;
        ri.make_full ();
        ri.dump (clog);
      }

      {
        clog << endl << "*** lower_unbounded ***" << endl;
        datatools::integer_range ri;
        ri.make_lower_unbounded (12);
        ri.dump (clog);
      }

      {
        clog << endl << "*** upper_unbounded ***" << endl;
        datatools::integer_range ri;
        ri.make_upper_unbounded (42);
        ri.dump (clog);
      }

      {
        clog << "*** set (4, 15) ***" << endl;
        datatools::integer_range ri;
        ri.make_bounded (4,     /* lower bound */
                         15,    /* upper bound */ 
                         false, /* lower bound exclusion */
                         true); /* upper bound exclusion */ 
        ri.dump (clog);
        
        for (int i = 0; i < 20; i++)
          {
            if (ri.has (i))
              {
                clog << i << " is in " << ri << endl;
              }
          }
        clog << "List of integers from this range : {";
        for (int i = ri.begin (); i != ri.end (); i++)
          {
            if (i != ri.begin ()) clog << ", ";
            clog << i;      
          }
        clog << '}' << endl;
      }
    }
  catch (exception & x)
    {
      cerr << "error: " << x.what () << endl;
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}

// end of test_integer_range.cxx

