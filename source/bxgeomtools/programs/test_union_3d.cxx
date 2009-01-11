// -*- mode: c++; -*- 
// test_union_3d.cxx
// gnuplot macro: ./tests/test_union_3d.gpl

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <geomtools/box.h>
#include <geomtools/union_3d.h>
#include <geomtools/gnuplot_draw.h>

int 
main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      bool debug = false;
      long seed = 314159;

      int iarg = 1;
      while (iarg < argc_) 
	{
	  std::string arg = argv_[iarg];

	  if (arg == "-d" || arg == "--debug") debug = true;

	  iarg++;
	}
    
      srand48 (seed);

      using namespace geomtools;

      // test 1:
      {
	box b1 (4.0 * CLHEP::mm, 
		4.0 * CLHEP::mm, 
		4.0 * CLHEP::mm);
	placement p1 (vector_3d (0, 0, 0), 
		      0, 0, 0);

	box b2 (2.0 * CLHEP::mm, 
		2.0 * CLHEP::mm, 
		2.0 * CLHEP::mm);
	placement p2 (vector_3d (2, 2, 2), 
		      M_PI / 3., M_PI / 3., M_PI / 6.);

	union_3d u1;
	u1.set_shape1 (b1, p1);
	u1.set_shape2 (b2, p2);
	u1.dump (std::clog);

	geomtools::vector_3d pos (4.0 * CLHEP::mm, 
				  3.0 * CLHEP::mm, 
				  6.0 * CLHEP::mm);
	geomtools::vector_3d dir (-1., -1., -1.);
	geomtools::vector_3d intercept;
	int intercept_face = geomtools::face_3d::FACE_NONE_BIT;
	
	if (u1.find_intercept (pos, dir, 
			       intercept, 
			       intercept_face))
	  {
	    std::clog << "test 1: Intercept face=" << intercept_face
		      << " at intercept=" << intercept
		      << std::endl;
	  }
	else
	  {
	    std::clog << "test 1: No intercept." << std::endl;
	  }
	

	geomtools::gnuplot_draw::draw_box (std::cout, 
					   p1.get_translation (), 
					   p1.get_rotation (), 
					   b1.get_x (), 
					   b1.get_y (), 
					   b1.get_z ());
	geomtools::gnuplot_draw::draw_box (std::cout, 
					   p2.get_translation (), 
					   p2.get_rotation (), 
					   b2.get_x (), 
					   b2.get_y (), 
					   b2.get_z ());
	std::cout << std::endl << std::endl;

	geomtools::gnuplot_draw::basic_draw_point (std::cout, pos);
	std::cout << std::endl << std::endl;

	geomtools::gnuplot_draw::basic_draw_point (std::cout, intercept);
	std::cout << std::endl << std::endl;

	geomtools::gnuplot_draw::basic_draw_point (std::cout, pos);
	geomtools::gnuplot_draw::basic_draw_point (std::cout, intercept);
	std::cout << std::endl << std::endl;

	std::clog << "test 1: End." << std::endl;

	size_t nshoots = 100000;
	for (int i = 0; i < (int) nshoots; i++)
	  {
	    double dim = 6. * CLHEP::mm;
	    geomtools::vector_3d pos (dim * drand48 (), 
				      dim * drand48 (), 
				      dim * drand48 ());
	    geomtools::vector_3d dir;
	    geomtools::randomize_direction (drand48, dir);
	    
	    geomtools::vector_3d intercept;
	    int intercept_face;
	    if (u1.find_intercept (pos, dir, 
				   intercept, 
				   intercept_face))
	      {
		if (debug) std::clog << "test 1: Intercept face=" << intercept_face
				     << " at intercept=" << intercept
				     << std::endl;
		geomtools::gnuplot_draw::basic_draw_point (std::cout, intercept);
	      }
	    else
	      {
		if (debug) std::clog << "test 1: No intercept." << std::endl;
	      }
	  }
      }
      
    }
  catch (std::exception & x)
    {
      std::cerr << "error: " << x.what () << std::endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      std::cerr << "error: " << "unexpected error!" << std::endl; 
      error_code = EXIT_FAILURE;
    }
  return error_code;
}

// end of test_box.cxx
