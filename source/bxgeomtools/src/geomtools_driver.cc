// geomtools_driver.cc

// Ourselves:
#include <geomtools/geomtools_driver.h>

// Standard Library:

// Third party:
// - Boost:
#include <boost/foreach.hpp>
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/library_loader.h>
#include <datatools/exception.h>

// This project:
#include <geomtools/shape_factory.h>
#include <geomtools/model_factory.h>
#include <geomtools/manager.h>
#include <geomtools/id_mgr.h>
#include <geomtools/mapping.h>
#include <geomtools/materials_plugin.h>
#include <geomtools/materials_utils.h>
#include <geomtools/gdml_export.h>
#include <geomtools/placement.h>

#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

// #if GEOMTOOLS_WITH_ROOT_DISPLAY == 1
// #include <TGeoManager.h>
// #include <TBrowser.h>
// #endif // GEOMTOOLS_WITH_ROOT_DISPLAY

#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
std::string get_drawer_view (const std::string & option_);
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

namespace geomtools {

  /* Driver parameters class */

  geomtools_driver_params::geomtools_driver_params()
  {
    logging = datatools::logger::PRIO_WARNING;
    mute = false;
    interactive = true;
    mapping_requested = false;
    mapping_max_depth = geomtools::mapping::NO_MAX_DEPTH;

#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    visu = true;
    visu_drawer_view = geomtools::gnuplot_drawer::view_3d();
    visu_drawer_labels = true;
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

    return;
  }

  void geomtools_driver_params::reset()
  {
    logging = datatools::logger::PRIO_WARNING;
    interactive = true;
    mute = false;
    LL_dlls.clear();
    LL_config.clear();
    geo_mgr_config_file.clear();
    setup_filenames.clear();
    mapping_requested = false;
    categories_filename.clear();
    top_mapping_model_name.clear();
    mapping_only_categories.clear();
    mapping_excluded_categories.clear();
    mapping_max_depth = geomtools::mapping::NO_MAX_DEPTH;
    materials_plugin_name.clear();

#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    visu = true;
    visu_drawer_view = geomtools::gnuplot_drawer::view_3d();
    visu_object_name.clear();
    visu_drawer_labels = true;
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

    return;
  }

  bool geomtools_driver_params::can_initialize() const
  {
    if (! geo_mgr_config_file.empty()) return true;
    if (setup_filenames.size()) return true;
    return false;
  }

  int geomtools_driver_params::parse(const std::vector<std::string> & argv_,
                                     std::ostream & out_)
  {
    int code = 0;
    DT_LOG_TRACE(logging, "Entering...");
    size_t iarg = 0;
    DT_LOG_TRACE(logging, "argv size = " << argv_.size());
    while (iarg < argv_.size()) {
      std::string token = argv_[iarg++];
      DT_LOG_TRACE(logging, "token = '" << token << "'");
      if (token[0] == '-' || token[0] == '+') {
        std::string option = token;
        if ((option == "-h") || (option == "--help") || (option == "-hs") || (option == "--help-short")) {
          if ((option == "-h") || (option == "--help")) {
            out_ << "  Usage:                                  \n";
            out_ << "    initialize [OPTIONS...] [MGRCFG_FILE] \n"
                 << "                                           \n";
            out_ << "  Options:                                \n";
            out_ << "    -h|--help             Print this help \n";
            out_ << "    -hs|--help-short      Print this help (short version) \n";
            out_ << "    --mute                Mute mode (inhibit message)     \n";
          }
          out_ << "  -G|--logging LOGPRIO  Set the logging priority threshold 'LOGPRIO'    \n"
               << "                        Accepted values are: 'trace', 'debug',          \n"
               << "                          'information', 'notice', 'warning',           \n"
               << "                          'error', 'critical', 'fatal'                  \n"
               << "  -B|--batch            Run in batch mode (no user interaction)         \n"
               << "  -I|--interactive      Run in interactive mode (with user interaction) \n"
               << "  -c|--manager-config MGRCFG_FILE                                       \n"
               << "                        Use the configuration file named 'MGRCFG_FILE'  \n"
               << "                        for the geometry manager                        \n"
               << "                        (incompatible with '--geometry-file')           \n"
               << "  -MP|--materials-plugin MATPLG                                         \n"
               << "                        Use the materials plugin named 'MATPLG'         \n"
               << "  -g|--geometry-file GEOM_FILE                                          \n"
               << "                        Use the geometry file named 'GEOM_FILE'         \n"
               << "                        for the plain model factory                     \n"
               << "                        (incompatible with '--manager-config')          \n"
               << "  -l|--load-dll DLL     Load the shared library named 'DLL'             \n";
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
          out_ << "  +V|--with-visu        Visualize the geometry setup                     \n";
          out_ << "  -V|--without-visu     Do not visualize the geometry setup (default)    \n";
          out_ << "  -xy|--visu-view-xy    Visualization defaults to XY view                \n";
          out_ << "  -yz|--visu-view-yz    Visualization defaults to YZ view                \n";
          out_ << "  -xz|--visu-view-xz    Visualization defaults to XZ view                \n";
          out_ << "  -3d|--visu-view-3d    Visualization defaults to 3D view (default)      \n";
          out_ << "  +VL|--visu-labels     Visualization shows axis and labels (default)    \n";
          out_ << "  -VL|--visu-no-labels  Visualization does not show axis and labels      \n";
          out_ << "  -VO|--visu-object MODEL_NAME|VOLUME_GID                                \n";
          out_ << "                        Visualization displays a specific geometry model \n"
               << "                        by name, a logical volume by name or a physical  \n";
          out_ << "                        volume by GID (default: autodetected)            \n";
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
          out_ << "\n";
          out_ << "  MGRCFG_FILE : Set the configuration file for the geometry       \n"
               << "                manager (optional, see also '--manager-config')   \n";
          out_ << std::flush;
          code = -1;
          break;
        } else if ((option == "-G") || (option == "--logging")) {
          std::string logging_label = argv_[iarg++];
          datatools::logger::priority lp = datatools::logger::get_priority(logging_label);
          if (lp == datatools::logger::PRIO_UNDEFINED) {
            DT_LOG_WARNING(logging, "Invalid logging priority '" << logging_label << "' ! Ignore !");
          } else {
            logging = lp;
          }
        } else if ((option == "--mute"))  {
          mute = true;
        } else if ((option == "-B") || (option == "--batch"))  {
          interactive = false;
        } else if ((option == "-I") || (option == "--interactive"))  {
          interactive = true;
        } else if (option == "-c" || option == "--manager-config")  {
          DT_THROW_IF (setup_filenames.size() > 0, std::logic_error,
                       "Option '--manager-config' is not compatible with option '--geometry-file' !");
          geo_mgr_config_file = argv_[iarg++];
        } else if (option == "-MP" || option == "--materials-plugin") {
          materials_plugin_name = argv_[iarg++];
        } else if (option == "-g" || option == "--geometry-file") {
          DT_THROW_IF (! geo_mgr_config_file.empty(), std::logic_error,
                       "Option '--geometry-file' is not compatible with option '--manager-config' !");
          std::string geom_file = argv_[iarg++];
          setup_filenames.push_back(geom_file);
        }
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
        else if (option == "+V" || option == "--with-visu") {
          visu = true;
        } else if (option == "-V" || option == "--without-visu") {
          visu = false;
        } else if (option == "-xy" || option == "--visu-view-xy") {
          visu_drawer_view = geomtools::geomtools_driver::get_drawer_view (option);
          visu = true;
        } else if (option == "-xz" || option == "--visu-view-xz") {
          visu_drawer_view = geomtools::geomtools_driver::get_drawer_view (option);
          visu = true;
        } else if (option == "-yz" || option == "--visu-view-yz")  {
          visu_drawer_view = geomtools::geomtools_driver::get_drawer_view (option);
          visu = true;
        } else if (option == "-3d" || option == "--visu-view-3d") {
          visu_drawer_view = geomtools::geomtools_driver::get_drawer_view (option);
          visu = true;
        } else if (option == "-3d-free" || option == "--visu-view-3d-free") {
          visu_drawer_view = geomtools::geomtools_driver::get_drawer_view (option);
          visu = true;
        } else if (option == "+VL" || option == "--visu-labels") {
          visu_drawer_labels = true;
          visu = true;
        } else if (option == "-VL" || option == "--visu-no-labels") {
          visu_drawer_labels = false;
          visu = true;
        } else if (option == "-VO" || option == "--visu-object") {
          visu_object_name = argv_[iarg++];
        }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
        else if (option == "-l" || option == "--load-dll") {
          LL_dlls.push_back (argv_[iarg++]);
          // } else if (option == "+M" || option == "--with-mapping") {
          //   mapping_requested = true;
          // } else if (option == "-M" || option == "--without-mapping") {
          //   mapping_requested = false;
          // } else if (option == "-MC" || option == "--mapping-categories") {
          //   categories_filename = argv_[iarg++];
          //   mapping_requested = true;
          // } else if (option == "-MD" || option == "--mapping-max-depth") {
          //   std::istringstream mmd_iss (argv_[iarg++]);
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
          //   mapping_only_categories.push_back (argv_[iarg++]);
          //   mapping_requested = true;
          // } else if (option == "-MX" || option == "--mapping-exclude") {
          //   mapping_excluded_categories.push_back (argv_[iarg++]);
          //   mapping_requested = true;
          // } else if (option == "-MT" || option == "--mapping-top-model") {
          //   top_mapping_model_name = argv_[iarg++];
          // }
        } else {
          DT_LOG_DEBUG (logging, "Ignoring option '" << option << "' !");
          // code = 0;
        }
      } else {
        std::string argument = token;
        if (geo_mgr_config_file.empty()) {
          geo_mgr_config_file = argument;
        } else {
          DT_LOG_DEBUG (logging, "Ignoring argument '" << argument << "' !");
          // code = 0;
        }
      }
    }
    DT_LOG_TRACE(logging, "Exiting with code " << code);
    return code;
  }

  /* Driver class */

  geomtools_driver::geomtools_driver()
  {
    _initialized_ = false;
    _geo_factory_ref_    = 0;
    _has_world_   = false;
    return;
  }

  geomtools_driver::~geomtools_driver()
  {
    if (is_initialized()) {
      if (_geo_mgr_) _geo_mgr_.reset(0);
      if (_geo_factory_) _geo_factory_.reset(0);
      //reset();
    }
    return;
  }

  void geomtools_driver::reset()
  {
    DT_THROW_IF(! is_initialized(), std::logic_error, "Driver is not initialized !");
    _initialized_ = false;

    _has_world_ = false;
    _geo_factory_ref_ = 0;
    if (_geo_mgr_) _geo_mgr_.reset(0);
    if (_geo_factory_) _geo_factory_.reset(0);

    _params_.reset();

    return;
  }

  bool geomtools_driver::is_initialized() const
  {
    return _initialized_;
  }

  bool geomtools_driver::has_manager() const
  {
    return is_initialized() && _geo_mgr_;
  }

  void geomtools_driver::initialize(const geomtools_driver_params & params_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Already initialized !");
    _params_ = params_;

    datatools::library_loader LL(_params_.LL_config);
    BOOST_FOREACH (const std::string & dll_name, _params_.LL_dlls) {
      DT_LOG_NOTICE(_params_.logging,"Loading DLL '" << dll_name << "'...");
      DT_THROW_IF (LL.load(dll_name) != EXIT_SUCCESS,
                   std::runtime_error,
                   "Loading DLL '" << dll_name << "' failed !");
    }

    if (! _params_.geo_mgr_config_file.empty()) {
      _geo_mgr_.reset(new manager);
      _geo_mgr_.get()->set_logging_priority (_params_.logging);
      std::string geo_mgr_config_file = _params_.geo_mgr_config_file;
      DT_THROW_IF(! datatools::fetch_path_with_env(geo_mgr_config_file),
                  std::runtime_error,
                  "Cannot fetch path from geometry manager configuration file '"
                  << geo_mgr_config_file << "' !");
      datatools::properties geo_mgr_config;
      datatools::properties::read_config (geo_mgr_config_file,
                                          geo_mgr_config);
      _geo_mgr_.get()->initialize (geo_mgr_config);
      _geo_factory_ref_ = &_geo_mgr_.get()->get_factory ();
    } else {
      _geo_factory_.reset(new model_factory);
      _geo_factory_.get()->set_debug (_params_.logging >= datatools::logger::PRIO_DEBUG);
      DT_THROW_IF (_params_.setup_filenames.size () == 0,
                   std::logic_error,
                   "Missing list of geometry models configuration files or geometry manager configuration file!");
      for (size_t i = 0; i  < _params_.setup_filenames.size(); i++) {
        std::string geom_filename = _params_.setup_filenames[i];
        datatools::fetch_path_with_env (geom_filename);
        _geo_factory_.get()->load (geom_filename);
      }
      _geo_factory_.get()->lock ();
      _geo_factory_ref_ = _geo_factory_.get();
    }

    const model_factory & geometry_factory = *_geo_factory_ref_;
    if (_params_.logging >= datatools::logger::PRIO_DEBUG) {
      _geo_factory_ref_->tree_dump (std::clog, "Geometry model factory:");
    }

    // Default model :
    //bool _has_world_ = false;
    for (models_col_type::const_iterator i
           = geometry_factory.get_models ().begin ();
         i != geometry_factory.get_models ().end ();
         i++) {
      if (i->second->get_name () == geomtools::model_factory::default_world_label()) {
        //_has_world_ = true;
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
        if (_params_.visu_object_name.empty ()) {
          _params_.visu_object_name = geomtools::model_factory::default_world_label();
        }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
        break;
      }
    }

    _initialized_ = true;
    return;
  }
  int geomtools_driver::command_initialize(const std::vector<std::string> & argv_,
                                           std::ostream & /*out_*/)
  {
    int code = 0;
    DT_LOG_TRACE(_params_.logging, "Entering...");
    if (is_initialized()) {
      DT_LOG_ERROR(_params_.logging,
                   "Geometry driver is already initialized !");
      code = 1;
    } else {
      geomtools::geomtools_driver_params GDP;
      int parse_error = GDP.parse(argv_, std::clog);
      if (parse_error < 0) {
        code = -1;
      } else if (parse_error > 0) {
        DT_LOG_ERROR(_params_.logging,
                     "Error parsing geometry driver parameters !");
        code = 1;
      }
      if (code != 0 ) {
        DT_LOG_TRACE(_params_.logging, "Exiting with code " << code);
        return code;
      }

      if (! GDP.can_initialize()) {
        if (! GDP.mute) {
          DT_LOG_ERROR(_params_.logging,
                       "Geometry driver parameters don't allow the driver to be initialized !");
        }
        code = 2;
      } else {
        try {
          this->initialize(GDP);
        } catch (std::exception & x) {
          DT_LOG_ERROR(_params_.logging,
                       "Geometry driver could not be initialized: " << x.what());
          GDP.reset();
          code = 1;
        }
      }
    }
    DT_LOG_TRACE(_params_.logging, "Exiting with code " << code);
    return code;
  }

  int geomtools_driver::command_reset()
  {
    if (! is_initialized()) {
      DT_LOG_ERROR(_params_.logging,
                   "The geometry driver is not initialized and cannot be reset !");
      return 1;
    } else {
      reset();
    }
    return 0;
  }

  int geomtools_driver::command_status(std::ostream & out_) const
  {
    out_ << "Geomtools driver status :" << std::endl;
    out_ << "|-- " << "Initialized: " << _initialized_ << std::endl;
    //out_ << "|-- " << "Parameters: " << _params_ << std::endl;
    //XXX
    if (_geo_mgr_) {
      out_ << "|-- " << "Geometry manager: " << _geo_mgr_.get()  << std::endl;
      out_ << "|   " << "|-- Initialized:             " << _geo_mgr_.get()->is_initialized()  << std::endl;
      out_ << "|   " << "|-- Geometry setup:             '" << _geo_mgr_.get()->get_setup_label() << "'" << std::endl;
      out_ << "|   " << "|-- Geometry setup version:     '" << _geo_mgr_.get()->get_setup_version() << "'" << std::endl;
      out_ << "|   " << "`-- Geometry setup description: '" << _geo_mgr_.get()->get_setup_description() << "'" << std::endl;
    }
    if (_geo_factory_) {
      out_ << "|-- " << "Plain geometry model factory: " << _geo_factory_.get()  << std::endl;
    }
    if (_geo_factory_ref_) {
      out_ << "|-- " << "Effective geometry model factory locked: " << _geo_factory_ref_->is_locked()  << std::endl;
    }
    out_ << "`-- " << "Has world volume: " << _has_world_ << std::endl;
    // out_ << "|-- " << "Visu object name: '" << _visu_object_name_ << "'" << std::endl;
    // out_ << "|-- " << "Visu drawer view: '" << _visu_drawer_view_ << "'" << std::endl;
    // out_ << "`-- " << "Visu drawer labels: " << _visu_drawer_labels_ << std::endl;

    return 0;
  }

  int geomtools_driver::export_gdml(const geomtools::manager & geo_mgr_,
                                    const std::string & materials_plugin_name_,
                                    const std::string & top_mapping_model_name_,
                                    const std::string & gdml_file_,
                                    bool replica_support_,
                                    bool root_display_,
                                    int root_vis_option_,
                                    int /*root_vis_level_*/,
                                    const std::string & root_top_volume_,
                                    datatools::logger::priority logging_)
  {
    DT_LOG_NOTICE(logging_, "Exporting GDML...");
    std::ostringstream ext_mat_oss;
    geomtools::gdml_export GDML;
    std::string materials_plugin_name = materials_plugin_name_;
    std::string top_mapping_model_name = top_mapping_model_name_;
    bool replica_support = replica_support_;
    if (replica_support && root_display_) {
      DT_LOG_WARNING(logging_,"Replica support is not compatible with ROOT export!");
      DT_LOG_WARNING(logging_,"Inhibit replica support...");
      replica_support = false;
    }
    if (top_mapping_model_name.empty ()) {
      top_mapping_model_name = geomtools::model_factory::default_world_label();
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
      GDML.attach_external_materials (material_writer.get_stream (geomtools::gdml_writer::materials_section()));
    } else {
      DT_LOG_WARNING(logging_,"No material definitions have been attached to the GDML export !");
    }

    GDML.add_auxiliary_support (false);
    GDML.add_replica_support (replica_support);
    GDML.parameters ().store ("xml_version",
                              geomtools::gdml_writer::default_xml_version());
    GDML.parameters ().store ("xml_encoding",
                              geomtools::gdml_writer::default_xml_encoding());
    GDML.parameters ().store ("gdml_schema",
                              geomtools::gdml_writer::default_gdml_schema());
    GDML.parameters ().store ("length_unit",
                              geomtools::gdml_export::default_length_unit());
    GDML.parameters ().store ("angle_unit",
                              geomtools::gdml_export::default_angle_unit());
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
      top_mapping_model_name = geomtools::model_factory::default_world_label();
    }
    GDML.export_gdml (fgdml, geometry_factory, top_mapping_model_name);
    DT_LOG_NOTICE(logging_, "GDML file '" << fgdml << "' has been generated !");
    std::cerr << "GDML file '" << fgdml << "' has been generated !" << std::endl;

#if GEOMTOOLS_WITH_ROOT_DISPLAY == 1
    if (root_display_) {
      unsigned int vis_option = root_vis_option_;
      std::string  top_volume = root_top_volume_;
      if (! root_top_volume_.empty() && root_top_volume_.length() > 2) {
        if (root_top_volume_[0] == '"' && root_top_volume_[root_top_volume_.length()-1] == '"') {
          top_volume = root_top_volume_.substr(1,root_top_volume_.length()-2);
        }
      }
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
        rvm << "g2->SetVisOption(" << vis_option << ");" << std::endl;
        rvm << "g2->SetVisLevel(100);" << std::endl;
        if (! root_top_volume_.empty()) {
          rvm << "TGeoVolume * top_volume = g2->GetVolume(\"" << top_volume << "\");" << std::endl;
          rvm << "if (top_volume != 0) { g2->SetTopVolume(top_volume); top_volume->Draw();}" << std::endl;
        } else {
          rvm << "g2->GetMasterVolume()->Draw("");" << std::endl;
        }
        rvm << "std::cout << \"Enter '.q' to quit...\n\";" << std::endl;
        rvm << "}" << std::endl;
        rvm.close();

        int errcode = 0;

        // DEVEL: cat the macro:
        // std::ostringstream cat_com;
        // cat_com << "cat " << rvm_name << std::ends;
        // errcode = system(cat_com.str().c_str());

        std::ostringstream rvm_com;
        rvm_com << "root -l " << rvm_name << std::ends;
        errcode = system(rvm_com.str().c_str());
        if (errcode != 0) {
          DT_LOG_ERROR(logging_, "Cannot run the ROOT visualization macro '" << rvm_name << "' !");
        }
        DT_LOG_NOTICE(logging_, "ROOT visualization macro '" << rvm_name << "' has been generated.");
      }
#endif // GEOMTOOLS_WITH_ROOT_DISPLAY
    }
    return 0;
  }

  /*
  int geomtools_driver::command_print_list_of_shapes(std::ostream & out_,
                                                     const std::string & print_shapes_options_) const
  {
    if (! is_initialized()) {
      DT_LOG_ERROR(_params_.logging, "Geometry driver is not initialized !");
      return 1;
    }
    return geomtools::shape_factory::print_list_of_shapes(*_shape_factory_ref_,
                                                          out_,
                                                          print_shapes_options_);
  }
  */

  int geomtools_driver::command_print_list_of_models(std::ostream & out_,
                                                     const std::string & print_models_options_) const
  {
    if (! is_initialized()) {
      DT_LOG_ERROR(_params_.logging, "Geometry driver is not initialized !");
      return 1;
    }
    return geomtools::model_factory::print_list_of_models(*_geo_factory_ref_,
                                                          out_,
                                                          print_models_options_);
  }

  int geomtools_driver::command_print_list_of_logicals(std::ostream & out_,
                                                       const std::string & print_logicals_options_) const
  {
    if (! is_initialized()) {
      DT_LOG_ERROR(_params_.logging, "Geometry driver is not initialized !");
      return 1;
    }
    return geomtools::model_factory::print_list_of_logicals(*_geo_factory_ref_,
                                                            out_,
                                                            print_logicals_options_);
  }

  int geomtools_driver::command_print_list_of_gids(std::ostream & out_,
                                                   const std::string & print_gids_options_) const
  {
    if (! is_initialized()) {
      DT_LOG_ERROR(_params_.logging, "Geometry driver is not initialized !");
      return 1;
    }
    if (! _geo_mgr_) {
      DT_LOG_ERROR(_params_.logging, "No embeded geometry manager !");
      return 1;
    }
    return geomtools::manager::print_list_of_gids(*_geo_mgr_.get(),
                                                  out_,
                                                  print_gids_options_);
  }

  int geomtools_driver::command_print_manager(std::ostream & out_) const
  {
    if (! is_initialized()) {
      DT_LOG_ERROR(_params_.logging, "Geometry driver is not initialized !");
      return 1;
    }
    if (_geo_mgr_) _geo_mgr_.get()->tree_dump(out_, "Geometry manager : ");
    return 0;
  }

  int geomtools_driver::command_print_mapping(std::ostream & out_,
                                              const std::string & mapping_name_) const
  {
    if (! is_initialized()) {
      DT_LOG_ERROR(_params_.logging, "Geometry driver is not initialized !");
      return 1;
    }
   try {
      const geomtools::mapping & gmpg
        = _geo_mgr_.get()->get_mapping(mapping_name_);
      std::ostringstream mapping_title;
      mapping_title << "Geometry mapping ";
      mapping_title << '(' << mapping_name_ << ')';
      mapping_title << " : ";
      out_ << mapping_title << std::endl;
      gmpg.smart_print(out_);
    } catch (std::exception & error) {
      DT_LOG_ERROR(_params_.logging, "Cannot find mapping with name '"
                   << mapping_name_ << "' !");
      return 1;
    }
    return 0;
  }

  int geomtools_driver::command_print_factory(std::ostream & out_) const
  {
    if (! is_initialized()) {
      DT_LOG_ERROR(_params_.logging, "Geometry driver is not initialized !");
      return 1;
    }
    _geo_factory_ref_->tree_dump(out_, "Geometry model factory : ");
    return 0;
  }

  int geomtools_driver::command_print_model(const std::string & model_name_,
                                            std::ostream & out_) const
  {
    if (! is_initialized()) {
      DT_LOG_ERROR(_params_.logging, "Geometry driver is not initialized !");
      return 1;
    }
    out_ << std::endl << "Geometry model '" << model_name_ << "' : " << std::endl;
    geomtools::models_col_type::const_iterator found
      = _geo_factory_ref_->get_models ().find(model_name_);
    if (found == _geo_factory_ref_->get_models ().end ()) {
      DT_LOG_ERROR(_params_.logging,
                   "Geometry model '" << model_name_ << "' does not exist !");
      return 1;
    }
    const geomtools::i_model & a_model = *found->second;
    a_model.tree_dump(out_, "");
    return 0;
  }

  int geomtools_driver::command_print_logical(const std::string & logical_name_,
                                      std::ostream & out_) const
  {
    if (! is_initialized()) {
      DT_LOG_ERROR(_params_.logging, "Driver is not initialized !");
      return 1;
    }
    out_ << std::endl << "Logical volume '" << logical_name_ << "' : " << std::endl;
    geomtools::logical_volume::dict_type::const_iterator found
      = _geo_factory_ref_->get_logicals ().find(logical_name_);
    if (found == _geo_factory_ref_->get_logicals ().end ()) {
      DT_LOG_ERROR(_params_.logging,
                   "Logical volume '" << logical_name_ << "' does not exist !");
      return 1;
    }
    const geomtools::logical_volume & a_logical = *found->second;
    a_logical.tree_dump(out_, "");
    return 0;
  }

  int geomtools_driver::command_export_gdml(const std::vector<std::string> & argv_,
                                            std::ostream & out_) const
  {
    if (! is_initialized()) {
      DT_LOG_ERROR(_params_.logging, "Driver is not initialized !");
      return 1;
    }
    if (! has_manager()) {
      DT_LOG_ERROR(_params_.logging, "No manager !");
      return 1;
    }
    std::string gdml_file;
    bool replica_support = true;
    bool root_display   = false;
    int root_vis_option = 1;
    int root_vis_level  = 100;
    std::string root_top_volume = "";
    size_t argcount = 0;
    while (argcount < argv_.size()) {
      const std::string & token = argv_[argcount++];
      if (token.empty()) break;
      if (token[0] == '-') {
        std::string option = token;
        if (option == "-h" || option == "--help") {
          out_ << "  Usage: \n";
          out_ << "    export_gdml [OPTIONS...] [GDML_FILE] \n"
               << "\n";
          out_ << "  Options: \n";
          out_ << "    -h | --help           Print this help\n"
               << "    --with-replica        With replica support\n"
               << "    --without-replica     Without replica support\n"
               << "    --no-root-display     Do not display the GDML geometry from ROOT\n"
               << "    --root-display        Display the GDML geometry from ROOT\n"
               << "                          (interactive mode only)\n"
               << "    --root-vis-option OPT Use ROOT visualization option OPT\n"
               << "    --root-vis-level LVL  Use ROOT visualization level LVL\n"
               << "    --root-top-volume VOL ROOT visualization of the volume VOL\n"
               << "\n";
          out_ << "  GDML_FILE : The name of the GDML file to be generated (optional)\n";
          out_ << std::flush;
          return -1;
        } else if (option  == "--root-display") {
          root_display = true;
        } else if (option  == "--with-replica") {
          replica_support = true;
        } else if (option  == "--without-replica") {
          replica_support = false;
        } else if (option == "--no-root-display") {
          root_display = false;
        } else if (option == "--root-vis-option") {
          std::istringstream iss(argv_[argcount++]);
          iss >> root_vis_option;
          if (! iss) {
            DT_LOG_ERROR(_params_.logging, "Invalid format for '" << "--root-vis-option" << "' !");
            root_vis_option = 0;
          }
          if (root_vis_option < 0) {
            DT_LOG_ERROR(_params_.logging, "Invalid value for '" << "--root-vis-option" << "' !");
            root_vis_option = 0;
          }
          root_display = true;
        } else if (option == "--root-vis-level") {
          std::istringstream iss(argv_[argcount++]);
          iss >> root_vis_level;
          if (! iss) {
            DT_LOG_ERROR(_params_.logging, "Invalid format for '" << "--root-vis-level" << "' !");
            root_vis_level = 100;
          }
          if (root_vis_option < 0) {
            DT_LOG_ERROR(_params_.logging, "Invalid value for '" << "--root-vis-level" << "' !");
            root_vis_level = 100;
          }
          root_display = true;
        } else if (option == "--root-top-volume") {
          root_top_volume = argv_[argcount++];
          root_display = true;
        } else {
          DT_LOG_ERROR(_params_.logging, "Invalid option '" << token << "' !");
          return -1;
        }
      } else {
        std::string argument = token;
        if (gdml_file.empty()) {
          gdml_file = argument;
        } else {
          DT_LOG_ERROR(_params_.logging, "Invalid argument '" << argument << "' !");
          return -1;
        }
      }
    } // while
    if (! _params_.interactive && root_display) {
      DT_LOG_ERROR(_params_.logging, "Root visualization is inhibited !");
      root_display = false;
    }
    return export_gdml(*_geo_mgr_.get(),
                       _params_.materials_plugin_name,
                       _params_.top_mapping_model_name,
                       gdml_file,
                       replica_support,
                       root_display,
                       root_vis_option,
                       root_vis_level,
                       root_top_volume,
                       _params_.logging);
  }

#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
  int geomtools_driver::command_gnuplot_display(const std::vector<std::string> & argv_,
                                                std::ostream & out_)
  {
    int error_code = 0;
    if (! is_initialized()) {
      DT_LOG_ERROR(_params_.logging, "Driver is not initialized !");
      error_code = 1;
      return error_code;
    }
    if (! _params_.visu) {
      DT_LOG_ERROR(_params_.logging, "Visualization is inhibited !");
      error_code = 1;
      return error_code;
    }
    std::string visu_object_name;
    std::string output;
    std::string terminal;
    std::string terminal_options;
    bool using_title = true;
    size_t argcount = 0;
    while (argcount < argv_.size()) {
      const std::string & token = argv_[argcount++];
      if (token.empty()) break;
      if (token[0] == '-') {
        std::string option = token;
        if (option  == "-h" || option  == "--help") {
          out_ << "   Usage: \n";
          out_ << "     display [OPTIONS...] [NAME] \n"
               << "\n";
          out_ << "   Options: \n";
          out_ << "     -h | --help           Print this help\n"
               << "     -xy|--visu-view-xy    Use X-Y 2D projection\n"
               << "     -xz|--visu-view-xz    Use X-Z 2D projection\n"
               << "     -yz|--visu-view-yz    Use Y-Z 2D projection\n"
               << "     -3d|--visu-view-3d    Use 3D view \n"
               << "     -3d-free|--visu-view-3d-free Use 3D view (free scale)\n"
               << "     --labels              Display axis with labels\n"
               << "     --no-labels           Do not display axis with labels\n"
               << "     --title               Display image title\n"
               << "     --no-title            Do not display image title\n"
               << "     -o|--output OUT_FILE  Print in the 'OUT_FILE' file\n"
               // << "     -t|--terminal TERM    Use terminal 'TERM'\n"
               // << "     -to|--terminal-options TERMOPT \n"
               // << "                           Use terminal options 'TERMOPT'\n"
               << "\n";
          out_ << "   NAME : The name of the object to be displayed (optional)\n";
          out_ << "          It can be:                       \n";
          out_ << "          - the name of a geometry model,  \n";
          out_ << "          - the name of a logical volume,  \n";
          out_ << "          - the GID associated to a volume \n";
          out_ << "            by the active mapping.         \n";
          out_ << std::flush;
          return -1;
        } else if (option  == "--labels") {
          _params_.visu_drawer_labels = true;
        } else if (option == "--no-labels") {
          _params_.visu_drawer_labels = false;
        } else if (option  == "--title") {
          using_title = true;
        } else if (option == "--no-title") {
          using_title = false;
        } else if (option == "-o" || option == "--output") {
          output = argv_[argcount++];
        } else {
          _params_.visu_drawer_view = get_drawer_view (option);
        }
      } else {
        std::string argument = token;
        if (visu_object_name.empty()) {
          visu_object_name = argument;
        } else {
          DT_LOG_ERROR(_params_.logging, "Invalid argument '" << argument << "' !");
          return -1;
        }
      }
    } // while

    if (visu_object_name.empty()) {
      if (! _params_.visu_object_name.empty()) {
        visu_object_name = _params_.visu_object_name;
      } else if (_has_world_) {
        visu_object_name = geomtools::model_factory::default_world_label();
      }
    }

    DT_LOG_DEBUG(_params_.logging, "Display parameters : ");
    if (_params_.logging >= datatools::logger::PRIO_DEBUG) {
      std::clog << "|-- Object name   : '" << visu_object_name  << "'" << std::endl;
      std::clog << "|-- View          : '" << _params_.visu_drawer_view << "'" << std::endl;
      std::clog << "`-- Show labels    : " << _params_.visu_drawer_labels << std::endl;
    }
    geomtools::gnuplot_drawer GPD;
    GPD.set_mode(geomtools::gnuplot_drawer::mode_wired());
    GPD.set_view (_params_.visu_drawer_view);
    if (! output.empty() || ! terminal.empty()) {
      GPD.set_output_medium (output, terminal, terminal_options);
    }
    GPD.set_using_title (using_title);
    GPD.set_drawing_display_data (true);
    GPD.set_labels(_params_.visu_drawer_labels);
    if (_params_.logging >= datatools::logger::PRIO_DEBUG) {
      GPD.print(std::clog);
    }
    if (has_manager()) {
      int code = GPD.draw (*_geo_mgr_.get(),
                           visu_object_name,
                           geomtools::gnuplot_drawer::DISPLAY_LEVEL_NO_LIMIT);
      if (code != 0) {
        DT_LOG_ERROR(_params_.logging,
                     "Cannot display the object with model name or GID : '"
                     << _params_.visu_object_name << "' !");
        _params_.visu_object_name.clear();
        error_code = 1;
      } else {
        _params_.visu_object_name = visu_object_name;
      }
    } else {
      try {
        geomtools::placement root_plcmt;
        root_plcmt.set (0, 0, 0, 0 * CLHEP::degree, 0 * CLHEP::degree, 0);
        GPD.draw_model(*_geo_factory_ref_,
                       visu_object_name,
                       root_plcmt,
                       geomtools::gnuplot_drawer::DISPLAY_LEVEL_NO_LIMIT);
        _params_.visu_object_name = visu_object_name;
      }
      catch (std::exception & error) {
        DT_LOG_ERROR(_params_.logging, error.what());
        _params_.visu_object_name.clear();
        error_code = 1;
      }
      catch (...) {
        DT_LOG_ERROR(_params_.logging, "Unexpected error !");
        _params_.visu_object_name.clear();
        error_code = 1;
      }
    }
    return error_code;
  }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY


#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
  std::string geomtools_driver::get_drawer_view (const std::string & view_label_)
  {
    std::string drawer_view = geomtools::gnuplot_drawer::view_3d();
    if (view_label_ == "-xy" || view_label_ == "--visu-view-xy")  {
      drawer_view = geomtools::gnuplot_drawer::view_2d_xy();
    } else if (view_label_ == "-xz" || view_label_ == "--visu-view-xz")  {
      drawer_view = geomtools::gnuplot_drawer::view_2d_xz();
    } else if (view_label_ == "-yz" || view_label_ == "--visu-view-yz") {
      drawer_view = geomtools::gnuplot_drawer::view_2d_yz();
    } else if (view_label_ == "-3d" || view_label_ == "--visu-view-3d") {
      drawer_view = geomtools::gnuplot_drawer::view_3d();
    } else if (view_label_ == "-3d-free" || view_label_ == "--visu-view-3d-free") {
      drawer_view = geomtools::gnuplot_drawer::view_3d_free_scale();
    } else {
      DT_LOG_WARNING(datatools::logger::PRIO_WARNING,
                     "Unknown view label '" << view_label_ << "' ! Using default 3D view...");
    }
    return drawer_view;
  }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

} // end of namespace geomtools
