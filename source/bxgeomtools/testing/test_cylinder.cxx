// test_cylinder.cxx

// Ourselves:
#include <geomtools/cylinder.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

// This project:
#include <geomtools/geomtools_config.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

// Third party:
// - Bayeux/datatools:
#include <datatools/temporary_files.h>
#include <datatools/utils.h>


int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    bool debug = false;
    bool draw = false;
    long seed = 314159;
    bool test1 = true;
    bool test2 = true;
    bool test3 = true;

    bool do_identity = false;
    bool side = true;
    bool top = true;
    bool bottom = true;
    bool bulk = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-d" || arg == "--debug") debug = true;
      if (arg == "-I" || arg == "--identity") do_identity = true;
      if (arg == "-D" || arg == "--draw") draw = true;
      if (arg == "-t") top = false;
      if (arg == "-b") bottom = false;
      if (arg == "-s") side = false;
      if (arg == "-B") bulk = true;

      iarg++;
    }

    srand48 (seed);

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_cylinder_");

    geomtools::cylinder my_cylinder(3.0 * CLHEP::mm, 10.0 * CLHEP::mm);
    geomtools::placement cylinder_placement(0.2, 0.15, 0.1, M_PI / 7.0, M_PI / 6.0, 0.0);
    if (do_identity) {
      cylinder_placement.set_identity();
    }
    // const geomtools::vector_3d & cylinder_pos = cylinder_placement.get_translation();
    // const geomtools::rotation_3d & cylinder_rot = cylinder_placement.get_rotation();

    // test 1:
    if (test1) {

      std::clog << "test 1: Cylinder #1 = " << my_cylinder << " " << std::endl;

      std::clog << "test 1: Volume   = " << my_cylinder.get_volume () / CLHEP::m3 << " m3" << std::endl;

      std::clog << "test 1: Side surface = "
                << my_cylinder.get_surface (geomtools::cylinder::FACE_SIDE) / CLHEP::m2
                << " m2" << std::endl;

      std::clog << "test 1: Top surface = "
                << my_cylinder.get_surface (geomtools::cylinder::FACE_TOP) / CLHEP::m2
                << " m2" << std::endl;

      std::clog << "test 1: Bottom surface = "
                << my_cylinder.get_surface (geomtools::cylinder::FACE_BOTTOM) / CLHEP::m2
                << " m2" << std::endl;

      std::clog << "test 1: Full surface = "
                << my_cylinder.get_surface (geomtools::cylinder::FACE_ALL) / CLHEP::m2
                << " m2" << std::endl;

      {
        bool test_in = false;
        if (test_in) {
          std::clog << "test 1: Enter a cylinder (example: '{cylinder 1000 1000}'): ";
          std::cin >> std::ws >> my_cylinder;
          if (std::cin) {
            std::clog << "test 1: Cylinder = " << my_cylinder << " " << std::endl;
          } else {
            throw std::runtime_error ("Invalid input for cylinder!");
          }
        }
      }
    }

    // test 2:
    if (test2) {
      std::clog << "test 2: Cylinder #2   = " << my_cylinder << " " << std::endl;

      geomtools::vector_3d pos(4.0 * CLHEP::mm,
                               4.0 * CLHEP::mm,
                               0.0 * CLHEP::mm);
      geomtools::vector_3d dir(-1.0, -1.0, 0.0);

      geomtools::vector_3d pos_c;
      cylinder_placement.mother_to_child(pos, pos_c);
      geomtools::vector_3d dir_c;
      cylinder_placement.mother_to_child_direction(dir, dir_c);

      geomtools::face_intercept_info intercept_c;
      geomtools::vector_3d impact;
      if (my_cylinder.find_intercept(pos_c, dir_c, intercept_c)) {
       cylinder_placement.child_to_mother(intercept_c.get_impact(), impact);
       std::clog << "test 2: Intercept face="
                 << intercept_c.get_face_id().get_face_bits()
                 << " at impact="
                 << impact
                 << std::endl;
      } else {
        std::clog << "test 2: No intercept." << std::endl;
      }

      tmp_file.out() << "# cylinder (index 0): " << std::endl;
      geomtools::gnuplot_draw::draw_cylinder(tmp_file.out(),
                                             cylinder_placement,
                                             my_cylinder);
      tmp_file.out() << std::endl << std::endl;

      tmp_file.out() << "# source (index 1): " << std::endl;
      geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), pos);
      tmp_file.out() << std::endl << std::endl;

      tmp_file.out() << "# impact (index 2): " << std::endl;
      geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(),impact);
      tmp_file.out() << std::endl << std::endl;

      tmp_file.out() << "# track (index 3): " << std::endl;
      geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), pos);
      geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), impact);
      tmp_file.out() << std::endl << std::endl;
    }

    // test 3:
    if (test3) {
      std::clog << "test 3: Cylinder #3   = " << my_cylinder
                << " " << std::endl;

      size_t nshoots = 50000;
      int counts = 0;
      tmp_file.out() << "# intercept (index 4)" << std::endl;
      for (int i = 0; i < (int) nshoots; i++) {
        double dim = 4. * CLHEP::mm;
        geomtools::vector_3d pos (dim * drand48(),
                                  dim * drand48(),
                                  dim * drand48());
        geomtools::vector_3d dir;
        geomtools::randomize_direction (drand48, dir);

        geomtools::vector_3d pos_c;
        cylinder_placement.mother_to_child(pos, pos_c);
        geomtools::vector_3d dir_c;
        cylinder_placement.mother_to_child_direction(dir, dir_c);

        geomtools::face_intercept_info intercept_c;
        double tolerance = 0.05;
        if (my_cylinder.find_intercept(pos_c, dir_c, intercept_c, tolerance)) {
          counts++;
          geomtools::vector_3d impact;
          cylinder_placement.child_to_mother(intercept_c.get_impact(), impact);
          geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), impact);
          if (debug) std::clog << "test 3: Intercept face="
                               << intercept_c.get_face_id().get_face_bits()
                               << " at impact=" << impact
                               << std::endl;
        } else {
          if (debug) std::clog << "test 3: No intercept." << std::endl;
        }
      }
      if (counts == 0) {
        tmp_file.out() << "0 0 0 -1" << std::endl;
      }
      tmp_file.out() << std::endl << std::endl;

      nshoots = 100000;
      counts = 0;
      tmp_file.out() << "# locate (index 5)" << std::endl;
      for (int i = 0; i < (int) nshoots; i++) {
        double dim = 5. * CLHEP::mm;
        geomtools::vector_3d position (dim * ( -1 + 2 * drand48 ()),
                                       dim * ( -1 + 2 * drand48 ()),
                                       dim * ( -1 + 2 * drand48 ()));
        geomtools::vector_3d position_c;
        cylinder_placement.mother_to_child(position, position_c);
        double skin = 0.25 * CLHEP::mm;
        if (top && my_cylinder.is_on_surface(position_c,
                                             geomtools::cylinder::FACE_TOP, skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 5.0);
          counts++;
        } else if (bottom && my_cylinder.is_on_surface(position_c,
                                                       geomtools::cylinder::FACE_BOTTOM, skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 6.0);
          counts++;
        } else if (side && my_cylinder.is_on_surface(position_c,
                                                     geomtools::cylinder::FACE_SIDE, skin)) {
          geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 7.0);
          counts++;
        } else if (bulk && my_cylinder.check_inside(position_c, skin)) {
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
      // Draw cylinder :
      tmp_file.out() << "# cylinder (index 6):" << std::endl;
      geomtools::wires_type cylinder_wires;
      my_cylinder.generate_wires(cylinder_wires,
                                 cylinder_placement,
                                 geomtools::i_wires_3d_rendering::WR_NONE
                                 | geomtools::i_wires_3d_rendering::WR_BASE_GRID
                                 | geomtools::i_wires_3d_rendering::WR_BASE_GRID_HIGH_DENSITY
                                 | geomtools::i_wires_3d_rendering::WR_BASE_BOUNDINGS
                                 // | geomtools::i_wires_3d_rendering::WR_BASE_EXPLODE
                                 // | geomtools::cylinder::WR_CYL_NO_BOTTOM_FACE
                                  | geomtools::cylinder::WR_CYL_NO_TOP_FACE
                                 // | geomtools::cylinder::WR_CYL_NO_SIDE_FACE
                                 );
      geomtools::gnuplot_draw::draw_wires(tmp_file.out(), cylinder_wires);
      tmp_file.out() << std::endl << std::endl;
    }

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test geomtools::cylinder' ");
      g1.cmd("set grid");
      g1.cmd("set size ratio -1");
      g1.cmd("set view equal xyz");
      g1.cmd("set xrange [-6:+6]");
      g1.cmd("set yrange [-6:+6]");
      g1.cmd("set zrange [-8:+8]");
      g1.cmd("set xyplane at -8");
      g1.set_xlabel("x").set_ylabel("y").set_zlabel("z");

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename() << "' index 0 title 'Cylinder' with lines ,";
        plot_cmd << " '' index 1 title 'Source' with points lt 3 pt 7 ps 1.0 ,";
        plot_cmd << " '' index 2 title 'Impact' with points lt 4 pt 7 ps 1.0 ,";
        plot_cmd << " '' index 3 title 'Track'  with lines lt 2 ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename() << "' index 0 title 'Cylinder' with lines ,";
        plot_cmd << " '' index 4 title 'Intercept' with dots ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename() << "' index 0 title 'Cylinder' with lines ,";
        plot_cmd << " '' index 5 using 1:2:3:4 title 'Locate' with points pt 6 ps 0.3 linecolor variable ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename() << "' index 6 title 'Cylinder (grid)' with lines ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
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
