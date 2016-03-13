// test_cylindrical_sector.cxx

// Ourselves:
#include <geomtools/cylindrical_sector.h>

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
    std::clog << "Test program for class 'geomtools::cylindrical_sector'!" << std::endl;

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
    tmp_file.create("/tmp", "test_cylindrical_sector_");

    geomtools::cylindrical_sector ss;
    ss.set_radius(1.5);
    ss.set_z(1.5);
    if (do_sector) {
      ss.set_start_angle(M_PI/4);
      ss.set_delta_angle(0.2 * M_PI);
    }
    ss.tree_dump(std::clog, "ss");

    geomtools::placement ss_placement(0.2, 0.15, 0.1, M_PI / 7.0, M_PI / 6.0, 0.0);
    if (do_identity) {
      ss_placement.set_identity();
    }

    {
      // Draw cylindrical_sector (index 0):
      tmp_file.out() << "# cylindrical_sector " << std::endl;
      uint32_t ss_options = geomtools::cylindrical_sector::WR_NONE;
      ss_options |= geomtools::cylindrical_sector::WR_BASE_GRID;
      ss_options |= geomtools::cylindrical_sector::WR_BASE_LOW_ANGLE_SAMPLING;
      geomtools::gnuplot_draw::draw_cylindrical_sector(tmp_file.out(),
                                                       ss_placement,
                                                       ss,
                                                       ss_options);
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw cylindrical_sector (index 1):
      tmp_file.out() << "# cylindrical_sector " << std::endl;
      uint32_t ss_options = geomtools::cylindrical_sector::WR_NONE;
      ss_options |= geomtools::cylindrical_sector::WR_BASE_GRID;
      // ss_options |= geomtools::cylindrical_sector::WR_BASE_HIGH_ANGLE_SAMPLING;
      ss_options |= geomtools::cylindrical_sector::WR_CYLSEC_NO_START_ANGLE_EDGE;
      ss_options |= geomtools::cylindrical_sector::WR_CYLSEC_NO_STOP_ANGLE_EDGE;
      geomtools::gnuplot_draw::draw_cylindrical_sector(tmp_file.out(),
                                                       ss_placement,
                                                       ss,
                                                       ss_options);
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw intercepts (index 2):
      tmp_file.out() << "# locate " << std::endl;
      size_t nshoots = 100000;
      int counts = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d pos(3. * drand48(),
                                 3. * drand48(),
                                 -0.5 + drand48());

        geomtools::vector_3d pos_c;
        double tolerance = 0.1;
        ss_placement.mother_to_child(pos, pos_c);
        if (ss.is_on_surface(pos_c, tolerance)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), pos, 2.0);
          counts++;
        // } else {
        //   geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), pos, 1.0);
        //   counts++;
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
        geomtools::vector_3d pos(3. * drand48(),
                                 3. * drand48(),
                                 -3 + 6. * drand48());
        geomtools::vector_3d dir;
        geomtools::randomize_direction(drand48, dir);

        geomtools::vector_3d pos_c;
        ss_placement.mother_to_child(pos, pos_c);
        geomtools::vector_3d dir_c;
        ss_placement.mother_to_child_direction(dir, dir_c);

        geomtools::face_intercept_info intercept_c;
        if (ss.find_intercept(pos_c, dir_c, intercept_c)) {
          counts++;
          geomtools::vector_3d impact;
          ss_placement.child_to_mother(intercept_c.get_impact(), impact);
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), impact, 3.0);
        }
      }
      if (counts == 0) {
        tmp_file.out() << "0 0 0 -1" << std::endl;
      }
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw intercepts (index 4):
      tmp_file.out() << "# intercepts " << std::endl;
      size_t nshoots = 50000;
      int counts = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d pos(3. * drand48(),
                                 3. * drand48(),
                                 -0.1 + 0.2 * drand48());
        geomtools::vector_3d dir;
        geomtools::randomize_direction(drand48, dir);
        dir.setZ(0.0);

        geomtools::vector_3d pos_c;
        ss_placement.mother_to_child(pos, pos_c);
        geomtools::vector_3d dir_c;
        ss_placement.mother_to_child_direction(dir, dir_c);

        geomtools::face_intercept_info intercept_c;
        if (ss.find_intercept(pos_c, dir_c, intercept_c)) {
          counts++;
          geomtools::vector_3d impact;
          ss_placement.child_to_mother(intercept_c.get_impact(), impact);
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), impact, 2.0);
        }
      }
      if (counts == 0) {
        tmp_file.out() << "0 0 0 -1" << std::endl;
      }
      tmp_file.out() << std::endl << std::endl;
    }

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test geomtools::cylindrical_sector' ");
      g1.cmd("set grid");
      g1.cmd("set size ratio -1");
      g1.cmd("set view equal xyz");
      g1.cmd("set xrange [-3:+3]");
      g1.cmd("set yrange [-3:+3]");
      g1.cmd("set zrange [-3:+3]");
      g1.cmd("set xyplane at -3");
      g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 0 title 'Cylindrical_Sector' with lines ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 1 title 'Cylindrical_Sector' with lines ";
         plot_cmd << ", '' index 2 using 1:2:3:4 title 'Surface' with points pt 6 ps 0.25 linecolor variable";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 1 title 'Cylindrical_Sector' with lines ";
        plot_cmd << ", '' index 3 using 1:2:3:4 title 'Intercepts' with points pt 6 ps 0.25 linecolor variable";
        plot_cmd << ", '' index 4 using 1:2:3:4 title 'Intercepts horiz' with points pt 6 ps 0.25 linecolor variable";
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
