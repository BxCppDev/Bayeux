// -*- mode: c++; -*- 
// test_serializable_2.cxx
// Author(s) : Francois Mauger <mauger@lpccaen.in2p3.fr>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <stdexcept>

#include <boost/cstdint.hpp>
#include <boost/serialization/nvp.hpp>

#include <datatools/io_factory.h>
#include <datatools/event_id.h>

using namespace std;

/// Some test class representing a hit (serializable)
class hit : public datatools::i_serializable
{
public:
  static int g_count;
  static bool g_debug;

private:
  int32_t   id_;
  double    tdc_;

  /** Macro for automatic generation of the serialization features */
  DATATOOLS_SERIALIZATION_DECLARATION()

  /* Macro to enable the use of an old serialization tag */
  DATATOOLS_SERIALIZATION_BACKWARD_SERIAL_TAG_SUPPORT() 

public:
  hit & set_id (int a_id)
  {
    id_ = a_id;
    return *this;
  }
  hit & set_tdc (double a_tdc)
  {
    tdc_ = a_tdc;
    return *this;
  }
  hit  () :
    id_ (-1) , tdc_ (-1.0)
  {
    g_count++;
    if (g_debug) clog << "DEBUG: hit::ctor: g_count=" << g_count << endl;
  }
  hit (int a_id, double a_tdc) :
    id_ (a_id) , tdc_ (a_tdc)
  {
    g_count++;
    if (g_debug) clog << "DEBUG: hit::ctor: g_count=" << g_count << endl;
  }
  ~hit () 
  {
    if (g_debug) clog << "DEBUG: hit::dtor: Destruction @" << this << "." << endl;
    g_count--;
    if (g_debug) clog << "DEBUG: hit::dtor: g_count=" << g_count << endl;
  }
  void print () const
  {
    cout << "hit @ " << this << " : {id=\"" << id_ << "\" , " << "tdc=" << tdc_ << '}'<< endl;
  }
};

int  hit::g_count  = 0;
bool hit::g_debug = false;

/** Macro for automatic implementation of the 'get_serial_tag' method signature */
DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(hit, "hit");

// Support for serialization tag :
DATATOOLS_SERIALIZATION_EXT_SERIAL_TAG_DECLARATION(hit);
DATATOOLS_SERIALIZATION_EXT_SERIAL_TAG_IMPLEMENTATION(hit, "hit");

// Support for old serialization tag :
DATATOOLS_SERIALIZATION_EXT_BACKWARD_SERIAL_TAG_DECLARATION(hit);
DATATOOLS_SERIALIZATION_EXT_BACKWARD_SERIAL_TAG_IMPLEMENTATION(hit, "old_hit");

template<class Archive>
void hit::serialize (Archive & ar_, 
                     const unsigned int version_)
{
  ar_ & boost::serialization::make_nvp ("id",  id_);
  ar_ & boost::serialization::make_nvp ("tdc", tdc_);
  return;
}

void test1 ()
{
  clog << endl << "Storing..." << endl;
  {
    hit h0 (0, 123);
    datatools::event_id evId (3, 666);
    clog << "Hit serial tag is '" << datatools::serial_tag<hit> () << "'" << endl;
    clog << "Hit old serial tag is '" << datatools::backward_serial_tag<hit> () << "'" << endl;
    clog << "Event ID serial tag is '" << evId.get_serial_tag () << "'" << endl;
    clog << "Event ID old serial tag is '" << datatools::backward_serial_tag<datatools::event_id> () << "'" << endl;

    ofstream foa ("test_serializable_2.txt");
    boost::archive::text_oarchive oa (foa);
    oa << datatools::backward_serial_tag<hit> () << h0;
    oa << datatools::backward_serial_tag<datatools::event_id> () << evId;
  }
  clog << "Done." << endl;
  
  clog << endl << "Loading..." << endl;
  {
    hit h0;
    datatools::event_id evId;
    ifstream fia ("test_serializable_2.txt");
    boost::archive::text_iarchive ia (fia);
    std::string serial_tag;
    ia >> serial_tag;
    if (serial_tag == datatools::backward_serial_tag<hit> () 
        || serial_tag == datatools::serial_tag<hit> () )
      {
        ia >> h0;
      }
    else
      {
        std::cerr << "ERROR: Unexpected 'hit' serial tag '" << serial_tag << "' !" << std::endl; 
        return;
      }
    h0.print ();
    ia >> serial_tag;
    if (serial_tag == datatools::backward_serial_tag<datatools::event_id> ()
        || serial_tag == evId.get_serial_tag ())
      {
        ia >> evId;
      }
    else
      {
        std::cerr << "ERROR: Unexpected 'event_id' serial tag '" << serial_tag << "' !" << std::endl; 
        return;
      }
    evId.tree_dump (std::clog, "Event ID : ");
   }
  clog << "Done." << endl;
  return;
}


void test2 ()
{
  clog << endl << "Storing..." << endl;
  {
    hit h0 (0, 123);
    datatools::event_id evId (3, 666);
    datatools::data_writer writer ("test_serializable_2b.txt");
    writer.store (datatools::backward_serial_tag<hit> (), h0);
    writer.store (datatools::backward_serial_tag<datatools::event_id> (), evId);
  }
  clog << "Done." << endl;
  
  clog << endl << "Loading..." << endl;
  {
    hit h0;
    datatools::event_id evId;
    datatools::data_reader reader ("test_serializable_2b.txt");
    try
      {
        reader.load (h0);
        reader.load (evId);
        h0.print ();
        evId.tree_dump (std::clog, "Event ID : ");
      }
    catch (std::exception & x)
      {
        clog << "error: " << x.what () << endl;
      }
   }
  clog << "Done." << endl;
  return;
}

int main (int argc_ , char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      clog << "Test of features from the 'boost::serialization' lib..." << endl; 
      bool debug = false;

      int iarg =  1;
      while (iarg < argc_) 
        {
          string arg = argv_[iarg];
          if ((arg == "-d") || (arg == "--debug")) debug = true;
          iarg++;
        }

      hit::g_debug = debug;

      test1 ();
      test2 ();
    }
  catch (exception & x)
    { 
      clog << "error: " << x.what () << endl; 
      error_code =  EXIT_FAILURE;
    }
  catch (...) 
    { 
      clog << "error: " << "unexpected error!" << endl;  
      error_code = EXIT_FAILURE; 
    } 
  return error_code;
} 

// end of test_serializable_2.cxx 

