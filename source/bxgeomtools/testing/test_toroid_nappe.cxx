// test_toroid_nappe.cxx

// Ourselves:
#include <geomtools/toroid_nappe.h>

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

int main(int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'geomtools::toroid_nappe'!" << std::endl;

    bool draw = false;
    bool do_sector = false;
    bool do_identity = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-D" || arg == "--draw") draw = true;
      if (arg == "-S" || arg == "--sector") do_sector = true;
      if (arg == "-I" || arg == "--identity") do_identity = true;

      iarg++;
    }

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_toroid_nappe_");

    geomtools::toroid_nappe tn;
    double sr = 5.0;
    tn.set_sweep_radius(sr);
    double r = 2.0;
    tn.set_radius(r);
    if (do_sector) {
      tn.set_start_phi(M_PI/4);
      tn.set_delta_phi(0.7 * M_PI);
    }
    tn.tree_dump(std::clog, "tn");

    geomtools::placement tn_placement(0.5, 0.25, 1.0, M_PI / 7.0, M_PI / 6.0, 0.0);
    if (do_identity) {
      tn_placement.set_identity();
    }

    {
      // Draw toroid_nappe (index 0):
      tmp_file.out() << "# toroid_nappe " << std::endl;
      uint32_t tn_options = geomtools::toroid_nappe::WR_NONE;
      // tn_options |= geomtools::toroid_nappe::WR_BASE_HIGH_ANGLE_SAMPLING;
      geomtools::gnuplot_draw::draw_toroid_nappe(tmp_file.out(),
                                                 tn_placement,
                                                 tn,
                                                 tn_options);
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw toroid_nappe (index 1):
      tmp_file.out() << "# toroid_nappe " << std::endl;
      uint32_t tn_options = geomtools::toroid_nappe::WR_NONE;
      tn_options |= geomtools::toroid_nappe::WR_BASE_HIGH_ANGLE_SAMPLING;
      tn_options |= geomtools::toroid_nappe::WR_TOROID_NAPPE_NO_START_PHI_EDGE;
      // tn_options |= geomtools::toroid_nappe::WR_TOROID_NAPPE_NO_STOP_PHI_EDGE;
      geomtools::gnuplot_draw::draw_toroid_nappe(tmp_file.out(),
                                                 tn_placement,
                                                 tn,
                                                 tn_options);
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw intercepts (index 3):
      tmp_file.out() << "# locate " << std::endl;
      size_t nshoots = 20000;
      int counts = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d pos(8. * (-1.0 + 2.0 * drand48()),
                                 8. * (-1.0 + 2.0 * drand48()),
                                 2. * (-1.0 + 2.0 * drand48()));
        geomtools::vector_3d pos_c;
        double tolerance = 0.2;
        tn_placement.mother_to_child(pos, pos_c);
        if (tn.is_on_surface(pos_c, tolerance)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), pos, 2.0);
          counts++;
        }
      }
      if (counts == 0) {
        tmp_file.out() << "0 0 0 -1" << std::endl;
      }
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw intercepts (index 3):
      tmp_file.out() << "# intercepts " << std::endl;
      size_t nshoots = 50000;
      // nshoots = 100;
      int counts = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d pos(4 + 4. * drand48(),
                                 4 + 4. * drand48(),
                                 4 + 4. * drand48());
        pos.set(0.0,-8.0,0.0);
        geomtools::vector_3d dir;
        geomtools::randomize_direction(drand48, dir);

        geomtools::vector_3d pos_c;
        tn_placement.mother_to_child(pos, pos_c);
        geomtools::vector_3d dir_c;
        tn_placement.mother_to_child_direction(dir, dir_c);

        geomtools::face_intercept_info intercept_c;
        if (tn.find_intercept(pos_c, dir_c, intercept_c, 0.1)) {
          counts++;
          geomtools::vector_3d impact;
          tn_placement.child_to_mother(intercept_c.get_impact(), impact);
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), pos, -2.0, false);
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), impact, 3.0);
        }
      }
      if (counts == 0) {
        tmp_file.out() << "0 0 0 0 0 0 -1" << std::endl;
      }
      tmp_file.out() << std::endl << std::endl;
    }

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test geomtools::toroid_nappe' ");
      g1.cmd("set grid");
      g1.cmd("set size ratio -1");
      g1.cmd("set view equal xyz");
      g1.cmd("set xrange [-8:+8]");
      g1.cmd("set yrange [-8:+8]");
      g1.cmd("set zrange [-8:+8]");
      g1.cmd("set xyplane at -8");
      g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 0 title 'Toroid nappe' with lines ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 1 title 'Toroid nappe (2)' with lines ";
        plot_cmd << ", '' index 2 using 1:2:3:4 title 'Surface' with points pt 6 ps 0.25 linecolor variable";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 0 title 'Toroid nappe (3)' with lines ";
        plot_cmd << ", '' index 3 using 5:6:7:8 title 'Intercepts' with points pt 6 ps 0.25 linecolor variable";
        plot_cmd << ", '' every 1000 index 3 using 1:2:3:(column(5)-column(1)):(column(6)-column(2)):(column(7)-column(3)) title 'Rays' with vectors lt 4";
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
