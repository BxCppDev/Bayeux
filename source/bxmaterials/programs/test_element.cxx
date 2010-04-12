// -*- mode: c++ ; -*- 
// test_element.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <materials/element.h>


using namespace std;

int main (int argc_, char ** argv_)
{

  int error_code = EXIT_SUCCESS;
  
  try
    {
      clog << "Test program for class 'element'!" << endl; 
  
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
              /* Here you may add more argument handlers... */
              { 
                clog << "warning: ignoring argument '" << argument << "'!" << endl; 
              }
            }
          iarg++;
      }
    

      mat::isotope iso_H1; 
      iso_H1 = mat::isotope("H ",1);   
	
      mat::isotope iso_H2 = mat::isotope ("H ",2);   
      mat::isotope iso_H3 = mat::isotope ("H ",3);          
      mat::isotope iso_H4 = mat::isotope ("H ",4);   
      mat::isotope iso_H5 = mat::isotope (1,5);  
 
      mat::element elt_H("Hydrogen");
          
      elt_H.add_isotope (iso_H1, 0.2);
      elt_H.add_isotope (iso_H2, 0.5);
      elt_H.add_isotope (iso_H3, 0.1); 
      elt_H.build (); 
      elt_H.tree_dump (cout, "my element Hydrogen");   
      
      elt_H.add_isotope (iso_H4, 0.1); 
      elt_H.add_isotope (iso_H5, 0.1); 
      elt_H.build (); 
      elt_H.tree_dump (cout, "my element Hydrogen");     
      
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

// end of test_element.cxx
