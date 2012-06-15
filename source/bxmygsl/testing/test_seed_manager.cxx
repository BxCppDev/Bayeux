// test_seed_manager.cxx

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <exception>
#include <stdexcept>

#include <mygsl/seed_manager.h>
#include <mygsl/random_utils.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'mygsl::seed_manager'!" << endl; 
  
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
    
      mygsl::seed_manager SM;
      SM.add_seed ("manager",           123456);
      SM.add_seed ("primary_generator", 314159);
      SM.add_seed ("vertex_generator",  2718);
      SM.add_seed ("dummy_generator",   mygsl::random_utils::SEED_TIME);
      SM.dump (clog);

      clog << endl << "Updating..." << endl;
      SM.update_seed ("manager", mygsl::random_utils::SEED_INVALID);
      SM.add_seed ("smearing_processor", 76543);
      SM.dump (clog);

      clog << "SM = " << SM << endl;

      string line = "{a=4; b=56; c=356; d=-1; e=0; f=4321}";
      clog << "Parse a streamed expression : " << line<< endl;
      istringstream iss (line);
      mygsl::seed_manager SM2;
      iss >> SM2;
      if (! iss)
        {
          throw runtime_error ("Invalid format !");
        }
      clog << "SM2 = " << SM2 << endl;
      SM2.dump (clog);

      clog << endl << "All time seeds..." << endl;
      SM2.all_time_seeds ();
      SM2.dump (clog);

      clog << endl << "Manually set some seeds..." << endl;
      SM2.update_seed ("a", 314159);
      SM2.update_seed ("b", 999);
      SM2.update_seed ("manager", 666);
      SM2.update_seed ("manager1", 666);
      SM2.update_seed ("manager2", 666);
      SM2.update_seed ("r0", 1000);
      SM2.update_seed ("r1", 1000);
      SM2.update_seed ("r2", 1000);
      SM2.dump (clog);

      clog << endl << "Transform time seeds..." << endl;
      SM2.transform_time_seeds (true);
      SM2.dump (clog);

      clog << endl << "Make sure they are all different..." << endl;
      SM2.transform_time_seeds ();
      SM2.dump (clog);

      clog << endl << "Force some duplicated seed..." << endl;
      SM2.update_seed ("r1", 1000);
      SM2.dump (clog);

      clog << endl << "Ensure no duplicated seeds..." << endl;
      SM2.ensure_different_seeds ();
      SM2.dump (clog);

      clog << endl << "All time seeds (again)..." << endl;
      SM2.all_time_seeds ();
      SM2.dump (clog);

      clog << endl << "Ensure no duplicated seeds (again)..." << endl;
      SM2.ensure_different_seeds ();
      SM2.dump (clog);

      vector<string> labels;
      SM2.get_labels (labels);
      clog << endl << "Seed for '" << labels.front () << "' is : " << SM2.get_seed (labels.front ()) << endl;
      SM2.clear ();

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

// end of test_seed_manager.cxx
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
