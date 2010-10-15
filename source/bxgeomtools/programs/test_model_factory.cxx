// -*- mode: c++ ; -*- 
// test_model_factory.cxx  
  
#include <cstdlib> 
#include <iostream>
#include <string>
#include <exception>

#include <datatools/utils/utils.h>

#include <geomtools/model_factory.h>
#include <geomtools/gnuplot_drawer.h>
#include <geomtools/gdml_export.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'model_factory'!" << endl; 
  
      bool debug = false;
      bool devel = false;
      string drawer_view = geomtools::gnuplot_drawer::VIEW_3D;
      bool draw = true;
      bool gdml = true;
      bool dump = true;
      string setup_filename;
      string model_name; 

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
               else if (option == "--devel") 
                 {
                   devel = true;
                 }
               else if (option == "-D") 
                 {
                   draw = false;
                 }
               else if (option == "-G") 
                 {
                   gdml = false;
                 }
               else if (option == "-T") 
                 {
                   dump = false;
                 }
               else if (option == "-xy") 
                 {
                   drawer_view = geomtools::gnuplot_drawer::VIEW_2D_XY;
                 }
               else if (option == "-xz") 
                 {
                   drawer_view = geomtools::gnuplot_drawer::VIEW_2D_XZ;
                 }
               else if (option == "-yz") 
                 {
                   drawer_view = geomtools::gnuplot_drawer::VIEW_2D_YZ;
                 }
               else if (option == "-3d") 
                 {
                   drawer_view = geomtools::gnuplot_drawer::VIEW_3D;
                 }
               else if (option == "-m") 
                 {
		   model_name = argv_[++iarg];
                 }
	       else 
                 { 
                    clog << "warning: ignoring option '" << option << "'!" << endl; 
                 }
            }
          else
            {
              string argument = token; 
	      if (setup_filename.empty ())
		{
		  setup_filename = argument;
		}
	      else
		{ 
		  clog << "warning: ignoring argument '" << argument << "'!" << endl; 
		}
            }
          iarg++;
      }  
    
      geomtools::i_model::g_devel = devel;
 
      geomtools::model_factory factory;
      factory.set_debug (debug);

      if (setup_filename.empty ())
	{
	  setup_filename = "${GEOMTOOLS_ROOT}/resources/test/test_geometry_models.setup";
	} 
      datatools::utils::fetch_path_with_env (setup_filename);
      factory.load (setup_filename);
      factory.lock ();
      if (dump) factory.tree_dump (clog);

      geomtools::placement p;
      p.set (0, 0, 0, 0 * CLHEP::degree, 0 * CLHEP::degree, 0);
      if (dump) p.tree_dump (clog, "Placement");
 
      clog << "Models: " << endl;
      int count = 0;
      for (geomtools::models_col_t::const_iterator i 
	     = factory.get_models ().begin ();
	   i != factory.get_models ().end ();
	   i++)
	{
	  clog << "  " << i->second->get_name ();
	  count++;
	  if ((count % 5) == 0)  clog << endl;
	}
      clog << endl;

      if (model_name.empty ())
	{
	  clog << "Enter the name of the geometry model to be displayed: ";
	  getline (cin, model_name);
	}
      if (model_name.empty ())
	{
	  model_name = "world";
	}
      clog << "Name of the model : " << model_name << endl;

      if (draw)
	{   
	  geomtools::gnuplot_drawer::g_devel = devel;
	  geomtools::gnuplot_drawer GPD;
	  GPD.set_view (drawer_view);
	  GPD.set_mode (geomtools::gnuplot_drawer::MODE_WIRED);
	  GPD.draw (factory, 
		    model_name, 
		    p,  
		    geomtools::gnuplot_drawer::DISPLAY_LEVEL_NO_LIMIT);
	}

      ostringstream ext_mat_oss;
      geomtools::gdml_writer writer;
      {
	writer.add_element ("Hydrogen", 1, "H", 1);
	writer.add_element ("Oxygen", 8, "O", 16);
	writer.add_element ("Carbon", 6, "C", 12);
	writer.add_element ("Nitrogen", 7, "N", 14);
	writer.add_element ("Lead", 82, "Pb", 1);

	writer.add_material ("Al", 
			     13.0,
			     2.70 * CLHEP::g / CLHEP::cm3,
			     26.98);
	
	map<string, double> Air_map; 
	Air_map["Oxygen"] = 0.3;
	Air_map["Nitrogen"] = 0.7;
	writer.add_material ("Air",
			     "air",
			     1.29 * CLHEP::mg / CLHEP::cm3,
			     Air_map);
	
	map<string, size_t> H2O_map;
	H2O_map["Oxygen"] = 1;
	H2O_map["Hydrogen"] = 2;
	writer.add_material ("Water",
			     "H2O",
			     1.0 * CLHEP::g / CLHEP::cm3,
			     H2O_map);
      }

      if (gdml)
      {
	geomtools::gdml_export::g_devel = devel;
	geomtools::gdml_export GDML;
	GDML.add_replica_support (true);
	GDML.attach_external_materials (writer.get_stream (geomtools::gdml_writer::MATERIALS_SECTION));
	GDML.parameters ().store ("xml_version",  
				  geomtools::gdml_writer::DEFAULT_XML_VERSION);
	GDML.parameters ().store ("xml_encoding", 
				  geomtools::gdml_writer::DEFAULT_XML_ENCODING);
	GDML.parameters ().store ("gdml_schema",  
				  geomtools::gdml_writer::DEFAULT_GDML_SCHEMA);
	GDML.parameters ().store ("length_unit",  
				  geomtools::gdml_export::DEFAULT_LENGTH_UNIT);
	GDML.parameters ().store ("angle_unit",   
				  geomtools::gdml_export::DEFAULT_ANGLE_UNIT);
	GDML.export_gdml ("test_model_factory.gdml", factory, model_name);
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

// end of test_model_factory.cxx
