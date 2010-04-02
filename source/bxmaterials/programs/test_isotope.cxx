// -*- mode: c++ ; -*- 
// test_isotope.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <materials/isotope.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'isotope'!" << endl; 
  
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
      
     mat::isotope my_isotope_0; 
     my_isotope_0.tree_dump (cout, "my isotope 0");      
     
    mat::isotope my_isotope_1("H ",1,0); 
     my_isotope_1.tree_dump (cout, "my isotope 1"); 
     
     cerr<<endl;
     mat::isotope my_isotope_2(1,1); 
     my_isotope_2.find_decay ();
     my_isotope_2.tree_dump (cout, "my isotope 2");  
         
     cerr<<endl;
     mat::isotope my_isotope_3(11,22); 
     my_isotope_3.find_decay(); 
     my_isotope_3.tree_dump (cout, "my isotope 3"); 
      
      cerr<<endl;
      mat::isotope my_isotope_4("W ",184,0);  
      my_isotope_4.grab_properties ().store ("class", "transition metal"); 
      my_isotope_4.find_decay();
      my_isotope_4.tree_dump (cout, "my isotope 4"); 
      
      
      cerr<<endl;
      mat::isotope my_isotope_5("I ",120,1); 
      my_isotope_5.grab_properties ().store ("state","solid");
      my_isotope_5.find_decay();
      my_isotope_5.tree_dump (cout, "my isotope 5");  
      
      cerr<<endl;
      mat::isotope my_isotope_6(11,45,0);  
      my_isotope_6.tree_dump (cout, "my isotope 6");  

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

// end of test_isotope.cxx
