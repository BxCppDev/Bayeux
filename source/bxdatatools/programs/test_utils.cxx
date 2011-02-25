/* test_utils.cxx */

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <string>

#include <datatools/utils/utils.h>

using namespace std;

int main (void)
{
  try
    {
      string path = "${HOME}/.ssh/config.${USER}";
      string expanded = datatools::utils::expand_path (path);
      datatools::utils::fetch_path_with_env (path);
      clog << expanded << endl;
      clog << path << endl;

    }
  catch (exception & x)
    {
      cerr << "error: " << x.what () << endl;
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}

// end of test_utils.cxx
