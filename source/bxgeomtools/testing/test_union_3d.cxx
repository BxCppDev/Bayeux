// test_union_3d.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <geomtools/box.h>
#include <geomtools/union_3d.h>
#include <geomtools/gnuplot_draw.h>
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY
#include <datatools/temporary_files.h>
#include <datatools/utils.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      bool debug = false;
      long seed = 314159;
      bool interactive = false;
      bool draw = false;
      bool zero = false;
      bool center2 = false;
      bool surf_first = false;
      bool surf_second = false;
      bool bulk = false;

      int iarg = 1;
      while (iarg < argc_)
        {
          string arg = argv_[iarg];

          if (arg == "-d" || arg == "--debug") debug = true;
          if (arg == "-i" || arg == "--interactive") interactive = true;
          if (arg == "-D" || arg == "--draw") draw = true;
          if (arg == "-z" || arg == "--zero") zero = true;
          if (arg == "-c2" || arg == "--center2") center2 = true;

          if (arg == "-s1" || arg == "--surf1") surf_first = true;
          if (arg == "-s2" || arg == "--surf2") surf_second = true;
          if (arg == "-B" || arg == "--bulk") bulk = true;
          iarg++;
        }

      srand48 (seed);

      using namespace geomtools;

      datatools::temp_file tmp_file;
      tmp_file.set_remove_at_destroy (true);
      tmp_file.create ("/tmp", "test_union_3d_");

      box b1 (4.0 * CLHEP::mm,
              4.0 * CLHEP::mm,
              4.0 * CLHEP::mm);
      placement p1 (vector_3d (0, 0, 0),
                    0, 0, 0);

      box b2 (2.0 * CLHEP::mm,
              2.0 * CLHEP::mm,
              2.0 * CLHEP::mm);
      placement p2 (vector_3d (2, 2, 2),
                    M_PI / 3., M_PI / 3., M_PI / 6.);

      union_3d u1;
      u1.set_shape1 (b1, p1);
      u1.set_shape2 (b2, p2);
      u1.tree_dump (clog, "Union:");

      if (interactive) {
        char c;
        cin >> c;
      }

      geomtools::vector_3d pos (4.0 * CLHEP::mm,
                                3.0 * CLHEP::mm,
                                6.0 * CLHEP::mm);
      geomtools::vector_3d dir (-1., -1., -1.);
      geomtools::intercept_t intercept;

      if (u1.find_intercept (pos, dir, intercept)) {
        clog << "test 1: Intercept face=" << intercept.get_face ()
             << " at impact=" << intercept.get_impact ()
             << endl;
      } else {
        clog << "test 1: No intercept." << endl;
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
      tmp_file.out() << endl << endl;

      geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(), pos);
      tmp_file.out() << endl << endl;

      geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(), intercept.get_impact ());
      tmp_file.out() << endl << endl;

      geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(), pos);
      geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(), intercept.get_impact ());
      tmp_file.out() << endl << endl;

      clog << "test 1: End." << endl;

      size_t nshoots = 50000;
      for (int i = 0; i < (int) nshoots; i++) {
        if ((i%10000) == 0) clog << "Loop #" << i << endl;
        // clog << "DEVEL: Loop #" << i << endl;

        // special debug activation:
        // int idevel = -1;
        // geomtools::union_3d::g_devel = false;
        // idevel = 1817;
        // if (i == idevel)
        //   {
        //     geomtools::union_3d::g_devel = true;
        //   }

        double dim = 6. * CLHEP::mm;
        geomtools::vector_3d pos (0, 0, 0);
        if (! zero) pos.set(dim * drand48 (),
                            dim * drand48 (),
                            dim * drand48 ());
        if (center2) pos.set(2. * CLHEP::mm, 2. * CLHEP::mm, 2. * CLHEP::mm);

        geomtools::vector_3d dir;
        geomtools::randomize_direction (drand48, dir);

        geomtools::intercept_t intercept;
        if (u1.find_intercept (pos, dir, intercept)) {
          if (debug) clog << "test 1: Intercept face="
                          << intercept.get_face ()
                          << " at impact="
                          << intercept.get_impact ()
                          << endl;
          geomtools::gnuplot_draw::basic_draw_point (tmp_file.out(),
                                                     intercept.get_impact (),
                                                     false);
          int face = intercept.get_face ();
          tmp_file.out() << ' ' << face;
          tmp_file.out() << endl;
        } else {
          if (debug) clog << "test 1: No intercept." << endl;
        }
      }
      tmp_file.out() << endl << endl;

      nshoots = 10000000;
      for (int i = 0; i < (int) nshoots; i++)  {
        double dim = 6. * CLHEP::mm;
        geomtools::vector_3d position(0, 0, 0);
        position.set(dim * (-1 + 2 * drand48()),
                     dim * (-1 + 2 * drand48()),
                     dim * (-1 + 2 * drand48()));
        double skin = 0.1 * CLHEP::mm;
        if (surf_first && u1.is_on_surface(position,
                                           geomtools::union_3d::COMPONENT_SHAPE_FIRST, skin)) {
          tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                         << 3
                         << std::endl;
        } else if (surf_second && u1.is_on_surface(position,
                                                   geomtools::union_3d::COMPONENT_SHAPE_SECOND, skin)) {
          tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                         << 4
                         << std::endl;
        } else if (bulk && u1.is_inside(position, skin)) {
          tmp_file.out() << position.x() << ' ' << position.y() << ' ' << position.z() << ' '
                         << 2
                         << std::endl;
        }
      }
      tmp_file.out() << endl << endl;

      if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
        Gnuplot g1;
        g1.cmd ("set title 'Test geomtools::union_3d' ");
        g1.cmd ("set grid");
        g1.cmd ("set size ratio -1");
        g1.cmd ("set view equal xyz");
        //g1.cmd ("set xyplane at -10");
        g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
        {
          std::ostringstream plot_cmd;
          plot_cmd << "splot '" << tmp_file.get_filename () << "' index 0 title 'Union' with lines ";
          plot_cmd << ", '' index 1 title 'Source' with points pt 6 ps 1 ";
          plot_cmd << ", '' index 2 title 'Impact' with points pt 6 ps 1 ";
          plot_cmd << ", '' index 3 title 'Track' with lines ";
          g1.cmd (plot_cmd.str ());
          g1.showonscreen (); // window output
          geomtools::gnuplot_drawer::wait_for_key ();
          usleep (200);
        }
        {
          std::ostringstream plot_cmd;
          plot_cmd << "splot '" << tmp_file.get_filename () << "' index 0 title 'Union' with lines ";
          plot_cmd << ", '' index 4 title 'Impacts' with dots ";
          g1.cmd (plot_cmd.str ());
          g1.showonscreen (); // window output
          geomtools::gnuplot_drawer::wait_for_key ();
          usleep (200);
        }
        {
          std::ostringstream plot_cmd;
          plot_cmd << "splot '" << tmp_file.get_filename () << "' index 0 title 'Union' with lines , ";
          plot_cmd << " '' index 5 using 1:2:3:4 notitle with points pt 6 ps 0.3 linecolor variable ";
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
