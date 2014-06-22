// test_primary_event.cxx

// Standard library
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

// Third party:
// - Bayeux/datatools:
#include <datatools/io_factory.h>

// Ourselves:
#include <genbb_help/primary_event.h>
#include <genbb_help/primary_event.ipp> // Mandatory for BOOST_CLASS_VERSION

int main (int /* argc_ */, char ** /* argv_ */)
{
  using namespace std;
  int error_code = EXIT_SUCCESS;
  try {
    std::clog << "Hello, this is a sample program for class 'genbb::primary_event'!" << std::endl;

    /*
    bool debug = false;

    int iarg = 1;
    while (iarg < argc_) {
      string arg = argv_[iarg];

      if (arg == "-d" || arg == "--debug") debug = true;

      iarg++;
    }
    */

    genbb::primary_event my_event;
    my_event.grab_auxiliaries().store("generator", "genbb::toy");
    my_event.grab_auxiliaries().store("author", "The SuperNEMO collaboration");
    my_event.grab_auxiliaries().store_flag("test");

    my_event.set_time(15 * CLHEP::ns);

    genbb::primary_particle p1;
    p1.set_time(0.0 * CLHEP::ns);
    p1.set_type(genbb::primary_particle::ELECTRON);
    geomtools::vector_3d m1(3.0 * CLHEP::MeV, 0.0, 0.0);
    p1.set_momentum(m1);
    p1.grab_auxiliaries().store("creator", "beta_decay");
    p1.grab_auxiliaries().store("description", "The first electron");
    p1.grab_auxiliaries().store("visualization.color", "blue");
    my_event.add_particle(p1);

    genbb::primary_particle p2;
    p2.set_time(10.0 * CLHEP::ns);
    p2.set_type(genbb::primary_particle::GAMMA);
    geomtools::vector_3d m2(0.0, 2.0 * CLHEP::MeV, 0.0);
    p2.set_momentum(m2);
    my_event.add_particle(p2);

    genbb::primary_particle p3;
    p3.set_time(100.0 * CLHEP::ns);
    p3.set_type(genbb::primary_particle::ALPHA);
    geomtools::vector_3d m3(0.0, 0.0, 1.0 * CLHEP::MeV);
    p3.set_momentum(m3);
    my_event.add_particle(p3);

    // Ion '6He+'
    genbb::primary_particle p4;
    p4.set_time(0.0 * CLHEP::ns);
    p4.set_ion(2, 6, 0.0 * CLHEP::keV, +1);
    {
      int Z,A,Q;
      double Estar;
      if (genbb::primary_particle::label_to_ion(p4.get_particle_label(),
                                                Z,
                                                A,
                                                Estar,
                                                Q)) {
        std::clog << "Parsed an ion with "
                  << "Z=" << Z
                  << ", A=" << A
                  << ", E*=" << Estar / CLHEP::keV << " keV"
                  << ", Q=" << Q << " e"
                  << std::endl;
      } else {
        std::cerr << "Cannot parsed ion data!" << std::endl;
      }
    }
    geomtools::vector_3d m4(0.0, 0.0, 250.0 * CLHEP::keV);
    p4.set_momentum(m4);
    p4.grab_auxiliaries().store("half_life", 801 * CLHEP::ms);
    p4.grab_auxiliaries().store("J", 0);
    p4.grab_auxiliaries().store("parity", +1);
    p4.grab_auxiliaries().store("Delta", 17.5928 * CLHEP::MeV);
    my_event.add_particle(p4);

    // Nucleus '14C'
    genbb::primary_particle p5;
    p5.set_time(0.0 * CLHEP::ns);
    p5.set_nucleus(6, 14, 0.0 * CLHEP::keV);
    {
      int Z,A;
      double Estar;
      if (genbb::primary_particle::label_to_nucleus(p5.get_particle_label(),
                                                    Z,
                                                    A,
                                                    Estar)) {
        std::clog << "Parsed an ion with "
                  << "Z=" << Z
                  << ", A=" << A
                  << ", E*=" << Estar / CLHEP::keV << " keV"
                  << std::endl;
      } else {
        std::cerr << "Cannot parsed nucleus data!" << std::endl;
      }
    }
    geomtools::vector_3d m5(0.0, 0.0, 400.0 * CLHEP::keV);
    p5.set_momentum(m5);
    my_event.add_particle(p5);

    my_event.compute_classification();
    my_event.tree_dump(std::clog);

    {
      datatools::properties p;
      datatools::data_writer writer("test_primary_event.xml",
                                    true
                                    /* false *//* datatools::using_multi_archives */);
      writer.store(my_event);
      //writer.store(p);
    }

    {
      datatools::properties p;
      genbb::primary_event my_event2;
      datatools::data_reader reader("test_primary_event.xml",
                                    true
                                    /* false *//*datatools::using_multi_archives */);
      if (reader.has_record_tag()) {
        if (reader.record_tag_is(genbb::primary_event::SERIAL_TAG)) {
          reader.load(my_event2);
          my_event2.tree_dump(std::clog, "Loaded primary event: ", "CHECK: ");
        } else if (reader.record_tag_is(datatools::properties::SERIAL_TAG)) {
          reader.load(p);
          p.tree_dump(std::clog, "Loaded p: ", "CHECK: ");
        }
      }
    }

    int count = 0;
    for (genbb::primary_event::particles_col_t::const_iterator
           it = my_event.get_particles().begin();
         it != my_event.get_particles().end();
         it++) {
      const genbb::primary_particle & part = *it;
      geomtools::vector_3d p = part.get_momentum();
      std::clog << "0 0 0" << std::endl;
      if (count % 2) std::clog << "0 0 0" << std::endl;
      std::clog << p.x( ) << ' ' << p.y() << " " << p.z() << std::endl;
      std::clog << std::endl;
      count++;
    }
    std::clog << std::endl;

    double phi   = 20.0 * CLHEP::degree;
    double theta = 30.0 * CLHEP::degree;
    double psi   = 65.0 * CLHEP::degree;
    my_event.rotate(phi, theta, psi);

    count = 0;
    for (genbb::primary_event::particles_col_t::const_iterator
           it = my_event.get_particles().begin();
         it !=my_event.get_particles().end();
         it++) {
      const genbb::primary_particle & part = *it;
      geomtools::vector_3d p = part.get_momentum();
      std::clog << "0 0 0" << std::endl;
      if (count % 2) std::clog << "0 0 0" << std::endl;
      std::clog << p.x( ) << ' ' << p.y() << " " << p.z() << std::endl;
      std::clog << std::endl;
      count++;
    }
    std::clog << std::endl;
  }
  catch (std::exception & x) {
    std::cerr << "error: " << x.what() << std::endl;
    error_code = EXIT_FAILURE;
  }
  catch (...) {
    std::cerr << "error: " << "unexpected error !" << std::endl;
    error_code = EXIT_FAILURE;
  }
  return error_code;
}
