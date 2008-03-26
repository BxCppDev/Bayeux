/* test_serialization_2.cxx */

#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <boost/filesystem/operations.hpp>

#include <my_data.h>

#include <datatools/serialization/serialization.h>

int 
main( int argc_ , char ** argv_ ) 
{
  try {

    bool debug = false;
    long seed  = 12345;
    std::string filename = "";
    size_t nrecords = 3;

    enum format_t
      {
	FORMAT_TXT=0,
	FORMAT_XML=1,
      };
    int fmt   = FORMAT_XML;

    int iarg=1;
    while (iarg < argc_) 
      {
	std::string arg=argv_[iarg];
	if (arg[0] == '-') 
	  {
	    if ( arg == "-d" ) debug=true;
	    if ( arg == "-10" ) nrecords = 10;
	    if ( arg == "-100" ) nrecords = 100;
	    if ( arg == "-1000" ) nrecords = 1000;
	    if ( arg == "-xml" ) fmt=FORMAT_XML;
	    if ( arg == "-txt" ) fmt=FORMAT_TXT;
	  }
	else 
	  {
	    if ( filename.empty() ) 
	      {
		filename = arg;
	      }
	    else 
	      {
		std::istringstream iss(arg);
		iss >> seed;
		if (! iss) 
		  {
		    std::ostringstream message;
		    message << "format error for 'seed'!"; 
		    throw std::runtime_error(message.str());
		  }
	      }
	  }
	iarg++;
      }

    if ( filename.empty() ) 
      {
	if (fmt == FORMAT_XML) filename = "test_serialization_2.xml";
	if (fmt == FORMAT_TXT) filename = "test_serialization_2.txt";
      } 
    
    namespace ds=datatools::serialization;

    ds::io_factory::g_debug = debug;
    srand48(seed);
    std::cerr << "NOTICE: using filename '" << filename << "'" << std::endl;

    int mode_guess;
    if (ds::io_factory::guess_mode_from_filename(filename,mode_guess) 
	!= ds::io_factory::SUCCESS ) 
      {
	std::ostringstream message;
	message << "Cannot guess mode for file '" << filename << "'!";
	throw std::runtime_error(message.str());
      }
    std::cerr << "mode = " << std::hex 
	      << mode_guess
	      << std::dec << std::endl;
    
    if ( boost::filesystem::exists(filename)) 
      {
	std::ostringstream message;
	message << "File '" << filename << "' already exists!";
	throw std::runtime_error(message.str());
      }
    
    {
      std::cerr << "NOTICE: writing..." << std::endl;
      ds::data_writer writer(filename,ds::multi_archives);

      for ( int i=0; i<nrecords; i++ ) 
	{
	  if (debug) std::cerr << "DEBUG: Counts = " << i << std::endl;
	  double p = 0.25+0.5*(i%2);
	  if ( p < 0.5 ) 
	    {
	      data_t d;
	      randomize_data(d);
	      d.tree_dump(std::cerr,"data_t","<< ");
	      writer.store(d);
	    }
	  else 
	    {
	      more_data_t md;
	      randomize_more_data(md);
	      md.tree_dump(std::cerr,"more_data_t","<< ");
	      writer.store(md);
	    }
	}
      std::cerr << "NOTICE: writing done." << std::endl << std::endl;
    }

    bool read = true;
    if (read)
      {
      std::cerr << "NOTICE: reading..." << std::endl;

      size_t counts=0;
      ds::data_reader reader(filename,ds::multi_archives);    
      while (reader.has_record_tag() ) 
	{
	  if (debug) std::cerr << "DEBUG: read next record..." << std::endl;
	  if (reader.record_tag_is(data_t::SERIAL_TAG)) 
	    {
	      if (debug) std::cerr << "DEBUG: reading..." 
				   << data_t::SERIAL_TAG << std::endl;
	      data_t d;
	      reader.load(d);
	      d.tree_dump(std::cerr,"data_t",">> ");
	    }
	  else if (reader.record_tag_is(more_data_t::SERIAL_TAG)) 
	    {
	      if (debug )std::cerr << "DEBUG: reading..." 
				   << more_data_t::SERIAL_TAG 
				   << std::endl;
	      more_data_t md;
	      reader.load(md);
	      md.tree_dump(std::cerr,"more_data_t",">> ");
	    }
	  else 
	    {
	      std::string bad_tag = reader.get_record_tag();
	      std::cerr << "ERROR: unknown data tag '" 
			<< bad_tag << "'!" << std::endl; 
	      break;
	    }
	  counts++;
	  if (debug) std::cerr << "DEBUG: Counts = " << counts << std::endl;
	}
      std::cerr << "NOTICE: reading done." << std::endl << std::endl;   
    } 

  }
  catch (std::exception & x) {
    std::cerr << "test_serialization_2: ERROR: " << x.what() << std::endl;
    exit(EXIT_FAILURE);
  }

  return (EXIT_SUCCESS);
}

/* end of test_serialization_2.cxx */
