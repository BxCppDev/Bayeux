// -*- mode: c++ ; -*- 
// test_factory.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <vector>

#include <materials/factory.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'factory'!" << endl; 
  
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
 
      datatools::utils::properties iso_0_conf;
      iso_0_conf.store ("z", 1);
      iso_0_conf.store ("a", 1);
      iso_0_conf.store ("i", "GS");

      datatools::utils::properties iso_1_conf;
      iso_1_conf.store ("z", 1);
      iso_1_conf.store ("a", 2);
      iso_1_conf.store ("i", "GS");

      datatools::utils::properties iso_2_conf;
      iso_2_conf.store ("z", 6);
      iso_2_conf.store ("a", 12);
      iso_2_conf.store ("i", "GS");

      mat::factory my_factory;

      mat::isotope_dict_t isotopes;
      {
	mat::isotope * iso = my_factory.create_isotope ("iso_0", iso_0_conf);
	iso->tree_dump (clog, "Isotope 0 from the factory: ");
	isotopes[iso->get_name ()] = mat::smart_ref<mat::isotope> ();
	isotopes[iso->get_name ()].set_ref (iso);
      }
      {
	mat::isotope * iso = my_factory.create_isotope ("iso_1", iso_1_conf);
	iso->tree_dump (clog, "Isotope 1 from the factory: ");
	isotopes[iso->get_name ()] = mat::smart_ref<mat::isotope> ();
	isotopes[iso->get_name ()].set_ref (iso);
      }
      {
	mat::isotope * iso = my_factory.create_isotope ("iso_2", iso_2_conf);
	iso->tree_dump (clog, "Isotope 2 from the factory: ");
	isotopes[iso->get_name ()] = mat::smart_ref<mat::isotope> ();
	isotopes[iso->get_name ()].set_ref (iso);
      }

      mat::element_dict_t elements;
 
      datatools::utils::properties elmt_1_conf;
      elmt_1_conf.store ("z", 1);
      vector<string> iso_names;
      iso_names.push_back ("iso_0");
      iso_names.push_back ("iso_1");
      vector<double> iso_weights;
      iso_weights.push_back (0.75);
      iso_weights.push_back (0.25);
      elmt_1_conf.store ("isotope.names", iso_names);
      elmt_1_conf.store ("isotope.weights", iso_weights);
      {
	mat::element * elmt = my_factory.create_element ("elmt_1", 
							 elmt_1_conf,
							 isotopes);
	elmt->tree_dump (clog, "Element 1 from the factory: ");
	elements[elmt->get_name ()] = mat::smart_ref<mat::element> ();
	elements[elmt->get_name ()].set_ref (elmt);
      }

      mat::material_dict_t materials;

      datatools::utils::properties matl_1_conf;
      matl_1_conf.store ("density", 1.0);
      matl_1_conf.store ("density.unit", "g/cm3");
      {
	mat::material * matl = my_factory.create_material ("matl_1", 
							   matl_1_conf,
							   elements,
							   materials);
	matl->tree_dump (clog, "Material 1 from the factory: ");
	materials[matl->get_name ()] = mat::smart_ref<mat::material> ();
	materials[matl->get_name ()].set_ref (matl);
      }
      
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

// end of test_factory.cxx
