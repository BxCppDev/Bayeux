// test_conical_nappe.cxx

// Ourselves:
#include <geomtools/right_circular_conical_nappe.h>

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
    bool draw          = false; // Interactive plot

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-I" || arg == "--identity")  do_identity = true;
      if (arg == "-D" || arg == "--draw")      draw = true;
      if (arg == "-S" || arg == "--sector")    do_sector = true;

      iarg++;
    }

    double dim = 4.0 * CLHEP::mm;

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_conical_nappe_");

    geomtools::placement my_surface_placement(0.2, 0.15, 0.1, M_PI / 7.0, M_PI / 6.0, 0.0);
    if (do_identity) {
      my_surface_placement.set_identity();
    }

    geomtools::conical_nappe my_surface;
    my_surface.set_bottom_radius(1.5 * CLHEP::mm);
    my_surface.set_top_radius(0.75 * CLHEP::mm);
    my_surface.set_z(2.5 * CLHEP::mm);
    if (do_sector) {
      my_surface.set_start_angle(0.25 * M_PI);
      my_surface.set_delta_angle(0.75 * M_PI);
    }
    my_surface.tree_dump(std::clog, "my_surface", "test 1: ");

    // Compute some quantities:
    {
      std::clog << "Outer side surface = "
                << my_surface.get_surface() / CLHEP::mm2
                << " mm2" << std::endl;
    }

    // Draw the volume:
    {
      tmp_file.out() << "# Draw surface (index 0) " << std::endl;
      geomtools::gnuplot_draw::draw_right_circular_conical_nappe(tmp_file.out(),
                                                                    my_surface_placement,
                                                                    my_surface,
                                                                    geomtools::conical_nappe::WR_NONE
                                                                    );
      tmp_file.out() << std::endl << std::endl;
    }

    // Locate points with respect to the volume:
    {
      tmp_file.out() << "# Locate points (index 1) " << std::endl;
      size_t nshoots = 500000;
      size_t counts = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d position(0.5 * dim * ( -1.0 + 2.0 * drand48()),
                                      0.5 * dim * ( -1.0 + 2.0 * drand48()),
                                      0.5 * dim * ( -1.0 + 2.0 * drand48()));
        geomtools::vector_3d position_c;
        my_surface_placement.mother_to_child(position, position_c);
        double skin = 0.1 * CLHEP::mm;
        if (my_surface.is_on_surface(position_c, skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 3.0);
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
        my_surface_placement.mother_to_child(pos, pos_c);
        geomtools::vector_3d dir_c;
        my_surface_placement.mother_to_child_direction(dir, dir_c);
        geomtools::face_intercept_info intercept_c;
        if (my_surface.find_intercept(pos_c, dir_c, intercept_c)) {
          counts++;
          geomtools::vector_3d impact;
          my_surface_placement.child_to_mother(intercept_c.get_impact(), impact);
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
      geomtools::gnuplot_draw::draw_right_circular_conical_nappe(tmp_file.out(),
                                                                 my_surface_placement,
                                                                 my_surface,
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
      g1.cmd("set title 'Test geomtools::" + my_surface.get_shape_name() + "' ");
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
