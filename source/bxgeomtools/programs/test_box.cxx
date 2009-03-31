// -*- mode: c++; -*- 
// test_box.cxx
// gnuplot macro: ./tests/test_box.gpl
 
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <geomtools/box.h>
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

      // test 1:
      {
	geomtools::box my_box (2.0 * CLHEP::m, 
			       3.0 * CLHEP::m, 
			       0.5 * CLHEP::m);
	std::clog << "test 1: Box #1  = " << my_box << " " << std::endl;
	
	std::clog << "test 1: Volume  = " << my_box.get_volume () / CLHEP::m3 
		  << " m3" << std::endl;
	
	std::clog << "test 1: Front surface = " 
		  << my_box.get_surface (geomtools::box::FACE_FRONT) / CLHEP::m2 
		  << " m2" << std::endl;
	
	std::clog << "test 1: Top surface = " 
		  << my_box.get_surface (geomtools::box::FACE_TOP) / CLHEP::m2 
		  << " m2" << std::endl;
	
	std::clog << "test 1: Right surface = "
		  << my_box.get_surface (geomtools::box::FACE_RIGHT) / CLHEP::m2 
		  << " m2" << std::endl;
	
	std::clog << "test 1: Full surface = " 
		  << my_box.get_surface (geomtools::box::FACE_ALL) / CLHEP::m2 
		  << " m2" << std::endl;
	
	{
	  bool test_in = false;
	  if (test_in)
	    {
	      std::clog << "test 1: Enter a box (example: '{box 1000 1000 1000}'): ";
	      std::cin >> std::ws >> my_box;
	      if (std::cin) 
		{
		  std::clog << "test 1: Box     = " << my_box << " " << std::endl;
		}
	      else 
		{
		  throw std::runtime_error ("Invalid input for box!");
		}
	    }
	}
      }

      // test 2:
      {
	geomtools::box my_box (8.0 * CLHEP::mm, 
			       4.0 * CLHEP::mm, 
			       1.0 * CLHEP::mm);
	std::clog << "test 2: Box #2   = " << my_box << " " << std::endl;
      
	geomtools::vector_3d pos (2.0 * CLHEP::mm, 
				  3.0 * CLHEP::mm, 
				  2.0 * CLHEP::mm);
	geomtools::vector_3d dir (-1, -2.5, -1.5);
	//dir = -pos;
	geomtools::intercept_t intercept;
	int intercept_face;
	if (my_box.find_intercept (pos, dir, 
				   intercept))
	  {
	    std::clog << "test 2: Intercept face=" 
		      << intercept.get_face ()
		      << " at impact=" 
		      << intercept.get_impact ()
		      << std::endl;
	  }
	else
	  {
	    std::clog << "test 2: No intercept." << std::endl;
	  }

	geomtools::vector_3d box_pos;
	geomtools::rotation_3d box_rot;
	geomtools::create_rotation (box_rot, 0.0, 0.0, 0.0);
	geomtools::gnuplot_draw::draw_box (std::cout, box_pos, box_rot, 
					   my_box.get_x (), 
					   my_box.get_y (), 
					   my_box.get_z ());
	std::cout << std::endl << std::endl;

	geomtools::gnuplot_draw::basic_draw_point (std::cout, pos);
	std::cout << std::endl << std::endl;

	geomtools::gnuplot_draw::basic_draw_point (std::cout, 
						   intercept.get_impact ());
	std::cout << std::endl << std::endl;

	geomtools::gnuplot_draw::basic_draw_point (std::cout, pos);
	geomtools::gnuplot_draw::basic_draw_point (std::cout, pos + dir);
	std::cout << std::endl << std::endl;
      }
      
      // test 3:
      {
	geomtools::box my_box (5.0 * CLHEP::mm, 
			       5.0 * CLHEP::mm, 
			       5.0 * CLHEP::mm);
	std::clog << "test 3: Box #3   = " << my_box << " " << std::endl;
	geomtools::vector_3d box_pos;
	geomtools::rotation_3d box_rot;
	geomtools::create_rotation (box_rot, 0.0, 0.0, 0.0);
	geomtools::gnuplot_draw::draw_box (std::cout, box_pos, box_rot, 
					   my_box.get_x (), 
					   my_box.get_y (), 
					   my_box.get_z ());
	std::cout << std::endl << std::endl;
      
	size_t nshoots = 100000;
	for (int i = 0; i < (int) nshoots; i++)
	  {
	    geomtools::vector_3d pos (10.* drand48 () * CLHEP::mm, 
				      10.* drand48 () * CLHEP::mm, 
				      10.* drand48 () * CLHEP::mm);
	    geomtools::vector_3d dir;
	    geomtools::randomize_direction (drand48, dir);

	    geomtools::intercept_t intercept;
	    int intercept_face;
	    if (my_box.find_intercept (pos, dir, 
				       intercept))
	      {
		if (debug) std::clog << "test 3: Intercept face=" 
				     << intercept.get_face ()
				     << " at impact=" << intercept.get_impact ()
				     << std::endl;
		geomtools::gnuplot_draw::basic_draw_point (std::cout, 
							   intercept.get_impact ());
	      }
	    else
	      {
		if (debug) std::clog << "test 3: No intercept." << std::endl;
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
