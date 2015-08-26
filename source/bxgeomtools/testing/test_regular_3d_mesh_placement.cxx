// test_regular_3d_mesh_placement.cxx

// Ourselves:
#include <geomtools/regular_3d_mesh_placement.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <unistd.h>

// Third party:
// - Boost:
#include <boost/filesystem.hpp>
// - Bayeux/datatools:
#include <datatools/temporary_files.h>
#include <datatools/utils.h>
#include <datatools/clhep_units.h>

// This project:
#include <geomtools/cylinder.h>
#include <geomtools/box.h>
#include <geomtools/sphere.h>
#include <geomtools/subtraction_3d.h>
#include <geomtools/logical_volume.h>
#include <geomtools/physical_volume.h>
#include <geomtools/gnuplot_draw.h>
#include <geomtools/gnuplot_drawer.h>
#include <geomtools/utils.h>
#include <geomtools/overlapping.h>
#include <geomtools/regular_grid_placement.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_drawer.h>
#include <geomtools/gnuplot_i.h>
#endif

struct param_type {
  bool draw;
  bool delete_cache;
  std::string cache_filename;
  std::string config_filename;
};

void test_0(const param_type &);
void test_1(const param_type &);

int main(int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {

    param_type params;
    params.draw = false;
    params.delete_cache = false;
    params.cache_filename =  "~/.cache/Bayeux/geomtools/test_regular_3d_mesh_placement-nodes.xml";
    params.config_filename =  "${GEOMTOOLS_TESTING_DIR}/config/test_regular_3d_mesh_placement.conf";

    {
      int iarg = 1;
      while (iarg < argc_) {
        std::string token = argv_[iarg];

        if (token[0] == '-') {
          std::string option = token;
          if (option == "-D" || option == "--draw") params.draw = true;
          if (option == "-C" || option == "--delete-cache") params.delete_cache = true;

        } else {
          std::string argument = token;
          {
            std::clog << "warning: ignoring argument '" << argument << "'!" << std::endl;
          }
        }
        iarg++;
      }
    }

    test_0(params);
    test_1(params);

  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

void test_0(const param_type & params_)
{
  std::string cfn = params_.cache_filename;
  datatools::fetch_path_with_env(cfn);
  if (params_.delete_cache && boost::filesystem::exists(cfn)) {
    DT_LOG_NOTICE(datatools::logger::PRIO_ALWAYS, "Deleting cache file '" << params_.cache_filename << "'...");
    boost::filesystem::remove(cfn);
  }

  //geomtools::cylinder c(1.0 * CLHEP::m, 2.0 * CLHEP::m);
  geomtools::sphere c(1.0 * CLHEP::m);
  c.lock();

  geomtools::box b(2.0 * CLHEP::m, 1.0 * CLHEP::m, 2.1 * CLHEP::m);
  b.lock();

  geomtools::subtraction_3d s;
  geomtools::placement pc;
  pc.set_identity();
  s.set_shape1(c, pc);
  geomtools::placement pb(geomtools::vector_3d (1.0 * CLHEP::m, 0, 0),
                          0.0, 0.0, 0.0);
  s.set_shape2(b, pb);
  s.lock();

  geomtools::box sbb;
  geomtools::placement sbbp;
  s.get_bounding_data().compute_bounding_box(sbb, sbbp);

  geomtools::logical_volume zone_log("zone.log", s);
  zone_log.grab_parameters().store("visibility.color", "red");

  geomtools::box db(0.25 * CLHEP::m, 0.25 * CLHEP::m, 0.25 * CLHEP::m);
  db.lock();
  geomtools::logical_volume db_log("db.log", db);
  geomtools::regular_grid_placement dbp;
  dbp.set_centered(true);
  dbp.set_mode(geomtools::regular_grid_placement::MODE_YZ);
  dbp.set_column_step(0.25 * CLHEP::m);
  dbp.set_row_step(0.25 * CLHEP::m);
  dbp.grab_basic_placement().set_identity();
  dbp.grab_basic_placement().grab_translation().set(-0.15 * CLHEP::m, 0.0, 0.0);
  dbp.set_number_of_rows(3);
  dbp.set_number_of_columns(5);
  dbp.tree_dump(std::clog, "Daughter2 placement:");
  geomtools::physical_volume db_phys("daughter1", db_log, zone_log, dbp);
  db_log.grab_parameters().store("visibility.color", "blue");

  geomtools::cylinder dc(0.35 * CLHEP::m, 0.5 * CLHEP::m);
  dc.lock();
  geomtools::logical_volume dc_log("dc.log", dc);
  geomtools::placement dcp;
  dcp.set(-0.55 * CLHEP::m, 0.0, 0., 0.0, 90.0 * CLHEP::degree, 0.0);
  dcp.tree_dump(std::clog, "Daughter1 placement:");
  geomtools::physical_volume dc_phys("daughter2", dc_log, zone_log, dcp);
  dc_log.grab_parameters().store("visibility.color", "green");

  // Mesh spheres:
  double step = 0.10 * CLHEP::m;
  geomtools::sphere sph(0.5 * step);
  sph.lock();
  geomtools::logical_volume sph_log("sphere.log", sph);

  geomtools::regular_3d_mesh_placement mp;
  mp.set_log_vol(zone_log);
  mp.set_node_log_vol(sph_log);
  mp.set_steps(step, step, 6 * step);
  // mp.set_overlapping_flags(geomtools::overlapping::FLAG_WIRES_HIGH_SAMPLING|geomtools::overlapping::FLAG_OVERLAP_ALL);
  // mp.set_overlapping_flags(geomtools::overlapping::FLAG_OVERLAP_ALL);
  mp.set_cache_file_path(cfn);
  mp.initialize();
  mp.tree_dump(std::clog, "3D mesh placement:");

  geomtools::physical_volume mesh_phys("spheres", sph_log, zone_log, mp);
  sph_log.grab_parameters().store("visibility.hidden", true);

  zone_log.tree_dump(std::clog, "Log. volume:");

#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
  if (params_.draw) {

    {

      geomtools::gnuplot_drawer GPD;
      geomtools::placement zone_placement;
      zone_placement.set_identity();
      GPD.set_mode(geomtools::gnuplot_drawer::mode_wired());
      GPD.set_view(geomtools::gnuplot_drawer::view_3d());
      GPD.set_labels(true);
      GPD.draw_logical(zone_log,
                       zone_placement,
                       geomtools::gnuplot_drawer::display_level_no_limit(),
                       "zone");

    }

    {
      datatools::temp_file tmp_file;
      tmp_file.set_remove_at_destroy (true);
      tmp_file.create(".", "test_regular_3d_mesh_placement_");

      tmp_file.out() <<"# Subtraction (index 0):" << std::endl;
      geomtools::gnuplot_draw::draw(tmp_file.out(),
                                    geomtools::placement(0,0,0,0,0,0),
                                    s,
                                    geomtools::subtraction_3d::WR_NONE
                                    //| geomtools::subtraction_3d::WR_BASE_GRID_HIGH_DENSITY
                                    //| geomtools::subtraction_3d::WR_BASE_HIGH_ANGLE_SAMPLING
                                    | geomtools::subtraction_3d::WR_COMPOSITE_BOOST_SAMPLING
                                    );
      tmp_file.out() << std::endl << std::endl;

      tmp_file.out() <<"# Bounding box (index 1):" << std::endl;
      geomtools::gnuplot_draw::draw_box(tmp_file.out(),
                                        sbbp,
                                        sbb);
      tmp_file.out() << std::endl << std::endl;

      tmp_file.out() <<"# Spheres (index 2):" << std::endl;
      for (int i = 0; i < mp.get_number_of_items(); i++) {
        geomtools::placement p;
        mp.get_placement(i, p);
        geomtools::gnuplot_draw::draw_sphere(tmp_file.out(), p, sph,
                                             geomtools::subtraction_3d::WR_BASE_GRID_LOW_DENSITY
                                             | geomtools::subtraction_3d::WR_BASE_LOW_ANGLE_SAMPLING
                                             );
      }
      tmp_file.out() << std::endl;

      tmp_file.out() <<"# Daughter volumes (index 3):" << std::endl;
      for (geomtools::logical_volume::physicals_col_type::const_iterator idaughter
             = zone_log.get_physicals().begin();
           idaughter != zone_log.get_physicals().end();
           idaughter++) {
        if (idaughter->second->get_name() == "spheres") continue;
        tmp_file.out() << "# Daughter = " << idaughter->second->get_name()
                       << " shape=" << idaughter->second->get_logical().get_shape().get_shape_name()
                       << std::endl;
        const geomtools::i_placement & dp = idaughter->second->get_placement();
        const geomtools::logical_volume & dl = idaughter->second->get_logical();
        geomtools::gnuplot_draw::draw(tmp_file.out(),
                                      dp,
                                      dl.get_shape());
      }
      tmp_file.out() << std::endl;

      Gnuplot g1 ("lines");
      g1.cmd ("set title 'Test geomtools::regular_3d_mesh_placement' ");
      g1.cmd ("set grid");
      g1.cmd ("set size ratio -1");
      g1.cmd ("set view equal xyz");
      g1.cmd ("set xyplane at -1");
      g1.cmd ("set key outside right");
      double dim = 2.0 * CLHEP::m;
      g1.set_xrange(-dim, +dim)
        .set_yrange(-dim, +dim)
        .set_zrange(-dim, +dim);
      g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename() << "'"
                 << " index 0 title 'Subtraction' with lines lt 1";
        plot_cmd << ", '' index 1 title 'BB' with lines lt 7";
        plot_cmd << ", '' index 2 title 'spheres' with lines lt 3";
        plot_cmd << ", '' index 3 title 'daughters' with lines lt 2 lw 2";

        g1.cmd (plot_cmd.str ());
      }
      g1.showonscreen();
      geomtools::gnuplot_drawer::wait_for_key();
      usleep(200);
    }
  }
#endif
  return;
}

void test_1(const param_type & params_)
{
  // Main cylindric room:
  geomtools::cylinder room_cyl(2.0 * CLHEP::m, 2.0 * CLHEP::m);
  room_cyl.lock();

  geomtools::logical_volume room_log("room.log", room_cyl);
  room_log.grab_parameters().store("visibility.color", "red");

  geomtools::box room_cyl_bb;
  geomtools::placement room_cyl_bbp;
  room_cyl.get_bounding_data().compute_bounding_box(room_cyl_bb, room_cyl_bbp);


  geomtools::box wall_box(0.5 * CLHEP::m, 2.0 * CLHEP::m, 2.0 * CLHEP::m);
  wall_box.lock();
  geomtools::logical_volume wall_log("wall.log", wall_box);
  wall_log.grab_parameters().store("visibility.color", "blue");
  geomtools::placement wall0_p(1.25 * CLHEP::m, 0.0 * CLHEP::m, 0.0 * CLHEP::m);
  geomtools::placement wall1_p(0.0 * CLHEP::m, 0.75 * CLHEP::m, 0.0 * CLHEP::m,
                               geomtools::AXIS_Z, 90.0 * CLHEP::degree);

  geomtools::physical_volume wall0_phys("wall0", wall_log, room_log, wall0_p);
  geomtools::physical_volume wall1_phys("wall1", wall_log, room_log, wall1_p);

  // Mesh box:
  double step = 0.20 * CLHEP::m;
  geomtools::box mesh_box(step, step, step);
  mesh_box.lock();
  geomtools::logical_volume mesh_box_log("mesh_box_log.log", mesh_box);

  geomtools::regular_3d_mesh_placement mesh_placement;
  mesh_placement.set_log_vol(room_log);
  mesh_placement.set_node_log_vol(mesh_box_log);

  datatools::properties mesh_box_config;
  mesh_box_config.store_real_with_explicit_unit("start_x", -1.5 * CLHEP::m);
  mesh_box_config.store_real_with_explicit_unit("start_y", -1.0 * CLHEP::m);
  mesh_box_config.store_real_with_explicit_unit("start_z", -0.9 * CLHEP::m);
  mesh_box_config.store_real_with_explicit_unit("step_x",     step);
  mesh_box_config.store_real_with_explicit_unit("step_y",     step);
  mesh_box_config.store_real_with_explicit_unit("step_z", 3 * step);

  mesh_placement.initialize(mesh_box_config);
  mesh_placement.tree_dump(std::clog, "3D mesh placement:");

  geomtools::physical_volume mesh_phys("mesh", mesh_box_log, room_log, mesh_placement);
  mesh_box_log.grab_parameters().store("visibility.hidden", true);

  room_log.tree_dump(std::clog, "Log. volume:");

#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
  if (params_.draw) {

    {

      geomtools::gnuplot_drawer GPD;
      geomtools::placement room_placement;
      room_placement.set_identity();
      GPD.set_mode(geomtools::gnuplot_drawer::mode_wired());
      GPD.set_view(geomtools::gnuplot_drawer::view_3d());
      GPD.set_labels(true);
      GPD.draw_logical(room_log,
                       room_placement,
                       geomtools::gnuplot_drawer::display_level_no_limit(),
                       "room");

    }

    {
      datatools::temp_file tmp_file;
      tmp_file.set_remove_at_destroy (true);
      tmp_file.create(".", "test_regular_3d_mesh_placement2__");

      tmp_file.out() <<"# Subtraction (index 0):" << std::endl;
      geomtools::gnuplot_draw::draw(tmp_file.out(),
                                    geomtools::placement(0,0,0,0,0,0),
                                    room_cyl,
                                    geomtools::subtraction_3d::WR_NONE
                                    | geomtools::subtraction_3d::WR_BASE_GRID_HIGH_DENSITY
                                    | geomtools::subtraction_3d::WR_BASE_HIGH_ANGLE_SAMPLING
                                    // | geomtools::subtraction_3d::WR_COMPOSITE_BOOST_SAMPLING
                                    );
      tmp_file.out() << std::endl << std::endl;

      tmp_file.out() <<"# Bounding box (index 1):" << std::endl;
      geomtools::gnuplot_draw::draw_box(tmp_file.out(),
                                        room_cyl_bbp,
                                        room_cyl_bb);
      tmp_file.out() << std::endl << std::endl;

      tmp_file.out() <<"# Mesh (index 2):" << std::endl;
      for (int i = 0; i < mesh_placement.get_number_of_items(); i++) {
        geomtools::placement p;
        mesh_placement.get_placement(i, p);
        geomtools::gnuplot_draw::draw_box(tmp_file.out(), p, mesh_box,
                                             geomtools::subtraction_3d::WR_BASE_GRID_LOW_DENSITY
                                             | geomtools::subtraction_3d::WR_BASE_LOW_ANGLE_SAMPLING
                                             );
      }
      tmp_file.out() << std::endl;

      tmp_file.out() <<"# Walls (index 3):" << std::endl;
      for (geomtools::logical_volume::physicals_col_type::const_iterator idaughter
             = room_log.get_physicals().begin();
           idaughter != room_log.get_physicals().end();
           idaughter++) {
        if (idaughter->second->get_name() == "mesh") continue;
        tmp_file.out() << "# Daughter = " << idaughter->second->get_name()
                       << " shape=" << idaughter->second->get_logical().get_shape().get_shape_name()
                       << std::endl;
        const geomtools::i_placement & dp = idaughter->second->get_placement();
        const geomtools::logical_volume & dl = idaughter->second->get_logical();
        geomtools::gnuplot_draw::draw(tmp_file.out(),
                                      dp,
                                      dl.get_shape());
      }
      tmp_file.out() << std::endl;

      Gnuplot g1 ("lines");
      g1.cmd ("set title 'Test geomtools::regular_3d_mesh_placement' ");
      g1.cmd ("set grid");
      g1.cmd ("set size ratio -1");
      g1.cmd ("set view equal xyz");
      g1.cmd ("set xyplane at -1");
      g1.cmd ("set key outside right");
      double dim = 2.0 * CLHEP::m;
      g1.set_xrange(-dim, +dim)
        .set_yrange(-dim, +dim)
        .set_zrange(-dim, +dim);
      g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename() << "'"
                 << " index 0 title 'Subtraction' with lines lt 1";
        plot_cmd << ", '' index 1 title 'BB' with lines lt 7";
        plot_cmd << ", '' index 2 title 'mesh' with lines lt 2";
        plot_cmd << ", '' index 3 title 'walls' with lines lt 3 lw 2";

        g1.cmd (plot_cmd.str ());
      }
      g1.showonscreen();
      geomtools::gnuplot_drawer::wait_for_key();
      usleep(200);
    }

  }
#endif
  return;
}
