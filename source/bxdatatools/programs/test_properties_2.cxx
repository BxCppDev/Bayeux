// -*- mode: c++; -*- 
// test_properties_2.cxx

#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <vector>
#include <map>

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
      vector<int> vi;
      vi.push_back (1);
      vi.push_back (2);
      vi.push_back (3);
      my_dict.store ("vi", vi, "a vector of integers");    
      vector<double> vd;
      vd.push_back (1.6e-19);
      vd.push_back (M_PI);
      vd.push_back (6.022e23);
      my_dict.store ("vd", vd, "a vector of reals");    
      vector<string> vs;
      vs.push_back ("Hello");
      vs.push_back ("world");
      vs.push_back ("!");
      my_dict.store ("vs", vs, "a vector of strings");    
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

      //du::properties my_dict;
      //my_dict.export_starting_with (my_dict, "visibility.");

      map<string, string> string_based_dict;
      my_dict.export_to_string_based_dictionary (string_based_dict);

      cout << endl << "Exported string-based dictionary:" << endl;
      for (map<string,string>::const_iterator i = string_based_dict.begin ();
	   i != string_based_dict.end ();
	   i++)
	{
	  cout << i->first << " = " << i->second << endl;
	}
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
