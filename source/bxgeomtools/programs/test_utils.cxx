// -*- mode: c++; -*- 
// test_utils.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <geomtools/utils.h>
#include <geomtools/gnuplot_draw.h>

int 
main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      long seed = 314159;
      srand48 (seed);

      geomtools::vector_3d pos;
      pos.set (1., 2., 3.);
      std::clog << "Vector: " << pos << std::endl;

      geomtools::rotation rot;
      geomtools::create_rotation(rot, 0.0, 90. * CLHEP::degree, 0.0);
      geomtools::tree_dump (rot, std::clog, "Rotation matrix:");

      geomtools::vector_3d origin;
      origin.set (0., 0., 0.);
      geomtools::vector_3d dir;
      dir.set (drand48 (), drand48 (), drand48 ());
      std::clog << "Direction: " << dir << std::endl;

      geomtools::gnuplot_draw::basic_draw_point (std::cout, origin);
      geomtools::gnuplot_draw::basic_draw_point (std::cout, dir);
      std::cout << std::endl;
      std::cout << std::endl;

      for (int i = 0; i < 16; i++)
	{
	  geomtools::vector_3d orth_dir;
	  geomtools::randomize_orthogonal_direction (drand48, dir, orth_dir);
	  std::clog << "Random orthogonal direction: " << orth_dir << std::endl;
      
	  geomtools::gnuplot_draw::basic_draw_point (std::cout, origin);
	  geomtools::gnuplot_draw::basic_draw_point (std::cout, orth_dir);
	  if (i == 0) // trick
	    geomtools::gnuplot_draw::basic_draw_point (std::cout, origin);
	  std::cout << std::endl;
	}
      std::cout << std::endl;
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

// end of test_utils.cxx
