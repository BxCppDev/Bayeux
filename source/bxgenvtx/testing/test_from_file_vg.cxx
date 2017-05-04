// -*- mode: c++ ; -*-
// test_from_file_vg.cxx

// Ourselves:
#include <genvtx/from_file_vg.h>

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Bayeux:
#include <mygsl/rng.h>
#include <geomtools/gnuplot_draw.h>

int main (int argc_, char ** argv_)
{

using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    clog << "Test program for class 'genvtx::from_file_vg'!" << endl;

    // bool debug = false;
    std::string input_vertex_file;
    int iarg = 1;
    while (iarg < argc_) {
      string token = argv_[iarg];

      if (token[0] == '-') {
        // string option = token;
        // if ((option == "-d") || (option == "--debug")) {
        //   debug = true;
        // } else {
        //   clog << "warning: ignoring option '" << option << "'!" << endl;
        // }
      } else {
        string argument = token;
        if (input_vertex_file.empty()) {
          input_vertex_file = argument;
        } else {
          clog << "warning: ignoring argument '" << argument << "'!" << endl;
        }
      }
      iarg++;
    }

    string            rng_id   = "mt19937";
    unsigned long int rng_seed = 12345;
    mygsl::rng        random (rng_id, rng_seed);

    genvtx::from_file_vg vg;
    if (input_vertex_file.empty()) {
      input_vertex_file = "${GENVTX_TESTING_DIR}/data/test_cylinder_vertices.data";
    }
    vg.set_filename (input_vertex_file);
    // vg.set_length_unit(CLHEP::mm);
    vg.initialize_simple ();
    vg.tree_dump (clog, "From file vertex generator");

    geomtools::vector_3d vertex;
    double time;

    while (true) {
      if (vg.is_time_generator()) {
        vg.shoot_vertex_and_time(random, vertex, time);
      } else {
        vg.shoot_vertex(random, vertex);
      }
      if (! geomtools::is_valid(vertex)) {
        clog << "End of vertex source file." << endl;
        break;
      }
      geomtools::gnuplot_draw::basic_draw_point(std::cout, vertex, false);
      if (vg.is_time_generator()) {
        std::cout << ' ' << time;
      }
      std::cout << std::endl;
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
