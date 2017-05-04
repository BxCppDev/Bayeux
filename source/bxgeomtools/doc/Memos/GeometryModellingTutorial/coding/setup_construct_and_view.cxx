// -*- mode: c++ ; -*- 
// setup_construct_and_view.cxx
// Usage: setup_construct_and_view GEOM_FILE [MODEL_NAME]
// Example: setup_construct_and_view "setup_2.geom" "world"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include <geomtools/model_factory.h>
#include <geomtools/gnuplot_drawer.h>
#include <geomtools/placement.h>

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      if (argc_ < 2)
	{
	  throw std::logic_error ("Missing .geom filename !");
	}
      string geom_file = argv_[1];
      string model_name = "world";
      if (argc_ >= 3)
	{
	  model_name = argv_[2];
	}

      // Declare a GID manager :
      geomtools::model_factory the_model_factory;
      the_model_factory.load (geom_file);
      the_model_factory.lock ();

      // Declare a Gnuplot renderer :
      geomtools::gnuplot_drawer GPD;
      GPD.set_view (geomtools::gnuplot_drawer::VIEW_3D);
      GPD.set_mode (geomtools::gnuplot_drawer::MODE_WIRED);
      geomtools::placement reference_placement;
      reference_placement.set (0 * CLHEP::m, 0 * CLHEP::m, 0 * CLHEP::m, 
			       0 * CLHEP::degree, 0 * CLHEP::degree, 0);
      GPD.draw (the_model_factory, 
		model_name, 
		reference_placement,  
		geomtools::gnuplot_drawer::DISPLAY_LEVEL_NO_LIMIT);
 	
    }
  catch (exception & x)
    {
      std::cerr << "error: " << x.what () << std::endl; 
      error_code = EXIT_FAILURE;
    }
   return error_code;
}

// end of setup_construct_and_view.cxx
