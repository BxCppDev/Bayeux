// test_multi_eval.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#include <mygsl/multi_eval.h>

using namespace std;

struct hypot_function : public mygsl::multi_eval
{
  hypot_function () : mygsl::multi_eval (2)
  {
  }

protected:

  double _eval (const double * x_) const
  {
    return hypot (x_[0], x_[1]);
  }

};


int main (int argc_, char ** argv_)
{
  try
    {
      bool debug = false;

      int iarg = 1;
      while (iarg < argc_)
        {
          string arg = argv_[iarg];

          if (arg[0] == '-')
            {
              if (arg == "--debug" || arg == "-d")
                {
                  debug = true;
                }
              else
                {
                  clog << "WARNING: Ignoring option '" << arg << "'!" << endl;
                }
            }
          else
            {
              clog << "WARNING: Ignoring argument '" << arg << "'!" << endl;
            }
          iarg++;
        }

      if (debug) {}

      // create a multi-parameter function:
      hypot_function h1;

      // create a one-parameter function from a multi-parameter function:
      double params_y[2];
      params_y[0] = 3.0;
      params_y[1] = 0.0;
      mygsl::unary_eval_from_multi h2 (h1, 1, params_y);

      // create a one-parameter function from a multi-parameter function:
      double params_x[2];
      params_x[0] = 0.0;
      params_x[1] = 0.5;
      mygsl::unary_eval_from_multi h3 (h1, 0, params_x);

      for (double x = -5.0; x <= 5.0; x += 0.5)
        {
          for (double y = -3.0; y <= 3.0; y += 0.5)
            {
              cout.precision (12);
              cout << x << ' ' << y << ' '
                   << ' ' << h1.evaluate (x, y)
                   << endl;
            }
          cout << endl;
        }
      cout << endl;
      cout << endl;

      for (double y = -2.0; y <= 2.0; y += 0.25)
        {
          cout.precision (12);
          cout << h2.param(0) << ' ' << y << ' '
               << ' ' << h2 (y)
               << endl;
        }
      cout << endl;
      cout << endl;

      for (double x = -4.0; x <= 4.0; x += 0.25)
        {
          cout.precision (12);
          cout << x << ' ' << h3.param(1) << ' '
               << ' ' << h3 (x)
               << endl;
        }
      cout << endl;
      cout << endl;

      h3.param (1) = 1.5;
      for (double x = -4.0; x <= 4.0; x += 0.25)
        {
          cout.precision (12);
          cout << x << ' ' << h3.param(1) << ' '
               << ' ' << h3 (x)
               << endl;
        }
      cout << endl;
      cout << endl;

    }
  catch (exception & x)
    {
      cerr << "ERROR: " << x.what () << endl;
      exit (EXIT_FAILURE);
    }
  return (EXIT_SUCCESS);
}

// end of test_multi_eval.cxx
