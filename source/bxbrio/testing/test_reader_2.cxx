// -*- mode: c++ ; -*- 
// test_reader_2.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <brio_test_data.cc>
#include <datatools/properties.h>

// Serialization code :
#include <brio_test_data.ipp>
#include <datatools/properties.ipp>

#include <brio/reader.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'brio::reader' !" << endl; 
  
      bool debug = false;
      bool verbose = false;
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
                   debug = true;
                 }
               else if ((option == "-D") || (option == "--dump")) 
                {
                  dump = true;
                }
               else if ((option == "-v") || (option == "--verbose")) 
                 {
                   verbose = true;
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
      brio::reader my_reader (filename, verbose, debug);

      // Loop on serialized records in this store:
      while (my_reader.has_next ())
        {
          brio::test::data_t data;
          my_reader.load_next (data);
          if (dump) 
            {
              data.dump (clog, "Data loaded from the *automatic* store: ");
            }
        }

      {
        my_reader.select_store ("dummy");
        datatools::properties infos;
        my_reader.load (infos);
        infos.tree_dump (clog, "Loaded infos");
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

// end of test_reader_2.cxx
