// test_regular_circular_placement.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <unistd.h>

// This project:
#include <geomtools/geomtools_config.h>
#include <geomtools/regular_circular_placement.h>
#include <geomtools/box.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_drawer.h>
#include <geomtools/gnuplot_i.h>
#endif

#include <datatools/temporary_files.h>

using namespace std;

void test1(bool draw_)
{
  std::clog << "\ntest1" << endl;
    geomtools::vector_3d basic_position(0. * CLHEP::cm, 0. * CLHEP::cm, 0. * CLHEP::cm);
    cout << "Basic position : " << basic_position << endl;

    double phi   = -30.0 * CLHEP::degree;
    double theta =  75.0 * CLHEP::degree;
    double delta =   0.0 * CLHEP::degree;

    double radius      = 10.0 * CLHEP::cm;
    double step_angle  = 30.0 * CLHEP::degree;
    double start_angle =  40.0 * CLHEP::degree;


    geomtools::regular_circular_placement rlp(basic_position,
                                              radius,
                                              step_angle,
                                              start_angle,
                                              8,
                                              geomtools::ROTATION_AXIS_Z);
    rlp.tree_dump(clog, "regular_linear_placement", ">>> ");

    for (int i = 0; i < (int) rlp.get_number_of_items(); i++) {
      ostringstream title;
      title << "Item #" << i << " placement:";
      geomtools::placement pi = rlp.i_placement::get_placement(i);
      pi.tree_dump(clog, title.str(), ">>> ");
    }

#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    if (draw_) {
      datatools::temp_file tmp_file;
      tmp_file.set_remove_at_destroy (true);
      tmp_file.create ("/tmp", ".tmp_drawer_");

      geomtools::box b (3.0 * CLHEP::cm, 1.0 * CLHEP::cm, 2.0 * CLHEP::cm);
      for (int i = 0; i < (int) rlp.get_number_of_items (); i++) {
        geomtools::gnuplot_draw::draw (tmp_file.out (),
                                       rlp.i_placement::get_placement (i),
                                       b);
      }
      tmp_file.close ();
      Gnuplot g1 ("lines");
      g1.cmd ("set grid");
      g1.cmd ("set size ratio -1");
      g1.cmd ("set view equal xyz");
      g1.set_title ("test_regular_linear_placement");
      g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
      g1.plotfile_xyz (tmp_file.get_filename (), 1, 2, 3, "3D view");
      g1.showonscreen ();
      geomtools::gnuplot_drawer::wait_for_key ();
      usleep (200);
    }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
  return;
}


void test2(bool draw_)
{
  std::clog << "\ntest2" << endl;
    geomtools::vector_3d basic_position(0. * CLHEP::cm, 0. * CLHEP::cm, 0. * CLHEP::cm);
    cout << "Basic position : " << basic_position << endl;

    double phi   = -30.0 * CLHEP::degree;
    double theta =  75.0 * CLHEP::degree;
    double delta =   0.0 * CLHEP::degree;

    double radius      = 10.0 * CLHEP::cm;
    double step_angle  = 30.0 * CLHEP::degree;
    double start_angle =  40.0 * CLHEP::degree;


    geomtools::regular_circular_placement rlp(basic_position,
                                              radius,
                                              step_angle,
                                              start_angle,
                                              8,
                                              geomtools::ROTATION_AXIS_Z);
    rlp.set_follow_rotation(false);
    rlp.tree_dump(clog, "regular_linear_placement", ">>> ");

    for (int i = 0; i < (int) rlp.get_number_of_items(); i++) {
      ostringstream title;
      title << "Item #" << i << " placement:";
      geomtools::placement pi = rlp.i_placement::get_placement(i);
      pi.tree_dump(clog, title.str(), ">>> ");
    }

#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    if (draw_) {
      datatools::temp_file tmp_file;
      tmp_file.set_remove_at_destroy (true);
      tmp_file.create ("/tmp", ".tmp_drawer_");

      geomtools::box b (3.0 * CLHEP::cm, 1.0 * CLHEP::cm, 2.0 * CLHEP::cm);
      for (int i = 0; i < (int) rlp.get_number_of_items (); i++) {
        geomtools::gnuplot_draw::draw (tmp_file.out (),
                                       rlp.i_placement::get_placement (i),
                                       b);
      }
      tmp_file.close ();
      Gnuplot g1 ("lines");
      g1.cmd ("set grid");
      g1.cmd ("set size ratio -1");
      g1.cmd ("set view equal xyz");
      g1.set_title ("test_regular_linear_placement");
      g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
      g1.plotfile_xyz (tmp_file.get_filename (), 1, 2, 3, "3D view");
      g1.showonscreen ();
      geomtools::gnuplot_drawer::wait_for_key ();
      usleep (200);
    }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
  return;
}


void test3(bool draw_)
{
  std::clog << "\ntest3" << endl;
    geomtools::vector_3d basic_position(0. * CLHEP::cm, 0. * CLHEP::cm, 0. * CLHEP::cm);
    cout << "Basic position : " << basic_position << endl;

    double phi   = -30.0 * CLHEP::degree;
    double theta =  75.0 * CLHEP::degree;
    double delta =   0.0 * CLHEP::degree;

    double radius      = 10.0 * CLHEP::cm;
    double step_angle  = 30.0 * CLHEP::degree;
    double start_angle =  40.0 * CLHEP::degree;


    geomtools::regular_circular_placement rlp(basic_position,
                                              radius,
                                              step_angle,
                                              start_angle,
                                              8,
                                              geomtools::ROTATION_AXIS_X);
    // rlp.set_follow_rotation(false);
    rlp.tree_dump(clog, "regular_linear_placement", ">>> ");

    for (int i = 0; i < (int) rlp.get_number_of_items(); i++) {
      ostringstream title;
      title << "Item #" << i << " placement:";
      geomtools::placement pi = rlp.i_placement::get_placement(i);
      pi.tree_dump(clog, title.str(), ">>> ");
    }

#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    if (draw_) {
      datatools::temp_file tmp_file;
      tmp_file.set_remove_at_destroy (true);
      tmp_file.create ("/tmp", ".tmp_drawer_");

      geomtools::box b (3.0 * CLHEP::cm, 1.0 * CLHEP::cm, 2.0 * CLHEP::cm);
      for (int i = 0; i < (int) rlp.get_number_of_items (); i++) {
        geomtools::gnuplot_draw::draw (tmp_file.out (),
                                       rlp.i_placement::get_placement (i),
                                       b);
      }
      tmp_file.close ();
      Gnuplot g1 ("lines");
      g1.cmd ("set grid");
      g1.cmd ("set size ratio -1");
      g1.cmd ("set view equal xyz");
      g1.set_title ("test_regular_linear_placement");
      g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
      g1.plotfile_xyz (tmp_file.get_filename (), 1, 2, 3, "3D view");
      g1.showonscreen ();
      geomtools::gnuplot_drawer::wait_for_key ();
      usleep (200);
    }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
  return;
}

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    // bool debug = false;
    bool draw = false;
    int iarg = 1;
    while (iarg < argc_) {
      string token = argv_[iarg];

      if (token[0] == '-') {
        string option = token;
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
        if (option == "--no-draw") {
          draw = false;
        } else if (option == "--draw") {
          draw = true;
        }
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
        else {
          clog << "warning: ignoring option '" << option << "'!" << endl;
        }
      } else {
        string argument = token;
        {
          clog << "warning: ignoring argument '" << argument << "'!" << endl;
        }
      }
      iarg++;
    }

    test1(draw);

    test2(draw);

    test3(draw);

  }
  catch (exception & x) {
    cerr << "error: " << x.what() << endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    cerr << "error: " << "unexpected error!" << endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}
