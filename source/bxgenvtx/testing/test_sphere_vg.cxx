// -*- mode: c++ ; -*-
// test_sphere_vg.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <geomtools/gnuplot_draw.h>
#include <genvtx/sphere_vg.h>
#include <mygsl/rng.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try {
    clog << "Test program for class 'sphere_vg'!" << endl;

    bool debug = false;

    int iarg = 1;
    while (iarg < argc_) {
      string token = argv_[iarg];

      if (token[0] == '-') {
        string option = token;
        if ((option == "-d") || (option == "--debug")) {
          debug = true;
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
    mygsl::rng random(rng_id, rng_seed);

    size_t nshoots = 50000;

    geomtools::sphere ball(2.0);

    {
      geomtools::vector_3d pos;
      geomtools::rotation_3d rot;
      geomtools::gnuplot_draw::draw_sphere(cout, pos, rot, ball);
      cout << endl << endl;
    }

    genvtx::sphere_vg vg;
    vg.set_debug (debug);

    bool surface = true;
    if (surface) {
      vg.set_sphere(ball);
      vg.set_mode(genvtx::sphere_vg::MODE_SURFACE);
      clog << "warning: mode = " << vg.get_mode() << "\n";
      vg.set_skin_skip(0.40);
      vg.set_skin_thickness(0.10);
      vg.initialize_simple();
      vg.tree_dump(clog, "Sphere vertex generator (side, skipped)");
      geomtools::vector_3d vertex;
      for (int i = 0; i < nshoots; i++) {
        vg.shoot_vertex(random, vertex);
        if (vertex.x() < 0.0 && vertex.y() < 0.0) {
          geomtools::gnuplot_draw::basic_draw_point (cout, vertex, true);
        }
      }
      cout << endl << endl;
    }

    vg.reset ();

    bool bulk = true;
    //bulk = false;
    if (bulk) {
      vg.set_sphere(ball);
      vg.set_mode(genvtx::sphere_vg::MODE_BULK);
      clog << "warning: mode = " << vg.get_mode() << "\n";
      vg.set_skin_skip(0.0);
      vg.set_skin_thickness(0.20);
      vg.initialize_simple();
      vg.tree_dump(clog, "Sphere vertex generator (bulk)");
      //nshoots = 50;
      geomtools::vector_3d vertex;
      for (int i = 0; i < nshoots; i++) {
        vg.shoot_vertex(random, vertex);
        if (vertex.y() > 0.0) { // && vertex.x() > 0.0) {
          geomtools::gnuplot_draw::basic_draw_point(cout, vertex, true);
        }
      }
      cout << endl << endl;
    }

    {
      geomtools::vector_3d vertex;
      for (int i = 0; i < nshoots/10; i++) {
        genvtx::randomize_sphere(random,
                                 0.6, 1.4,
                                 M_PI/5, M_PI/3,
                                 -M_PI/2, -M_PI,
                                 vertex);
        // genvtx::randomize_sphere(random,
        //                          1.2, 1.25,
        //                          0.0, 0.5*M_PI,
        //                          0.0, M_PI,
        //                          vertex);
        geomtools::gnuplot_draw::basic_draw_point(cout, vertex, true);
      }
      cout << endl << endl;
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
  return (error_code);
}

// end of test_sphere_vg.cxx
