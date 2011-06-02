// -*- mode: c++; -*- 
// test_propinit.cxx

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <vector>

#include <datatools/utils/i_propinit.h>
#include <datatools/utils/properties.h>

using namespace std;

struct foo : public datatools::utils::i_propinit
{
public:
  bool   debug_;
  string name_;
  int    value_;
public:

  foo () : debug_ (false), name_ (""), value_ (0)
  {
  }
     
  void init_from (const datatools::utils::properties & a_config) 
  {
    if (a_config.has_flag ("debug"))
      {
	debug_ = true;
      }
    if (a_config.has_key ("name"))
      {
	name_ = a_config.fetch_string ("name");
      }
    if (a_config.has_key ("value"))
      {
	value_ = a_config.fetch_integer ("value");
      }
    return;
  }
 
  void print () const
  {
    cout << "foo : " 
	 << "debug=" << debug_ << ", "
	 << "name=\"" << name_ << "\" , " 
	 << "value=" << value_ << endl;
  }

};

int main (int argc_ , char ** argv_)
{
  int error_code = EXIT_SUCCESS;
  try 
    {
      clog << "Test of the 'datatools::utils::i_propinit' interface..." << endl; 
      bool debug = false;

      int iarg =  1;
      while (iarg < argc_) 
	{
          string arg = argv_[iarg];
	  if ((arg == "-d") || (arg == "--debug")) debug = true;
	  iarg++;
	}
    
      {
	foo the_foo;
	the_foo.print ();

	datatools::utils::properties the_config;
	if (debug) the_config.store_flag ("debug");
	the_config.store ("name", "foo");
	the_config.store ("value", 666);

	the_foo.init_from (the_config);
	the_foo.print ();
      }

    }
  catch (exception & x)
    { 
      clog << "error: " << x.what () << endl; 
      error_code =  EXIT_FAILURE;
    }
  catch (...) 
    { 
      clog << "error: " << "unexpected error!" << endl;  
      error_code = EXIT_FAILURE; 
    } 
  return error_code;
} 

// end of test_propinit.cxx 
  
