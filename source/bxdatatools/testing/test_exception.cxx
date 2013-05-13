// test_exception.cxx - unit tests for datatools/exception.h
//
// Copyright (c) 2013 by Ben Morgan <bmorgan.warwick@gmail.com> 
// Copyright (c) 2013 by The University of Warwick

// Ourselves
#include "datatools/exception.h"

// Standard Library
#include <iostream>
// Third Party
// - A

// This Project

//! throw an exception if parameter is negative
void check_throws(int i) {
  DT_THROW_IF(i < 0, 
              std::invalid_argument, 
              "argument i[i = " << i << "] is negative");
}

int main(int argc, const char *argv[])
{
  //TEST: Must throw
  try {
    check_throws(-1);
  }
  catch (std::invalid_argument& e) {
    std::cout << e.what() << std::endl;
  }
  catch (std::exception& e) {
    std::cerr << "check_throws(-1) threw incorrect exception" << std::endl;
    return 1;
  }

  //TEST:  Must not throw
  try {
    check_throws(1);
  }
  catch (std::exception& e) {
    std::cerr << "check_throws(1) threw exception and should not have" << std::endl;
    return 1;
  }

  return 0;
}



