// -*- mode: c++; -*- 
// test_tube.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <geomtools/tube.h>

int 
main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    bool debug = false;

    int iarg = 1;
    while (iarg < argc_) 
      {
	std::string arg = argv_[iarg];

	if (arg == "-d" || arg == "--debug") debug = true;

	iarg++;
      }
    
    geomtools::tube my_tube (500 * CLHEP::mm,
			     1000 * CLHEP::mm,
			     230 * CLHEP::mm);
    std::cout << "Tube     = " << my_tube << " " << std::endl;

    std::cout << "Volume   = " << my_tube.get_volume () / CLHEP::m3 << " m3" << std::endl;

    std::cout << "Outer side surface = " 
	      << my_tube.get_surface (geomtools::tube::FACE_OUTER_SIDE) / CLHEP::m2 
	      << " m2" << std::endl;

    std::cout << "Inner side surface = " 
	      << my_tube.get_surface (geomtools::tube::FACE_INNER_SIDE) / CLHEP::m2 
	      << " m2" << std::endl;

    std::cout << "Top surface = " 
	      << my_tube.get_surface (geomtools::tube::FACE_TOP) / CLHEP::m2 
	      << " m2" << std::endl;

    std::cout << "Bottom surface = " << my_tube.get_surface (geomtools::tube::FACE_BOTTOM) / CLHEP::m2 
	      << " m2" << std::endl;

    std::cout << "Full surface = " 
	      << my_tube.get_surface (geomtools::tube::FACE_ALL)/CLHEP::m2 
	      << " m²" << std::endl;

    std::cout << "Enter a tube (example: '{tube 900 1000 1000}'): ";
    std::cin >> std::ws >> my_tube;
    if (std::cin) 
      {
	std::cout << "Tube = " << my_tube << " " << std::endl;
      }
    else 
      {
	throw std::runtime_error ("Invalid input for tube!");
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

// end of test_tube.cxx
