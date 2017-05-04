// test_disk.cxx

// Ourselves:
#include <geomtools/disk.h>

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
  try {
    std::clog << "Test program for class 'geomtools::disk'!" << std::endl;

    bool draw = false;
    bool with_inner_r = false;
    bool with_full_angle = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-D" || arg == "--draw") draw = true;
      if (arg == "-I" || arg == "--inner-r") with_inner_r = true;
      if (arg == "-F" || arg == "--full-angle") with_full_angle = true;

      iarg++;
    }

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_disk_");

    geomtools::disk my_disk;
    double outer_radius = 1.0;
    my_disk.set_outer_r(outer_radius);
    if (with_inner_r) {
      double inner_radius = 0.5;
      my_disk.set_inner_r(inner_radius);
    }
    if (!with_full_angle) {
      my_disk.set_start_angle(M_PI/7);
      my_disk.set_delta_angle(1.35 * M_PI);
    }
    my_disk.tree_dump(std::clog, "my_disk");

    geomtools::placement disk_placement(0.2, 0.15, 0.1, M_PI / 7.0, M_PI / 6.0, 0.0);

    {
      // Draw disk (index 0):
      tmp_file.out() << "# disk: " << std::endl;
      geomtools::gnuplot_draw::draw_disk(tmp_file.out(),
                                         disk_placement,
                                         my_disk);
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw intercepts (index 1):
      size_t nshoots = 100000;
      int counts = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d pos (2.* drand48 () * CLHEP::mm,
                                  2.* drand48 () * CLHEP::mm,
                                  -2 + 4.* drand48 () * CLHEP::mm);
        geomtools::vector_3d dir;
        geomtools::randomize_direction (drand48, dir);

        geomtools::vector_3d pos_c;
        disk_placement.mother_to_child(pos, pos_c);
        geomtools::vector_3d dir_c;
        disk_placement.mother_to_child_direction(dir, dir_c);

        geomtools::face_intercept_info intercept_c;
        double tolerance = 0.10 * CLHEP::mm;
        if (my_disk.find_intercept (pos_c, dir_c, intercept_c, tolerance)) {
          counts++;
          geomtools::vector_3d impact;
          disk_placement.child_to_mother(intercept_c.get_impact(), impact);
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
        disk_placement.mother_to_child(pos, pos_c);
        geomtools::vector_3d dir_c;
        disk_placement.mother_to_child_direction(dir, dir_c);
        geomtools::face_intercept_info intercept_c;
        if (my_disk.find_intercept(pos_c, dir_c, intercept_c)) {
          counts++;
          geomtools::vector_3d impact;
          disk_placement.child_to_mother(intercept_c.get_impact(), impact);
          geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), impact);
        }
      }
      if (counts == 0) {
        tmp_file.out() << "0 0 0 -1" << std::endl;
      }
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw disk (index 3):
      tmp_file.out() << "# disk (grid, low):" << std::endl;
      geomtools::wires_type disk_wires;
      my_disk.generate_wires(disk_wires,
                             disk_placement,
                             geomtools::i_wires_3d_rendering::WR_NONE
                             | geomtools::i_wires_3d_rendering::WR_BASE_GRID
                             | geomtools::i_wires_3d_rendering::WR_BASE_GRID_LOW_DENSITY
                             | geomtools::i_wires_3d_rendering::WR_BASE_LOW_ANGLE_SAMPLING
                             );
      geomtools::gnuplot_draw::draw_wires(tmp_file.out(), disk_wires);
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw disk (index 4):
      tmp_file.out() << "# disk (grid, normal):" << std::endl;
      geomtools::wires_type disk_wires;
      my_disk.generate_wires(disk_wires,
                             disk_placement,
                             geomtools::i_wires_3d_rendering::WR_NONE
                             | geomtools::i_wires_3d_rendering::WR_BASE_GRID
                             );
      geomtools::gnuplot_draw::draw_wires(tmp_file.out(), disk_wires);
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw disk (index 5):
      tmp_file.out() << "# disk (grid, high):" << std::endl;
      geomtools::wires_type disk_wires;
      my_disk.generate_wires(disk_wires,
                             disk_placement,
                             geomtools::i_wires_3d_rendering::WR_NONE
                             | geomtools::i_wires_3d_rendering::WR_BASE_GRID
                             | geomtools::i_wires_3d_rendering::WR_BASE_GRID_VERY_HIGH_DENSITY
                             | geomtools::i_wires_3d_rendering::WR_BASE_VERY_HIGH_ANGLE_SAMPLING
                             | geomtools::disk::WR_DISK_NO_EXT_EDGE
                             | geomtools::disk::WR_DISK_NO_INT_EDGE
                             | geomtools::disk::WR_DISK_NO_START_ANGLE_EDGE
                             );
      geomtools::gnuplot_draw::draw_wires(tmp_file.out(), disk_wires);
      tmp_file.out() << std::endl << std::endl;
    }

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test geomtools::disk' ");
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
                 << "' index 0 title 'Disk (gnuplot)' with lines ";
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
                 << "' index 3 title 'Disk (grid, low)' with lines lw 2";
        plot_cmd << ", '' index 4 title 'Disk (grid, normal)' with lines lw 1 ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 5 title 'Disk (grid, high)' with lines lw 1";
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
