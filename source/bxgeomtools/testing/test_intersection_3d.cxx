// test_intersection_3d.cxx

// Ourselves:
#include <geomtools/intersection_3d.h>

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
#include <geomtools/box.h>
#include <geomtools/cylinder.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    bool debug = false;
    long seed = 314159;
    bool draw = false;
    bool zero = false;
    bool center2 = false;
    bool surf_first = false;
    bool surf_second = false;
    bool bulk = false;
    bool cyl2 = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-d" || arg == "--debug") debug = true;
      if (arg == "-D" || arg == "--draw") draw = true;
      if (arg == "-z" || arg == "--zero") zero = true;
      if (arg == "-c2" || arg == "--center2") center2 = true;
      if (arg == "-s1" || arg == "--surf1") surf_first = true;
      if (arg == "-s2" || arg == "--surf2") surf_second = true;
      // if (arg == "-B" || arg == "--bulk") bulk = true;
      if (arg == "-C2") {
        cyl2 = true;
      }

      iarg++;
    }

    srand48 (seed);

    using namespace geomtools;

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy (true);
    tmp_file.create ("/tmp", "test_intersection_3d_");

    box b1 (4.0 * CLHEP::mm,
            4.0 * CLHEP::mm,
            4.0 * CLHEP::mm);
    placement p1 (vector_3d (0, 0, 0),
                  0, 0, 0);

    box b2 (3.0 * CLHEP::mm,
            3.0 * CLHEP::mm,
            3.0 * CLHEP::mm);
    placement p2 (vector_3d (2, 2, 2),
                  M_PI / 3., 0.0, 0.0);
    b1.lock();
    b2.lock();

    cylinder c2 (2.0 * CLHEP::mm,
                 2.0 * CLHEP::mm);
    c2.lock();
    if (cyl2) {
      c2.tree_dump (std::clog, "Cylinder 2:");
    }

    intersection_3d inter1;
    inter1.set_shape1(b1, p1);
    if (!cyl2) {
      inter1.set_shape2(b2, p2);
    } else {
      inter1.set_shape2(c2, p2);
    }
    inter1.dump (std::clog);
    inter1.lock();

    box ibb;
    placement ibbp;
    inter1.get_bounding_data().compute_bounding_box(ibb, ibbp);

    geomtools::vector_3d pos (4.0 * CLHEP::mm,
                              3.0 * CLHEP::mm,
                              5.0 * CLHEP::mm);
    geomtools::vector_3d dir (-1., -1., -2.);
    geomtools::intercept_t intercept;
    std::clog << "test 1: pos=" << pos
              << " dir=" << dir
              << " impact=" << intercept.get_impact ()
              << std::endl;

    if (inter1.find_intercept (pos, dir, intercept)) {
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
    if (!cyl2) {
      geomtools::gnuplot_draw::draw_box (tmp_file.out(),
                                         p2.get_translation (),
                                         p2.get_rotation (),
                                         b2.get_x (),
                                         b2.get_y (),
                                         b2.get_z ());
    } else {
      geomtools::gnuplot_draw::draw_cylinder(tmp_file.out(),
                                             p2.get_translation(),
                                             p2.get_rotation(),
                                             c2.get_r(),
                                             c2.get_z());
    }
    tmp_file.out() << std::endl << std::endl;

    geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(), pos);
    tmp_file.out() << std::endl << std::endl;

    geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(),
                                               intercept.get_impact ());
    tmp_file.out() << std::endl << std::endl;

    geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(), pos);
    std::clog << "test 1: intercept=" << intercept.get_impact () << std::endl;
    if (intercept.is_valid ()) {
      geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(),
                                                 intercept.get_impact ());
    } else {
      geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(),
                                                 pos + 3 * dir);
    }
    tmp_file.out() << std::endl << std::endl;

    std::clog << "test 1: End." << std::endl;

    size_t nshoots = 100000;
    for (int i = 0; i < (int) nshoots; i++) {
      if ((i%1000) == 0) std::clog << "Loop #" << i << std::endl;

      double dim = 6. * CLHEP::mm;
      geomtools::vector_3d pos (0, 0, 0);
      if (! zero) pos.set(dim * drand48 (),
                          dim * drand48 (),
                          dim * drand48 ());
      if (center2) pos.set(2. * CLHEP::mm, 2. * CLHEP::mm, 2. * CLHEP::mm);

      geomtools::vector_3d dir;
      geomtools::randomize_direction (drand48, dir);

      geomtools::intercept_t intercept;
      // int intercept_face;
      if (inter1.find_intercept (pos, dir, intercept)) {
        if (debug) std::clog << "test 1: Intercept face="
                             << intercept.get_face ()
                             << " at impact="
                             << intercept.get_impact ()
                             << std::endl;
        geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(),
                                                   intercept.get_impact ());
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
      if (surf_first && inter1.is_on_surface(position,
                                             geomtools::intersection_3d::COMPONENT_SHAPE_FIRST, skin)) {
        tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                       << 3
                       << std::endl;
      } else if (surf_second && inter1.is_on_surface(position,
                                                     geomtools::intersection_3d::COMPONENT_SHAPE_SECOND, skin)) {
        tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                       << 4
                       << std::endl;
      } else if (bulk && inter1.is_inside(position, skin)) {
        tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                       << 2
                       << std::endl;
      }
    }
    tmp_file.out() << std::endl << std::endl;

    ibb.tree_dump(std::clog, "Intersection BB: ", "NOTICE: " );
    ibbp.tree_dump(std::clog, "Intersection BB placement: ", "NOTICE: " );
    tmp_file.out() <<"# Bounding box:" << std::endl;
    geomtools::gnuplot_draw::draw_box(tmp_file.out(),
                                      ibbp.get_translation(),
                                      ibbp.get_rotation(),
                                      ibb.get_x(),
                                      ibb.get_y(),
                                      ibb.get_z());
    tmp_file.out() << std::endl << std::endl;

    tmp_file.out() <<"# Intersection:" << std::endl;
    geomtools::gnuplot_draw::draw(tmp_file.out(),
                                  geomtools::placement(0,0,0,0,0,0),
                                  inter1);
    tmp_file.out() << std::endl << std::endl;

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd ("set title 'Test geomtools::intersection_3d' ");
      g1.cmd ("set grid");
      g1.cmd ("set key out");
      g1.cmd ("set size ratio -1");
      g1.cmd ("set view equal xyz");
      g1.cmd ("set xrange [-5:5]");
      g1.cmd ("set yrange [-5:5]");
      g1.cmd ("set zrange [-5:5]");
      g1.cmd ("set xyplane at -5");
      g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 0 title 'Intersection' with lines ";
        plot_cmd << ", '' index 1 title 'Source' with points pt 6 ps 1 ";
        plot_cmd << ", '' index 2 title 'Impact' with points pt 6 ps 1 ";
        plot_cmd << ", '' index 3 title 'Track' with lines ";
        plot_cmd << ", '' index 5 title 'BB' with lines lt 3 lw 3";
        g1.cmd (plot_cmd.str ());
        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (200);
      }
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 0 title 'Intersection' with lines ";
        plot_cmd << ", '' index 4 title 'Impacts' with dots ";
        g1.cmd (plot_cmd.str ());
        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (200);
      }
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename ()
                 << "' index 6 title 'Intersection' with lines ";
        plot_cmd << ", '' index 5 title 'BB' with lines lt 0";
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
