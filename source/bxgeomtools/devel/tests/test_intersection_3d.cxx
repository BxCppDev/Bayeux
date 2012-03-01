// -*- mode: c++; -*- 
// test_intersection_3d.cxx
// gnuplot macro: ./tests/test_intersection_3d.gpl

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <geomtools/box.h>
#include <geomtools/intersection_3d.h>
#include <geomtools/gnuplot_draw.h>

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      bool debug = false;
      long seed = 314159;
      bool test2 = true;

      int iarg = 1;
      while (iarg < argc_) 
	{
	  std::string arg = argv_[iarg];

	  if (arg == "-d" || arg == "--debug") debug = true;
	  if (arg == "-t2" || arg == "--test2") test2 = false;

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

	box b2 (3.0 * CLHEP::mm, 
		3.0 * CLHEP::mm, 
		3.0 * CLHEP::mm);
	placement p2 (vector_3d (2, 2, 2), 
		      M_PI / 2., 0.0, 0.0);

	//		      M_PI / 3., M_PI / 3., M_PI / 6.);

	intersection_3d inter1;
	inter1.set_shape1 (b1, p1);
	inter1.set_shape2 (b2, p2);
	inter1.dump (std::clog);
	
	geomtools::vector_3d pos (4.0 * CLHEP::mm, 
				  3.0 * CLHEP::mm, 
				  5.0 * CLHEP::mm);
	geomtools::vector_3d dir (-1., -1., -2.);
	geomtools::intercept_t intercept;
	std::clog << "test 1: pos=" << pos
		  << " dir=" << dir
		  << " impact=" << intercept.get_impact ()
		  << std::endl;

	geomtools::intersection_3d::g_devel = debug;	
	if (inter1.find_intercept (pos, dir, intercept))
	  {
	    std::clog << "test 1: Intercept face=" << intercept.get_face ()
		      << " at impact=" << intercept.get_impact ()
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

	geomtools::gnuplot_draw::basic_draw_point (std::cout, 
						   intercept.get_impact ());
	std::cout << std::endl << std::endl;

	geomtools::gnuplot_draw::basic_draw_point (std::cout, pos);
	std::clog << "test 1: intercept=" << intercept.get_impact () << std::endl;
	if (intercept.is_valid ())
	  {
	    geomtools::gnuplot_draw::basic_draw_point (std::cout, 
						       intercept.get_impact ());
	  }
	else
	  {
	    geomtools::gnuplot_draw::basic_draw_point (std::cout, 
						       pos + 3 * dir);
	  }
	std::cout << std::endl << std::endl;

	std::clog << "test 1: End." << std::endl;

	if (test2)
	  {
	    size_t nshoots = 100000;
	    for (int i = 0; i < (int) nshoots; i++)
	      {
		if ((i%1000) == 0) std::clog << "Loop #" << i << std::endl;
		//std::clog << "DEVEL: Loop #" << i << std::endl;

		// special debug activation:
		int idevel = -1;
		geomtools::intersection_3d::g_devel = false;
		idevel = 1817;
		if (i == idevel)
		  {
		    geomtools::intersection_3d::g_devel = true;
		  }

		double dim = 6. * CLHEP::mm;
		geomtools::vector_3d pos (dim * drand48 (), 
					  dim * drand48 (), 
					  dim * drand48 ());
		//pos.set (2.0, 2.0, 2.0);
		//pos.set (.0, .0, .0);
		geomtools::vector_3d dir;
		geomtools::randomize_direction (drand48, dir);
	    
		geomtools::intercept_t intercept;
		int intercept_face;
		if (inter1.find_intercept (pos, dir, 
					   intercept))
		  {
		    if (debug) std::clog << "test 1: Intercept face=" 
					 << intercept.get_face ()
					 << " at impact=" 
					 << intercept.get_impact ()
					 << std::endl;
		    geomtools::gnuplot_draw::basic_draw_point (std::cout, 
							       intercept.get_impact ());
		  }
		else
		  {
		    if (debug) std::clog << "test 1: No intercept." << std::endl;
		  }
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

// end of test_interscetion_3d.cxx
