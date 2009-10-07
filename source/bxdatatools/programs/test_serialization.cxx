/* test_serialization.cxx */

#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
 
#include <boost/filesystem.hpp>

#include <my_data.h>

#include <datatools/serialization/serialization.h>

using namespace std;

int main (int argc_, char ** argv_) 
{
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
	  if (fmt == FORMAT_XML) filename = "test_serialization.xml";
	  if (fmt == FORMAT_TXT) filename = "test_serialization.txt";
	  if (fmt == FORMAT_BIN) filename = "test_serialization.data";
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
      clog << "mode = " << hex 
	   << mode_guess
	   << dec << endl;
    
      if (boost::filesystem::exists (filename)) 
	{
	  ostringstream message;
	  message << "File '" << filename << "' already exists!";
	  throw runtime_error (message.str ());
	}
    
      {
	clog << "NOTICE: writing..." << endl;
	datatools::serialization::data_writer writer (filename);

	// safe_serial
	datatools::serialization::safe_serial<data_t>      ss_data;
	datatools::serialization::safe_serial<more_data_t> ss_more_data;

	for (int i = 0; i < (int) nrecords; i++) 
	  {
	    if (debug) clog << "DEBUG: Counts = " << i << endl;
	    double p = 0.25 + 0.5 * (i % 2);
	    if (p < 0.5) 
	      {
		ss_data.make ();
		randomize_data (ss_data.get ());
		ss_data.get ().tree_dump (clog, "data_t", "<< ");
		writer.store (ss_data.get ());
	      }
	    else 
	      {
		ss_more_data.make ();
		randomize_more_data (ss_more_data.get ());
		ss_more_data.get ().tree_dump (clog, "more_data_t", "<< ");
		writer.store (ss_more_data.get ());
	      }
	  }
	if (test)
	  {
	    string test_str = "test-failure";
	    writer.store ("__TEST__", test_str);
	  }
	clog << "NOTICE: writing done." << endl << endl;
      }
    
      {
	clog << "NOTICE: reading..." << endl;

	datatools::serialization::safe_serial<data_t>      ss_data;
	datatools::serialization::safe_serial<more_data_t> ss_more_data;

	size_t counts = 0;
	datatools::serialization::data_reader reader (filename);    
	while (reader.has_record_tag ()) 
	  {
	    if (debug) clog << "DEBUG: read next record..." << endl;
	    if (reader.record_tag_is (data_t::SERIAL_TAG)) 
	      {
		if (debug) clog << "DEBUG: reading..." 
				<< data_t::SERIAL_TAG << endl;
		if (debug) clog << "DEBUG: making a new safe record..." 
				<< endl;
		ss_data.make ();
		if (debug) clog << "DEBUG: loading the new safe record..." 
				<< endl;
		reader.load (ss_data.get ());
		if (debug) clog << "DEBUG: loading done." 
				<< endl;
		ss_data.get ().tree_dump (clog, "data_t", ">> ");
	      }
	    else if (reader.record_tag_is (more_data_t::SERIAL_TAG)) 
	      {
		if (debug)clog << "DEBUG: reading..." 
			       << more_data_t::SERIAL_TAG 
			       << endl;
		if (debug) clog << "DEBUG: making a new safe record..." 
				<< endl;
		ss_more_data.make ();
		if (debug) clog << "DEBUG: loading the new safe record..." 
				<< endl;
		reader.load (ss_more_data.get ());
		if (debug) clog << "DEBUG: loading done." 
				<< endl;
		ss_more_data.get ().tree_dump (clog, "more_data_t", ">> ");
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
      cerr << "test_serialization: ERROR: " << x.what () << endl;
      exit (EXIT_FAILURE);
    }

  return (EXIT_SUCCESS);
}

/* end of test_serialization.cxx */
