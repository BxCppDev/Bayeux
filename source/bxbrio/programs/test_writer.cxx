// -*- mode: c++ ; -*- 
// test_writer.cxx
 
#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <datatools/test/my_data.h>
#include <datatools/utils/properties.h>

/* high-level serialization code from the 'datatools' library 
 * - released classes (properties)
 * - test classes (data_t)
 */
#include <datatools/the_serializable.h> 
#include <datatools/test/the_serializable.h> 

#include <brio/test/data.h>
#include <brio/test/data.ipp> 

#include <brio/writer.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try
    {
      clog << "Test program for class 'brio::writer' !" << endl; 
  
      bool debug = false;
      bool verbose = false;
      bool with_mixed = false;
      bool dump = false;
      size_t data_count = 10;
      bool pre_set_stores = true;
      long seed = 314159;
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
	       else if ((option == "-x")) 
                 {
                   with_mixed = true;
                 } 
               else if ((option == "-p")) 
                 {
                   pre_set_stores = false;
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
 
      srand48 (seed);

      // Declare a brio writer:           
      brio::writer my_writer;

      /* Some setup before opening the output file */
      my_writer.set_debug (debug);
      my_writer.set_verbose (verbose);

      // Allow to use some stores with serialized objects of different types (dangerous):
      my_writer.set_allow_mixed_types_in_stores (with_mixed); 

      // Allow the automatic creation of the so-called *automatic* store:
      my_writer.set_allow_automatic_store (true); 

      // Attach the brio writer to a ROOT file:
      string filename = "test_io.brio";
      if (text)
	{
	  clog << "notice: using '" << brio::writer::TEXT_LABEL << "' archive format !" << endl; 
 	  filename = "test_io.trio";
	}
      else
	{
	  clog << "notice: using '" << brio::writer::QPBA_LABEL << "' archive format !" << endl; 
	}
      my_writer.open (filename); 

      // Print writer's status: 
      my_writer.print_info (clog);

      // Setup pre-defined stores:
      if (pre_set_stores)
	{
	  /* Setup a store labelled 'header' 
	   * with dedicated serialization tag 
	   * and dedicated buffer size:
	   */
	  my_writer.add_store ("header", datatools::utils::properties::SERIAL_TAG, 32000);

	  // Idem for a store labelled 'data': 
	  my_writer.add_store ("data", datatools::test::data_t::SERIAL_TAG, 256000);

	  /* Setup a store labelled 'mixed_data' with
	   * dedicated buffer size but possibly mixing
	   * serialized objects with different serialization tags:
	   */
	  if (my_writer.is_allow_mixed_types_in_stores ())
	    {
	      my_writer.add_mixed_store ("mixed_data", 256000);
	    }
	  
	  // Idem for a store labelled 'data': 
	  my_writer.add_store ("data2", brio::test::data::SERIAL_TAG, 256000);

	  // Lock the writer to prevent to add more stores:
	  my_writer.lock ();
	}

      // Print writer's status:
      my_writer.print_info (clog);

      // Create a `properties' container:
      datatools::utils::properties infos;
      infos.store_flag ("test");
      infos.store ("library", "brio");
      infos.store ("io_system", "ROOT");
      infos.store ("archive_format", "portable archive");
      infos.store ("author", "King Arthur");
      if (dump) 
	{
	  infos.tree_dump (clog, "Properties to be stored in the 'header' store: ");
	}
      
      // Store it in the `header' store:
      my_writer.store (infos, "header");

      infos.clear ();
      // Set another `properties' container:
      infos.store_flag ("test2");
      infos.store ("name", "King Arthur");
      infos.store ("favorite_colour", "blue");
      infos.store ("favorite_animal", "swallow");
      infos.store ("favorite_fruit", "coconut");
      if (dump) 
	{
	  infos.tree_dump (clog, "Another properties to be stored in the 'header' store: ");
	}

      // Store it in the `header' store (i.e. the current store from the last 'store' call):
      my_writer.store (infos);

      // Try to select a new store:
      if (my_writer.has_store ("data"))
	{
	  // If the `data' store already exists within the writer, select it
	  // for next objects to be serialized:
	  my_writer.select_store ("data");
	}
      else
	{
	  // Unselect the current working store ('header') because
	  // we want the 'data' objects to be saved in some dedicated store:
	  my_writer.unselect_store ();
	}
      // Store `data' randomized objects within another store;
      for (int i = 0; i < data_count; i++) 
	{
	  datatools::test::data_t data;
	  datatools::test::randomize_data (data);
	  if (dump) 
	    {
	      data.tree_dump (clog, "Data to be stored in the 'data' store: ");
	    }
	  // 
	  my_writer.store (data);
	}

      // Store a set of mixed data within the 'mixed' store:
      if (my_writer.has_store ("mixed_data"))
	{
	  my_writer.select_store ("mixed_data");
	  
	  datatools::test::data_t data;
	  datatools::test::randomize_data (data);
	  if (dump) 
	    {
	      data.tree_dump (clog, "Data to be stored in the 'mixed_data' store in ROOT file: ");
	    }
	  my_writer.store (data);

	  datatools::utils::properties infos;
	  infos.store_flag ("in_mixed_store");
	  if (dump) 
	    {
	      infos.tree_dump (clog, "Properties to be stored in the 'mixed_data' store in ROOT file: ");
	    }
	  my_writer.store (infos);
	}

      // Unselect the current store:
      my_writer.unselect_store ();

      /*
      // Store a set of mixed data within the 'mixed' store:
      if (my_writer.has_store ("data2"))
	{
	  my_writer.select_store ("data2");
	  brio::test::data data;
	  data.randomize ();
	  if (dump) 
	    {
	      data.dump (clog);
	    }
	  // 
	  //my_writer.store (data);
	}
      */

      // Store a properties object int the *automatic* store:
      {
	my_writer.unlock ();
	datatools::utils::properties infos;
	infos.store_flag ("in_automatic_store");
	my_writer.store (infos);
      }

      // Print writer's status:
      my_writer.print_info (clog);
      
      // Close the file.
      my_writer.close ();
  
      clog << "The end." << endl;
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

// end of test_writer.cxx
