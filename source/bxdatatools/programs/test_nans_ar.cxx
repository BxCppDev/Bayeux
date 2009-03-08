/* test_nans_ar.cxx */

#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <boost/filesystem.hpp>

#include <datatools/serialization/serialization.h>

using namespace std;

class data_t : public datatools::serialization::i_serializable 
{
  double __v1, __v2;
  
public:

  //>>> from `i_serializable'
  static const std::string SERIAL_TAG;

  virtual const std::string & get_serial_tag () const;
  //<<<

  bool values_are_ok () const
  {
    return isnormal (__v1) &&  isnormal (__v2);
  }

  data_t ()
  {
    __v1 = drand48 () * 1.e-10;
    __v2 = drand48 () * 1.e+6;
  }

  data_t (double v1_, double v2_)
  {
    __v1 = v1_;
    __v2 = v2_;
  }

  void inf ()
  {
    __v1 = -std::numeric_limits<double>::infinity ();
    __v2 = std::numeric_limits<double>::infinity ();
  }

  void nan ()
  {
    __v1 = std::numeric_limits<double>::quiet_NaN ();
    __v2 = std::numeric_limits<double>::quiet_NaN ();
  }

  double get_v1 () const
  {
    return __v1;
  }

  double get_v2 () const
  {
    return __v2;
  }

private:
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
    ar & boost::serialization::make_nvp ("v1", __v1);
    ar & boost::serialization::make_nvp ("v2", __v2);
  }
};

ostream & operator<< (ostream & out_, const data_t & d_)
{
  out_ << d_.get_v1 () << ' ' << d_.get_v2 ();
  return out_;
}

const std::string data_t::SERIAL_TAG = "__DATA__";

const std::string & data_t::get_serial_tag () const
{
  return data_t::SERIAL_TAG;
}

int 
main (int argc_ , char ** argv_) 
{
  try {
    long seed = 314159;
    string filename = "test_nans_ar.txt";
    bool debug = false;
    namespace ds = datatools::serialization;

    ds::io_factory::g_debug = debug;
    srand48 (seed);
    clog << "NOTICE: using filename '" << filename << "'" << endl;
   
    if (boost::filesystem::exists (filename)) 
      {
	ostringstream message;
	message << "File '" << filename << "' already exists!";
	clog << message.str () << endl;
      }
    
    {
      clog << "NOTICE: writing..." << endl;
      ds::safe_serial<data_t> ss_data;
      ds::data_writer writer (filename, ds::using_multi_archives);
      int counts = 10;
      for (int i = 0; i < counts; i++) 
	{
	  ss_data.make ();
	  if (drand48 () < 0.25) ss_data.get ().nan ();
	  else if (drand48 () < 0.5) ss_data.get ().inf ();
	  clog << ss_data.get () << endl;
	  writer.store (ss_data.get ());
	}
      clog << "NOTICE: writing done." << endl << endl;
    }
    
    {
      clog << "NOTICE: reading..." << endl;
      ds::safe_serial<data_t> ss_data;
      ds::data_reader reader (filename, ds::using_multi_archives);    
      int counts = 0;
      while (reader.has_record_tag ()) 
	{
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
	      clog << ss_data.get () << endl;
	      if (debug) clog << "DEBUG: loading done." << endl;
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
      cerr << "test_nans_ar: ERROR: " << x.what () << endl;
      exit (EXIT_FAILURE);
    }
  return (EXIT_SUCCESS);
}

/* end of test_nans_ar.cxx */
