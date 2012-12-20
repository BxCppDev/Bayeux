// -*- mode: c++; -*- 
// test_serializable_1.cxx
// Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <stdexcept>

#include <boost/cstdint.hpp>
#include <boost/serialization/nvp.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>

#include <datatools/i_serializable.h>
#include <datatools/serialization_macros.h>

using namespace std;

/** Some test class representing a hit (serializable)
 *
 */
class hit : public datatools::i_serializable
{
public:
  static int g_count;
  static bool g_debug;

  /** Macro for automatic generation of the 'get_serial_tag' method signature */
  DATATOOLS_SERIALIZATION_SERIAL_TAG_DECLARATION();

private:
  int32_t   id_;
  double    tdc_;
private:
  friend class boost::serialization::access; 
  /** Macro for automatic generation of the 'serialize' method signature */
  BOOST_SERIALIZATION_SERIALIZE_DECLARATION();

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

template<class Archive>
void hit::serialize (Archive & ar_, 
                     const unsigned int version_)
{
  ar_ & boost::serialization::make_nvp ("id",  id_);
  ar_ & boost::serialization::make_nvp ("tdc", tdc_);
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

      if (hit::g_debug) clog << "DEBUG: g_count=" << hit::g_count << endl;

 
      hit h0 (0, 123);
      clog << "Hit serial tag is '" << h0.get_serial_tag () << "'" << endl;
      clog << endl << "Serialize..." << endl;
      {
        ofstream foa ("test_serializable_1.txt");
        boost::archive::text_oarchive oa (foa);
        oa << h0.get_serial_tag () << h0;
      }
      clog << "Done." << endl;

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

// end of test_serializable_1.cxx 

