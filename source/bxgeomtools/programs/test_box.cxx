// -*- mode: c++; -*- 
// test_box.cxx

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

      int iarg = 1;
      while (iarg < argc_) 
	{
	  std::string arg = argv_[iarg];

	  if (arg == "-d" || arg == "--debug") debug = true;

	  iarg++;
	}
    
      geomtools::box my_box (2.0 * CLHEP::m, 
			     3.0 * CLHEP::m, 
			     0.5 * CLHEP::m);
      std::clog << "Box     = " << my_box << " " << std::endl;

      std::clog << "Volume  = " << my_box.get_volume () / CLHEP::m3 
		<< " m3" << std::endl;

      std::clog << "Front surface = " 
		<< my_box.get_surface (geomtools::box::FACE_FRONT) / CLHEP::m2 
		<< " m2" << std::endl;

      std::clog << "Top surface = " 
		<< my_box.get_surface (geomtools::box::FACE_TOP) / CLHEP::m2 
		<< " m2" << std::endl;

      std::clog << "Right surface = "
		<< my_box.get_surface (geomtools::box::FACE_RIGHT) / CLHEP::m2 
		<< " m2" << std::endl;

      std::clog << "Full surface = " 
		<< my_box.get_surface (geomtools::box::FACE_ALL) / CLHEP::m2 
		<< " m2" << std::endl;

      bool test_in = false;
      if (test_in)
	{
	  std::clog << "Enter a box (example: '{box 1000 1000 1000}'): ";
	  std::cin >> std::ws >> my_box;
	  if (std::cin) 
	    {
	      std::clog << "Box     = " << my_box << " " << std::endl;
	    }
	  else 
	    {
	      throw std::runtime_error ("Invalid input for box!");
	    }
	}

      {
	geomtools::box my_box2 (8.0 * CLHEP::mm, 
				4.0 * CLHEP::mm, 
				1.0 * CLHEP::mm);
	std::clog << "Box #2   = " << my_box2 << " " << std::endl;
      
	geomtools::vector_3d pos (2.0 * CLHEP::mm, 
				  3.0 * CLHEP::mm, 
				  2.0 * CLHEP::mm);
	geomtools::vector_3d dir (-1, -2.5, -1.5);
	//dir = -pos;
	geomtools::vector_3d intercept;
	int intercept_face;
	if (my_box2.find_intercept (pos, dir, 
				    intercept, 
				    intercept_face))
	  {
	    std::clog << "Intercept face=" << intercept_face
		      << " at intercept=" << intercept
		      << std::endl;
	  }
	else
	  {
	    std::clog << "No intercept." << std::endl;
	  }

	geomtools::vector_3d box_pos;
	geomtools::rotation box_rot;
	geomtools::create_rotation (box_rot, 0.0, 0.0, 0.0);
	geomtools::gnuplot_draw::draw_box (std::cout, box_pos, box_rot, 
					   my_box2.get_x (), 
					   my_box2.get_y (), 
					   my_box2.get_z ());
	std::cout << std::endl << std::endl;

	geomtools::gnuplot_draw::basic_draw_point (std::cout, pos);
	std::cout << std::endl << std::endl;

	geomtools::gnuplot_draw::basic_draw_point (std::cout, intercept);
	std::cout << std::endl << std::endl;

	geomtools::gnuplot_draw::basic_draw_point (std::cout, pos);
	geomtools::gnuplot_draw::basic_draw_point (std::cout, pos + dir);
	std::cout << std::endl << std::endl;
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
