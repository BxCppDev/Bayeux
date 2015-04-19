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
#include <geomtools/polycone.h>
#include <geomtools/tube.h>
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
    // bool fr0 = false;
    // bool fr1 = false;
    // bool fr2 = false;
    // bool fr3 = false;

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
        // } else if ((option == "-0")) {
        //   fr0 = true;
        // } else if ((option == "-1")) {
        //   fr1 = true;
        // } else if ((option == "-2")) {
        //   fr2 = true;
        // } else if ((option == "-3")) {
        //   fr3 = true;
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
    tmp_file.create ("/tmp", "test_trt_");

    geomtools::polycone P;
    P.add(0.0, 35);
    P.add(1.0, 35);
    P.add(2.0, 34.5);
    P.add(3.0, 25.);
    P.add(4.0, 18.);
    P.add(5.0, 13.);
    P.add(7.5,  8.5);
    P.add(10.0, 6.5);
    P.add(12.5, 5.);
    P.add(15.0, 4.);
    P.add(20.0, 4.);
    P.add(25.0, 4.);
    P.add(30.0, 4.);
    P.add(35.0, 5.);
    P.add(40.0, 7.5);
    P.add(45.0, 12.);
    P.add(50.0, 18.);
    P.add(55.0, 24.);
    P.add(56.0, 10.0, 25.);
    P.add(57.5, 17.0, 27.);
    P.add(60.0, 22.5, 29.);
    P.add(65.0, 27.0, 31.5);
    P.add(70.0, 29.0, 32.5);
    P.add(75.0, 30.0, 33.0);
    P.add(80.0, 31.0, 33.0);
    P.add(85.0, 31.5, 33.0);
    P.add(90.0, 31.5, 32.75);
    P.add(100.0, 30.9, 31.9);
    P.add(110.0, 30.0, 31.0);
    P.add(120.0, 29, 30.);
    P.add(130.0, 28.15, 29.15);
    P.add(140.0, 27.25, 28.25);
    P.add(150.0, 26.5, 27.5);

    {
      geomtools::gnuplot_draw::draw_polycone(tmp_file.out(), P);
      tmp_file.out() << endl << endl;
      {
        geomtools::tube paille(2.7, 2.8, 150);
        geomtools::vector_3d pos;
        pos.setX(-10.0);
        pos.setZ(140.0);
        geomtools::rotation_3d rot;
        geomtools::create_rotation(rot,
                                   0.0 * CLHEP::degree,
                                   -10.0 * CLHEP::degree,
                                   0.0 * CLHEP::degree);
        geomtools::gnuplot_draw::draw_tube(tmp_file.out(), pos, rot, paille, 18);
        tmp_file.out() << endl << endl;
      }
    }

    {
      genvtx::polycone_vg vg;
      if (debug) {
        vg.set_logging_priority(datatools::logger::PRIO_TRACE);
      }
      vg.set_polycone(P);
      vg.set_angles(M_PI/4, M_PI/2);
      vg.set_active_frustrum(27, true);
      vg.set_active_frustrum(28, true);
      vg.set_mode(genvtx::polycone_vg::MODE_SURFACE);
      int surface_mask = 0;
      surface_mask |= geomtools::polycone::FACE_OUTER_SIDE;
      //surface_mask |= geomtools::polycone::FACE_BOTTOM;
      vg.set_surface_mask(surface_mask);
      vg.set_skin_skip(0.0);
      vg.set_skin_thickness(0.0);
      vg.initialize_simple();
      vg.tree_dump(clog, "Fingerprint : ");
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
      vg.set_angles(M_PI, 1.5*M_PI);
      vg.set_active_frustrum(31, true);
      vg.set_mode(genvtx::polycone_vg::MODE_SURFACE);
      int surface_mask = 0;
      surface_mask |= geomtools::polycone::FACE_OUTER_SIDE;
      surface_mask |= geomtools::polycone::FACE_TOP;
      vg.set_surface_mask(surface_mask);
      vg.set_skin_skip(0.0);
      vg.set_skin_thickness(0.0);
      vg.initialize_simple();
      vg.tree_dump(clog, "Lipstick : ");
      size_t nshoots = 4000;
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
      vg.set_active_frustrum(18, true);
      vg.set_active_frustrum(19, true);
      vg.set_active_frustrum(20, true);
      vg.set_active_frustrum(21, true);
      vg.set_active_frustrum(22, true);
      vg.set_active_frustrum(23, true);
      vg.set_active_frustrum(24, true);
      vg.set_mode(genvtx::polycone_vg::MODE_BULK);
      int bulk_mask = 0;
      bulk_mask |= genvtx::polycone_vg::BULK_CAVITY;
      vg.set_bulk_mask(bulk_mask);
      // vg.set_skin_skip(0.20);
      // vg.set_skin_thickness(0.10);
      vg.set_skin_skip(0.0);
      vg.set_skin_thickness(0.0);
      vg.initialize_simple();
      vg.tree_dump(clog, "Polycone vertex generator 2 : ");
      size_t nshoots = 30000;
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
      vg.set_active_frustrum(26, true);
      vg.set_active_frustrum(25, true);
      vg.set_mode(genvtx::polycone_vg::MODE_BULK);
      int bulk_mask = 0;
      bulk_mask |= genvtx::polycone_vg::BULK_CAVITY;
      vg.set_bulk_mask(bulk_mask);
      vg.set_skin_skip(0.0);
      vg.set_skin_thickness(0.0);
      vg.initialize_simple();
      vg.tree_dump(clog, "Polycone vertex generator 3 : ");
      size_t nshoots = 30000;
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
      //     vg.set_active_frustrum(26, true);
      vg.set_active_frustrum(27, true);
      vg.set_mode(genvtx::polycone_vg::MODE_BULK);
      int bulk_mask = 0;
      bulk_mask |= genvtx::polycone_vg::BULK_CAVITY;
      vg.set_bulk_mask(bulk_mask);
      vg.set_skin_skip(0.0);
      vg.set_skin_thickness(0.0);
      vg.set_active_all_frustrum();
      vg.initialize_simple();
      vg.tree_dump(clog, "Polycone vertex generator 4 : ");
      size_t nshoots = 30000;
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
      vg.set_active_frustrum(0, true);
      vg.set_active_frustrum(1, true);
      vg.set_active_frustrum(2, true);
      vg.set_active_frustrum(3, true);
      vg.set_active_frustrum(4, true);
      vg.set_active_frustrum(5, true);
      vg.set_active_frustrum(6, true);
      vg.set_active_frustrum(7, true);
      vg.set_active_frustrum(8, true);
      vg.set_active_frustrum(9, true);
      vg.set_mode(genvtx::polycone_vg::MODE_BULK);
      int bulk_mask = 0;
      bulk_mask |= genvtx::polycone_vg::BULK_BODY;
      vg.set_bulk_mask(bulk_mask);
      vg.set_skin_skip(0.0);
      vg.set_skin_thickness(0.0);
      vg.initialize_simple();
      vg.tree_dump(clog, "Polycone vertex generator 3 : ");
      size_t nshoots = 10000;
      geomtools::vector_3d vertex;
      for (int i = 0; i < (int) nshoots; i++) {
        vg.shoot_vertex(random, vertex);
         geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), vertex, true);
      }
      tmp_file.out() << endl << endl;
    }

    {
      geomtools::polycone Q;
      Q.add(-3.5, 26.0);
      Q.add(-2.5, 27.0);
      Q.add( 2.5, 27.0);
      Q.add( 3.5, 26.0);
      Q.set_start_angle((2.0-0.45)*M_PI);
      Q.set_delta_angle(1.45*M_PI);
      Q.tree_dump(clog, "Q polycone : ");
      geomtools::vector_3d pos(27.0, 0.0, 160.0);
      geomtools::rotation_3d rot;
      geomtools::create_rotation(rot,
                                 90.0 * CLHEP::degree,
                                 90.0 * CLHEP::degree,
                                 90.0 * CLHEP::degree);
      geomtools::placement pl;
      pl.set_translation(pos);
      pl.set_orientation_zyz(90.0 * CLHEP::degree,
                             90.0 * CLHEP::degree,
                             90.0 * CLHEP::degree);
      {
        geomtools::gnuplot_draw::draw_polycone(tmp_file.out(), pos, rot, Q);
        // -0.45*M_PI, 1.45*M_PI, 26);
        tmp_file.out() << endl << endl;
      }

      {
        genvtx::polycone_vg vg;
        if (debug) {
          vg.set_logging_priority(datatools::logger::PRIO_TRACE);
        }
        vg.set_polycone(Q);
        vg.set_active_frustrum(0, true);
        vg.set_active_frustrum(1, true);
        vg.set_active_frustrum(2, true);
        vg.set_angles(-0.45*M_PI, 1.45*M_PI);
        vg.set_mode(genvtx::polycone_vg::MODE_BULK);
        int bulk_mask = 0;
        bulk_mask |= genvtx::polycone_vg::BULK_BODY;
        //bulk_mask |= genvtx::polycone_vg::BULK_CAVITY;
        vg.set_bulk_mask(bulk_mask);
        vg.set_skin_thickness(0.0);
        vg.initialize_simple();
        vg.tree_dump(clog, "Orange vertex generator: ");
        size_t nshoots = 10000;
        geomtools::vector_3d vertex;
        for (int i = 0; i < (int) nshoots; i++) {
          vg.shoot_vertex(random, vertex);
          geomtools::vector_3d vtx;
          pl.child_to_mother(vertex, vtx);
          geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), vtx, true);
        }
        tmp_file.out() << endl << endl;

        geomtools::polycone Q1;
        Q1.add(-3.5, 23.0);
        Q1.add(-2.5, 24.0);
        Q1.add( 2.5, 24.0);
        Q1.add( 3.5, 23.0);
        Q1.tree_dump(clog, "Q1 polycone : ");
        for (int i = 0; i < 8; i ++) {
          genvtx::polycone_vg vg;
          if (debug) {
            vg.set_logging_priority(datatools::logger::PRIO_TRACE);
          }
          vg.set_polycone(Q1);
          vg.set_active_frustrum(1, true);
          double phi1 = -0.40*M_PI + i * 0.25 * M_PI;
          double phi2 = phi1 + 0.20 * M_PI;
          if (phi2 > 1.40*M_PI) phi2 = 1.40*M_PI;
          vg.set_angles(phi1, phi2);
          vg.set_mode(genvtx::polycone_vg::MODE_BULK);
          int bulk_mask = 0;
          bulk_mask |= genvtx::polycone_vg::BULK_BODY;
          vg.set_bulk_mask(bulk_mask);
          vg.set_skin_thickness(10.0);
          vg.initialize_simple();
          vg.tree_dump(clog, "Orange pulp vertex generator: ");
          size_t nshoots = 200 * (phi2-phi1) / 0.25 * M_PI;
          geomtools::vector_3d vertex;
          for (int i = 0; i < (int) nshoots; i++) {
            vg.shoot_vertex(random, vertex);
            geomtools::vector_3d vtx;
            pl.child_to_mother(vertex, vtx);
            geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), vtx, true);
          }
        }
      }
      tmp_file.out() << endl << endl;
    }

    {
      genvtx::polycone_vg vg;
      if (debug) {
        vg.set_logging_priority(datatools::logger::PRIO_TRACE);
      }
      vg.set_polycone(P);
      vg.set_mode(genvtx::polycone_vg::MODE_SURFACE);
      int surface_mask = 0;
      surface_mask |= geomtools::polycone::FACE_TOP;
      vg.set_surface_mask(surface_mask);
      vg.set_skin_skip(1.0);
      vg.set_skin_thickness(2.0);
      vg.set_active_all_frustrum();
      vg.initialize_simple();
      vg.tree_dump(clog, "Sugar : ");
      size_t nshoots = 5000;
      geomtools::vector_3d vertex;
      for (int i = 0; i < (int) nshoots; i++) {
        vg.shoot_vertex(random, vertex);
        if (vertex.y() > 0. || vertex.x() > 0.) {
          geomtools::gnuplot_draw::basic_draw_point(tmp_file.out(), vertex, true);
        }
      }
      tmp_file.out() << endl << endl;
    }

    if (draw) {
      {
        Gnuplot g1 ("dots");
        g1.cmd ("unset grid");
        g1.cmd ("unset tics");
        g1.cmd ("set title 'Test polycone_vg'");
        g1.cmd ("set size ratio -1");
        g1.cmd ("set view equal xyz");
        //g1.cmd ("set xyplane at 0");
        g1.set_xrange (-150, +150).set_yrange (-150, +150).set_zrange (-50, +250);
        //g1.set_xlabel ("x").set_ylabel ("y").set_zlabel ("z");
        std::ostringstream plot_cmd;
        plot_cmd << "splot ";
        plot_cmd << "'" << tmp_file.get_filename ()
                 << "' index 0 title 'Goblet' with lines lt 5";
        plot_cmd << ", '" << tmp_file.get_filename ()
                 << "' index 1 title 'Straw' with lines lt 4";
        plot_cmd << ", '" << tmp_file.get_filename ()
                 << "' index 2 title 'Fingerprint' with dots lt -1";
        plot_cmd << ", '" << tmp_file.get_filename ()
                 << "' index 3 title 'Lipstick' with dots lt 1";
        plot_cmd << ", '" << tmp_file.get_filename ()
                 << "' index 4 title 'Mint' with dots lt 2";
        plot_cmd << ", '" << tmp_file.get_filename ()
                 << "' index 5 title 'Curacao' with dots lt 3";
        plot_cmd << ", '" << tmp_file.get_filename ()
                 << "' index 6 title 'Orange juice' with dots lt 7";
        plot_cmd << ", '" << tmp_file.get_filename ()
                 << "' index 7 title 'Foot' with dots lt 8";
        plot_cmd << ", '" << tmp_file.get_filename ()
                 << "' index 8 title 'Orange slice' with lines lt 7";
        plot_cmd << ", '" << tmp_file.get_filename ()
                 << "' index 9 title 'Orange pulp' with dots lt 8";
        plot_cmd << ", '" << tmp_file.get_filename ()
                 << "' index 10 title 'Orange pulp' with dots lt 1";
        plot_cmd << ", '" << tmp_file.get_filename ()
                 << "' index 11 title 'Sugar' with dots lt 3";
        g1.cmd (plot_cmd.str ());

        g1.showonscreen (); // window output
        geomtools::gnuplot_drawer::wait_for_key ();
        usleep (200);
      }
      {
        Gnuplot g1 ("dots");
        g1.cmd ("unset grid");
        g1.cmd ("set title 'Test polycone_vg'");
        g1.cmd ("set size ratio -1");
        g1.set_xrange(-150, +150).set_yrange(-50, +250);
        g1.set_xlabel("x").set_ylabel ("z");
        std::ostringstream plot_cmd;
        plot_cmd << "plot ";
        plot_cmd << "'" << tmp_file.get_filename ()
                 << "' index 0 u 1:3 title 'Goblet' with lines lt 5";
        plot_cmd << ", '" << tmp_file.get_filename ()
                 << "' index 1 u 1:3 title 'Straw' with lines lt 4";
        plot_cmd << ", '" << tmp_file.get_filename ()
                 << "' index 2 u 1:3 title 'Fingerprint' with dots lt -1";
        plot_cmd << ", '" << tmp_file.get_filename ()
                 << "' index 3 u 1:3 title 'Lipstick' with dots lt 1";
        plot_cmd << ", '" << tmp_file.get_filename ()
                 << "' index 4 u 1:3 title 'Mint' with dots lt 2";
        plot_cmd << ", '" << tmp_file.get_filename ()
                 << "' index 5 u 1:3 title 'Curacao' with dots lt 3";
        plot_cmd << ", '" << tmp_file.get_filename ()
                 << "' index 6 u 1:3 title 'Orange juice' with dots lt 7";
        plot_cmd << ", '" << tmp_file.get_filename ()
                 << "' index 7 u 1:3 title 'Foot' with dots lt 8";
        plot_cmd << ", '" << tmp_file.get_filename ()
                 << "' index 8 u 1:3 title 'Orange slice' with lines lt 7";
        plot_cmd << ", '" << tmp_file.get_filename ()
                 << "' index 9 u 1:3 title 'Orange pulp' with dots lt 8";
        plot_cmd << ", '" << tmp_file.get_filename ()
                 << "' index 10 u 1:3 title 'Orange pulp' with dots lt 1";
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

// end of test_polycone_vg.cxx
