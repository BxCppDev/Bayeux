// -*- mode: c++; -*- 
// test_cylinder.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <geomtools/cylinder.h>

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
    
      geomtools::cylinder my_cylinder (1000 * CLHEP::mm,
				       230 * CLHEP::mm);

      std::cout << "Cylinder = " << my_cylinder << " " << std::endl;

      std::cout << "Volume   = " << my_cylinder.get_volume () / CLHEP::m3 << " m3" << std::endl;

      std::cout << "Side surface = " 
		<< my_cylinder.get_surface (geomtools::cylinder::FACE_SIDE) / CLHEP::m2 
		<< " m2" << std::endl;

      std::cout << "Top surface = " 
		<< my_cylinder.get_surface (geomtools::cylinder::FACE_TOP) / CLHEP::m2
		<< " m2" << std::endl;

      std::cout << "Bottom surface = "
		<< my_cylinder.get_surface (geomtools::cylinder::FACE_BOTTOM) / CLHEP::m2 
		<< " m2" << std::endl;

      std::cout << "Full surface = " 
		<< my_cylinder.get_surface (geomtools::cylinder::FACE_ALL) / CLHEP::m2 
		<< " m2" << std::endl;

      std::cout << "Enter a cylinder (example: '{cylinder 1000 1000}'): ";
      std::cin >> std::ws >> my_cylinder;
      if (std::cin) 
	{
	  std::cout << "Cylinder = " << my_cylinder << " " << std::endl;
	}
      else 
	{
	  throw std::runtime_error ("Invalid input for cylinder!");
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

// end of test_cylinder.cxx
