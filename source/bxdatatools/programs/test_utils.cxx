/* test_utils.cxx */

#include <cstdlib>
#include <stdexcept>
#include <iostream>

#include <datatools/utils/utils.h>

int 
main(void)
{
  try 
    {
      std::string path="${HOME}/.ssh/config.${USER}";
      datatools::utils::fetch_path_with_env(path);
      std::cout << path  << std::endl;

      return EXIT_SUCCESS;
    }
  catch(std::exception & x)
    {
      std::cerr << "error: " << x.what() << std::endl;
    }
  return EXIT_FAILURE;
}

// end of test_utils.cxx
