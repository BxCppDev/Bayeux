/* test_utils.cxx */

#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <string>

#include <datatools/utils/utils.h>

int main (void)
{
  try 
    {
      std::string path = "${HOME}/.ssh/config.${USER}";
      std::string expanded = datatools::utils::expand_path (path);
      datatools::utils::fetch_path_with_env (path);
      std::clog << expanded << std::endl;
      std::clog << path << std::endl;
    }
  catch (std::exception & x)
    {
      std::cerr << "error: " << x.what () << std::endl;
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}

// end of test_utils.cxx
