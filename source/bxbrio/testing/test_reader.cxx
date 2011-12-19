// -*- mode: c++ ; -*- 
// test_reader.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <datatools/utils/properties.h>
//#include <datatools_test_my_data.cc>
#include <brio_test_data.cc>

// Serialization implementation code :
#include <datatools/utils/properties.ipp>
//#include <datatools_test_my_data.ipp>
#include <brio_test_data.ipp> 

#include <brio/reader.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'brio::reader'!" << endl; 
  
      bool debug = false;
      bool verbose = false;
      size_t data_count = 10;
      bool check_tag = true;
      int iarg = 1;
      bool text = false;

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
               else if ((option == "-c")) 
                 {
                   check_tag = false;
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

      // Declare a brio reader:           
      brio::reader my_reader;

      /* Some setup before opening the output file */
      my_reader.set_debug (debug); 
      my_reader.set_verbose (verbose);

      // Set the flag to check serialization tags while loading archives:
      my_reader.set_check_serial_tag (check_tag);

      // Attach the brio reader to a ROOT file:
      string filename = "test_io.brio";
      if (text)
	{
	  filename = "test_io.trio";
	}
      my_reader.open (filename); 

      // Print reader's status: 
      my_reader.print_info (clog);

      /* Select the 'header' store to be used for 
       * current deserialization source:
       */
      my_reader.select_store ("header");

      // Print reader's status: 
      my_reader.print_info (clog);

      // Loop on serialized records in this store:
      while (my_reader.has_next ())
	{
	  datatools::utils::properties infos;
	  my_reader.load_next (infos);
	  infos.tree_dump (clog, "Properties loaded from the 'header' store: ");
	}
  
      // Print reader's status:
      my_reader.print_info (clog);

      // Select the 'data' store:
      my_reader.select_store ("data");

      // Loop on serialized records in this store:
      while (my_reader.has_next ())
	{
	  brio::test::data_t data;
	  my_reader.load_next (data);
	  data.dump (clog, "Data loaded from the 'data' store: ");
	}

      // Load from *automatic* store:
      if (my_reader.has_automatic_store ())
	{
	  clog << "Reader has found an automatic store to be selected !" << endl;
	  my_reader.select_automatic_store ();     
	  datatools::utils::properties infos;
	  my_reader.load_next (infos);
	  infos.tree_dump (clog, "Properties loaded from the *automatic* store: ");
	}

      // Print reader's status:
      my_reader.print_info (clog);

      // Close the file.
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

// end of test_reader.cxx
