// Standard library:
#include <iostream>
#include <cstdlib>

// This project:
#include <bayeux/bayeux_config.h>
#include <bayeux/bayeux.h>
#include <bayeux/detail/bayeux_library.h>

int main( int /*argc_*/, char** /*argv_*/ )
{
  int result = EXIT_SUCCESS;

  try {

  } catch (std::exception & error) {
    std::cerr << "[fatal] " << error.what() << std::endl;
    result = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "[fatal] " << "An unexpected error occured!" << std::endl;
    result = EXIT_FAILURE;
  }

  return ( result < 0xff ? result : 0xff );
}
