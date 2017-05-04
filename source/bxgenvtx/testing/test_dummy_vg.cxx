// -*- mode: c++ ; -*-
// test_dummy_vg.cxx

// Standard library:
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Bayeux:
#include <mygsl/rng.h>
#include <geomtools/gnuplot_draw.h>

// This project:
#include <genvtx/i_vertex_generator.h>

#include "dummy_vg.h"

int main (int /* argc_ */, char ** /* argv_ */)
{
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Test program for class 'genvtx::testing::dummy_vg' vertex/time generator!" << std::endl;

    std::string       rng_id   = "mt19937";
    unsigned long int rng_seed = 12345;
    mygsl::rng        random(rng_id, rng_seed);

    genvtx::testing::dummy_vg vg;
    vg.set_name("dummy");
    vg.set_length(20.0 * CLHEP::mm);
    vg.set_tau(1.5 * CLHEP::second);
    vg.initialize_simple();
    vg.tree_dump(std::clog, "Dummy vertex/time generator");

    geomtools::vector_3d vertex;
    double time;

    std::cout << "#@length_unit=mm" << std::endl;
    std::cout << "#@time_unit=second" << std::endl;
    int counter = 0;
    int max_counts = 100;
    while (counter < max_counts) {
      vg.shoot_vertex_and_time(random, vertex, time);
      geomtools::gnuplot_draw::basic_draw_point(std::cout, vertex / CLHEP::mm, false);
      std::cout << ' ' << time / CLHEP::second << std::endl;
      counter++;
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
  return (error_code);
}
