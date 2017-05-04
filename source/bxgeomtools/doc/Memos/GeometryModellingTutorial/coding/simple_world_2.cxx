// -*- mode: c++ ; -*- 
// simple_world_2.cxx

#include <cstdlib>
#include <iostream>
#include <exception>

#include <geomtools/model_factory.h>
#include <geomtools/gnuplot_drawer.h>
#include <geomtools/gdml_export.h>
#include <geomtools/placement.h>

int main (void)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      // Declare a GID manager :
      geomtools::model_factory the_model_factory;
      the_model_factory.load ("simple_world_1.geom");
      the_model_factory.lock ();

      // Declare a Gnuplot renderer :
      geomtools::gnuplot_drawer GPD;
      GPD.set_view (geomtools::gnuplot_drawer::VIEW_3D);
      GPD.set_mode (geomtools::gnuplot_drawer::MODE_WIRED);
      geomtools::placement reference_placement;
      reference_placement.set (0 * CLHEP::m, 0 * CLHEP::m, 0 * CLHEP::m, 
			       0 * CLHEP::degree, 0 * CLHEP::degree, 0);
      GPD.draw (the_model_factory, 
		"world", 
		reference_placement,  
		geomtools::gnuplot_drawer::DISPLAY_LEVEL_NO_LIMIT);

      // Declare a GDML export driver :
      geomtools::gdml_export GDML;
      GDML.export_gdml ("simple_world_2.gdml", the_model_factory, "world");
 	
    }
  catch (exception & x)
    {
      std::cerr << "error: " << x.what () << std::endl; 
      error_code = EXIT_FAILURE;
    }
   return error_code;
}

// end of simple_world_2.cxx
