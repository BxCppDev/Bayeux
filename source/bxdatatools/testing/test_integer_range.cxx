// test_integer_range.cxx

// Standard library:
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>

// Ourselves:
#include <datatools/integer_range.h>

int main(void)
{
  try {
    {
      std::clog << "*** ri ***" << std::endl;
      datatools::integer_range ri;
      ri.dump(std::clog);

      std::clog << "*** set(4, 45) ***" << std::endl;
      ri.set(4, 45);
      ri.dump(std::clog);

      std::clog << "*** reset ***" << std::endl;
      ri.reset();
      ri.dump(std::clog);
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** ri(3) ***" << std::endl;
      datatools::integer_range ri(0, 3) ;
      ri.dump(std::clog);
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** ri(0) ***" << std::endl;
      datatools::integer_range ri(0, 0);
      ri.dump(std::clog);
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** ri(0, 5) ***" << std::endl;
      datatools::integer_range ri(0, 5);
      ri.dump(std::clog);
      for(int32_t i = ri.begin();
          i != ri.end();
          i++) {
        std::clog << "Index = " << i << std::endl;
      }
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** ri(0, 6, excluded) ***" << std::endl;
      datatools::integer_range ri(0, 6, datatools::range_bound_excluded);
      ri.dump(std::clog);
      for(int32_t i = ri.begin();
          i != ri.end();
          i++) {
        std::clog << "Index = " << i << std::endl;
      }
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** full_positive ***" << std::endl;
      datatools::integer_range ri;
      ri.make_full_positive();
      ri.dump(std::clog);
    }

    {
      std::clog << std::endl << "*** full_negative ***" << std::endl;
      datatools::integer_range ri;
      ri.make_full_negative();
      ri.dump(std::clog);
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** full ***" << std::endl;
      datatools::integer_range ri;
      ri.make_full();
      ri.dump(std::clog);
    }

    {
      std::clog << std::endl << "*** lower_unbounded ***" << std::endl;
      datatools::integer_range ri;
      ri.make_lower_unbounded(12);
      ri.dump(std::clog);
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** upper_unbounded ***" << std::endl;
      datatools::integer_range ri;
      ri.make_upper_unbounded(42);
      ri.dump(std::clog);
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << "*** set(4, 15) ***" << std::endl;
      datatools::integer_range ri;
      ri.make_bounded(4,     /* lower bound */
                      15,    /* upper bound */
                      false, /* lower bound exclusion */
                      true); /* upper bound exclusion */
      ri.dump(std::clog);

      for(int i = 0; i < 20; i++) {
        if(ri.has(i)) {
          std::clog << i << " is in " << ri << std::endl;
        }
      }
      std::clog << "List of integers from this range : {";
      for(int i = ri.begin(); i != ri.end(); i++) {
        if(i != ri.begin()) std::clog << ", ";
        std::clog << i;
      }
      std::clog << '}' << std::endl;
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** ri(10,15,E,I) ***" << std::endl;
      datatools::integer_range ri(10,
                                  15,
                                  datatools::range_bound_excluded,
                                  datatools::range_bound_included);
      ri.dump(std::clog);
      std::clog << "ri  = " << ri << std::endl;
      datatools::integer_range cri;
      ri.make_canonical(cri);
      std::clog << "cri = " << cri << std::endl;
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** ri(10,15,I,E) ***" << std::endl;
      datatools::integer_range ri(10,
                                  15,
                                  datatools::range_bound_included,
                                  datatools::range_bound_excluded);
      ri.dump(std::clog);
      std::clog << "ri  = " << ri << std::endl;
      datatools::integer_range cri;
      ri.make_canonical(cri);
      std::clog << "cri = " << cri << std::endl;
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** ri(10,15,E,E) ***" << std::endl;
      datatools::integer_range ri(10,
                                  15,
                                  datatools::range_bound_excluded,
                                  datatools::range_bound_excluded);
      ri.dump(std::clog);
      std::clog << "ri  = " << ri << std::endl;
      datatools::integer_range cri;
      ri.make_canonical(cri);
      std::clog << "cri = " << cri << std::endl;
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** ri(10,15,I,I) ***" << std::endl;
      datatools::integer_range ri(10,
                                  15,
                                  datatools::range_bound_included,
                                  datatools::range_bound_included);
      ri.dump(std::clog);
      std::clog << "ri  = " << ri << std::endl;
      datatools::integer_range cri;
      ri.make_canonical(cri);
      std::clog << "cri = " << cri << std::endl;
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** ri1 < ri2 ***" << std::endl;
      datatools::integer_range ri1(10,15);
      datatools::integer_range ri2(12,13);
      if (ri1 < ri2) {
        std::clog << ri1 << " < " << ri2 << std::endl;
      }
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** ri1 > ri2 ***" << std::endl;
      datatools::integer_range ri1(13,15);
      datatools::integer_range ri2(12,13);
      if (ri1 > ri2) {
        std::clog << ri1 << " > " << ri2 << std::endl;
      }
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** ri1 == ri2 ***" << std::endl;
      datatools::integer_range ri1(13,13);
      datatools::integer_range ri2(12,13, datatools::range_bound_excluded);
      if (ri1 == ri2) {
        std::clog << ri1 << " == " << ri2 << std::endl;
      }
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** ri1 < ri2 ***" << std::endl;
      datatools::integer_range ri1;
      ri1.make_lower_unbounded(20, true);
      datatools::integer_range ri2(12, 20);
      if (ri1 < ri2) {
        std::clog << ri1 << " < " << ri2 << std::endl;
      }
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** ri1 < ri2 ***" << std::endl;
      datatools::integer_range ri1;
      ri1.make_lower_unbounded(20, true);
      datatools::integer_range ri2;
      ri2.make_lower_unbounded(21, true);
      if (ri1 < ri2) {
        std::clog << ri1 << " < " << ri2 << std::endl;
      }
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** ri1 == ri2 ***" << std::endl;
      datatools::integer_range ri1;
      ri1.make_lower_unbounded(20, true);
      datatools::integer_range ri2;
      ri2.make_lower_unbounded(21, false);
      if (ri1 == ri2) {
        std::clog << ri1 << " == " << ri2 << std::endl;
      }
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** ri1 * ri2 ***" << std::endl;
      datatools::integer_range ri1(5, 15);
      datatools::integer_range ri2(10, 20);
      datatools::integer_range ri;
      if (ri1.intersect(ri2, ri)) {
        std::clog << ri1 << " * " << ri2 << " == " << ri << std::endl;
      } else {
        std::clog << ri1 << " * " << ri2 << " == " << "0" << std::endl;
      }
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** ri1 * ri2 ***" << std::endl;
      datatools::integer_range ri1(5, 15);
      datatools::integer_range ri2(16, 20);
      datatools::integer_range ri;
      if (ri1.intersect(ri2, ri)) {
        std::clog << ri1 << " * " << ri2 << " == " << ri << std::endl;
      } else {
        std::clog << ri1 << " * " << ri2 << " == " << "0" << std::endl;
      }
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** ri1 * ri2 ***" << std::endl;
      datatools::integer_range ri1;
      ri1.make_lower_unbounded(15);
      datatools::integer_range ri2(10, 20);
      datatools::integer_range ri;
      if (ri1.intersect(ri2, ri)) {
        std::clog << ri1 << " * " << ri2 << " == " << ri << std::endl;
      } else {
        std::clog << ri1 << " * " << ri2 << " == " << "0" << std::endl;
      }
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** ri1 * ri2 ***" << std::endl;
      datatools::integer_range ri1;
      ri1.make_lower_unbounded(25);
      datatools::integer_range ri2(10, 20);
      datatools::integer_range ri;
      if (ri1.intersect(ri2, ri)) {
        std::clog << ri1 << " * " << ri2 << " == " << ri << std::endl;
      } else {
        std::clog << ri1 << " * " << ri2 << " == " << "0" << std::endl;
      }
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** ri1 * ri2 ***" << std::endl;
      datatools::integer_range ri1(10, 25);
      datatools::integer_range ri2;
      ri2.make_upper_unbounded(20);
      datatools::integer_range ri;
      if (ri1.intersect(ri2, ri)) {
        std::clog << ri1 << " * " << ri2 << " == " << ri << std::endl;
      } else {
        std::clog << ri1 << " * " << ri2 << " == " << "0" << std::endl;
      }
      std::clog << std::endl << std::endl;
    }

    {
      std::clog << std::endl << "*** ri input *** " << std::endl;
      datatools::integer_range ri;
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

    {
      std::clog << std::endl << "*** inclusion *** " << std::endl;
      datatools::integer_range ri1(0, 20);
      datatools::integer_range ri2(2, 10);
      if (ri1.has(ri2)) {
        std::clog << ri2 << " in " << ri1 << std::endl;
      } else {
        std::clog << ri2 << " not in " << ri1 << std::endl;
      }
      std::clog << std::endl << std::endl;
    }

  }
  catch(std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
