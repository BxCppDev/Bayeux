// ex01_read_plain_simdata.cxx
/* Copyright (C) 2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <datatools/io_factory.h>
#include <datatools/properties.h>

#include <geomtools/manager.h>
#include <geomtools/gnuplot_draw.h>
#include <geomtools/gnuplot_drawer.h>
#include <geomtools/display_data.h>
#include <geomtools/line_3d.h>
#include <geomtools/blur_spot.h>

#include <mctools/simulated_data.h>

#include <datatools/bio_guard.h>
#include <mygsl/bio_guard.h>
#include <geomtools/bio_guard.h>
#include <mctools/bio_guard.h>

class simulated_data_inspector {

public:

  simulated_data_inspector();

  void set_interactive(bool);

  void set_with_visualization(bool);

  void set_geometry_manager(const geomtools::manager &);

  bool inspect(const mctools::simulated_data & sd_);

  void display(const mctools::simulated_data & sd_);

private:

  bool _interactive_;
  bool _with_visualization_;
  const geomtools::manager * _geometry_manager_;

};

int main(int argc_, char **argv_) {
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
  int error_code = EXIT_SUCCESS;
  try {

    std::string plain_simulated_data_filename;
    std::string geometry_config_filename;
    bool interactive = false;
    bool visualization = false;

    namespace po = boost::program_options;
    po::options_description opts("Allowed options");
    opts.add_options()
      ("help,h", "produce help message")
      ("logging-priority,g",
       po::value<std::string>()
       ->default_value("warning"),
       "set the logging priority threshold")
      ("input-file,i",
       po::value<std::string>(&plain_simulated_data_filename),
       "set the input plain simulated data file from which to load the Monte-Carlo data")
      ("geometry-config,g",
       po::value<std::string>(&geometry_config_filename),
       "set the main configuration file of the geometry manager")
       ("interactive,I",
       po::value<bool>(&interactive)
       ->zero_tokens()
       ->default_value(false),
       "run in interactive mode")
       ("with-visualization,V",
       po::value<bool>(&visualization)
       ->zero_tokens()
       ->default_value(false),
       "activate visualization")
      ; // end of options' description

    // Describe command line arguments :
    po::positional_options_description args;
    args.add("input-file", 1);
    po::variables_map vm;
    po::store(po::command_line_parser(argc_, argv_)
      .options(opts)
              .positional(args)
              .run(), vm);
    po::notify(vm);
    // Use command line arguments :
    if (vm.count("help")) {
      std::cout << "Usage : " << std::endl;
      std::cout << opts << std::endl;
      return(1);
    }
    if (vm.count("logging-priority")) {
      std::string logging_label = vm["logging-priority"].as<std::string>();
      logging = datatools::logger::get_priority(logging_label);
      DT_THROW_IF(logging == datatools::logger::PRIO_UNDEFINED, std::logic_error,
                  "Invalid logging priority label '" << logging_label << "' !");
    }

    DT_THROW_IF (plain_simulated_data_filename.empty(), std::logic_error,
                 "Missing plain simulated data input file !");

    // The input plain simulated data file :
    DT_THROW_IF (! boost::filesystem::exists(plain_simulated_data_filename),
                 std::runtime_error,
                 "File '" << plain_simulated_data_filename << "' does not exists !");

    // The  plain simulated data reader :
    datatools::data_reader psd_reader(plain_simulated_data_filename,
                                      datatools::using_multi_archives);

    datatools::properties header;
    datatools::properties footer;
    bool has_header = false;
    bool has_footer = false;

    // Geometry manager :
    DT_LOG_DEBUG(logging, "Initializing the geometry manager...");
    if (geometry_config_filename.empty()) {
      geometry_config_filename = "config/geometry/manager.conf";
    }
    geomtools::manager geo_mgr;
    datatools::fetch_path_with_env(geometry_config_filename);
    datatools::properties geo_mgr_config;
    datatools::properties::read_config(geometry_config_filename, geo_mgr_config);
    geo_mgr.initialize(geo_mgr_config);

    // The simulated data object to be loaded :
    mctools::simulated_data simdata;

    // The simulated data inspector :
    simulated_data_inspector SDI;
    SDI.set_interactive(interactive);
    SDI.set_with_visualization(visualization);
    SDI.set_geometry_manager(geo_mgr);

    // The simulated data loading loop :
    int psd_count = 0;
    while (psd_reader.has_record_tag()) {

      if (psd_reader.record_tag_is(datatools::properties::SERIAL_TAG)) {

        // Depending on the Geant4 simulation options, a header and footer could have been
        // stored as the first and last records of the archive file respectively :
        if (! has_header) {
          psd_reader.load(header); // Load the header
          has_header = true;
          header.tree_dump(std::clog, "Plain simulated data header:");
        } else if (! has_footer) {
          psd_reader.load(footer); // Load the footer
          has_footer = true;
          footer.tree_dump(std::clog, "Plain simulated data footer:");
        } else {
          DT_THROW_IF(true, std::logic_error,
                      "Header and footer have already been loaded ! "
                      << "Cannot identify additional record with serial tag '"
                      << datatools::properties::SERIAL_TAG << "' ! Abort !");
        }
      } else if (psd_reader.record_tag_is(mctools::simulated_data::SERIAL_TAG)) {

        // A plain `mctools::simulated_data' object is stored here :
        psd_reader.load(simdata); // Load the simulated data object
        DT_LOG_NOTICE(logging, "Simulated data #" << psd_count);
        bool goon = SDI.inspect(simdata);
        if (! goon) {
          break;
        }
        simdata.clear(); // Reset the simulated data object before to read the next one (if any).
        psd_count++;

      } else {

        // Here we are unable to identify the type of the record :
        DT_THROW_IF(true, std::logic_error,
                    "Cannot load record with serial tag '"
                    << psd_reader.get_record_tag() << "' ! Abort !");
      }
    }
  }
  catch(std::exception& x) {
    DT_LOG_FATAL(logging, x.what());
    error_code = EXIT_FAILURE;
  }
  catch(...) {
    DT_LOG_FATAL(logging, "Unexpected error !");
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

simulated_data_inspector::simulated_data_inspector()
{
  _interactive_ = true;
  _with_visualization_ = false;
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
  sd_.tree_dump(std::clog, "Simulated data : ");
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
      truth_hit.tree_dump(std::clog, "Truth '__visu.tracks' hit:");
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
                              "group::mc::step_hit",
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
      truth_hit.tree_dump(std::clog, "Truth 'scin' hit:");
      const geomtools::vector_3d & step_start = truth_hit.get_position_start();
      const geomtools::vector_3d & step_stop = truth_hit.get_position_stop();

      double sprite_dim = (step_stop - step_start).mag();
      geomtools::blur_spot scin_hit_sprite(3, 1 * CLHEP::mm);
      scin_hit_sprite.set_errors(sprite_dim, sprite_dim, sprite_dim);

      // Display color:
      std::string step_color = "red";

      // Label:
      std::ostringstream scin_hit_name_oss;
      scin_hit_name_oss << "scin_step_" << i;

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
              << "Enter the name of a geometry model (ex: 'world', type '.m' to print the list)" <<  "\n"
              << "   or the GID of a volume (ex: '[1234:0.1.2]', type '.g' to print the list) " << "\n"
              << "   or type '.q' to quit ";
    if (! visu_object_name.empty ()) {
      std::cerr << " (default : '" << visu_object_name << "')";
    }
    std::cerr << ": ";
    std::string line;
    std::getline(std::cin, line);
    std::istringstream line_iss(line);
    std::string token;
    line_iss >> token;
    if (token == ".m") {
      geomtools::model_factory::print_list_of_models(_geometry_manager_->get_factory(),std::clog, 0);
    } else if (token == ".g") {
      geomtools::manager::print_list_of_gids(*_geometry_manager_, std::clog, 0);
    } else if (token == ".q") {
      loop_stop = true;
    } else {
      visu_object_name = token;
      line_iss >> token;
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

// end of ex01_read_plain_simdata
