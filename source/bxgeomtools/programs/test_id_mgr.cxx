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
	  cat_info.create (id);
	  clog << "ID = " << id << ' ' 
	       << (id.is_valid () ? "[Valid]": "[Invalid]")<< endl;
	}
      clog << endl << endl;
      
      clog << "Test of ID mapping tools:" << endl << endl;
      
      geomtools::geom_id world_id (0, 0);
      
      string module_id_info = "  [ module : module=3]  ";
      geomtools::geom_id module_id;
      
      my_id_mgr.compute_id_from_info (module_id, world_id, module_id_info);
      clog << "Module ID = " << module_id
	   << " -> " << my_id_mgr.id_to_human_readable_format_string (module_id) 
	   << ' ' << ((my_id_mgr.validate_id (module_id)) ? "[Valid]": "[Invalid]")
	   << endl;

      string submodule_id_info = "  [ source_submodule : ]  ";
      geomtools::geom_id submodule_id;
      my_id_mgr.compute_id_from_info (submodule_id, module_id, submodule_id_info);
      clog << "Source submodule ID = " << submodule_id
	   << " -> " << my_id_mgr.id_to_human_readable_format_string (submodule_id) 
	   << ' ' << ((my_id_mgr.validate_id (submodule_id)) ? "[Valid]": "[Invalid]")
	   << endl;
       
      string layer_id_info = "  [ source_layer : layer=3]  ";
      geomtools::geom_id layer_id;
      my_id_mgr.compute_id_from_info (layer_id, module_id, layer_id_info);
      clog << "Layer ID = " << layer_id 
	   << " -> " << my_id_mgr.id_to_human_readable_format_string (layer_id) 
	   << ' ' << (my_id_mgr.validate_id (layer_id) ? "[Valid]": "[Invalid]")
	   << endl;
      
      string strip_id_info = "  [ source_strip : strip=8]  ";
      geomtools::geom_id strip_id;

      my_id_mgr.compute_id_from_info (strip_id, layer_id, strip_id_info, 3, 4);
      clog << "Strip ID = " << strip_id
	   << " -> " << my_id_mgr.id_to_human_readable_format_string (strip_id) 
	   << ' ' << (my_id_mgr.validate_id (strip_id) ? "[Valid]": "[Invalid]")
	   << endl;

      for (int ipad = 0; ipad < 3; ipad++)
	{
	  string pad_id_info = "  [ source_pad : pad+2]  ";
	  geomtools::geom_id pad_id;
	  
	  my_id_mgr.compute_id_from_info (pad_id, strip_id, pad_id_info, ipad);
	  clog << "Pad ID = " << pad_id
	       << " -> " << my_id_mgr.id_to_human_readable_format_string (pad_id) 
	       << ' ' <<  (my_id_mgr.validate_id (pad_id) ? "[Valid]": "[Invalid]")
	       << endl;
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
