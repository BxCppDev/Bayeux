// -*- mode: c++ ; -*- 
// test_materials_geom_plugin.cxx
 
#include <cstdlib>
#include <iostream> 
#include <string>
#include <exception>
 
#include <datatools/properties.h>
#include <datatools/utils.h>

#include <geomtools/gnuplot_drawer.h>
#include <geomtools/gdml_export.h>
#include <geomtools/visibility.h>

#include <geomtools/manager.h>
#include <geomtools/manager_macros.h>
#include <geomtools/mapping.h>

#include <materials/materials_geom_plugin.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      std::clog << "Test program for class 'materials::materials_geom_plugin' !" << std::endl; 
  
      bool   debug = false;
      bool   devel = false;
      bool   du_debug = false;
      bool   verbose = false;
      std::string manager_config_file;
      bool   dump = false;
      bool   visu = false;
      std::string visu_model_name = "";
      int    visu_depth  = geomtools::gnuplot_drawer::DISPLAY_LEVEL_NO_LIMIT;
      std::string drawer_view = geomtools::gnuplot_drawer::VIEW_3D;
      bool   gdml = false;
      bool   gdml_replica_support = false;
      bool   force_show = false;
      bool   force_show_envelop = false;
      bool   force_show_children = false;
      std::string tmp_output_dir = "";

      // parse command line argument:
      int iarg = 1;
      while (iarg < argc_)
        {
          std::string token = argv_[iarg];
 
          if (token[0] == '-')
            {
              std::string option = token; 
              if ((option == "-d") || (option == "--debug")) 
                {
                  debug = true;
                }
              else if (option == "--devel") 
                {
                  devel = true;
                }
              else if (option == "--du-debug") 
                {
                  du_debug = true;
                }
              else if ((option == "-V") || (option == "--verbose")) 
                {
                  verbose = true;
                }
              else if ((option == "-v") || (option == "--visu")) 
                {
                  visu = true;
                }
              else if ((option == "-f") || (option == "--force-show")) 
                {
                  force_show = true;
                }
              else if ((option == "-e") || (option == "--force-show-envelop")) 
                {
                  force_show_envelop = true;
                }
              else if ((option == "-c") || (option == "--force-show-children")) 
                {
                  force_show_children = true;
                }
              else if ((option == "-p") || (option == "--dump")) 
                {
                  dump = true;
                }
              else if ((option == "-g") || (option == "--gdml")) 
                {
                  gdml = true;
                }
              else if ((option == "-R") || (option == "--replica")) 
                {
                  gdml_replica_support = true;
                }
              else if ((option == "-m") || (option == "--model")) 
                {
                  std::string s = argv_[++iarg];
                  if (s.empty ())
                    {
                      throw std::logic_error ("Missing model name !");
                    }
                  visu_model_name = s;
                }
              else if ((option == "-D") || (option == "--visu-depth")) 
                {
                  std::string s = argv_[++iarg];
                  if (s.empty ())
                    {
                      throw std::logic_error ("Missing visualization depth !");
                    }
                  std::istringstream iss (s);
                  iss >> visu_depth;
                  if (! iss)
                    {
                      std::cerr << "warning: invalid format for visualization depth !" << std::endl;                     
                    }
                  if (visu_depth < 0)
                    {
                      std::cerr << "warning: invalid value for visualization depth !" << std::endl;                      
                      visu_depth = 0;
                    }
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
              else if ((option == "-o") || (option == "--output-dir")) 
                {
                  std::string s = argv_[++iarg];
                  if (s.empty ())
                    {
                      throw std::logic_error ("Missing temporary output dir !");
                    }
                  tmp_output_dir = s;
                }
              else 
                { 
                  std::clog << "warning: ignoring option '" 
                            << option << "'!" << std::endl; 
                }
            }
          else
            {
              std::string argument = token; 
              if (manager_config_file.empty ())
                {
                  manager_config_file = argument;
                }
              else 
                { 
                  std::clog << "warning: ignoring argument '" 
                            << argument << "'!" << std::endl; 
                }
            }
          iarg++;
        }

      if (du_debug)
        {
          datatools::properties::g_debug = du_debug;
        }

      if (manager_config_file.empty ())
        {
          manager_config_file = "${MATERIALS_DATA_DIR}/testing/config/test-1.0/test_manager.conf";
        }
      datatools::fetch_path_with_env (manager_config_file);
      std::clog << "Manager config. file : '" << manager_config_file << "'" << std::endl; 
    
      // load properties from the configuration file:
      datatools::properties manager_config;
      datatools::properties::read_config (manager_config_file, 
                                          manager_config); 

      geomtools::manager geo_mgr;
      geo_mgr.set_debug (debug);
      geo_mgr.set_verbose (verbose);
      geo_mgr.initialize (manager_config);
      if (dump) 
        {
          geo_mgr.get_factory ().tree_dump (std::clog, 
                                               "A test geometry model factory");
          geo_mgr.get_id_mgr ().tree_dump (std::clog, 
                                              "A test geometry ID manager");
        }

      if (visu)
        {
          while (true)
            {
              std::clog << "Models: " << std::endl;
              int count = 0;
              for (geomtools::models_col_type::const_iterator i 
                     = geo_mgr.get_factory ().get_models ().begin ();
                   i != geo_mgr.get_factory ().get_models ().end ();
                   i++)
                {
                  bool visible = true;
                  visible = geomtools::visibility::is_shown (i->second->get_logical ().parameters ());
                  if (force_show)
                    {
                      visible = true;
                    }
                  if (visible)
                    {
                      std::clog << "  " << datatools::io::left 
                                << datatools::io::width (35) << i->second->get_name ();
                      count++;
                      if ((count % 2) == 0) std::clog << std::endl;
                    }
                }
              std::clog << std::endl;

              std::string option_view;
              if (visu_model_name.empty ())
                {
                  std::clog << "Enter the name of the geometry model to display [.q = quit]: ";
                  std::string line;
                  std::getline (std::cin, line);
                  std::istringstream iss (line);
                  iss >> visu_model_name >> std::ws >> option_view;
                  std::clog << std::endl;
                }
              if (visu_model_name == ".q")
                {
                  visu_model_name = "";
                  break;
                }
              if (! option_view.empty ())
                {
                  if (option_view == "-xy") 
                    {
                      drawer_view = geomtools::gnuplot_drawer::VIEW_2D_XY;
                    }
                  else if (option_view == "-xz") 
                    {
                      drawer_view = geomtools::gnuplot_drawer::VIEW_2D_XZ;
                    }
                  else if (option_view == "-yz") 
                    {
                      drawer_view = geomtools::gnuplot_drawer::VIEW_2D_YZ;
                    }
                  else if (option_view == "-3d") 
                    {
                      drawer_view = geomtools::gnuplot_drawer::VIEW_3D;
                    }
                }
      
              if (visu_model_name.empty ())
                {
                  visu_model_name = "world";
                }
              if (debug) 
                {
                  std::clog << "DEBUG: " << "visu_model_name : '" << visu_model_name << "'" << std::endl; 
                }
              geomtools::placement visu_placement;
              visu_placement.set (0, 0, 0, 
                                  0 * CLHEP::degree, 0 * CLHEP::degree, 0 * CLHEP::degree);
              if (debug) 
                {
                  visu_placement.tree_dump (std::clog, "Placement", "DEBUG: ");
                }

              geomtools::id_mgr::g_devel = devel;
              geomtools::gnuplot_drawer::g_devel = devel;
              geomtools::gnuplot_drawer GPD;
              if (force_show)
                {
                  GPD.get_properties ().store (geomtools::gnuplot_drawer::FORCE_SHOW_PROPERTY_NAME, true);
                }
              if (force_show_envelop)
                {
                  GPD.get_properties ().store (geomtools::gnuplot_drawer::FORCE_SHOW_ENVELOP_PROPERTY_NAME, true);
                }
              if (force_show_children)
                {
                  GPD.get_properties ().store (geomtools::gnuplot_drawer::FORCE_SHOW_CHILDREN_PROPERTY_NAME, true);
                }
              GPD.set_view (drawer_view);
              GPD.set_mode (geomtools::gnuplot_drawer::MODE_WIRED);
              GPD.draw (geo_mgr.get_factory (), 
                        visu_model_name, 
                        visu_placement, 
                        visu_depth);
              visu_model_name = "";
            }
        } // visu
      else
        {
          std::clog << "NOTICE: " << "No visu..." << std::endl;
        }

      if (gdml)
        {

          std::clog << "NOTICE: " << "Accessing the materials driver plugin..." << std::endl;

          const materials::manager * mat_mgr_ref = 0;
          // Access to a given plugin by name and type :
          std::string  materials_plugin_name = "materials_driver";
          if (geo_mgr.has_plugin (materials_plugin_name) 
              && geo_mgr.is_plugin_a<materials::materials_geom_plugin>(materials_plugin_name))
            {
              std::clog << "NOTICE: " << "Found plugin named '" << materials_plugin_name 
                        << "'" << std::endl;
              const materials::materials_geom_plugin & mgp 
                = geo_mgr.get_plugin<materials::materials_geom_plugin>(materials_plugin_name);
              const materials::manager & mat_mgr = mgp.get_manager();
              mat_mgr_ref = &mat_mgr;
            }

          std::string world_model_name = geo_mgr.get_world_name ();

          geomtools::gdml_export::g_devel = debug;
          geomtools::gdml_writer material_writer; // GDML writer for materials
          geomtools::gdml_export GDML;            // factory->GDML exporter

          if (mat_mgr_ref != 0)
            {         
              std::clog << "NOTICE: " << "Export GDML materials from the materials driver plugin: "<< std::endl;
              //mat_mgr_ref->export_gdml (std::cout);
              mat_mgr_ref->export_gdml (material_writer);
              GDML.attach_external_materials (material_writer.get_stream (geomtools::gdml_writer::MATERIALS_SECTION));
            }

          GDML.add_auxiliary_support (false);
          GDML.add_replica_support (gdml_replica_support);
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
          std::ostringstream gdml_filename_oss;
          if (! tmp_output_dir.empty())
            {
              gdml_filename_oss << tmp_output_dir << '/';
            }
          gdml_filename_oss << "test_manager.gdml";
          GDML.export_gdml (gdml_filename_oss.str(), 
                            geo_mgr.get_factory (), 
                            world_model_name);
        } // GDML
      else
        {
          std::clog << "NOTICE: " << "No GDML..." << std::endl;
        } 

      std::clog << "NOTICE: " << "The end." << std::endl;
    }
  catch (std::exception & x)
    {
      std::cerr << "ERROR: " << x.what () << std::endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      std::cerr << "ERROR: " << "unexpected error!" << std::endl; 
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}

// end of test_materials_geom_plugin.cxx
