// -*- mode: c++; -*- 
// test_event_id.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <datatools/event/event_id.h>

int main( int argc_ , char ** argv_ )
{
  int error_code=EXIT_SUCCESS;
  try {

    std::cerr << "Hello, this is a sample program for class 'event_id'!" << std::endl; 
    bool debug=false;

    int iarg=1;
    while ( iarg<argc_ ) {
    
      std::string arg=argv_[iarg];

      if ( arg == "-d" || arg == "--debug" ) debug=true;

      iarg++;
    }
    
    datatools::event::event_id my_id;
    
    my_id.set(666,34);

    std::cout << "Event id = " << my_id << std::endl;

    std::cerr << "Enter event id [format=XXX_YYY] ? ";
    std::cin >> my_id;
    if ( ! std::cin ) {
      throw std::runtime_error("Format error!");
    }
    std::cout << "Event id = " << my_id << std::endl;
    my_id.tree_dump(std::cout,"datatools::event::event_id:");
  
  }
  catch(std::exception & x){
    std::cerr << "error: " << x.what() << std::endl; 
    error_code=EXIT_FAILURE;
  }
  catch(...){
    std::cerr << "error: " << "unexpected error!" << std::endl; 
    error_code=EXIT_FAILURE;
  }
  return error_code;
}

// end of test_event_id.cxx
