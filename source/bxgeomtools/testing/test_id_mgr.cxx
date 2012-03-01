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
      bool test_mapping = false;
      bool test_extraction = true;
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
	      else if ((option == "-m") || (option == "--mapping")) 
		{
		  test_mapping = true;
		}
	      else if ((option == "-X") || (option == "--no-extraction")) 
		{
		  test_extraction = false;
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
      my_id_mgr.load ("${GEOMTOOLS_DATA_DIR}/testing/config/test_geom_id_mgr.lis");
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
      
      if (test_extraction)
	{
	  clog << "Test ID inheritance extraction:" << endl << endl;

	  // make an ID for a Geiger cell:
	  geomtools::geom_id gg_cell_id;
	  // build the Geiger cell ID:
	  my_id_mgr.make_id ("geiger_cell", gg_cell_id);
	  my_id_mgr.set (gg_cell_id, "module", 4); // module==4
	  my_id_mgr.set (gg_cell_id, "side",   1); // submodule==1 (front)
	  my_id_mgr.set (gg_cell_id, "layer",  2); // layer #2
	  my_id_mgr.set (gg_cell_id, "column", 28); // cell #28
	  cout << "Geiger cell ID = " << gg_cell_id  
	       << " in category '" 
	       << my_id_mgr.get_category (gg_cell_id) 
	       << "'" << endl;

	  if (! my_id_mgr.is_category (gg_cell_id, "source_layer"))
	    {
	      cout << "ID " << gg_cell_id <<
		" is not from 'source_layer' category !" << endl;
	    }

	  if (my_id_mgr.is_category (gg_cell_id, "geiger_cell"))
	    {
	      cout << "ID " << gg_cell_id <<
		" is from 'geiger_cell' category !" << endl;

	      cout << "  Module number is : " 
		   << my_id_mgr.get (gg_cell_id, "module") << endl;
	      cout << "  Side number is   : " 
		   << my_id_mgr.get (gg_cell_id, "side") << endl;
	      cout << "  Layer number is  : " 
		   << my_id_mgr.get (gg_cell_id, "layer") << endl;
	      cout << "  Cell number is   : " 
		   << my_id_mgr.get (gg_cell_id, "column") << endl;
	    }

	  clog << endl << "Extraction: " << endl;
	  geomtools::geom_id module_id;
	  // build the module ID:
	  my_id_mgr.make_id ("module", module_id);
	  my_id_mgr.extract (gg_cell_id, module_id);
	  cout << "Module ID = " << module_id << endl;

	  geomtools::geom_id submodule_id;
	  // build the module ID:
	  my_id_mgr.make_id ("tracker_submodule", submodule_id);
	  my_id_mgr.extract (gg_cell_id, submodule_id);
	  cout << "Tracker submodule ID = " << submodule_id << endl;
	  cout << endl << endl;
	
	}

      if (test_mapping)
	{
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
