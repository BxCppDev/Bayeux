// test_conical_frustrum.cxx

// Ourselves:
#include <geomtools/right_circular_conical_frustrum.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

// Third party:
// - Bayeux:
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

    bool do_identity   = false; // No placement for the solid
    bool do_sector     = false; // Angular section
    bool do_hole       = false; // Extrusion of the solid
    bool do_no_top     = false; // No top surface
    bool locate_outer_side = true;
    bool locate_inner_side = true;
    bool locate_top    = true;
    bool locate_bottom = true;
    bool locate_stop   = true;
    bool locate_start  = true;
    bool locate_bulk   = false;
    bool draw          = false; // Interactive plot

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-I" || arg == "--identity")  do_identity = true;
      if (arg == "-D" || arg == "--draw")      draw = true;
      if (arg == "-o" || arg == "--no-outer-side")   locate_outer_side = false;
      if (arg == "-i" || arg == "--no-inner-side")   locate_inner_side = false;
      if (arg == "-b" || arg == "--no-bottom") locate_bottom = false;
      if (arg == "-t" || arg == "--no-top")    locate_top = false;
      if (arg == "-s" || arg == "--no-start")  locate_start = false;
      if (arg == "-p" || arg == "--no-stop")   locate_stop = false;
      if (arg == "-B" || arg == "--bulk")      locate_bulk = true;
      if (arg == "-S" || arg == "--sector")    do_sector = true;
      if (arg == "-H" || arg == "--hole")      do_hole = true;
      if (arg == "-T" || arg == "--no-top")    do_no_top = true;

      iarg++;
    }

    double dim = 4.0 * CLHEP::mm;

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_conical_frustrum_");

    geomtools::placement my_solid_placement(0.2, 0.15, 0.1, M_PI / 7.0, M_PI / 6.0, 0.0);
    if (do_identity) {
      my_solid_placement.set_identity();
    }

    geomtools::conical_frustrum my_solid;
    my_solid.set_outer_bottom_radius(1.5 * CLHEP::mm);
    my_solid.set_outer_top_radius(0.75 * CLHEP::mm);
    my_solid.set_z(2.5 * CLHEP::mm);
    if (do_sector) {
      my_solid.set_start_angle(0.25 * M_PI);
      my_solid.set_delta_angle(0.75 * M_PI);
    }
    if (do_hole) {
      my_solid.set_inner_bottom_radius(0.85 * CLHEP::mm);
      my_solid.set_inner_top_radius(0.5 * CLHEP::mm);
    }
    if (do_no_top) {
      my_solid.set_inner_top_radius(my_solid.get_outer_top_radius());
    }
    my_solid.lock();
    my_solid.tree_dump(std::clog, "my_solid", "test 1: ");

    // Compute some quantities:
    {

      std::clog << "Volume   = " << my_solid.get_volume() / CLHEP::mm3 << " mm3" << std::endl;

      std::clog << "Outer side surface = "
                << my_solid.get_surface(geomtools::conical_frustrum::FACE_OUTER_SIDE) / CLHEP::mm2
                << " mm2" << std::endl;

      std::clog << "Top surface = "
                << my_solid.get_surface(geomtools::conical_frustrum::FACE_TOP) / CLHEP::mm2
                << " mm2" << std::endl;

      std::clog << "Bottom surface = " << my_solid.get_surface(geomtools::conical_frustrum::FACE_BOTTOM) / CLHEP::mm2
                << " mm2" << std::endl;

      if (do_hole) {
        std::clog << "Inner side surface = "
                  << my_solid.get_surface(geomtools::conical_frustrum::FACE_INNER_SIDE) / CLHEP::mm2
                  << " mm2" << std::endl;
      }

      if (do_sector) {
        std::clog << "Start angle side surface = "
                  << my_solid.get_surface(geomtools::conical_frustrum::FACE_START_ANGLE) / CLHEP::mm2
                  << " mm2" << std::endl;
        std::clog << "Stop angle side surface = "
                  << my_solid.get_surface(geomtools::conical_frustrum::FACE_STOP_ANGLE) / CLHEP::mm2
                  << " mm2" << std::endl;
      }

      std::clog << "Full surface = "
                << my_solid.get_surface(geomtools::conical_frustrum::FACE_ALL) / CLHEP::mm2
                << " mm2" << std::endl;
    }

    // Draw the volume:
    {
      tmp_file.out() << "# Draw solid (index 0) " << std::endl;
      geomtools::gnuplot_draw::draw_right_circular_conical_frustrum(tmp_file.out(),
                                                                    my_solid_placement,
                                                                    my_solid,
                                                                    geomtools::conical_frustrum::WR_NONE
                                                                    | geomtools::conical_frustrum::WR_BASE_BOUNDINGS
                                                                    );
      tmp_file.out() << std::endl << std::endl;
    }

    // Locate points with respect to the volume:
    {
      tmp_file.out() << "# Locate points (index 1) " << std::endl;
      bool locate = locate_outer_side | locate_inner_side | locate_bottom | locate_top | locate_start | locate_stop | locate_bulk;
      size_t nshoots = 500000;
      size_t counts = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d position(0.5 * dim * ( -1.0 + 2.0 * drand48()),
                                      0.5 * dim * ( -1.0 + 2.0 * drand48()),
                                      0.5 * dim * ( -1.0 + 2.0 * drand48()));
        geomtools::vector_3d position_c;
        my_solid_placement.mother_to_child(position, position_c);

        double skin = 0.1 * CLHEP::mm;
        if (locate_outer_side && my_solid.is_on_surface(position_c,
                                                  geomtools::conical_frustrum::FACE_OUTER_SIDE,
                                                  skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 3.0);
          counts++;
        } else if (do_hole && locate_inner_side && my_solid.is_on_surface(position_c,
                                                                          geomtools::conical_frustrum::FACE_INNER_SIDE,
                                                                          skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 7.0);
          counts++;
        } else if (locate_top && my_solid.is_on_surface(position_c,
                                                        geomtools::conical_frustrum::FACE_TOP,
                                                        skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 4.0);
          counts++;
        } else if (locate_bottom && my_solid.is_on_surface(position_c,
                                                           geomtools::conical_frustrum::FACE_BOTTOM,
                                                           skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 5.0);
          counts++;
        } else if (locate_start && my_solid.is_on_surface(position_c,
                                                           geomtools::conical_frustrum::FACE_START_ANGLE,
                                                           skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 6.0);
          counts++;
        } else if (locate_stop && my_solid.is_on_surface(position_c,
                                                           geomtools::conical_frustrum::FACE_STOP_ANGLE,
                                                           skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 8.0);
          counts++;
        } else if (locate_bulk && my_solid.is_inside(position_c, skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 2.0);
          counts++;
        }
      }
      if (counts == 0) {
        tmp_file.out() << "0 0 0 -1" << std::endl;
      }
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Find intercepts:
      tmp_file.out() << "# Intercepts (index 2):" << std::endl;
      size_t nshoots = 100000;
      int counts = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d pos(dim * drand48(),
                                 dim * drand48(),
                                 dim * (-1.0 + 2.0 * drand48()));
        geomtools::vector_3d dir;
        geomtools::randomize_direction(drand48, dir);
        geomtools::vector_3d pos_c;
        my_solid_placement.mother_to_child(pos, pos_c);
        geomtools::vector_3d dir_c;
        my_solid_placement.mother_to_child_direction(dir, dir_c);
        geomtools::face_intercept_info intercept_c;
        if (my_solid.find_intercept(pos_c, dir_c, intercept_c)) {
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
    }

    // Draw the volume with high wires sampling:
    {
      tmp_file.out() << "# Draw solid (index 3) " << std::endl;
      geomtools::gnuplot_draw::draw_right_circular_conical_frustrum(tmp_file.out(),
                                                                    my_solid_placement,
                                                                    my_solid,
                                                                    geomtools::i_wires_3d_rendering::WR_NONE
                                                                    | geomtools::i_wires_3d_rendering::WR_BASE_GRID
                                                                    | geomtools::i_wires_3d_rendering::WR_BASE_GRID_HIGH_DENSITY
                                                                    | geomtools::i_wires_3d_rendering::WR_BASE_HIGH_ANGLE_SAMPLING
                                                                    );
      tmp_file.out() << std::endl << std::endl;
    }

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test geomtools::" + my_solid.get_shape_name() + "' ");
      g1.cmd("set grid");
      g1.cmd("set size ratio -1");
      g1.cmd ("set view equal xyz");
      g1.cmd("set xrange [-4:+4]");
      g1.cmd("set yrange [-4:+4]");
      g1.cmd("set zrange [-4:+4]");
      g1.cmd("set xyplane at -4");
      g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 0 title 'Solid' with lines ";
        plot_cmd << ", '' index 1 using 1:2:3:4 title 'Locate' with points pt 6 ps 0.3 linecolor variable ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(2);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 0 title 'Solid' with lines ";
        plot_cmd << ", '' index 2 using 1:2:3:4 title 'Intercepts' with points pt 6 ps 0.3 linecolor variable ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(2);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 3 title 'Solid (high sampling)' with lines ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(2);
      }

#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    }

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
