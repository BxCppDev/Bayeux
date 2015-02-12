// test_extruded_box.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

// This project:
#include <geomtools/geomtools_config.h>
#include <geomtools/extruded_box.h>
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
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    bool draw  = false;
    long seed  = 314159;
    bool test1 = true;
    bool test2 = true;

    int iarg = 1;
    while (iarg < argc_) {
      string arg = argv_[iarg];

      if (arg == "-D" || arg == "--draw") draw = true;
      if (arg == "-1") test1 = false;
      if (arg == "-2") test2 = false;

      iarg++;
    }

    srand48(seed);

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_extruded_box_");

    // test 1:
    if (test1) {
      geomtools::extruded_box my_box (2.0 * CLHEP::m,
                                      3.0 * CLHEP::m,
                                      0.5 * CLHEP::m,
                                      0.1 * CLHEP::m);
      my_box.remove_top();
      // my_box.grab_auxiliaries ().store ("color", "red");
      // my_box.grab_auxiliaries ().store ("material", "paper");
      // my_box.grab_auxiliaries ().store ("temperature", 300.* CLHEP::kelvin);
      my_box.tree_dump (clog, "my_box", "test 1: ");
      clog << "test 1: Extruded_Box #1         = " << my_box << " " << endl;

      clog << "test 1: Volume                  = " << my_box.get_volume () / CLHEP::m3
           << " m3" << endl;

      clog << "test 1: Top surface             = "
           << my_box.get_surface (geomtools::extruded_box::FACE_TOP) / CLHEP::m2
           << " m2" << endl;

      clog << "test 1: Bottom surface          = "
           << my_box.get_surface (geomtools::extruded_box::FACE_BOTTOM) / CLHEP::m2
           << " m2" << endl;

      clog << "test 1: Bottom surface (inside) = "
           << my_box.get_surface (geomtools::extruded_box::FACE_INSIDE_BOTTOM) / CLHEP::m2
           << " m2" << endl;

      clog << "test 1: Front surface           = "
           << my_box.get_surface (geomtools::extruded_box::FACE_FRONT) / CLHEP::m2
           << " m2" << endl;

      clog << "test 1: Front surface (inside)  = "
           << my_box.get_surface (geomtools::extruded_box::FACE_INSIDE_FRONT) / CLHEP::m2
           << " m2" << endl;

      clog << "test 1: Right surface           = "
           << my_box.get_surface (geomtools::extruded_box::FACE_RIGHT) / CLHEP::m2
           << " m2" << endl;

      clog << "test 1: Right surface (inside)  = "
           << my_box.get_surface (geomtools::extruded_box::FACE_INSIDE_RIGHT) / CLHEP::m2
           << " m2" << endl;

      clog << "test 1: Full surface            = "
           << my_box.get_surface (geomtools::extruded_box::FACE_ALL) / CLHEP::m2
           << " m2" << endl;

      {
        bool test_in = false;
        if (test_in) {
          clog << "test 1: Enter a extruded_box (example: '{extruded_box 1000 1000 1000}'): ";
          cin >> ws >> my_box;
          if (cin) {
            clog << "test 1: Extruded_Box     = " << my_box << " " << endl;
          } else {
            throw runtime_error ("Invalid input for extruded_box!");
          }
        }
      }
    }

    // test 2:
    if (test2) {
      geomtools::extruded_box my_box (9. * CLHEP::mm,
                                      9. * CLHEP::mm,
                                      9. * CLHEP::mm,
                                      3. * CLHEP::mm);
      my_box.remove_bottom();

      clog << "test 2: Extruded_Box #2 = " << my_box << " " << endl;
      geomtools::vector_3d extruded_box_pos;
      geomtools::rotation_3d extruded_box_rot;
      geomtools::create_rotation (extruded_box_rot, 0.0, 0.0, 0.0);
      geomtools::gnuplot_draw::draw_box (tmp_file.out(), extruded_box_pos, extruded_box_rot,
                                         my_box.get_x (),
                                         my_box.get_y (),
                                         my_box.get_z ());
      tmp_file.out() << endl << endl;
      extruded_box_pos.set (0., 0., -my_box.get_thickness()/2.);
      geomtools::gnuplot_draw::draw_box (tmp_file.out(), extruded_box_pos, extruded_box_rot,
                                         my_box.get_x () - 2. * my_box.get_thickness(),
                                         my_box.get_y () - 2. * my_box.get_thickness(),
                                         my_box.get_z () - my_box.get_thickness());
      tmp_file.out() << endl << endl;
      size_t nshoots = 100000;
      for (int i = 0; i < (int) nshoots; i++) {
        double dim = 5. * CLHEP::mm;
        geomtools::vector_3d position (dim * (-1 + 2 * drand48 ()),
                                       dim * (-1 + 2 * drand48 ()),
                                       dim * (-1 + 2 * drand48 ()));
        double skin = 0.2 * CLHEP::mm;
        if (my_box.is_outside(position, skin)) {
          tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                         << 1 << std::endl;
        } else if (my_box.is_inside(position, skin)) {
          tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                         << 2 << std::endl;
        } else if (my_box.is_on_surface(position, geomtools::extruded_box::FACE_ALL, skin)) {
          tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                         << 3 << std::endl;
        }
      }
      tmp_file.out() << endl << endl;
    }

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd ("set title 'Test geomtools::extruded_box' ");
      g1.cmd ("set grid");
      g1.cmd ("set size ratio -1");
      g1.cmd ("set view equal xyz");
      g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 0 lt 1 title 'Extruded_Box' with lines,";
        plot_cmd << " '' index 1 lt 1 notitle  with lines,";
        plot_cmd << " '' index 2 using 1:2:3:4 notitle with points pt 6 ps 0.3 linecolor variable";
        g1.cmd (plot_cmd.str ());
        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (2);
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
