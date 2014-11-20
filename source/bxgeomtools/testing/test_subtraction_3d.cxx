// test_subtraction_3d.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <geomtools/box.h>
#include <geomtools/subtraction_3d.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
#include <datatools/temporary_files.h>
#include <datatools/utils.h>

int
main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    bool debug = false;
    long seed = 314159;
    bool draw = false;
    // bool zero = false;
    // bool center2 = false;
    bool surf_first = false;
    bool surf_second = false;
    bool bulk = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-d" || arg == "--debug") debug = true;
      if (arg == "-D" || arg == "--draw") draw = true;
      // if (arg == "-z" || arg == "--zero") zero = true;
      // if (arg == "-c2" || arg == "--center2") center2 = true;

      if (arg == "-s1" || arg == "--surf1") surf_first = true;
      if (arg == "-s2" || arg == "--surf2") surf_second = true;
      if (arg == "-B" || arg == "--bulk") bulk = true;

      iarg++;
    }

    srand48 (seed);

    using namespace geomtools;

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy (true);
    tmp_file.create ("/tmp", "test_subtraction_3d_");

    box b1 (4.0 * CLHEP::mm,
            4.0 * CLHEP::mm,
            4.0 * CLHEP::mm);
    b1.lock();

    placement p1 (vector_3d (0, 0, 0),
                  0, 0, 0);

    box b2 (2.0 * CLHEP::mm,
            2.0 * CLHEP::mm,
            2.0 * CLHEP::mm);
    b2.lock();
    placement p2 (vector_3d (2, 2, 2),
                  M_PI / 3., M_PI / 3., M_PI / 6.);

    subtraction_3d sub1;
    sub1.set_shape1 (b1, p1);
    sub1.set_shape2 (b2, p2);
    sub1.lock();

    sub1.dump (std::clog);

    box sbb;
    placement sbbp;
    sub1.get_bounding_data().compute_bounding_box(sbb, sbbp);

    geomtools::vector_3d pos (4.0 * CLHEP::mm,
                              3.0 * CLHEP::mm,
                              6.0 * CLHEP::mm);
    geomtools::vector_3d dir (-1., -1., -1.);
    geomtools::intercept_t intercept;

    if (sub1.find_intercept (pos, dir, intercept)) {
      std::clog << "test 1: Intercept face=" << intercept.get_face ()
                << " at impact=" << intercept.get_impact ()
                << std::endl;
    } else {
      std::clog << "test 1: No intercept." << std::endl;
    }

    geomtools::gnuplot_draw::draw_box (tmp_file.out(),
                                       p1.get_translation (),
                                       p1.get_rotation (),
                                       b1.get_x (),
                                       b1.get_y (),
                                       b1.get_z ());
    geomtools::gnuplot_draw::draw_box (tmp_file.out(),
                                       p2.get_translation (),
                                       p2.get_rotation (),
                                       b2.get_x (),
                                       b2.get_y (),
                                       b2.get_z ());
    tmp_file.out() << std::endl << std::endl;

    geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(), pos);
    tmp_file.out() << std::endl << std::endl;

    geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(), intercept.get_impact ());
    tmp_file.out() << std::endl << std::endl;

    geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(), pos);
    geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(), intercept.get_impact ());
    tmp_file.out() << std::endl << std::endl;

    std::clog << "test 1: End." << std::endl;

    size_t nshoots = 500000;
    for (int i = 0; i < (int) nshoots; i++) {
      if ((i%1000) == 0) std::clog << "Loop #" << i << std::endl;
      //std::clog << "DEVEL: Loop #" << i << std::endl;

      // special debug activation:
      double dim = 6. * CLHEP::mm;
      geomtools::vector_3d pos (dim * drand48 (),
                                dim * drand48 (),
                                dim * drand48 ());
      geomtools::vector_3d dir;
      geomtools::randomize_direction (drand48, dir);

      geomtools::intercept_t intercept;
      if (sub1.find_intercept (pos, dir, intercept)) {
        if (debug) std::clog << "test 1: Intercept face="
                             << intercept.get_face ()
                             << " at impact="
                             << intercept.get_impact ()
                             << std::endl;
        geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(),
                                                   intercept.get_impact (),
                                                   false);
        int face = 0;
        switch (intercept.get_face ()) {
        case 0x1: face = 1; break;
        case 0x2: face = 2; break;
        case 0x4: face = 3; break;
        case 0x8: face = 4; break;
        case 0x10: face = 5; break;
        case 0x20: face = 6; break;
        }
        if (intercept.get_shape_index () == 1) face += 6;
        tmp_file.out() << ' ' << face;
        tmp_file.out() << std::endl;
      } else {
        if (debug) std::clog << "test 1: No intercept." << std::endl;
      }
    }
    tmp_file.out() << std::endl << std::endl;
    nshoots = 10000000;
    for (int i = 0; i < (int) nshoots; i++) {
      double dim = 6. * CLHEP::mm;
      geomtools::vector_3d position(0, 0, 0);
      position.set(dim * (-1 + 2 * drand48()),
                   dim * (-1 + 2 * drand48()),
                   dim * (-1 + 2 * drand48()));
      double skin = 0.1 * CLHEP::mm;
      if (surf_first && sub1.is_on_surface(position,
                                           geomtools::subtraction_3d::COMPONENT_SHAPE_FIRST, skin)) {
        tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                       << 3
                       << std::endl;
      } else if (surf_second && sub1.is_on_surface(position,
                                                   geomtools::subtraction_3d::COMPONENT_SHAPE_SECOND, skin)) {
        tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                       << 4
                       << std::endl;
      } else if (bulk && sub1.is_inside(position, skin)) {
        tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                       << 2
                       << std::endl;
      }
    }
    tmp_file.out() << std::endl << std::endl;


      sbb.tree_dump(std::clog, "Subtraction BB: ", "NOTICE: " );
      sbbp.tree_dump(std::clog, "Subtraction BB placement: ", "NOTICE: " );
      tmp_file.out() <<"# Bounding box:" << std::endl;
      geomtools::gnuplot_draw::draw_box(tmp_file.out(),
                                        sbbp.get_translation (),
                                        sbbp.get_rotation (),
                                        sbb.get_x (),
                                        sbb.get_y (),
                                        sbb.get_z ());
      tmp_file.out() << std::endl << std::endl;

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd ("set title 'Test geomtools::subtraction_3d' ");
      g1.cmd ("set grid");
      g1.cmd ("set key out");
      g1.cmd ("set size ratio -1");
      g1.cmd ("set view equal xyz");
      //g1.cmd ("set xyplane at -10");
      g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 0 title 'Subtraction' with lines ";
        plot_cmd << ", '' index 1 title 'Source' with points pt 6 ps 1 ";
        plot_cmd << ", '' index 2 title 'Impact' with points pt 6 ps 1 ";
        plot_cmd << ", '' index 3 title 'Track' with lines ";
        plot_cmd << ", '' index 5 title 'BB' with lines lt 7";
        g1.cmd (plot_cmd.str ());
        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (200);
      }
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 0 title 'Subtraction' with lines ";
        plot_cmd << ", '' index 4 using 1:2:3:4 title 'Impacts' with  points pt 6 ps 0.3 linecolor variable ";
        g1.cmd (plot_cmd.str ());
        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (200);
      }
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 0 title 'Subtraction' with lines , ";
        plot_cmd << " '' index 5 using 1:2:3:4 notitle with points pt 6 ps 0.3 linecolor variable ";
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
