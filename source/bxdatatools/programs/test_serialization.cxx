/* test_serialization.cxx */

#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <boost/filesystem/operations.hpp>

#include <datatools/serialization/io_factory.h>
#include <my_data.h>

void randomize_data( data_t & data_ )
{
  const data_t d((char)(32+drand48()*90),
		 (int) (drand48()*1000),
		 (double) drand48(),
		 1+(size_t) (drand48()*5));
  data_ = d;
}

void randomize_more_data( more_data_t & data_ )
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


int main( int argc_ , char ** argv_ ) 
{
  try {

    bool debug = false;
    long seed  = 12345;
    std::string filename = "";
    size_t nrecords = 3;
    bool step1=true;
    bool step2=true;
    
    int iarg=1;
    while ( iarg < argc_ ) {
      std::string arg=argv_[iarg];
      if ( arg[0] == '-' ) {
	if ( arg == "-d" ) debug=true;
	if ( arg == "-10" ) nrecords = 10;
	if ( arg == "-100" ) nrecords = 100;
	if ( arg == "-1000" ) nrecords = 1000;
	if ( arg == "-nostep1" ) step1 = false;
	if ( arg == "-nostep2" ) step2 = false;
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
      filename = "test_serialization.txt.gz";
    } 

    datatools::serialization::io_factory::g_debug = debug;
    srand48(seed);

    const std::string DATA_TAG("__data__");
    const std::string MORE_DATA_TAG("__more_data__");

    std::cerr << "NOTICE: using filename '" << filename << "'" << std::endl;

    int mode_guess;
    if ( datatools::serialization::io_factory::guess_mode_from_filename(filename,mode_guess) 
	 != datatools::serialization::io_factory::SUCCESS ) {
      std::ostringstream message;
      message << "Cannot guess mode for file '" << filename << "'!";
      throw std::runtime_error(message.str());
    }
    std::cerr << "mode = " << std::hex 
	      << mode_guess
	      << std::dec << std::endl;

    int mode_format=mode_guess;

    if ( step1 ) {
      {
	std::cerr << "writing..." << std::endl;

	if ( boost::filesystem::exists(filename)) {
	  std::ostringstream message;
	  message << "File '" << filename << "' already exists!";
	  throw std::runtime_error(message.str());
	}

	datatools::serialization::io_writer writer(filename,mode_format);
	if ( debug ) writer.dump(std::cerr);

	for ( int i=0; i<nrecords; i++ ) 
	  {
	    double p = 0.25+0.5*(i%2);
	    if ( p < 0.5 ) {
	      data_t d;
	      randomize_data(d);
	      d.tree_dump(std::cerr,"data_t","<< ");
	      writer.store(DATA_TAG);
	      writer.store(d);
	    }
	    else {
	      more_data_t md;
	      randomize_more_data(md);
	      md.tree_dump(std::cerr,"more_data_t","<< ");
	      writer.store(MORE_DATA_TAG);
	      writer.store(md);
	    }
	  }
	std::cerr << "writing done." << std::endl << std::endl;
      }

      {
	std::cerr << "reading..." << std::endl;
	if (  ! boost::filesystem::exists(filename)) {
	  std::ostringstream message;
	  message << "File '" << filename << "' does not exist!";
	  throw std::runtime_error(message.str());
	}
	datatools::serialization::io_reader reader(filename,mode_format);
	if ( debug ) reader.dump(std::cerr);
	size_t record_count=0;
	while ( true ) {
	  std::cerr << "read next record..." << std::endl;
	  try {
	    std::string tag_id;
	    reader.load(tag_id);
	    if ( tag_id == DATA_TAG ) {
	      data_t d;
	      reader.load(d);
	      d.tree_dump(std::cerr,"data_t",">> ");
	    }
	    else if ( tag_id == MORE_DATA_TAG ) {
	      more_data_t md;
	      reader.load(md);
	      md.tree_dump(std::cerr,"more_data_t",">> ");
	    }
	    else {
	      std::cerr << "WARNING: unknown tag '" << tag_id 
			<< "'" << std::endl;
	    }
	  }
	  catch (std::exception & x) {
	    std::cerr << "ERROR: " << x.what() << std::endl;
	    break;
	  }
	  record_count++;
	  std::cerr << "done." << std::endl;
	  std::cerr << "#record read = " << record_count << std::endl;
	}
	std::cerr << "reading done." << std::endl << std::endl;
      }
    }

    if ( step2 ) {
      {
	std::cerr << "writing(2)..." << std::endl;
	datatools::serialization::data_writer writer2(filename);
	for ( int i=0; i<nrecords; i++ ) 
	  {
	    std::cerr << "DEBUG: Counts = " << i << std::endl;
	    double p = 0.25+0.5*(i%2);
	    if ( p < 0.5 ) {
	      data_t d;
	      randomize_data(d);
	      d.tree_dump(std::cerr,"data_t","<< ");
	      writer2.store(DATA_TAG,d);
	    }
	    else {
	      more_data_t md;
	      randomize_more_data(md);
	      md.tree_dump(std::cerr,"more_data_t","<< ");
	      writer2.store(MORE_DATA_TAG,md);
	    }
	  }
	std::cerr << "writing(2) done." << std::endl << std::endl;
      }

      {
	std::cerr << "reading(2)..." << std::endl;
	size_t counts=0;
	datatools::serialization::data_reader reader2(filename);    
	while ( reader2.has_record_tag() ) {
	  std::cerr << "read next record(2)..." << std::endl;
	  if ( reader2.record_tag_is(DATA_TAG) ) {
	    data_t d;
	    reader2.load(d);
	    d.tree_dump(std::cerr,"data_t",">> ");
	  }
	  else if ( reader2.record_tag_is(MORE_DATA_TAG)) {
	    more_data_t md;
	    reader2.load(md);
	    md.tree_dump(std::cerr,"more_data_t",">> ");
	  }
	  else {
	    std::string bad_tag=reader2.get_record_tag();
            std::cerr << "ERROR: unknown data tag '" 
		      << bad_tag << "'!" << std::endl; 
	    /*
	    std::cerr << "ERROR: unknown data tag '" 
	    	      << reader2.get_record_tag() << "'!" <<
	    std::endl; 
	    */
	    break;
	  }
	  counts++;
	  std::cerr << "DEBUG: Counts = " << counts << std::endl;
	}
	std::cerr << "reading(2) done." << std::endl << std::endl;
      }   
    } 

  }
  catch (std::exception & x) {
    std::cerr << "test_serialization: ERROR: " << x.what() << std::endl;
    exit(EXIT_FAILURE);
  }

  return (EXIT_SUCCESS);
}

/* end of test_serialization.cxx */
