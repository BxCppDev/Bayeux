/* safe_serial.h */ 
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

/*
 * `safe_serial' is a template container class useful 
 * to record a list of instances of some type in a safe way
 * for boost serialization, i.e. to make sure memory traking 
 * won't corrupt data while reusing some memory region. 
 *
 * One can access one instance at a time (the current instance).
 * 
 * Example:
 *
 * struct test_data_t
 * {
 *   int count;
 * };
 *
 * datatools::serialization::safe_serial<test_data_t> safe;
 *
 * safe.make(); // create a new instance and make it the current one
 * safe.get();  // access to the current instance
 * 
 */

#ifndef __boost_case_study__serialization__safe_serial_h
#define __boost_case_study__serialization__safe_serial_h 1

#include <list>
#include <stdexcept>

namespace boost_case_study {

  namespace serialization {
    
    template<typename T>
      class safe_serial 
      {
      public:
	typedef T                    data_type;
	typedef std::list<data_type> data_col;

      private:
	data_col                   __data;
	typename data_col::reverse_iterator __last;

      public:

	const data_col & data () const
	  {
	    return __data;
	  }

	void clear ()
	{
	  __data.clear ();
	  __last = __data.rend ();
	}

	safe_serial () : __data ()
	{
	  __last = __data.rend ();
	}

	virtual ~safe_serial ()
	{
	  clear ();
	}

	bool empty () const
	{
	  return __data.size () == 0;
	}
      
	data_type & get ()
	{
	  if (empty ()) 
	    {
	      throw std::runtime_error ("safe_serial::get: No data!");
	    }
	  return *__last;
	}

	void make ()
	{
	  try
	    {
	      __data.push_back (data_type ());
	    }
	  catch (...)
	    {
	      throw std::runtime_error ("safe_serial::make: "
					"Cannot insert new data element!");
	    }
	  __last = __data.rbegin ();
	}

	void set (const data_type & data_)
	{
	  make ();
	  get () = data_;
	}

      };
    
  } // end of namespace serialization 

} // end of namespace boost_case_study 

#endif // __boost_case_study__serialization__safe_serial_h

/* end of safe_serial.h */
