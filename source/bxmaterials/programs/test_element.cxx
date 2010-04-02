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
      
      mat::isotope my_isotope_1("H ",1);  
      mat::isotope my_isotope_2("H ",2);   
      mat::isotope my_isotope_3("H ",3); 
      mat::isotope my_isotope_4("O ",16);   
      mat::isotope my_isotope_5(8,16);   
      
      mat::element my_element_1;
      my_element_1.add_isotope(my_isotope_1, 0.2);
      my_element_1.add_isotope(my_isotope_2, 0.5);
      my_element_1.add_isotope(my_isotope_3, 0.3);
      my_element_1.add_isotope(new mat::isotope("C ", 12), 0.1);  
      my_element_1.tree_dump (cout, "my element 1");   
      
      
/*
       mat::map_isotope_weight   comp1;
       
       comp1[&my_isotope_1] = 1; 
       comp1[&my_isotope_3] = 1;
       comp1[&my_isotope_3] = 2;
       	
       mat::map_isotope_weight   comp2; comp1[&my_isotope_1] = 1; 
       comp2[&my_isotope_4] = 1;
       comp2[&my_isotope_5] = 2;
       
       
       comp2.insert(pair<mat::isotope*, double>(&my_isotope_1, 2));  
       comp2.insert(pair<mat::isotope*, double>(&my_isotope_2, 1));

       cerr<<endl;

       mat::element my_element_1;
       my_element_1.set_name("H2O");
       my_element_1.tree_dump (cout, "my element 1");  
      
   
       cerr<<endl; 
       my_element_1.set_composition(comp1);
       my_element_1.set_composition(comp2);
       my_element_1.tree_dump (cout, "my element 1");
*/
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
