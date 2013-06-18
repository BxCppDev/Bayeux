// -*- mode: c++ ; -*-
// geomtools_inspector.cxx

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
#include <datatools/exception.h>
#include <datatools/logger.h>

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

void print_shell_help (std::ostream & out_ = std::clog);

int print_model (const geomtools::model_factory &,
                 const std::string & model_name_,
                 std::ostream & out_ = std::clog);

int print_logical (const geomtools::model_factory &,
                   const std::string & logical_name_,
                   std::ostream & out_ = std::clog);

int export_gdml(const geomtools::manager & geo_mgr_,
                const std::string & materials_plugin_name_ = "",
                const std::string & top_mapping_model_name_ = "",
                const std::string & gdml_file_= "",
                bool root_display_ = false,
                datatools::logger::priority logging_ = datatools::logger::PRIO_WARNING);

/************************************************************/
int main (int argc_, char ** argv_)
{
  datatools::logger::priority logging = datatools::logger::PRIO_WARNING;

  int error_code = EXIT_SUCCESS;
  try {

    std::vector<std::string> LL_dlls;
    std::string LL_config;
    std::string prompt = "geomtools> ";
    std::string prompt_continue = "> ";
    std::string geo_mgr_config_file;
    std::vector<std::string> setup_filenames;
    bool dump_factory = false;
    bool splash = true;
    bool interactive = true;
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
        if ((option == "-g") || (option == "--logging")) {
          std::string logging_label = argv_[++iarg];
          datatools::logger::priority lp = datatools::logger::get_priority(logging_label);
          if (lp == datatools::logger::PRIO_UNDEFINED) {
            DT_LOG_WARNING(logging, "Invalid logging priority '" << logging_label << "' ! Ignore !");
          } else {
            logging = lp;
          }
        } else if ((option == "-h") || (option == "--help"))  {
          print_help ();
          return 0;
        } else if ((option == "-B") || (option == "--batch"))  {
          interactive = false;
        } else if ((option == "-I") || (option == "--interactive"))  {
          interactive = true;
        } else if ((option == "-S") || (option == "--no-splash"))  {
          splash = false;
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
        // } else if (option == "+M" || option == "--with-mapping") {
        //   mapping_requested = true;
        // } else if (option == "-M" || option == "--without-mapping") {
        //   mapping_requested = false;
        // } else if (option == "-MC" || option == "--mapping-categories") {
        //   categories_filename = argv_[++iarg];
        //   mapping_requested = true;
        // } else if (option == "-MD" || option == "--mapping-max-depth") {
        //   std::istringstream mmd_iss (argv_[++iarg]);
        //   mmd_iss >> mapping_max_depth;
        //   if (! mmd_iss) {
        //     DT_LOG_WARNING (logging, "Invalid mapping max depth value '" << argv_[iarg] << "' ! Ignore !");
        //     mapping_max_depth = geomtools::mapping::NO_MAX_DEPTH;
        //   }
        //   if (mapping_max_depth < 1) {
        //     mapping_max_depth = geomtools::mapping::NO_MAX_DEPTH;
        //   }
        //   mapping_requested = true;
        // } else if (option == "-MO" || option == "--mapping-only") {
        //   mapping_only_categories.push_back (argv_[++iarg]);
        //   mapping_requested = true;
        // } else if (option == "-MX" || option == "--mapping-exclude") {
        //   mapping_excluded_categories.push_back (argv_[++iarg]);
        //   mapping_requested = true;
        // } else if (option == "-MT" || option == "--mapping-top-model") {
        //   top_mapping_model_name = argv_[++iarg];
        } else {
          DT_LOG_WARNING (logging, "Ignoring option '" << option << "' !");
          print_help ();
        }
      } else {
        std::string argument = token;
        setup_filenames.push_back (argument);
      }
      iarg++;
    }
    if (! interactive) {
      visu = false;
      splash = false;
    }
    if (splash) {
      std::cerr << "                                           \n"
                << "\tG E O M T O O L S                        \n"
                << "\tVersion " << GEOMTOOLS_LIB_VERSION << "  \n"
                << "                                           \n"
                << "\tCopyright (C) 2009-2013                  \n"
                << "\tFrancois Mauger, Xavier Garrido and Ben Morgan\n"
                << "                                           \n"
                << "\timmediate help: type \"help\"            \n"
                << "\tquit:           type \"quit\"            \n";
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      std::cerr << "\tsupport:        Gnuplot display          \n";
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
#if GEOMTOOLS_WITH_ROOT_DISPLAY == 1
      std::cerr << "\tsupport:        Root display             \n";
#endif // GEOMTOOLS_WITH_ROOT_DISPLAY
      std::cerr << "                                           \n";
    }

    /*************************************************************************************/

    uint32_t LL_flags = datatools::library_loader::allow_unregistered;
    datatools::library_loader LL(LL_flags, LL_config);
    BOOST_FOREACH (const std::string & dll_name, LL_dlls) {
      DT_LOG_NOTICE(logging,"Loading DLL '" << dll_name << "'...");
      DT_THROW_IF (LL.load(dll_name) != EXIT_SUCCESS,
                   std::runtime_error,
                   "Loading DLL '" << dll_name << "' failed !");
    }

    /*************************************************************************************/

    // Geometry data structures :
    const geomtools::model_factory * geo_factory_ref = 0;
    geomtools::manager geo_mgr;
    geomtools::model_factory geo_factory;
    bool use_geo_mgr = false;

    if (!geo_mgr_config_file.empty()) {
      geo_mgr.set_logging_priority (logging);
      datatools::properties geo_mgr_config;
      datatools::properties::read_config (geo_mgr_config_file,
                                          geo_mgr_config);
      geo_mgr.initialize (geo_mgr_config);
      geo_factory_ref = &geo_mgr.get_factory ();
      use_geo_mgr = true;
    } else {
      geo_factory.set_debug (logging >= datatools::logger::PRIO_DEBUG);
      DT_THROW_IF (setup_filenames.size () == 0,
                   std::logic_error,
                   "Missing list of geometry models configuration files or geometry manager configuration file!");
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

    if (interactive) {
      DT_LOG_DEBUG(logging,"Current drawer view : '" << visu_drawer_view << "'");
      DT_LOG_DEBUG(logging,"Available geometry models : ");
      if (logging >= datatools::logger::PRIO_DEBUG) {
        geomtools::model_factory::print_list_of_models(geometry_factory, std::cerr);
      }
      bool go_on = true;
      std::string last_visu_object_name;
      /// Browser main loop :
      do {
        // Prompt:
        std::cerr << prompt << std::flush;
        if (! last_visu_object_name.empty ()) {
          visu_object_name = last_visu_object_name;
        }
        std::string line;
        std::getline (std::cin, line);
        {
          // Skip blank line and lines starting with '#' :
          std::istringstream dummy(line);
          std::string word;
          dummy >> word;
          if (word.empty()) continue;
          if (word[0] == '#') continue;
        }
        // Manage continuation marks :
        while (line[line.length()-1] == '\\') {
          std::cerr << prompt_continue << std::flush;
          std::string more;
          std::getline (std::cin, more);
          line += more;
        }
        // Interpreter block:
        {
          // Parse the line :
          std::istringstream command_iss (line);
          std::string command;
          command_iss >> command >> std::ws;
          if (command == "h" || command == "help") {
            print_shell_help(std::cerr);
          } else if (command == "q" || command == "quit") {
            go_on = false;
          } else if (command == "m" || command == "model") {
            std::string model_name;
            command_iss >> model_name >> std::ws;
            print_model(geometry_factory, model_name, std::cout);
            model_name.clear();
          } else if (command == "l" || command == "logical") {
            std::string logical_name;
            command_iss >> logical_name >> std::ws;
            print_logical(geometry_factory, logical_name, std::cout);
            logical_name.clear();
          } else if (command == "M" || command == "list_of_models") {
            std::string print_models_options;
            std::getline(command_iss, print_models_options);
            int error = geomtools::model_factory::print_list_of_models(geometry_factory, std::cout, print_models_options);
            if (error > 0) {
              DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                           "Invalid options '" << print_models_options << "' !");
            }
          } else if (command == "L" || command == "list_of_logicals") {
            std::string print_logical_options;
            std::getline(command_iss, print_logical_options);
            int error = geomtools::model_factory::print_list_of_logicals(geometry_factory, std::cout, print_logical_options);
            if (error > 0) {
              DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                           "Invalid options '" << print_logical_options << "' !");
            }
          } else if (command == "G" || command == "list_of_gids") {
            std::string print_gids_options;
            std::getline(command_iss, print_gids_options);
            if (use_geo_mgr) {
              int error = geomtools::manager::print_list_of_gids(geo_mgr, std::clog, print_gids_options);
              if (error > 0) {
                DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Invalid options '" << print_gids_options << "' !");
              }
            } else {
              DT_LOG_WARNING(logging, "Sorry ! Mapping is only supported by a geometry manager !");
            }
            command.clear();
          } else if (command == "x" || command == "export_gdml") {
            if (use_geo_mgr) {
              std::string gdml_file;
              bool root_display = false;
              while (command_iss) {
                std::string token;
                command_iss >> token >> std::ws;
                if (token.empty()) break;
                if (token[0] == '-') {
                  std::string option = token;
                  if (option  == "--root-display") {
                    root_display = true;
                  } else if (option == "--no-root-display") {
                    root_display = false;
                  }
                } else {
                  std::string argument = token;
                  if (gdml_file.empty()) {
                    gdml_file = argument;
                  }
                }
                if (command_iss.eof()) break;
              } // end of parsing
              if (! visu && root_display) {
                std::cerr << "error: " << "'" << command << "'" << " : Root visualization is inhibited !" << std::endl;
                root_display = false;
              }
              int error = export_gdml(geo_mgr, materials_plugin_name, top_mapping_model_name, gdml_file, root_display, logging);
              if (error > 0) {
                DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Cannot export GDML file '" << gdml_file << "' !");
              }
            } else {
              DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Plain factory does not export GDML file !");
            }
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
          } else if (command == "d" || command == "display") {
            // Parse options and arguments:
            while (command_iss) {
              std::string token;
              command_iss >> token >> std::ws;
              if (token.empty()) break;
              if (token[0] == '-') {
                std::string option = token;
                if (option  == "--labels") {
                  visu_drawer_labels = true;
                } else if (option == "--no-labels") {
                  visu_drawer_labels = false;
                } else {
                  visu_drawer_view = get_drawer_view (option);
                }
              } else {
                std::string argument = token;
                visu_object_name = argument;
              }
              if (command_iss.eof()) break;
            } // end of parsing

            if (visu_object_name.empty()) {
              if (! last_visu_object_name.empty()) {
                visu_object_name = last_visu_object_name;
              } else if (has_world) {
                visu_object_name = geomtools::model_factory::DEFAULT_WORLD_LABEL;
              }
            }

            if (visu) {
              DT_LOG_DEBUG(logging, "Display parameters : ");
              if (logging >= datatools::logger::PRIO_NOTICE) {
                std::clog << "|-- Object name   : '" << visu_object_name  << "'" << std::endl;
                std::clog << "|-- View          : '" << visu_drawer_view << "'" << std::endl;
                std::clog << "`-- Show labels    : " << visu_drawer_labels << std::endl;
              }
              geomtools::gnuplot_drawer GPD;
              GPD.set_mode (geomtools::gnuplot_drawer::MODE_WIRED);
              GPD.set_view (visu_drawer_view);
              GPD.set_labels (visu_drawer_labels);
              if (use_geo_mgr) {
                int code = GPD.draw (geo_mgr,
                                     visu_object_name,
                                     geomtools::gnuplot_drawer::DISPLAY_LEVEL_NO_LIMIT);
                if (code != 0) {
                  DT_LOG_ERROR(logging, "Cannot display the object with model name or GID : '"
                               << visu_object_name << "' !");
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
            } else {
              std::cerr << "error: " << "'" << command << "'" << " : Gnuplot visualization is inhibited !" << std::endl;
            }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
          } else {
            // Invalid command:
            if (! interactive) {
              DT_THROW_IF(! interactive, std::runtime_error, "'" << command << "'" << " : Invalid command !");
            } else {
              std::cerr << "error: " << "'" << command << "'" << " : Invalid command !" << std::endl;
            }
          }
          if (! go_on) break;
          if (visu_object_name.empty ()) continue;
          visu_object_name.clear();
        } // End of interpreter block.
      } while (go_on); /// End of browser main loop.
    } // end of interactive

      /*************************************************************************************/

      // The manager for GID :
      /*
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
      DT_LOG_WARNING(logging, "Cannot find model named '" << top_mapping_model_name << "'");
      }
      mapping_manager.smart_print (std::cout,
      "",
      geomtools::mapping::PRINT_TITLE |
      geomtools::mapping::PRINT_PAGER);
      }
      }
    */

    /*************************************************************************************/


    // if (gdml) {
    //   DT_LOG_NOTICE(logging,"Exporting GDML...");
    //   std::ostringstream ext_mat_oss;

    //   geomtools::gdml_export GDML;

    //   geomtools::gdml_writer material_writer; // GDML writer for materials
    //   DT_LOG_NOTICE(logging,"Accessing the materials driver plugin...");
    //   const materials::manager * mat_mgr_ref = 0;
    //   if (use_geo_mgr) {
    //     if (materials_plugin_name.empty()) {
    //       // We try to find a material plugin associated to the manager :
    //       typedef geomtools::manager::plugins_dict_type dict_type;
    //       const dict_type & plugins = geo_mgr.get_plugins ();
    //       for (dict_type::const_iterator ip = plugins.begin();
    //            ip != plugins.end();
    //            ip++) {
    //         const std::string & plugin_name = ip->first;
    //         if (geo_mgr.is_plugin_a<geomtools::materials_plugin>(plugin_name)) {
    //           materials_plugin_name = plugin_name;
    //           break;
    //         }
    //       }
    //     }
    //     // Access to a given plugin by name and type :
    //     if (geo_mgr.has_plugin (materials_plugin_name)
    //         && geo_mgr.is_plugin_a<geomtools::materials_plugin>(materials_plugin_name)) {
    //       DT_LOG_NOTICE(logging,"Found materials plugin named '" << materials_plugin_name << "'");
    //       const geomtools::materials_plugin & mgp
    //         = geo_mgr.get_plugin<geomtools::materials_plugin>(materials_plugin_name);
    //       const materials::manager & mat_mgr = mgp.get_manager();
    //       mat_mgr_ref = &mat_mgr;
    //     }
    //   }
    //   if (mat_mgr_ref != 0) {
    //     DT_LOG_NOTICE(logging,"Exporting GDML materials from the materials driver plugin... ");
    //     geomtools::export_gdml (*mat_mgr_ref, material_writer);
    //     GDML.attach_external_materials (material_writer.get_stream (geomtools::gdml_writer::MATERIALS_SECTION));
    //   } else {
    //     DT_LOG_WARNING(logging,"No material definitions have been attached to the GDML export !");
    //   }

    //   /*
    //     geomtools::gdml_writer writer;
    //     {
    //     writer.add_element ("Hydrogen", 1, "H", 1);
    //     writer.add_element ("Oxygen", 8, "O", 16);
    //     writer.add_element ("Carbon", 6, "C", 12);
    //     writer.add_element ("Nitrogen", 7, "N", 14);
    //     writer.add_element ("Lead", 82, "Pb", 1);

    //     writer.add_material ("Al",
    //     13.0,
    //     2.70 * CLHEP::g / CLHEP::cm3,
    //     26.98);

    //     std::map<std::string, double> Air_map;
    //     Air_map["Oxygen"] = 0.3;
    //     Air_map["Nitrogen"] = 0.7;
    //     writer.add_material ("Air",
    //     "air",
    //     1.29 * CLHEP::mg / CLHEP::cm3,
    //     Air_map);

    //     std::map<std::string, size_t> H2O_map;
    //     H2O_map["Oxygen"] = 1;
    //     H2O_map["Hydrogen"] = 2;
    //     writer.add_material ("Water",
    //     "H2O",
    //     1.0 * CLHEP::g / CLHEP::cm3,
    //     H2O_map);
    //     }
    //   */

    //   GDML.add_auxiliary_support (false);
    //   GDML.add_replica_support (true);
    //   GDML.parameters ().store ("xml_version",
    //                             geomtools::gdml_writer::DEFAULT_XML_VERSION);
    //   GDML.parameters ().store ("xml_encoding",
    //                             geomtools::gdml_writer::DEFAULT_XML_ENCODING);
    //   GDML.parameters ().store ("gdml_schema",
    //                             geomtools::gdml_writer::DEFAULT_GDML_SCHEMA);
    //   GDML.parameters ().store ("length_unit",
    //                             geomtools::gdml_export::DEFAULT_LENGTH_UNIT);
    //   GDML.parameters ().store ("angle_unit",
    //                             geomtools::gdml_export::DEFAULT_ANGLE_UNIT);
    //   std::ostringstream fgdml_oss;
    //   if (use_geo_mgr) {
    //     fgdml_oss << geo_mgr.get_setup_label () << '-' << geo_mgr.get_setup_version () << ".gdml";
    //   } else {
    //     fgdml_oss << "geomtools_inspector.gdml";
    //   }
    //   std::string fgdml = fgdml_oss.str();

    //   if (top_mapping_model_name.empty ()) {
    //     top_mapping_model_name = geomtools::model_factory::DEFAULT_WORLD_LABEL;
    //   }
    //   GDML.export_gdml (fgdml, geometry_factory, top_mapping_model_name);
    //   DT_LOG_NOTICE(logging, "GDML file '" << fgdml << "' has been generated !");

    /*
#if GEOMTOOLS_WITH_ROOT_DISPLAY == 1
      if (gdml_root_display) {
        DT_LOG_NOTICE(logging, "Importing GDML file '" << fgdml << "' from ROOT TGeoManager...");
        std::ostringstream setup_title_oss;
        setup_title_oss << "Virtual geometry setup '";
        if (use_geo_mgr) {
          setup_title_oss << geo_mgr.get_setup_label () << '-' << geo_mgr.get_setup_version ();
        } else {
          setup_title_oss << "geomtools_inspector";
        }
        setup_title_oss << "'";
        {
          // ROOT visualization macro file :
          std::string rvm_name = "geomtools_inspector.C";
          std::ofstream rvm(rvm_name.c_str());
          rvm << "#include <iostream>" << std::endl;
          rvm << "void geomtools_inspector() {" << std::endl;
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
            DT_LOG_ERROR(logging, "Cannot run the ROOT visualization macro '" << rvm_name << "' !");
          }
          DT_LOG_NOTICE(logging, "ROOT visualization macro '" << rvm_name << "' has been generated.");
        }


        // // This does not work from within plain C++ :
        // TGeoManager * geo = new TGeoManager("geo", setup_title_oss.str().c_str());
        // TGeoManager * g2 = geo->Import(fgdml.c_str());
        // g2->SetVisOption(0);
        // g2->SetVisLevel(100);
        // g2->GetMasterVolume()->Draw("");
        // geomtools::gnuplot_drawer::wait_for_key ();

      }
#endif // GEOMTOOLS_WITH_ROOT_DISPLAY
      //   }
*/

  }
  catch (std::exception & x) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL, "geomtools_inspector: " << x.what ());
    print_help ();
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    DT_LOG_FATAL(datatools::logger::PRIO_FATAL, "geomtools_inspector: " << "unexpected error !");
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

/*****************************************************************************/

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
      std::clog << "WARNING: geomtools_inspector: Unknown view option '" << option_ << "' ! Using default 3D view..."
                << std::endl;
    }
  return drawer_view;
}
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

void print_help (std::ostream & out_)
{
  out_ << "geomtools_inspector -- Check and display geometry models" << std::endl;
  out_ << "\n";
  out_ << "Usage: \n\n";

  out_ << "  geomtools_inspector [OPTIONS] GEOMFILE [GEOMFILE [...]] \n\n";

  out_ << "Options: \n\n";

  out_ << "   -g|--logging LOGGING_PRIORITY : Set the logging priority threshold\n";

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

//   out_ << "   +G|--with-gdml    : Generate GDML output\n";

//   out_ << "   -G|--without-gdml : Do not generate GDML output (default)\n";
// #if GEOMTOOLS_WITH_ROOT_DISPLAY == 1

//   out_ << "   -GRD|--gdml-to-root-display : \n"
//        << "                       ROOT display from GDML import\n";
// #endif // GEOMTOOLS_WITH_ROOT_DISPLAY

  // out_ << "   +M|--with-mapping : Print geometry mapping informations\n";

  // out_ << "   -M|--without-mapping : \n"
  //      << "                       Do not print geometry mapping informations (default)\n";

  // out_ << "   Without a manager config file: \n";

  // out_ << "   -MC|--mapping-categories CATEGORIES_FILE:\n";

  // out_ << "                       Load a specific geometry category file\n";
  // out_ << "   -MD|--mapping-max-depth DEPTH  : \n";
  // out_ << "                       Maximum depth of the geometry mapping\n";

  // out_ << "   -MO|--mapping-only CATEGORY   : \n";
  // out_ << "                       Specify a category to be build in the geometry mapping\n";

  // out_ << "   -MX|--mapping-exclude CATEGORY   : \n";
  // out_ << "                       Specify a category to be excluded from the geometry mapping\n";

  // out_ << "   -MT|--mapping-top-model MODEL_NAME : \n"
  //      << "                       Identify the top-level model for mapping\n";

  out_ << "Examples:\n\n";
  out_ << "\tgeomtools_inspector --manager-config ${GEOMTOOLS_DATA_DIR}/testing/config/test-1.0/test_manager.conf \n\n";
  //out_ << "  geomtools_inspector --mapping-categories setup_categories.lis --mapping-max-depth 3 setup.geom \n\n";
  return;
}

int print_model (const geomtools::model_factory & mf_,
                 const std::string & model_name_,
                 std::ostream & out_)
{
  out_ << std::endl
       << "Geometry model '" << model_name_ << "' : " << std::endl;
  geomtools::models_col_type::const_iterator found
    = mf_.get_models ().find(model_name_);
  if (found ==  mf_.get_models ().end ()) {
    DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                 "Geometry model '" << model_name_ << "' does not exist !");
    return 1;
  }
  const geomtools::i_model & a_model = *found->second;
  a_model.tree_dump(out_, "");
  return 0;
}

int print_logical (const geomtools::model_factory & mf_,
                   const std::string & logical_name_,
                   std::ostream & out_)
{
  out_ << std::endl
       << "Logical volume '" << logical_name_ << "' : " << std::endl;
  geomtools::logical_volume::dict_type::const_iterator found
    = mf_.get_logicals ().find(logical_name_);
  if (found ==  mf_.get_logicals ().end ()) {
    DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                 "Logical volume '" << logical_name_ << "' does not exist !");
    return 1;
  }
  const geomtools::logical_volume & a_logical = *found->second;
  a_logical.tree_dump(out_, "");
  return 0;
}

void print_shell_help (std::ostream & out_)
{
  out_ <<  "\th | help                       : Print this help                                  \n";
  out_ <<  "\tM | list_of_models [OPTIONS]   : Print the list of geometry models                \n";
  out_ <<  "\tL | list_of_logicals [OPTIONS] : Print the list of logical volumes                \n";
  out_ <<  "\tG | list_of_gids  [OPTIONS]    : Print the list of geometry identifiers (GID)     \n";
  out_ <<  "\tm | model MODEL_NAME           : Print the geometry model named 'MODEL_NAME'      \n";
  out_ <<  "\tl | logical LOGICAL_NAME       : Print the logical volume named 'LOGICAL_NAME'    \n";
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
  out_ <<  "\td | display [-3d|-xy|-yz|xz] NAME :                                               \n";
  out_ <<  "\t                                 Display the object named 'NAME' where            \n";
  out_ <<  "\t                                 'NAME' can be:                                   \n";
  out_ <<  "\t                                  - the name of a geometry model,                 \n";
  out_ <<  "\t                                  - the name of a logical volume,                 \n";
  out_ <<  "\t                                  - the GID associated to a volume                \n";
  out_ <<  "\t                                    by the active mapping.                        \n";
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
  out_ <<  "\tx | export_gdml [--root-display] GDML_FILENAME  :                                 \n";
  out_ <<  "\t                                 Generate the GDML file 'GDML_FILENAME'.          \n";
  out_ <<  "\t                                 The '--root-display' option triggers a           \n";
  out_ <<  "\t                                 Root interactive visualization session.          \n";
  out_ <<  "\tq | quit                       : Quit                                             \n";
  return;
}

int export_gdml(const geomtools::manager & geo_mgr_,
                const std::string & materials_plugin_name_,
                const std::string & top_mapping_model_name_,
                const std::string & gdml_file_,
                bool root_display_,
                datatools::logger::priority logging_)
{
  DT_LOG_NOTICE(logging_,"Exporting GDML...");
  std::ostringstream ext_mat_oss;
  geomtools::gdml_export GDML;
  std::string materials_plugin_name = materials_plugin_name_;
  std::string top_mapping_model_name = top_mapping_model_name_;
  if (top_mapping_model_name.empty ()) {
    top_mapping_model_name = geomtools::model_factory::DEFAULT_WORLD_LABEL;
  }
  geomtools::gdml_writer material_writer; // GDML writer for materials
  DT_LOG_NOTICE(logging_,"Accessing the materials driver plugin...");
  const materials::manager * mat_mgr_ref = 0;
  if (materials_plugin_name.empty()) {
    // We try to find a material plugin associated to the manager :
    typedef geomtools::manager::plugins_dict_type dict_type;
    const dict_type & plugins = geo_mgr_.get_plugins ();
    for (dict_type::const_iterator ip = plugins.begin();
         ip != plugins.end();
         ip++) {
      const std::string & plugin_name = ip->first;
      if (geo_mgr_.is_plugin_a<geomtools::materials_plugin>(plugin_name)) {
        materials_plugin_name = plugin_name;
        break;
      }
    }
  }
  // Access to a given plugin by name and type :
  if (geo_mgr_.has_plugin (materials_plugin_name)
      && geo_mgr_.is_plugin_a<geomtools::materials_plugin>(materials_plugin_name)) {
    DT_LOG_NOTICE(logging_,"Found materials plugin named '" << materials_plugin_name << "'");
    const geomtools::materials_plugin & mgp
      = geo_mgr_.get_plugin<geomtools::materials_plugin>(materials_plugin_name);
    const materials::manager & mat_mgr = mgp.get_manager();
    mat_mgr_ref = &mat_mgr;
  }
  if (mat_mgr_ref != 0) {
    DT_LOG_NOTICE(logging_,"Exporting GDML materials from the materials driver plugin... ");
    geomtools::export_gdml (*mat_mgr_ref, material_writer);
    GDML.attach_external_materials (material_writer.get_stream (geomtools::gdml_writer::MATERIALS_SECTION));
  } else {
    DT_LOG_WARNING(logging_,"No material definitions have been attached to the GDML export !");
  }

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
  if (! gdml_file_.empty()) {
    std::string gdml_file = gdml_file_;
    datatools::fetch_path_with_env(gdml_file);
    fgdml_oss << gdml_file;
  } else {
    fgdml_oss << geo_mgr_.get_setup_label () << '-' << geo_mgr_.get_setup_version () << ".gdml";
  }
  std::string fgdml = fgdml_oss.str();
  const geomtools::model_factory & geometry_factory = geo_mgr_.get_factory ();

  if (top_mapping_model_name.empty ()) {
    top_mapping_model_name = geomtools::model_factory::DEFAULT_WORLD_LABEL;
  }
  GDML.export_gdml (fgdml, geometry_factory, top_mapping_model_name);
  DT_LOG_NOTICE(logging_, "GDML file '" << fgdml << "' has been generated !");

  if (root_display_) {
    DT_LOG_NOTICE(logging_, "Importing GDML file '" << fgdml << "' from ROOT TGeoManager...");
    std::ostringstream setup_title_oss;
    setup_title_oss << "Virtual geometry setup '";
    setup_title_oss << geo_mgr_.get_setup_label () << '-' << geo_mgr_.get_setup_version ();
    setup_title_oss << "'";
    {
      // ROOT visualization macro file :
      std::string rvm_name = "geomtools_inspector.C";
      std::ofstream rvm(rvm_name.c_str());
      rvm << "#include <iostream>" << std::endl;
      rvm << "void geomtools_inspector() {" << std::endl;
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
        DT_LOG_ERROR(logging_, "Cannot run the ROOT visualization macro '" << rvm_name << "' !");
      }
      DT_LOG_NOTICE(logging_, "ROOT visualization macro '" << rvm_name << "' has been generated.");
    }
  }
  return 0;
}

// end of geomtools_inspector.cxx
