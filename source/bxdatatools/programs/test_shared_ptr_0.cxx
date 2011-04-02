// -*- mode: c++; -*- 
// test_shared_ptr_0.cxx
// Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>

#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include <datatools/serialization/utils.h>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>

using namespace std;

class hit 
{
private:
  int32_t   id_;
  double    tdc_;
private:
  friend class boost::serialization::access; 
  BOOST_SERIALIZATION_SERIALIZE_DECLARATION()
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
  }
  hit (int a_id, double a_tdc) :
    id_ (a_id) , tdc_ (a_tdc)
  {
  }
  ~hit () 
  {
    clog << "DEBUG: hit::dtor: Destruction @" << this << "." << endl;
  }
  void print () const
  {
    cout << "hit @ " << this << " : {id=\"" << id_ << "\" , " << "tdc=" << tdc_ << '}'<< endl;
  }
};

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
      clog << "Test of the 'boost::shared_ptr<>' class..." << endl; 
      bool debug = false;

      int iarg =  1;
      while (iarg < argc_) 
	{
          string arg = argv_[iarg];
	  if ((arg == "-d") || (arg == "--debug")) debug = true;
	  iarg++;
	}
    
      {
	clog << endl << "Test 1: " << endl;
	hit h0 (0, 123);
	hit h1 (1, 456);
	hit h2 (2, 789);
	h0.print ();
	h1.print ();
	h2.print ();
      }

      {
	typedef boost::shared_ptr<hit> hit_ptr;
	typedef vector<hit_ptr> hits_col_t;
	// see: 
	// http://www.boost.org/doc/libs/1_43_0/libs/smart_ptr/example/shared_ptr_example.cpp
	clog << endl << "Test 2: " << endl;

	{
	  hits_col_t hits;
	  hit_ptr a_hit (new hit (0, 123));
	  hits.push_back (a_hit);
	  a_hit.reset (new hit (1, 456));
	  hits.push_back (a_hit);
	  a_hit.reset (new hit (2, 789));
	  hits.push_back (a_hit);
	  a_hit.reset (new hit (3, 321));
	  hits.push_back (a_hit);
	  a_hit.reset (new hit (4, 654));
	  hits.push_back (a_hit);
	  a_hit.reset (new hit (5, 987));
	  hits.push_back (a_hit);
	  clog << "Hits : " << endl;
	  for (hits_col_t::const_iterator i = hits.begin ();
	       i != hits.end ();
	       i++)
	    {
	      i->get ()->print ();
	    }

	  hits_col_t hits2;
	  hits2.push_back (hits[1]);
	  hits2.push_back (hits[3]);
	  hits2.push_back (hits[4]);
	  clog << "Hits2 : " << endl;
	  for (hits_col_t::const_iterator i = hits2.begin ();
	       i != hits2.end ();
	       i++)
	    {
	      i->get ()->print ();
	    }

	  clog << "Erase +0 : " << endl;
	  hits.erase (hits.begin ());
	  clog << "Erase +2 : " << endl;
	  hits.erase (hits.begin () + 2);
	  clog << "Erase +3 : " << endl;
	  hits.erase (hits.begin () + 3);
	  clog << "Add : " << endl;
	  hits.push_back (hits2[0]);
	  clog << "Hits(bis) : " << endl;
	  for (hits_col_t::const_iterator i = hits.begin ();
	       i != hits.end ();
	       i++)
	    {
	      i->get ()->print ();
	    }

	  clog << endl << "Serialize..." << endl;
	  {
	    ofstream foa ("test_shared_ptr_0.txt");
	    boost::archive::text_oarchive oa (foa);
	    oa << hits << hits2;
	  }
	  {
	    ofstream foa ("test_shared_ptr_0.xml");
	    boost::archive::xml_oarchive oa (foa);
	    oa << boost::serialization::make_nvp ("hits", hits);
	    oa << boost::serialization::make_nvp ("hits2", hits2);
	  }
	  clog << "Done." << endl;

	  clog << "Destroy 'hits'..." << endl;
	  hits.clear ();
	  clog << "Destroy 'hits2'..." << endl;
	  hits2.clear ();
	  clog << "Done." << endl;
	}

	{	
	  hits_col_t hits;
	  hits_col_t hits2;

	  {
	    clog << endl << "Deserialize (from text archive)..." << endl;
	    ifstream fia ("test_shared_ptr_0.txt");
	    boost::archive::text_iarchive ia (fia);
	    ia >> hits >> hits2;
	    clog << "Done." << endl;
	    clog << "Hits (loaded) : " << endl;
	    for (hits_col_t::const_iterator i = hits.begin ();
		 i != hits.end ();
		 i++)
	      {
		i->get ()->print ();
	      }
	    clog << "Hits2 (loaded) : " << endl;
	    for (hits_col_t::const_iterator i = hits2.begin ();
		 i != hits2.end ();
		 i++)
	      {
		i->get ()->print ();
	      }
	  }

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

// end of test_shared_ptr_0.cxx 
  
