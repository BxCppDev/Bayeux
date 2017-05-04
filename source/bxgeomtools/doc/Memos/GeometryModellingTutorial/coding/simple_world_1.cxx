// -*- mode: c++ ; -*- 
// simple_world_1.cxx

#include <cstdlib>
#include <iostream>
#include <exception>

#include <geomtools/model_factory.h>

int main (void)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      // Declare a GID manager :
      geomtools::model_factory the_model_factory;

      // Load the configuration file for geometry models :
      the_model_factory.load ("simple_world_1.geom");

      // Lock the model factory and trigger the construction :
      the_model_factory.lock ();

      // Dump the model factory :
      the_model_factory.tree_dump (std::clog);

    }
  catch (exception & x)
    {
      std::cerr << "error: " << x.what () << std::endl; 
      error_code = EXIT_FAILURE;
    }
   return error_code;
}

// end of simple_world_1.cxx
