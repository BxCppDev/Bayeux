// genvtx_driver.cc

// Ourselves:
#include <genvtx/genvtx_driver.h>

// Third Party:
// - Boost:
#include <boost/scoped_ptr.hpp>
#include <boost/foreach.hpp>
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/clhep_units.h>
#include <datatools/properties.h>
#include <datatools/clhep_units.h>
#include <datatools/library_loader.h>
#include <datatools/configuration/io.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>
// - Bayeux/geomtools:
#include <geomtools/geomtools_config.h>
#include <geomtools/display_data.h>
#include <geomtools/box.h>
#include <geomtools/blur_spot.h>
#include <geomtools/manager.h>
#include <geomtools/gnuplot_drawer.h>
#include <geomtools/gnuplot_draw.h>

// This project:
#include <genvtx/genvtx_config.h>
#include <genvtx/version.h>
#include <genvtx/manager.h>
#include <genvtx/genvtx_driver.h>

namespace genvtx {

  genvtx_driver_params::genvtx_driver_params()
  {
    reset();
    return;
  }

  void genvtx_driver_params::reset()
  {
    // General parameters:
    logging_label = "warning";
    help = false;

    // Initialization parameters:
    LL_dlls.clear();
    LL_config.clear();
    GeoMgrConfigFile.clear();
    VGMgrConfigFile.clear();

    // Action parameters:
    // - List:
    action_list = false;
    //dump_gids = false;

    // - Show | shoot
    generator_name.clear();

    // - Show
    action_show = false;

    // - Shoot:
    action_shoot = false;
    prng_type.clear();
    prng_seed = mygsl::random_utils::SEED_INVALID;
    nshoots = 0;
    VtxOutputFile.clear();
    VtxInputFile.clear();

    // - Visu:
    action_visu = false;
    visu_spot_zoom = 1.0;
    visu_spot_size = 1.0 * CLHEP::mm;
    visu_object.clear();
    visu_max_counts = 10000;

    return;
  }

  void genvtx_driver_params::dump() const
  {
    std::cerr << "genvtx driver parameters : " << std::endl;
    std::cerr << " - logging_label    = " << logging_label << std::endl;
    std::cerr << " - LL_dlls          = " << LL_dlls.size() << std::endl;
    std::cerr << " - LL_config        = '" << LL_config << "'" << std::endl;
    std::cerr << " - GeoMgrConfigFile = '" << GeoMgrConfigFile << "'" << std::endl;
    std::cerr << " - VGMgrConfigFile  = '" << VGMgrConfigFile << "'" << std::endl;
    std::cerr << " - action_list      = " << action_list << std::endl;
    std::cerr << " - generator_name   = '" << generator_name << "'" << std::endl;
    std::cerr << " - action_show      = " << action_show << std::endl;
    std::cerr << " - action_shoot     = " << action_shoot << std::endl;
    std::cerr << " - prng_seed        = " << prng_seed << std::endl;
    std::cerr << " - nshoots          = " << nshoots << std::endl;
    std::cerr << " - VtxOutputFile    = '" << VtxOutputFile << "'" << std::endl;
    std::cerr << " - action_visu      = " << action_visu << std::endl;
    std::cerr << " - visu_spot_zoom   = " << visu_spot_zoom << std::endl;
    std::cerr << " - visu_spot_size   = " << visu_spot_size << std::endl;
    std::cerr << " - visu_object      = '" << visu_object << "'" << std::endl;
    std::cerr << " - visu_max_counts  = '" << visu_max_counts << "'" << std::endl;
    return;
  }

  // ------------------------------------------------

  genvtx_driver::ui_access::ui_access(genvtx_driver & driver_)
  {
    _driver_ = &driver_;
    return;
  }

  genvtx_driver_params & genvtx_driver::ui_access::params()
  {
    return _driver_->_params_;
  }

  genvtx_driver & genvtx_driver::ui_access::driver()
  {
    return *_driver_;
  }

  // ------------------------------------------------

  genvtx_driver::genvtx_driver()
  {
    _initialized_ = false;
    _action_      = 0;
    _logging_     = datatools::logger::PRIO_WARNING;
    return;
  }

  genvtx_driver::genvtx_driver(const genvtx_driver_params & params_)
  {
    _initialized_ = false;
    _action_      = 0;
    _logging_     = datatools::logger::PRIO_WARNING;
    setup(params_);
    return;
  }

  genvtx_driver::~genvtx_driver()
  {
    if (is_initialized()) {
      reset();
    }
    return;
  }

  /*
  void genvtx_driver::set_logging(datatools::logger::priority lp_)
  {
    _logging_ = lp_;
    return;
  }
  */

  datatools::logger::priority genvtx_driver::get_logging() const
  {
    return _logging_;
  }

  const genvtx_driver_params & genvtx_driver::get_params() const
  {
    return _params_;
  }

  bool genvtx_driver::is_initialized() const
  {
    return _initialized_;
  }

  void genvtx_driver::run()
  {
    DT_THROW_IF (!is_initialized(),
                 std::logic_error,
                 "Driver is not initialized !");

    // A display object that contains vertex rendering infos:
    geomtools::display_data dd;

    if (_action_ & genvtx_driver::ACTION_LIST) {
      // Prints the list of available vertex generators
      std::vector<std::string> generator_names;
      std::vector<std::string> vg_descriptions;
      std::string vg_current;
      _vtx_mgr_->vg_names(generator_names, vg_descriptions, vg_current);
      std::cerr << "List of vertex generators : " << std::endl;
      for (size_t i = 0; i < generator_names.size(); i++) {
        const std::string & generator_name = generator_names[i];
        if (i < generator_names.size() - 1 ) std::cerr << "|-- ";
        else std::cerr << "`-- ";
        std::cerr << std::flush;
        std::cout << std::flush << generator_name << ' ' << std::flush;
        if (generator_name == vg_current) std::cout << "(current)" << std::flush;
        if (! vg_descriptions[i].empty()) {
          std::cout << " : " << vg_descriptions[i];
        } else {
          std::cout << " : " << "(no description)";
        }
        std::cout << std::endl;
      }
    }

    if (_action_ & genvtx_driver::ACTION_SHOW ||
        _action_ & genvtx_driver::ACTION_SHOOT) {
      DT_THROW_IF(_params_.generator_name.empty(),
                  std::logic_error,
                  "Missing vertex generator name !");
      DT_THROW_IF(! _vtx_mgr_->has_generator(_params_.generator_name),
                  std::logic_error,
                  "Cannot find vertex generator with name '" << _params_.generator_name << "' !");
    }

    if (_action_ & genvtx_driver::ACTION_SHOW) {
      genvtx::vg_handle_type vgh = _vtx_mgr_->grab_generator(_params_.generator_name);
      const genvtx::i_vertex_generator & vh = vgh.get();
      std::ostringstream title;
      title << "Vertex generator '" << _params_.generator_name << "' :";
      vh.tree_dump(std::cout, title.str());
    }

    if (_action_ & genvtx_driver::ACTION_SHOOT) {
      if (! _params_.generator_name.empty()) {
        _vtx_mgr_->activate_current_vg(_params_.generator_name);
      }

      DT_THROW_IF(_params_.nshoots < 1, std::logic_error, "Number of random vertices is invalid !");
      DT_LOG_DEBUG(_logging_, "Number of random vertices is : " << _params_.nshoots);

      // The 3D representation of a vertex :
      double vertex_dim = _params_.visu_spot_size;
      vertex_dim *= _params_.visu_spot_zoom;
      geomtools::blur_spot vertex_spot(3, 1 *CLHEP::mm);
      vertex_spot.set_errors(vertex_dim, vertex_dim, vertex_dim);
      boost::scoped_ptr<std::ofstream> fout;
      if (! _params_.VtxOutputFile.empty()) {
        fout.reset(new std::ofstream);
        std::string ofn = _params_.VtxOutputFile;
        datatools::fetch_path_with_env(ofn);
        fout.get()->open(ofn.c_str());
        DT_THROW_IF (! *fout.get(), std::runtime_error,
                     "Cannot open output file '" << ofn << "' !");
        *fout.get() << "# generated by : genvtx::genvtx_driver" << std::endl;
        *fout.get() << "#@genvtx.version=" << GENVTX_LIB_VERSION << std::endl;
        *fout.get() << "#@geometry.setup.label=" << _geo_mgr_->get_setup_label() << std::endl;
        *fout.get() << "#@geometry.setup.version=" << _geo_mgr_->get_setup_version() << std::endl;
        *fout.get() << "#@vertex_generator.name=" << _vtx_mgr_->get_generator_name () << std::endl;
        *fout.get() << "#@length_unit=mm" << std::endl;
        if (_vtx_mgr_->is_time_generator()) {
          *fout.get() << "#@time_unit=ns" << std::endl;
        }
      }
      int vtx_counter = 0;
      while (vtx_counter < _params_.nshoots && _vtx_mgr_->can_shoot_vertex()) {
        DT_LOG_TRACE(_logging_, "Vertex count : " << vtx_counter);
        geomtools::vector_3d vertex_pos;
        double time = datatools::invalid_real();
        if (_vtx_mgr_->is_time_generator()) {
          _vtx_mgr_->shoot_vertex_and_time(vertex_pos, time);
        } else {
          _vtx_mgr_->shoot_vertex(vertex_pos);
        }
        if (fout) {
          // Save the vertex[/time] in an text output file:
          geomtools::gnuplot_draw::basic_draw_point(*fout.get(), (vertex_pos / CLHEP::mm), false);
          if (_vtx_mgr_->is_time_generator()) {
            *fout.get() << ' ' << time / CLHEP::nanosecond;
          }
          *fout.get() << std::endl;
        }
        if (_action_ & genvtx_driver::ACTION_VISU) {
          if (_params_.visu_max_counts == 0 || vtx_counter < _params_.visu_max_counts) {
            std::ostringstream vertex_name_oss;
            vertex_name_oss << "vertex_" << vtx_counter;
            geomtools::display_data::display_item & vertex_spot_DI
              = dd.add_static_item (vertex_name_oss.str(),
                                    "group::vertex",
                                    "magenta");
            geomtools::placement vertex_plcmt;
            vertex_plcmt.set_translation(vertex_pos);
            vertex_spot.generate_wires(vertex_spot_DI.wires, vertex_plcmt);
          }
        }
        vtx_counter++;
      }
    }

    // Now the rendering :
    if (_action_ & genvtx_driver::ACTION_VISU) {
      if (_logging_ >= datatools::logger::PRIO_DEBUG) {
        dd.tree_dump(std::cerr, "Vertex display data : ");
      }
      if (_params_.visu_object.empty()) {
        _params_.visu_object = _geo_mgr_->get_world_name ();
      }
      DT_LOG_DEBUG(_logging_,"Visu object label : '" << _params_.visu_object << "'");
      geomtools::gnuplot_drawer GPD;
      GPD.grab_properties().store(geomtools::gnuplot_drawer::world_name_key(),
                                  _geo_mgr_->get_world_name ());
      geomtools::placement dd_pl;
      dd_pl.set_translation (0.0, 0.0*CLHEP::cm, 0.0);
      GPD.add_display_data (dd, dd_pl);
      GPD.set_drawing_display_data (true);
      GPD.set_view (geomtools::gnuplot_drawer::view_3d());
      GPD.set_mode (geomtools::gnuplot_drawer::mode_wired());
      int visu_depth = 100;
      int view_code = GPD.draw(*_geo_mgr_, _params_.visu_object, visu_depth);
      if (view_code != 0) {
        DT_LOG_ERROR(_logging_,"Cannot display the object with label '" << _geo_mgr_->get_world_name () << "' !");
      }
    }
    return;
  }

  void genvtx_driver::setup(const genvtx_driver_params & params_)
  {
    DT_THROW_IF (is_initialized(),
                 std::logic_error,
                 "Driver is already initialized !");
    _params_ = params_;
    return;
  }

  void genvtx_driver::initialize()
  {
    DT_THROW_IF (is_initialized(),
                 std::logic_error,
                 "Driver is already initialized !");

    //std::cerr << "DEVEL: " << "***** genvtx_driver::initialize: Entering... *****" << std::endl;

    if (_logging_ >= datatools::logger::PRIO_DEBUG) {
      _params_.dump();
    }

    datatools::library_loader LL(_params_.LL_config);
    BOOST_FOREACH (const std::string & dll_name, _params_.LL_dlls) {
      DT_LOG_NOTICE(_logging_,"Loading DLL '" << dll_name << "'...");
      DT_THROW_IF (LL.load(dll_name) != EXIT_SUCCESS,
                   std::runtime_error,
                   "Loading DLL '" << dll_name << "' failed !");
    }

    if (_action_ == ACTION_NONE) {
      DT_LOG_NOTICE(_logging_, "Perform default action : list...");
      _action_ = genvtx_driver::ACTION_LIST;
    }

    // Configuration files :
    DT_THROW_IF(_params_.GeoMgrConfigFile.empty(),
                std::logic_error,
                "Missing geometry manager configuration file !");

    if (_action_ & genvtx_driver::ACTION_LIST
        || _action_ & genvtx_driver::ACTION_SHOOT) {
      DT_THROW_IF(_params_.VGMgrConfigFile.empty(),
                  std::logic_error,
                  "Missing vertex generator manager configuration file !");
    }

    // Geometry manager :
    //std::cerr << "DEVEL: " << "***** genvtx_driver::initialize: Geometry manager *****" << std::endl;
    _geo_mgr_.reset(new geomtools::manager);
    datatools::fetch_path_with_env (_params_.GeoMgrConfigFile);
    datatools::properties GeoMgrConfig;
    datatools::properties::read_config (_params_.GeoMgrConfigFile, GeoMgrConfig);
    _geo_mgr_->initialize (GeoMgrConfig);
    if (_logging_ >= datatools::logger::PRIO_DEBUG) {
      _geo_mgr_->tree_dump(std::clog, "Geometry manager embedded in the vertex generator manager: ");
    }
    //std::cerr << "DEVEL: " << "***** genvtx_driver::initialize: Geometry manager done. *****" << std::endl;

    /*
      if (_params_.dump_gids) {
      DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "The embedded GID mapping : ");
      _geo_mgr_->get_mapping ().dump_dictionnary (std::clog);
      }
    */

    // PRNG
    if (_action_ & genvtx_driver::ACTION_SHOOT) {
      //std::cerr << "DEVEL: " << "***** genvtx_driver::initialize: ACTION_SHOOT *****" << std::endl;
      if (_params_.prng_type.empty()) {
        _params_.prng_type = "taus2";
      }
      if (_params_.prng_seed <= mygsl::random_utils::SEED_INVALID) {
        _params_.prng_seed = 1;
      }

      //std::cerr << "DEVEL: " << "***** genvtx_driver::initialize: _params_.prng_seed = " << _params_.prng_seed << std::endl;

      _prng_.reset(new mygsl::rng);
      _prng_->initialize(_params_.prng_type, _params_.prng_seed);
      //std::cerr << "DEVEL: " << "***** genvtx_driver::initialize: ACTION_SHOOT done. *****" << std::endl;
    }

    // Vertex generator manager :
    //std::cerr << "DEVEL: " << "***** genvtx_driver::initialize: Vertex generator manager... *****" << std::endl;
    _vtx_mgr_.reset(new genvtx::manager(_logging_));
    _vtx_mgr_->set_geometry_manager(*_geo_mgr_);
    _vtx_mgr_->set_random_seed(1); /* 2013-10-23 FM: a trick that sets a valid dummy seed
                                    * in the embedded PRNG to make sure it initialize properly.
                                    */
    if (_action_ & genvtx_driver::ACTION_SHOOT) {
      // In "shoot" mode, plug an external PRNG, which has been properly initialized before:
      _vtx_mgr_->set_external_random(*_prng_);
    }
    datatools::fetch_path_with_env(_params_.VGMgrConfigFile);
    datatools::properties VGMgrConfig;
    datatools::properties::read_config(_params_.VGMgrConfigFile, VGMgrConfig);
    _vtx_mgr_->initialize(VGMgrConfig);
    DT_LOG_DEBUG(_logging_, "The vertex generator manager: ");
    if (_logging_ >= datatools::logger::PRIO_DEBUG) {
      _vtx_mgr_->tree_dump(std::clog);
    }

    _logging_ = datatools::logger::get_priority(_params_.logging_label);
    _action_ = ACTION_NONE;
    if (_params_.action_list) {
      _action_ |= genvtx_driver::ACTION_LIST;
    }
    if (_params_.action_shoot) {
      _action_ |= genvtx_driver::ACTION_SHOOT;
      DT_THROW_IF (_params_.prng_seed <= mygsl::random_utils::SEED_INVALID,
                   std::logic_error,
                   "Invalid PRNG seed (" << _params_.prng_seed << ") ! ");
    }
    if (_params_.action_visu) {
      _action_ |= genvtx_driver::ACTION_VISU;
    }
    if (_action_ & genvtx_driver::ACTION_SHOOT) {
      DT_THROW_IF(_params_.nshoots <= 0, std::range_error,
                  "Invalid number of vertexes to be generated (" << _params_.nshoots << ") !");
    }

    _initialized_ = true;
    return;
  }

  void genvtx_driver::reset()
  {
    DT_THROW_IF (!is_initialized(),
                 std::logic_error,
                 "Driver is not initialized !");
    _initialized_ = false;
    _action_ = ACTION_UNDEFINED;
    _prng_.reset();
    _vtx_mgr_.reset();
    _geo_mgr_.reset();
    _params_.reset();
    _logging_ = datatools::logger::PRIO_WARNING;
    return;
  }

} // end of namespace genvtx
