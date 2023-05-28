// -*- mode: c++ ; -*-
// test_reusing_vg.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <geomtools/gnuplot_draw.h>
#include <genvtx/box_vg.h>
#include <mygsl/rng.h>
#include <genvtx/reusing_vg.h>
#include <datatools/properties.h>

int main (int argc_, char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    clog << "Test program for class 'reusing_vg'!" << endl;

    // bool debug = false;

    int iarg = 1;
    while (iarg < argc_) {
      string token = argv_[iarg];
      if (token[0] == '-') {
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

    // Define a box and its placement:
    geomtools::box b(4., 2., 3.);
    geomtools::placement pl(0., 0., 0.,
			    0. * CLHEP::degree,
			    0. * CLHEP::degree,
			    0. * CLHEP::degree);
    {
      // Draw it:
      geomtools::gnuplot_draw::draw_box(cout,
					pl.get_translation(),
					pl.get_rotation(),
					b);
      cout << endl << endl;
    }

    // Define a box vertex generator (surface mode) :
    genvtx::box_vg vg;
    vg.set_debug(true);
    vg.set_box(b);
    vg.set_mode(genvtx::box_vg::MODE_SURFACE);
    int surface_mask = 0;
    // surface_mask |= geomtools::box::FACE_FRONT;
    // surface_mask |= geomtools::box::FACE_RIGHT;
    surface_mask |= geomtools::box::FACE_TOP;
    vg.set_surface_mask(surface_mask);
    vg.set_skin_skip(0.10);
    vg.set_skin_thickness(0.20);
    vg.initialize_simple();
    vg.tree_dump(clog, "Box vertex generator");

    // Define a reusing_vg (using the box_vg above) :
    genvtx::reusing_vg pvg;
    pvg.set_vg(vg);
    pvg.set_reusing_count(3);

    // Shoot vertices:
    size_t nshoots = 9;
    geomtools::vector_3d vertex;
    for (int i = 0; i < (int) nshoots; i++) {
      pvg.shoot_vertex(random, vertex);
      geomtools::gnuplot_draw::basic_draw_point(cout, vertex, true);
    }
    cout << endl << endl;
    clog << "Base VG's counter : " << vg.get_counter() << '\n';
    clog << "Reusing VG's counter : " << pvg.get_counter() << '\n';
  } catch (exception & x) {
    cerr << "error: " << x.what() << endl;
    error_code = EXIT_FAILURE;
  } catch (...) {
    cerr << "error: " << "unexpected error!" << endl;
    error_code = EXIT_FAILURE;
  }
  return (error_code);
}
