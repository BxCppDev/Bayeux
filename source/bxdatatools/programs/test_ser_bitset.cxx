/* test_ser_bitset.cxx */

#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
 
#include <boost/filesystem.hpp>

#include <boost/serialization/bitset.hpp>

#include <datatools/serialization/serialization.h>

using namespace std;

int main (int argc_, char ** argv_) 
{
  // 2009-11-17 FM: do not compile with gcc 3:
#if __GNUC__ > 3
  try 
    {
      bool debug = false;
      long seed  = 12345;
      string filename = "";
      size_t nrecords = 3;

      enum format_t
	{
	  FORMAT_TXT = 0, 
	  FORMAT_XML = 1, 
	  FORMAT_BIN = 2
	};
      int fmt   = FORMAT_XML;
      bool test = false;

      int iarg = 1;
      while (iarg < argc_) 
	{
	  string arg=argv_[iarg];
	  if (arg[0] == '-') 
	    {
	      if (arg == "-d") debug = true;
	      if (arg == "-10") nrecords = 10;
	      if (arg == "-100") nrecords = 100;
	      if (arg == "-1000") nrecords = 1000;
	      if (arg == "-xml") fmt = FORMAT_XML;
	      if (arg == "-txt") fmt = FORMAT_TXT;
	      if (arg == "-bin") fmt = FORMAT_BIN;
	      if (arg == "-test") test = true;
	    }
	  else 
	    {
	      if (filename.empty ()) 
		{
		  filename = arg;
		}
	      else 
		{
		  istringstream iss (arg);
		  iss >> seed;
		  if (! iss)
		    {
		      ostringstream message;
		      message << "format error for 'seed'!"; 
		      throw runtime_error (message.str ());
		    }
		}
	    }
	  iarg++;
	}

      if (filename.empty ()) 
	{
	  if (fmt == FORMAT_XML) filename = "test_ser_bitset.xml";
	  if (fmt == FORMAT_TXT) filename = "test_ser_bitset.txt";
	  if (fmt == FORMAT_BIN) filename = "test_ser_bitset.data";
	} 

      datatools::serialization::io_factory::g_debug = debug;
      srand48 (seed);
      clog << "NOTICE: using filename '" << filename << "'" << endl;

      int mode_guess;
      if (datatools::serialization::\
	  io_factory::guess_mode_from_filename (filename, mode_guess) 
	  != datatools::serialization::io_factory::SUCCESS) 
	{
	  ostringstream message;
	  message << "Cannot guess mode for file '" << filename << "'!";
	  throw runtime_error (message.str ());
	}
      
      if (boost::filesystem::exists (filename)) 
	{
	  ostringstream message;
	  message << "File '" << filename << "' already exists!";
	  throw runtime_error (message.str ());
	}
      
      const size_t BSSZ = 8;
      {
	clog << "NOTICE: writing..." << endl;
	datatools::serialization::data_writer writer (filename);

	// safe_serial
	datatools::serialization::safe_serial<std::bitset<BSSZ> > bitset_data;

	for (int i = 0; i < (int) nrecords; i++) 
	  {
	    if (debug) clog << "DEBUG: Counts = " << i << endl;
	    bitset_data.make ();
	    for (int i = 0; i < BSSZ; i++)
	      {
		bitset_data.get ().set(i, drand48 () < 0.5 ? true: false);
	      }
#if __GNUC__ > 3
	    clog << "bitset = " << bitset_data.get ().to_string () << endl;
#endif
	    writer.store ("__std::bitset__", bitset_data.get ());
	  }
	clog << "NOTICE: writing done." << endl << endl;
      }
    
      {
	clog << "NOTICE: reading..." << endl;

	datatools::serialization::safe_serial<std::bitset<BSSZ> > bitset_data;

	size_t counts = 0;
	datatools::serialization::data_reader reader (filename);    
	while (reader.has_record_tag ()) 
	  {
	    if (debug) clog << "DEBUG: read next record..." << endl;
	    if (reader.record_tag_is ("__std::bitset__")) 
	      {
		if (debug) clog << "DEBUG: making a new safe bitset..." 
				<< endl;
		bitset_data.make ();
		if (debug) clog << "DEBUG: loading the new safe bitset..." 
				<< endl;
		reader.load ("__std::bitset__", bitset_data.get ());
		if (debug) clog << "DEBUG: loading done." 
				<< endl;
#if __GNUC__ > 3
		clog << "bitset = " << bitset_data.get ().to_string () << endl;
#endif
	      }
	    else 
	      {
		string bad_tag = reader.get_record_tag ();
		clog << "ERROR: unknown data tag '" 
		     << bad_tag << "'!" << endl; 
		break;
	      }
	    counts++;
	    if (debug) clog << "DEBUG: Counts = " << counts << endl;
	  }
	clog << "NOTICE: reading done." << endl << endl;   
      } 

    }
  catch (exception & x) 
    {
      cerr << "test_ser_bitset: ERROR: " << x.what () << endl;
      exit (EXIT_FAILURE);
    }
#endif
  return (EXIT_SUCCESS);
}

/* end of test_ser_bitset.cxx */
