// -*- mode: c++; -*- 
// test_basic_event.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

#include <datatools/serialization/io_factory.h>
#include <datatools/event/basic_event.h>

int main( int argc_ , char ** argv_ )
{
  int error_code=EXIT_SUCCESS;
  try {

    std::cerr << "Hello, this is a sample program for class 'basic_event'!" 
	      << std::endl; 
    bool debug=false;

    int iarg=1;
    while ( iarg<argc_ ) {
    
      std::string arg=argv_[iarg];

      if ( arg == "-d" || arg == "--debug" ) debug=true;

      iarg++;
    }
    
    datatools::event::basic_event my_event;
    
    my_event.id(datatools::event::event_id(666,34));

    my_event.properties().store("hell",666);

    my_event.properties().store("pi",3.14159);

    std::vector<double> coords;
    coords.assign(3,0.0);
    coords[0] = +1.0;
    coords[1] = -2.0;
    coords[2] = +0.25;

    my_event.properties().store("position",coords);

    my_event.tree_dump(std::cout,"datatools::event::basic_event:");

    std::cout << "serialize: writing..." << std::endl;
    std::string filename = "test_basic_event.xml";
    {
      datatools::serialization::data_writer writer(filename);
      writer.store(my_event);
    }

    std::cout << "reset event..." << std::endl;
    my_event.clear();

    std::cout << "serialize: reading..." << std::endl;
    {
      datatools::serialization::data_reader reader(filename);
      if ( reader.has_record_tag() ) {
	if ( reader.get_record_tag() == "__BASIC_EVENT__" ) {
	  reader.load(my_event);
	}
      }
    }

    my_event.tree_dump(std::cout,"datatools::event::basic_event:");
  
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

// end of test_basic_event.cxx
