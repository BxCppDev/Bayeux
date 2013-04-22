// -*- mode: c++; -*-
// test_tabulated_function_2.cxx
/*
 * Test program for the 'tabulated_function' class
 *
 * shell$ test_tabulated_function_2 > test_tabulated_function_2.data
 *
 * gnuplot> plot [0:2][0:1.2] \
 *   'test_tabulated_function_2.data' index 0 w dots, \
 *   'test_tabulated_function_2.data' index 1 w points ps 1 pt 6 lt 3, \
 *   cos(x) w lines lt 4
 */

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <cmath>
#include <vector>

#include <mygsl/tabulated_function.h>
#include <mygsl/numerical_differentiation.h>

using namespace std;

int main (int argc_ , char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      bool debug = false;
      cerr << "Test program for the 'mygsl::tabulated_function' class"
                << endl;
      string mode = "cspline";
      mode = "akima";

      mygsl::tabulated_function tf (mode); // also available "linear";

      vector<double> xs;
      xs.push_back (0.0);
      xs.push_back (0.01);
      xs.push_back (0.03);
      xs.push_back (0.07);
      xs.push_back (0.11);
      xs.push_back (0.13);
      xs.push_back (0.2);
      xs.push_back (0.36);
      xs.push_back (0.524);
      xs.push_back (0.8);
      xs.push_back (1.07);
      xs.push_back (1.5);
      xs.push_back (2.0);
      //xs.push_back (2.2);
      //xs.push_back (2.3);

      // Build tabulated function f(x):
      clog << "Build tabulated function f(x): " << endl;
      for (int i = 0; i < xs.size (); i++)
        {
          tf.add_point (xs[i], exp (-xs[i]), false);
        }
      vector<double> xs2;
      xs2.push_back (2.5);
      xs2.push_back (2.8);
      xs2.push_back (3.3);
      xs2.push_back (3.5);
      for (int i = 0; i < xs2.size (); i++)
        {
          tf.add_point (xs2[i], exp ((xs2[i] - xs2[0])) - 0.7, false);
        }
      tf.lock_table ();

      if (debug)
        {
          cerr.precision (10);
          cerr << "DEBUG: min=" << tf.x_min () << endl;
          cerr << "DEBUG: max=" << tf.x_max () << endl;
        }

      // Draw it:
      {
        double dx = 0.01;
        for (double x = tf.x_min (); x <= tf.x_max () + 0.001 * dx; x += dx)
          {
            if (tf.is_valid (x))
              {
                cout << x << ' ' << tf (x) << endl;
              }
          }
        cout << endl << endl;
      }

      // Draw it:
      {
        tf.print_points (std::cout);
        std::cout << std::endl << std::endl;
      }

      // Build tabulated functions f2(x)/f3(x):
      clog << "Build tabulated function f2(x)/f3(x): " << endl;
      mygsl::tabulated_function tf2 (mode);
      mygsl::tabulated_function tf3 (mode);
      double skin = 0.1;
      for (mygsl::tabulated_function::points_map_type::const_iterator
             i = tf.points ().begin ();
           i != tf.points ().end ();
           i++)
        {
          mygsl::tabulated_function::points_map_type::const_iterator j = i;
          double xi = i->first;
          double yi = i->second;
          j++;
          double deriv;
          double dx = 0.01;
          if (i == tf.points ().begin ())
            {
              deriv = mygsl::derivative_forward (tf, xi, 0.1 * dx);
            }
          else if (j == tf.points ().end ())
            {
              deriv = mygsl::derivative_backward (tf, xi, 0.1 * dx);
            }
          else
            {
              deriv = mygsl::derivative_central (tf, xi, 0.1 * dx);
            }

          double ux = 1.0;
          double uy = deriv;
          double dyij = skin / sqrt (1 + uy * uy);
          double dxij = - dyij * uy;
          //clog << "deriv=" << deriv << " dxij=" << dxij << " dyij=" << dyij << endl;
          double xj = xi + dxij;
          double yj = yi + dyij;
          double xk = xi - dxij;
          double yk = yi - dyij;
          //clog << xi << " " << yi << " -> " << xj << " " << yj << endl;
          tf2.add_point (xj, yj, false);
          tf3.add_point (xk, yk, false);
        }
      tf2.lock_table (); // change interpolation scheme
      tf3.lock_table (); // change interpolation scheme

      // Draw it:
      {
        double dx = 0.01;
        for (double x = tf2.x_min (); x <= tf2.x_max () + 0.001 * dx; x += dx)
          {
            if (tf2.is_valid (x))
              {
                cout << x << ' ' << tf2 (x) << endl;
              }
          }
        cout << endl << endl;
      }

      // Draw it:
      {
        tf2.print_points (std::cout);
        std::cout << std::endl << std::endl;
      }

      // Draw it:
      {
        double dx = 0.01;
        for (double x = tf3.x_min (); x <= tf3.x_max () + 0.001 * dx; x += dx)
          {
            if (tf3.is_valid (x))
              {
                cout << x << ' ' << tf3 (x) << endl;
              }
          }
        cout << endl << endl;
      }

      // Draw it:
      {
        tf3.print_points (std::cout);
        std::cout << std::endl << std::endl;
      }

      // Manage bounds:
      mygsl::tabulated_function tf3bis (mode);
      for (mygsl::tabulated_function::points_map_type::const_iterator
             i = tf.points ().begin ();
           i != tf.points ().end ();
           i++)
        {
          double dx = 0.01;
          double x = i->first;
          double y;
          if (tf3.is_valid (x))
            {
              y = tf3 (x);
            }
          else
            {
              if (x < tf3.x_min ())
                {
                  double df3dx = mygsl::derivative_forward (tf3, tf3.x_min (), 0.1 * dx);
                  y = tf3 (tf3.x_min ()) + (x - tf3.x_min ()) * df3dx;
                }
              else //if (x > tf3.x_max ())
                {
                  double df3dx = mygsl::derivative_backward (tf3, tf3.x_max (), 0.1 * dx);
                  y = tf3 (tf3.x_max ()) + (tf3.x_max () - x) * df3dx;
                }
            }
          tf3bis.add_point (x, y, false);
        }
      tf3bis.lock_table (); // change interpolation scheme

      // Draw it:
      {
        double dx = 0.01;
        for (double x = tf3bis.x_min (); x <= tf3bis.x_max () + 0.001 * dx; x += dx)
          {
            if (tf3bis.is_valid (x))
              {
                cout << x << ' ' << tf3bis (x) << endl;
              }
          }
        cout << endl << endl;
      }

      // Draw points:
      {
        tf3bis.print_points (std::cout);
        std::cout << std::endl << std::endl;
      }

    }
  catch (exception & x)
    {
      cerr << "ERROR: " << x.what () << endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "ERROR: " << "unexpected error!" << endl;
      error_code = EXIT_FAILURE;
    }
  return error_code;
}

// end of test_tabulated_function_2.cxx
