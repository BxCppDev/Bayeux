// -*- mode: c++; -*- 
// test_polyline_3d.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <geomtools/polyline_3d.h>
#include <geomtools/gnuplot_draw.h>
#include <datatools/serialization/serialization.h>

using namespace std;
using namespace geomtools;

int 
main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      long seed = 314159;
      srand48 (seed);

      polyline_3d pl (polyline_3d::open);

      for (int i = 0; i < 10; i++)
	{
	  geomtools::vector_3d p(2.0 * i + geomtools::random_tools::random_flat (),
				 geomtools::random_tools::random_flat (),
				 geomtools::random_tools::random_flat ());
	  pl.add (p);
	}

      basic_polyline_t bpl = pl.make_vertex_collection ();
      gnuplot_draw::basic_draw_polyline (cout, bpl);

      
      vector_3d pos;
      pos.set (1., 2., 3.);
      std::clog << "Vector: " << pos << std::endl;

      rotation rot;
      create_rotation(rot, 0.0, 90. * CLHEP::degree, 0.0);
      tree_dump (rot, std::clog, "Rotation matrix:");

      gnuplot_draw::draw_polyline (cout, pos, rot, bpl);

    }
  catch (std::exception & x)
    {
      std::cerr << "error: " << x.what() << std::endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      std::cerr << "error: " << "unexpected error!" << std::endl; 
      error_code = EXIT_FAILURE;
    }
  return error_code;
}

// end of test_polyline_3d.cxx
