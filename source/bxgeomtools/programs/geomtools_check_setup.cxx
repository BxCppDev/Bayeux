// -*- mode: c++ ; -*-
// geomtools_check_setup.cxx

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <vector>
#include <map>

#include <boost/foreach.hpp>

#include <datatools/utils.h>
#include <datatools/library_loader.h>

#include <geomtools/geomtools_config.h>
#include <geomtools/manager.h>
#include <geomtools/model_factory.h>
#include <geomtools/id_mgr.h>
#include <geomtools/mapping.h>
#include <geomtools/materials_plugin.h>
#include <geomtools/materials_utils.h>

#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

#include <geomtools/gdml_export.h>
#include <geomtools/placement.h>
#include <datatools/bio_guard.h>
#include <mygsl/bio_guard.h>
#include <geomtools/bio_guard.h>

#if GEOMTOOLS_WITH_ROOT_DISPLAY == 1
#include <TGeoManager.h>
#include <TBrowser.h>
#endif // GEOMTOOLS_WITH_ROOT_DISPLAY

#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
std::string get_drawer_view (const std::string & option_);
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

/************************************************************/

void print_help (std::ostream & out_ = std::clog);

void print_list_of_modules (const geomtools::model_factory &,
                            std::ostream & out_ = std::clog);

void print_list_of_gids (const geomtools::manager & mgr_,
                         std::ostream & out_ = std::clog);

/************************************************************/
int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Check geometry setup" << std::endl;

    bool debug = false;
    bool devel = false;

    std::vector<std::string> LL_dlls;
    std::string LL_config;

    std::string geo_mgr_config_file;
    std::vector<std::string> setup_filenames;
    bool dump_factory = false;

#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    bool visu = true;
    std::string visu_drawer_view = geomtools::gnuplot_drawer::VIEW_3D;
    bool visu_drawer_labels = true;
    std::string visu_object_name;
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

    bool gdml = false;
#if GEOMTOOLS_WITH_ROOT_DISPLAY == 1
    bool gdml_root_display = false;
#endif // GEOMTOOLS_WITH_ROOT_DISPLAY

    bool mapping_requested = false;
    std::string categories_filename;
    std::string top_mapping_model_name;
    std::vector<std::string> mapping_only_categories;
    std::vector<std::string> mapping_excluded_categories;
    int32_t mapping_max_depth = geomtools::mapping::NO_MAX_DEPTH;
    std::string materials_plugin_name;

    /*************************************************************************************/

    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];

      if (token[0] == '-' || token[0] == '+') {
        std::string option = token;
        if ((option == "-d") || (option == "--debug")) {
          debug = true;
        } else if ((option == "-h") || (option == "--help"))  {
          print_help ();
          return 0;
        } else if (option == "--devel") {
          devel = true;
        } else if (option == "-c" || option == "--manager-config")  {
          geo_mgr_config_file = argv_[++iarg];
        } else if (option == "-MP" || option == "--materials-plugin") {
          materials_plugin_name = argv_[++iarg];
        } else if (option == "+G" || option == "--with-gdml") {
          gdml = true;
        } else if (option == "-G" || option == "--without-gdml") {
          gdml = false;
        }
#if GEOMTOOLS_WITH_ROOT_DISPLAY == 1
        else if (option == "-GRD" || option == "--gdml-to-root-display") {
          gdml = true;
          gdml_root_display = true;
        }
#endif // GEOMTOOLS_WITH_ROOT_DISPLAY
        else if (option == "+F")  {
          dump_factory = true;
        } else if (option == "-F") {
          dump_factory = false;
        }
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
        else if (option == "+V" || option == "--with-visu") {
          visu = true;
        } else if (option == "-V" || option == "--without-visu") {
          visu = false;
        } else if (option == "-xy" || option == "--visu-view-xy") {
          visu_drawer_view = get_drawer_view (option);
          visu = true;
        } else if (option == "-xz" || option == "--visu-view-xz") {
          visu_drawer_view = get_drawer_view (option);
          visu = true;
        } else if (option == "-yz" || option == "--visu-view-yz")  {
          visu_drawer_view = get_drawer_view (option);
          visu = true;
        } else if (option == "-3d" || option == "--visu-view-3d") {
          visu_drawer_view = get_drawer_view (option);
          visu = true;
        } else if (option == "-3d-free" || option == "--visu-view-3d-free") {
          visu_drawer_view = get_drawer_view (option);
          visu = true;
        } else if (option == "+VL" || option == "--visu-labels") {
          visu_drawer_labels = true;
          visu = true;
        } else if (option == "-VL" || option == "--visu-no-labels") {
          visu_drawer_labels = false;
          visu = true;
        } else if (option == "-VO" || option == "--visu-object") {
          visu_object_name = argv_[++iarg];
        }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
        else if (option == "-l" || option == "--load-dll") {
          LL_dlls.push_back (argv_[++iarg]);
        } else if (option == "+M" || option == "--with-mapping") {
          mapping_requested = true;
        } else if (option == "-M" || option == "--without-mapping") {
          mapping_requested = false;
        } else if (option == "-MC" || option == "--mapping-categories") {
          categories_filename = argv_[++iarg];
          mapping_requested = true;
        } else if (option == "-MD" || option == "--mapping-max-depth") {
          std::istringstream mmd_iss (argv_[++iarg]);
          mmd_iss >> mapping_max_depth;
          if (! mmd_iss) {
            std::clog << "WARNING: invalid mapping max depth value '" << argv_[iarg] << "' !" << std::endl;
            mapping_max_depth = geomtools::mapping::NO_MAX_DEPTH;
          }
          if (mapping_max_depth < 1) {
            mapping_max_depth = geomtools::mapping::NO_MAX_DEPTH;
          }
          mapping_requested = true;
        } else if (option == "-MO" || option == "--mapping-only") {
          mapping_only_categories.push_back (argv_[++iarg]);
          mapping_requested = true;
        } else if (option == "-MX" || option == "--mapping-exclude") {
          mapping_excluded_categories.push_back (argv_[++iarg]);
          mapping_requested = true;
        } else if (option == "-MT" || option == "--mapping-top-model") {
          top_mapping_model_name = argv_[++iarg];
        } else {
          std::clog << "WARNING: ignoring option '" << option << "'!" << std::endl;
          print_help ();
        }
      } else {
        std::string argument = token;
        setup_filenames.push_back (argument);
      }
      iarg++;
    }

    /*************************************************************************************/

    geomtools::i_model::g_devel = devel;

    uint32_t LL_flags = datatools::library_loader::allow_unregistered;
    datatools::library_loader LL (LL_flags, LL_config);
    BOOST_FOREACH (const std::string & dll_name, LL_dlls) {
      std::clog << "NOTICE: " << "Loading DLL '" << dll_name << "'." << std::endl;
      if (LL.load (dll_name) != EXIT_SUCCESS) {
        std::ostringstream message;
        message << "Loading DLL '" << dll_name << "' fails !";
        throw std::logic_error (message.str ());
      }
    }

    /*************************************************************************************/

    // Geometry data structures :
    const geomtools::model_factory * geo_factory_ref = 0;
    geomtools::manager geo_mgr;
    geomtools::model_factory geo_factory;
    bool use_geo_mgr = false;

    if (!geo_mgr_config_file.empty()) {
      geo_mgr.set_debug (debug);
      datatools::properties geo_mgr_config;
      datatools::properties::read_config (geo_mgr_config_file,
                                          geo_mgr_config);
      geo_mgr.initialize (geo_mgr_config);
      geo_factory_ref = &geo_mgr.get_factory ();
      use_geo_mgr = true;
    } else {
      geo_factory.set_debug (debug);
      if (setup_filenames.size () == 0) {
        throw std::logic_error ("Missing list of geometry models configuration files or geometry manager configuration file!");
      }
      for (int i = 0; i  < setup_filenames.size(); i++) {
        std::string geom_filename = setup_filenames[i];
        datatools::fetch_path_with_env (geom_filename);
        geo_factory.load (geom_filename);
      }
      geo_factory.lock ();
      geo_factory_ref = &geo_factory;
    }

    const geomtools::model_factory & geometry_factory = *geo_factory_ref;
    if (dump_factory) {
      geometry_factory.tree_dump (std::clog, "Geometry model factory:");
    }

    /*************************************************************************************/

    // Default model :
    bool has_world = false;
    for (geomtools::models_col_type::const_iterator i
           = geometry_factory.get_models ().begin ();
         i != geometry_factory.get_models ().end ();
         i++) {
      if (i->second->get_name () == geomtools::model_factory::DEFAULT_WORLD_LABEL) {
        has_world = true;
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
        if (visu_object_name.empty ()) {
          visu_object_name = geomtools::model_factory::DEFAULT_WORLD_LABEL;
        }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
        break;
      }
    }

    /*************************************************************************************/

#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    if (visu) {
      //std::clog << "Current drawer view : '" << visu_drawer_view << "'" << std::endl;
      print_list_of_modules(geometry_factory, std::clog);
      bool go_on = true;
      std::string last_visu_object_name;
      /// Browser main loop :
      do {
        if (! last_visu_object_name.empty ()) {
          visu_object_name = last_visu_object_name;
        }
        std::clog << std::endl
                  << "Enter the name of a geometry model (ex: 'world', type '.m' to print the list)" <<  "\n"
                  << "   or the GID of a volume (ex: '[1234:0.1.2]', type '.g' to print the list) " << "\n"
                  << "   or type '.q' to quit ";
        if (! visu_object_name.empty ()) {
          std::clog << " (default : '" << visu_object_name << "')";
        }
        std::clog << " : ";

        std::string line;
        std::getline (std::cin, line);
        bool do_display;
        //<<<
        {
          do_display = false;
          // parse the line :
          std::string token;
          std::istringstream token_iss (line);
          while (token_iss){
            token.clear ();
            token_iss >> token >> std::ws;
            if (token[0] == '#') {
              do_display = true;
              break;
            }
            if (token == ".q" || token == ".quit") {
              go_on = false;
              break;
            }
            if (token == ".m") {
              print_list_of_modules(geometry_factory, std::clog);
              token.clear();
              do_display = true;
              break;
            }
            if (token == ".g") {
              if (use_geo_mgr) {
                print_list_of_gids(geo_mgr, std::clog);
              } else {
                std::clog << "WARNING: Mapping is only supported through a geometry manager !"
                          << std::endl;
              }
              token.clear();
              do_display = true;
              break;
            }
            if (! token.empty ()) {
              if (token[0] != '-') {
                visu_object_name = token;
              } else {
                if (token == "--labels") {
                  visu_drawer_labels = true;
                } else if (token == "--no-labels") {
                  visu_drawer_labels = false;
                } else {
                  visu_drawer_view = get_drawer_view (token);
                }
              }
            }
          }
          if (! go_on) break;
          if (visu_object_name.empty ()) continue;
        }
        if (! do_display) {
          //>>>
          if (visu_object_name.empty ()) {
            if (! last_visu_object_name.empty()) {
              visu_object_name = last_visu_object_name;
            } else if (has_world) {
              visu_object_name = geomtools::model_factory::DEFAULT_WORLD_LABEL;
              //geo_mgr.get_world_name ();
            }
          }
          std::clog << "Display parameters : " << std::endl;
          std::clog << "|-- Object name   : '" << visu_object_name  << "'" << std::endl;
          std::clog << "|-- View          : '" << visu_drawer_view << "'" << std::endl;
          std::clog << "`-- Show labels    : " << visu_drawer_labels << std::endl;

          geomtools::gnuplot_drawer::g_devel = devel;
          geomtools::gnuplot_drawer GPD;
          GPD.set_mode (geomtools::gnuplot_drawer::MODE_WIRED);
          GPD.set_view (visu_drawer_view);
          GPD.set_labels (visu_drawer_labels);
          if (use_geo_mgr) {
            int code = GPD.draw (geo_mgr,
                                 visu_object_name,
                                 geomtools::gnuplot_drawer::DISPLAY_LEVEL_NO_LIMIT);
            if (code != 0) {
              std::cerr << "ERROR: geomtools_check_setup: "
                        << "Cannot display the object with model name or GID : '"
                        << visu_object_name << "' !" << std::endl;

              last_visu_object_name.clear();
            } else {
              last_visu_object_name = visu_object_name;
            }
          } else {
            geomtools::placement root_plcmt;
            root_plcmt.set (0, 0, 0, 0 * CLHEP::degree, 0 * CLHEP::degree, 0);
            GPD.draw (geometry_factory,
                      visu_object_name,
                      root_plcmt,
                      geomtools::gnuplot_drawer::DISPLAY_LEVEL_NO_LIMIT);
            last_visu_object_name = visu_object_name;
          }
          //continue;
        }
        visu_object_name.clear();
      } while (go_on); /// End of browser main loop.
    }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

    /*************************************************************************************/

    // The manager for GID :
    if (use_geo_mgr) {
      if (mapping_requested) {
        geo_mgr.get_mapping ().smart_print (std::cout,
                                            "",
                                            geomtools::mapping::PRINT_TITLE |
                                            geomtools::mapping::PRINT_PAGER);
      }
    } else {
      geomtools::id_mgr gid_manager;
      if (! categories_filename.empty ()) {
        std::string categories_lis = categories_filename;
        datatools::fetch_path_with_env (categories_lis);
        gid_manager.load (categories_lis);
      }
      if (mapping_requested) {
        std::clog << "NOTICE: geomtools_check_setup: " << "Mapping..." << std::endl;

        // The mapping manager :
        geomtools::mapping mapping_manager;
        mapping_manager.set_id_manager (gid_manager);
        datatools::properties mapping_config;
        mapping_config.store ("mapping.max_depth", mapping_max_depth);
        bool can_exclude_categories = true;
        if (mapping_only_categories.size ()) {
          mapping_config.store ("mapping.only_categories", mapping_only_categories);
          can_exclude_categories = false;
        }
        if (can_exclude_categories && mapping_excluded_categories.size ()) {
          mapping_config.store ("mapping.excluded_categories", mapping_excluded_categories);
        }
        mapping_manager.initialize (mapping_config);
        if (top_mapping_model_name.empty ()) {
          top_mapping_model_name = geomtools::model_factory::DEFAULT_WORLD_LABEL;
        }
        if (geometry_factory.get_models ().find (top_mapping_model_name)
            != geometry_factory.get_models ().end ()) {
          mapping_manager.build_from (geometry_factory, top_mapping_model_name);
        } else {
          std::clog << "WARNING: geomtools_check_setup: Cannot find model named '" << top_mapping_model_name << "'" << std::endl;
        }
        mapping_manager.smart_print (std::cout,
                                     "*** ",
                                     geomtools::mapping::PRINT_TITLE |
                                     geomtools::mapping::PRINT_PAGER);
      }
    }

    /*************************************************************************************/

    if (gdml) {
      std::clog << "NOTICE: " << "GDML..." << std::endl;
      std::ostringstream ext_mat_oss;

      geomtools::gdml_export::g_devel = devel;
      geomtools::gdml_export GDML;

      geomtools::gdml_writer material_writer; // GDML writer for materials
      std::clog << "NOTICE: " << "Accessing the materials driver plugin..." << std::endl;
      const materials::manager * mat_mgr_ref = 0;

      if (use_geo_mgr) {
        if (materials_plugin_name.empty()) {
          // We try to find a material plugin associated to the manager :
          typedef geomtools::manager::plugins_dict_type dict_type;
          const dict_type & plugins = geo_mgr.get_plugins ();
          for (dict_type::const_iterator ip = plugins.begin();
               ip != plugins.end();
               ip++) {
            const std::string & plugin_name = ip->first;
            if (geo_mgr.is_plugin_a<geomtools::materials_plugin>(plugin_name)) {
              materials_plugin_name = plugin_name;
              break;
            }
          }
        }
        // Access to a given plugin by name and type :
        if (geo_mgr.has_plugin (materials_plugin_name)
            && geo_mgr.is_plugin_a<geomtools::materials_plugin>(materials_plugin_name)) {
          std::clog << "NOTICE: " << "Found materials plugin named '" << materials_plugin_name
                    << "'" << std::endl;
          const geomtools::materials_plugin & mgp
            = geo_mgr.get_plugin<geomtools::materials_plugin>(materials_plugin_name);
          const materials::manager & mat_mgr = mgp.get_manager();
          mat_mgr_ref = &mat_mgr;
        }
      }
      if (mat_mgr_ref != 0) {
        std::clog << "NOTICE: "
                  << "Export GDML materials from the materials driver plugin: "<< std::endl;
        geomtools::export_gdml (*mat_mgr_ref, material_writer);
        GDML.attach_external_materials (material_writer.get_stream (geomtools::gdml_writer::MATERIALS_SECTION));
      } else {
        std::clog << "WARNING: "
                  << "No material definitions have been attached to the GDML export !"<< std::endl;
      }

      /*
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
      */

      GDML.add_auxiliary_support (false);
      GDML.add_replica_support (true);
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
      std::ostringstream fgdml_oss;
      if (use_geo_mgr) {
        fgdml_oss << geo_mgr.get_setup_label () << '-' << geo_mgr.get_setup_version () << ".gdml";
      } else {
        fgdml_oss << "geomtools_check_setup.gdml";
      }
      std::string fgdml = fgdml_oss.str();

      if (top_mapping_model_name.empty ()) {
        top_mapping_model_name = geomtools::model_factory::DEFAULT_WORLD_LABEL;
      }
      GDML.export_gdml (fgdml, geometry_factory, top_mapping_model_name);
      std::clog << "NOTICE: geomtools_check_setup: GDML file '" << fgdml << "' has been generated !" << std::endl;
#if GEOMTOOLS_WITH_ROOT_DISPLAY == 1
      //std::clog << "DEVEL: geomtools_check_setup: GDML ROOT display : " << gdml_root_display << std::endl;
      if (gdml_root_display) {
        std::clog << "NOTICE: geomtools_check_setup: Importing GDML file '" << fgdml << "' from ROOT TGeoManager..." << std::endl;
        std::ostringstream setup_title_oss;
        setup_title_oss << "Virtual geometry setup '";
        if (use_geo_mgr) {
          setup_title_oss << geo_mgr.get_setup_label () << '-' << geo_mgr.get_setup_version ();
        } else {
          setup_title_oss << "geomtools_check_setup";
        }
        setup_title_oss << "'";
        {
          // ROOT visualization macro file :
          std::string rvm_name = "geomtools_check_setup.C";
          std::ofstream rvm(rvm_name.c_str());
          rvm << "#include <iostream>" << std::endl;
          rvm << "void geomtools_check_setup() {" << std::endl;
          rvm << "TGeoManager * geo = new TGeoManager(\"geo\",\""
              << setup_title_oss.str().c_str() << "\");" << std::endl;
          rvm << "TGeoManager * g2 = geo->Import(\"" << fgdml.c_str() << "\");" << std::endl;
          rvm << "g2->SetVisOption(0);" << std::endl;
          rvm << "g2->SetVisLevel(100);" << std::endl;
          rvm << "g2->GetMasterVolume()->Draw("");" << std::endl;
          rvm << "std::cout << \"Enter '.q' to quit...\n\";" << std::endl;
          rvm << "}" << std::endl;
          rvm.close();

          int errcode = 0;

          std::ostringstream cat_com;
          cat_com << "cat " << rvm_name << std::ends;
          errcode = system(cat_com.str().c_str());

          std::ostringstream rvm_com;
          rvm_com << "root -l " << rvm_name << std::ends;
          errcode = system(rvm_com.str().c_str());
          if (errcode != 0) {
            std::cerr << "ERROR: geomtools_check_setup: Cannot run the ROOT visualization macro '"
                      << rvm_name << "' !" << std::endl;
          }

          std::clog << "NOTICE: geomtools_check_setup: ROOT visualization macro '"
                    << rvm_name << "' has been generated." << std::endl;
        }

        /*
        // This does not work from within plain C++ :
        TGeoManager * geo = new TGeoManager("geo", setup_title_oss.str().c_str());
        TGeoManager * g2 = geo->Import(fgdml.c_str());
        g2->SetVisOption(0);
        g2->SetVisLevel(100);
        g2->GetMasterVolume()->Draw("");
        geomtools::gnuplot_drawer::wait_for_key ();
        */
      }
#endif // GEOMTOOLS_WITH_ROOT_DISPLAY
    }

  }
  catch (std::exception & x) {
    std::cerr << "ERROR: geomtools_check_setup: " << x.what () << std::endl;
    print_help ();
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "ERROR: geomtools_check_setup: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
std::string get_drawer_view (const std::string & option_)
{
  std::string drawer_view = geomtools::gnuplot_drawer::VIEW_3D;
  if (option_ == "-xy" || option_ == "--visu-view-xy")  {
    drawer_view = geomtools::gnuplot_drawer::VIEW_2D_XY;
  } else if (option_ == "-xz" || option_ == "--visu-view-xz")  {
    drawer_view = geomtools::gnuplot_drawer::VIEW_2D_XZ;
  } else if (option_ == "-yz" || option_ == "--visu-view-yz")
    {
      drawer_view = geomtools::gnuplot_drawer::VIEW_2D_YZ;
    } else if (option_ == "-3d" || option_ == "--visu-view-3d")
    {
      drawer_view = geomtools::gnuplot_drawer::VIEW_3D;
    } else if (option_ == "-3d-free" || option_ == "--visu-view-3d-free")
    {
      drawer_view = geomtools::gnuplot_drawer::VIEW_3D_FREE_SCALE;
    } else
    {
      std::clog << "WARNING: geomtools_check_setup: Unknown view option '" << option_ << "' ! Using default 3D view..."
                << std::endl;
    }
  return drawer_view;
}
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

void print_help (std::ostream & out_)
{
  out_ << "geomtools_check_setup -- Check and display geometry models" << std::endl;
  out_ << "\n";
  out_ << "Usage: \n\n";

  out_ << "  geomtools_check_setup [OPTIONS] GEOMFILE [GEOMFILE [...]] \n\n";

  out_ << "Options: \n\n";

  out_ << "   -d|--debug        : Activate debug mode\n";

  out_ << "   -l|--load-dll DLL_NAME :\n";
  out_ << "                       Load a specific DLL\n";

  out_ << "   -c|--manager-config CONFIG_FILENAME: \n"
       << "                       Use configuration file for the geometry manager\n";

  out_ << "   +F                : Detailed print of the factory contents\n";

  out_ << "   -F                : No detailed print of the factory contents\n";

  out_ << "   -MP|--materials-plugin PLUGIN_NAME:\n";
  out_ << "                       Load a specific materials plugin (default: autodetected)\n";

#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
  out_ << "   +V|--with-visu    : Visualize the geometry setup\n";

  out_ << "   -V|--without-visu : Do not visualize the geometry setup (default)\n";

  out_ << "   -xy|--visu-view-xy : \n"
       << "                       Visualization defaults to XY view\n";

  out_ << "   -yz|--visu-view-yz :  \n"
       << "                       Visualization defaults to YZ view\n";

  out_ << "   -xz|--visu-view-xz :  \n"
       << "                       Visualization defaults to XZ view\n";

  out_ << "   -3d|--visu-view-3d :  \n"
       << "                       Visualization defaults to 3D view (default)\n";

  out_ << "   +VL|--visu-labels :  \n"
       << "                       Visualization shows axis and labels (default)\n";

  out_ << "   -VL|--visu-no-labels :  \n"
       << "                       Visualization does not show axis and labels\n";

  out_ << "   -VO|--visu-object MODEL_NAME|VOLUME_GID :\n";
  out_ << "                       Visualization shows a specific geometry model by name\n";
  out_ << "                       or volume by GID (default: autodetected)\n";

#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

  out_ << "   +G|--with-gdml    : Generate GDML output\n";

  out_ << "   -G|--without-gdml : Do not generate GDML output (default)\n";
#if GEOMTOOLS_WITH_ROOT_DISPLAY == 1

  out_ << "   -GRD|--gdml-to-root-display : \n"
       << "                       ROOT display from GDML import\n";
#endif // GEOMTOOLS_WITH_ROOT_DISPLAY

  out_ << "   +M|--with-mapping : Print geometry mapping informations\n";

  out_ << "   -M|--without-mapping : \n"
       << "                       Do not print geometry mapping informations (default)\n";

  out_ << "   Without a manager config file: \n";

  out_ << "   -MC|--mapping-categories CATEGORIES_FILE:\n";

  out_ << "                       Load a specific geometry category file\n";
  out_ << "   -MD|--mapping-max-depth DEPTH  : \n";
  out_ << "                       Maximum depth of the geometry mapping\n";

  out_ << "   -MO|--mapping-only CATEGORY   : \n";
  out_ << "                       Specify a category to be build in the geometry mapping\n";

  out_ << "   -MX|--mapping-exclude CATEGORY   : \n";
  out_ << "                       Specify a category to be excluded from the geometry mapping\n";

  out_ << "   -MT|--mapping-top-model MODEL_NAME : \n"
       << "                       Identify the top-level model for mapping\n";

  out_ << "Examples:\n\n";
  out_ << "  geomtools_check_setup --manager-config ${GEOMTOOLS_DATA_DIR}/testing/config/test-1.0/test_manager.conf \n\n";
  out_ << "  geomtools_check_setup --mapping-categories setup_categories.lis --mapping-max-depth 3 setup.geom \n\n";
  return;
}

void print_list_of_gids (const geomtools::manager & mgr_, std::ostream & out_)
{
  if (!mgr_.is_mapping_available())
    {
      std::clog << "WARNING: " << "print_list_of_gids: "
                << "Sorry! No mapping information is available from the geometry manager!"
                << std::endl;
      return;
    }
  out_ << std::endl
       << "List of available GIDs : " << std::endl;
  int count = 0;
  const geomtools::mapping & the_mapping = mgr_.get_mapping();
  const geomtools::geom_info_dict_type & ginfo_dict = the_mapping.get_geom_infos ();
  for (geomtools::geom_info_dict_type::const_iterator i = ginfo_dict.begin();
       i != ginfo_dict.end();
       i++)
    {
      bool long_name = false;
      size_t max_width = 36;
      const geomtools::geom_info & ginfo = i->second;
      const geomtools::geom_id & gid = ginfo.get_gid ();

      const geomtools::id_mgr::categories_by_type_col_type & cats
        = the_mapping.get_id_manager ().categories_by_type ();
      const geomtools::id_mgr::category_info * cat_info = cats.find(gid.get_type())->second;
      const std::string & category = cat_info->get_category ();
      std::ostringstream oss;
      oss << gid << " in '" << category << "'";
      if (oss.str().size () > max_width) {
        long_name = true;
      }
      if ((count % 2) == 0) {
        out_ << std::endl;
      }
      out_  << "  " << std::setw (max_width)
            << std::setiosflags(std::ios::left)
            << std::resetiosflags(std::ios::right)
            << oss.str() << "  ";
      if (long_name) {
        out_ << std::endl;
        count = 0;
      }
      count++;
    }
  if ((count % 2) == 1) {
    out_ << std::endl;
  }
  out_ << std::endl;
  return;
}

void print_list_of_modules (const geomtools::model_factory & mf_, std::ostream & out_)
{
  out_ << std::endl
       << "List of available geometry models : " << std::endl;
  int count = 0;
  for (geomtools::models_col_type::const_iterator i
         = mf_.get_models ().begin ();
       i != mf_.get_models ().end ();
       i++) {
    bool long_name = false;
    size_t max_width = 32;
    if (i->second->get_name ().size () > max_width) {
      long_name = true;
    }
    if ((count % 2) == 0) {
      out_ << std::endl;
    }
    out_  << "  " << std::setw (max_width)
          << std::setiosflags(std::ios::left)
          << std::resetiosflags(std::ios::right)
          << i->second->get_name () << "  ";
    if (long_name) {
      out_ << std::endl;
      count = 0;
    }
    count++;
  }
  if ((count % 2) == 1)  {
    out_ << std::endl;
  }
  out_ << std::endl;
  return;
}

// end of geomtools_check_setup.cxx
