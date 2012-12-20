/* test_utils.cxx */

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <string>

#include <datatools/utils.h>
#include <datatools/ioutils.h>

using namespace std;

int main (void)
{
  try
    {
      {
        string path = "${HOME}/.ssh/config.${USER}";
        clog << "Original path: '" << path << "'" << endl;
        string expanded = datatools::expand_path (path);
        datatools::fetch_path_with_env (path);
        clog << "Derived expanded path : '" << expanded << "'" << endl;
        clog << "Expanded path         : '" << path << "'" << endl;
      }

      {
        string path = "file://${HOME}/.ssh/config.${USER}";
        clog << "Original path: '" << path << "'" << endl;
        string expanded = datatools::expand_path (path);
        datatools::fetch_path_with_env (path);
        clog << "Derived expanded path : '" << expanded << "'" << endl;
        clog << "Expanded path         : '" << path << "'" << endl;
      }

      {
        string path = "file://~/.ssh/config.${USER}";
        clog << "Original path: '" << path << "'" << endl;
        string expanded = datatools::expand_path (path);
        datatools::fetch_path_with_env (path);
        clog << "Derived expanded path : '" << expanded << "'" << endl;
        clog << "Expanded path         : '" << path << "'" << endl;
      }

      {
        string path = "https://nemo.lpc-caen.in2p3.fr/wiki/datatools/trunk/README.txt";
        clog << "Original path: '" << path << "'" << endl;
        string expanded = datatools::expand_path (path);
        datatools::fetch_path_with_env (path);
        clog << "Derived expanded path : '" << expanded << "'" << endl;
        clog << "Expanded path         : '" << path << "'" << endl;
      }

      {
        uint16_t s = 1;
        clog << "s = " << datatools::io::to_binary (s) << std::endl;
        datatools::set_bit (s, 3);
        datatools::set_bit (s, 7);
        datatools::set_bit (s, 8);
        clog << "s = " << datatools::io::to_binary (s) << std::endl;
        datatools::unset_bit (s, 0);
        clog << "s = " << datatools::io::to_binary (s) << std::endl;
        datatools::toggle_bit (s, 7);
        clog << "s = " << datatools::io::to_binary (s) << std::endl;
        datatools::toggle_bit (s, 15);
        clog << "s = " << datatools::io::to_binary (s) << std::endl;
        datatools::toggle_bit (s, 15);
        clog << "s = " << datatools::io::to_binary (s) << std::endl;
        clog << "s = " << datatools::io::to_binary (s, true) << std::endl;
       }

    }
  catch (exception & x)
    {
      cerr << "error: " << x.what () << endl;
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}

// end of test_utils.cxx
