// -*- mode: c++; -*- 
// test_material.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>

#include <geomtools/material.h>

int 
main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      bool debug = false;

      geomtools::element::load_table_of_elements();

      geomtools::material::print_table_of_materials(std::cout);

      geomtools::material::create("Vacuum", 
				  "Hydrogen",
				  CLHEP::universe_mean_density,
				  geomtools::material::STATE_GAS
				  );

      std::vector<std::string> list_elements;
      list_elements.push_back("Oxygen");
      list_elements.push_back("Hydrogen");

      std::vector<int> list_natoms;
      list_natoms.push_back(1);
      list_natoms.push_back(2);

      geomtools::material::create("Water_liquid", 
				  list_elements,
				  list_natoms,
				  1.0 * CLHEP::gram / CLHEP::cm3,
				  geomtools::material::STATE_LIQUID
				  );

      list_elements.clear();
      list_elements.push_back("Carbon");
      list_elements.push_back("Oxygen");
      list_elements.push_back("Hydrogen");
      list_natoms.clear();
      list_natoms.push_back(10);
      list_natoms.push_back(4);
      list_natoms.push_back(8);
      geomtools::material::create("Mylar", 
				  list_elements,
				  list_natoms,
				  1.397 * CLHEP::gram / CLHEP::cm3,
				  geomtools::material::STATE_SOLID
				  );

      geomtools::material::create("Copper", 
				  "Copper",
				  8.96 * CLHEP::gram / CLHEP::cm3,
				  geomtools::material::STATE_SOLID
				  );

      geomtools::material::create("Aluminium", 
				  "Aluminium",
				  2.7 * CLHEP::gram / CLHEP::cm3
				  );

      geomtools::material::create("Silicon", 
				  "Silicon",
				  2.33 * CLHEP::gram / CLHEP::cm3
				  );

      geomtools::material::create("Iron", 
				  "Iron",
				  7.87 * CLHEP::gram / CLHEP::cm3
				  );

      geomtools::material::create("Lead", 
				  "Lead",
				  11.35 * CLHEP::gram / CLHEP::cm3
				  );

      geomtools::material::print_table_of_materials(std::cout);

      
      geomtools::material::get("Mylar").tree_dump(std::cout, 
						  "A well known material:");
      
      geomtools::material::get("Vacuum").tree_dump(std::cout, 
						   "The vacuum material:");

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

// end of test_material.cxx
