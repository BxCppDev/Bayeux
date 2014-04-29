// test_polynomial.cxx

#include <cstdlib>
#include <iostream>
#include <limits>
#include <vector>

#include <mygsl/polynomial.h>

int main (int /* argc_  */, char ** /* argv_ */)
{
  using namespace std;
  try {
    mygsl::polynomial p0 (7.5);
    mygsl::polynomial p1 (7.5, -1.4);
    mygsl::polynomial p2 (7.5, +7.24, -0.82);

    p0.print (clog, 0, true);
    p1.print (clog, 0, true);
    p2.print (clog, 1, true);

    bool test = true;
    if (test) {
      double dx = 0.1;

      for (double x = 0.0; x < 10.0; x += dx) {
        cout << x << ' ' << p0 (x) << endl;
      }
      cout << endl;
      cout << endl;

      for (double x = 0.0; x < 10.0; x += dx) {
        cout << x << ' ' << p1 (x) << endl;
      }
      cout << endl;
      cout << endl;

      for (double x = 0.0; x < 10.0; x += dx) {
        cout << x << ' ' << p2 (x) << endl;
      }
      cout << endl;
      cout << endl;

      p2.print (clog);
      clog << endl;

      vector<double> coeffs;
      coeffs.push_back (7.5);
      coeffs.push_back (-1.5);
      coeffs.push_back (1.5);
      coeffs.push_back (-2.5);
      coeffs.push_back (1.65);
      mygsl::polynomial p4 (coeffs);

      p4.print (clog, 1, true);
      clog << endl;
    }

  }
  catch (std::exception & x) {
    std::cerr << "test_polynomial: " << x.what () << std::endl;
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}

// end of test_polynomial.cxx
