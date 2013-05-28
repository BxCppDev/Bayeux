// test_tracer.cxx - unit tests for datatools/logger.h
//
// Copyright (c) 2013 by François Mauger <mauger@lpccaen.in2p3.fr>
// Copyright (c) 2013 by Université de Caen Basse-Normandie

// Ourselves
#include "datatools/tracer.h"

// Standard Library
#include <iostream>
#include <cstdlib>

// Third Party
// - Boost
#include <boost/filesystem.hpp>

// This Project
#include "datatools/logger.h"

void test_tracer_1() {

  int seed48  = 314159;
  srand48(seed48);
  boost::filesystem::remove("test_tracer.log");
  datatools::tracer t(1, "test_tracer.log");
  for (int i = 0; i < 20; ++i) {
    t.trace(drand48());
  }
  for (int i = 0; i < 5; ++i) {
    t.out(true);
    t.trace(drand48(), false);
    t.trace((int) (100 * drand48()), false);
    t.trace("token", false);
    t.out(false) << std::endl;
  }
}

void test_tracer_2() {
  boost::filesystem::remove("test_tracer_1000.log");
  DT_TRACER_INIT(1000, "test_tracer_1000.log");
  for (int i = 0; i < 20; ++i) {
    DT_TRACER_TRACE(1000, drand48());
  }
  for (int i = 0; i < 5; ++i) {
    DT_TRACER_TRACE(1000, (int) (100 * drand48()));
  }
}

void test_tracer_3() {
  boost::filesystem::remove("tracer_3000.log");
  for (int i = 0; i < 20; ++i) {
    DT_TRACER_TRACE(3000, drand48());
  }
  for (int i = 0; i < 5; ++i) {
    DT_TRACER_MESSAGE(3000, "Value=" << (int) (100 * drand48()) << " token" );
  }
}

int main(int argc, const char *argv[])
{
  datatools::logger::priority logging = datatools::logger::PRIO_NOTICE;
  try {
    DT_LOG_NOTICE(logging, "Running test_tracer_1...");
    test_tracer_1();

    DT_LOG_NOTICE(logging, "Running test_tracer_2...");
    test_tracer_2();

    DT_LOG_NOTICE(logging, "Running test_tracer_3...");
    test_tracer_3();
  }
  catch (std::exception& e) {
    std::cerr << "test_tracer failed : " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
