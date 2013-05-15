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
    DT_LOG_FATAL(prio_, "fatal message");
    DT_LOG_CRITICAL(prio_, "critical message");
    DT_LOG_ERROR(prio_, "error message");
    DT_LOG_WARNING(prio_, "warning message");
    DT_LOG_NOTICE(prio_, "notice message");
    DT_LOG_INFORMATION(prio_, "information message");
    DT_LOG_DEBUG(prio_, "debug message");
    DT_LOG_TRACE(prio_, "trace message");
    DT_LOG_TRACE(prio_, "trace message at " << prio_);
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
  datatools::logger::priority p = datatools::logger::get_priority("foo");
  DT_THROW_IF(p != datatools::logger::PRIO_UNDEFINED,
              std::runtime_error,
              "get_priority did not return PRIO_UNDEFINED for bogus priority label");
}

//! Check that priorities are converted to valid strings
void test_priority_to_string() {
  typedef std::map<datatools::logger::priority, std::string> TPSMap;
  TPSMap mapValues;

  boost::assign::insert(mapValues)
      (datatools::logger::PRIO_FATAL,     "fatal")        
      (datatools::logger::PRIO_CRITICAL,  "critical")    
      (datatools::logger::PRIO_ERROR,     "error")       
      (datatools::logger::PRIO_WARNING,   "warning")     
      (datatools::logger::PRIO_NOTICE,    "notice")      
      (datatools::logger::PRIO_INFORMATION, "information")
      (datatools::logger::PRIO_DEBUG,     "debug")       
      (datatools::logger::PRIO_TRACE,    "trace");

  BOOST_FOREACH(TPSMap::value_type& v, mapValues) {
    std::string label = datatools::logger::get_priority_label(v.first);
    std::cout << "Checking " << v.first << "->" << v.second << ", got " << label << std::endl;
    DT_THROW_IF(label != v.second, 
                std::runtime_error, 
                "get_priority_label(\"" << v.first << "\") != " << v.second);
  }

  // Check for empty string if user does some out-of-bounds nastiness
  std::string negOutOfBounds = datatools::logger::get_priority_label(static_cast<datatools::logger::priority>(-2));
  DT_THROW_IF(negOutOfBounds != "",
              std::runtime_error,
              "get_priority_label does not return empty string on negative out of bounds argument");

  std::string posOutOfBounds = datatools::logger::get_priority_label(static_cast<datatools::logger::priority>(84));
  DT_THROW_IF(posOutOfBounds != "",
              std::runtime_error,
              "get_priority_label does not return empty string on positive out of bounds argument");
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
  
  try {
    test_priority_to_string();
  }
  catch (std::exception& e) {
    std::cerr << "test_priority_to_string failed : " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
