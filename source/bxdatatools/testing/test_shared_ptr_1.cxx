// -*- mode: c++; -*-
// test_shared_ptr_1.cxx
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

#include <datatools/serialization_macros.h>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>

using namespace std;


template <class T>
class handle
{
  boost::shared_ptr<T> sp_;

public:

  handle (T * a_t_ptr = 0) : sp_ (a_t_ptr != 0 ? a_t_ptr : new T)
  {
    return;
  }

  const T & get () const
  {
    return *sp_.get ();
  }

  T & get ()
  {
    return *sp_.get ();
  }

  T & reset (T * a_t_ptr = 0)
  {
    sp_.reset (a_t_ptr != 0 ? a_t_ptr : new T);
    return *sp_.get ();
  }

private:
  friend class boost::serialization::access;
  template <class Archive>
  void serialize (Archive & ar, int /*version*/)
  {
    ar & boost::serialization::make_nvp ("sp", sp_);
    return;
  }
};

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

int hit::g_count  = 0;
bool hit::g_debug = false;

template<class Archive>
void hit::serialize (Archive & ar_,
                     const unsigned int /*version_*/)
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

      hit::g_debug = debug;

      if (hit::g_debug) clog << "DEBUG: g_count=" << hit::g_count << endl;

      {
        typedef handle<hit> hit_handle_t;
        typedef vector<hit_handle_t> hit_handles_col_t;
        clog << endl << "Test 1: " << endl;

        {
          hit_handles_col_t hits;

          hit_handle_t hh (new hit (0, 123));
          hits.push_back (hh);

          hh.reset (new hit (1, 456));
          hits.push_back (hh);

          hh.reset (new hit (2, 789));
          hits.push_back (hh);

          hh.reset (new hit (3, 321));
          hits.push_back (hh);

          hh.reset (new hit (4, 654));
          hits.push_back (hh);

          hh.reset (new hit (5, 987));
          hits.push_back (hh);

          hh.reset ();
          hits.push_back (hh);

          hits.push_back (hit_handle_t (new hit (20, 20000)));

          hits[3].get ().set_id (100).set_tdc (10000);

          clog << "Hits : " << endl;
          for (hit_handles_col_t::const_iterator i = hits.begin ();
               i != hits.end ();
               i++)
            {
              i->get ().print ();
            }

          hit_handles_col_t hits2;
          hits2.push_back (hits[1]);
          hits2.push_back (hits[3]);
          hits2.push_back (hits[4]);
          clog << "Hits2 : " << endl;
          for (hit_handles_col_t::const_iterator i = hits2.begin ();
               i != hits2.end ();
               i++)
            {
              i->get ().print ();
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
          for (hit_handles_col_t::const_iterator i = hits.begin ();
               i != hits.end ();
               i++)
            {
              i->get ().print ();
            }
          clog << endl << "Serialize..." << endl;
          {
            ofstream foa ("test_shared_ptr_1.txt");
            boost::archive::text_oarchive oa (foa);
            // oa << hits << hits2;
            oa & hits & hits2;
          }
          {
            ofstream foa ("test_shared_ptr_1.xml");
            boost::archive::xml_oarchive oa (foa);
            // oa << boost::serialization::make_nvp ("hits", hits);
            // oa << boost::serialization::make_nvp ("hits2", hits2);
            oa & boost::serialization::make_nvp ("hits", hits);
            oa & boost::serialization::make_nvp ("hits2", hits2);
          }
          clog << "Done." << endl;

          clog << "Destroy 'hits'..." << endl;
          hits.clear ();
          clog << "Destroy 'hits2'..." << endl;
          hits2.clear ();
          clog << "Done." << endl;

        }
        if (hit::g_debug) clog << "DEBUG: g_count=" << hit::g_count << endl;

        {
          hit_handles_col_t hits;
          hit_handles_col_t hits2;
          if (hit::g_debug) clog << "DEBUG: g_count=" << hit::g_count << endl;
          {
            clog << endl << "Deserialize (from text archive)..." << endl;
            ifstream fia ("test_shared_ptr_1.txt");
            boost::archive::text_iarchive ia (fia);
            //ia >> hits >> hits2;
            ia & hits & hits2;
            clog << "Done." << endl;
            clog << "Hits (loaded) : " << endl;
            for (hit_handles_col_t::const_iterator i = hits.begin ();
                 i != hits.end ();
                 i++)
              {
                i->get ().print ();
              }
            clog << "Hits2 (loaded) : " << endl;
            for (hit_handles_col_t::const_iterator i = hits2.begin ();
                 i != hits2.end ();
                 i++)
              {
                i->get ().print ();
              }
          }

        }
        if (hit::g_debug) clog << "DEBUG: g_count=" << hit::g_count << endl;
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
