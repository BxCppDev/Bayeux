// test_ellipsoid.cxx

// Ourselves:
#include <geomtools/ellipsoid.h>

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

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {

    bool interactive = false;
    bool side   = true;
    bool top    = true;
    bool bottom = true;
    bool bulk   = false;
    bool draw   = false;
    bool do_identity = false;
    bool do_cut      = false;
    bool do_locate   = true;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      if (arg == "-D" || arg == "--draw") draw = true;
      if (arg == "-I" || arg == "--identity") do_identity = true;
      if (arg == "-C" || arg == "--cut") do_cut = true;
      if (arg == "-L" || arg == "--no-locate") do_locate = false;
      if (arg == "-s" || arg == "--side") side = false;
      if (arg == "-b" || arg == "--bottom") bottom = false;
      if (arg == "-t" || arg == "--top") top = false;
      if (arg == "-B" || arg == "--bulk") bulk = true;

      iarg++;
    }

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy (true);
    tmp_file.create ("/tmp", "test_ellipsoid_");

    geomtools::ellipsoid my_ellipsoid(1.0, 0.5, 2.0);
    if (do_cut) {
      my_ellipsoid.set_bottom_z_cut(-1.5);
      my_ellipsoid.set_top_z_cut(+0.5);
    }
    std::clog << "Ellipsoid     = " << my_ellipsoid << " " << std::endl;
    my_ellipsoid.tree_dump(std::clog, "My ellipsoid: ");

    std::clog << "Volume   = " << my_ellipsoid.get_volume () / CLHEP::m3 << " m3" << std::endl;

    std::clog << "Side surface = "
              << my_ellipsoid.get_surface (geomtools::ellipsoid::FACE_SIDE) / CLHEP::m2
              << " m2" << std::endl;

    std::clog << "Top surface = "
              << my_ellipsoid.get_surface (geomtools::ellipsoid::FACE_TOP) / CLHEP::m2
              << " m2" << std::endl;

    std::clog << "Bottom surface = " << my_ellipsoid.get_surface (geomtools::ellipsoid::FACE_BOTTOM) / CLHEP::m2
              << " m2" << std::endl;

    std::clog << "Full surface = "
              << my_ellipsoid.get_surface (geomtools::ellipsoid::FACE_ALL) / CLHEP::m2
              << " m2" << std::endl;

    geomtools::placement ellipsoid_placement(0.2, 0.15, 0.1, M_PI / 7.0, M_PI / 6.0, 0.0);
    if (do_identity) {
      ellipsoid_placement.set_identity();
    }

    tmp_file.out() << "# ellipsoid (index 0): " << std::endl;
    geomtools::gnuplot_draw::draw_ellipsoid(tmp_file.out(), ellipsoid_placement, my_ellipsoid);
    tmp_file.out() << std::endl << std::endl;


    {
      // Draw elipsoid :
      tmp_file.out() << "# ellipsoid (index 1):" << std::endl;
      geomtools::wires_type ellipsoid_wires;
      my_ellipsoid.generate_wires(ellipsoid_wires,
                            ellipsoid_placement,
                            geomtools::i_wires_3d_rendering::WR_NONE
                            | geomtools::i_wires_3d_rendering::WR_BASE_GRID
                            | geomtools::i_wires_3d_rendering::WR_BASE_GRID_HIGH_DENSITY
                            | geomtools::i_wires_3d_rendering::WR_BASE_BOUNDINGS
                            // | geomtools::ellipsoid::WR_ELLIPSOID_NO_SIDE_FACE
                            // | geomtools::ellipsoid::WR_ELLIPSOID_NO_BOTTOM_FACE
                            // | geomtools::ellipsoid::WR_ELLIPSOID_NO_TOP_FACE
                            );
      geomtools::gnuplot_draw::draw_wires(tmp_file.out(), ellipsoid_wires);
      tmp_file.out() << std::endl << std::endl;
    }

    {
      int counts = 0;
      tmp_file.out() << "# locate (index 2)" << std::endl;
      size_t nshoots = 500000;
      for (int i = 0; i < (int) nshoots; i++) {
        double dim = 2.;
        geomtools::vector_3d position(dim * ( -1 + 2 * drand48 ()),
                                      dim * ( -1 + 2 * drand48 ()),
                                      dim * ( -1 + 2 * drand48 ()));
        double skin = 0.10;
        geomtools::vector_3d position_c;
        ellipsoid_placement.mother_to_child(position, position_c);
        if (do_locate) {
          if (side && my_ellipsoid.is_on_surface(position_c,
                                                 geomtools::ellipsoid::FACE_SIDE, skin)) {
            geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 3.0);
            counts++;
          } else if (top && my_ellipsoid.is_on_surface(position_c,
                                                       geomtools::ellipsoid::FACE_TOP, skin)) {
            geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 5.0);
            counts++;
          } else if (bottom && my_ellipsoid.is_on_surface(position_c,
                                                          geomtools::ellipsoid::FACE_BOTTOM, skin)) {
            geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 6.0);
            counts++;
          } else if (bulk && my_ellipsoid.is_inside(position_c, skin)) {
            geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 2.0);
            counts++;
          }
        }
      }
      if (counts == 0) {
        tmp_file.out() << "0 0 0 -1" << std::endl;
      }
      tmp_file.out() << std::endl << std::endl;
    }


    {
      std::clog << "Intercept ellipsoid = "
                << " " << std::endl;

      size_t nshoots = 200000;
      int counts = 0;
      tmp_file.out() << "# intercept (index 3)" << std::endl;
      for (int i = 0; i < (int) nshoots; i++) {
        double dim = 4. * CLHEP::mm;
        geomtools::vector_3d pos (dim * drand48(),
                                  dim * (-1 + 2 * drand48()),
                                  dim * (-1 + 2 * drand48()));
        geomtools::vector_3d dir;
        geomtools::randomize_direction (drand48, dir);

        geomtools::vector_3d pos_c;
        ellipsoid_placement.mother_to_child(pos, pos_c);
        geomtools::vector_3d dir_c;
        ellipsoid_placement.mother_to_child_direction(dir, dir_c);

        geomtools::face_intercept_info intercept_c;
        double tolerance = 0.05;
        if (my_ellipsoid.find_intercept(pos_c, dir_c, intercept_c, tolerance)) {
          counts++;
          geomtools::vector_3d impact;
          ellipsoid_placement.child_to_mother(intercept_c.get_impact(), impact);
          geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), impact);
          // if (debug) std::clog << "test 3: Intercept face="
          //                      << intercept_c.get_face()
          //                      << " at impact=" << impact
          //                      << std::endl;
        } else {
          // if (debug) std::clog << "test 3: No intercept." << std::endl;
        }
      }
      if (counts == 0) {
        tmp_file.out() << "0 0 0 -1" << std::endl;
      }
      tmp_file.out() << std::endl << std::endl;
    }

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd ("set title 'Test geomtools::ellipsoid' ");
      g1.cmd ("set grid");
      g1.cmd ("set size ratio -1");
      g1.cmd ("set view equal xyz");
      //g1.cmd ("set xyplane at -10");
      g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 0 title 'Ellipsoid' with lines ";
        g1.cmd (plot_cmd.str ());
        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (2);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 1 title 'Ellipsoid' with lines ";
        plot_cmd << ", '' index 2 using 1:2:3:4 title 'locate' with points pt 6 ps 0.3 linecolor variable ";
        g1.cmd (plot_cmd.str ());
        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (2);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 1 title 'Ellipsoid' with lines ";
        plot_cmd << ", '' index 3 using 1:2:3 title 'intercepts' with points pt 6 ps 0.5";
        g1.cmd (plot_cmd.str ());
        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (2);
      }

#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
    }

  }
  catch (std::exception & x)
    {
      std::cerr << "error: " << x.what () << std::endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      std::cerr << "error: " << "unexpected error!" << std::endl;
      error_code = EXIT_FAILURE;
    }
  return error_code;
}
