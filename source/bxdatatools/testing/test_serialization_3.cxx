/* test_serialization_3.cxx */

#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
 
#include <boost/filesystem.hpp>

#include <datatools/serialization/utils.h>
#include <datatools/serialization/archives_instantiation.h>
#include <datatools/serialization/io_factory.h>
#include <datatools/serialization/i_serializable.h>
#include <datatools/serialization/safe_serial.h>

#include <datatools/serialization/i_serializable.ipp>
#include <boost/serialization/export.hpp>

using namespace std;

class data_t : public datatools::serialization::i_serializable     
{
public:
  static const string SERIAL_TAG;
  virtual const string & get_serial_tag () const;

public:
  int value;

private:
  friend class boost::serialization::access; 
  BOOST_SERIALIZATION_SERIALIZE_DECLARATION()

};

const string data_t::SERIAL_TAG = "data_t";

const string & data_t::get_serial_tag () const
{
  return data_t::SERIAL_TAG;
}

template<class Archive>
void data_t::serialize (Archive & ar_, 
			const unsigned int version_)
{
  ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
  ar_ & boost::serialization::make_nvp ("value", value);
  return;
}

// No need for registration here :
// BOOST_CLASS_EXPORT_KEY2 (data_t, "data_t")
// BOOST_CLASS_EXPORT_IMPLEMENT (data_t)
// DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(data_t)

int main (int argc_, char ** argv_) 
{
  try 
    {
      bool debug = false;
      string filename = "";
      size_t nrecords = 3;

      enum format_t
	{
	  FORMAT_TXT = 0, 
	  FORMAT_XML = 1, 
	  FORMAT_BIN = 2
	};
      bool test = false;

      int iarg = 1;
      while (iarg < argc_) 
	{
	  string arg = argv_[iarg];
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
	  filename = "test_serialization_3.xml";
	} 

      datatools::serialization::io_factory::g_debug = debug;
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
	  clog << "WARNING: " << message.str () << " Remove it !" << endl; 
	  unlink (filename.c_str ());
	}
    
      {
	clog << "NOTICE: writing..." << endl;
	datatools::serialization::data_writer writer (filename);

	// safe_serial
	datatools::serialization::safe_serial<data_t> ss_data;

	for (int i = 0; i < (int) nrecords; i++) 
	  {
	    if (debug) clog << "DEBUG: Counts = " << i << endl;
	    ss_data.make ();
	    ss_data.get ().value = i;
	    clog << "Value = '" << ss_data.get ().value << "'" << endl;
	    writer.store (ss_data.get ());
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

	datatools::serialization::safe_serial<data_t> ss_data;

	size_t counts = 0;
	datatools::serialization::data_reader reader (filename); 
	reader.dump (clog);

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
		clog << "DEBUG: Value=" << ss_data.get ().value << endl;
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

/* end of test_serialization_3.cxx */
