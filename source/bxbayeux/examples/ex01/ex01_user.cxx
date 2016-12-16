// Standard library:
#include <cstdlib>
#include <iostream>
#include <stdexcept>

// Third party:
#include <boost/filesystem.hpp>

// Bayeux:
#include <bayeux/bayeux.h>
#include <bayeux/version.h>
#include <bayeux/datatools/version.h>
#include <bayeux/datatools/properties.h>
#include <bayeux/materials/isotope.h>
#include <bayeux/materials/resource.h>

int main()
{
  bayeux::initialize();
  int error_code = EXIT_SUCCESS;
  try {

    std::clog << "Bayeux version : [" << BAYEUX_VERSION << "]" << std::endl;
    std::clog << "Bayeux version : " << BAYEUX_LIB_VERSION << std::endl;
    std::clog << "Bayeux/datatools version : " << DATATOOLS_LIB_VERSION << std::endl;
    std::clog << std::endl;

    datatools::properties config("Dummy config");
    config.store("foo", "bar");
    config.store("Pi", 3.14159);
    config.tree_dump(std::clog, "Configuration:");
    std::clog << std::endl;

    materials::isotope He6(2, 6, materials::isotope::GROUND_STATE);
    He6.build(materials::isotope::BF_MASS_DATA);
    He6.lock();
    He6.tree_dump(std::clog, "He6:");
    std::clog << std::endl;

    boost::filesystem::path mrdir = materials::get_resource_dir();
    std::clog << "Bayeux/materials resource directory : " << mrdir << std::endl;
    std::clog << std::endl;

  } catch (std::exception & error) {
    std::cerr << "error: " << error.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  bayeux::terminate();
  return error_code;
}
