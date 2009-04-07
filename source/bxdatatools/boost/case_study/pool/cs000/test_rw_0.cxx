/* test_rw_0.cxx */
/* 
 * Author: F. Mauger <mauger@lpccaen.in2p3.fr>
 * Date:   2009-03-10
 *
 * Copyright (c) 2008-2009 Francois Mauger
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy 
 * at http://www.boost.org/LICENSE_1_0.txt)
 *
 * Description: a sample program that illustrates how to use
 * the `io_factory' classes for data serialization in files.
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

void print_usage (std::ostream & out_ = clog)
{
  out_ << "Usage:\n";
  out_ << "\n";
  out_ << "  test_rw_0 [OPTIONS] [FILENAME] [RANDOM_SEED]\n";
  out_ << "\n";
  out_ << "Options:\n\n";
  out_ << "  --help         : print this help then exit\n";
  out_ << "  --debug        : print debug info\n";
  out_ << "  --single       : use `single archive' mode\n";
  out_ << "  --multi        : use `multi-archives' mode\n";
  out_ << "  --with-nans    : allow randomized non-finite floating numbers\n";
  out_ << "  --without-nans : do not allow non-finite floating numbers\n";
  out_ << "\n";
  out_ << "Arguments:\n\n";
  out_ << "  FILENAME        : the serialization filename\n";
  out_ << "  RANDOM_SEED     : the PRNG seed\n";
  out_ << "\n";
}

int 
main (int argc_ , char ** argv_) 
{
  try 
    {
      bool debug            = false;
      long seed             = -1;
      string filename;
      int number_of_objects = 10;
      bool multi_archives   = false; // default uses the `single-archive' mode
      bool with_nans        = false;

      // parse command line options and arguments:
      int iarg = 1;
      while (iarg < argc_)
	{
	  string arg = argv_[iarg];

	  if (arg[0] == '-')
	    {
	      if (arg == "--help")
		{
		  print_usage ();
		  exit (0);
		}
	      if (arg == "--debug")
		{
		  debug = true;
		}
	      if (arg == "--single")
		{
		  multi_archives = false;
		}
	      if (arg == "--multi")
		{
		  multi_archives = true;
		}
	      if (arg == "--with-nans")
		{
		  with_nans = true;
		}
	      if (arg == "--without-nans")
		{
		  with_nans = false;
		}
	    }
	  else
	    {
	      if (filename.empty ())
		{ 
		  filename = arg; // self explanatory
		}
	      else if (seed < 0)
		{ 
		  istringstream seed_ss (arg);
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
	    }
	  iarg++;
	}

      io_factory::io_factory::g_debug = debug; // activate debug printing

      // check:
      if (filename.empty ())
	{
	  filename = "io_factory.txt";
	}
      if (seed < 0)
	{
	  seed = 314159265;
	}
      clog << "NOTICE: Filename     : `" << filename << "'" << endl;
      clog << "NOTICE: Seed         :  " << seed  << endl;
      clog << "NOTICE: Archive mode : `" 
	   << (multi_archives ? "multi": "single") << "'" << endl;
      
      if (boost::filesystem::exists (filename)) 
	{
	  ostringstream message;
	  message << "File '" << filename << "' already exists!";
	  clog << "WARNING: " << message.str () << endl;
	}

      // initialize the pseudo-random number generator:
      srand48 (seed);

      // set `single/multi-archives' mode:
      bool multi_mode = io_factory::using_single_archive;
      if (multi_archives) multi_mode = io_factory::using_multi_archives;

      // Store some random objects:
      {
	clog << endl;
	clog << "NOTICE: Writing..." << endl;

	io_factory::safe_serial<foo> foo_safe;
	io_factory::safe_serial<foo2> foo2_safe;

	// create the data serializer (writer):
	io_factory::data_writer writer (filename, multi_mode);

	int counts = number_of_objects;
	for (int i = 0; i < counts; i++) 
	  {
	    double prob = drand48 ();
	    if (prob < 0.75)
	      {
		// generate a new `foo' instance:
		foo_safe.make ();

		// randomize it:
		foo_safe.get ().randomize (with_nans);

		// print it:
		clog << foo_safe.get () << endl; 

		// store it using a dedicated string tag as class ID:
		writer.store (foo::SERIAL_TAG, foo_safe.get ());

		// safely clear the instance as memory tracking
		// cannot corrupt data with `multi-archives' mode:
		if (multi_archives) foo_safe.clear ();
	      }
	    else
	      {
		// generate a new `foo2' instance:
		foo2_safe.make (); 

		// randomize it: 
		foo2_safe.get ().randomize ();

		// print it: 
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
	    // With `single archive' mode, clear the instances 
	    // addressed by `foo_safe' and `foo2_safe' only 
	    // AFTER full serialization is done. This is because
	    // memory tracking is likely to `corrupt' data located 
	    // at previously used memory location:
	    foo_safe.clear ();
	    foo2_safe.clear ();
	    
	    // Remark: the `single archive' mode may induce a large 
	    // memory consumption if one wants to store a large number
	    // of `big' objects. This may be a limitation depending of
	    // the machine for one cannot free memory while the
	    // serialization process is running.
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
    
      // Load previously serialized objects:
      {
	clog << endl;
	clog << "NOTICE: Reading..." << endl;

	io_factory::safe_serial<foo> foo_safe;
	io_factory::safe_serial<foo2> foo2_safe;

	// create the data serializer (reader):
	io_factory::data_reader reader (filename, multi_mode);  
  
	int counts = 0;
	while (reader.has_record_tag ()) 
	  {
	    if (reader.record_tag_is (foo::SERIAL_TAG)) 
	      {
		// generate a new `foo' instance:
		foo_safe.make ();

		// load it fro the file:
		reader.load (foo::SERIAL_TAG, foo_safe.get ());

		// do something with this `foo' instance...
		clog << foo_safe.get () << endl;

		// safely clear the instance as memory tracking
		// cannot corrupt data with `multi-archives' mode:
		if (multi_archives) foo_safe.clear ();
	      }
	    else if (reader.record_tag_is (foo2::SERIAL_TAG)) 
	      {
		// generate a new `foo2' instance:
		foo2_safe.make ();

		// load it fro the file:
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
		clog << "ERROR: Cannot handle this kind of object! Abort!" << endl; 
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
