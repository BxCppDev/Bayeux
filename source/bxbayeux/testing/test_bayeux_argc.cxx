// Standard library:
#include <iostream>
#include <cstdlib>

// This project:
#include <bayeux/bayeux_config.h>
#include <bayeux/bayeux.h>
#include <bayeux/reloc.h>
#include <bayeux/version.h>
#include <bayeux/resource.h>
#include <bayeux/datatools/kernel.h>
#include <bayeux/datatools/urn_query_service.h>
#include <bayeux/datatools/utils.h>

int main( int argc_, char** argv_ )
{
  int result = EXIT_SUCCESS;
  ::bayeux::initialize(argc_, argv_, 0);

  try {
 
  } catch (std::exception & error) {
    std::cerr << "[fatal] " << error.what() << std::endl;
    result = EXIT_FAILURE;
  } catch (...) {
    std::cerr << "[fatal] " << "An unexpected error occured!" << std::endl;
    result = EXIT_FAILURE;
  }

  bayeux::terminate();
  return (result);
}
