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

// Support for XML 'key-value' based archives:
#include <boost/serialization/nvp.hpp>

// Support Boost serialization of STL string objects:
#include <boost/serialization/string.hpp>

// Support Boost serialization of STL vector templatized objects:
#include <boost/serialization/vector.hpp>

// Interface base class from datatools to support serialization tools:
#include <datatools/serialization/i_serializable.h>


namespace brio {

  namespace test {

    using namespace std;

    /** A sample serializable class using datatools and/or brio serialization  
     *  concept.
     */
    class data : public datatools::serialization::i_serializable     
      {
      public:

	/** A constant static string member that store the serialization tag
	 *  for this class.
	 */
	static const std::string SERIAL_TAG; 

	/** Mandatory for we inherit from the 
	 *  `datatools::serialization::i_serializable' abstract interface:
	 *  This member function returns the SERIAL_TAG above.
	 */
	virtual const std::string & get_serial_tag () const;
	
      public:

	// Here we provide a default constructor:
	data ();

	// Desctructor:
	virtual ~data ();

	void randomize ();

	void dump (ostream & out_ = clog) const;

	/* Here we may have many other member methods. */

      private:

	/* Boost serialization concept implies to add the following 
	 * private template `serialize' method in our class. This acts
	 * as a streamer.
	 *
	 * Here each field member is serialized.
	 *
	 */

	friend class boost::serialization::access; 
	BOOST_SERIALIZATION_SERIALIZE_DECLARATION()

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
      

      };

  } // namespace test 

} // namespace brio 

#endif // __brio__test__data_h

/* end of data.h */
