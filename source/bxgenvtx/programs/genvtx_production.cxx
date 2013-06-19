// -*- mode: c++ ; -*-
// genvtx_production.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <boost/scoped_ptr.hpp>
#include <boost/foreach.hpp>

#include <datatools/properties.h>
#include <datatools/clhep_units.h>
#include <datatools/library_loader.h>

#include <geomtools/geomtools_config.h>
#include <geomtools/display_data.h>
#include <geomtools/box.h>
#include <geomtools/blur_spot.h>
#include <geomtools/manager.h>
#include <geomtools/gnuplot_drawer.h>
#include <geomtools/gnuplot_draw.h>

#include <genvtx/genvtx_config.h>
#include <genvtx/manager.h>

void print_help(std::ostream & out_);

enum action_flag_type {
  ACTION_UNDEFINED = -1,
  ACTION_LIST      =  0x1,
  ACTION_SHOOT     =  0x2,
  ACTION_VISU      =  0x4
};

int main (int argc_, char ** argv_)
{
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  int error_code = EXIT_SUCCESS;
  try {

    int         nshoots = 0;
    std::string vg_name;
    std::string visu_object;
    int         visu_max_counts = 10000;
    bool        dump_gids = false;
    double      tiny = 1.0;
    std::string GeoMgrConfigFile;
    std::string VGMgrConfigFile;
    std::string VtxOutputFile;
    int         action = 0;
    int         prng_seed = mygsl::random_utils::SEED_INVALID;
    std::vector<std::string> LL_dlls;
    std::string LL_config;

    int iarg = 1;
    while (iarg < argc_) {
      std::string token = argv_[iarg];
      if (token[0] == '-') {
        std::string option = token;
        if ((option == "-h") || (option == "--help")) {
          print_help(std::cout);
          return 0;
        } else if (option == "--load-dll") {
          LL_dlls.push_back (argv_[++iarg]);
        } else if (option == "--dll-config") {
          LL_config = argv_[++iarg];
        } else if ((option == "-L") || (option == "--logging-priority")) {
          std::string lp_name = argv_[++iarg];
          logging = datatools::logger::get_priority(lp_name);
          DT_THROW_IF(logging == datatools::logger::PRIO_UNDEFINED,
                      std::logic_error, "Undefined logging priority '"
                      << lp_name << "' !");
        } else if ((option == "-G") || (option == "--geometry-manager")) {
          GeoMgrConfigFile = argv_[++iarg];
        } else if ((option == "-X") || (option == "--vertex-generator-manager")) {
          VGMgrConfigFile = argv_[++iarg];
        } else if ((option == "-l") || (option == "--list")) {
          action |= ACTION_LIST;
        } else if ((option == "-s") || (option == "--shoot")) {
          action |= ACTION_SHOOT;
        } else if ((option == "-g") || (option == "--vertex-generator")) {
          action |= ACTION_SHOOT;
          vg_name = argv_[++iarg];
        } else if ((option == "-n") || (option == "--number-of-vertices")) {
          action |= ACTION_SHOOT;
          std::string n = argv_[++iarg];
          std::istringstream iss(n);
          iss >> nshoots;
          DT_THROW_IF(!iss, std::logic_error, "Invalid '--number-of-vertices' format !");
          DT_THROW_IF(nshoots < 1, std::logic_error, "Invalid '--number-of-vertices' value !");
        } else if ((option == "-S") || (option == "--prng-seed")) {
          action |= ACTION_SHOOT;
          std::string s = argv_[++iarg];
          std::istringstream iss(s);
          iss >> prng_seed;
          DT_THROW_IF(!iss, std::logic_error, "Invalid '--prng-seed' format '" << s << "'!");
          DT_THROW_IF(prng_seed < 0, std::logic_error, "Invalid '--seed' value '" << prng_seed << "' !");
        } else if ((option == "-o") || (option == "--output-file")) {
          action |= ACTION_SHOOT;
          VtxOutputFile = argv_[++iarg];
        } else if ((option == "-v") || (option == "--visu")) {
          action |= ACTION_VISU;
        } else if ((option == "-c") || (option == "--visu-max-counts")) {
          action |= ACTION_VISU;
          std::string n = argv_[++iarg];
          std::istringstream iss(n);
          iss >> visu_max_counts;
          DT_THROW_IF(!iss, std::logic_error, "Invalid '--visu-max-counts' format !");
          DT_THROW_IF(visu_max_counts < 0, std::logic_error, "Invalid '--visu-max-counts' value !");
        } else if ((option == "-V") || (option == "--visu-object")) {
          action |= ACTION_VISU;
          visu_object = argv_[++iarg];
        } else if ((option == "-t") || (option == "--tiny")) {
          tiny *= 0.5;
        } else {
          DT_LOG_WARNING(logging, "Ignoring option '" << option << "' !");
        }
      } else {
        std::string argument = token;
        {
          DT_LOG_WARNING(logging, "Ignoring argument '" << argument << "' !");
        }
      }
      iarg++;
    }

    uint32_t LL_flags = datatools::library_loader::allow_unregistered;
    datatools::library_loader LL(LL_flags, LL_config);
    BOOST_FOREACH (const std::string & dll_name, LL_dlls) {
      DT_LOG_NOTICE(logging,"Loading DLL '" << dll_name << "'...");
      DT_THROW_IF (LL.load(dll_name) != EXIT_SUCCESS,
                   std::runtime_error,
                   "Loading DLL '" << dll_name << "' failed !");
    }

    if (action == 0) {
      DT_LOG_NOTICE(logging, "Perform default action : shoot...");
      action = ACTION_SHOOT;
    }

    // Configuration files :
    DT_THROW_IF(GeoMgrConfigFile.empty(),
                std::logic_error,
                "Missing geometry manager configuration file !");

    if (action & ACTION_LIST || action & ACTION_SHOOT) {
      DT_THROW_IF(VGMgrConfigFile.empty(),
                  std::logic_error,
                  "Missing vertex generator manager configuration file !");
    }

    // Geometry manager :
    geomtools::manager GeoMgr;
    datatools::fetch_path_with_env (GeoMgrConfigFile);
    datatools::properties GeoMgrConfig;
    datatools::properties::read_config (GeoMgrConfigFile, GeoMgrConfig);
    GeoMgr.initialize (GeoMgrConfig);
    if (dump_gids) {
      DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "The embeded GID mapping : ");
      GeoMgr.get_mapping ().dump_dictionnary (std::clog);
    }

    // Vertex generator manager :
    genvtx::manager VGMgr(logging);
    VGMgr.set_geometry_manager(GeoMgr);
    VGMgr.set_rng_id("taus2");
    VGMgr.set_random_seed(prng_seed);
    datatools::fetch_path_with_env (VGMgrConfigFile);
    datatools::properties VGMgrConfig;
    datatools::properties::read_config (VGMgrConfigFile, VGMgrConfig);
    VGMgr.initialize (VGMgrConfig);
    DT_LOG_DEBUG(logging, "The vertex generator manager: ");
    if (logging >= datatools::logger::PRIO_DEBUG) {
      VGMgr.tree_dump(std::clog);
    }

    // A display object that contains vertex rendering infos:
    geomtools::display_data dd;

    if (action & ACTION_LIST) {
      // Prints the list of available vertex generators
      std::vector<std::string> vg_names;
      std::vector<std::string> vg_descriptions;
      std::string vg_current;
      VGMgr.vg_names(vg_names, vg_descriptions, vg_current);
      std::cerr << "List of vertex generators : " << std::endl;
      for (int i = 0; i < vg_names.size(); i++) {
        const std::string & vg_name = vg_names[i];
        if (i < vg_names.size() - 1 ) std::cerr << "|-- ";
        else std::cerr << "`-- ";
        std::cerr << std::flush;
        std::cout << std::flush << vg_name << ' ' << std::flush;
        if (vg_name == vg_current) std::cout << "(current)" << std::flush;
        if (! vg_descriptions[i].empty()) {
          std::cout << " : " << vg_descriptions[i];
        } else {
          std::cout << " : " << "(no description)";
        }
        std::cout << std::endl;
      }
    }

    if (action & ACTION_SHOOT) {
      if (! vg_name.empty()) {
        VGMgr.activate_current_vg(vg_name);
      }
      DT_THROW_IF(nshoots < 1, std::logic_error, "Number of random vertices is invalid !");
      DT_LOG_DEBUG(logging, "Number of random vertices is : " << nshoots);

      // The 3D representation of a vertex :
      double vertex_dim = 1.0*CLHEP::mm;
      vertex_dim *= tiny;
      geomtools::blur_spot vertex_spot(3, 1 *CLHEP::mm);
      vertex_spot.set_errors(vertex_dim,vertex_dim,vertex_dim);
      boost::scoped_ptr<std::ofstream> fout;
      if (! VtxOutputFile.empty()) {
        fout.reset(new std::ofstream);
        std::string ofn = VtxOutputFile;
        datatools::fetch_path_with_env (ofn);
        fout.get()->open(ofn.c_str());
        DT_THROW_IF (! *fout.get(), std::runtime_error,
                     "Cannot open output file '" << ofn << "' !");
        *fout.get() << "# generated by : genvtx_production." << std::endl;
        *fout.get() << "#@geometry.setup.label=" << GeoMgr.get_setup_label() << std::endl;
        *fout.get() << "#@geometry.setup.version=" << GeoMgr.get_setup_version() << std::endl;
        *fout.get() << "#@vertex_generator.name=" << VGMgr.get_generator_name () << std::endl;
        *fout.get() << "#@length_unit=mm" << std::endl;
      }
      int vtx_counter = 0;
      while (vtx_counter < nshoots && VGMgr.can_shoot_vertex()) {
        DT_LOG_TRACE(logging, "Vertex count : " << vtx_counter);
        geomtools::vector_3d vertex_pos;
        VGMgr.shoot_vertex(vertex_pos);
        if (fout) {
          geomtools::gnuplot_draw::basic_draw_point(*fout.get(), (vertex_pos / CLHEP::mm));
        }
        if (action & ACTION_VISU) {
          if (visu_max_counts == 0 || vtx_counter < visu_max_counts) {
            std::ostringstream vertex_name_oss;
            vertex_name_oss << "vertex_" << vtx_counter;
            geomtools::display_data::display_item & vertex_spot_DI
              = dd.add_static_item (vertex_name_oss.str(),
                                    "group::vertex",
                                    "magenta");
            geomtools::placement vertex_plcmt;
            vertex_plcmt.set_translation(vertex_pos);
            vertex_spot.generate_wires (vertex_spot_DI.paths, vertex_plcmt);
          }
        }
        vtx_counter++;
      }
    }

    // Now the rendering :
    if (action & ACTION_VISU) {
      if (visu_object.empty()) {
        visu_object = GeoMgr.get_world_name ();
      }
      DT_LOG_DEBUG(logging,"Visu object label : '" << visu_object << "'");
      geomtools::gnuplot_drawer GPD;
      GPD.grab_properties().store(geomtools::gnuplot_drawer::WORLD_NAME_KEY,
                                  GeoMgr.get_world_name ());
      geomtools::placement dd_pl;
      dd_pl.set_translation (0.0, 0.0*CLHEP::cm, 0.0);
      GPD.add_display_data (dd, dd_pl);
      GPD.set_view (geomtools::gnuplot_drawer::VIEW_3D);
      GPD.set_mode (geomtools::gnuplot_drawer::MODE_WIRED);
      int visu_depth = 100;
      int view_code = GPD.draw (GeoMgr, visu_object, visu_depth);
      if (view_code != 0) {
        DT_LOG_ERROR(logging,"Cannot display the object with label '" << GeoMgr.get_world_name () << "' !");
      }
    }
  }
  catch (std::exception & x) {
    DT_LOG_FATAL(logging, x.what());
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    DT_LOG_FATAL(logging, "Unexpected error !");
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}

void print_help(std::ostream & out)
{
  out << "Usage : genvtx_production [OPTION]... [FILE]...                             \n";
  out << "                                                                            \n";
  out << "  Generate vertices from a vertex generator managed                          \n";
  out << "  by a vertex generator manager and coupled to a                            \n";
  out << "  geometry manager.                                                         \n";
  out << "                                                                            \n";
  out << "Options:                                                                    \n";
  out << "  -h, --help :                                                              \n";
  out << "     print this help then exit.                                             \n";
  out << "  --load-dll LIBRARYNAME :                                                  \n";
  out << "     load the LIBRARYNAME dynamic library                                   \n";
  out << "  --dll-config DLLCONFIGFILE :                                              \n";
  out << "     load the DLLCONFIGFILE dynamic library loader configuration file       \n";
  out << "  -L, --logging-priority PRIORITY :                                         \n";
  out << "     set the logging priority (default=\"fatal\").                          \n";
  out << "  -G, --geometry-manager CONFIGFILE :                                       \n";
  out << "     set the configuration file for the geometry manager.                   \n";
  out << "  -X, --vertex-generator-manager CONFIGFILE :                               \n";
  out << "     set the configuration file for the vertex generator                    \n";
  out << "     manager.                                                               \n";
  out << "  -l, --list :                                                              \n";
  out << "     list the available vertex generators from the                          \n";
  out << "     vertex generator manager (list mode).                                  \n";
  out << "  -s, --shoot :                                                             \n";
  out << "     generate random vertices (shoot mode).                                 \n";
  out << "  -S, --prng-seed SEED :                                                    \n";
  out << "     seed for the pseudo random generator (shoot mode only).                \n";
  out << "  -n, --number-of-vertices NUMBER :                                         \n";
  out << "     set the number of vertices to be generated (shoot mode only).          \n";
  out << "  -g, --vertex-generator NAME :                                             \n";
  out << "     set the name of the active vertex generator (shoot mode only).         \n";
  out << "  -o, --output-file FILENAME :                                              \n";
  out << "     set the name of the ASCII output file (shoot mode only).               \n";
  out << "  -v, --visu :                                                              \n";
  out << "     activate the visualization of generated vertices within the            \n";
  out << "     geometry setup.                                                        \n";
  out << "  -c, --visu-max-counts VISUCOUNTS                                          \n";
  out << "     set the maximum number of vertices to be displayed (visu mode only,    \n";
  out << "     default is 10000)                                                      \n";
  out << "                                                                            \n";
  out << "                                                                            \n";
  out << "Example:                                                                    \n";
  out << "                                                                            \n";
  out << "  List the available vertex generators:                                     \n";
  out << "                                                                            \n";
  out << "     genvtx_production \\                                                   \n";
  out << "       --geometry-manager \"${CONFIG_DIR}/geometry/manager.conf\" \\        \n";
  out << "       --vertex-generator-manager \"${CONFIG_DIR}/vertex/manager.conf\" \\  \n";
  out << "       --list                                                               \n";
  out << "                                                                            \n";
  out << "  Generate some vertices, store then in a file and display:                 \n";
  out << "                                                                            \n";
  out << "     genvtx_production \\                                                   \n";
  out << "       --geometry-manager \"${CONFIG_DIR}/geometry/manager.conf\" \\        \n";
  out << "       --vertex-generator-manager \"${CONFIG_DIR}/vertex/manager.conf\" \\  \n";
  out << "       --shoot \\                                                           \n";
  out << "       --prng-seed 314159 \\                                                \n";
  out << "       --number-of-vertices 10000 \\                                        \n";
  out << "       --vertex-generator \"scintillator_block.vg\" \\                      \n";
  out << "       --output-file \"vertices.data\"    \\                                \n";
  out << "       --visu                                                               \n";
  out << "                                                                            \n";
  out << "                                                                            \n";

}

// end of genvtx_production.cxx
