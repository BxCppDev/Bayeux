/* test_serialization_3.cxx */

#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <boost/filesystem.hpp>

#include <datatools/serialization/serialization.h>

class data_t : public datatools::serialization::i_serializable     
{
 public:
  static const std::string SERIAL_TAG;
  virtual const std::string & get_serial_tag () const;

 public:
  int value;

 private:
  friend class boost::serialization::access; 
  template<class Archive>
  void serialize (Archive & ar_, 
		  const unsigned int version_)
  {
    ar_ & boost::serialization::make_nvp ("value", value);
  }

};

const std::string data_t::SERIAL_TAG = "__DATA__";

const std::string & 
data_t::get_serial_tag () const
{
  return data_t::SERIAL_TAG;
}

int 
main (int argc_, char ** argv_) 
{
  try 
    {

    bool debug = false;
    long seed  = 12345;
    std::string filename = "";
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
	std::string arg = argv_[iarg];
	if (arg[0] == '-') 
	  {
	    if (arg == "-d") debug = true;
	    if (arg == "-test") test = true;
	  }
	else 
	  {
	    if (filename.empty ()) 
	      {
		filename = arg;
	      }
	  }
	iarg++;
      }

    if (filename.empty ()) 
      {
	filename = "test_serialization_3.txt";
      } 

    datatools::serialization::io_factory::g_debug = debug;
    std::clog << "NOTICE: using filename '" << filename << "'" << std::endl;

    int mode_guess;
    if (datatools::serialization::\
	io_factory::guess_mode_from_filename (filename, mode_guess) 
	!= datatools::serialization::io_factory::SUCCESS) 
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
	std::ostringstream message;
	message << "File '" << filename << "' already exists!";
	throw std::runtime_error (message.str ());
      }
    
    {
      std::clog << "NOTICE: writing..." << std::endl;
      datatools::serialization::data_writer writer (filename);

      // safe_serial
      datatools::serialization::safe_serial<data_t> ss_data;

      for (int i = 0; i < nrecords; i++) 
	{
	  if (debug) std::clog << "DEBUG: Counts = " << i << std::endl;
	  ss_data.make ();
	  ss_data.get ().value = i;
	  std::clog << "Value = '" << ss_data.get ().value << "'" << std::endl;
	  writer.store (ss_data.get ());
	}
      if (test)
	{
	  std::string test_str = "test-failure";
	  writer.store ("__TEST__", test_str);
	}
      std::clog << "NOTICE: writing done." << std::endl << std::endl;
    }
    
    {
      std::clog << "NOTICE: reading..." << std::endl;

      datatools::serialization::safe_serial<data_t> ss_data;

      size_t counts = 0;
      datatools::serialization::data_reader reader (filename);    
      while (reader.has_record_tag ()) 
	{
	  if (debug) std::clog << "DEBUG: read next record..." << std::endl;
	  if (reader.record_tag_is (data_t::SERIAL_TAG)) 
	    {
	      if (debug) std::clog << "DEBUG: reading..." 
				   << data_t::SERIAL_TAG << std::endl;
	      if (debug) std::clog << "DEBUG: making a new safe record..." 
				   << std::endl;
	      ss_data.make ();
	      if (debug) std::clog << "DEBUG: loading the new safe record..." 
				   << std::endl;
	      reader.load (ss_data.get ());
	      if (debug) std::clog << "DEBUG: loading done." 
				   << std::endl;
	      std::clog << "DEBUG: Value=" << ss_data.get ().value << std::endl;
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
      std::cerr << "test_serialization: ERROR: " << x.what () << std::endl;
      exit (EXIT_FAILURE);
    }

  return (EXIT_SUCCESS);
}

/* end of test_serialization_3.cxx */
