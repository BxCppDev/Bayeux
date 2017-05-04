// test_helix.cxx

// Ourselves:
#include <geomtools/helix_3d.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

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

void test0();

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    bool draw = false;
    bool only_test_0 = false;
    bool do_identity = false;
    bool do_curve = true;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-D" || arg == "--draw")
        draw = true;
      if (arg == "-0" || arg == "--only-test-0")
        only_test_0 = true;
      if (arg == "-I" || arg == "--identity") do_identity = true;

      iarg++;
    }

    test0 ();
    if (only_test_0) {
      return error_code;
    }

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy(true);
    tmp_file.create("/tmp", "test_helix_3d_");

    geomtools::helix_3d my_helix_1;
    geomtools::helix_3d my_helix_2;

    geomtools::vector_3d helix_center_1( 0., 0., 0. );
    geomtools::placement helix_placement_1( 0., 0., 0. ,
                                            30 * CLHEP::degree,
                                            25 * CLHEP::degree,
                                            0.0);
    if (do_identity) {
      helix_placement_1.set_identity();
    }

    my_helix_1.set_radius( 1. * CLHEP::mm  );
    my_helix_1.set_center( helix_center_1 );
    my_helix_1.set_step( 0.5 * CLHEP::mm  );
    my_helix_1.set_t1( 0.0 );
    my_helix_1.set_t2( 2.0 );

    geomtools::vector_3d helix_center_2( 0., 0., 0. );
    geomtools::placement helix_placement_2( 0., 0., 0. ,
                                            20 * CLHEP::degree,
                                            60 * CLHEP::degree,
                                            0.0);

    my_helix_2.set_radius( 0.5 * CLHEP::mm );
    my_helix_2.set_center( helix_center_2 );
    my_helix_2.set_step( -0.25 * CLHEP::mm );
    my_helix_2.set_t1( 0.0 );
    my_helix_2.set_t2( 0.5 );

    my_helix_1.tree_dump(std::clog, "helix #1:" );
    std::clog << std::endl;

    my_helix_2.tree_dump(std::clog, "helix #2:" );
    std::clog << std::endl;

    {
      // Draw helix (index 0):
      tmp_file.out() << "# helix 1: " << std::endl;
      geomtools::gnuplot_draw::draw_helix(tmp_file.out(),
                                          helix_placement_1,
                                          my_helix_1,
                                          geomtools::helix_3d::WR_HELIX_NO_ANGULAR_SAMPLE_BOOST
                                          );
      tmp_file.out() << std::endl;

      // Draw helix (index 1):
      tmp_file.out() << "# helix 2: " << std::endl;
      geomtools::gnuplot_draw::draw_helix(tmp_file.out(),
                                          helix_placement_2,
                                          my_helix_2,
                                          geomtools::helix_3d::WR_BASE_HIGH_ANGLE_SAMPLING
                                          );
      tmp_file.out() << std::endl;
    }

    {
      // Draw on curve (index 2):
      tmp_file.out() << "# on curve: " << std::endl;
      size_t  nshoots = 500000;
      int counts = 0;
      int counts2 = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d pos((-2 + 4.* drand48()),
                                 (-2 + 4.* drand48()),
                                 (-2 + 4 * drand48()));
        geomtools::vector_3d pos_c;
        helix_placement_1.mother_to_child(pos, pos_c);
        double tolerance = 0.25;
        if (do_curve) {
          if (my_helix_1.is_on_curve(pos_c, tolerance)) {
            counts++;
            geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), pos, 3.0);
          }
        }
      }
      if (counts == 0 && counts2 == 0) {
        tmp_file.out() << "0 0 0 0.0" << std::endl;
      }
      tmp_file.out() << std::endl << std::endl;
    }

    {
      // Draw on curve (index 3):
      tmp_file.out() << "# on curve: " << std::endl;
      size_t  nshoots = 500000;
      int counts = 0;
      int counts2 = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        geomtools::vector_3d pos((-2 + 4.* drand48()),
                                 (-2 + 4.* drand48()),
                                 (-2 + 4 * drand48()));
        geomtools::vector_3d pos_c;
        helix_placement_2.mother_to_child(pos, pos_c);
        double tolerance = 0.25;
        if (do_curve) {
          if (my_helix_2.is_on_curve(pos_c, tolerance)) {
            counts++;
            geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), pos, 4.0);
          }
        }
      }
      if (counts == 0 && counts2 == 0) {
        tmp_file.out() << "0 0 0 0.0" << std::endl;
      }
      tmp_file.out() << std::endl << std::endl;
    }

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd("set title 'Test geomtools::helix_3d' ");
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
                 << "' index 0 title 'Helix 1' with lines ";
        plot_cmd << ", '' index 1 title 'Helix 2' with lines ";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 0 title 'Helix_1' with lines ";
        plot_cmd << ", '' index 1 title 'Helix 2' with lines ";
        plot_cmd << ", '' index 2:3 using 1:2:3:4 title 'On curve(s)' with points pt 6 ps 0.5 linecolor variable";
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
  return error_code;
}

void test0 ()
{
  std::clog << "\n********** test0: Check 'get_length' value \n";
  geomtools::vector_3d O( 0., 0., 0. );

  geomtools::helix_3d H;
  H.set_radius ( 1. * CLHEP::m  );
  H.set_center ( O );
  H.set_step   ( 1. * CLHEP::m );
  H.set_t1     ( 0.0 );
  H.set_t2     ( 0.1 );

  double l = H.get_length ();

  std::clog << "H length = " << l /CLHEP::mm  << " mm" << std::endl;

  double sum = 0.0;
  double dt =  1.e-4;
  for (double t = H.get_t1 (); t <= H.get_t2 (); t += dt) {
    double tp = t + dt;
    geomtools::vector_3d p = H.get_point (t);
    geomtools::vector_3d pp = H.get_point (tp);
    sum += (pp - p).mag ();
  }
  double S = H.get_curvilinear_position (H.get_t2 ())
    - H.get_curvilinear_position (H.get_t1 ());
  std::clog << "H sum-length = " << sum / CLHEP::mm  << " mm" << std::endl;
  std::clog << "H S-length = " << S / CLHEP::mm  << " mm" << std::endl;

  return;
}
