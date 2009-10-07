// -*- mode: c++; -*- 
// test_event_id.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>

#include <datatools/serialization/io_factory.h>
#include <datatools/event/event_id.h>

using namespace std;

int main (int argc_ , char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      clog << "Hello, this is a sample program for class 'event_id'!" << endl; 
      bool debug = false;

      int iarg = 1;
      while (iarg < argc_) 
	{
	  string arg = argv_[iarg];

	  if ((arg == "-d") || (arg == "--debug")) debug = true;

	  iarg++;
	}
    
      datatools::event::event_id my_id;
    
      my_id.set (666, 34);

      clog << "Event id = " << my_id << endl;

      clog << "Enter event id [format=XXX_YYY] ? ";
      cin >> my_id;
      if (! cin) 
	{
	  throw runtime_error ("Format error!");
	}
      clog << "Event id = " << my_id << endl;
      my_id.tree_dump (clog, "datatools::event::event_id:");
  
      string filename = "test_event_id.xml";
      {
	datatools::serialization::data_writer writer(filename);
	
	for (int i = 0; i < 4; i++) 
	  {
	    my_id.set (666, i);
	    writer.store (my_id);
	  }
      }

      {
	datatools::serialization::data_reader reader (filename);
	while (reader.has_record_tag ()) 
	  {
	    if (reader.record_tag_is (my_id.get_serial_tag ())) 
	      {
		reader.load (my_id);
		my_id.tree_dump (clog, "event_id",">>> ");
	      }
	    else 
	      {
		break;
	      }
	  }
      }

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

// end of test_event_id.cxx
