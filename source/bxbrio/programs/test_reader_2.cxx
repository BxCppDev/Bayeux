// -*- mode: c++ ; -*- 
// test_reader_2.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <brio/reader.h>
#include <datatools/test/my_data.h>
#include <datatools/utils/properties.h>

#include <datatools/serialization/i_serializable.h>

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
      brio::reader my_reader ("test_io_2.brio", verbose, debug);

      // Loop on serialized records in this store:
      while (my_reader.has_next ())
	{
	  datatools::test::data_t data;
	  my_reader.load_next (data);
	  if (dump) 
	    {
	      data.tree_dump (clog, "Data loaded from the *automatic* store: ");
	    }
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
