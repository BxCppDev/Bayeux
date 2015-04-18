// test_rectangle.cxx

// Ourselves:
#include <geomtools/rectangle.h>

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
    std::clog << "Test program for class 'geomtools::rectangle'!" << std::endl;

    bool draw = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-D" || arg == "--draw") draw = true;
      iarg++;
    }

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_rectangle_");

    geomtools::rectangle my_rectangle;
    double x = 2.0;
    double y = 1.0;
    my_rectangle.set_x(x);
    my_rectangle.set_y(y);
    my_rectangle.tree_dump(std::clog, "my_rectangle");

    geomtools::placement rect_placement(0.2, 0.15, 0.1, M_PI / 7.0, M_PI / 6.0, 0.0);

    {
      // Draw rectangle (index 0):
      tmp_file.out() << "# rectangle " << std::endl;
      geomtools::gnuplot_draw::draw_rectangle(tmp_file.out(),
                                              rect_placement,
                                              my_rectangle);
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw intercepts (index 1):
      tmp_file.out() << "# intercepts " << std::endl;
      size_t nshoots = 10000;
      int counts = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d pos(2. * drand48(),
                                 2. * drand48(),
                                 -2 + 4. * drand48());
        geomtools::vector_3d dir;
        geomtools::randomize_direction(drand48, dir);

        geomtools::vector_3d pos_c;
        rect_placement.mother_to_child(pos, pos_c);
        geomtools::vector_3d dir_c;
        rect_placement.mother_to_child_direction(dir, dir_c);

        geomtools::face_intercept_info intercept_c;
        if (my_rectangle.find_intercept(pos_c, dir_c, intercept_c)) {
          counts++;
          geomtools::vector_3d impact;
          rect_placement.child_to_mother(intercept_c.get_impact(), impact);
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
      tmp_file.out() << "# intercepts 2" << std::endl;
      size_t nshoots = 1000;
      int counts = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d pos(2. * drand48(),
                                 2. * drand48(),
                                 0.5);
        geomtools::vector_3d dir;
        geomtools::randomize_direction(drand48, dir);
        dir.setZ(0.0);

        geomtools::vector_3d pos_c;
        rect_placement.mother_to_child(pos, pos_c);
        geomtools::vector_3d dir_c;
        rect_placement.mother_to_child_direction(dir, dir_c);
        geomtools::face_intercept_info intercept_c;
        if (my_rectangle.find_intercept(pos_c, dir_c, intercept_c)) {
          counts++;
          geomtools::vector_3d impact;
          rect_placement.child_to_mother(intercept_c.get_impact(), impact);
          geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), impact);
        }
      }
      if (counts == 0) {
        tmp_file.out() << "0 0 0 -1" << std::endl;
      }
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw rectangle (index 3):
      tmp_file.out() << "# rectangle " << std::endl;
      geomtools::wires_type rect_wires;
      my_rectangle.generate_wires(rect_wires,
                                  rect_placement,
                                  geomtools::i_wires_3d_rendering::WR_BASE_GRID
                                  | geomtools::i_wires_3d_rendering::WR_BASE_GRID_HIGH_DENSITY
                                  | geomtools::rectangle::WR_RECT_NO_YMINUS_SIDE
                                  | geomtools::rectangle::WR_RECT_NO_YPLUS_SIDE);
      geomtools::gnuplot_draw::basic_draw_wires(tmp_file.out(), rect_wires);
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw rectangle (index 4):
      tmp_file.out() << "# rectangle " << std::endl;
      geomtools::wires_type rect_wires;
      my_rectangle.generate_wires(rect_wires,
                                  rect_placement,
                                  geomtools::i_wires_3d_rendering::WR_BASE_GRID
                                  | geomtools::rectangle::WR_RECT_NO_XPLUS_SIDE);
      geomtools::gnuplot_draw::basic_draw_wires(tmp_file.out(), rect_wires);
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw rectangle (index 5):
      tmp_file.out() << "# rectangle " << std::endl;
      geomtools::wires_type rect_wires;
      my_rectangle.generate_wires(rect_wires,
                                  rect_placement,
                                  geomtools::i_wires_3d_rendering::WR_BASE_GRID
                                  | geomtools::i_wires_3d_rendering::WR_BASE_GRID_VERY_HIGH_DENSITY
                                  );
      geomtools::gnuplot_draw::basic_draw_wires(tmp_file.out(), rect_wires);
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Locate rectangle (index 6):

      int counts = 0;
      tmp_file.out() << "# locate (index 1)" << std::endl;
      size_t nshoots = 500000;
      for (int i = 0; i < (int) nshoots; i++) {
        double dim = 5.0;
        geomtools::vector_3d position (dim * ( -1 + 2 * drand48 ()),
                                       dim * ( -1 + 2 * drand48 ()),
                                       dim * ( -1 + 2 * drand48 ()));
        double skin = 0.20;
        geomtools::vector_3d position_c;
        rect_placement.mother_to_child(position, position_c);
        if (my_rectangle.is_on_surface(position_c, skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 6.0);
          counts++;
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
      g1.cmd("set title 'Test geomtools::rectangle' ");
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
                 << "' index 0 title 'Rectangle' with lines ";
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
                 << "' index 3 title 'Rectangle (grid)' with lines ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 4 title 'Rectangle (partial grid)' with lines ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 5 title 'Rectangle (VHD grid)' with lines ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 0 title 'Rectangle' with lines ";
        plot_cmd << ", '' index 6 using 1:2:3:4 title 'Locate' with points pt 6 ps 0.3 linecolor variable ";
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
