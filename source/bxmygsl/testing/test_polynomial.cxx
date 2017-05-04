// test_polynomial.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <limits>
#include <vector>

// This project:
#include <mygsl/polynomial.h>

void test0();

int main (int /* argc_  */, char ** /* argv_ */)
{
  try {

    test0();

  }
  catch (std::exception & x) {
    std::cerr << "test_polynomial: " << x.what() << std::endl;
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}


void test0()
{
  mygsl::polynomial p0(7.5);
  mygsl::polynomial p1(7.5, -1.4);
  mygsl::polynomial p2(7.5, +7.24, -0.82);

  p0.print(std::clog, 0, true);
  p1.print(std::clog, 0, true);
  p2.print(std::clog, 1, true);

  bool test = true;
  if(test) {
    double dx = 0.1;
    for(double x = 0.0; x < 10.0; x += dx) {
      std::clog << x << ' ' << p0(x) << std::endl;
    }
    std::clog << std::endl;
    std::clog << std::endl;

    for(double x = 0.0; x < 10.0; x += dx) {
      std::clog << x << ' ' << p1(x) << std::endl;
    }
    std::clog << std::endl;
    std::clog << std::endl;

    for(double x = 0.0; x < 10.0; x += dx) {
      std::clog << x << ' ' << p2(x) << std::endl;
    }
    std::clog << std::endl;
    std::clog << std::endl;

    p2.print(std::clog);
    std::clog << std::endl;

    std::vector<double> coeffs;
    coeffs.push_back(7.5);
    coeffs.push_back(-1.5);
    coeffs.push_back(1.5);
    coeffs.push_back(-2.5);
    coeffs.push_back(1.65);
    mygsl::polynomial p4(coeffs);

    p4.print(std::clog, 1, true);
    std::clog << std::endl;
  }
  return;
}
