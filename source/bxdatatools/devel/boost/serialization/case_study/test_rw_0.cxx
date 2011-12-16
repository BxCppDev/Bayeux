/* test_rw_0.cxx */
/* 
 * Author: F. Mauger <mauger@lpccaen.in2p3.fr>
 * Date:   2009-03-10
 *
 * Copyright (c) 2008 François Mauger
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy 
 * at http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <boost/filesystem.hpp>

#include "safe_serial.h" // utility container for safe memory tracking of 
                         // serialized instances of one type

#include "io_factory.h"  // provides `smart' reader and writer class

#include "foo.h"         // sample user class to be serialized
#include "foo2.h"        // another sample user class to be serialized

using namespace std;

int 
main (int argc_ , char ** argv_) 
{
  try 
    {
      bool debug            = false;
      long seed             = 314159;
      string filename;
      int number_of_objects = 10;
      bool multi_archives   = false; // default is one `single' archive
				     // per file.

      // parse command line arguments:
      if (argc_ > 1)
	{ 
	  filename = argv_[1]; // self explanatory
	}
      if (argc_ > 2) 
	{
	  istringstream seed_ss (argv_[2]);
	  seed_ss >> seed;
	  if (! seed_ss)
	    {
	      throw runtime_error ("Seed format error!");
	    }
	  if (seed < 0)
	    {
	      throw runtime_error ("Invalid seed value!");
	    }
	}
      if (argc_ > 3) 
	{
	  if (string (argv_[3]) == "multi" )
	    {
	      // activates the `multi-archives' mode:
	      multi_archives = true;
	    }
	}

      // shortcut:
      namespace BCSS = boost_case_study::serialization;
      
      BCSS::io_factory::g_debug = debug; // activate debug
					 // printing... brrrr!

      // initialize the pseudo-random number generator:
      srand48 (seed);

      // check:
      if (filename.empty ())
	{
	  throw runtime_error ("Missing filename!");
	}
      clog << "NOTICE: Using filename : `" << filename << "'" << endl;
      clog << "NOTICE: Archive mode   :  " 
	   << (multi_archives ? "multi": "single") << "'" << endl;
      
      if (boost::filesystem::exists (filename)) 
	{
	  ostringstream message;
	  message << "File '" << filename << "' already exists!";
	  clog << "WARNING: " << message.str () << endl;
	}

      // set `single/multi-archives' mode:
      bool multi_mode = BCSS::using_single_archive;
      if (multi_archives) multi_mode = BCSS::using_multi_archives;

      // Store objects:
      {
	clog << endl;
	clog << "NOTICE: Writing..." << endl;

	BCSS::safe_serial<foo> foo_safe;
	BCSS::safe_serial<foo2> foo2_safe;

	// create the data serializer (writer):
	BCSS::data_writer writer (filename, multi_mode);

	int counts = number_of_objects;
	for (int i = 0; i < counts; i++) 
	  {
	    double prob = drand48 ();
	    if (prob < 0.75)
	      {
		foo_safe.make (); // generate a new `foo' instance
		foo_safe.get ().randomize (); // randomize it
		clog << foo_safe.get () << endl; // print it

		// store it using a dedicated string tag as class ID:
		writer.store (foo::SERIAL_TAG, foo_safe.get ());

		// safely clear the instance as memory tracking
		// cannot corrupt data with `multi-archives' mode:
		if (multi_archives) foo_safe.clear ();
	      }
	    else
	      {
		foo2_safe.make (); // generate a new `foo2' instance
		foo2_safe.get ().randomize ();
		clog << foo2_safe.get () << endl;

		// store it using a dedicated string tag as class ID:
		writer.store (foo2::SERIAL_TAG, foo2_safe.get ());

		// safely clear the instance as memory tracking
		// cannot corrupt data with `multi-archives' mode:
		if (multi_archives) foo2_safe.clear ();
	      }
	  }
	clog << "NOTICE: Writing done." << endl;
	if (! multi_archives) 
	  {
	    // In `single archive' mode, clear the instances 
	    // addressed by `foo_safe' and `foo2_safe' only 
	    // AFTER full serialization is done. This is because
	    // memory tracking is likely to `corrupt' data located 
	    // at previously used memory storage:
	    foo_safe.clear ();
	    foo2_safe.clear ();
	    //
	    // Remark: the `single archive' mode may induce a large 
	    // memory consumption if one wants to store a large number
	    // of `big' objects. This may be a limitation depending of
	    // the machine.
	    // In this case, I recommend to use the `multi-archives'
	    // mode because it confines memory tracking within one 
	    // instance per archive, with no risk of data corruption.
	    // More, the use of the `safe_serial<>' utility is not
	    // needed and one can simply use an instance of the
	    // serialized class:
	    //
	    //   foo my_foo;
	    //   my_foo.randomize ();
	    //	 clog << my_foo << endl;
	    //	 writer.store (foo::SERIAL_TAG, my_foo.get ());
	    // 
	  }
      }
    
      // Load objects:
      {
	clog << endl;
	clog << "NOTICE: Reading..." << endl;

	BCSS::safe_serial<foo> foo_safe;
	BCSS::safe_serial<foo2> foo2_safe;

	// create the data serializer (reader):
	BCSS::data_reader reader (filename, multi_mode);  
  
	int counts = 0;
	while (reader.has_record_tag ()) 
	  {
	    if (reader.record_tag_is (foo::SERIAL_TAG)) 
	      {
		foo_safe.make ();
		reader.load (foo::SERIAL_TAG, foo_safe.get ());
		// do something with this `foo' instance...
		clog << foo_safe.get () << endl;
		// safely clear the instance as memory tracking
		// cannot corrupt data with `multi-archives' mode:
		if (multi_archives) foo_safe.clear ();
	      }
	    else if (reader.record_tag_is (foo2::SERIAL_TAG)) 
	      {
		foo2_safe.make ();
		reader.load (foo2::SERIAL_TAG, foo2_safe.get ());
		// do something with this `foo2' instance...
		clog << foo2_safe.get () << endl;
		// safely clear the instance as memory tracking
		// cannot corrupt data with `multi-archives' mode:
		if (multi_archives) foo2_safe.clear ();
	      }
	    else 
	      {
		string bad_tag = reader.get_record_tag ();
		clog << "ERROR: Unknown data tag '" 
		     << bad_tag << "'!" << endl; 
		clog << "ERROR: Cannot handle this kind of object!" << endl; 
		break;
	      }
	    counts++;
	  }
	clog << "NOTICE: Counts = " << counts << endl;
	if (! multi_archives) 
	  {
	    // In `single archive' mode, clear the instances 
	    // addressed by `foo_safe' and `foo2_safe' only 
	    // AFTER full serialization is done. This is because
	    // memory tracking is likely to `corrupt' data located 
	    // at previously used memory storage:
	    foo_safe.clear ();
	    foo2_safe.clear ();
	  }
	clog << "NOTICE: Reading done." << endl << endl;   
      } 

    }
  catch (exception & x) 
    {
      cerr << "test_rw_0: ERROR: " << x.what () << endl;
      exit (EXIT_FAILURE);
    }
  return (EXIT_SUCCESS);
}

/* end of test_rw_0.cxx */
