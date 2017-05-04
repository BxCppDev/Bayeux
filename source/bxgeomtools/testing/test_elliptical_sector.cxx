// test_elliptical_sector.cxx

// Ourselves:
#include <geomtools/elliptical_sector.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
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
  try{
    std::clog << "Test program for class 'geomtools::elliptical_sector'!" << std::endl;

    bool draw = false;
    bool with_full_angle = false;
    bool do_identity = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-D" || arg == "--draw") draw = true;
      if (arg == "-F" || arg == "--full-angle") with_full_angle = true;
      if (arg == "-I" || arg == "--identity") do_identity = true;
      iarg++;
    }

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_elliptical_sector_");

    geomtools::elliptical_sector my_ellsec;
    my_ellsec.set_x_radius(1.0);
    my_ellsec.set_y_radius(0.5);
    if (! with_full_angle) {
      my_ellsec.set_start_angle(M_PI/4);
      my_ellsec.set_delta_angle(0.75 * M_PI);
    }
    my_ellsec.tree_dump(std::clog, "my_ellsec");

    geomtools::placement ellsec_placement(0.2, 0.15, 0.1, M_PI / 7.0, M_PI / 6.0, 0.0);
    if (do_identity) {
      ellsec_placement.set_identity();
    }

    {
      // Draw ellsec (index 0):
      tmp_file.out() << "# ellsec: " << std::endl;
      geomtools::gnuplot_draw::draw_elliptical_sector(tmp_file.out(),
                                                      ellsec_placement,
                                                      my_ellsec);
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw intercepts (index 1):
      size_t nshoots = 100000;
      int counts = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d pos (1.5 * drand48() * CLHEP::mm,
                                  1.5 * drand48() * CLHEP::mm,
                                  -1.5 + 3.* drand48() * CLHEP::mm);
        geomtools::vector_3d dir;
        geomtools::randomize_direction (drand48, dir);

        geomtools::vector_3d pos_c;
        ellsec_placement.mother_to_child(pos, pos_c);
        geomtools::vector_3d dir_c;
        ellsec_placement.mother_to_child_direction(dir, dir_c);

        geomtools::face_intercept_info intercept_c;
        if (my_ellsec.find_intercept (pos_c, dir_c, intercept_c)) {
          counts++;
          geomtools::vector_3d impact;
          ellsec_placement.child_to_mother(intercept_c.get_impact(), impact);
          geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), impact);
        }
      }
      if (counts == 0) {
        tmp_file.out() << "0 0 0 -1" << std::endl;
      }
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw intercepts (index 2):
      size_t nshoots = 1000;
      int counts = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d pos(2. * drand48() * CLHEP::mm,
                                 2. * drand48() * CLHEP::mm,
                                 0.2 * CLHEP::mm);
        geomtools::vector_3d dir;
        geomtools::randomize_direction(drand48, dir);
        dir.setZ(0.0);

        geomtools::vector_3d pos_c;
        ellsec_placement.mother_to_child(pos, pos_c);
        geomtools::vector_3d dir_c;
        ellsec_placement.mother_to_child_direction(dir, dir_c);
        geomtools::face_intercept_info intercept_c;
        if (my_ellsec.find_intercept(pos_c, dir_c, intercept_c)) {
          counts++;
          geomtools::vector_3d impact;
          ellsec_placement.child_to_mother(intercept_c.get_impact(), impact);
          geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), impact);
        }
      }
      if (counts == 0) {
        tmp_file.out() << "0 0 0 -1" << std::endl;
      }
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw ellsec (index 3):
      tmp_file.out() << "# ellsec (grid, low):" << std::endl;
      geomtools::wires_type ellsec_wires;
      my_ellsec.generate_wires(ellsec_wires,
                             ellsec_placement,
                             geomtools::i_wires_3d_rendering::WR_NONE
                             | geomtools::i_wires_3d_rendering::WR_BASE_GRID
                             | geomtools::i_wires_3d_rendering::WR_BASE_GRID_LOW_DENSITY
                             | geomtools::i_wires_3d_rendering::WR_BASE_LOW_ANGLE_SAMPLING
                             );
      geomtools::gnuplot_draw::draw_wires(tmp_file.out(), ellsec_wires);
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw ellsec (index 4):
      tmp_file.out() << "# ellsec (grid, normal):" << std::endl;
      geomtools::wires_type ellsec_wires;
      my_ellsec.generate_wires(ellsec_wires,
                             ellsec_placement,
                             geomtools::i_wires_3d_rendering::WR_NONE
                             | geomtools::i_wires_3d_rendering::WR_BASE_GRID
                             );
      geomtools::gnuplot_draw::draw_wires(tmp_file.out(), ellsec_wires);
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw ellsec (index 5):
      tmp_file.out() << "# ellsec (grid, high):" << std::endl;
      geomtools::wires_type ellsec_wires;
      my_ellsec.generate_wires(ellsec_wires,
                             ellsec_placement,
                             geomtools::i_wires_3d_rendering::WR_NONE
                             | geomtools::i_wires_3d_rendering::WR_BASE_GRID
                             | geomtools::i_wires_3d_rendering::WR_BASE_GRID_VERY_HIGH_DENSITY
                             | geomtools::i_wires_3d_rendering::WR_BASE_VERY_HIGH_ANGLE_SAMPLING
                             | geomtools::elliptical_sector::WR_ELLIPTICAL_SECTOR_NO_EXT_EDGE
                             | geomtools::elliptical_sector::WR_ELLIPTICAL_SECTOR_NO_START_ANGLE_EDGE
                             );
      geomtools::gnuplot_draw::draw_wires(tmp_file.out(), ellsec_wires);
      tmp_file.out() << std::endl << std::endl;
    }

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test geomtools::ellsec' ");
      g1.cmd("set grid");
      g1.cmd("set size ratio -1");
      g1.cmd("set view equal xyz");
      g1.cmd("set xrange [-2:+2]");
      g1.cmd("set yrange [-2:+2]");
      g1.cmd("set zrange [-2:+2]");
      g1.cmd("set xyplane at -2");
      g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 0 title 'Ellsec (gnuplot)' with lines ";
        plot_cmd << ", '' index 1 title 'Intercepts 1' with points pt 6 ps 0.25 ";
        plot_cmd << ", '' index 2 title 'Horizontal intercepts 2' with points pt 6 ps 0.25 ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 3 title 'Ellsec (grid, low)' with lines lw 2";
        plot_cmd << ", '' index 4 title 'Ellsec (grid, normal)' with lines lw 1 ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 5 title 'Ellsec (grid, high)' with lines lw 1";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
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
