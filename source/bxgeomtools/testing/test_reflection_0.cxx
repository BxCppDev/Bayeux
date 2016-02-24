// test_reflection_0.cxx

// Standard library:
#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

// Third party:
// - Boost:
#include <boost/scoped_ptr.hpp>
// - Camp:
#include <camp/camptype.hpp>
#include <camp/class.hpp>
#include <camp/enum.hpp>
#include <camp/value.hpp>
#include <camp/args.hpp>
#include <camp/userobject.hpp>
#include <camp/classget.hpp>

// - Bayeux/datatools:
#include <datatools/logger.h>
#include <datatools/clhep_units.h>

// Introspectable classes :
#include <geomtools/utils.h>
#include <geomtools/geom_id.h>
#include <geomtools/base_hit.h>
#include <geomtools/placement.h>

// Some pre-processor guard about CAMP reflection usage and linkage :
#include <datatools/reflection_guard.h>
#include <geomtools/reflection_guard.h>

void test_placement()
{
  datatools::logger::priority logging = datatools::logger::PRIO_NOTICE;
  const camp::Class & plctMetaClass  = camp::classByName("geomtools::placement");

  camp::UserObject plctObj0 = plctMetaClass.construct();

  DT_LOG_NOTICE(logging, "Initializing the placement object...");

  plctObj0.call("set_translation", camp::Args(3.9 * CLHEP::mm,
                                              2.3 * CLHEP::mm,
                                              4.5 * CLHEP::mm));
  plctObj0.call("set_orientation_zyz", camp::Args(30.0 * CLHEP::degree,
                                                  25.0 * CLHEP::degree,
                                                   0.0 * CLHEP::degree));
  plctObj0.call("tree_print", camp::Args(1,"The placement: "));

  // Does not work... to be investigated...
  // camp::UserObject plctObj1(*plct1.get());
  // camp::Value plctVal0 = plctObj0.call("get_placement", camp::Args(0));
  // plctObj1 = plctVal0.to<camp::UserObject>();
  // plctObj1.call("tree_print", camp::Args(1,"The extracted placement: "));

}

void test_base_hit()
{
  datatools::logger::priority logging = datatools::logger::PRIO_NOTICE;
  const camp::Class & hitMetaClass  = camp::classByName("geomtools::base_hit");
  const camp::Class & gidMetaClass  = camp::classByName("geomtools::geom_id");
  const camp::Class & propMetaClass = camp::classByName("datatools::properties");


  camp::UserObject gObj0 = gidMetaClass.construct(camp::Args(1203, 1,3,4));
  DT_LOG_NOTICE(logging, "Mutable GID from the hit object : " << gObj0.get<geomtools::geom_id>());

  camp::UserObject pObj0 = propMetaClass.construct(camp::Args("Hit auxiliary properties"));
  pObj0.call("store_flag",    camp::Args("raw", "", false));
  pObj0.call("store_real",    camp::Args("time", 0.343, "Hit timestamp", false));
  pObj0.call("store_real",    camp::Args("energy", 1.278, "Hit energy", false));
  pObj0.call("store_integer", camp::Args("status", 3, "Hit status", false));

  DT_LOG_NOTICE(logging, "Modify the ID and GID of the hit object : ");
  camp::UserObject hObj0 = hitMetaClass.construct();
  hObj0.set("hit_id",        camp::Value(3));
  hObj0.set("geom_id",       camp::Value(gObj0));
  hObj0.set("auxiliaries",   camp::Value(pObj0));

  DT_LOG_NOTICE(logging, "The hit object : ");
  hObj0.call("tree_print", camp::Args(1,"The base hit: "));

  {
    camp::Value gidVal = hObj0.call("grab_geom_id");
    camp::UserObject gObj1 = gidVal.to<camp::UserObject>();
    gObj1.set("type", camp::Value(1206));
    geomtools::geom_id & gid_ref = gObj1.get<geomtools::geom_id>();
    DT_LOG_NOTICE(logging, "Mutable GID from the hit object : " << gid_ref);
  }

  {
    camp::Value gidVal = hObj0.call("get_geom_id");
    camp::UserObject gObj1 = gidVal.to<camp::UserObject>();
    gObj1.set("type", camp::Value(1206));
    const geomtools::geom_id & gid_ref = gObj1.get<geomtools::geom_id>();
    DT_LOG_NOTICE(logging, "Non mutable GID from the hit object : " << gid_ref);
  }

  DT_LOG_NOTICE(logging, "The hit object : ");
  hObj0.call("tree_print", camp::Args(1,"The base hit: "));
  return;
}


void test_geom_id()
{
  datatools::logger::priority logging = datatools::logger::PRIO_NOTICE;
  const camp::Class & gidMetaClass = camp::classByName("geomtools::geom_id");

  DT_LOG_NOTICE(logging, "Modify the type and depth of the GID object : ");
  camp::UserObject vObj0 = gidMetaClass.construct(camp::Args(1203, 1,3,4));
  vObj0.set("type", camp::Value(1220));
  vObj0.set("depth", camp::Value(5));

  DT_LOG_NOTICE(logging, "Set missing sub-addresses in the GID object : ");
  gidMetaClass.function("set").call(vObj0, camp::Args(3, 17) );
  gidMetaClass.function("set").call(vObj0, camp::Args(4, 666) );

  camp::UserObject vObj1 = gidMetaClass.construct(camp::Args(1220, 1,3,4));
  gidMetaClass.function("set_any").call(vObj1, camp::Args(3) );
  gidMetaClass.function("set_any").call(vObj1, camp::Args(4) );
  camp::Value matchVal = gidMetaClass.function("match").call(vObj1, camp::Args(vObj0, false) );
  std::clog << "matchVal = " << matchVal << std::endl;

  camp::UserObject vObj2 = gidMetaClass.construct(camp::Args(1223, 9,4));
  camp::Value matchVal2 = gidMetaClass.function("match").call(vObj2, camp::Args(vObj1, false) );
  std::clog << "matchVal2 = " << matchVal2 << std::endl;
  return;
}

void test_vector_3d()
{
  const camp::Class & v3dMetaClass = camp::classByName("geomtools::vector_3d");

  camp::UserObject vObj0 = v3dMetaClass.construct(camp::Args(1.0, 2.0, 3.0));

  vObj0.set("x", camp::Value(4.0));
  vObj0.set("y", camp::Value(0.0));
  vObj0.set("z", camp::Value(3.0));
  std::clog << "v0.x = " << vObj0.get("x") << std::endl;
  std::clog << "v0.y = " << vObj0.get("y") << std::endl;
  std::clog << "v0.z = " << vObj0.get("z") << std::endl;

  camp::Value magValue = vObj0.get("mag");
  std::clog << "|v0| = " << magValue << std::endl;

  return;
}

void test_enums()
{
  std::clog << std::endl << "*** test_enums : Reflection..." << std::endl;

  std::clog <<  "Number of meta-classes = " << camp::classCount() << std::endl;
  for (int i = 0; i < (int) camp::classCount(); i++) {
    const camp::Class & c = camp::classByIndex(i);
    std::clog << "Metaclass #" << i << " : " << c.name() << std::endl;
  }

  std::clog <<  "Number of meta-enums = " << camp::enumCount() << std::endl;
  for (int i = 0; i < (int) camp::enumCount(); i++) {
    const camp::Enum & e = camp::enumByIndex(i);
    std::clog << "Metaenum #" << i << " : " << e.name() << std::endl;
  }

  camp::detail::EnumManager & emgr = camp::detail::EnumManager::instance();
  std::clog << "Count: " << emgr.count() << std::endl;

  {
    const camp::Enum & tMetaEnum = camp::enumByName("datatools::logger::priority");
    std::clog << "Meta Enum '" << tMetaEnum.name() << std::endl;
    for (int i = 0; i < (int) tMetaEnum.size(); i++) {
      std::clog << " - Key '" << tMetaEnum.pair(i).name
                << "' has value = " << tMetaEnum.pair(i).value << std::endl;
    }
  }


  {
    const camp::Enum & tMetaEnum = camp::enumByName("geomtools::orientation_type");
    std::clog << "Meta Enum '" << tMetaEnum.name() << std::endl;
    for (int i = 0; i < (int) tMetaEnum.size(); i++) {
      std::clog << " - Key '" << tMetaEnum.pair(i).name
                << "' has value = " << tMetaEnum.pair(i).value << std::endl;
    }
  }

  {
    const camp::Enum & tMetaEnum = camp::enumByName("geomtools::axis_type");
    std::clog << "Meta Enum '" << tMetaEnum.name() << std::endl;
     for (int i = 0; i < (int) tMetaEnum.size(); i++) {
      std::clog << " - Key '" << tMetaEnum.pair(i).name
                << "' has value = " << tMetaEnum.pair(i).value << std::endl;
    }
  }

  return;
}


int main (int /* argc_ */, char ** /* argv_ */)
{
  datatools::logger::priority logging = datatools::logger::PRIO_NOTICE;
  int error_code = EXIT_SUCCESS;
  try {

    DT_LOG_NOTICE(logging, "Number of metaclasses = " << camp::classCount());

    for (int i = 0; i < (int) camp::classCount(); i++) {
      const camp::Class & c = camp::classByIndex(i);
      DT_LOG_NOTICE(logging, "Metaclass #" << i << " : " << c.name());
    }

    DT_LOG_NOTICE(logging, "Running test_enums...");
    test_enums();

    DT_LOG_NOTICE(logging, "Running test_vector_3d...");
    test_vector_3d();

    DT_LOG_NOTICE(logging, "Running test_geom_id...");
    test_geom_id();

    DT_LOG_NOTICE(logging, "Running test_base_hit...");
    test_base_hit();

    DT_LOG_NOTICE(logging, "Running test_placement...");
    test_placement();

  }
  catch (std::exception & x) {
    DT_LOG_ERROR(datatools::logger::PRIO_ERROR, x.what());
    error_code = EXIT_FAILURE;
  }
  return error_code;
}

/* end of test_reflection_0.cxx */
