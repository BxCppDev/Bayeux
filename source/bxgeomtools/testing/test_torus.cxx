// test_torus.cxx

// Ourselves:
#include <geomtools/torus.h>

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
    // bool interactive = false;
    bool debug = false;
    bool outer = true;
    bool inner = true;
    bool bulk = false;
    bool do_identity = false;
    bool do_locate = true;
    bool do_sector = false;
    bool do_hole = false;
    bool draw = false;

    int iarg = 1;
    while (iarg < argc_) {
      std::string arg = argv_[iarg];

      // if (arg == "-r" || arg == "--interactive") interactive = true;
      if (arg == "-I" || arg == "--identity") do_identity = true;
      if (arg == "-D" || arg == "--draw") draw = true;
      if (arg == "-B" || arg == "--bulk") bulk = true;
      if (arg == "-S" || arg == "--sector") do_sector = true;
      if (arg == "-H" || arg == "--hole") do_hole = true;
      if (arg == "-l" || arg == "--locate") do_locate = false;
      if (arg == "-i" || arg == "--no-inner") inner = false;
      if (arg == "-o" || arg == "--no-outer") outer = false;

      iarg++;
    }

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy (true);
    tmp_file.create ("/tmp", "test_torus_");

    geomtools::torus my_torus;
    my_torus.set(6.5 * CLHEP::mm,
		 3.2 * CLHEP::mm);
    if (do_hole) {
      my_torus.set_inside_radius(1.3 * CLHEP::mm);
    }
    if (do_sector) {
      my_torus.set_phi(M_PI / 6, 0.75 * M_PI);
    }

    std::clog << "Torus  = " << my_torus << " " << std::endl;

    std::clog << "Volume = " << my_torus.get_volume () / CLHEP::m3 << " m3" << std::endl;

    std::clog << "Outer side surface = "
              << my_torus.get_surface (geomtools::torus::FACE_OUTSIDE) / CLHEP::mm2
              << " mm2" << std::endl;
    std::clog << "Inner side surface = "
              << my_torus.get_surface (geomtools::torus::FACE_INSIDE) / CLHEP::mm2
              << " mm2" << std::endl;

    std::clog << "phi surface        = "
              << my_torus.get_surface (geomtools::torus::FACE_START_PHI) / CLHEP::mm2
              << " mm2" << std::endl;

    std::clog << "Full surface       = "
              << my_torus.get_surface (geomtools::torus::FACE_ALL) / CLHEP::mm2
              << " mm2" << std::endl;
    my_torus.tree_dump(std::clog, "Torus: ", "INFO: ");

    geomtools::placement torus_placement(0.2, 0.15, 0.1, M_PI / 7.0, M_PI / 6.0, 0.0);
    if (do_identity) {
      torus_placement.set_identity();
    }

    {
      tmp_file.out() << "# torus (index 0): " << std::endl;
      geomtools::gnuplot_draw::draw_torus(tmp_file.out(), torus_placement, my_torus);
      tmp_file.out() << std::endl << std::endl;
    }

    {
      int counts = 0;
      tmp_file.out() << "# locate (index 1)" << std::endl;
      size_t nshoots = 50000;
      for (int i = 0; i < (int) nshoots; i++) {
        double dim = 5. * CLHEP::mm;
        geomtools::vector_3d position (dim * ( -1 + 2 * drand48 ()),
                                       dim * ( -1 + 2 * drand48 ()),
                                       dim * ( -1 + 2 * drand48 ()));
        double skin = 0.20 * CLHEP::mm;
        geomtools::vector_3d position_c;
        torus_placement.mother_to_child(position, position_c);
        if (do_locate) {
          if (outer && my_torus.is_on_surface(position_c,
                                             geomtools::torus::FACE_OUTSIDE, skin)) {
            geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 3.0);
            counts++;
          } else if (inner && my_torus.is_on_surface(position_c,
                                                    geomtools::torus::FACE_INSIDE, skin)) {
            geomtools::gnuplot_draw::basic_draw_point_with_color(tmp_file.out(), position, 4.0);
            counts++;
          } else if (bulk && my_torus.check_inside(position_c, skin)) {
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
      // Draw torus :
      tmp_file.out() << "# torus (index 2):" << std::endl;
      geomtools::wires_type torus_wires;
      my_torus.generate_wires(torus_wires,
                             torus_placement,
                             geomtools::i_wires_3d_rendering::WR_NONE
                             | geomtools::i_wires_3d_rendering::WR_BASE_GRID
                             | geomtools::i_wires_3d_rendering::WR_BASE_GRID_HIGH_DENSITY
                             | geomtools::i_wires_3d_rendering::WR_BASE_BOUNDINGS
                             );
      geomtools::gnuplot_draw::draw_wires(tmp_file.out(), torus_wires);
      tmp_file.out() << std::endl << std::endl;
    }

    {
      std::clog << "test 3: Intercept..." << std::endl;

      size_t nshoots = 10000;
      int counts = 0;
      tmp_file.out() << "# intercept (index 3)" << std::endl;
      for (int i = 0; i < (int) nshoots; i++) {
        double dim = 4. * CLHEP::mm;
        geomtools::vector_3d pos (dim * drand48(),
                                  dim * drand48(),
                                  dim * drand48());
        geomtools::vector_3d dir;
        geomtools::randomize_direction (drand48, dir);

        geomtools::vector_3d pos_c;
        torus_placement.mother_to_child(pos, pos_c);
        geomtools::vector_3d dir_c;
        torus_placement.mother_to_child_direction(dir, dir_c);

        geomtools::face_intercept_info intercept_c;

        if (my_torus.find_intercept (pos_c, dir_c, intercept_c)) {
          counts++;
          geomtools::vector_3d impact;
          torus_placement.child_to_mother(intercept_c.get_impact(), impact);
          geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), impact);
          if (debug) std::clog << "test 3: Intercept face="
                               << intercept_c.get_face_id()
                               << " at impact=" << impact
                               << std::endl;
        } else {
          if (debug) std::clog << "test 3: No intercept." << std::endl;
        }
      }
      if (counts == 0) {
        tmp_file.out() << "0 0 0 -1" << std::endl;
      }
      tmp_file.out() << std::endl << std::endl;
    }

    // if (interactive) {
    //   std::clog << "Enter a torus (example: '{torus 900 100 10 0 90}'): ";
    //   std::cin >> std::ws >> my_torus;
    //   if (std::cin) {
    //     std::clog << "Torus = " << my_torus << " " << std::endl;
    //   } else {
    //     throw std::runtime_error ("Invalid input for torus!");
    //   }
    // }

    if (draw) {
#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
      Gnuplot g1;
      g1.cmd ("set title 'Test geomtools::torus' ");
      g1.cmd ("set grid");
      g1.cmd ("set size ratio -1");
      g1.cmd ("set view equal xyz");
      g1.cmd("set xrange [-10:+10]");
      g1.cmd("set yrange [-10:+10]");
      g1.cmd("set zrange [-8:+8]");
      g1.cmd("set xyplane at -8");
      // g1.cmd ("set xyplane at -10");
      g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 0 title 'Torus' with lines ";
        plot_cmd << ", '' index 1 using 1:2:3:4 title 'Locate' with points pt 6 ps 0.3 linecolor variable ";
        g1.cmd (plot_cmd.str ());
        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 2 title 'Torus (grid)' with lines ";
        g1.cmd (plot_cmd.str ());
        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (200);
      }

      {
        std::ostringstream plot_cmd;
        plot_cmd << "splot '" << tmp_file.get_filename () << "' index 0 title 'Torus' with lines ";
        plot_cmd << " , '' index 3 title 'Intercepts' with dots ";
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
