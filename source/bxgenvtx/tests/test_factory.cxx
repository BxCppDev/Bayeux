// -*- mode: c++ ; -*- 
// test_factory.cxx

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

#include <datatools/utils/properties.h>
#include <mygsl/rng.h>
#include <genvtx/factory.h>

using namespace std;
using namespace genvtx;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'genvtx::factory'!" << endl; 
  
      bool debug = false;

      int iarg = 1;
      while (iarg < argc_)
        {
          string token = argv_[iarg];

          if (token[0] == '-')
            {
               string option = token; 
               if ((option == "-d") || (option == "--debug")) 
                 {
                   debug = true;
                 }
               else 
                 { 
                    clog << "WARNING: ignoring option '" << option << "'!" << endl; 
                 }
            }
          else
            {
              string argument = token; 
              { 
                clog << "WARNING: ignoring argument '" << argument << "'!" << endl; 
              }
            }
          iarg++;
      }
    
      genvtx::factory my_factory;
	
      std::string       rng_id = "mt19937";
      unsigned long int rng_seed = 12345;
      mygsl::rng random (rng_id, rng_seed);
      
      size_t nshoots = 3;
      geomtools::vector_3d vertex;
     
      i_vertex_generator * vg = 0;

      { 
	string vg_id = "genvtx::spot_vertex_generator";
	clog << "Generating vertices from a '" << vg_id << "' generator:" << endl;
	datatools::utils::properties vg_configuration;
	vg_configuration.store ("spot.x", -4.0);
	vg_configuration.store ("spot.y", +2.0);
	vg_configuration.store ("spot.z", -1.0);
	vg_configuration.store ("spot.length_unit", "mm");
 	vg = my_factory.create_vertex_generator (vg_id, 
						 vg_configuration);
	if (vg == 0)
	  {
	    ostringstream message;
	    message << "Unkwon vertex generator '" << vg_id << "'";
	    throw runtime_error (message.str ());
	  }
	clog << "NOTICE: vertex_generator is '" << vg->vg_id () << "'." << endl; 
      
	for (int i = 0; i < nshoots; i++)
	  {
	    vg->shoot_vertex (random, vertex);
	    cout << vertex << endl;
	  }
      }

      {
	string vg_id = "genvtx::from_file_vg";
	clog << "Generating vertices from a '" << vg_id << "' generator:" << endl;
	datatools::utils::properties vg_configuration;
	vg_configuration.store ("filename", "${GENVTX_ROOT}/tests/vertices.data");
	vg = my_factory.create_vertex_generator (vg_id, 
						 vg_configuration);
	if (vg == 0)
	  {
	    ostringstream message;
	    message << "Unkwon vertex generator '" << vg_id << "'";
	    throw runtime_error (message.str ());
	  }
	clog << "NOTICE: vertex_generator is '" << vg->vg_id () << "'." << endl; 
      
	while (true)
	  {
	    vg->shoot_vertex (random, vertex);
	    if (! geomtools::is_valid (vertex))
	      {
		break;
	      }
	    cout << vertex << endl;
	  }
      }

    }
  catch (exception & x)
    {
      cerr << "ERROR: " << x.what () << endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "ERROR: " << "unexpected error!" << endl; 
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}

// end of test_factory.cxx
