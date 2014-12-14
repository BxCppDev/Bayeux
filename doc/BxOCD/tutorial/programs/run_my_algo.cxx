//! \file programs/run_my_algo.cxx Program using the my::algo configurable class

// Standard library:
#include <iostream>
#include <exception>
#include <cstdlib>

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>
#include <datatools/object_configuration_description.h>

// This project:
#include <my/algo.h>

//! \brief Main function
int main(int argc_, char * argv_[])
{
  int error_code = EXIT_SUCCESS;
  std::string config_filename;

  try {

    int iarg = 1;
    while (iarg < argc_) {
      std::string opt = argv_[iarg];
      if (opt == "-c") {
        config_filename = argv_[++iarg];
      }
      iarg++;
    }

    // Instantiate a collection of configuration parameters:
    datatools::properties parameters;
    if (config_filename.empty()) {
      parameters.store("debug", true);     // Add a boolean parameter
      parameters.store("max_nloops", 500); // Add an integer parameter
      parameters.store("epsilon", 1.e-2);  // Add a real parameter
      parameters.store("output_filename", "results.data"); // Add a character string parameter
    } else {
      parameters.read_configuration(config_filename);
    }

    // Instantiate the configurable algorithm object:
    my::algo a;

    // Configure and initialize the object using the
    // above collection of parameters:
    a.initialize(parameters);

    // Run/use the configured and initialized the algorithm
    //  object:
    a.run();

    // Reset/terminate the algorithm object:
    a.reset();
  }
  catch (std::exception & error) {
    std::cerr << "error: run_my_algo: " << error.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}
