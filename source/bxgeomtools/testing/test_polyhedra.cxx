// test_polyhedra.cxx

// Ourselves:
#include <geomtools/polyhedra.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - GSL:
#include <gsl/gsl_math.h>
// - Bayeux/datatools:
#include <datatools/temporary_files.h>
#include <datatools/utils.h>

// This project:
#include <geomtools/geomtools_config.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'polyhedra'!" << std::endl;

    bool do_identity   = false; // No placement for the solid
    // bool do_sector     = false; // Angular section
    // bool do_no_top     = false; // No top surface
    bool do_hole       = false; // Hole
    bool do_locate     = true;
    bool locate_outer_side = true;
    bool locate_inner_side = true;
    bool locate_top    = true;
    bool locate_bottom = true;
    bool locate_bulk   = false;
    bool draw          = false; // Interactive plot
    bool do_intercept  = true;
    size_t nsides      = 5;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-I" || arg == "--identity")  do_identity = true;
      if (arg == "-D" || arg == "--draw")      draw = true;
      if (arg == "-o" || arg == "--no-outer-side")   locate_outer_side = false;
      if (arg == "-i" || arg == "--no-inner-side")   locate_inner_side = false;
      if (arg == "-b" || arg == "--no-bottom") locate_bottom = false;
      if (arg == "-t" || arg == "--no-top")    locate_top = false;
      if (arg == "-B" || arg == "--bulk")      locate_bulk = true;
      if (arg == "-H" || arg == "--hole")      do_hole = true;
      if (arg == "-L" || arg == "--no-locate") do_locate = false;
      if (arg == "-P" || arg == "--no-intercept") do_intercept = false;
      // if (arg == "-S" || arg == "--sector")    do_sector = true;
      // if (arg == "-T" || arg == "--no-top")    do_no_top = true;
      if (arg == "-3") nsides = 3;
      if (arg == "-4") nsides = 4;
      if (arg == "-5") nsides = 5;
      if (arg == "-6") nsides = 6;

      iarg++;
    }

    std::map<std::string, int> data_index;

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_polyhedra_");

    geomtools::placement my_solid_placement(0.2, 0.15, 0.1, M_PI / 7.0, M_PI / 6.0, 0.0);
    if (do_identity) {
      my_solid_placement.set_identity();
    }

    double dim = 150.0 * CLHEP::mm;

    geomtools::polyhedra my_solid;
    {
      my_solid.set_n_sides(nsides);
      if (!do_hole) {
        my_solid.add( 80.0 * CLHEP::mm, 60.0 * CLHEP::mm, false);
        my_solid.add( 60.0 * CLHEP::mm, 47.0 * CLHEP::mm, false);
        my_solid.add( 40.0 * CLHEP::mm, 32.0 * CLHEP::mm, false);
        my_solid.add( 20.0 * CLHEP::mm, 25.0 * CLHEP::mm, false);
        my_solid.add(  0.0 * CLHEP::mm, 20.0 * CLHEP::mm, false);
        my_solid.add(-20.0 * CLHEP::mm, 20.0 * CLHEP::mm, false);
        my_solid.add(-40.0 * CLHEP::mm, 25.0 * CLHEP::mm, false);
        my_solid.add(-60.0 * CLHEP::mm, 25.0 * CLHEP::mm, false);
        my_solid.add(-80.0 * CLHEP::mm, 20.0 * CLHEP::mm, true);
      } else {
        my_solid.add( 80.0 * CLHEP::mm, 55.0 * CLHEP::mm, 60.0 * CLHEP::mm, false);
        my_solid.add( 60.0 * CLHEP::mm, 42.0 * CLHEP::mm, 47.0 * CLHEP::mm, false);
        my_solid.add( 40.0 * CLHEP::mm, 27.0 * CLHEP::mm, 32.0 * CLHEP::mm, false);
        my_solid.add( 20.0 * CLHEP::mm, 20.0 * CLHEP::mm, 25.0 * CLHEP::mm, false);
        my_solid.add(  0.0 * CLHEP::mm, 15.0 * CLHEP::mm, 20.0 * CLHEP::mm, false);
        my_solid.add(-20.0 * CLHEP::mm, 15.0 * CLHEP::mm, 20.0 * CLHEP::mm, false);
        my_solid.add(-40.0 * CLHEP::mm, 20.0 * CLHEP::mm, 25.0 * CLHEP::mm, false);
        my_solid.add(-60.0 * CLHEP::mm, 20.0 * CLHEP::mm, 25.0 * CLHEP::mm, false);
        my_solid.add(-80.0 * CLHEP::mm, 15.0 * CLHEP::mm, 20.0 * CLHEP::mm, true);
      }
      my_solid.lock();
      my_solid.tree_dump (std::clog, "Polyhedra");
      std::clog << "Polyhedra: " << my_solid << std::endl;

      const geomtools::face_info_collection_type & finfos = my_solid.get_computed_faces();
      geomtools::print_face_infos(finfos, std::clog, "Solid faces: ", "INFO: ");
    }

    int gpindex = 0;

    // Draw the volume:
    {
      tmp_file.out() << "# Draw solid (index 0) " << std::endl;
      geomtools::gnuplot_draw::draw_polyhedra (tmp_file.out(),
                                               my_solid_placement,
                                               my_solid,
                                               geomtools::polyhedra::WR_NONE
                                               | geomtools::polyhedra::WR_BASE_BOUNDINGS
                                               // | geomtools::polyhedra::WR_POLYHEDRA_NO_OUTER_FACE
                                               );
      tmp_file.out() << std::endl << std::endl;
      data_index["draw"] = gpindex++;
      std::clog << "INFO: Draw solid done." << std::endl;
    }

    // Locate points with respect to the volume:
    std::clog << "INFO: Locate start..." << std::endl;
    if (do_locate) {
      tmp_file.out() << "# Locate points (index 1) " << std::endl;
      size_t nshoots = 100000;
      size_t counts = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d position(dim * ( -1.0 + 2.0 * drand48()),
                                      dim * ( -1.0 + 2.0 * drand48()),
                                      1.5 * dim * ( -1.0 + 2.0 * drand48()));
        geomtools::vector_3d position_c;
        my_solid_placement.mother_to_child(position, position_c);

        double skin = 8.0 * CLHEP::mm;
        // std::cerr << "DEVEL: test_polyhedra: locate..." << std::endl;
        if (locate_outer_side && my_solid.is_on_surface(position_c,
                                                        geomtools::polyhedra::FACE_OUTER_SIDE,
                                                        skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 2.0);
          counts++;
        } else if (locate_inner_side && my_solid.is_on_surface(position_c,
                                                               geomtools::polyhedra::FACE_INNER_SIDE,
                                                               skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 3.0);
          counts++;
        } else if (locate_top && my_solid.is_on_surface(position_c,
                                                        geomtools::polyhedra::FACE_TOP,
                                                        skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 4.0);
          counts++;
        } else if (locate_bottom && my_solid.is_on_surface(position_c,
                                                           geomtools::polyhedra::FACE_BOTTOM,
                                                           skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 5.0);
          counts++;
        } else if (locate_bulk && my_solid.check_inside(position_c, skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 8.0);
          counts++;
        }
      }
      if (counts == 0) {
        tmp_file.out() << "0 0 0 -1" << std::endl;
      }
      tmp_file.out() << std::endl << std::endl;
      data_index["locate"] = gpindex++;
      std::clog << "INFO: Locate done." << std::endl;
    }

    if (do_intercept) {
      // Find intercepts:
      std::clog << "INFO: Intercepts start..." << std::endl;
      tmp_file.out() << "# Intercepts (index 2):" << std::endl;
      size_t nshoots = 100000;
      int counts = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d pos(dim * (-1.0 + 2.0 * drand48()),
                                 dim * (-1.0 + 2.0 * drand48()),
                                 1.5 * dim * (-1.0 + 2.0 * drand48()));
        geomtools::vector_3d dir;
        geomtools::randomize_direction(drand48, dir);
        geomtools::vector_3d pos_c;
        my_solid_placement.mother_to_child(pos, pos_c);
        geomtools::vector_3d dir_c;
        my_solid_placement.mother_to_child_direction(dir, dir_c);
        geomtools::face_intercept_info intercept_c;
        double tolerance = dim * 0.01;
        if (my_solid.find_intercept(pos_c, dir_c, intercept_c, tolerance)) {
          counts++;
          geomtools::vector_3d impact;
          my_solid_placement.child_to_mother(intercept_c.get_impact(), impact);
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), impact, 3.0);
        }
      }
      if (counts == 0) {
        tmp_file.out() << "0 0 0 -1" << std::endl;
      }
      tmp_file.out() << std::endl << std::endl;
      data_index["intercept"] = gpindex++;
      std::clog << "INFO: Intercept done." << std::endl;
    }

    // Draw the volume with high wires sampling:
    {
      tmp_file.out() << "# Draw solid (index 3) " << std::endl;
      geomtools::gnuplot_draw::draw_polyhedra(tmp_file.out(),
                                              my_solid_placement,
                                              my_solid,
                                              geomtools::i_wires_3d_rendering::WR_NONE
                                              | geomtools::i_wires_3d_rendering::WR_BASE_GRID
                                              | geomtools::i_wires_3d_rendering::WR_BASE_GRID_HIGH_DENSITY
                                              | geomtools::i_wires_3d_rendering::WR_BASE_HIGH_ANGLE_SAMPLING
                                              );
      tmp_file.out() << std::endl << std::endl;
      data_index["draw2"] = gpindex++;
      std::clog << "INFO: Draw solid again done." << std::endl;
    }

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test geomtools::" + my_solid.get_shape_name() + "' ");
      g1.cmd("set grid");
      g1.cmd("set size ratio -1");
      g1.cmd("set view equal xyz");
      g1.cmd("set xrange [-150:+150]");
      g1.cmd("set yrange [-150:+150]");
      g1.cmd("set zrange [-200:+125]");
      g1.cmd("set xyplane at -200");
      g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index " << data_index["draw"]
                 << " title 'Solid' with lines ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      if (do_locate) {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index " << data_index["draw"]
                 << " title 'Solid' with lines ";
        plot_cmd << ", '' index " << data_index["locate"] << " using 1:2:3:4 title 'Locate' with points pt 6 ps 0.3 linecolor variable ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      if (do_intercept) {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index " << data_index["draw"] << " title 'Solid' with lines ";
        plot_cmd << ", '' index " << data_index["intercept"] << " using 1:2:3:4 title 'Intercepts' with points pt 6 ps 0.3 linecolor variable ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index  " << data_index["draw2"] << " title 'Solid (high sampling)' with lines ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(2);
      }

#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    }

  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what () << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error!" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}
