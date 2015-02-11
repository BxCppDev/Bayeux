// test_cylinder.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

// This project:
#include <geomtools/geomtools_config.h>
#include <geomtools/cylinder.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

// Third party:
// - Bayeux/datatools:
#include <datatools/temporary_files.h>
#include <datatools/utils.h>


int
main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    bool debug = false;
    bool draw = false;
    long seed = 314159;
    bool test1 = true;
    bool test2 = true;
    bool test3 = true;

    bool side = false;
    bool top = false;
    bool bottom = false;
    bool bulk = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-d" || arg == "--debug") debug = true;
      if (arg == "-D" || arg == "--draw") draw = true;
      if (arg == "-t1" || arg == "--test1") test1 = false;
      if (arg == "-t2" || arg == "--test2") test2 = false;
      if (arg == "-t3" || arg == "--test3") test3 = false;

      if (arg == "-t") top = true;
      if (arg == "-b") bottom = true;
      if (arg == "-s") side = true;
      if (arg == "-B") bulk = true;

      iarg++;
    }

    srand48 (seed);

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_cylinder_");

    // test 1:
    if (test1) {
      geomtools::cylinder my_cylinder (1000 * CLHEP::mm,
                                       230 * CLHEP::mm);

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
      geomtools::cylinder my_cylinder (3.0 * CLHEP::mm,
                                       10.0 * CLHEP::mm);
      std::clog << "test 2: Cylinder #2   = " << my_cylinder << " " << std::endl;

      geomtools::vector_3d pos (4.0 * CLHEP::mm,
                                0.0 * CLHEP::mm,
                                0.0 * CLHEP::mm);
      geomtools::vector_3d dir (-1, 0, 0);
      geomtools::intercept_t intercept;
      // int intercept_face;
      if (my_cylinder.find_intercept (pos, dir, intercept)) {
        std::clog << "test 2: Intercept face=" << intercept.get_face ()
                  << " at impact=" << intercept.get_impact ()
                  << std::endl;
      } else {
        std::clog << "test 2: No intercept." << std::endl;
      }

      geomtools::vector_3d cylinder_pos;
      geomtools::rotation cylinder_rot;
      geomtools::create_rotation (cylinder_rot, 0.0, 0.0, 0.0);
      geomtools::gnuplot_draw::draw_cylinder (tmp_file.out(), cylinder_pos, cylinder_rot,
                                              my_cylinder.get_r (),
                                              my_cylinder.get_z ());
      tmp_file.out() << std::endl << std::endl;

      geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(), pos);
      tmp_file.out() << std::endl << std::endl;

      geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(),
                                                 intercept.get_impact ());
      tmp_file.out() << std::endl << std::endl;

      geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(), pos);
      geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(), pos + dir);
      tmp_file.out() << std::endl << std::endl;
    }

    // test 3:
    if (test3) {
      geomtools::cylinder my_cylinder (2.0 * CLHEP::mm,
                                       4.0 * CLHEP::mm);
      std::clog << "test 3: Cylinder #3   = " << my_cylinder
                << " " << std::endl;
      geomtools::vector_3d cylinder_pos;
      geomtools::rotation cylinder_rot;
      geomtools::create_rotation (cylinder_rot, 0.0, 0.0, 0.0);
      geomtools::gnuplot_draw::draw_cylinder (tmp_file.out(),
                                              cylinder_pos,
                                              cylinder_rot,
                                              my_cylinder.get_r (),
                                              my_cylinder.get_z ());
      tmp_file.out() << std::endl << std::endl;
      size_t nshoots = 100000;
      for (int i = 0; i < (int) nshoots; i++) {
        double dim = 4. * CLHEP::mm;
        //dim = 0. * CLHEP::mm;
        geomtools::vector_3d pos (dim * drand48 (),
                                  dim * drand48 (),
                                  dim * drand48 ());
        //pos.setZ (3. * CLHEP::mm);
        geomtools::vector_3d dir;
        geomtools::randomize_direction (drand48, dir);

        geomtools::intercept_t intercept;
        if (my_cylinder.find_intercept (pos, dir, intercept)) {
          if (debug) std::clog << "test 3: Intercept face="
                               << intercept.get_face ()
                               << " at impact=" << intercept.get_impact ()
                               << std::endl;
          geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(),
                                                     intercept.get_impact ());
        } else {
          if (debug) std::clog << "test 3: No intercept." << std::endl;
        }
      }
      tmp_file.out() << std::endl << std::endl;

      nshoots = 1000000;
      for (int i = 0; i < (int) nshoots; i++) {
        double dim = 5. * CLHEP::mm;
        geomtools::vector_3d position (dim * ( -1 + 2 * drand48 ()),
                                       dim * ( -1 + 2 * drand48 ()),
                                       dim * ( -1 + 2 * drand48 ()));
        double skin = 0.25 * CLHEP::mm;
        if (top && my_cylinder.is_on_surface(position,
                                             geomtools::cylinder::FACE_TOP, skin)) {
          tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                         << 5
                         << std::endl;
        } else if (bottom && my_cylinder.is_on_surface(position,
                                                       geomtools::cylinder::FACE_BOTTOM, skin)) {
          tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                         << 6
                         << std::endl;
        } else if (side && my_cylinder.is_on_surface(position,
                                                     geomtools::cylinder::FACE_SIDE, skin)) {
          tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                         << 7
                         << std::endl;
        } else if (bulk && my_cylinder.is_inside(position, skin)) {
          tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                         << 2
                         << std::endl;
        }
      }
      tmp_file.out() << std::endl << std::endl;

    }

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd ("set title 'Test geomtools::cylinder' ");
      g1.cmd ("set grid");
      g1.cmd ("set size ratio -1");
      g1.cmd ("set view equal xyz");
      g1.cmd ("set xyplane at -6");
      g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 0 title 'Cylinder' with lines ,";
        plot_cmd << " '' index 1 title 'Source' with points lt 3 pt 7 ps 1.0 ,";
        plot_cmd << " '' index 2 title 'Impact' with points lt 4 pt 7 ps 1.0 ,";
        plot_cmd << " '' index 3 title 'Track'  with lines lt 2 ";
        g1.cmd (plot_cmd.str ());
        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (200);
      }
      {
        g1.cmd ("set xyplane at -2");
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 4 notitle with lines ,";
        plot_cmd << " '' index 5 notitle with dots ";
        g1.cmd (plot_cmd.str ());
        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (200);
      }
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 4 notitle with lines ,";
        plot_cmd << " '' index 6 using 1:2:3:4 notitle with points pt 6 ps 0.3 linecolor variable ";
        g1.cmd (plot_cmd.str ());
        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (200);
      }

#endif
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
  return error_code;
}
