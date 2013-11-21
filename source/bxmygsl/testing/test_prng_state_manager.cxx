// test_prng_state_manager.cxx

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <iomanip>
#include <string>
#include <exception>

#include <mygsl/rng.h>
#include <mygsl/prng_state_manager.h>
#include <datatools/utils.h>
#include <boost/filesystem.hpp>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'mygsl::prng_state_manager'!" << endl;

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

      mygsl::rng r1("taus2", 123456);
      mygsl::rng r2("taus2", 314159);
      mygsl::rng r3("taus2", (unsigned long int) time (0));

      std::string fpath = "/tmp/${USER}/mygsl_tests";
      std::string fstates = fpath +"/test_prng_state_manager.data";
      std::string fstates_bak = fstates + ".~backup~";

      mygsl::prng_state_manager PSM;
      PSM.set_filename (fstates);
      PSM.add_state ("r1", r1.get_internal_state_size ( ));
      PSM.add_state ("r2", r2.get_internal_state_size ( ));
      PSM.add_state ("r3", r3.get_internal_state_size ( ));
      PSM.dump (clog);

      clog << "Record some PRNG states in the dictionary : " << endl;
      r1.to_buffer (PSM.get_state ("r1").state_buffer);
      r3.to_buffer (PSM.get_state ("r3").state_buffer);
      PSM.dump (clog);
      //r2.to_buffer (PSM.get_state ("r2").state_buffer);
      //PSM.dump (clog);

      clog << "Storing the dictionary content in file '"
           << PSM.get_filename () << "'..." << endl;
      PSM.store ();

      int n1 = 2;
      int n2 = 3;
      clog << "Shoot some random values : " << endl;
      cout.width (12);
      cout << "r1" << ' ';
      cout.width (12);
      cout << "r2" << ' ';
      cout.width (12);
      cout << "r3" << endl;
      for (int i = 0; i < n1; i++) {
        cout.width (12);
        cout << r1 () << ' ';
        cout.width (12);
        cout << r2 () << ' ';
        cout.width (12);
        cout << r3 () << endl;
      }
      clog << "Record the PRNG states in the dictionary : " << endl;
      r1.to_buffer (PSM.get_state ("r1").state_buffer);
      r2.to_buffer (PSM.get_state ("r2").state_buffer);
      r3.to_buffer (PSM.get_state ("r3").state_buffer);
      PSM.dump (clog);

      cout.width (12);
      cout << "r1" << ' ';
      cout.width (12);
      cout << "r2" << ' ';
      cout.width (12);
      cout << "r3" << endl;
      for (int i = 0; i < n2; i++)
        {
          cout.width (12);
          cout << r1 () << ' ';
          cout.width (12);
          cout << r2 () << ' ';
          cout.width (12);
          cout << r3 () << endl;
        }
      clog << "Reload the previous dictionary content in file '"
           << PSM.get_filename () << "'..." << endl;
      PSM.load ();
      PSM.dump (clog);

      clog << "Reseting the PRNG with the state within the dictionary content..." << endl;
      r1.from_buffer (PSM.get_state ("r1").state_buffer);
      r2.from_buffer (PSM.get_state ("r2").state_buffer);
      r3.from_buffer (PSM.get_state ("r3").state_buffer);

      clog << "Replay some random values : " << endl
           << "  r1 and r3 give old values, r2 gives new ones" << endl;
      cout.width (12);
      cout << "r1" << ' ';
      cout.width (12);
      cout << "r2" << ' ';
      cout.width (12);
      cout << "r3" << endl;
      for (int i = 0; i < (n1 + n2); i++)
        {
          cout.width (12);
          cout << r1 () << ' ';
          cout.width (12);
          cout << r2 () << ' ';
          cout.width (12);
          cout << r3 () << endl;
        }

      {
        clog << "Remove temporary files..." << endl;
        std::string tmp = fpath;
        datatools::fetch_path_with_env(tmp);
        clog << "tmp='" << tmp << "'" << endl;
        boost::filesystem::remove_all(tmp);
      }
      clog << "The end." << endl;
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

// end of test_prng_state_manager.cxx
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
