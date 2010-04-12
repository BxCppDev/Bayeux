// -*- mode: c++ ; -*- 
// test_id_mgr.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <geomtools/id_mgr.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'id_mgr'!" << endl; 
  
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
    
      geomtools::id_mgr my_id_mgr;
      my_id_mgr.load ("${GEOMTOOLS_ROOT}/resources/geom_id_mgr.lis");
      my_id_mgr.tree_dump (clog);

      string cat_name = "calorimeter_column";
      if (my_id_mgr.has_category_info (cat_name))
	{
	  const geomtools::id_mgr::category_info & cat_info
	    = my_id_mgr.get_category_info (cat_name);
	  cat_info.tree_dump (clog, "Category info: ");
	  geomtools::geom_id id;
	  cat_info.make (id);
	  clog << "ID = " << id << ' ' 
	       << (id.is_valid () ? "[Valid]": "[Invalid]")<< endl;
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

// end of test_id_mgr.cxx
