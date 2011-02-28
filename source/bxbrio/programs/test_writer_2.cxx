// -*- mode: c++ ; -*- 
// test_writer_2.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <brio/writer.h>
#include <datatools/test/my_data.h>

#include <datatools/serialization/i_serializable.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'brio::writer'!" << endl; 
  
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

      // Setup a brio writer:           
      brio::writer my_writer ("test_io_2.brio", verbose, debug);

      // Uncomment the following instruction to use a 'data' store:
      //my_writer.add_store ("data");
	
      // Store `data' randomized objects within an *automatic* store;
      for (int i = 0; i < data_count; i++) 
	{
	  datatools::test::data_t data;
	  datatools::test::randomize_data (data);
	  if (dump) 
	    {
	      data.tree_dump (clog, "Data to be stored in the *automatic* store: ");
	    }
	  my_writer.store (data);
	}

      // Print writer's status:
      if (dump) my_writer.tree_dump (clog, "brio::writer: ");

      // Close the file:
      my_writer.close ();
      
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

// end of test_writer_2.cxx
