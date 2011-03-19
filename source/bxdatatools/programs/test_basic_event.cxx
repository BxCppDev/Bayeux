// -*- mode: c++; -*- 
// test_basic_event.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

#include <boost/serialization/vector.hpp>

#include <datatools/event/basic_event.h>

#include <datatools/the_serializable.h>
#include <datatools/serialization/io_factory.h>

using namespace std;

int main (int argc_ , char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      clog << "Test of the 'basic_event' class..." << endl; 
      bool debug = false;

      int iarg = 1;
      while (iarg < argc_) 
	{
	  string arg = argv_[iarg];

	  if (arg == "-d" || arg == "--debug") debug = true;
	  iarg++;
	}
    
      datatools::event::basic_event my_event;
    
      my_event.id (datatools::event::event_id (666, 34));

      my_event.properties ().store ("hell", 666);

      my_event.properties ().store ("pi", 3.14159);

      vector<double> coords;
      coords.assign (3, 0.0);
      coords[0] = +1.0;
      coords[1] = -2.0;
      coords[2] = +0.25;

      my_event.properties ().store ("position", coords);

      my_event.tree_dump (cout, "datatools::event::basic_event:");

      cout << "serialize: writing..." << endl;
      string filename = "test_basic_event.xml";
      {
	datatools::serialization::data_writer writer (filename);
	writer.store (my_event);
      }

      cout << "reset event..." << endl;
      my_event.clear ();

      cout << "serialize: reading..." << endl;
      {
	datatools::serialization::data_reader reader (filename);
	if (reader.has_record_tag ()) 
	  {
	    if (reader.get_record_tag () == "__BASIC_EVENT__") 
	      {
		reader.load (my_event);
	      }
	  }
      }

      my_event.tree_dump (cout, "datatools::event::basic_event:");
  
    }
  catch (exception & x)
    {
      cerr << "error: " << x.what () << endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error!" << endl; 
      error_code = EXIT_FAILURE;
    }
  return error_code;
}

// end of test_basic_event.cxx
