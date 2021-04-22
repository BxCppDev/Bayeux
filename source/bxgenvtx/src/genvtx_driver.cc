// genvtx_driver.cc

// Ourselves:
#include <genvtx/genvtx_driver.h>

// Third Party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/clhep_units.h>
#include <datatools/properties.h>
#include <datatools/clhep_units.h>
#include <datatools/library_loader.h>
#include <datatools/configuration/io.h>
#include <datatools/io_factory.h>
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
#include <geomtools/color.h>

// This project:
#include <genvtx/genvtx_config.h>
#include <genvtx/version.h>
#include <genvtx/manager.h>

namespace genvtx {

  genvtx_driver_params::genvtx_driver_params()
  {
    _set_defaults();
    return;
  }

  void genvtx_driver_params::_set_defaults()
  {
    // General parameters:
    logging_label = "warning";
    // Action parameters:
    // - List:
    action_list = false;
    // - Show
    action_show = false;
    // - Shoot:
    action_shoot = false;
    prng_seed    = mygsl::random_utils::SEED_INVALID;
    nshoots      = 0;
    shoot_modulo = 0;
    // - Visu:
    action_visu     = false;
    visu_spot_zoom  = 1.0;
    visu_spot_size  = 1.0 * CLHEP::mm;
    visu_spot_color = geomtools::color::magenta();
    visu_max_counts = 10000;
    visu_view  = geomtools::gnuplot_drawer::view_3d();
     // - Visu: store display data
    action_visu_store_dd = false;
    return;
  }

  void genvtx_driver_params::reset()
  {
    LL_dlls.clear();
    LL_config.clear();
    GeoMgrConfigFile.clear();
    VGMgrConfigFile.clear();
    generator_name.clear();
    prng_type.clear();
    VtxOutputFile.clear();
    VtxInputFile.clear();
    visu_object.clear();
    visu_store_dd_out.clear();
    _set_defaults();
    return;
  }

  void genvtx_driver_params::dump(std::ostream & out_) const
  {
    out_ << "genvtx driver parameters : " << std::endl;
    out_ << " - logging_label    = '" << logging_label << "'" << std::endl;
    out_ << " - LL_dlls          = #" << LL_dlls.size() << std::endl;
    out_ << " - LL_config        = '" << LL_config << "'" << std::endl;
    out_ << " - GeoMgrConfigFile = '" << GeoMgrConfigFile << "'" << std::endl;
    out_ << " - VGMgrConfigFile  = '" << VGMgrConfigFile << "'" << std::endl;
    out_ << " - action_list      = " << action_list << std::endl;
    out_ << " - generator_name   = '" << generator_name << "'" << std::endl;
    out_ << " - action_show      = " << action_show << std::endl;
    out_ << " - action_shoot     = " << action_shoot << std::endl;
    out_ << " - prng_seed        = " << prng_seed << std::endl;
    out_ << " - nshoots          = " << nshoots << std::endl;
    out_ << " - shoot modulo     = " << shoot_modulo << std::endl;
    out_ << " - VtxOutputFile    = '" << VtxOutputFile << "'" << std::endl;
    out_ << " - action_visu      = " << action_visu << std::endl;
    out_ << " - visu_spot_zoom   = " << visu_spot_zoom << std::endl;
    out_ << " - visu_spot_size   = " << visu_spot_size << std::endl;
    out_ << " - visu_spot_color  = '" << visu_spot_color << "'" << std::endl;
    out_ << " - visu_object      = '" << visu_object << "'" << std::endl;
    out_ << " - visu_max_counts  = '" << visu_max_counts << "'" << std::endl;
    out_ << " - visu_view        = '" << visu_view << "'" << std::endl;
    out_ << " - action_visu_store_dd  = " << action_visu_store_dd << std::endl;
    out_ << " - visu_store_dd_out     = '" << visu_store_dd_out << "'" << std::endl;
    return;
  }

  void genvtx_driver_params::check() const
  {
    if (action_visu_store_dd) {
      if (!VtxOutputFile.empty() && !visu_store_dd_out.empty()) {
        DT_THROW_IF(VtxOutputFile == visu_store_dd_out,
                    std::logic_error,
                    "Vertex output and vertex visu data files cannot be the same!");
      }
    }
    return;
  }

  // ------------------------------------------------

  genvtx_driver::genvtx_driver()
  {
    _initialized_ = false;
    _action_      = 0;
    _logging_     = datatools::logger::PRIO_FATAL;
    return;
  }

  genvtx_driver::genvtx_driver(const genvtx_driver_params & params_)
  {
    _initialized_ = false;
    _action_      = 0;
    _logging_     = datatools::logger::PRIO_FATAL;
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
    DT_LOG_TRACE_ENTERING(_logging_);
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
        if (generator_name == vg_current) std::cerr << "(current)" << std::flush;
        if (! vg_descriptions[i].empty()) {
          std::cout << " : " << vg_descriptions[i];
        } else {
          std::cout << " : " << std::flush;
          std::cerr << "(no description)" << std::flush;
        }
        std::cout << std::endl;
      }
    }

    // Set the generator name to be used:
    std::string generator_name;
    if (!_params_.generator_name.empty()) {
      DT_THROW_IF(! _vtx_mgr_->has_generator(_params_.generator_name),
                  std::logic_error,
                  "Cannot find vertex generator with name '" << _params_.generator_name << "' !");
      generator_name = _params_.generator_name;
    } else {
      if (_vtx_mgr_->has_generator_name()) {
        // Default from the manager, if any:
        generator_name = _vtx_mgr_->get_generator_name();
      }
    }

    if (_action_ & genvtx_driver::ACTION_SHOW ||
        _action_ & genvtx_driver::ACTION_SHOOT) {
      DT_THROW_IF(generator_name.empty(),
                  std::logic_error,
                  "Missing vertex generator name !");
    }

    if (_action_ & genvtx_driver::ACTION_SHOW) {
      genvtx::vg_handle_type vgh = _vtx_mgr_->grab_generator(generator_name);
      const genvtx::i_vertex_generator & vh = vgh.get();
      std::ostringstream title;
      title << "Vertex generator '" << generator_name << "' :";
      vh.tree_dump(std::cout, title.str());
    }

    if (_action_ & genvtx_driver::ACTION_SHOOT) {
      _vtx_mgr_->activate_current_vg(generator_name);
      DT_LOG_DEBUG(_logging_, "Number of random vertices is : " << _params_.nshoots);
      DT_THROW_IF(_params_.nshoots < 1, std::logic_error,
                  "Number of random vertices is invalid !");

      // The 3D representation of a vertex :
      double vertex_dim = _params_.visu_spot_size;
      vertex_dim *= _params_.visu_spot_zoom;
      geomtools::blur_spot vertex_spot(3, 1 *CLHEP::mm);
      vertex_spot.set_errors(vertex_dim, vertex_dim, vertex_dim);
      std::unique_ptr<std::ofstream> fout;
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
        *fout.get() << "#@vertex_generator.name=" << _vtx_mgr_->get_generator_name() << std::endl;
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
        if ((_action_ & genvtx_driver::ACTION_VISU)
            || (_action_ & genvtx_driver::ACTION_VISU_STORE_DD)) {
          std::string visu_spot_color = _params_.visu_spot_color;
          if (visu_spot_color.empty()) {
            visu_spot_color = geomtools::color::magenta();
          } else {
            geomtools::color::code_type color_code = geomtools::color::get_color(visu_spot_color);
            if (color_code == geomtools::color::COLOR_INVALID) {
              visu_spot_color = geomtools::color::magenta();
            }
          }
          if (_params_.visu_max_counts == 0 || vtx_counter < _params_.visu_max_counts) {
            std::ostringstream vertex_name_oss;
            vertex_name_oss << "vertex_" << vtx_counter;
            geomtools::display_data::display_item & vertex_spot_DI
              = dd.add_static_item (vertex_name_oss.str(),
                                    "group::vertex",
                                    visu_spot_color);
            geomtools::placement vertex_plcmt;
            vertex_plcmt.set_translation(vertex_pos);
            vertex_spot.generate_wires(vertex_spot_DI.wires, vertex_plcmt);
          }
        }
        vtx_counter++;
        if (_params_.shoot_modulo > 0 &&
            (vtx_counter == 1
             || vtx_counter % _params_.shoot_modulo == 0
             || vtx_counter ==_params_.nshoots)) {
          std::clog << "Generated vertex : " << vtx_counter << '/' << _params_.nshoots << std::endl;
        }
      }
    }

    // Now the rendering :
    if (_action_ & genvtx_driver::ACTION_VISU) {
      if (datatools::logger::is_debug(_logging_)) {
        dd.tree_dump(std::cerr, "Vertex display data : ", "[debug] ");
      }
      if (_params_.visu_object.empty()) {
        _params_.visu_object = _geo_mgr_->get_world_name();
      }
      DT_LOG_DEBUG(_logging_,"Visu object label : '" << _params_.visu_object << "'");
      geomtools::gnuplot_drawer GPD;
      GPD.grab_properties().store(geomtools::gnuplot_drawer::world_name_key(),
                                  _geo_mgr_->get_world_name());
      geomtools::placement dd_pl;
      dd_pl.set_translation(0.0, 0.0, 0.0);
      GPD.add_display_data(dd, dd_pl);
      GPD.set_drawing_display_data(true);
      std::string visu_view = _params_.visu_view;
      if (!geomtools::gnuplot_drawer::check_view(visu_view)) {
        visu_view = geomtools::gnuplot_drawer::view_3d();
      }
      GPD.set_view(visu_view);
      GPD.set_mode(geomtools::gnuplot_drawer::mode_wired());
      int visu_depth = 100;
      int view_code = GPD.draw(*_geo_mgr_, _params_.visu_object, visu_depth);
      if (view_code != 0) {
        DT_LOG_ERROR(_logging_,"Cannot display the object with label '" << _geo_mgr_->get_world_name() << "' !");
      }
    }

    if ((_action_ & genvtx_driver::ACTION_SHOOT)
        && (_action_ & genvtx_driver::ACTION_VISU_STORE_DD)) {
      DT_LOG_DEBUG(_logging_, "Visu store dd...");
      datatools::data_writer writer;
      std::string visu_store_dd_out = _params_.visu_store_dd_out;
      // if (visu_store_dd_out.empty()) {
      //   visu_store_dd_out = "vertexes-as-display-data.data.gz";
      // }
      datatools::fetch_path_with_env(visu_store_dd_out);
      writer.init(visu_store_dd_out, datatools::using_multi_archives);
      writer.store(dd);
    }

    DT_LOG_TRACE_EXITING(_logging_);
    return;
  }

  void genvtx_driver::setup(const genvtx_driver_params & params_)
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Driver is already initialized !");
    params_.check();
    _params_ = params_;
    return;
  }

  void genvtx_driver::initialize(const genvtx_driver_params & params_)
  {
    setup(params_);
    initialize();
    return;
  }

  void genvtx_driver::initialize()
  {
    DT_THROW_IF(is_initialized(), std::logic_error, "Driver is already initialized !");



    if (_logging_ >= datatools::logger::PRIO_DEBUG) {
      _params_.dump();
    }

    datatools::library_loader LL(_params_.LL_config);
    for (const auto & dll_name : _params_.LL_dlls) {
      DT_LOG_NOTICE(_logging_,"Loading DLL '" << dll_name << "'...");
      DT_THROW_IF(LL.load(dll_name) != EXIT_SUCCESS,
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
    _geo_mgr_.reset(new geomtools::manager);
    datatools::fetch_path_with_env(_params_.GeoMgrConfigFile);
    datatools::properties GeoMgrConfig;
    datatools::read_config(_params_.GeoMgrConfigFile, GeoMgrConfig);
    _geo_mgr_->initialize(GeoMgrConfig);
    if (_logging_ >= datatools::logger::PRIO_DEBUG) {
      _geo_mgr_->tree_dump(std::clog, "Geometry manager embedded in the vertex generator manager: ");
    }

    /*
      if (_params_.dump_gids) {
      DT_LOG_DEBUG(datatools::logger::PRIO_DEBUG, "The embedded GID mapping : ");
      _geo_mgr_->get_mapping().dump_dictionnary(std::clog);
      }
    */

    // PRNG
    if (_action_ & genvtx_driver::ACTION_SHOOT) {
      if (_params_.prng_type.empty()) {
        _params_.prng_type = "taus2";
      }
      if (_params_.prng_seed <= mygsl::random_utils::SEED_INVALID) {
        _params_.prng_seed = 1;
      }
      _prng_.reset(new mygsl::rng);
      _prng_->initialize(_params_.prng_type, _params_.prng_seed);
    }

    // Vertex generator manager :
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
    datatools::read_config(_params_.VGMgrConfigFile, VGMgrConfig);
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
      DT_THROW_IF(_params_.prng_seed <= mygsl::random_utils::SEED_INVALID,
                   std::logic_error,
                   "Invalid PRNG seed (" << _params_.prng_seed << ") ! ");
    }
    if (_params_.action_visu) {
      _action_ |= genvtx_driver::ACTION_VISU;
    }
    if (_action_ & genvtx_driver::ACTION_SHOOT) {
      DT_THROW_IF(_params_.nshoots <= 0, std::range_error,
                  "Invalid number of vertexes to be generated (" << _params_.nshoots << ") !");
      if (!_params_.visu_store_dd_out.empty()) {
        _params_.action_visu_store_dd = true;
      }
      if (_params_.action_visu_store_dd) {
        DT_THROW_IF(_params_.visu_store_dd_out.empty(), std::logic_error,
                    "Missing display data store output filename!");
        _action_ |= genvtx_driver::ACTION_VISU_STORE_DD;
      }
    }

    _initialized_ = true;
    DT_LOG_TRACE_EXITING(_logging_);
    return;
  }

  void genvtx_driver::reset()
  {
    DT_THROW_IF(!is_initialized(), std::logic_error, "Driver is not initialized !");
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
