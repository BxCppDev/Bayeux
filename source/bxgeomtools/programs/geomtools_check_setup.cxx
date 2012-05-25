// -*- mode: c++ ; -*- 
// geomtools_check_setup.cxx  
   
#include <cstdlib> 
#include <iostream>
#include <string>
#include <exception>
#include <vector>
#include <map>

#include <boost/foreach.hpp>

#include <datatools/utils/utils.h>
#include <datatools/utils/library_loader.h>

#include <geomtools/model_factory.h>
#include <geomtools/id_mgr.h>
#include <geomtools/mapping.h>
#include <geomtools/gnuplot_drawer.h>
#include <geomtools/gdml_export.h>
#include <geomtools/placement.h>

std::string get_drawer_view (const std::string & option_);

void print_help ();

int main (int argc_, char ** argv_)
{ 
  using namespace std;

  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Check geometry setup" << endl; 
  
      bool debug = false;
      bool devel = false;

      std::vector<std::string> LL_dlls;
      std::string LL_config;
 
      std::vector<std::string> setup_filenames;
      bool dump_factory = false;

      bool visu = true;
      std::string visu_drawer_view = geomtools::gnuplot_drawer::VIEW_3D;
      bool visu_drawer_labels = true;
      std::string visu_model_name; 

      bool gdml = false;

       bool mapping_requested = false;
      std::string categories_filename;
      std::string top_mapping_model_name;
      std::vector<std::string> mapping_only_categories;
      std::vector<std::string> mapping_excluded_categories;
      int32_t mapping_max_depth = geomtools::mapping::NO_MAX_DEPTH;

      int iarg = 1;
      while (iarg < argc_)
        {
          std::string token = argv_[iarg];

          if (token[0] == '-' || token[0] == '+')
            {
              std::string option = token; 
              if ((option == "-d") || (option == "--debug")) 
                {
                  debug = true;
                }
              else if ((option == "-h") || (option == "--help")) 
                {
                  print_help ();
                  return 0;
                }
              else if (option == "--devel") 
                {
                  devel = true;
                }
              else if (option == "+V" || option == "--with-visu") 
                {
                  visu = true;
                }
              else if (option == "-V" || option == "--without-visu") 
                {
                  visu = false;
                }
              else if (option == "+G" || option == "--with-gdml") 
                {
                  gdml = true;
                }
              else if (option == "-G" || option == "--without-gdml") 
                {
                  gdml = false;
                }
              else if (option == "+F") 
                {
                  dump_factory = true;
                }
              else if (option == "-F") 
                {
                  dump_factory = false;
                }
              else if (option == "-xy" || option == "--visu-view-xy") 
                {
                  visu_drawer_view = get_drawer_view (option);
                  visu = true;
                }
              else if (option == "-xz" || option == "--visu-view-xz") 
                {
                  visu_drawer_view = get_drawer_view (option);
                  visu = true;
               }
              else if (option == "-yz" || option == "--visu-view-yz") 
                { 
                  visu_drawer_view = get_drawer_view (option);
                  visu = true;
                }
              else if (option == "-3d" || option == "--visu-view-3d") 
                {
                  visu_drawer_view = get_drawer_view (option);
                  visu = true;
                }
              else if (option == "-3d-free" || option == "--visu-view-3d-free") 
                {
                  visu_drawer_view = get_drawer_view (option);
                  visu = true;
                }
              else if (option == "+VL" || option == "--visu-labels") 
                {
                  visu_drawer_labels = true;
                  visu = true;
                }
              else if (option == "-VL" || option == "--visu-no-labels") 
                {
                  visu_drawer_labels = false;
                  visu = true;
               }
              else if (option == "-VM" || option == "--visu-model") 
                {
                  visu_model_name = argv_[++iarg];
                }
              else if (option == "-l" || option == "--load-dll") 
                {
                  LL_dlls.push_back (argv_[++iarg]);
                }
              else if (option == "+M" || option == "--with-mapping") 
                {
                  mapping_requested = true;
                }
              else if (option == "-M" || option == "--without-mapping") 
                {
                  mapping_requested = false;
                }
               else if (option == "-MC" || option == "--mapping-categories") 
                {
                  categories_filename = argv_[++iarg];
                  mapping_requested = true;
                }
               else if (option == "-MD" || option == "--mapping-max-depth") 
                {
                  std::istringstream mmd_iss (argv_[++iarg]);
                  mmd_iss >> mapping_max_depth;
                  if (! mmd_iss)
                    {
                      std::clog << "WARNING: invalid mapping max depth value '" << argv_[iarg] << "' !" << std::endl;                 
                      mapping_max_depth = geomtools::mapping::NO_MAX_DEPTH;
                    }
                  if (mapping_max_depth < 1)
                    {
                      mapping_max_depth = geomtools::mapping::NO_MAX_DEPTH;
                    }
                  mapping_requested = true;
                }
              else if (option == "-MO" || option == "--mapping-only") 
                {
                  mapping_only_categories.push_back (argv_[++iarg]);
                  mapping_requested = true;
                }
              else if (option == "-MX" || option == "--mapping-exclude") 
                {
                  mapping_excluded_categories.push_back (argv_[++iarg]);
                  mapping_requested = true;
                }
              else if (option == "-MT" || option == "--mapping-top-model") 
                {
                  top_mapping_model_name = argv_[++iarg];
                }
              else 
                { 
                  std::clog << "WARNING: ignoring option '" << option << "'!" << std::endl; 
                  print_help ();
                }
            }
          else
            {
              std::string argument = token; 
              setup_filenames.push_back (argument);
            }
          iarg++;
        }  
    
      geomtools::i_model::g_devel = devel;
 
      uint32_t LL_flags = datatools::utils::library_loader::allow_unregistered;
      datatools::utils::library_loader LL (LL_flags, LL_config);
      BOOST_FOREACH (const std::string & dll_name, LL_dlls)
        {
          std::clog << "NOTICE: " << "Loading DLL '" << dll_name << "'." << std::endl;
          if (LL.load (dll_name) != EXIT_SUCCESS)
            {
              std::ostringstream message;
              message << "Loading DLL '" << dll_name << "' fails !";
              throw std::logic_error (message.str ());
            }
        }

      geomtools::model_factory geometry_factory;
      geometry_factory.set_debug (debug);

      if (setup_filenames.size () == 0)
        {
          throw std::logic_error ("Missing list of geometry models configuration files !");
        } 
      for (int i = 0; i  < setup_filenames.size(); i++)
        {
          std::string geom_filename = setup_filenames[i];
          datatools::utils::fetch_path_with_env (geom_filename);
          geometry_factory.load (geom_filename);
        }
      geometry_factory.lock ();

      if (dump_factory) 
        {
          geometry_factory.tree_dump (clog, "Geometry model factory:");
        }
      
      // Default model :
      bool has_world = false;
      for (geomtools::models_col_t::const_iterator i 
             = geometry_factory.get_models ().begin ();
           i != geometry_factory.get_models ().end ();
           i++)
        {
          if (i->second->get_name () == geomtools::model_factory::DEFAULT_WORLD_LABEL)
            {
              has_world = true;
              if (visu_model_name.empty ())
                {
                  visu_model_name = geomtools::model_factory::DEFAULT_WORLD_LABEL;
                }
              break;
            }
        }
       
      if (visu)
        {   
          std::clog << "Current drawer view : '" << visu_drawer_view << "'" << std::endl;
          bool go_on = true;
          do 
            {

              geomtools::placement p;
              p.set (0, 0, 0, 0 * CLHEP::degree, 0 * CLHEP::degree, 0);
          
              std::clog << "List of available geometry models : " << std::endl;
              int count = 0;
              for (geomtools::models_col_t::const_iterator i 
                     = geometry_factory.get_models ().begin ();
                   i != geometry_factory.get_models ().end ();
                   i++)
                {
                  bool long_name = false;
                  size_t max_width = 32;
                  if (i->second->get_name ().size () > max_width)
                    {
                      long_name = true;
                    }
                  if ((count % 2) == 0)  
                    {
                      std::clog << std::endl;
                    }
                  std::clog  << "  " << std::setw (max_width) << std::setiosflags(ios::left) << std::resetiosflags(ios::right) 
                       << i->second->get_name () << "  ";
                  if (long_name)
                    {
                      std::clog << std::endl;
                      count = 0;
                    }
                  count++;
                }
              std::clog << std::endl;

              std::clog << std::endl << "Enter the name of the geometry model to be displayed " << "\n"
                        << " or type '.q'";
              if (! visu_model_name.empty ())
                {
                  std::clog << " [" << visu_model_name << "] : ";
                }
              std::clog << std::endl;

              std::string user;
              std::getline (std::cin, user);
              {
                std::string token;
                std::istringstream token_iss (user);
                while (token_iss)
                  {
                    token.clear ();
                    token_iss >> token >> ws;
                    if (token == ".q" || token == ".quit")
                      {
                        go_on = false;
                        break;
                      }
                    if (! token.empty ())
                      {
                        if (token[0] != '-')
                          {
                            visu_model_name = token;
                          }
                        else
                          {
                            if (token == "--labels")
                              {
                                visu_drawer_labels = true;   
                              }
                            else if (token == "--no-labels")
                              {
                                visu_drawer_labels = false;  
                              }
                            else
                              {
                                visu_drawer_view = get_drawer_view (token);
                              }
                          }
                      }             
                  }
                if (! go_on) break;
              }
              if (visu_model_name.empty () && has_world)
                {
                  visu_model_name = "world";
                }
              std::clog << "Name of the model : '" << visu_model_name  << "'" << std::endl;
              std::clog << "|-- View          : '" << visu_drawer_view << "'" << std::endl;
              std::clog << "`-- Labels        : " << visu_drawer_labels << std::endl;

              geomtools::gnuplot_drawer::g_devel = devel;
              geomtools::gnuplot_drawer GPD;
              GPD.set_mode (geomtools::gnuplot_drawer::MODE_WIRED);
              GPD.set_view (visu_drawer_view);
              GPD.set_labels (visu_drawer_labels);
              GPD.draw (geometry_factory, 
                        visu_model_name, 
                        p,  
                        geomtools::gnuplot_drawer::DISPLAY_LEVEL_NO_LIMIT);
            } while (go_on);
        }

      // The manager for GID :
      geomtools::id_mgr  gid_manager;
      if (! categories_filename.empty ())
        {
          std::string categories_lis = categories_filename;
          datatools::utils::fetch_path_with_env (categories_lis);
          gid_manager.load (categories_lis);
        }

      if (mapping_requested)
        {
          std::clog << "NOTICE: " << "Mapping..." << std::endl;
          
          // The mapping manager :
          geomtools::mapping mapping_manager; 
          mapping_manager.set_id_manager (gid_manager);
          datatools::utils::properties mapping_config;
          mapping_config.store ("mapping.max_depth", mapping_max_depth);
          bool can_exclude_categories = true;
          if (mapping_only_categories.size ())
            {
              mapping_config.store ("mapping.only_categories", mapping_only_categories);
              can_exclude_categories = false;
            }
          if (can_exclude_categories && mapping_excluded_categories.size ())
            {
              mapping_config.store ("mapping.excluded_categories", mapping_excluded_categories);
            }
          mapping_manager.initialize (mapping_config);
          if (top_mapping_model_name.empty ())
            {
              top_mapping_model_name = geomtools::model_factory::DEFAULT_WORLD_LABEL;
            }
          if (geometry_factory.get_models ().find (top_mapping_model_name) != geometry_factory.get_models ().end ())
            {
              mapping_manager.build_from (geometry_factory, top_mapping_model_name);
            }
          else
            {
              std::clog << "WARNING: Cannot find model named '" << top_mapping_model_name << "'" << std::endl;
            }
          mapping_manager.smart_print (std::cout, 
                                       "*** ", 
                                       geomtools::mapping::PRINT_TITLE |
                                       geomtools::mapping::PRINT_PAGER);
        }

      if (gdml)
        {
          std::clog << "NOTICE: " << "GDML..." << std::endl;
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
        
            std::map<std::string, double> Air_map; 
            Air_map["Oxygen"] = 0.3;
            Air_map["Nitrogen"] = 0.7;
            writer.add_material ("Air",
                                 "air",
                                 1.29 * CLHEP::mg / CLHEP::cm3,
                                 Air_map);
        
            std::map<std::string, size_t> H2O_map;
            H2O_map["Oxygen"] = 1;
            H2O_map["Hydrogen"] = 2;
            writer.add_material ("Water",
                                 "H2O",
                                 1.0 * CLHEP::g / CLHEP::cm3,
                                 H2O_map);
          }
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
          std::string fgdml = "geomtools_check_setup.gdml";
          if (top_mapping_model_name.empty ())
            {
              top_mapping_model_name = geomtools::model_factory::DEFAULT_WORLD_LABEL;
            }
          GDML.export_gdml (fgdml, geometry_factory, top_mapping_model_name);
          clog << "NOTICE: GDML file '" << fgdml << "' has been generated !" << endl;
        }

    }
  catch (exception & x)
    {
      cerr << "ERROR: " << x.what () << endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "ERROR: " << "unexpected error!" << endl; 
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}

std::string get_drawer_view (const std::string & option_)
{
  std::string drawer_view = geomtools::gnuplot_drawer::VIEW_3D;
  if (option_ == "-xy" || option_ == "--visu-view-xy") 
    {
      drawer_view = geomtools::gnuplot_drawer::VIEW_2D_XY;
    }
  else if (option_ == "-xz" || option_ == "--visu-view-xz") 
    {
      drawer_view = geomtools::gnuplot_drawer::VIEW_2D_XZ;
    }
  else if (option_ == "-yz" || option_ == "--visu-view-yz") 
    { 
      drawer_view = geomtools::gnuplot_drawer::VIEW_2D_YZ;
    }
  else if (option_ == "-3d" || option_ == "--visu-view-3d") 
    {
      drawer_view = geomtools::gnuplot_drawer::VIEW_3D;
    }
  else if (option_ == "-3d-free" || option_ == "--visu-view-3d-free") 
    {
      drawer_view = geomtools::gnuplot_drawer::VIEW_3D_FREE_SCALE;
    }
  else 
    {
      std::clog << "WARNING: get_drawer_view: Unknown view option '" << option_ << "' ! Using default 3D view..."
                << std::endl;
    }
  return drawer_view;
}

void print_help ()
{
  std::clog << "geomtools_check_setup -- Check and display geometry models" << std::endl;
  std::clog << "\n";
  std::clog << "Usage: \n\n";
  std::clog << "  geomtools_check_setup [OPTIONS] GEOMFILE [GEOMFILE [...]] \n\n";
  std::clog << "Options: \n\n";
  std::clog << "   -d|--debug        : Activate debug mode\n";
  std::clog << "   -l|--load-dll DLL_NAME :\n";
  std::clog << "                       Load a specific DLL\n";
  std::clog << "   +F                : Detailed print of the factory contents\n";
  std::clog << "   -F                : No detailed print of the factory contents\n";
  std::clog << "   +V|--with-visu    : Visualize the geometry setup\n";
  std::clog << "   -V|--without-visu : Do not visualize the geometry setup\n";
  std::clog << "   -xy|--visu-view-xy : \n"
            << "                       Visualization defaults to XY view\n";
  std::clog << "   -yz|--visu-view-yz :  \n"
            << "                       Visualization defaults to YZ view\n";
  std::clog << "   -xz|--visu-view-xz :  \n"
            << "                       Visualization defaults to XZ view\n";
  std::clog << "   -3d|--visu--view-3d :  \n"
            << "                       Visualization defaults to 3D view\n";
  std::clog << "   +VL|--visu-labels :  \n"
            << "                       Visualization shows axis and labels\n";
  std::clog << "   -VL|--visu-no-labels :  \n"
            << "                       Visualization does not show axis and labels\n";
  std::clog << "   -VM|--visu-model MODEL_NAME :\n";
  std::clog << "                       Visualization shows a specific geometry model\n";
  std::clog << "   +M|--with-mapping : Build geometry mapping informations\n";
  std::clog << "   -M|--without-mapping : \n"
            << "                       Do not build geometry mapping informations\n";
  std::clog << "   -MC|--mapping-categories CATEGORIES_FILE:\n";
  std::clog << "                       Load a specific geometry category file\n";
  std::clog << "   -MD|--mapping-max-depth DEPTH  : \n";
  std::clog << "                       Maximum depth of the geometry mapping\n";
  std::clog << "   -MO|--mapping-only CATEGORY   : \n";
  std::clog << "                       Specify a category to be build in the geometry mapping\n";
  std::clog << "   -MX|--mapping-exclude CATEGORY   : \n";
  std::clog << "                       Specify a category to be excluded from the geometry mapping\n";
  std::clog << "   -MT|--mapping-top-model MODEL_NAME : \n"
            << "                       Identify the top-level model for mapping\n";
  std::clog << "   +G|--with-gdml    : Generate GDML output\n";
  std::clog << "   -G|--without-gdml : Do not generate GDML output\n";
  std::clog << "Examples:\n\n";
  std::clog << "  geomtools_check_setup --mapping-categories setup_categories.lis --mapping-max-depth 3 setup.geom \n\n";
  return;
}

// end of geomtools_check_setup.cxx
