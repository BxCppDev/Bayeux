// test_real_range.cxx

// Standard library:
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <string>
#include <sstream>

// Ourselves:
#include <datatools/real_range.h>

int main (void)
{
  try {
    {
      std::clog << "*** ri ***" << std::endl;
      datatools::real_range ri;
      ri.dump (std::clog);

      std::clog << "*** set (4.1, 45.8) ***" << std::endl;
      ri.set (4.1, 45.8);
      ri.dump (std::clog);

      std::clog << "*** reset ***" << std::endl;
      ri.reset ();
      ri.dump (std::clog);
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** ri (3.14) ***" << std::endl;
      datatools::real_range ri (0, 3.14) ;
      ri.dump (std::clog);
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** ri (0) ***" << std::endl;
      datatools::real_range ri (0, 0);
      ri.dump (std::clog);
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** ri (0, 5.0) ***" << std::endl;
      datatools::real_range ri (0, 5.0);
      ri.dump (std::clog);
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** ri (0, 6, excluded) ***" << std::endl;
      datatools::real_range ri (0, 6.0, datatools::range_bound_excluded);
      ri.dump (std::clog);
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** full_positive ***" << std::endl;
      datatools::real_range ri;
      ri.make_full_positive ();
      ri.dump (std::clog);
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** full_positive ***" << std::endl;
      datatools::real_range ri;
      ri.make_full_negative ();
      ri.dump (std::clog);
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** full ***" << std::endl;
      datatools::real_range ri;
      ri.make_full ();
      ri.dump (std::clog);
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** lower_unbounded ***" << std::endl;
      datatools::real_range ri;
      ri.make_lower_unbounded (12);
      ri.dump (std::clog);
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** upper_unbounded ***" << std::endl;
      datatools::real_range ri;
      ri.make_upper_unbounded (42.3);
      ri.dump (std::clog);
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << "*** set (4.1, 15.2) ***" << std::endl;
      datatools::real_range ri;
      ri.make_bounded (4.1,     /* lower bound */
                       15.2,    /* upper bound */
                       false, /* lower bound inclusion */
                       false); /* upper bound inclusion */
      ri.dump (std::clog);

      for (double x = 3.9; x < 15.5; x += 0.1){
        if (ri.has (x)) {
          std::clog.precision (15);
          std::clog << x << " is in ";
          std::clog.precision (15);
          std::clog << ri << std::endl;
        }
      }
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** ri input *** " << std::endl;
      datatools::real_range ri;
      std::clog << "Input : ";
      std::istringstream inss("[2,5)");
      inss >> ri;
      if (inss) {
        std::clog << "ri = " << ri << std::endl;
      } else {
        std::clog << "Invalid format!" << std::endl;
      }
      ri.dump(std::clog);
      std::clog << std::endl << std::endl;
    }

  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
