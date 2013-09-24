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

      materials::isotope iso_H1 = materials::isotope ("H1", "H", 1, materials::isotope::GROUND_STATE, true);
      materials::isotope iso_H2 = materials::isotope ("H2", "H", 2, materials::isotope::GROUND_STATE, true);


      materials::isotope iso_O16 = materials::isotope ("O16", "O",  16, materials::isotope::GROUND_STATE, true);
      materials::isotope iso_O17 = materials::isotope ("O17", "O",  17, materials::isotope::GROUND_STATE, true);
      materials::isotope iso_O18 = materials::isotope ("O18", "O",  18, materials::isotope::GROUND_STATE, true);


      materials::isotope iso_B10 = materials::isotope ("B10", "B",  10, materials::isotope::GROUND_STATE, true);
      materials::isotope iso_B11 = materials::isotope ("B11", "B",  11, materials::isotope::GROUND_STATE, true);


// Definition of  Elements :

      materials::element elt_H("Hydrogen",1);
      elt_H.add_isotope (iso_H1, 99.985);
      elt_H.add_isotope (iso_H2, 0.015);
      elt_H.build ();
      elt_H.tree_dump (cout, "my element Hydrogen");

      materials::element elt_O("Oxygen",8);
      elt_O.add_isotope (iso_O16, 99.76);
      elt_O.add_isotope (iso_O17, 0.039);
      elt_O.add_isotope (iso_O18, 0.201);
      elt_O.build ();
      elt_O.tree_dump (cout, "my element Oxygen");

      materials::element elt_B("Bore",5);
      elt_B.add_isotope (iso_B10,  19.9);
      elt_B.add_isotope (iso_B11,  80.1);
      elt_B.build ();
      elt_B.tree_dump (cout, "my element Bore");


 // Definition Of Materials :

      materials::material mat_H2O;
      mat_H2O.set_name ("water (H2O)");
      mat_H2O.add_element_by_nb_of_atoms (elt_H,2);
      mat_H2O.add_element_by_nb_of_atoms (elt_O,1);
      mat_H2O.grab_properties ().store ("state", "liquid");
      mat_H2O.set_density (1.* materials::material::g_per_cm3());
      mat_H2O.build();
      mat_H2O.tree_dump (clog, "First material (H2O) : ");


      materials::material mat_borated_water;
      double boron_mass_concentration = 1e-2;
      mat_borated_water.set_name ("borated water");
      mat_borated_water.add_element_by_mass (elt_B, boron_mass_concentration);
      mat_borated_water.add_material_by_mass (mat_H2O, 1. - boron_mass_concentration);
      mat_borated_water.grab_properties ().store ("state", "liquid");
      mat_borated_water.set_density (1.* materials::material::g_per_cm3());
      mat_borated_water.build ();
      mat_borated_water.tree_dump (clog, "2nd material Borated water : ");

      materials::material mat_diamond;
      mat_diamond.set_name ("Diamond");
      mat_diamond.set_mean_z_a (6.0, 12.0);
      mat_diamond.set_density (3.517 * materials::material::g_per_cm3());
      mat_diamond.grab_properties ().store ("state", "solid");
      mat_diamond.build ();
      mat_diamond.tree_dump (clog, "3rd material Diamond : ");

    /*
      materials::material my_material0;
      my_material0.set_name ("Copper");
      my_material0.set_density (8.9 * materials::material::g_per_cm3());
      my_material0.grab_properties ().store ("state", "solid");

      my_material0.tree_dump (clog, "A material: ");





      materials::material  mat_H2O("Eau");
      mat_H2O.add_element_by_nb_of_atoms (elt_H,2);
      mat_H2O.add_element_by_nb_of_atoms (elt_O,1);
      mat_H2O.set_density (1 * materials::material::g_per_cm3());
      mat_H2O.grab_properties ().store ("state", "liquid");
      mat_H2O.tree_dump (clog, "H2O material: ");


 */

  /*
      materials::material my_material1("Eau Boree");
      my_material1.add_element (elt_C,1,KP_MOL);
      my_material1.add_element (elt_C,0,KP_MASS);

     */
   //   my_material1.set_density (8.9 * materials::material::g_per_cm3());
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
