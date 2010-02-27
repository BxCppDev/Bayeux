// -*- mode: c++; -*- 
// test_properties_2.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>

#include <datatools/serialization/io_factory.h>
#include <datatools/serialization/serialization.h>
#include <datatools/utils/properties.h>

using namespace std;

int main (int argc_, char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      clog << "Test of the 'properties' class..." << endl; 
  
      bool debug = false;
      bool use_validator = true;
      long seed = 12345;

      int iarg = 1;
      while (iarg < argc_) 
	{
	  string arg = argv_[iarg];
	  if ( arg == "-d" || arg == "--debug" ) debug = true;
	  if ( arg == "-v" || arg == "--no-validator" ) use_validator = false;
	  iarg++;
	}

      srand48 (seed);

      namespace du = datatools::utils;

      du::properties::g_debug = debug;

      du::properties my_dict ("a list of user properties");
      if (! use_validator) 
	{
	  my_dict.unset_key_validator ();
	}
      my_dict.dump (clog);
      my_dict.store ("name","my name");
      my_dict.store ("firstname","my firstname");
      my_dict.store (du::properties::make_private_key ("private"),
		     "a private data", "Confidential stuff");
      my_dict.store ("visibility.shown", true);
      my_dict.store ("visibility.color", "red");
      my_dict.store ("visibility.rendering", "shaded");
      my_dict.store ("visibility.texture", "marble");
      my_dict.store ("age", 24, "the age of the captain");    
      my_dict.dump (clog);

      du::properties::keys_col_t all_keys;
      my_dict.keys (all_keys);

      du::properties::keys_col_t visibility_keys;
      my_dict.keys_starting_with (visibility_keys, "visibility.");
      du::properties::keys_col_t other_keys;
      my_dict.keys_not_starting_with (other_keys, "visibility.");

      clog << "All keys: " << endl;
      for (du::properties::keys_col_t::const_iterator i
	     = all_keys.begin ();
	    i != all_keys.end ();
	   i++)
	{
	  clog << " " << '"' << *i << '"' << endl; 
	}
      clog << endl;
 
      clog << "Visibility keys: " << endl;
      for (du::properties::keys_col_t::const_iterator i 
	     = visibility_keys.begin ();
	    i != visibility_keys.end ();
	   i++)
	{
	  clog << " " << '"' << *i << '"' << endl; 
	}
      clog << endl;
 
      clog << "Other keys: " << endl;
      for (du::properties::keys_col_t::const_iterator i 
	     = other_keys.begin ();
	    i != other_keys.end ();
	   i++)
	{
	  clog << " " << '"' << *i << '"' << endl; 
	}

      du::properties my_dict2 = my_dict;
      my_dict2.erase_all_not_starting_with ("visibility.");
      my_dict2.dump (clog);

      du::properties my_dict3 = my_dict;
      my_dict3.erase_all_starting_with ("visibility.");
      my_dict3.dump (clog);

      du::properties my_dict4;
      my_dict.export_starting_with (my_dict4, "visibility.");
      my_dict4.dump (clog);

      du::properties my_dict5;
      my_dict.export_not_starting_with (my_dict5, "visibility.");
      my_dict5.dump (clog);

      my_dict.export_starting_with (my_dict, "visibility.");

    }
  catch (exception & x)
    {
      clog << "error: " << x.what () << endl; 
      error_code = EXIT_FAILURE;
    }
  catch (...)
    {
      cerr << "error: " << "unexpected error!" << endl; 
      error_code = EXIT_FAILURE;
    }
  return error_code;
}

// end of test_properties_2.cxx
