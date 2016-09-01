// test_reflection_0.cxx

// Standard library;
#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Camp:
#include <camp/class.hpp>
#include <camp/userobject.hpp>
#include <camp/value.hpp>
#include <camp/args.hpp>
// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/clhep_units.h>

// Introspectable classes :
#include <genbb_help/primary_particle.h>
#include <genbb_help/primary_event.h>

void test_primary_particle();
void test_primary_event();

int main (int /* argc_ */, char ** /* argv_ */)
{
  datatools::logger::priority logging = datatools::logger::PRIO_NOTICE;
  int error_code = EXIT_SUCCESS;

  {
    // trick to force linking to Bayeux:
    genbb::primary_particle dummy;
  }

  try {

    DT_LOG_NOTICE(logging, "Number of metaclasses = " << camp::classCount());

    for (int i = 0; i < (int) camp::classCount(); i++) {
      const camp::Class & c = camp::classByIndex(i);
      DT_LOG_NOTICE(logging, "Metaclass #" << i << " : " << c.name());
    }

    DT_LOG_NOTICE(logging, "Running test_primary_particle...");
    test_primary_particle();

    DT_LOG_NOTICE(logging, "Running test_primary_event...");
    test_primary_event();

  } catch (std::exception & x) {
    DT_LOG_ERROR(datatools::logger::PRIO_ERROR, x.what());
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

void test_primary_particle()
{
  datatools::logger::priority logging = datatools::logger::PRIO_NOTICE;
  const camp::Class & ppMetaClass  = camp::classByName("genbb::primary_particle");

  camp::UserObject partObj0 = ppMetaClass.construct();
  DT_LOG_NOTICE(logging, "Initializing the primary particle object...");

  partObj0.set("type",           camp::Value(3));
  partObj0.set("particle_label", camp::Value("electron"));
  partObj0.set("time",           camp::Value(3.9 * CLHEP::ns));

  geomtools::vector_3d momentum(0.23 * CLHEP::MeV,
                                0.13 * CLHEP::MeV,
                                1.35 * CLHEP::MeV);
  geomtools::vector_3d vertex(0.0 * CLHEP::mm,
                              0.0 * CLHEP::mm,
                              5.3 * CLHEP::mm);
  camp::UserObject momObj = momentum;
  camp::UserObject vtxObj = vertex;
  partObj0.set("momentum", camp::Value(momObj));
  partObj0.set("vertex", camp::Value(vtxObj));
  partObj0.call("tree_print", camp::Args(1,"The primary particle: "));

}

void test_primary_event()
{
  //datatools::logger::priority logging = datatools::logger::PRIO_NOTICE;
  const camp::Class & peMetaClass  = camp::classByName("genbb::primary_event");

  camp::UserObject eventObj0 = peMetaClass.construct();

  eventObj0.call("tree_print", camp::Args(1,"The primary event: "));

}

#include <camp/classget.hpp>

/* end of test_reflection_0.cxx */
