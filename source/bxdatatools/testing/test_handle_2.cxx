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

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <datatools/serialization_macros.h>

#include <datatools/handle.h>
#include <datatools/handle_pool.h>
#include <datatools/time_tools.h>

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
    if (g_debug) std::clog << "DEBUG: hit::ctor: g_count=" << g_count << std::endl;
  }
  hit (int a_id, double a_tdc) :
    id_ (a_id) , tdc_ (a_tdc)
  {
    g_count++;
    if (g_debug) std::clog << "DEBUG: hit::ctor: g_count=" << g_count << std::endl;
  }
  ~hit ()
  {
    if (g_debug) std::clog << "DEBUG: hit::dtor: Destruction @" << this << "." << std::endl;
    g_count--;
    if (g_debug) std::clog << "DEBUG: hit::dtor: g_count=" << g_count << std::endl;
  }
  void print () const
  {
    std::cout << "hit @ " << this << " : {id=\"" << id_ << "\" , " << "tdc=" << tdc_ << '}'<< std::endl;
  }
};

// define a predicate for the 'hit' class :
struct hit_is_invalid_predicate : public datatools::i_predicate <hit>
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
      std::clog << "Test of the 'handle<>' template class..." << std::endl;
      bool debug = false;

      int iarg =  1;
      while (iarg < argc_) {
          std::string arg = argv_[iarg];
          if ((arg == "-d") || (arg == "--debug")) debug = true;
          iarg++;
        }

      typedef datatools::handle<hit> hit_handle_t;
      typedef std::vector<hit_handle_t> hit_handles_col_t;
      typedef std::vector<hit> hits_col_t;

      {
        std::clog << std::endl << "Test 1: " << std::endl;
        {
          hit_handles_col_t hits;
          size_t nevents = 100;
          size_t nhits = 100;
          datatools::computing_time CT;
          datatools::computing_time CT2;
          std::ofstream foa ("test_handle_2a.txt");
          hits.reserve (nhits);
          for (int i = 0; i < (int) nevents; i++) {
              if ((i % 100) == 0) std::clog << "#i=" << i << std::endl;
              CT2.start ();
              for (int j = 0; j < (int) nhits; j++) {
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
          CT.tree_dump (std::clog, "CT hit_handles_col_t"); // 33 sec
          CT2.tree_dump (std::clog, "CT2 hit_handles_col_t"); //  1.36152
        }
      }

      {
        std::clog << std::endl << "Test 2: " << std::endl;
        {
          hits_col_t hits;
          size_t nevents = 100;
          size_t nhits = 100;
          datatools::computing_time CT;
          datatools::computing_time CT2;
          std::ofstream foa ("test_handle_2b.txt");
          hits.reserve (nhits);
          for (int i = 0; i < (int) nevents; i++) {
              if ((i % 100) == 0) std::clog << "#i=" << i << std::endl;
              CT2.start ();
              for (int j = 0; j < (int) nhits; j++)
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
          CT.tree_dump (std::clog, "CT hit_col_t"); // 23 sec
          CT2.tree_dump (std::clog, "CT2 hit_col_t"); // 0.174498
        }
      }

      {
        std::clog << std::endl << "Test 3: " << std::endl;
        {
          hit_handles_col_t hits;
          size_t nevents = 100;
          size_t nhits = 100;
          datatools::handle_pool<hit> pool (nhits);
          datatools::computing_time CT;
          datatools::computing_time CT2;
          std::ofstream foa ("test_handle_2c.txt");
          hits.reserve (nhits);
          for (int i = 0; i < (int) nevents; i++)
            {
              if ((i % 100) == 0) std::clog << "#i=" << i << std::endl;
              CT2.start ();
              for (int j = 0; j < (int) nhits; j++) {
                  hits.push_back (pool.create ());
                  hits.back ().grab ().set_id (j).set_tdc (j * 10.);
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
          CT.tree_dump (std::clog, "CT hit_handles_col_t (using pool)"); // 32 sec
          CT2.tree_dump (std::clog, "CT2 hit_handles_col_t (using pool)"); // 0.393747
        }
      }

    }
  catch (std::exception & x)
    {
      std::cerr << "error: " << x.what () << std::endl;
      error_code =  EXIT_FAILURE;
    }
  catch (...)
    {
      std::cerr << "error: " << "unexpected error!" << std::endl;
      error_code = EXIT_FAILURE;
    }
  return error_code;
}

// end of test_handle_2.cxx

