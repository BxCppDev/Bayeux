// test_ellipse.cxx

// Ourselves:
#include <geomtools/ellipse.h>

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
    std::clog << "Test program for class 'geomtools::ellipse'!" << std::endl;

    bool draw = false;
    bool do_identity = false;
    bool do_sector = false;
    bool do_curve = true;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-D" || arg == "--draw") draw = true;
      if (arg == "-I" || arg == "--identity") do_identity = true;
      if (arg == "-S" || arg == "--sector") do_sector = true;

      iarg++;
    }

    srand48(314159);

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_ellipse_");

    geomtools::ellipse my_ellipse;
    my_ellipse.set_x_radius(1.25);
    my_ellipse.set_y_radius(0.65);
    if (do_sector) {
      my_ellipse.set_start_angle(M_PI / 4);
      my_ellipse.set_delta_angle(1.25 * M_PI);
    }
    my_ellipse.tree_dump(std::clog, "my_ellipse");

    geomtools::placement ellipse_placement(0.2, 0.15, 0.1, M_PI / 7.0, M_PI / 6.0, 0.0);
    if (do_identity) {
      ellipse_placement.set_identity();
    }

    // First plot (index 0):
    for (double theta0 = 0.1 * M_PI; theta0 <= 2.4 * M_PI; theta0 += 0.05 * M_PI) {
      double theta = std::atan2(std::sin(theta0), std::cos(theta0));
      double rho = 0.1 + 2.0 * drand48();
      double z = 2.0 * (-1.0 + 2.0 * drand48());
      // rho = 2.0;
      // z = 2.0;
      geomtools::vector_3d pos(rho * std::cos(theta), rho * std::sin(theta), z);
      geomtools::vector_3d proj;
      double tolerance = 0.05;
      std::clog << "x_radius = " << my_ellipse.get_x_radius() << std::endl;
      std::clog << "y_radius = " << my_ellipse.get_y_radius() << std::endl;
      std::clog << "rho      = " << rho << std::endl;
      std::clog << "z        = " << z << std::endl;
      std::clog << "theta    = " << theta << std::endl;
      // if (my_ellipse.compute_orthogonal_projection(pos, proj, tolerance)) {
      //   geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), pos);
      //   geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), proj);
      //   tmp_file.out() << std::endl;
      // }
    }

    {
      // Draw ellipse (still index 0):
      tmp_file.out() << "# ellipse: " << std::endl;
      geomtools::gnuplot_draw::draw_ellipse(tmp_file.out(),
                                            ellipse_placement,
                                            my_ellipse,
                                            geomtools::ellipse::WR_NONE
                                            | geomtools::ellipse::WR_BASE_HIGH_ANGLE_SAMPLING);
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw ellipse (index 1):
      tmp_file.out() << "# ellipse (LAS):" << std::endl;
      geomtools::wires_type ellipse_wires;
      my_ellipse.generate_wires(ellipse_wires,
                                ellipse_placement,
                                geomtools::ellipse::WR_NONE
                                | geomtools::ellipse::WR_BASE_LOW_ANGLE_SAMPLING
                                );
      geomtools::gnuplot_draw::basic_draw_wires(tmp_file.out(), ellipse_wires);
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw ellipse (index 2):
      tmp_file.out() << "# ellipse (HAS):" << std::endl;
      geomtools::wires_type ellipse_wires;
      my_ellipse.generate_wires(ellipse_wires,
                                ellipse_placement,
                                geomtools::ellipse::WR_NONE);
      geomtools::gnuplot_draw::basic_draw_wires(tmp_file.out(), ellipse_wires);
      tmp_file.out() << std::endl << std::endl;
    }


    {
      // Draw on line (index 3):
      tmp_file.out() << "# on curve: " << std::endl;
      size_t  nshoots = 200000;
      int counts = 0;
      int counts2 = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d pos((-2 + 4.* drand48()),
                                 (-2 + 4.* drand48()),
                                 (-2 + 4 * drand48()));
        geomtools::vector_3d pos_c;
        ellipse_placement.mother_to_child(pos, pos_c);
        double tolerance = 0.25;
        if (do_curve) {
          if (my_ellipse.is_on_curve(pos_c, tolerance)) {
            counts++;
            geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), pos, 2.0);
          }
        }
      }
      if (counts == 0 && counts2 == 0) {
        tmp_file.out() << "0 0 0 0.0" << std::endl;
      }
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw ellipse (index 4):
      tmp_file.out() << "# ellipse: " << std::endl;
      double dtheta = M_PI/100;
      for (double theta = 0.0; theta <= 2 * M_PI + 0.5 * dtheta; theta += dtheta) {
        double x, y;
        my_ellipse.compute_x_y_from_theta(theta, x , y);
        geomtools::vector_3d p(x, y , 0.0);
        geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), p, 2.0);
      }
      tmp_file.out() << std::endl << std::endl;
    }

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test geomtools::ellipse' ");
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
                 << "' index 0 title 'Ellipse (gnuplot draw)' with lines ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 1 title 'Ellipse (low sampling)' with lines ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 2 title 'Ellipse (high sampling)' with lines ";
        plot_cmd << ", '' index 3 using 1:2:3:4 title 'On curve' with points pt 6 ps 0.5 linecolor variable";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 4 using 1:2:3:4 title 'Ellipse' with lines lw 3";
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
