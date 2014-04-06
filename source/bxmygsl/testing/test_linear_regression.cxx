// test_linear_regression.cxx

#include <cstdlib>
#include <iostream>
#include <limits>
#include <algorithm>
#include <ctime>

#include <mygsl/rng.h>
#include <mygsl/linear_regression.h>

using namespace std;

int main (int argc_ , char ** argv_)
{
  try
    {
      string rng_id = "mt19937";
      unsigned long int seed = 54231;

      time_t now;
      time (&now);
      seed = (long) now;

      mygsl::rng r (rng_id, seed);

      double a     = -0.83;
      double b     = +10.21;
      double y_err = 0.5;
      double x_min = 0.0;
      double x_max = 10.0;

      size_t npoints = 15;

      // first method:
      vector<mygsl::datapoint> points;

      // second method:
      vector<double> vx;
      vector<double> vy;
      vector<double> vy_err;

      // fill data vectors:
      for (int i = 0; i < (int) npoints; i++)
        {
          double x = r.flat (x_min, x_max);
          double y = r.gaussian (a * x + b, y_err);

          // first method:
          points.push_back (mygsl::datapoint (x, y, y_err));

          // second method:
          vx.push_back (x);
          vy.push_back (y);
          vy_err.push_back (y_err);
        }

      sort (points.begin (), points.end (), mygsl::datapoint::comp_by_x);

      // draw datapoints:
      for (int i = 0; i < (int) points.size (); i++)
        {
          cout << points[i].x () << ' '
               << points[i].y () << ' '
               << points[i].sigma_y () << endl;

        }
      cout << endl;
      cout << endl;

      mygsl::linear_regression lr (points);

      // draw unweighted fit result:
      if (lr.fit_linear ())
        {
          mygsl::linear_regression::function lr_func(lr.get_fit_data());
          double dx = 0.1;
          for (double x = min (0.0, x_min); x <= (x_max + 0.5 * dx); x += dx)
            {
              double y;
              double y_err;
              lr_func.eval_err (x, y, y_err);
              cout << x << ' '
                   << y << ' '
                   << y_err << endl;

            }
          cout << endl;
          cout << endl;

          for (int i = 0; i < (int) points.size (); i++)
            {
              double x = points[i].x ();
              double y;
              double y_err;
              lr_func.eval_err (x, y, y_err);
              cout << x << ' '
                   << y << ' '
                   << y_err << endl;
            }
          cout << endl;
          cout << endl;

          clog << "Unweighted linear least-squares fit (method 1): " << endl;
          clog << "  slope    = " << lr.get_fit_data ().get_slope ()
               << " +/- "  <<  lr.get_fit_data ().get_slope_err ()
               << endl;
          clog << "  constant = " << lr.get_fit_data ().get_constant ()
               << " +/- "  <<  lr.get_fit_data ().get_constant_err ()
               << endl;
          clog << "  sumsq    = " << lr.get_fit_data ().get_sumsq () << endl;
          clog << "  ndof     = " << lr.get_fit_data ().get_ndof () << endl;
          clog << endl;
        }
      else
        {
          cerr << "WARNING: Cannot fit a straight line (ULF)!" << endl;
        }

      // draw weighted fit result:
      if (lr.fit_weighted_linear ())
        {
          mygsl::linear_regression::function lr_func(lr.get_fit_data());

          double dx = 0.1;
          for (double x = min (0.0, x_min); x <= (x_max + 0.5 * dx); x += dx)
            {
              double y;
              double y_err;
              lr_func.eval_err (x, y, y_err);
              cout << x << ' '
                   << y << ' '
                   << y_err << endl;

            }
          cout << endl;
          cout << endl;

          for (int i = 0; i < points.size (); i++)
            {
              double x = points[i].x ();
              double y;
              double y_err;
              lr_func.eval_err (x, y, y_err);
              cout << x << ' '
                   << y << ' '
                   << y_err << endl;
            }
          cout << endl;
          cout << endl;

          clog << "Weighted linear least-squares fit (method 1): " << endl;
          clog << "  slope    = " << lr_func.get_fit_data ().get_slope ()
               << " +/- "  <<  lr_func.get_fit_data ().get_slope_err ()
               << endl;
          clog << "  constant = " << lr_func.get_fit_data ().get_constant ()
               << " +/- "  <<  lr_func.get_fit_data ().get_constant_err ()
               << endl;
          clog << "  chisq    = " << lr_func.get_fit_data ().get_chisq () << endl;
          clog << "  ndof     = " << lr_func.get_fit_data ().get_ndof () << endl;
          clog << "  chisq/ndof = " << lr_func.get_fit_data ().get_chisq () / lr.get_fit_data ().get_ndof () << endl;
          clog << endl;
        }
      else
        {
          cerr << "WARNING: Cannot fit a straight line (WLF)!" << endl;
        }

      lr.init (vx, vy, vy_err);
      // draw weighted fit result:
      if (lr.fit_weighted_linear ())
        {

          clog << "Weighted linear least-squares fit (method 2): " << endl;
          clog << "  slope    = " << lr.get_fit_data ().get_slope ()
               << " +/- "  <<  lr.get_fit_data ().get_slope_err ()
               << endl;
          clog << "  constant = " << lr.get_fit_data ().get_constant ()
               << " +/- "  <<  lr.get_fit_data ().get_constant_err ()
               << endl;
          clog << "  chisq    = " << lr.get_fit_data ().get_chisq () << endl;
          clog << "  ndof     = " << lr.get_fit_data ().get_ndof () << endl;
          clog << "  chisq/ndof = " << lr.get_fit_data ().get_chisq () / lr.get_fit_data ().get_ndof () << endl;
          clog << endl;
        }

      lr.init (vx, vy);
      // draw unweighted fit result:
      if (lr.fit_linear ())
        {
          clog << "Unweighted linear least-squares fit (method 2): " << endl;
          clog << "  slope    = " << lr.get_fit_data ().get_slope ()
               << " +/- "  <<  lr.get_fit_data ().get_slope_err ()
               << endl;
          clog << "  constant = " << lr.get_fit_data ().get_constant ()
               << " +/- "  <<  lr.get_fit_data ().get_constant_err ()
               << endl;
          clog << "  sumsq    = " << lr.get_fit_data ().get_sumsq () << endl;
          clog << "  ndof     = " << lr.get_fit_data ().get_ndof () << endl;
          clog << endl;
        }

    }
  catch (exception & x)
    {
      cerr << "ERROR: test_linear_regression: " << x.what () << endl;
      return (EXIT_FAILURE);
    }
  return (EXIT_SUCCESS);
}

// end of test_linear_regression.cxx
