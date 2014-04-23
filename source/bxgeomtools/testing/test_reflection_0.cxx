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
  const DR_CLASS & plctMetaClass  = DR_CLASS_BY_NAME("geomtools::placement");

  boost::scoped_ptr<geomtools::placement> plct0(plctMetaClass.construct<geomtools::placement>());

  DR_OBJECT plctObj0(*plct0.get());
  DT_LOG_NOTICE(logging, "Initializing the placement object...");

  plctObj0.DR_CALL("set_translation", DR_ARGS(3.9 * CLHEP::mm,
                                              2.3 * CLHEP::mm,
                                              4.5 * CLHEP::mm));
  plctObj0.DR_CALL("set_orientation_zyz", DR_ARGS(30.0 * CLHEP::degree,
                                                  25.0 * CLHEP::degree,
                                                   0.0 * CLHEP::degree));
  plctObj0.DR_CALL("tree_print", DR_ARGS(1,"The placement: "));

  boost::scoped_ptr<geomtools::placement> plct1(plctMetaClass.construct<geomtools::placement>());

  // Does not work... to be investigated...
  // DR_OBJECT plctObj1(*plct1.get());
  // DR_VALUE plctVal0 = plctObj0.DR_CALL("get_placement", DR_ARGS(0));
  // plctObj1 = plctVal0.to<DR_OBJECT>();
  // plctObj1.DR_CALL("tree_print", DR_ARGS(1,"The extracted placement: "));

}

void test_base_hit()
{
  datatools::logger::priority logging = datatools::logger::PRIO_NOTICE;
  const DR_CLASS & hitMetaClass  = DR_CLASS_BY_NAME("geomtools::base_hit");
  const DR_CLASS & gidMetaClass  = DR_CLASS_BY_NAME("geomtools::geom_id");
  const DR_CLASS & propMetaClass = DR_CLASS_BY_NAME("datatools::properties");

  boost::scoped_ptr<geomtools::base_hit> hit0(hitMetaClass.construct<geomtools::base_hit>());
  boost::scoped_ptr<geomtools::geom_id> gid0(gidMetaClass.construct<geomtools::geom_id>(DR_ARGS(1203, 1,3,4)));
  boost::scoped_ptr<datatools::properties> prop0(propMetaClass.construct<datatools::properties>(DR_ARGS("Hit auxiliary properties")));

  DR_OBJECT gObj0(*gid0.get());
  DT_LOG_NOTICE(logging, "Mutable GID from the hit object : " << gObj0);

  DR_OBJECT pObj0(*prop0.get());
  pObj0.DR_CALL("store_flag",    DR_ARGS("raw", "", false));
  pObj0.DR_CALL("store_real",    DR_ARGS("time", 0.343, "Hit timestamp", false));
  pObj0.DR_CALL("store_real",    DR_ARGS("energy", 1.278, "Hit energy", false));
  pObj0.DR_CALL("store_integer", DR_ARGS("status", 3, "Hit status", false));

  DT_LOG_NOTICE(logging, "Modify the ID and GID of the hit object : ");
  DR_OBJECT hObj0(*hit0.get());
  hObj0.set("hit_id",        DR_VALUE(3));
  hObj0.set("geom_id",       DR_VALUE(gObj0));
  hObj0.set("auxiliaries",   DR_VALUE(pObj0));

  DT_LOG_NOTICE(logging, "The hit object : ");
  hObj0.DR_CALL("tree_print", DR_ARGS(1,"The base hit: "));

  {
    DR_VALUE gidVal = hObj0.DR_CALL("grab_geom_id");
    DR_OBJECT gObj1 = gidVal.to<DR_OBJECT>();
    gObj1.set("type", DR_VALUE(1206));
    geomtools::geom_id & gid_ref = gObj1.get<geomtools::geom_id>();
    DT_LOG_NOTICE(logging, "Mutable GID from the hit object : " << gid_ref);
  }

  {
    DR_VALUE gidVal = hObj0.DR_CALL("get_geom_id");
    DR_OBJECT gObj1 = gidVal.to<DR_OBJECT>();
    gObj1.set("type", DR_VALUE(1206));
    const geomtools::geom_id & gid_ref = gObj1.get<geomtools::geom_id>();
    DT_LOG_NOTICE(logging, "Non mutable GID from the hit object : " << gid_ref);
  }

  DT_LOG_NOTICE(logging, "The hit object : ");
  hObj0.DR_CALL("tree_print", DR_ARGS(1,"The base hit: "));

}


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
  // datatools::logger::priority logging = datatools::logger::PRIO_NOTICE;

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

  DR_VALUE magValue = vObj0.get("mag");
  std::clog << "|v0| = " << magValue << std::endl;

  std::clog << "v0 = " << *v0.get() << std::endl;

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
    const DR_ENUM & tMetaEnum = camp::enumByName("datatools::logger::priority");
    std::clog << "Meta Enum '" << tMetaEnum.name() << std::endl;
    for (int i = 0; i < (int) tMetaEnum.size(); i++) {
      std::clog << " - Key '" << tMetaEnum.pair(i).name
                << "' has value = " << tMetaEnum.pair(i).value << std::endl;
    }
  }


  {
    const DR_ENUM & tMetaEnum = camp::enumByName("geomtools::orientation_type");
    std::clog << "Meta Enum '" << tMetaEnum.name() << std::endl;
    for (int i = 0; i < (int) tMetaEnum.size(); i++) {
      std::clog << " - Key '" << tMetaEnum.pair(i).name
                << "' has value = " << tMetaEnum.pair(i).value << std::endl;
    }
  }

  {
    const DR_ENUM & tMetaEnum = camp::enumByName("geomtools::axis_type");
    std::clog << "Meta Enum '" << tMetaEnum.name() << std::endl;
     for (int i = 0; i < (int) tMetaEnum.size(); i++) {
      std::clog << " - Key '" << tMetaEnum.pair(i).name
                << "' has value = " << tMetaEnum.pair(i).value << std::endl;
    }
  }


  return;
}

#include <camp/classget.hpp>

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
