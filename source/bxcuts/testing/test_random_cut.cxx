// -*- mode: c++ ; -*- 
// test_random_cut.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <cuts/random_cut.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'cuts::random_cut'!" << endl; 
  
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
    
      datatools::properties config;
      config.store("seed", 314159);
      config.store("accept_probability", 0.3);

      cuts::random_cut my_random_cut;
      my_random_cut.initialize_standalone (config);

      int max_count = 10000;
      int count = 0;
      for (int ishoot = 0; ishoot < max_count; ishoot++)
        {
          int i;
          my_random_cut.set_user_data (&i);
          int status = my_random_cut.process ();
          if (status == cuts::i_cut::ACCEPTED)
            {
              count++;
            }
        }
      clog << "Accepted count=" << count << endl;

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

// end of test_random_cut.cxx
