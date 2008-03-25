/* test_serialization.cxx */

#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <boost/filesystem/operations.hpp>

#include <my_data.h>
#include <datatools/serialization/serialization.h>

void 
randomize_data(data_t & data_)
{
  const data_t d((char)(32+drand48()*90),
		 (int) (drand48()*1000),
		 (double) drand48(),
		 1+(size_t) (drand48()*5));
  data_ = d;
}

void 
randomize_more_data(more_data_t & data_)
{
  std::ostringstream name_oss;
  name_oss << "data_" << (int) (drand48()*1000);
  const more_data_t md((char)(32+drand48()*90),
		       (int) (drand48()*1000),
		       (double) drand48(),
		       1+(size_t) (drand48()*5),
		       name_oss.str());
  data_ = md;
}


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
    bool test = false;
    int iarg=1;
    while ( iarg < argc_ ) {
      std::string arg=argv_[iarg];
      if ( arg[0] == '-' ) {
	if ( arg == "-d" ) debug=true;
	if ( arg == "-10" ) nrecords = 10;
	if ( arg == "-100" ) nrecords = 100;
	if ( arg == "-1000" ) nrecords = 1000;
	if ( arg == "-xml" ) fmt=FORMAT_XML;
	if ( arg == "-txt" ) fmt=FORMAT_TXT;
	if ( arg == "-test" ) test=true;
      }
      else {
	if ( filename.empty() ) {
	  filename = arg;
	}
	else {
	  std::istringstream iss(arg);
	  iss >> seed;
	  if ( ! iss ) {
	    std::ostringstream message;
	    message << "format error for 'seed'!"; 
	    throw std::runtime_error(message.str());
	  }
	}
      }
      iarg++;
    }

    if ( filename.empty() ) {
      if (fmt == FORMAT_XML) filename = "test_serialization.xml";
      if (fmt == FORMAT_TXT) filename = "test_serialization.txt";
    } 

    datatools::serialization::io_factory::g_debug = debug;
    srand48(seed);
    std::cerr << "NOTICE: using filename '" << filename << "'" << std::endl;

    int mode_guess;
    if (datatools::serialization::\
	io_factory::guess_mode_from_filename(filename,mode_guess) 
	!= datatools::serialization::io_factory::SUCCESS ) {
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
      datatools::serialization::data_writer writer(filename);

      // safe_serial
      datatools::serialization::safe_serial<data_t>      ss_data;
      datatools::serialization::safe_serial<more_data_t> ss_more_data;

      for ( int i=0; i<nrecords; i++ ) 
	{
	  if (debug) std::cerr << "DEBUG: Counts = " << i << std::endl;
	  double p = 0.25+0.5*(i%2);
	  if ( p < 0.5 ) 
	    {
	      ss_data.make();
	      randomize_data(ss_data.get());
	      ss_data.get().tree_dump(std::cerr,"data_t","<< ");
	      writer.store(ss_data.get());
	    }
	  else 
	    {
	      ss_more_data.make();
	      randomize_more_data(ss_more_data.get());
	      ss_more_data.get().tree_dump(std::cerr,"more_data_t","<< ");
	      writer.store(ss_more_data.get());
	    }
	}
      if (test)
	{
	  std::string test_str="test-failure";
	  writer.store("__TEST__", test_str);
	}
      std::cerr << "NOTICE: writing done." << std::endl << std::endl;
    }
    
    {
      std::cerr << "NOTICE: reading..." << std::endl;

      datatools::serialization::safe_serial<data_t>      ss_data;
      datatools::serialization::safe_serial<more_data_t> ss_more_data;

      size_t counts=0;
      datatools::serialization::data_reader reader(filename);    
      while (reader.has_record_tag() ) 
	{
	  if (debug) std::cerr << "DEBUG: read next record..." << std::endl;
	  if (reader.record_tag_is(data_t::SERIAL_TAG)) 
	    {
	      if (debug) std::cerr << "DEBUG: reading..." 
				   << data_t::SERIAL_TAG << std::endl;
	      if (debug) std::cerr << "DEBUG: making a new safe record..." 
				   << std::endl;
	      ss_data.make();
	      if (debug) std::cerr << "DEBUG: loading the new safe record..." 
				   << std::endl;
	      reader.load(ss_data.get());
	      if (debug) std::cerr << "DEBUG: loading done." 
				   << std::endl;
	      ss_data.get().tree_dump(std::cerr,"data_t",">> ");
	    }
	  else if (reader.record_tag_is(more_data_t::SERIAL_TAG)) 
	    {
	      if (debug )std::cerr << "DEBUG: reading..." 
				   << more_data_t::SERIAL_TAG 
				   << std::endl;
	      if (debug) std::cerr << "DEBUG: making a new safe record..." 
				   << std::endl;
	      ss_more_data.make();
	      if (debug) std::cerr << "DEBUG: loading the new safe record..." 
				   << std::endl;
	      reader.load(ss_more_data.get());
	      if (debug) std::cerr << "DEBUG: loading done." 
				   << std::endl;
	      ss_more_data.get().tree_dump(std::cerr,"more_data_t",">> ");
	    }
	  else 
	    {
	      std::string bad_tag=reader.get_record_tag();
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
    std::cerr << "test_serialization: ERROR: " << x.what() << std::endl;
    exit(EXIT_FAILURE);
  }

  return (EXIT_SUCCESS);
}

/* end of test_serialization.cxx */
