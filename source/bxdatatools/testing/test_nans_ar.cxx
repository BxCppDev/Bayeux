// test_nans_ar.cxx

// Standard library:
#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

// Third party:
// - Boost:
#include <boost/filesystem.hpp>

// This project:
#include <datatools/i_serializable.h>
#include <datatools/io_factory.h>
#include <datatools/safe_serial.h>
#include <datatools/i_serializable.ipp>

#if BOOST_VERSION < 103600 || BOOST_VERSION >= 106900
namespace bmath=boost::math;
#else
namespace bmath=boost::spirit::math;
#endif

using namespace std;

string get_fp_classify_label (int fpc_)
{
  if (fpc_ == FP_INFINITE)
    {
      return "infinite";
    }
  else if (fpc_ == FP_NAN)
    {
      return "nan";
    }
  else if (fpc_ == FP_NORMAL)
    {
      return "normal";
    }
  else if (fpc_ == FP_SUBNORMAL)
    {
      return "subnormal";
    }
  else if (fpc_ == FP_ZERO)
    {
      return "zero";
    }
  return "?";
}

class data_t : public datatools::i_serializable
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

  void info () const
  {
    clog << "DEVEL: info: " << endl;
    clog << "DEVEL: v1 is " << get_fp_classify_label (bmath::fpclassify (__v1)) << endl;
    clog << "DEVEL: v2 is " << get_fp_classify_label (bmath::fpclassify (__v2)) << endl;
    if (! isfinite (__v1))
      {
        clog << "DEVEL: v1 is " << ((__v1 < 0)? " - ": " + ") << endl;
      }
    if (! isfinite (__v2))
      {
        clog << "DEVEL: v2 is " << ((__v2 < 0)? " - ": " + ") << endl;
      }
  }

  data_t ()
  {
    __v1 = drand48 () * 1.e-4;
    __v2 = drand48 () * 1.e+6;
  }

  data_t (double v1_, double v2_)
  {
    __v1 = v1_;
    __v2 = v2_;
  }

  void inf ()
  {
    __v1 = -numeric_limits<double>::infinity ();
    __v2 = numeric_limits<double>::infinity ();
  }

  void nan ()
  {
    __v1 = numeric_limits<double>::quiet_NaN ();
    __v2 = numeric_limits<double>::quiet_NaN ();
  }

  void zero ()
  {
    __v1 = 0.0;
    __v2 = 0.0;
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
  void serialize(Archive & ar_,
                 const unsigned int // version_
                 )
  {
    ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    ar_ & boost::serialization::make_nvp ("v1", __v1);
    ar_ & boost::serialization::make_nvp ("v2", __v2);
    return;
  }
};

ostream & operator<< (ostream & out_, const data_t & d_)
{
  out_ << d_.get_v1 () << ' ' << d_.get_v2 ();
  return out_;
}

const string data_t::SERIAL_TAG = "test_nans::data_t";


const string & data_t::get_serial_tag () const
{
  return data_t::SERIAL_TAG;
}

void test ()
{
  double x = 2.0625;
  double * px = &x;
  void * apx = static_cast<void *>(px);
  uint64_t ux = *(static_cast<uint64_t *>(apx));
  clog << "test: x = " << x << endl;
  clog << "test: ux = " << hex << ux << dec << endl;

  return;
}

int main (int argc_ , char ** argv_)
{
  try
    {
      long seed = 314159;
      bool debug = false;
      bool do_test = false;
      enum format_t
        {
          FORMAT_TXT = 0,
          FORMAT_XML = 1,
          FORMAT_BIN = 2
        };
      int fmt   = FORMAT_XML;

      int iarg = 1;
      while (iarg < argc_)
        {
          string arg=argv_[iarg];
          if (arg[0] == '-')
            {
              if (arg == "-xml") fmt = FORMAT_XML;
              if (arg == "-txt") fmt = FORMAT_TXT;
              if (arg == "-bin") fmt = FORMAT_BIN;
            }
          else if (arg == "-t")
            {
              do_test = true;
            }
          iarg++;
        }

      if (do_test)
        {
          test ();
          return 0;
        }

      string ext = ".txt";
      if (fmt == FORMAT_XML) ext = ".xml";
      if (fmt == FORMAT_BIN) ext = ".data";
      string filename = "test_nans_ar" + ext;

      //boost::math::detail::fp_traits<double>::type::coverage dc;
      //clog << "Coverage = " << dc << endl;

      srand48 (seed);
      clog << "NOTICE: using filename '" << filename << "'" << endl;

      if (boost::filesystem::exists (filename))
        {
          ostringstream message;
          message << "File '" << filename << "' already exists!";
          clog << "warning: " << message.str () << endl;
        }

      {
        clog << "NOTICE: writing..." << endl;
        datatools::safe_serial<data_t> ss_data;
        //datatools::data_writer writer (filename, datatools::using_multi_archives);
        datatools::data_writer writer (filename, datatools::using_single_archive);
        int counts = 10;
        for (int i = 0; i < counts; i++)
          {
            ss_data.make ();
            if (drand48 () < 0.15) ss_data.grab().nan ();
            else if (drand48 () < 0.30) ss_data.grab().inf ();
            else if (drand48 () < 0.45) ss_data.grab().zero ();
            clog << ss_data.get () << endl;
            if (debug) ss_data.get ().info ();
            writer.store(ss_data.get());
          }
        clog << "NOTICE: writing done." << endl << endl;
      }

      {
        clog << "NOTICE: reading..." << endl;
        datatools::safe_serial<data_t> ss_data;
        //datatools::data_reader reader (filename, datatools::using_multi_archives);
        datatools::data_reader reader (filename, datatools::using_single_archive);
        int counts = 0;
        while (reader.has_record_tag ())
          {
            if (reader.record_tag_is (data_t::SERIAL_TAG))
              {
                if (debug) clog << "DEBUG: reading..."
                                << data_t::SERIAL_TAG << endl;
                if (debug) clog << "DEBUG: making a new safe record..."
                                << endl;
                ss_data.make();
                if (debug) clog << "DEBUG: loading the new safe record..."
                                << endl;
                reader.load(ss_data.grab());
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
