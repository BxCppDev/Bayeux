// test_real_range.cxx

// Standard library:
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <string>
#include <sstream>

// Ourselves:
#include <datatools/real_range.h>
#include <datatools/clhep_units.h>

void test_in(const datatools::real_range &rr_ , double value_, double tolerance_ = 0.01)
{
  if (rr_.has(value_, tolerance_)) {
    std::clog << "Value '" << value_ << "' is IN real range '" << rr_
              << "' with precision '" << tolerance_ << "'."
              << std::endl;
  } else {
    std::clog << "Value '" << value_ << "' is OUT real range '" << rr_
              << "' with precision '" << tolerance_ << "'."
              << std::endl;
  }
  return;
}

int main (void)
{
  try {
    {
      double tolerance = 0.01;
      std::clog << "*** ri ***" << std::endl;
      datatools::real_range ri;
      ri.dump(std::clog, tolerance);

      std::clog << "*** [4.1, 5.8] ***" << std::endl;
      ri.set(4.1, 5.8);
      test_in(ri, 4.0);
      test_in(ri, 4.05);
      test_in(ri, 4.09);
      test_in(ri, 4.099);
      test_in(ri, 4.10);
      test_in(ri, 4.101);
      test_in(ri, 4.11);
      test_in(ri, 5.79);
      test_in(ri, 5.799);
      test_in(ri, 5.80);
      test_in(ri, 5.801);
      test_in(ri, 5.81);
      ri.dump(std::clog, tolerance);

      std::clog << "*** reset ***" << std::endl;
      ri.reset();
      ri.dump(std::clog, tolerance);
      std::clog << std::endl << std::endl;
    }

    {
      double tolerance = 0.01;
      std::clog << std::endl << "*** [0, 3.14] ***" << std::endl;
      datatools::real_range ri(0, 3.14) ;
      test_in(ri, -0.0001, tolerance);
      ri.dump(std::clog, tolerance);
      std::clog << std::endl << std::endl;
    }

    {
      double tolerance = 0.01;
      std::clog << std::endl << "*** [2, 2] ***" << std::endl;
      datatools::real_range ri(2, 2);
      test_in(ri, 2-0.0002, tolerance);
      test_in(ri, 2-0.0001, tolerance);
      test_in(ri, 2, tolerance);
      test_in(ri, 2+0.0001, tolerance);
      test_in(ri, 2+0.0002, tolerance);
      if (ri.is_singleton(tolerance)) {
        std::clog << "Range " << ri << " is a singleton." << std::endl;
        std::clog << "Singleton is : " << ri.singleton(tolerance) << std::endl;
        std::clog << "Width is : " << ri.width(tolerance) << std::endl;
      }
      ri.dump(std::clog, tolerance);
      std::clog << std::endl << std::endl;
    }

    {
      double tolerance = 0.01;
      std::clog << std::endl << "*** [2, 2.007] ***" << std::endl;
      datatools::real_range ri(2, 2.007);
      test_in(ri, 2-0.0002, tolerance);
      test_in(ri, 2-0.0001, tolerance);
      test_in(ri, 2, tolerance);
      test_in(ri, 2+0.0001, tolerance);
      test_in(ri, 2+0.0002, tolerance);
      test_in(ri, 2.007-0.0002, tolerance);
      test_in(ri, 2.007-0.0001, tolerance);
      test_in(ri, 2.007, tolerance);
      test_in(ri, 2.007+0.0001, tolerance);
      test_in(ri, 2.007+0.0002, tolerance);
      if (ri.is_singleton(tolerance)) {
        std::clog << "Range " << ri << " is a singleton." << std::endl;
        std::clog << "Singleton is : " << ri.singleton(tolerance) << std::endl;
        std::clog << "Width is : " << ri.width(tolerance) << std::endl;
      }
      ri.dump(std::clog, tolerance);
      std::clog << std::endl << std::endl;
    }

    {
      double tolerance = 0.01;
      std::clog << std::endl << "*** [2, 2.007) ***" << std::endl;
      datatools::real_range ri;
      ri.make_bounded(2, 2.007, true, false);
      test_in(ri, 2-0.0002, tolerance);
      test_in(ri, 2-0.0001, tolerance);
      test_in(ri, 2, tolerance);
      test_in(ri, 2+0.0001, tolerance);
      test_in(ri, 2+0.0002, tolerance);
      test_in(ri, 2.007-0.0002, tolerance);
      test_in(ri, 2.007-0.0001, tolerance);
      test_in(ri, 2.007, tolerance);
      test_in(ri, 2.007+0.0001, tolerance);
      test_in(ri, 2.007+0.0002, tolerance);
      if (ri.is_singleton(tolerance)) {
        std::clog << "Range " << ri << " is a singleton." << std::endl;
        std::clog << "Singleton is : " << ri.singleton(tolerance) << std::endl;
        std::clog << "Width is : " << ri.width(tolerance) << std::endl;
      }
      ri.dump(std::clog, tolerance);
      std::clog << std::endl << std::endl;
    }

    {
      double tolerance = 0.01;
      std::clog << std::endl << "*** (2, 2.007) ***" << std::endl;
      datatools::real_range ri;
      ri.make_bounded(2, 2.007, false, false);
      test_in(ri, 2 - 0.0002, tolerance);
      test_in(ri, 2 - 0.0001, tolerance);
      test_in(ri, 2, tolerance);
      test_in(ri, 2+0.0001, tolerance);
      test_in(ri, 2+0.0002, tolerance);
      test_in(ri, 2.007-0.0002, tolerance);
      test_in(ri, 2.007-0.0001, tolerance);
      test_in(ri, 2.007, tolerance);
      test_in(ri, 2.007+0.0001, tolerance);
      test_in(ri, 2.007+0.0002, tolerance);
      if (ri.is_singleton(tolerance)) {
        std::clog << "Range " << ri << " is a singleton." << std::endl;
        std::clog << "Singleton is : " << ri.singleton(tolerance) << std::endl;
        std::clog << "Width is : " << ri.width(tolerance) << std::endl;
      }
      ri.dump(std::clog, tolerance);
      std::clog << std::endl << std::endl;
    }

    {
      double tolerance = 0.01;
      std::clog << std::endl << "*** [2, 2.004] ***" << std::endl;
      datatools::real_range ri(2, 2.004);
      test_in(ri, 2-0.0002, tolerance);
      test_in(ri, 2-0.0001, tolerance);
      test_in(ri, 2, tolerance);
      test_in(ri, 2+0.0001, tolerance);
      test_in(ri, 2+0.0002, tolerance);
      test_in(ri, 2.004-0.0002, tolerance);
      test_in(ri, 2.004-0.0001, tolerance);
      test_in(ri, 2.004, tolerance);
      test_in(ri, 2.004+0.0001, tolerance);
      test_in(ri, 2.004+0.0002, tolerance);
      if (ri.is_singleton(tolerance)) {
        std::clog << "Range " << ri << " is a singleton." << std::endl;
        std::clog << "Singleton is : " << ri.singleton(tolerance) << std::endl;
        std::clog << "Width is : " << ri.width(tolerance) << std::endl;
      }
      ri.dump(std::clog, tolerance);
      std::clog << std::endl << std::endl;
    }

    {
      double tolerance = 0.01;
      std::clog << std::endl << "*** (2, 2.004) ***" << std::endl;
      datatools::real_range ri;
      ri.make_bounded(2, 2.004, false, false);
      test_in(ri, 2-0.0002, tolerance);
      test_in(ri, 2-0.0001, tolerance);
      test_in(ri, 2, tolerance);
      test_in(ri, 2+0.0001, tolerance);
      test_in(ri, 2+0.0002, tolerance);
      test_in(ri, 2.004-0.0002, tolerance);
      test_in(ri, 2.004-0.0001, tolerance);
      test_in(ri, 2.004, tolerance);
      test_in(ri, 2.004+0.0001, tolerance);
      test_in(ri, 2.004+0.0002, tolerance);
      if (ri.is_singleton(tolerance)) {
        std::clog << "Range " << ri << " is a singleton." << std::endl;
        std::clog << "Singleton is : " << ri.singleton(tolerance) << std::endl;
        std::clog << "Width is : " << ri.width(tolerance) << std::endl;
      }
      ri.dump(std::clog, tolerance);
      std::clog << std::endl << std::endl;
    }

    {
      double tolerance = 0.01;
      std::clog << std::endl << "*** (2, 2) ***" << std::endl;
      datatools::real_range ri;
      ri.make_bounded(2, 2, false, false);
      test_in(ri, 2-0.0002, tolerance);
      test_in(ri, 2-0.0001, tolerance);
      test_in(ri, 2, tolerance);
      test_in(ri, 2+0.0001, tolerance);
      test_in(ri, 2+0.0002, tolerance);
      if (ri.is_empty(tolerance)) {
        std::clog << "Range " << ri << " is empty." << std::endl;
      }
      if (ri.is_singleton(tolerance)) {
        std::clog << "Range " << ri << " is a singleton." << std::endl;
        std::clog << "Singleton is : " << ri.singleton(tolerance) << std::endl;
        std::clog << "Width is : " << ri.width(tolerance) << std::endl;
      }
      ri.dump(std::clog, tolerance);
      std::clog << std::endl << std::endl;
    }

    {
      double tolerance = 0.01;
      std::clog << std::endl << "*** ri(0, 5.0) ***" << std::endl;
      datatools::real_range ri(0, 5.0);
      ri.dump(std::clog, tolerance);
      std::clog << std::endl << std::endl;
    }

    {
      double tolerance = 0.01;
      std::clog << std::endl << "*** ri(0, 6, excluded, excluded) ***" << std::endl;
      datatools::real_range ri(0, 6.0, datatools::range_bound_excluded, datatools::range_bound_excluded);
      ri.dump(std::clog, tolerance);
      std::clog << std::endl << std::endl;
    }

    {
      double tolerance = 0.01;
      std::clog << std::endl << "*** make_positive_unbounded ***" << std::endl;
      datatools::real_range ri;
      ri.make_positive_unbounded();
      ri.dump(std::clog, tolerance);
      std::clog << std::endl << std::endl;
    }

    {
      double tolerance = 0.01;
      std::clog << std::endl << "*** make_negative_unbounded ***" << std::endl;
      datatools::real_range ri;
      ri.make_negative_unbounded();
      ri.dump(std::clog, tolerance);
      std::clog << std::endl << std::endl;
    }

    {
      double tolerance = 0.01;
      std::clog << std::endl << "*** make_unbounded ***" << std::endl;
      datatools::real_range ri;
      ri.make_unbounded();
      ri.dump(std::clog, tolerance);
      std::clog << std::endl << std::endl;
    }

    {
      double tolerance = 0.01;
      std::clog << std::endl << "*** lower_unbounded ***" << std::endl;
      datatools::real_range ri;
      ri.make_lower_unbounded(12);
      ri.dump(std::clog, tolerance);
      std::clog << std::endl << std::endl;
    }

    {
      double tolerance = 0.01;
      std::clog << std::endl << "*** upper_unbounded ***" << std::endl;
      datatools::real_range ri;
      ri.make_upper_unbounded(42.3);
      ri.dump(std::clog, tolerance);
      std::clog << std::endl << std::endl;
    }

    {
      double tolerance = 0.01;
      std::clog << "*** (4.1, 5.2) ***" << std::endl;
      datatools::real_range ri;
      ri.make_bounded(4.1,    /* lower bound */
                      4.5,    /* upper bound */
                      false,  /* lower bound inclusion */
                      false   /* upper bound inclusion */
                      );
      ri.dump(std::clog, tolerance);

      for (double x = 3.9; x < 4.7; x += 0.01){
        if (ri.has(x)) {
          std::clog.precision(15);
          std::clog << x << " is in ";
          std::clog.precision(15);
          std::clog << ri << std::endl;
        } else {
          std::clog.precision(15);
          std::clog << x << " is not in ";
          std::clog.precision(15);
          std::clog << ri << std::endl;
        }
      }
      std::clog << std::endl << std::endl;
    }

    {
      double tolerance = 0.01;
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
      ri.dump(std::clog, tolerance);
      std::clog << std::endl << std::endl;
    }

    {
      double tolerance = 0.01;
      std::clog << std::endl << "*** ri input 2 *** " << std::endl;
      datatools::real_range ri;
      std::clog << "Input : ";
      std::istringstream inss("[2 mm, 5 cm)");
      inss >> ri;
      if (inss) {
        std::clog << "ri = " << ri << std::endl;
      } else {
        std::clog << "Invalid format!" << std::endl;
      }
      ri.dump(std::clog, tolerance);
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** far_negative_region *** " << std::endl;
      datatools::real_range far_negative_region;
      far_negative_region.make_lower_unbounded(-50.0 * CLHEP::cm, false);
      double z = -10 * CLHEP::cm;
      /*
      std::cerr << "DEVEL: "
                << "value=" << z / CLHEP::mm << " mm   region=" << far_negative_region << "." << std::endl;
      */
      if (far_negative_region.has(z)) {
        std::cerr << "DEVEL: "
                  << z / CLHEP::mm << " mm is in " << far_negative_region << "." << std::endl;
      } else {
        std::cerr << "DEVEL: "
                  << z / CLHEP::mm << " mm is not in " << far_negative_region << "." << std::endl;
      }
      std::clog << std::endl << std::endl;
    }

  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
