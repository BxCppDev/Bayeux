// test_subtraction_3d.cxx

// Ourselves:
#include <geomtools/subtraction_3d.h>

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
    // bool zero = false;
    // bool center2 = false;
    bool surf_first = false;
    bool surf_second = false;
    bool bulk = false;
    bool cyl2 = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-d" || arg == "--debug") debug = true;
      if (arg == "-D" || arg == "--draw") draw = true;
      // if (arg == "-z" || arg == "--zero") zero = true;
      // if (arg == "-c2" || arg == "--center2") center2 = true;
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
    if (!cyl2) {
      b2.tree_dump (std::clog, "Box 2:");
    }
    placement p2 (vector_3d (2, 2, 2),
                  M_PI / 3., M_PI / 3., M_PI / 6.);

    cylinder c2 (1.0 * CLHEP::mm,
                 10.0 * CLHEP::mm);
    c2.lock();
    if (cyl2) {
      p2.set_translation(1,1,1);
      c2.tree_dump (std::clog, "Cylinder 2:");
    }

    subtraction_3d sub1;
    sub1.set_shape1(b1, p1);
    if (!cyl2) {
      sub1.set_shape2(b2, p2);
    } else {
      sub1.set_shape2(c2, p2);
    }
    sub1.lock();

    sub1.tree_dump(std::clog, "Subtraction 3D: ");

    box sbb;
    placement sbbp;
    sub1.get_bounding_data().compute_bounding_box(sbb, sbbp);

    geomtools::vector_3d pos (4.0 * CLHEP::mm,
                              3.0 * CLHEP::mm,
                              6.0 * CLHEP::mm);
    geomtools::vector_3d dir (-1., -1., -1.);
    geomtools::face_intercept_info intercept;

    double tolerance = 0.01;
    if (sub1.find_intercept (pos, dir, intercept, tolerance)) {
      std::clog << "test 1: Intercept face=" << intercept.get_face_id ()
                << " at impact=" << intercept.get_impact ()
                << std::endl;
    } else {
      std::clog << "test 1: No intercept." << std::endl;
    }

    tmp_file.out() << "# draw box (index 0): " << std::endl;
    geomtools::gnuplot_draw::draw_box (tmp_file.out(),
                                       p1.get_translation (),
                                       p1.get_rotation (),
                                       b1.get_x (),
                                       b1.get_y (),
                                       b1.get_z ());
    if (!cyl2) {
      tmp_file.out() << "# draw box 2 (index 0): " << std::endl;
      geomtools::gnuplot_draw::draw_box (tmp_file.out(),
                                         p2.get_translation (),
                                         p2.get_rotation (),
                                         b2.get_x (),
                                         b2.get_y (),
                                         b2.get_z ());
    } else {
      tmp_file.out() << "# draw cyl 2 (index 0): " << std::endl;
      geomtools::gnuplot_draw::draw_cylinder(tmp_file.out(),
                                             p2.get_translation(),
                                             p2.get_rotation(),
                                             c2.get_r(),
                                             c2.get_z());
    }
    tmp_file.out() << std::endl << std::endl;

    tmp_file.out() << "# draw source (index 1): " << std::endl;
    geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(), pos);
    tmp_file.out() << std::endl << std::endl;

    tmp_file.out() << "# draw intercept (index 2): " << std::endl;
    geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(), intercept.get_impact ());
    tmp_file.out() << std::endl << std::endl;

    tmp_file.out() << "# draw traj (index 3): " << std::endl;
    geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(), pos);
    geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(), intercept.get_impact ());
    tmp_file.out() << std::endl << std::endl;

    std::clog << "test 1: End." << std::endl;

    {
      bool do_intercept = false;
      tmp_file.out() << "# draw intercepts (index 4): " << std::endl;
      size_t nshoots = 500000;
      int count = 0;
      for (int i = 0; i < (int) nshoots; i++) {
        if ((i%1000) == 0) std::clog << "Loop #" << i << std::endl;
        //std::clog << "DEVEL: Loop #" << i << std::endl;

        // special debug activation:
        double dim = 6. * CLHEP::mm;
        geomtools::vector_3d local_pos (dim * drand48 (),
                                  dim * drand48 (),
                                  dim * drand48 ());
        geomtools::vector_3d local_dir;
        geomtools::randomize_direction (drand48, local_dir);

        geomtools::face_intercept_info local_intercept;
        double local_tolerance = 0.01;
        if (do_intercept && sub1.find_intercept (local_pos, local_dir, local_intercept, local_tolerance)) {
          if (debug) std::clog << "test 1: Intercept face="
                               << local_intercept.get_face_id ()
                               << " at impact="
                               << local_intercept.get_impact ()
                               << std::endl;
          geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(),
                                                     local_intercept.get_impact(),
                                                     false);
          int face = 0;
          switch (local_intercept.get_face_id().get_face_bits()) {
          case 0x1: face = 1; break;
          case 0x2: face = 2; break;
          case 0x4: face = 3; break;
          case 0x8: face = 4; break;
          case 0x10: face = 5; break;
          case 0x20: face = 6; break;
          }
          if (local_intercept.get_face_id().get_part(0) == 1) face += 6;
          tmp_file.out() << ' ' << face;
          tmp_file.out() << std::endl;
          count++;
        } else {
          if (debug) std::clog << "test 1: No intercept." << std::endl;
        }
      }
      if (!count) {
        tmp_file.out() << "0 0 0 1" << std::endl;
      }
      tmp_file.out() << std::endl << std::endl;
    }

    {
      tmp_file.out() << "# draw locate (index 5): " << std::endl;
      int nshoots = 10000000;
      int count = 0;
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
          count++;
        } else if (surf_second && sub1.is_on_surface(position,
                                                     geomtools::subtraction_3d::COMPONENT_SHAPE_SECOND, skin)) {
          tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                         << 4
                         << std::endl;
          count++;
        } else if (bulk && sub1.check_inside(position, skin)) {
          tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                         << 2
                         << std::endl;
          count++;
        }
      }
      if (!count) {
        tmp_file.out() << "0 0 0 1" << std::endl;
      }
      tmp_file.out() << std::endl << std::endl;
    }

    sbb.tree_dump(std::clog, "Subtraction BB: ", "NOTICE: " );
    sbbp.tree_dump(std::clog, "Subtraction BB placement: ", "NOTICE: " );
    tmp_file.out() <<"# Bounding box (index 6):" << std::endl;
    geomtools::gnuplot_draw::draw_box(tmp_file.out(),
                                      sbbp.get_translation (),
                                      sbbp.get_rotation (),
                                      sbb.get_x (),
                                      sbb.get_y (),
                                      sbb.get_z ());
    tmp_file.out() << std::endl << std::endl;

    tmp_file.out() <<"# Subtraction (index 7):" << std::endl;
    geomtools::gnuplot_draw::draw(tmp_file.out(),
                                  geomtools::placement(0,0,0,0,0,0),
                                  sub1,
                                  geomtools::subtraction_3d::WR_NONE
                                  | geomtools::subtraction_3d::WR_COMPOSITE_BOOST_SAMPLING
                                  );
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
        plot_cmd << ", '' index 6 title 'BB' with lines lt 7";
        g1.cmd (plot_cmd.str ());
        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (200);
      }
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 0 title 'Subtraction' with lines ";
        plot_cmd << ", '' index 4 using 1:2:3:4 title 'Impacts' with dots ";
        g1.cmd (plot_cmd.str ());
        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (200);
      }
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename()
                 << "' index 7 title 'Subtraction' with lines ";
        plot_cmd << ", '' index 5 title 'Vertexes' with point pt 6 ps 0.35 linecolor variable";
        g1.cmd(plot_cmd.str());
        g1.showonscreen(); // window output
        geomtools::gnuplot_drawer::wait_for_key();
        usleep(200);
      }
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename ()
                 << "' index 7 title 'Subtraction' with lines ";
        plot_cmd << ", '' index 6 title 'BB' with lines lt -1";
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
