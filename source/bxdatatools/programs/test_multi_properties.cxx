// -*- mode: c++; -*- 
// test_multi_properties.cxx 

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>

#include <datatools/serialization/io_factory.h>
#include <datatools/serialization/serialization.h>
#include <datatools/utils/multi_properties.h>

using namespace std;

int main (int argc_ , char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      clog << "Test of the 'multi_properties' class..." << endl; 
      bool debug = false;

      int iarg = 1;
      while (iarg < argc_) 
	{
	  string arg = argv_[iarg];

	  if (arg == "-d" || arg == "--debug") debug = true;
	  iarg++;
	}

      namespace du = datatools::utils;
      du::multi_properties::g_debug = debug;

      {
	clog << endl << "Write test:" << endl;
	du::multi_properties mp ("name", "type", "A sample list of setups");
	mp.set_debug (debug);

	mp.add ("object_1", "type_1");
	du::multi_properties::entry & e1 = mp.get ("object_1");
	e1.get_properties ().store ("debug", true);
	e1.get_properties ().store ("colour", "blue");
	e1.get_properties ().store ("depth", 1.234);
	
	mp.add ("object_2", "type_2");
	du::multi_properties::entry & e2 = mp.get ("object_2");
	e2.get_properties ().store ("debug", false);
	e2.get_properties ().store ("temperature", 300.0);
	 
	mp.add ("object_3", "type_1");
	du::multi_properties::entry & e3 = mp.get ("object_3");
	e3.get_properties ().store ("test", true);
	e3.get_properties ().store ("mass", 15.0);
	e3.get_properties ().store ("length", 0.12);
	e3.get_properties ().store ("width", 0.04);
	e3.get_properties ().store ("depth", 0.06);
	 
	mp.add (du::properties::make_private_key ("object_4"), "type_3");
	du::multi_properties::entry & e4 
	  = mp.get (du::properties::make_private_key ("object_4"));
	e4.get_properties ().store ("secret", true);
	e4.get_properties ().store ("password", "********");
	e4.get_properties ().store ("code", 1234567);

	mp.add ("object_3a", "type_1");
	du::multi_properties::entry & e3a = mp.get ("object_3a");
	e3a.get_properties ().store ("to_be_removed", true);
	mp.add ("object_3b", "type_1");
	du::multi_properties::entry & e3b = mp.get ("object_3b");
	e3b.get_properties ().store ("to_be_removed", true);

	mp.dump (clog);

	clog << endl << "Removing 2 entries..." << endl;
	mp.remove ("object_3a");
	mp.remove ("object_3b");

	mp.dump (clog);

	clog << endl << "Saving in ASCII file keeping private entries..." << endl;
	mp.write ("test_multi_properties.conf", 
		  du::multi_properties::with_header_footer,
		  du::multi_properties::write_private_also);  
 
	{
	  clog << endl << "Serializing..." << endl;
	  string filename = "test_multi_properties.xml";
	  clog << "Writing to '" << filename << "' ..." << endl;
	  datatools::serialization::data_writer writer (filename);
	  writer.store (mp);
	  clog << "Serialization is done." << endl;
	}

      }

      {
	clog << endl << "Read test:" << endl;
	du::multi_properties mp ("name", "type");
	clog << "Reading from ASCII file skipping private entries..." << endl;
	mp.read ("test_multi_properties.conf", 
		 du::multi_properties::read_public_only);  
	mp.dump (clog);
	mp.write ("test_multi_properties_2.conf", 
		  du::multi_properties::with_header_footer,
		  du::multi_properties::write_public_only);  
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

// end of test_multi_properties.cxx
