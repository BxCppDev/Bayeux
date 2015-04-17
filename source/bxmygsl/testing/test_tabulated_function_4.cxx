// test_tabulated_function_4.cxx
/*
 * Test program for the 'tabulated_function' class
 *
 * shell$ test_tabulated_function > test_tabulated_function.data
 *
 * gnuplot> plot [0:2][0:1.2] \
 *   'test_tabulated_function.data' index 0 w dots, \
 *   'test_tabulated_function.data' index 1 w points ps 1 pt 6 lt 3, \
 *   cos(x) w lines lt 4
 */

// Ourselves:
#include <mygsl/tabulated_function.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>

int main (int /* argc_ */ , char ** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
     std::cerr << "Test program for the 'mygsl::tabulated_function' class" << std::endl;

    mygsl::tabulated_function tf;
    std::string datafile = "${MYGSL_TESTING_DIR}/config/tabfunc.data";
    datatools::fetch_path_with_env(datafile);

    tf.load_from_file(datafile);
    double dx = 0.01;
    std::cout << "# linear" << std::endl;
    for (double x = tf.x_min (); x <= tf.x_max () + 0.001 * dx; x += dx) {
      if (tf.is_valid(x)) {
        std::cout << x << ' ' << tf (x) << std::endl;
      }
    }
    std::cout << std::endl << std::endl;

    std::cout << "# cspline" << std::endl;
    tf.lock_table("cspline");
    for (double x = tf.x_min (); x <= tf.x_max () + 0.001 * dx; x += dx) {
      if (tf.is_valid(x)) {
        std::cout << x << ' ' << tf (x) << std::endl;
      }
    }
    std::cout << std::endl << std::endl;

    std::cout << "# akima" << std::endl;
    tf.lock_table("akima");
    for (double x = tf.x_min (); x <= tf.x_max () + 0.001 * dx; x += dx) {
      if (tf.is_valid(x)) {
        std::cout << x << ' ' << tf (x) << std::endl;
      }
    }
    std::cout << std::endl << std::endl;

  }
  catch (std::exception & x) {
    std::cerr << "ERROR: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "ERROR: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}
