// -*- mode: c++ ; -*- 
// gmanager.cxx
// Usage: gmanager GEOM_FILE [MODEL_NAME]
// Example: gmanager "setup.geom" "categories.lis" "world"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include <geomtools/model_factory.h>
#include <geomtools/gnuplot_drawer.h>
#include <geomtools/id_mgr.h>
#include <geomtools/mapping.h>
#include <geomtools/placement.h>

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      std::string view_mode;
      std::string geom_file;
      std::string category_file;
      std::string model_name;
      int iarg = 1;
      while (iarg < argc_)
	{
	  std::string token = argv_[iarg];
	  if (token == "-3d") view_mode = geomtools::gnuplot_drawer::VIEW_3D;
	  else if (token == "-xy") view_mode = geomtools::gnuplot_drawer::VIEW_2D_XY;
	  else if (token == "-xz") view_mode = geomtools::gnuplot_drawer::VIEW_2D_XZ;
	  else if (token == "-yz") view_mode = geomtools::gnuplot_drawer::VIEW_2D_YZ;
	  else if (geom_file.empty ()) geom_file = token;
	  else if (category_file.empty ()) category_file = token;
	  else if (model_name.empty ()) model_name = token;
	  ++iarg;
	}
      if (view_mode.empty ()) view_mode = geomtools::gnuplot_drawer::VIEW_3D;
      if (geom_file.empty ()) throw std::logic_error ("Missing .geom filename !");
      if (category_file.empty ()) throw std::logic_error ("Missing category filename !");      
      if (model_name.empty ()) model_name = "world";

      // Declare a model factory :
      geomtools::model_factory the_model_factory;
      the_model_factory.load (geom_file);
      the_model_factory.lock ();

      // Print the list of geometry models :
      clog << "List of models: " << endl;
      int count = 0;
      for (geomtools::models_col_t::const_iterator i 
	     = the_model_factory.get_models ().begin ();
	   i != the_model_factory.get_models ().end ();
	   i++)
	{
	  std::clog << "  " << i->second->get_name () << std::endl;
	}
      std::clog << std::endl;

      // Declare the GID manager :
      geomtools::id_mgr the_gid_manager;
      the_gid_manager.load (category_file); 

      // Declare the mapping manager :
      geomtools::mapping the_mapping;
      the_mapping.set_id_manager (the_gid_manager);
      // Do not limit the depth of the numbering scheme trhough the hierarchy :
      the_mapping.set_max_depth (geomtools::mapping::NO_MAX_DEPTH);
      // Do not generate GID entries for some specific geometry categories :
      the_mapping.add_excluded ("mailrow");
      the_mapping.add_excluded ("mailcolumn");
      the_mapping.build_from (the_model_factory);
      std::clog << "Congratulations ! Mapping has been built !" << std::endl;
      std::clog << "List of available GIDs : " << std::endl;
      for (geomtools::geom_info_dict_t::const_iterator i 
	     = the_mapping.get_geom_infos ().begin ();
	   i != the_mapping.get_geom_infos ().end ();
	   ++i)
	{
	  std::cout << "  Geom ID : " << i->first << std::endl;
	}

      // Declare a Gnuplot renderer :
      geomtools::gnuplot_drawer GPD;
      GPD.set_view (view_mode);
      geomtools::placement reference_placement;
      reference_placement.set (0 * CLHEP::m, 0 * CLHEP::m, 0 * CLHEP::m, 
			       0 * CLHEP::degree, 0 * CLHEP::degree, 0);
      GPD.draw (the_model_factory, 
		model_name, 
		reference_placement,  
		geomtools::gnuplot_drawer::DISPLAY_LEVEL_NO_LIMIT);
 	
    }
  catch (exception & x)
    {
      std::cerr << "error: " << x.what () << std::endl; 
      error_code = EXIT_FAILURE;
    }
   return error_code;
}

// end of gmanager.cxx
