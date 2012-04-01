// -*- mode: c++ ; -*- 
// check_geomtools_setup.cxx  
   
#include <cstdlib> 
#include <iostream>
#include <string>
#include <exception>

#include <datatools/utils/utils.h>

#include <geomtools/model_factory.h>
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
      std::string drawer_view = geomtools::gnuplot_drawer::VIEW_3D;
      bool draw = true;
      bool gdml = false;
      bool dump = false;
      std::vector<std::string> setup_filenames;
      std::string model_name; 

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
              else if ((option == "-h") || (option == "--help")) 
                {
                  print_help ();
                  return 0;
                }
              else if (option == "--devel") 
                {
                  devel = true;
                }
              else if (option == "-D" || option == "--no-draw") 
                {
                  draw = false;
                }
              else if (option == "-G" || option == "--with-gdml") 
                {
                  gdml = true;
                }
              else if (option == "-S") 
                {
                  dump = false;
                }
              else if (option == "-xy") 
                {
                  drawer_view = get_drawer_view (option);
                }
              else if (option == "-xz") 
                {
                  drawer_view = get_drawer_view (option);
                }
              else if (option == "-yz") 
                { 
                  drawer_view = get_drawer_view (option);
                }
              else if (option == "-3d") 
                {
                  drawer_view = get_drawer_view (option);
                }
              else if (option == "-m" || option == "--model") 
                {
                  model_name = argv_[++iarg];
                }
              else 
                { 
                  clog << "WARNING: ignoring option '" << option << "'!" << endl; 
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
 
      geomtools::model_factory factory;
      factory.set_debug (debug);

      if (setup_filenames.size () == 0)
        {
          throw std::logic_error ("Missing list of geometry models configuration files !");
        } 
      for (int i = 0; i  < setup_filenames.size(); i++)
        {
          std::string geom_filename = setup_filenames[i];
          datatools::utils::fetch_path_with_env (geom_filename);
          factory.load (geom_filename);
        }
      factory.lock ();

      if (dump) 
        {
          factory.tree_dump (clog, "Geometry model factory:");
        }
      
      // Default model :
      bool has_world = false;
      for (geomtools::models_col_t::const_iterator i 
             = factory.get_models ().begin ();
           i != factory.get_models ().end ();
           i++)
        {
          if (i->second->get_name () == "world")
            {
              has_world = true;
              if (model_name.empty ())
                {
                  model_name = "world";
                }
              break;
            }
        }
      
      if (draw)
        {   
          bool go_on = true;
          do 
            {

              geomtools::placement p;
              p.set (0, 0, 0, 0 * CLHEP::degree, 0 * CLHEP::degree, 0);
              //if (dump) p.tree_dump (clog, "Placement");
          
              std::clog << "List of available geometry models : " << std::endl;
              int count = 0;
              for (geomtools::models_col_t::const_iterator i 
                     = factory.get_models ().begin ();
                   i != factory.get_models ().end ();
                   i++)
                {
                  std::clog << std::setw (25) << std::setiosflags(ios::left) << std::resetiosflags(ios::right) 
                       << i->second->get_name () << " ";
                  count++;
                  if ((count % 3) == 0)  std::clog << std::endl;
                }
              std::clog << std::endl;

              std::clog << "Enter the name of the geometry model to be displayed " << "\n"
                        << " or type '.q'";
              if (! model_name.empty ())
                {
                  std::clog << " [" << model_name << "] : ";
                }
              std::clog << std::endl;

              std::string user;
              std::getline (std::cin, user);
              {
                std::string token;
                std::istringstream token_iss (user);
                token_iss >> token;
                if (token == ".q" || token == ".quit")
                  {
                    go_on = false;
                    break;
                  }
                if (! token.empty () && token[0] != '-')
                  {
                    model_name = token;
                    token_iss >> token;
                  }
                if (! token.empty ())
                  {
                    drawer_view = get_drawer_view (token);
                  }
              }
              if (model_name.empty () && has_world)
                {
                  model_name = "world";
                }
              std::clog << "Name of the model : " << model_name << std::endl;

              geomtools::gnuplot_drawer::g_devel = devel;
              geomtools::gnuplot_drawer GPD;
              GPD.set_view (drawer_view);
              GPD.set_mode (geomtools::gnuplot_drawer::MODE_WIRED);
              GPD.draw (factory, 
                        model_name, 
                        p,  
                        geomtools::gnuplot_drawer::DISPLAY_LEVEL_NO_LIMIT);
            } while (go_on);
        }

      if (gdml)
        {
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
          model_name = "world";
          GDML.export_gdml (fgdml, factory, model_name);
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
  if (option_ == "-xy") 
    {
      drawer_view = geomtools::gnuplot_drawer::VIEW_2D_XY;
    }
  else if (option_ == "-xz") 
    {
      drawer_view = geomtools::gnuplot_drawer::VIEW_2D_XZ;
    }
  else if (option_ == "-yz") 
    { 
      drawer_view = geomtools::gnuplot_drawer::VIEW_2D_YZ;
    }
  else if (option_ == "-3d") 
    {
      drawer_view = geomtools::gnuplot_drawer::VIEW_3D;
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
  std::clog << "Example: \n\n";
  std::clog << "  geomtools_check_setup setup.geom \n\n";
  return;
}

// end of check_geomtools_setup.cxx
