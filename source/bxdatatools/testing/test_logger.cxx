// test_logger.cxx - unit tests for datatools/logger.h
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com> 
// Copyright (c) 2013 by The University of Warwick

// Ourselves
#include "datatools/logger.h"

// Standard Library
#include <iostream>
#include <map>

// Third Party
// - Boost
#include "boost/foreach.hpp"
#include "boost/assign/list_inserter.hpp"

// This Project
#include "datatools/exception.h"

//! Test class with logging priority
class foo_loggable {
 public:
  foo_loggable() : prio_(datatools::logger::PRIO_FATAL) {}

  void logged_method() const {
    datatools_fatal(prio_, "fatal message");
    datatools_critical(prio_, "critical message");
    datatools_error(prio_, "error message");
    datatools_warning(prio_, "warning message");
    datatools_notice(prio_, "notice message");
    datatools_information(prio_, "information message");
    datatools_debug(prio_, "debug message");
    datatools_trace(prio_, "trace message");
    datatools_trace(prio_, "trace message at " << prio_);
  }

  void set_priority(const datatools::logger::priority& p) {
    prio_ = p;
  }
 private:
  datatools::logger::priority prio_;
};

//! Dumb zero check of calling each logging method
void test_logger_macros() {
  foo_loggable a;
  a.set_priority(datatools::logger::PRIO_FATAL);
  a.logged_method();

  a.set_priority(datatools::logger::PRIO_CRITICAL);
  a.logged_method();

  a.set_priority(datatools::logger::PRIO_ERROR);
  a.logged_method();

  a.set_priority(datatools::logger::PRIO_WARNING);
  a.logged_method();

  a.set_priority(datatools::logger::PRIO_NOTICE);
  a.logged_method();

  a.set_priority(datatools::logger::PRIO_INFORMATION);
  a.logged_method();

  a.set_priority(datatools::logger::PRIO_DEBUG);
  a.logged_method();

  a.set_priority(datatools::logger::PRIO_TRACE);
  a.logged_method();
}

//! Check that strings are converted to valid priorities
void test_string_to_priority() {
  typedef std::map<std::string, datatools::logger::priority> TSPMap;
  TSPMap mapValues;

  boost::assign::insert(mapValues)
      ("PRIO_FATAL", datatools::logger::PRIO_FATAL)
      ("FATAL", datatools::logger::PRIO_FATAL)
      ("fatal", datatools::logger::PRIO_FATAL)
      ("PRIO_CRITICAL", datatools::logger::PRIO_CRITICAL)
      ("CRITICAL", datatools::logger::PRIO_CRITICAL)
      ("critical", datatools::logger::PRIO_CRITICAL)
      ("PRIO_ERROR", datatools::logger::PRIO_ERROR)
      ("ERROR", datatools::logger::PRIO_ERROR)
      ("error", datatools::logger::PRIO_ERROR)
      ("PRIO_WARNING", datatools::logger::PRIO_WARNING)
      ("WARNING", datatools::logger::PRIO_WARNING)
      ("warning", datatools::logger::PRIO_WARNING)
      ("PRIO_NOTICE", datatools::logger::PRIO_NOTICE)
      ("NOTICE", datatools::logger::PRIO_NOTICE)
      ("notice", datatools::logger::PRIO_NOTICE)
      ("PRIO_INFORMATION", datatools::logger::PRIO_INFORMATION)
      ("INFORMATION", datatools::logger::PRIO_INFORMATION)
      ("information", datatools::logger::PRIO_INFORMATION)
      ("PRIO_DEBUG", datatools::logger::PRIO_DEBUG)
      ("DEBUG", datatools::logger::PRIO_DEBUG)
      ("debug", datatools::logger::PRIO_DEBUG)
      ("PRIO_TRACE", datatools::logger::PRIO_TRACE)
      ("TRACE", datatools::logger::PRIO_TRACE)
      ("trace", datatools::logger::PRIO_TRACE);

  BOOST_FOREACH(TSPMap::value_type& v, mapValues) {
    datatools::logger::priority p = datatools::logger::get_priority(v.first);
    std::cout << "Checking " << v.first << "->" << v.second << ", got " << p << std::endl;
    DT_THROW_IF(p != v.second, std::runtime_error, "get_priority(\"" << v.first << "\") != " << v.second);
  }
}

int main(int argc, const char *argv[])
{
  test_logger_macros();

  try {
    test_string_to_priority();
  }
  catch (std::exception& e) { 
    std::cerr << "test_string_to_priority failed : " << e.what() << std::endl;
    return 1;
  }
  
  return 0;
}
