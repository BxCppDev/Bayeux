// ex00_inspector.cc

#include <ex00_inspector.h>

// Geomtools:
#include <geomtools/manager.h>
#include <geomtools/gnuplot_draw.h>
#include <geomtools/gnuplot_drawer.h>
#include <geomtools/display_data.h>
#include <geomtools/line_3d.h>
#include <geomtools/box.h>
#include <geomtools/blur_spot.h>

// Mctools:
#include <mctools/simulated_data.h>

namespace mctools {
  namespace ex00 {

    simulated_data_inspector::simulated_data_inspector()
    {
      _interactive_ = true;
      _dump_simulated_data_ = false;
      _dump_hits_ = false;
      _with_visualization_ = false;
    }

    void simulated_data_inspector::set_dump_simulated_data(bool dsd_)
    {
      _dump_simulated_data_ = dsd_;
    }

    void simulated_data_inspector::set_dump_hits(bool dh_)
    {
      _dump_hits_ = dh_;
    }

    void simulated_data_inspector::set_interactive(bool i_)
    {
      _interactive_ = i_;
    }

    void simulated_data_inspector::set_with_visualization(bool wv_)
    {
      if (wv_) set_interactive(true);
      _with_visualization_ = wv_;
    }

    void simulated_data_inspector::set_geometry_manager(const geomtools::manager & gm_)
    {
      _geometry_manager_ = &gm_;
    }

    bool simulated_data_inspector::inspect(const mctools::simulated_data & sd_)
    {
      if (_dump_simulated_data_) {
        sd_.tree_dump(std::clog, "Simulated data : ", " ");
        std::clog << std::endl;
      }
      if (_interactive_) {
        if (_with_visualization_) {
          display(sd_);
        }
        std::clog << "Hit `[Enter]' to go to the next event or `quit [Enter]' : ";
        std::string line;
        std::getline(std::cin, line);
        std::istringstream line_iss(line);
        std::string token;
        line_iss >> token;
        if (token == "q" || line == "token") return false;
      }
      return true;
    }

    void simulated_data_inspector::display(const mctools::simulated_data & sd_)
    {
      // A display object that contains simulated event rendering infos:
      geomtools::display_data dd;

      if (sd_.has_vertex()) {
        // Build the display informations about the vertex (if any) :
        double vertex_dim = 1.0 * CLHEP::mm;
        geomtools::blur_spot vertex_spot(3, 0.25 * CLHEP::mm);
        vertex_spot.set_errors(vertex_dim,vertex_dim,vertex_dim);
        std::ostringstream vertex_name_oss;
        vertex_name_oss << "vertex";
        geomtools::display_data::display_item & vertex_spot_DI
          = dd.add_static_item (vertex_name_oss.str(),
                                "group::mc::vertex",
                                "magenta");
        geomtools::placement vertex_plcmt;
        const geomtools::vector_3d & vertex_pos = sd_.get_vertex ();
        vertex_plcmt.set_translation(vertex_pos);
        vertex_spot.generate_wires (vertex_spot_DI.paths, vertex_plcmt);
      }

      if (sd_.has_step_hits("__visu.tracks")) {
        // Build the display informations about the "__visu.tracks" MC hits (if any) :
        int nsteps = sd_.get_number_of_step_hits("__visu.tracks");
        for (int i = 0; i < nsteps; i++) {
          const mctools::base_step_hit & truth_hit = sd_.get_step_hit("__visu.tracks", i);
          if (_dump_hits_) {
            truth_hit.tree_dump(std::clog, "Truth '__visu.tracks' hit : ", " ");
            std::clog << std::endl;
          }
          const geomtools::vector_3d & step_start = truth_hit.get_position_start();
          const geomtools::vector_3d & step_stop = truth_hit.get_position_stop();
          const std::string & pname = truth_hit.get_particle_name();
          geomtools::line_3d visu_step_segment(step_start, step_stop);

          // Display color:
          std::string step_color = "white";
          if (pname == "gamma") step_color = "green";
          else if (pname == "e+") step_color = "blue";
          else if (pname == "e-") step_color = "red";

          // Label:
          std::ostringstream visu_step_name_oss;
          visu_step_name_oss << "visu_step_" << i;

          geomtools::display_data::display_item & visu_step_DI
            = dd.add_static_item (visu_step_name_oss.str(),
                                  "group::mc::visu_step_hit",
                                  step_color);

          geomtools::placement visu_step_plcmt;
          visu_step_segment.generate_wires (visu_step_DI.paths, visu_step_plcmt);
        }
      }

      if (sd_.has_step_hits("probe")) {
        // Build the display informations about the "probe" MC hits (if any) :
        int nsteps = sd_.get_number_of_step_hits("probe");
        for (int i = 0; i < nsteps; i++) {
          const mctools::base_step_hit & truth_hit = sd_.get_step_hit("probe", i);
          if (_dump_hits_) {
            truth_hit.tree_dump(std::clog, "Truth 'probe' hit : ", " ");
            std::clog << std::endl;
          }
          const geomtools::vector_3d & step_start = truth_hit.get_position_start();
          const geomtools::vector_3d & step_stop = truth_hit.get_position_stop();
          const std::string & pname = truth_hit.get_particle_name();
          geomtools::line_3d visu_step_segment(step_start, step_stop);

          // Display color:
          std::string step_color = "white";
          if (pname == "gamma") step_color = "green";
          else if (pname == "e+") step_color = "blue";
          else if (pname == "e-") step_color = "red";

          // Label:
          std::ostringstream visu_step_name_oss;
          visu_step_name_oss << "probe_" << i;

          geomtools::display_data::display_item & visu_step_DI
            = dd.add_static_item (visu_step_name_oss.str(),
                                  "group::mc::probe_hit",
                                  step_color);

          geomtools::placement visu_step_plcmt;
          visu_step_segment.generate_wires (visu_step_DI.paths, visu_step_plcmt);
        }
      }

      if (sd_.has_step_hits("scin")) {
        // Build the display informations about the "scin" MC hits (if any) :
        int nsteps = sd_.get_number_of_step_hits("scin");
        for (int i = 0; i < nsteps; i++) {
          const mctools::base_step_hit & truth_hit = sd_.get_step_hit("scin", i);
          if (_dump_hits_) {
            truth_hit.tree_dump(std::clog, "Truth 'scin' hit : ", " ");
            std::clog << std::endl;
          }
          const geomtools::vector_3d & step_start = truth_hit.get_position_start();
          const geomtools::vector_3d & step_stop = truth_hit.get_position_stop();

          double sprite_dim = (step_stop - step_start).mag();
          //geomtools::blur_spot scin_hit_sprite(3, 1 * CLHEP::mm);
          //scin_hit_sprite.set_errors(sprite_dim, sprite_dim, sprite_dim);
          geomtools::box scin_hit_sprite((step_stop - step_start).x(),
                                         (step_stop - step_start).y(),
                                         (step_stop - step_start).z());

          // Display color:
          std::string step_color = "red";

          // Label:
          std::ostringstream scin_hit_name_oss;
          scin_hit_name_oss << "scin_" << i;

          geomtools::display_data::display_item & scin_hit_DI
            = dd.add_static_item (scin_hit_name_oss.str(),
                                  "group::mc::scin_hit",
                                  step_color);

          geomtools::placement scin_hit_plcmt;
          scin_hit_plcmt.set_translation(0.5 * (step_stop + step_start));
          scin_hit_sprite.generate_wires(scin_hit_DI.paths, scin_hit_plcmt);
        }
      }

      std::string visu_object_name;
      std::string view_label = geomtools::gnuplot_drawer::VIEW_3D;
      int visu_depth = geomtools::gnuplot_drawer::DISPLAY_LEVEL_NO_LIMIT;
      bool loop_stop = false;
      do {
        if (visu_object_name.empty()) {
          visu_object_name = _geometry_manager_->get_world_name ();
        }
        geomtools::gnuplot_drawer GPD;
        GPD.grab_properties().store(geomtools::gnuplot_drawer::WORLD_NAME_KEY,
                                    _geometry_manager_->get_world_name ());
        geomtools::placement dd_pl;
        dd_pl.set_translation(0.0, 0.0, 0.0);
        GPD.add_display_data(dd, dd_pl);
        GPD.set_mode(geomtools::gnuplot_drawer::MODE_WIRED);
        GPD.set_view(view_label);
        GPD.set_labels(true);
        int view_code = GPD.draw(*_geometry_manager_,
                                 visu_object_name,
                                 visu_depth);
        if (view_code != 0) {
          DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                       "Cannot display the object with label '" << visu_object_name << "' !");
          visu_object_name.clear();
        }

        std::clog << std::flush;
        std::cerr << std::endl
                  << "Enter the name of a geometry model (ex: 'world',        type '.M' to print the list)" <<  "\n"
                  << "   or the name of a logical volume (ex: '',             type '.L' to print the list) " << "\n"
                  << "   or the GID of a volume          (ex: '[1234:0.1.2]', type '.G' to print the list) " << "\n"
                  << "   or type '.q' to quit";
        if (! visu_object_name.empty ()) {
          std::cerr << "            (default : '" << visu_object_name << "')";
        }
        std::cerr << " : ";
        std::string line;
        std::getline(std::cin, line);
        std::istringstream command_iss(line);
        std::string token;
        command_iss >> token;
        if (token == ".M") {
          std::string print_models_options;
          std::getline(command_iss, print_models_options);
          int error = geomtools::model_factory::print_list_of_models(_geometry_manager_->get_factory(), std::cerr, print_models_options);
          if (error > 0) {
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                         "Invalid options '" << print_models_options << "' !");
          }
        } else if (token == ".L") {
          std::string print_logical_options;
          std::getline(command_iss, print_logical_options);
          int error = geomtools::model_factory::print_list_of_logicals(_geometry_manager_->get_factory(), std::cerr, print_logical_options);
          if (error > 0) {
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR,
                         "Invalid options '" << print_logical_options << "' !");
          }
        } else if (token == ".G") {
          std::string print_gids_options;
          std::getline(command_iss, print_gids_options);
          int error = geomtools::manager::print_list_of_gids(*_geometry_manager_, std::cerr, print_gids_options);
          if (error > 0) {
            DT_LOG_ERROR(datatools::logger::PRIO_ERROR, "Invalid options '" << print_gids_options << "' !");
          }
        } else if (token == ".q") {
          loop_stop = true;
        } else {
          visu_object_name = token;
          command_iss >> token;
          if (! token.empty() && token[0] == '-') {
            if (token == "-xy") view_label = geomtools::gnuplot_drawer::VIEW_2D_XY;
            if (token == "-xz") view_label = geomtools::gnuplot_drawer::VIEW_2D_XZ;
            if (token == "-yz") view_label = geomtools::gnuplot_drawer::VIEW_2D_YZ;
            if (token == "-3d") view_label = geomtools::gnuplot_drawer::VIEW_3D;
          }
        }
      } while (!loop_stop);

      return;
    }

  }  // namespace ex00
} // namespace mctools
