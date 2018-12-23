// test_tabulated_function.cxx
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
#include <fstream>
#include <string>
#include <stdexcept>
#include <cmath>
#include <vector>

/**
  Usage:
  @code
  $ mygsl-test_tabulated_function > a.data
  $ gnuplot
  gnuplot> set grid
  gnuplot> plot 'a.data' i 0 title "points" with points, \
                '' i 1 title "linear"  w l lw 3, \
                '' i 2 title "cspline" w l lw 3 , \
                "" i 3 title "polynomial" w l lw 3 , \
                "" i 4 title "akima" w l lw 3 , \
                "" i 5 title "test" w l lw 3 , \
                "" i 6 title "exp(-x)" w l lw 3 , \
                "" i 7 title "clone" w l lw 2
  @encode
  Usage:
  @code
 */
int main(int /* argc_ */ , char ** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
    bool debug = false;
    std::cerr << "Test program for the 'mygsl::tabulated_function' class"
              << std::endl;

    mygsl::tabulated_function tf("cspline"); // also available "linear";

    std::vector<double> xs;
    xs.push_back(0.0);
    xs.push_back(0.1);
    xs.push_back(0.3);
    xs.push_back(0.7);
    xs.push_back(1.1);
    xs.push_back(1.3);
    xs.push_back(2);
    xs.push_back(3.6);
    xs.push_back(5.24);
    xs.push_back(8);
    xs.push_back(10.7);

    for (int i = 0; i < (int) xs.size(); i++) {
      tf.add_point(xs[i],
                   std::exp(-xs[i]),
                   i == ((int)xs.size() - 1));
    }
    tf.lock_table("linear"); // change interpolation scheme

    std::clog << "Exporting to vectors:" << std::endl;
    std::vector<double> vx;
    std::vector<double> vf;
    tf.export_to_vectors(vx, vf);
    for (int i = 0; i < (int) vx.size(); i++) {
      std::clog << vx[i] << ' ' << vf[i] << std::endl;
    }
    std::clog << "End of export." << std::endl;
    
    if (debug) {
      std::cerr.precision(10);
      std::cerr << "DEBUG: min=" << tf.x_min() << std::endl;
      std::cerr << "DEBUG: max=" << tf.x_max() << std::endl;
    }

    double dx = 0.01;
    for (double x = tf.x_min(); x <= tf.x_max() + 0.001 * dx; x += dx) {
      if (tf.is_valid(x)) {
        std::cout << x << ' ' << tf(x) << std::endl;
      }
    }
    std::cout << std::endl << std::endl;

    tf.lock_table("cspline"); // change interpolation scheme
    for (double x = tf.x_min(); x <= tf.x_max() + 0.001 * dx; x += dx) {
      if (tf.is_valid(x)) {
        std::cout << x << ' ' << tf(x) << std::endl;
      }
    }
    std::cout << std::endl << std::endl;

    tf.lock_table("polynomial"); // change interpolation scheme
    for (double x = tf.x_min(); x <= tf.x_max() + 0.001 * dx; x += dx)
      {
        if (tf.is_valid(x))
          {
            std::cout << x << ' ' << tf(x) << std::endl;
          }
      }
    std::cout << std::endl << std::endl;

    tf.lock_table("akima"); // change interpolation scheme
    for (double x = tf.x_min(); x <= tf.x_max() + 0.001 * dx; x += dx) {
      if (tf.is_valid(x)) {
        std::cout << x << ' ' << tf(x) << std::endl;
      }
    }
    std::cout << std::endl << std::endl;

    tf.print_points(std::cout);
    std::cout << std::endl << std::endl;

    for (double x = tf.x_min(); x <= tf.x_max() + 0.001 * dx; x += dx) {
      if (tf.is_valid(x)) {
        std::cout << x << ' ' << std::exp(-x) << std::endl;
      }
    }
    std::cout << std::endl << std::endl;

    mygsl::tabulated_function * tf2 = tf.clone();

    for (double x = tf2->x_min(); x <= tf2->x_max() + 0.001 * dx; x += dx) {
      if (tf2->is_valid(x)) {
        std::cout << x << ' ' << tf2->eval(x) << std::endl;
      }
    }
    std::cout << std::endl << std::endl;

    delete tf2;

  } catch (std::exception & x) {
    std::cerr << "ERROR: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "ERROR: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}
