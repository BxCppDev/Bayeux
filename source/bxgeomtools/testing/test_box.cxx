// test_box.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

// This project:
#include <geomtools/geomtools_config.h>
#include <geomtools/box.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

// Third party:
// - Bayeux/datatools:
#include <datatools/temporary_files.h>
#include <datatools/utils.h>

using namespace std;

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

    bool front = false;
    bool back = false;
    bool top = false;
    bool bottom = false;
    bool left = false;
    bool right = false;
    bool bulk = false;

    int iarg = 1;
    while (iarg < argc_) {
      string arg = argv_[iarg];

      if (arg == "-d" || arg == "--debug") debug = true;
      if (arg == "-D" || arg == "--draw") draw = true;
      if (arg == "-1") test1 = false;
      if (arg == "-2") test2 = false;
      if (arg == "-3") test3 = false;

      if (arg == "-b") back = true;
      if (arg == "-f") front = true;
      if (arg == "-t") top = true;
      if (arg == "-m") bottom = true;
      if (arg == "-l") left = true;
      if (arg == "-r") right = true;
      if (arg == "-B") bulk = true;

      iarg++;
    }

    srand48 (seed);

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_box_");

    // test 1:
    if (test1) {
      geomtools::box my_box (2.0 * CLHEP::m,
                             3.0 * CLHEP::m,
                             0.5 * CLHEP::m);
      // my_box.grab_auxiliaries ().store ("color", "red");
      // my_box.grab_auxiliaries ().store ("material", "paper");
      // my_box.grab_auxiliaries ().store ("temperature", 300.* CLHEP::kelvin);
      my_box.tree_dump (clog, "my_box", "test 1: ");
      clog << "test 1: Box #1  = " << my_box << " " << endl;

      clog << "test 1: Volume  = " << my_box.get_volume () / CLHEP::m3
           << " m3" << endl;

      clog << "test 1: Front surface = "
           << my_box.get_surface (geomtools::box::FACE_FRONT) / CLHEP::m2
           << " m2" << endl;

      clog << "test 1: Top surface = "
           << my_box.get_surface (geomtools::box::FACE_TOP) / CLHEP::m2
           << " m2" << endl;

      clog << "test 1: Right surface = "
           << my_box.get_surface (geomtools::box::FACE_RIGHT) / CLHEP::m2
           << " m2" << endl;

      clog << "test 1: Full surface = "
           << my_box.get_surface (geomtools::box::FACE_ALL) / CLHEP::m2
           << " m2" << endl;

      {
        bool test_in = false;
        if (test_in) {
          clog << "test 1: Enter a box (example: '{box 1000 1000 1000}'): ";
          cin >> ws >> my_box;
          if (cin) {
            clog << "test 1: Box     = " << my_box << " " << endl;
          } else {
            throw runtime_error ("Invalid input for box!");
          }
        }
      }
    }

    // test 2:
    if (test2) {
      geomtools::box my_box (8.0 * CLHEP::mm,
                             4.0 * CLHEP::mm,
                             1.0 * CLHEP::mm);
      clog << "test 2: Box #2   = " << my_box << " " << endl;

      geomtools::vector_3d pos (2.0 * CLHEP::mm,
                                3.0 * CLHEP::mm,
                                2.0 * CLHEP::mm);
      geomtools::vector_3d dir (-1, -2.5, -1.5);
      //dir = -pos;
      geomtools::intercept_t intercept;
      // int intercept_face;
      if (my_box.find_intercept (pos, dir, intercept)) {
        clog << "test 2: Intercept face="
             << intercept.get_face ()
             << " at impact="
             << intercept.get_impact ()
             << endl;
      } else {
        clog << "test 2: No intercept." << endl;
      }

      geomtools::vector_3d box_pos;
      geomtools::rotation_3d box_rot;
      geomtools::create_rotation (box_rot, 0.0, 0.0, 0.0);
      geomtools::gnuplot_draw::draw_box (tmp_file.out(), box_pos, box_rot,
                                         my_box.get_x (),
                                         my_box.get_y (),
                                         my_box.get_z ());
      tmp_file.out() << endl << endl;

      geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(), pos);
      tmp_file.out() << endl << endl;

      geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(),
                                                 intercept.get_impact ());
      tmp_file.out() << endl << endl;

      geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(), pos);
      geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(), pos + dir);
      tmp_file.out() << endl << endl;
    }

    // test 3:
    if (test3) {
      geomtools::box my_box (5.0 * CLHEP::mm,
                             5.0 * CLHEP::mm,
                             5.0 * CLHEP::mm);
      clog << "test 3: Box #3   = " << my_box << " " << endl;
      geomtools::vector_3d box_pos;
      geomtools::rotation_3d box_rot;
      geomtools::create_rotation (box_rot, 0.0, 0.0, 0.0);
      geomtools::gnuplot_draw::draw_box (tmp_file.out(), box_pos, box_rot,
                                         my_box.get_x (),
                                         my_box.get_y (),
                                         my_box.get_z ());
      tmp_file.out() << endl << endl;
      size_t nshoots = 100000;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d pos (10.* drand48 () * CLHEP::mm,
                                  10.* drand48 () * CLHEP::mm,
                                  10.* drand48 () * CLHEP::mm);
        geomtools::vector_3d dir;
        geomtools::randomize_direction (drand48, dir);

        geomtools::intercept_t intercept;
        // int intercept_face;
        if (my_box.find_intercept (pos, dir, intercept)) {
          if (debug) clog << "test 3: Intercept face="
                          << intercept.get_face ()
                          << " at impact=" << intercept.get_impact ()
                          << endl;
          geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(),
                                                     intercept.get_impact ());
        } else {
          if (debug) clog << "test 3: No intercept." << endl;
        }
      }
      tmp_file.out() << endl << endl;

      nshoots = 1000000;
      for (int i = 0; i < (int) nshoots; i++) {
        double dim = 10. * CLHEP::mm;
        geomtools::vector_3d position (dim * ( -1 + 2 * drand48 ()),
                                       dim * ( -1 + 2 * drand48 ()),
                                       dim * ( -1 + 2 * drand48 ()));
        double skin = 0.25 * CLHEP::mm;
        if (back && my_box.is_on_surface(position,
                                         geomtools::box::FACE_BACK, skin)) {
          tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                         << 3
                         << std::endl;
        } else if (front && my_box.is_on_surface(position,
                                                 geomtools::box::FACE_FRONT, skin)) {
          tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                         << 4
                         << std::endl;
        } else if (top && my_box.is_on_surface(position,
                                               geomtools::box::FACE_TOP, skin)) {
          tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                         << 5
                         << std::endl;
        } else if (bottom && my_box.is_on_surface(position,
                                                  geomtools::box::FACE_BOTTOM, skin)) {
          tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                         << 6
                         << std::endl;
        } else if (left && my_box.is_on_surface(position,
                                                geomtools::box::FACE_LEFT, skin)) {
          tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                         << 7
                         << std::endl;
        } else if (right && my_box.is_on_surface(position,
                                                 geomtools::box::FACE_RIGHT, skin)) {
          tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                         << 8
                         << std::endl;
        } else if (bulk && my_box.is_inside(position, skin)) {
          tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                         << 2
                         << std::endl;
        }

      }
      tmp_file.out() << endl << endl;
    }

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd ("set title 'Test geomtools::box' ");
      g1.cmd ("set grid");
      g1.cmd ("set size ratio -1");
      g1.cmd ("set view equal xyz");
      //g1.cmd ("set xyplane at -10");
      g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 0 title 'Box' with lines ,";
        plot_cmd << " '' index 1 title 'Source' with points lt 3 pt 7 ps 1.0 ,";
        plot_cmd << " '' index 2 title 'Impact' with points lt 4 pt 7 ps 1.0 ,";
        plot_cmd << " '' index 3 title 'Track'  with lines lt 2 ";
        g1.cmd (plot_cmd.str ());
        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (200);
      }
      {
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
  catch (exception & x) {
    cerr << "error: " << x.what () << endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    cerr << "error: " << "unexpected error!" << endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}
