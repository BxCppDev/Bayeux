// -*- mode: c++; -*- 
// test_sphere.cxx
// gnuplot macro: ./tests/test_sphere.gpl

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <geomtools/sphere.h>
#include <geomtools/gnuplot_draw.h>

int 
main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      bool debug = false;
      long seed = 314159;
      bool test1 = true;
      bool test2 = true;
      bool test3 = true;

      int iarg = 1;
      while (iarg < argc_) 
	{
	  std::string arg = argv_[iarg];

	  if (arg == "-d" || arg == "--debug") debug = true;
	  if (arg == "-t1" || arg == "--test1") test1 = false;
	  if (arg == "-t2" || arg == "--test2") test2 = false;
	  if (arg == "-t3" || arg == "--test3") test3 = false;

	  iarg++;
	}
    
      srand48 (seed);
    
      // test 1:
      if (test1) 
	{
	  geomtools::sphere my_sphere (1000 * CLHEP::mm);

	  std::clog << "test 1: Sphere #1 = " << my_sphere << " " << std::endl;

	  std::clog << "test 1: Volume   = " << my_sphere.get_volume () / CLHEP::m3 << " m3" << std::endl;

	  std::clog << "test 1: Side surface = " 
		    << my_sphere.get_surface (geomtools::sphere::FACE_SIDE) / CLHEP::m2 
		    << " m2" << std::endl;

	  std::clog << "test 1: Full surface = " 
		    << my_sphere.get_surface (geomtools::sphere::FACE_ALL) / CLHEP::m2 
		    << " m2" << std::endl;

	  {
	    bool test_in = false;
	    if (test_in)
	      {
		std::clog << "test 1: Enter a sphere (example: '{sphere 1000}'): ";
		std::cin >> std::ws >> my_sphere;
		if (std::cin) 
		  {
		    std::clog << "test 1: Sphere = " << my_sphere << " " << std::endl;
		  }
		else 
		  {
		    throw std::runtime_error ("Invalid input for sphere!");
		  }
	      }
	  }
	}

      // test 2:
      if (test2) 
	{
	  geomtools::sphere my_sphere (3.0 * CLHEP::mm);
	  std::clog << "test 2: Sphere #2   = " << my_sphere << " " << std::endl;
      
	  geomtools::vector_3d pos (4.0 * CLHEP::mm, 
				    0.0 * CLHEP::mm, 
				    0.0 * CLHEP::mm);
	  geomtools::vector_3d dir (-1, 0, 0);
	  geomtools::vector_3d intercept;
	  int intercept_face;
	  if (my_sphere.find_intercept (pos, dir, 
					intercept, 
					intercept_face))
	    {
	      std::clog << "test 2: Intercept face=" << intercept_face
			<< " at intercept=" << intercept
			<< std::endl;
	    }
	  else
	    {
	      std::clog << "test 2: No intercept." << std::endl;
	    }

	  geomtools::vector_3d sphere_pos;
	  geomtools::rotation sphere_rot;
	  geomtools::create_rotation (sphere_rot, 0.0, 0.0, 0.0);
	  geomtools::gnuplot_draw::draw_sphere (std::cout, sphere_pos, sphere_rot, 
						my_sphere.get_r ());
	  std::cout << std::endl << std::endl;

	  geomtools::gnuplot_draw::basic_draw_point (std::cout, pos);
	  std::cout << std::endl << std::endl;

	  geomtools::gnuplot_draw::basic_draw_point (std::cout, intercept);
	  std::cout << std::endl << std::endl;

	  geomtools::gnuplot_draw::basic_draw_point (std::cout, pos);
	  geomtools::gnuplot_draw::basic_draw_point (std::cout, pos + dir);
	  std::cout << std::endl << std::endl;
	}
 
      // test 3:
      if (test3) 
	{
	  geomtools::sphere my_sphere (2.0 * CLHEP::mm);
	  std::clog << "test 3: Sphere #3   = " << my_sphere 
		    << " " << std::endl;
	  geomtools::vector_3d sphere_pos;
	  geomtools::rotation sphere_rot;
	  geomtools::create_rotation (sphere_rot, 0.0, 0.0, 0.0);
	  geomtools::gnuplot_draw::draw_sphere (std::cout, 
						sphere_pos, 
						sphere_rot, 
						my_sphere.get_r ());
	  std::cout << std::endl << std::endl;
	
	  size_t nshoots = 100000;
	  for (int i = 0; i < (int) nshoots; i++)
	    {
	      double dim = 4. * CLHEP::mm;
	      //dim = 0. * CLHEP::mm;
	      geomtools::vector_3d pos (dim * drand48 (),
					dim * drand48 (),
					dim * drand48 ());
	      //pos.setZ (3. * CLHEP::mm);
	      geomtools::vector_3d dir;
	      geomtools::randomize_direction (drand48, dir);

	      geomtools::vector_3d intercept;
	      int intercept_face;
	      if (my_sphere.find_intercept (pos, dir, 
					    intercept, 
					    intercept_face))
		{
		  if (debug) std::clog << "test 3: Intercept face=" << intercept_face
				       << " at intercept=" << intercept
				       << std::endl;
		  geomtools::gnuplot_draw::basic_draw_point (std::cout, intercept);
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

// end of test_sphere.cxx
