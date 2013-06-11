/* test_reflection_0.cxx */

#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <boost/scoped_ptr.hpp>

#include <datatools/logger.h>
#include <datatools/clhep_units.h>

// Introspectable classes :
#include <genbb_help/primary_particle.h>
#include <genbb_help/primary_event.h>

// Some pre-processor guard about Bio usage and linkage :
#include <datatools/bio_guard.h>

// Some pre-processor guard about CAMP reflection usage and linkage :
#include <datatools/reflection_guard.h>
#include <geomtools/reflection_guard.h>
#include <genbb_help/reflection_guard.h>


void test_primary_particle()
{
  datatools::logger::priority logging = datatools::logger::PRIO_NOTICE;
  const DR_CLASS & ppMetaClass  = DR_CLASS_BY_NAME("genbb::primary_particle");

  boost::scoped_ptr<genbb::primary_particle> part0(ppMetaClass.construct<genbb::primary_particle>());

  DR_OBJECT partObj0(*part0.get());
  DT_LOG_NOTICE(logging, "Initializing the primary particle object...");

  partObj0.set("type",           DR_VALUE(3));
  partObj0.set("particle_label", DR_VALUE("electron"));
  partObj0.set("time",           DR_VALUE(3.9 * CLHEP::ns));

  geomtools::vector_3d momentum(0.23 * CLHEP::MeV,
                                0.13 * CLHEP::MeV,
                                1.35 * CLHEP::MeV);
  geomtools::vector_3d vertex(0.0 * CLHEP::mm,
                              0.0 * CLHEP::mm,
                              5.3 * CLHEP::mm);
  DR_OBJECT momObj = momentum;
  DR_OBJECT vtxObj = vertex;
  partObj0.set("momentum", DR_VALUE(momObj));
  partObj0.set("vertex", DR_VALUE(vtxObj));

  partObj0.DR_CALL("tree_print", DR_ARGS(1,"The primary particle: "));

}

void test_primary_event()
{
  datatools::logger::priority logging = datatools::logger::PRIO_NOTICE;
  const DR_CLASS & peMetaClass  = DR_CLASS_BY_NAME("genbb::primary_event");
  boost::scoped_ptr<genbb::primary_event> event0(peMetaClass.construct<genbb::primary_event>());

  DR_OBJECT eventObj0(*event0.get());

  eventObj0.DR_CALL("tree_print", DR_ARGS(1,"The primary event: "));

}

#include <camp/classget.hpp>

int main (int argc_, char ** argv_)
{
  datatools::logger::priority logging = datatools::logger::PRIO_NOTICE;
  int error_code = EXIT_SUCCESS;
  try {

    DT_LOG_NOTICE(logging, "Number of metaclasses = " << camp::classCount());

    for (int i = 0; i < camp::classCount(); i++) {
      const camp::Class & c = camp::classByIndex(i);
      DT_LOG_NOTICE(logging, "Metaclass #" << i << " : " << c.name());
    }

    DT_LOG_NOTICE(logging, "Running test_primary_particle...");
    test_primary_particle();

    DT_LOG_NOTICE(logging, "Running test_primary_event...");
    test_primary_event();

  }
  catch (std::exception & x) {
    DT_LOG_ERROR(datatools::logger::PRIO_ERROR, x.what());
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

/* end of test_reflection_0.cxx */
