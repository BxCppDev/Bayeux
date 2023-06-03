// test_polycone_vg.cxx

// Ourselves:
#include <genvtx/polycone_vg.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/temporary_files.h>
// - Bayeux/geomtools:
#include <geomtools/geomtools_config.h>
#include <geomtools/gnuplot_draw.h>
// - Bayeux/mygsl:
#include <mygsl/rng.h>

#if GEOMTOOLS_WITH_GNUPLOT_DISPLAY == 1
#include <geomtools/gnuplot_i.h>
#include <geomtools/gnuplot_drawer.h>
#endif // GEOMTOOLS_WITH_GNUPLOT_DISPLAY

int main (int argc_, char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    clog << "Test program for class 'polycone_vg'!" << endl;
    bool debug = false;
    // bool bulk = false;
    bool draw = false;
    bool fr0 = false;
    bool fr1 = false;
    bool fr2 = false;
    bool fr3 = false;

    int iarg = 1;
    while (iarg < argc_) {
      string token = argv_[iarg];
      if (token[0] == '-') {
        string option = token;
        if ((option == "-d") || (option == "--debug")) {
          debug = true;
        // } else if ((option == "-b") || (option == "--bulk")) {
        //   bulk = true;
        } else if ((option == "-D") || (option == "--draw")) {
          draw = true;
        } else if ((option == "-0")) {
          fr0 = true;
        } else if ((option == "-1")) {
          fr1 = true;
        } else if ((option == "-2")) {
          fr2 = true;
        } else if ((option == "-3")) {
          fr3 = true;
        } else {
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

    std::string       rng_id = "mt19937";
    unsigned long int rng_seed = 12345;
    mygsl::rng random (rng_id, rng_seed);

    datatools::temp_file tmp_file;
    tmp_file.set_remove_at_destroy (true);
    tmp_file.create ("/tmp", "test_polycone_vg_");

    geomtools::polycone P;
    P.add(0.0, 1.0, 1.5);
    P.add(2.0, 1.0, 1.5);
    P.add(3.0, 0.5, 1.0);
    P.add(5.0, 0.5, 1.0);
    P.add(6.0, 0.0, 0.0);

    {
      geomtools::gnuplot_draw::draw_polycone(tmp_file.out(), P);
      tmp_file.out() << endl << endl;
    }


    {
      genvtx::polycone_vg vg;
      if (debug) {
        vg.set_logging_priority(datatools::logger::PRIO_TRACE);
      }
      vg.set_polycone(P);
      vg.set_active_frustrum(0, fr0);
      vg.set_active_frustrum(1, fr1);
      vg.set_active_frustrum(2, fr2);
      vg.set_active_frustrum(3, fr3);
      vg.set_mode(genvtx::polycone_vg::MODE_SURFACE);
      int surface_mask = 0;
      surface_mask |= geomtools::polycone::FACE_INNER_SIDE;
      //surface_mask |= geomtools::polycone::FACE_OUTER_SIDE;
      surface_mask |= geomtools::polycone::FACE_BOTTOM;
      //surface_mask |= geomtools::polycone::FACE_TOP;
      // std::cerr << "*** DEVEL *** Surface mask : " << surface_mask << std::endl;
      vg.set_surface_mask(surface_mask);
      vg.set_skin_skip(0.0);
      vg.set_skin_thickness(0.0);
      vg.set_active_all_frustrum();
      vg.initialize_simple();
      // std::cerr << "*** DEVEL *** initialize_simple done." << std::endl;
      vg.tree_dump(clog, "Polycone vertex generator 1 : ");
      size_t nshoots = 5000;
      geomtools::vector_3d vertex;
      for (int i = 0; i < (int) nshoots; i++) {
        vg.shoot_vertex(random, vertex);
        geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), vertex, true);
      }
      tmp_file.out() << endl << endl;
    }

    {
      genvtx::polycone_vg vg;
      if (debug) {
        vg.set_logging_priority(datatools::logger::PRIO_TRACE);
      }
      vg.set_polycone(P);
      vg.set_active_frustrum(0, false);
      vg.set_active_frustrum(1, true);
      vg.set_active_frustrum(2, false);
      vg.set_active_frustrum(3, true);
      vg.set_mode(genvtx::polycone_vg::MODE_BULK);
      int bulk_mask = 0;
      bulk_mask |= genvtx::polycone_vg::BULK_BODY;
      // std::cerr << "*** DEVEL *** Bulk mask : " << bulk_mask << std::endl;
      vg.set_bulk_mask(bulk_mask);
      // vg.set_skin_skip(0.20);
      // vg.set_skin_thickness(0.10);
      vg.set_skin_skip(0.0);
      vg.set_skin_thickness(0.0);
      vg.initialize_simple();
      vg.tree_dump(clog, "Polycone vertex generator 2 : ");
      size_t nshoots = 10000;
      geomtools::vector_3d vertex;
      for (int i = 0; i < (int) nshoots; i++) {
        vg.shoot_vertex(random, vertex);
        vertex += geomtools::vector_3d(3.5,0.0,0.0);
        geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), vertex, true);
      }
      tmp_file.out() << endl << endl;
    }

    {
      genvtx::polycone_vg vg;
      if (debug) {
        vg.set_logging_priority(datatools::logger::PRIO_TRACE);
      }
      vg.set_polycone(P);
      vg.set_active_frustrum(0, true);
      vg.set_active_frustrum(1, false);
      vg.set_active_frustrum(2, false);
      vg.set_active_frustrum(3, true);
      vg.set_mode(genvtx::polycone_vg::MODE_BULK);
      int bulk_mask = 0;
      bulk_mask |= genvtx::polycone_vg::BULK_CAVITY;
      // std::cerr << "*** DEVEL *** Bulk mask : " << bulk_mask << std::endl;
      vg.set_bulk_mask(bulk_mask);
      // vg.set_skin_skip(0.20);
      // vg.set_skin_thickness(0.10);
      vg.set_skin_skip(0.0);
      vg.set_skin_thickness(0.0);
      vg.initialize_simple();
      vg.tree_dump(clog, "Polycone vertex generator 3 : ");
      size_t nshoots = 10000;
      geomtools::vector_3d vertex;
      for (int i = 0; i < (int) nshoots; i++) {
        vg.shoot_vertex(random, vertex);
        vertex += geomtools::vector_3d(0.0, 3.5, 0.0);
        geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), vertex, true);
      }
      tmp_file.out() << endl << endl;
    }

    if (draw) {
      Gnuplot g1 ("dots");
      {
        g1.cmd ("set grid");
        g1.cmd ("set title 'Test polycone_vg'");
        g1.cmd ("set size ratio -1");
        g1.cmd ("set view equal xyz");
        g1.cmd ("set xyplane at -1");
        g1.set_xrange (-5, +5).set_yrange (-5, +5).set_zrange (-1, +8);
        g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
        std::ostringstream plot_cmd;
        plot_cmd << "splot ";
        plot_cmd << "'" << tmp_file.get_filename ()
                 << "' index 0 title 'Polycone' with lines lt 4";
        plot_cmd << ", '" << tmp_file.get_filename ()
                 << "' index 1 title 'Surface' with dots lt 1";
        plot_cmd << ", '" << tmp_file.get_filename ()
                 << "' index 2 title 'Bulk body (X-shifted)' with dots lt 3";
        plot_cmd << ", '" << tmp_file.get_filename ()
                 << "' index 3 title 'Bulk cavity (Y-shifted)' with dots lt 2";
        g1.cmd (plot_cmd.str ());

        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (200);

      }
    }

    clog << "The end." << endl;

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
