// -*- mode: c++; -*-
/** data.h 
 *
 * A serializable sample class
 * 
 */

#ifndef __brio__test__data_h
#define __brio__test__data_h 1

#include <iostream>
#include <string>
#include <vector>

// Portable integral types (mandatory):
#include <boost/cstdint.hpp>

// Interface base class from datatools to support serialization tools:
#include <datatools/serialization/i_serializable.h>


namespace brio {

  namespace test {

    using namespace std;

    /** A sample serializable class using datatools and/or brio serialization  
     *  concept.
     */
    class data : DATATOOLS_SERIALIZABLE_CLASS     
      {

      public:

	// Here we provide a default constructor:
	data ();

	// Desctructor:
	virtual ~data ();

	void randomize ();

	void dump (ostream & a_out = clog, const string & a_title = "") const;

      private:

	bool     __bval; // A boolean 
	int8_t   __cval; /* A signed char (8 bits) 
			  * imples #include <boost/cstdint.hpp>
			  */
	int32_t  __ival; // A 32 bits signed integral  
	float    __fval; // A 32 bits float  
	double   __dval; // A 64 bits float
	string   __sval; /** A STL string
			  * implies #include <boost/serialization/string.hpp>
			  */
	std::vector<double> __dval_vec; /** a STL vector of 64 bits floats
					 * implies #include <boost/serialization/vector.hpp>
					 */

	/* interface i_serializable */
	DATATOOLS_SERIALIZATION_DECLARATION();

      };

  } // namespace test 

} // namespace brio 

#endif // __brio__test__data_h

/* end of data.h */
