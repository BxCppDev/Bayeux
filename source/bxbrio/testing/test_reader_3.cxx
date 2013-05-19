// -*- mode: c++ ; -*-
// test_reader_3.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <brio_test_data.cc>

// Serialization code :
#include <brio_test_data.ipp>

#include <brio/reader.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'brio::reader' !" << endl;

      datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
      bool dump = false;
      size_t data_count = 10;
      bool text = false;

      int iarg = 1;
      while (iarg < argc_)
        {
          string token = argv_[iarg];

          if (token[0] == '-')
            {
              string option = token;
              if ((option == "-d") || (option == "--debug"))
                {
                  logging = datatools::logger::PRIO_DEBUG;
                }
               else if ((option == "-D") || (option == "--dump"))
                {
                  dump = true;
                }
               else if ((option == "-v") || (option == "--verbose"))
                 {
                   logging = datatools::logger::PRIO_NOTICE;
                 }
               else if ((option == "-m") || (option == "--many"))
                 {
                   data_count = 1000;
                 }
               else if ((option == "-M") || (option == "--manymany"))
                 {
                   data_count = 100000;
                 }
               else if ((option == "-t"))
                 {
                  text = true;
                 }
               else
                 {
                    clog << "warning: ignoring option '" << option << "'!" << endl;
                 }
            }
          else
            {
              string argument = token;
              {
                clog << "warning: ignoring argument '" << argument << "'!" << endl;
              }
            }
          iarg++;
      }

      // Setup a brio reader:
      string filename = "test_io_2.brio";
      if (text)
        {
          filename = "test_io_2.trio";
        }
      brio::reader my_reader (filename, logging);

      // Loop on serialized records in this store:
      brio::test::data_t data;
      my_reader.load (data, 2);
      if (dump)
        {
          data.dump (clog, "Data #2 loaded from the *automatic* store: ");
        }

      my_reader.load (data, 7);
      if (dump)
        {
          data.dump (clog, "Data #7 loaded from the *automatic* store: ");
        }

      my_reader.load (data);
      if (dump)
        {
          data.dump (clog, "Next data loaded from the *automatic* store: ");
        }

      my_reader.rewind_store ();

      my_reader.load (data);
      if (dump)
        {
          data.dump (clog, "First data loaded from the *automatic* store: ");
        }

      my_reader.unwind_store ();

      my_reader.load_previous (data);
      if (dump)
        {
          data.dump (clog, "Last data loaded from the *automatic* store: ");
        }

      // Print reader's status:
      if (dump) my_reader.tree_dump (clog, "brio::reader: ");

      // Close the file:
      my_reader.close ();

    }
  catch (exception & x)
    {
      cerr << "error: " << x.what () << endl;
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error!" << endl;
      error_code = EXIT_FAILURE;
    }
  return (error_code);
}

// end of test_reader_3.cxx
