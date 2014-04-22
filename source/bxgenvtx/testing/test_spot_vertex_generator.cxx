// -*- mode: c++ ; -*-
// test_spot_vertex_generator.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <genvtx/spot_vertex_generator.h>
#include <mygsl/rng.h>

using namespace std;

int main (int /* argc_ */, char ** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'spot_vertex_generator'!" << endl;

      /*
      bool debug = false;

      int iarg = 1;
      while (iarg < argc_)
        {
          string token = argv_[iarg];

          if (token[0] == '-')
            {
               string option = token;
               if ((option == "-d") || (option == "--debug"))
                 {
                   debug = true;
                 }
               else
                 {
                    clog << "warning: ignoring option '" << option << "'!" << endl;
                 }
            }
          else
            {
              string argument = token;
               {
                clog << "warning: ignoring argument '" << argument << "'!" << endl;
              }
            }
          iarg++;
      }

      */

      std::string       rng_id = "mt19937";
      unsigned long int rng_seed = 12345;
      mygsl::rng random (rng_id, rng_seed);

      genvtx::spot_vertex_generator vg (1., 2., -3.);

      size_t nshoots = 3;
      geomtools::vector_3d vertex;
      for (int i = 0; i < (int) nshoots; i++)
        {
          vg.shoot_vertex (random, vertex);
          cout << vertex << endl;
        }

    }
  catch (exception & x)
    {
      cerr << "error: " << x.what () << endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error!" << endl;
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}
