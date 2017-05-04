// test_best_value.cxx

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

#include <mygsl/best_value.h>

using namespace std;

int main(int /* argc_ */, char ** /* argv_ */)
{
  try {
      mygsl::best_value bv1 (M_E, 0.1, 0.953);
      mygsl::best_value bv2 (M_PI, 0.05, 0.12, 0.67);

      clog.precision (10);
      clog << "bv1 : " << bv1 << endl;
      clog.precision (3);
      clog << "bv2 : " << bv2 << endl;

      double a = 2 * bv1 + bv2;

      clog << "a   : " << a << endl;

    }
  catch(exception & x) {
      cerr << "ERROR: " << x.what () << endl;
      return (EXIT_FAILURE);
    }
  return (EXIT_SUCCESS);
}

// end of test_best_value.cxx
