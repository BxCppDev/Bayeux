// -*- mode: c++ ; -*- 
// test_material.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

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
    
      mat::material mat_H20;
      mat_H20.set_name ("Eau (H20)");
      mat_H20.add_element_by_nb_of_atoms (elt_H,2);
      mat_H20.add_element_by_nb_of_atoms (elt_O,1);      
      mat_H20.grab_properties ().store ("state", "liquid");
      mat_H20.set_density (1.* mat::material::g_per_cm3);
      mat_H20.build();
      mat_H20.tree_dump (clog, "First material (H20) : ");    
      
      double conc = 1e-2;     
      
      mat::material mat_Eau_Boree;
      mat_Eau_Boree.set_name ("Eau boree");
      mat_Eau_Boree.add_element_by_mass (elt_B,conc);
      mat_Eau_Boree.add_material_by_mass (mat_H20,1.-conc);      
      mat_Eau_Boree.grab_properties ().store ("state", "liquid");
      mat_Eau_Boree.set_density (1.* mat::material::g_per_cm3);
      mat_Eau_Boree.build();
      mat_Eau_Boree.tree_dump (clog, "2nd material Eau boree : "); 
     
    /*  
      mat::material my_material0;
      my_material0.set_name ("Copper");
      my_material0.set_density (8.9 * mat::material::g_per_cm3);
      my_material0.grab_properties ().store ("state", "solid");

      my_material0.tree_dump (clog, "A material: "); 
     
     
   
     
      
      mat::material  mat_H20("Eau");  
      mat_H20.add_element_by_nb_of_atoms (elt_H,2);
      mat_H20.add_element_by_nb_of_atoms (elt_O,1); 
      mat_H20.set_density (1 * mat::material::g_per_cm3); 
      mat_H20.grab_properties ().store ("state", "liquid");
      mat_H20.tree_dump (clog, "H2O material: "); 
  
  
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
