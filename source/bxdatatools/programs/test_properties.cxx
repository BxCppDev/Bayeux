// -*- mode: c++; -*- 
// test_properties.cxx

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

      clog << "========================================" << endl;

      du::properties::data my_data ('I', 3);
      my_data.dump (clog);
      clog << endl;

      if (int error = my_data.set_value (666)) 
	{
	  clog << "1 - du::properties::data::set_value: " 
	       << du::properties::data::get_error_message (error) 
	       << "!" << endl;
	}
      my_data.lock ();
      my_data.dump (clog);
      clog << endl;

      if (int error = my_data.set_value (666, 2)) 
	{
	  clog << "2 - du::properties::data::set_value: " 
	       << du::properties::data::get_error_message (error) 
	       << "!" << endl;
	}
      my_data.unlock ();
      my_data.dump (clog);
      clog << endl;

      if (int error = my_data.set_value (666, 2)) 
	{
	  clog << "3 - du::properties::data::set_value: " 
	       << du::properties::data::get_error_message (error)
	       << "!" << endl;
	}
      my_data.dump (clog);
      clog << endl;
    
      if (int error = my_data.set_value (666, 8)) 
	{
	  clog << "4 - du::properties::data::set_value: " 
	       << du::properties::data::get_error_message (error) 
	       << "!" << endl;
	}
      my_data.dump (clog);
      clog << endl;
    
      if (int error = my_data.set_value ("bad value"))
	{
	  clog << "5 - du::properties::data::set_value: " 
	       << du::properties::data::get_error_message (error) 
	       << "!" << endl;
	}
      my_data.dump (clog);
      clog << endl;

      clog << "========================================" << endl;

      du::properties my_dict ("a list of user properties");
      if (! use_validator) 
	{
	  my_dict.unset_key_validator ();
	}
      my_dict.dump (clog);
      clog << endl;
      clog << "========================================" << endl;

      //du::properties::g_debug=true;
      my_dict.store ("name","my name");
      clog << "========================================" << endl;
      my_dict.store ("firstname","my firstname");
      clog << "========================================" << endl;
      my_dict.store (du::properties::make_private_key ("private"),
		     "a private data", "Confidential stuff");

      try 
	{
	  my_dict.store ("#number", 666);
	}
      catch (exception & x) 
	{
	  clog << "ERROR: " << x.what () << endl;
	}
      my_dict.store ("age", 24, "the age of the captain");

      try 
	{
	  my_dict.store ("007", "James Bond");
	}
      catch (exception & x) 
	{
	  clog << "ERROR: " << x.what () << endl;
	}
    
      try 
	{
	  my_dict.store ("", "Money Penny");
	}
      catch (exception & x) 
	{
	  clog << "ERROR: " << x.what () << endl;
	}

      try 
	{
	  my_dict.store ("bad_token", "a bad \"char\"");
	}
      catch (exception & x) 
	{
	  clog << "ERROR: " << x.what () << endl;
	}
    
      my_dict.store ("weight", 23.4);
      my_dict.store ("male", true);
      vector<double> vals;
      vals.push_back (1.0);
      vals.push_back (2.0);
      vals.push_back (3.0);
      my_dict.store ("position", vals, "the position of an object (mm)");
      my_dict.dump (clog);
      clog << endl;

      my_dict.change ("position", 3.14, 1);
      my_dict.change ("age", 77);
      my_dict.lock ("age");
      my_dict.change ("male", false);
      my_dict.update ("phantom", 4);

      my_dict.erase ("weight");
      my_dict.dump (clog);
      bool male;
      my_dict.fetch ("male", male);
      int age;
      my_dict.fetch ("age", age);
      vector<double> vals1;
      my_dict.fetch ("position", vals1);
      clog << endl;

      vector<double> vals2;
      double s = 3.14159;
      for (int i = 1; i < 13; i++) 
	{
	  s /= i;
	  vals2.push_back (s);
	}
      my_dict.change ("position", vals2);
      if (debug) my_dict.tree_dump (clog,
				    "du::properties","DEBUG: ");
      clog << endl;

      vector<bool> bits;
      for (int i = 0; i < 23; i++) bits.push_back (drand48 () > 0.6? true: false);
      my_dict.store ("bits", bits);
      clog << endl;

      vector<string> tokens;
      for (int i = 0; i < 12; i++) 
	{
	  ostringstream tok;
	  tok << "token_" << i;
	  tokens.push_back (tok.str ());
	}
      tokens.push_back (">DEBUG<");
      my_dict.store ("tokens", tokens);

      vector<int> counts;
      my_dict.store ("counts", counts, "Event counts");
      my_dict.update ("phantom", 5);

      my_dict.dump (clog);
      if (debug) my_dict.tree_dump (clog, "du::properties", "DEBUG: ");
      clog << endl;

      clog << "========================================" << endl;

      string filename = "test_properties.xml";
      clog << "serialize: writing to '" << filename << "' ..." << endl;
      {
	datatools::serialization::data_writer writer (filename);
	writer.store (my_dict);
      }
      clog << "serialize: writing done." << endl;
      clog << endl;

      my_dict.clear ();
      my_dict.tree_dump (clog,"Dict after clear:");
      clog << endl;
    
      clog << "serialize: reading from '" << filename << "'..." 
	   << endl;
      {
	datatools::serialization::data_reader reader (filename);
	if (reader.has_record_tag ()) 
	  {
	    if (reader.get_record_tag () == du::properties::SERIAL_TAG) 
	      {
		du::properties a_dict;
		reader.load (a_dict);
		my_dict = a_dict;
	      }
	    else 
	      {
		clog << "Record tag : '"<< reader.get_record_tag () << "'" << endl;
	      }
	  }
      }
    
      clog << "serialize: reading done." << endl;
      clog << endl; 
      my_dict.tree_dump (clog, "Dict after reading:");

      clog << "========================================" << endl;

      string filename_cfg = "test_properties.conf";

      du::properties::write_config (filename_cfg, my_dict);
      du::properties::write_config ("", my_dict);
      my_dict.clear ();
      my_dict.dump (clog);
      clog << endl;

      clog << "========================================" << endl;

      du::properties::read_config (filename_cfg, my_dict);

      my_dict.dump (clog);
      clog << endl;

      clog << "========================================" << endl;

      clog << "Enter a list of strings (\"hello\",\"world\"): " 
	   << endl;
      string as;
      getline (cin, as);

      string s2;
      istringstream iss (as);

      do 
	{
	  iss >> ws;
	  if (iss.eof ()) break;
	  s2 = "";
	  if (du::properties::config::read_quoted_string (iss, s2))
	    {
	      clog << "String = '" << s2 << "'" << endl;
	    }
	  else 
	    {
	      clog << "Cannot parse quoted string from '" << as 
		   << "'!" << endl;
	    }
	} while (iss);
    
      clog << "========================================" << endl;

      string s3 = "azerty";
      clog << "s3='" << s3 << "'" << endl;
      if (du::properties::data::has_forbidden_char (s3)) 
	{
	  clog << "Oops! there are forbidden chars found in 's3'!" 
	       << endl;
	}
      else 
	{
	  clog << "'s3' is ok!" << endl;
	}
    
      string s4 = "aze\"rty";
      clog << "s4='" << s4 << "'" << endl;
      if (du::properties::data::has_forbidden_char (s4))
	{
	  clog << "Oops! there are forbidden chars found in 's4'!"
	       << endl;
	}
      else 
	{
	  clog << "'s4' is ok!" << endl;
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

// end of test_properties.cxx
