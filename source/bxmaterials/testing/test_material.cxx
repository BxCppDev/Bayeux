// -*- mode: c++ ; -*- 
// test_material.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <materials/isotope.h>
#include <materials/element.h>
#include <materials/material.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'material'!" << endl; 
  
      bool debug = false;

      int iarg = 1;
      while (iarg < argc_)
        {
          string token = argv_[iarg];

          if (token[0] == '-')
            {
               string option = token; 
               if ((option == "-d") || (option == "--debug")) 
                 {
                   debug = true;
                 }
               else 
                 { 
                    clog << "warning: ignoring option '" << option << "'!" << endl; 
                 }
            }
          else
            {
              string argument = token; 
              { 
                clog << "warning: ignoring argument '" << argument << "'!" << endl; 
              }
            }
          iarg++;
      }
    
 // Definition of Isotopes :    
    
      mat::isotope iso_H1 = mat::isotope ("H1", "H", 1, mat::isotope::GROUND_STATE, true);   	 
      mat::isotope iso_H2 = mat::isotope ("H2", "H", 2, mat::isotope::GROUND_STATE, true);
 
 
      mat::isotope iso_O16 = mat::isotope ("O16", "O",  16, mat::isotope::GROUND_STATE, true);   	 
      mat::isotope iso_O17 = mat::isotope ("O17", "O",  17, mat::isotope::GROUND_STATE, true);
      mat::isotope iso_O18 = mat::isotope ("O18", "O",  18, mat::isotope::GROUND_STATE, true);   
      
      
      mat::isotope iso_B10 = mat::isotope ("B10", "B",  10, mat::isotope::GROUND_STATE, true);   	 
      mat::isotope iso_B11 = mat::isotope ("B11", "B",  11, mat::isotope::GROUND_STATE, true);    
  
  
// Definition of  Elements :  
   
      mat::element elt_H("Hydrogen",1);
      elt_H.add_isotope (iso_H1, 99.985);
      elt_H.add_isotope (iso_H2, 0.015);
      elt_H.build (); 
      elt_H.tree_dump (cout, "my element Hydrogen");     
            
      mat::element elt_O("Oxygen",8);
      elt_O.add_isotope (iso_O16, 99.76);
      elt_O.add_isotope (iso_O17, 0.039);
      elt_O.add_isotope (iso_O18, 0.201);
      elt_O.build (); 
      elt_O.tree_dump (cout, "my element Oxygen");    
      
      mat::element elt_B("Bore",5);
      elt_B.add_isotope (iso_B10,  19.9);
      elt_B.add_isotope (iso_B11,  80.1);
      elt_B.build (); 
      elt_B.tree_dump (cout, "my element Bore");    
  
      
 // Definition Of Materials :        
    
      mat::material mat_H2O;
      mat_H2O.set_name ("water (H2O)");
      mat_H2O.add_element_by_nb_of_atoms (elt_H,2);
      mat_H2O.add_element_by_nb_of_atoms (elt_O,1);      
      mat_H2O.grab_properties ().store ("state", "liquid");
      mat_H2O.set_density (1.* mat::material::g_per_cm3);
      mat_H2O.build();
      mat_H2O.tree_dump (clog, "First material (H2O) : ");    
      
      
      mat::material mat_borated_water;
      double boron_mass_concentration = 1e-2;     
      mat_borated_water.set_name ("borated water");
      mat_borated_water.add_element_by_mass (elt_B, boron_mass_concentration);
      mat_borated_water.add_material_by_mass (mat_H2O, 1. - boron_mass_concentration);      
      mat_borated_water.grab_properties ().store ("state", "liquid");
      mat_borated_water.set_density (1.* mat::material::g_per_cm3);
      mat_borated_water.build ();
      mat_borated_water.tree_dump (clog, "2nd material Borated water : "); 

      mat::material mat_diamond;
      mat_diamond.set_name ("Diamond");
      mat_diamond.set_mean_z_a (6.0, 12.0);
      mat_diamond.set_density (3.517 * mat::material::g_per_cm3);
      mat_diamond.grab_properties ().store ("state", "solid");
      mat_diamond.build ();
      mat_diamond.tree_dump (clog, "3rd material Diamond : "); 
     
    /*  
      mat::material my_material0;
      my_material0.set_name ("Copper");
      my_material0.set_density (8.9 * mat::material::g_per_cm3);
      my_material0.grab_properties ().store ("state", "solid");

      my_material0.tree_dump (clog, "A material: "); 
     
     
   
     
      
      mat::material  mat_H2O("Eau");  
      mat_H2O.add_element_by_nb_of_atoms (elt_H,2);
      mat_H2O.add_element_by_nb_of_atoms (elt_O,1); 
      mat_H2O.set_density (1 * mat::material::g_per_cm3); 
      mat_H2O.grab_properties ().store ("state", "liquid");
      mat_H2O.tree_dump (clog, "H2O material: "); 
  
  
 */ 
      
  /*    
      mat::material my_material1("Eau Boree");  
      my_material1.add_element (elt_C,1,KP_MOL);
      my_material1.add_element (elt_C,0,KP_MASS);
      
     */ 
   //   my_material1.set_density (8.9 * mat::material::g_per_cm3);
     // my_material1.grab_properties ().store ("state", "solid");
    


    }
  catch (exception & x)
    {
      cerr << "error: " << x.what () << endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error!" << endl; 
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}

// end of test_material.cxx
