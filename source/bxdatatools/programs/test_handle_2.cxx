// -*- mode: c++; -*- 
// test_handle_2.cxx
// Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>

#include <boost/cstdint.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include <datatools/serialization/utils.h>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>

#include <datatools/utils/handle.h>
#include <datatools/utils/handle_pool.h>
#include <datatools/utils/time_tools.h>

using namespace std;

/** Some test class representing a hit (serializable)
 *
 */
class hit 
{
public:
  static int g_count;
  static bool g_debug;
private:
  int32_t   id_;
  double    tdc_;
private:
  friend class boost::serialization::access; 
  BOOST_SERIALIZATION_SERIALIZE_DECLARATION()

public:
  int32_t get_id () const
  {
    return id_;
  }

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

// define a predicate for the 'hit' class :
struct hit_is_invalid_predicate : public datatools::utils::i_predicate <hit>
{
  bool operator () (const hit & a_hit) const
  {
    return (a_hit.get_id () < 0);
  }
};

int  hit::g_count  = 0;
bool hit::g_debug = false;

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
      clog << "Test of the 'handle<>' template class..." << endl; 
      bool debug = false;

      int iarg =  1;
      while (iarg < argc_) 
	{
          string arg = argv_[iarg];
	  if ((arg == "-d") || (arg == "--debug")) debug = true;
	  iarg++;
	}

      using namespace datatools::utils;

      typedef handle<hit> hit_handle_t;
      typedef vector<hit_handle_t> hit_handles_col_t;
      typedef vector<hit> hits_col_t;
 
      {
       	clog << endl << "Test 1: " << endl;
	{
	  hit_handles_col_t hits;
	  size_t nevents = 100;
	  size_t nhits = 100;
	  computing_time CT;
	  computing_time CT2;
	  ofstream foa ("test_handle_2a.txt");
	  hits.reserve (nhits);
	  for (int i = 0; i < nevents; i++)
	    {
	      if ((i % 100) == 0) clog << "#i=" << i << endl;
	      CT2.start ();
	      for (int j = 0; j < nhits; j++)
		{
		  hit_handle_t hh (new hit (j, j * 10. ));
		  hits.push_back (hh);
		}
	      CT2.stop ();
	      {
		CT.start ();
		boost::archive::text_oarchive oa (foa);
		oa << hits;
		CT.stop ();
	      }
	      hits.clear ();
	    }
	  CT.tree_dump (clog, "CT hit_handles_col_t"); // 33 sec
	  CT2.tree_dump (clog, "CT2 hit_handles_col_t"); //  1.36152
	}
      }
     
      {
       	clog << endl << "Test 2: " << endl;
	{
	  hits_col_t hits;
	  size_t nevents = 100;
	  size_t nhits = 100;
	  computing_time CT;
	  computing_time CT2;
	  ofstream foa ("test_handle_2b.txt");
	  hits.reserve (nhits);
	  for (int i = 0; i < nevents; i++)
	    {
	      if ((i % 100) == 0) clog << "#i=" << i << endl;
	      CT2.start ();
	      for (int j = 0; j < nhits; j++)
		{
		  hits.push_back (hit (j, j * 10. ));
		}
	      CT2.stop ();
	      {
		CT.start ();
		boost::archive::text_oarchive oa (foa);
		oa << hits;
		CT.stop ();
	      }
	      hits.clear ();
	    }
	  CT.tree_dump (clog, "CT hit_col_t"); // 23 sec
	  CT2.tree_dump (clog, "CT2 hit_col_t"); // 0.174498
	}
      }

      {
       	clog << endl << "Test 3: " << endl;
	{
	  hit_handles_col_t hits;
	  size_t nevents = 100;
	  size_t nhits = 100;
	  handle_pool<hit> pool (nhits);
	  computing_time CT;
	  computing_time CT2;
	  ofstream foa ("test_handle_2c.txt");
	  hits.reserve (nhits);
	  for (int i = 0; i < nevents; i++)
	    {
	      if ((i % 100) == 0) clog << "#i=" << i << endl;
	      CT2.start ();
	      for (int j = 0; j < nhits; j++)
		{
		  hits.push_back (pool.create ());
		  hits.back ().get ().set_id (j).set_tdc (j * 10.);
		}
	      CT2.stop ();
	      {
		CT.start ();
		boost::archive::text_oarchive oa (foa);
		oa << hits;
		CT.stop ();
	      }
	      hits.clear ();
	      pool.reset ();
	    }
	  CT.tree_dump (clog, "CT hit_handles_col_t (using pool)"); // 32 sec
	  CT2.tree_dump (clog, "CT2 hit_handles_col_t (using pool)"); // 0.393747
	}
      }
     
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
 
// end of test_handle_2.cxx 

