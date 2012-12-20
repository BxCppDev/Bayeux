/* test_serialization_2.cxx */

#include <cstdlib> 
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept> 

#include <boost/filesystem.hpp>

#include <datatools/io_factory.h>
#include <datatools/safe_serial.h>

// a serializable test class :
#include <datatools_test_my_data.cc>
#include <datatools_test_my_data.ipp>

// Some pre-processor guard about Boost I/O usage and linkage :
//#include <datatools/bio_guard.h>

// Special Boost serialization export/registration code :
/*
BOOST_CLASS_EXPORT_KEY2(datatools::test::data_t, "datatools::test::data_t")
BOOST_CLASS_EXPORT_IMPLEMENT(datatools::test::data_t)
BOOST_CLASS_EXPORT_KEY2(datatools::test::more_data_t, "datatools::test::more_data_t")
BOOST_CLASS_EXPORT_IMPLEMENT(datatools::test::more_data_t)
*/

using namespace std;

int main (int argc_, char ** argv_) 
{
  try 
    {
      bool debug = false;
      long seed  = 12345;
      string filename = "";
      size_t nrecords = 3;
      bool append = false;

      enum format_t
        {
          FORMAT_TXT = 0, 
          FORMAT_XML = 1, 
          FORMAT_BIN = 2
        };
      int fmt = FORMAT_XML;

      int iarg = 1;
      while (iarg < argc_) 
        {
          string arg = argv_[iarg];
          if (arg[0] == '-') 
            {
              if (arg == "-d")    debug    = true;
              if (arg == "-10")   nrecords = 10;
              if (arg == "-100")  nrecords = 100;
              if (arg == "-1000") nrecords = 1000;
              if (arg == "-xml")  fmt = FORMAT_XML;
              if (arg == "-txt")  fmt = FORMAT_TXT;
              if (arg == "-bin")  fmt = FORMAT_BIN;
              if (arg == "-a")    append = true;
            }
          else 
            {
              if (filename.empty ()) 
                {
                  filename = arg;
                }
              else 
                {
                  istringstream iss (arg);
                  iss >> seed;
                  if (! iss) 
                    {
                      ostringstream message;
                      message << "format error for 'seed'!"; 
                      throw runtime_error (message.str ());
                    }
                }
            }
          iarg++;
        }

      if (filename.empty ()) 
        {
          if (fmt == FORMAT_XML) filename = "test_serialization_2.xml";
          if (fmt == FORMAT_TXT) filename = "test_serialization_2.txt";
          if (fmt == FORMAT_BIN) filename = "test_serialization_2.data";
        } 

      datatools::io_factory::g_debug = debug;
      srand48 (seed);
      clog << "NOTICE: using filename '" << filename << "'" << endl;

      int mode_guess;
      if (datatools::io_factory::guess_mode_from_filename (filename, mode_guess) 
          != datatools::io_factory::SUCCESS) 
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
          if (append) 
            {
              clog << "WARNING: Append to existing file '" << filename << "'!"
                   << endl;
              /*
                ostringstream message;
                message << "File '" << filename << "' already exists!";
                throw runtime_error (message.str ());
              */
            }
        }
    
      {
        clog << "NOTICE: writing..." << endl;
        bool append_mode = datatools::no_append_mode;
        if (append) append_mode |= datatools::append_mode;
        datatools::data_writer writer (filename, datatools::using_multi_archives, append_mode);

        for (int i = 0; i < (int) nrecords; i++) 
          {
            if (debug) clog << "DEBUG: Counts = " << i << endl;
            double p = 0.25 + 0.5 * (i % 2);
            if (p < 0.5) 
              {
                datatools::test::data_t d;
                randomize_data (d);
                d.tree_dump (clog, "datatools::test::data_t", "<< ");
                writer.store (d);
              }
            else 
              {
                datatools::test::more_data_t md;
                randomize_more_data (md);
                md.tree_dump (clog, "datatools::test::more_data_t", "<< ");
                writer.store (md);
              }
          }
        clog << "NOTICE: writing done." << endl << endl;
      }

      bool read = true;
      if (read)
        {
          clog << "NOTICE: reading..." << endl;

          size_t counts = 0;
          datatools::data_reader reader (filename, datatools::using_multi_archives);    
          while (reader.has_record_tag ()) 
            {
              if (debug) clog << "DEBUG: read next record..." << endl;
              if (reader.record_tag_is (datatools::test::data_t::SERIAL_TAG)) 
                {
                  if (debug) clog << "DEBUG: reading..." 
                                  << datatools::test::data_t::SERIAL_TAG << endl;
                  datatools::test::data_t d;
                  reader.load (d);
                  d.tree_dump (clog, "datatools::test::data_t", ">> ");
                }
              else if (reader.record_tag_is (datatools::test::more_data_t::SERIAL_TAG)) 
                {
                  if (debug)clog << "DEBUG: reading..." 
                                 << datatools::test::more_data_t::SERIAL_TAG 
                                 << endl;
                  datatools::test::more_data_t md;
                  reader.load (md);
                  md.tree_dump (clog, "datatools::test::more_data_t", ">> ");
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
      cerr << "test_serialization_2: ERROR: " << x.what () << endl;
      exit (EXIT_FAILURE);
    }

  return (EXIT_SUCCESS);
}

/* end of test_serialization_2.cxx */
