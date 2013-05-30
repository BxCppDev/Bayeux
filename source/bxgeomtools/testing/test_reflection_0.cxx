/* test_reflection_0.cxx */

#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <boost/scoped_ptr.hpp>

#include <datatools/logger.h>

// Introspectable classes :
#include <geomtools/utils.h>
#include <geomtools/geom_id.h>

// Some pre-processor guard about CAMP reflection usage and linkage :
#include <geomtools/reflection_guard.h>


void test_geom_id()
{
  datatools::logger::priority logging = datatools::logger::PRIO_NOTICE;
  const DR_CLASS & gidMetaClass = DR_CLASS_BY_NAME("geomtools::geom_id");

  boost::scoped_ptr<geomtools::geom_id> gid0(gidMetaClass.construct<geomtools::geom_id>(DR_ARGS(1203, 1,3,4)));

  DT_LOG_NOTICE(logging, "Original GID object : ");
  std::clog << "gid0 = " << *gid0.get() << std::endl;

  DT_LOG_NOTICE(logging, "Modify the type and depth of the GID object : ");
  DR_OBJECT vObj0(*gid0.get());
  vObj0.set("type", DR_VALUE(1220));
  vObj0.set("depth", DR_VALUE(5));
  std::clog << "gid0 = " << *gid0.get() << std::endl;

  DT_LOG_NOTICE(logging, "Set missing sub-addresses in the GID object : ");
  gidMetaClass.function("set").call(vObj0, DR_ARGS(3, 17) );
  gidMetaClass.function("set").call(vObj0, DR_ARGS(4, 666) );
  std::clog << "gid0 = " << *gid0.get() << std::endl;

  boost::scoped_ptr<geomtools::geom_id> gid1(gidMetaClass.construct<geomtools::geom_id>(DR_ARGS(1220, 1,3,4)));
  DR_OBJECT vObj1(*gid1.get());
  gidMetaClass.function("set_any").call(vObj1, DR_ARGS(3) );
  gidMetaClass.function("set_any").call(vObj1, DR_ARGS(4) );
  std::clog << "gid1 = " << *gid1.get() << std::endl;
  DR_VALUE matchVal = gidMetaClass.function("match").call(vObj1, DR_ARGS(vObj0, false) );
  std::clog << "matchVal = " << matchVal << std::endl;

  boost::scoped_ptr<geomtools::geom_id> gid2(gidMetaClass.construct<geomtools::geom_id>(DR_ARGS(1223, 9,4)));
  DR_OBJECT vObj2(*gid2.get());
  std::clog << "gid2 = " << *gid2.get() << std::endl;
  DR_VALUE matchVal2 = gidMetaClass.function("match").call(vObj2, DR_ARGS(vObj1, false) );
  std::clog << "matchVal2 = " << matchVal2 << std::endl;

}

void test_vector_3d()
{
  datatools::logger::priority logging = datatools::logger::PRIO_NOTICE;

  const DR_CLASS & v3dMetaClass = DR_CLASS_BY_NAME("geomtools::vector_3d");

  boost::scoped_ptr<geomtools::vector_3d> v0(v3dMetaClass.construct<geomtools::vector_3d>(DR_ARGS(1.0, 2.0, 3.0)));

  std::clog << "v0 = " << *v0.get() << std::endl;
  DR_OBJECT vObj0(*v0.get());

  vObj0.set("x", DR_VALUE(4.0));
  vObj0.set("y", DR_VALUE(0.0));
  vObj0.set("z", DR_VALUE(3.0));
  std::clog << "v0.x = " << vObj0.get("x") << std::endl;
  std::clog << "v0.y = " << vObj0.get("y") << std::endl;
  std::clog << "v0.z = " << vObj0.get("z") << std::endl;
  std::clog << "v0   = " << *v0.get() << std::endl;

  DR_VALUE magValue = v3dMetaClass.function("mag").call(vObj0);
  std::clog << "|v0| = " << magValue << std::endl;

  std::clog << "v0 = " << *v0.get() << std::endl;

  return;
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

    DT_LOG_NOTICE(logging, "Running test_vector_3d...");
    test_vector_3d();

    DT_LOG_NOTICE(logging, "Running test_geom_id...");
    test_geom_id();

  }
  catch (std::exception & x) {
    DT_LOG_ERROR(datatools::logger::PRIO_ERROR, x.what());
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

/* end of test_reflection_0.cxx */
