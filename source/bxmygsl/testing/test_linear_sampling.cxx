// test_linear_sampling.cxx

// Standard library:
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

// This project:
#include <mygsl/linear_sampling.h>

/** Usage:

    \code
    mygsl-test_linear_sampling  > a.data
    \endcode

    Macro gnuplot:
    \code
    set grid
    set xrange [-3:15]
    set xlabel 'index'
    set yrange [0:5]
    set ylabel 'value'
    plot 'a.data' i 0 notitle, '' i 1 notitle
    \endcode

*/

void test0();
// void test1();

int main(int /* argc_ */, char ** /* argv_ */)
{
  try {
    test0();
  } catch (std::exception & x) {
    std::cerr << "ERROR: " << x.what () << std::endl;
    return (EXIT_FAILURE);
  }
  return (EXIT_SUCCESS);
}

void test0()
{
  mygsl::linear_sampling ls;
  double abseps = 0.01;

  {
    ls.make_range(1.5, 3.5, 10, abseps);
    for (auto i = 0; i < ls.get_nsamples(); i++) {
      double x;
      mygsl::sampling::index_classification_type ic
        = ls.index_to_value(i, x);
      if (ic == mygsl::sampling::INDEX_NORMAL) {
        std::cout << i << ' ' << x << std::endl;
      }
    }
    std::cout << std::endl << std::endl;

    std::vector<double> vals = { 1.0, 1.3, 1.495, 1.5, 1.55,
                                 1.7, 1.71, 1.87, 1.91, 2.45,
                                 2.699, 2.701, 3.1,  3.14159, 3.2999,
                                 3.2, 3.4, 3.48, 3.5, 3.51, 3.52
    };
    for (const auto x : vals) {
      std::size_t index = mygsl::sampling::INVALID_INDEX;
      mygsl::sampling::index_classification_type ic = mygsl::sampling::INDEX_UNDEFINED;
      // ls.value_to_index(x, index);
      if (ls.value_to_index(x, index, ic)) {
        std::cout << index << ' ' << x << std::endl;
      } else {
        std::cout << -1  << ' ' << x << std::endl;
      }
    }
    std::cout << std::endl << std::endl;
  }
  return;
}
