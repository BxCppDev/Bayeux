// -*- mode: c++; -*-
// test_primary_event.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <genbb_help/primary_event.h>

int main (int argc_, char ** argv_)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    clog << "Hello, this is a sample program for class 'dummy_genbb_help'!" << endl;

    bool debug = false;

    int iarg = 1;
    while (iarg < argc_) {
      string arg = argv_[iarg];

      if (arg == "-d" || arg == "--debug") debug = true;

      iarg++;
    }

    genbb::primary_event my_event;
    my_event.set_time (15 * CLHEP::ns);

    genbb::primary_particle p1;
    p1.set_time (0.0 * CLHEP::ns);
    p1.set_type (genbb::primary_particle::ELECTRON);
    geomtools::vector_3d m1 (3.0 * CLHEP::MeV, 0.0, 0.0);
    p1.set_momentum (m1);
    my_event.add_particle (p1);

    genbb::primary_particle p2;
    p2.set_time (10.0 * CLHEP::ns);
    p2.set_type (genbb::primary_particle::GAMMA);
    geomtools::vector_3d m2 (0.0, 2.0 * CLHEP::MeV, 0.0);
    p2.set_momentum (m2);
    my_event.add_particle (p2);

    genbb::primary_particle p3;
    p3.set_time (100.0 * CLHEP::ns);
    p3.set_type (genbb::primary_particle::ALPHA);
    geomtools::vector_3d m3 (0.0, 0.0, 1.0 * CLHEP::MeV);
    p3.set_momentum (m3);
    my_event.add_particle (p3);

    my_event.compute_classification();
    my_event.dump (clog);

    int count = 0;
    for (genbb::primary_event::particles_col_t::const_iterator
           it = my_event.get_particles ().begin ();
         it !=my_event.get_particles ().end ();
         it++) {
      const genbb::primary_particle & part = *it;
      geomtools::vector_3d p = part.get_momentum ();
      cout << "0 0 0" << endl;
      if (count % 2) cout << "0 0 0" << endl;
      cout << p.x ( ) << ' ' << p.y () << " " << p.z () << endl;
      cout << endl;
      count++;
    }
    cout << endl;

    double phi   = 20.0 * CLHEP::degree;
    double theta = 30.0 * CLHEP::degree;
    double psi   = 65.0 * CLHEP::degree;
    // theta = 0.0 * CLHEP::degree;
    // psi   = 0.0 * CLHEP::degree;
    my_event.rotate (phi, theta, psi);

    count = 0;
    for (genbb::primary_event::particles_col_t::const_iterator
           it = my_event.get_particles ().begin ();
         it !=my_event.get_particles ().end ();
         it++) {
      const genbb::primary_particle & part = *it;
      geomtools::vector_3d p = part.get_momentum ();
      cout << "0 0 0" << endl;
      if (count % 2) cout << "0 0 0" << endl;
      cout << p.x ( ) << ' ' << p.y () << " " << p.z () << endl;
      cout << endl;
      count++;
    }
    cout << endl;
  }
  catch (exception & x) {
    cerr << "error: " << x.what () << endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    cerr << "error: " << "unexpected error !" << endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

// end of test_primary_event.cxx
