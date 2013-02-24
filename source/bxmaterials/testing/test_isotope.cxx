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
      
      //  my isotope 0 :   
   
      materials::isotope my_isotope_0;
      cerr << endl;
      my_isotope_0.tree_dump ( cout, "my isotope 0");   
     
      my_isotope_0.set_zai (1,1);
      cerr << endl;
      my_isotope_0.tree_dump ( cout, "my isotope 0");     
        
      my_isotope_0.set_name("hydrogen-1 (ground state)");
      my_isotope_0.find_mass();
      my_isotope_0.find_decay();
      cerr << endl; 
      my_isotope_0.tree_dump ( cout, "my isotope 0");    

      //  my isotope 1 :   
     
      materials::isotope my_isotope_1("unknown hydrogen-1 (1st excited state)");
      cerr << endl;
      my_isotope_1.tree_dump ( cout, "my isotope 1");   
     
      my_isotope_1.set_zai(1,1,1);
      my_isotope_1.find_mass ();
      my_isotope_1.set_decay (1.5,1.5);
      cerr << endl;
      my_isotope_1.tree_dump ( cout,"my_isotope 1"); 


      //  my isotope 2 :       

      materials::isotope my_isotope_2; 
      my_isotope_2.set_zai(1,2);
      cerr << endl;
      my_isotope_2.tree_dump (cout, "my_isotope 2");  
      my_isotope_2.find_mass (); 
      my_isotope_2.find_decay ();
      cerr << endl;
      my_isotope_2.tree_dump (cout, "my_isotope 2");  

      //  my isotope 3 :          

      materials::isotope my_isotope_3("Sodium 22 (ground state)",11,22,0); 
      my_isotope_3.find_mass();  
      my_isotope_3.find_decay(); 
      cerr << endl;
      my_isotope_3.tree_dump (cout, "my_isotope 3"); 

      //  my isotope 4 :       

      cerr << endl;
      materials::isotope my_isotope_4("Tungsten-184","W",184,0);  
      my_isotope_4.grab_properties ().store ("class", "transition metal"); 
      my_isotope_4.find_mass();  
      my_isotope_4.find_decay();
      my_isotope_4.tree_dump (cout, "my_isotope 4"); 
      
      //  my isotope 5 :       
      
      cerr << endl;
      materials::isotope my_isotope_5("Iode-120 (1st excited state)","I",120,1); 
      my_isotope_5.find_mass();
      my_isotope_5.find_decay();
      my_isotope_5.grab_properties ().store ("state","solid");
      my_isotope_5.tree_dump (cout, "my_isotope 5");  

      //  my isotope 6 :        

      cerr << endl;
      try
        {
        materials::isotope my_isotope_6("Unknown Sodium-45",11,45,0);  
        }
      catch (exception & x)
        {
          clog << "As expected, Sodium-45 is not valid !" << endl;
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

// end of test_isotope.cxx
