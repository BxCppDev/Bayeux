/* test_serialization_2.cxx */

#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <boost/filesystem.hpp>

#include <my_data.h>

#include <datatools/serialization/serialization.h>

int 
main (int argc_, char ** argv_) 
{
  try {

    bool debug = false;
    long seed  = 12345;
    std::string filename = "";
    size_t nrecords = 3;
    bool append = false;

    enum format_t
      {
	FORMAT_TXT = 0, 
	FORMAT_XML = 1, 
	FORMAT_BIN = 2
      };
    int fmt = FORMAT_XML;

    int iarg = 1;
    while (iarg < argc_) 
      {
	std::string arg = argv_[iarg];
	if (arg[0] == '-') 
	  {
	    if (arg == "-d")    debug    = true;
	    if (arg == "-10")   nrecords = 10;
	    if (arg == "-100")  nrecords = 100;
	    if (arg == "-1000") nrecords = 1000;
	    if (arg == "-xml")  fmt = FORMAT_XML;
	    if (arg == "-txt")  fmt = FORMAT_TXT;
	    if (arg == "-bin")  fmt = FORMAT_BIN;
	    if (arg == "-a")    append = true;
	  }
	else 
	  {
	    if (filename.empty ()) 
	      {
		filename = arg;
	      }
	    else 
	      {
		std::istringstream iss (arg);
		iss >> seed;
		if (! iss) 
		  {
		    std::ostringstream message;
		    message << "format error for 'seed'!"; 
		    throw std::runtime_error (message.str ());
		  }
	      }
	  }
	iarg++;
      }

    if (filename.empty ()) 
      {
	if (fmt == FORMAT_XML) filename = "test_serialization_2.xml";
	if (fmt == FORMAT_TXT) filename = "test_serialization_2.txt";
	if (fmt == FORMAT_BIN) filename = "test_serialization_2.data";
      } 
    
    namespace ds = datatools::serialization;

    ds::io_factory::g_debug = debug;
    srand48 (seed);
    std::clog << "NOTICE: using filename '" << filename << "'" << std::endl;

    int mode_guess;
    if (ds::io_factory::guess_mode_from_filename (filename, mode_guess) 
	!= ds::io_factory::SUCCESS) 
      {
	std::ostringstream message;
	message << "Cannot guess mode for file '" << filename << "'!";
	throw std::runtime_error (message.str ());
      }
    std::clog << "mode = " << std::hex 
	      << mode_guess
	      << std::dec << std::endl;
   
    if (boost::filesystem::exists (filename)) 
      {
	if (append) 
	  {
	    std::clog << "WARNING: Append to existing file '" << filename << "'!"
		      << std::endl;
	    /*
	      std::ostringstream message;
	      message << "File '" << filename << "' already exists!";
	      throw std::runtime_error (message.str ());
	    */
	  }
      }
    
    {
      std::clog << "NOTICE: writing..." << std::endl;
      bool append_mode = ds::no_append_mode;
      if (append) append_mode |= ds::append_mode;
      ds::data_writer writer (filename, ds::using_multi_archives, append_mode);

      for (int i = 0; i < (int) nrecords; i++) 
	{
	  if (debug) std::clog << "DEBUG: Counts = " << i << std::endl;
	  double p = 0.25 + 0.5 * (i % 2);
	  if (p < 0.5) 
	    {
	      data_t d;
	      randomize_data (d);
	      d.tree_dump (std::clog, "data_t", "<< ");
	      writer.store (d);
	    }
	  else 
	    {
	      more_data_t md;
	      randomize_more_data (md);
	      md.tree_dump (std::clog, "more_data_t", "<< ");
	      writer.store (md);
	    }
	}
      std::clog << "NOTICE: writing done." << std::endl << std::endl;
    }

    bool read = true;
    if (read)
      {
	std::clog << "NOTICE: reading..." << std::endl;

	size_t counts = 0;
	ds::data_reader reader (filename, ds::using_multi_archives);    
	while (reader.has_record_tag ()) 
	  {
	    if (debug) std::clog << "DEBUG: read next record..." << std::endl;
	    if (reader.record_tag_is (data_t::SERIAL_TAG)) 
	      {
		if (debug) std::clog << "DEBUG: reading..." 
				     << data_t::SERIAL_TAG << std::endl;
		data_t d;
		reader.load (d);
		d.tree_dump (std::clog, "data_t", ">> ");
	      }
	    else if (reader.record_tag_is (more_data_t::SERIAL_TAG)) 
	      {
		if (debug)std::clog << "DEBUG: reading..." 
				     << more_data_t::SERIAL_TAG 
				     << std::endl;
		more_data_t md;
		reader.load (md);
		md.tree_dump (std::clog, "more_data_t", ">> ");
	      }
	    else 
	      {
		std::string bad_tag = reader.get_record_tag ();
		std::clog << "ERROR: unknown data tag '" 
			  << bad_tag << "'!" << std::endl; 
		break;
	      }
	    counts++;
	    if (debug) std::clog << "DEBUG: Counts = " << counts << std::endl;
	  }
	std::clog << "NOTICE: reading done." << std::endl << std::endl;   
      } 

  }
  catch (std::exception & x) 
    {
      std::cerr << "test_serialization_2: ERROR: " << x.what () << std::endl;
      exit (EXIT_FAILURE);
    }

  return (EXIT_SUCCESS);
}

/* end of test_serialization_2.cxx */
