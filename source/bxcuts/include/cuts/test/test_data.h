// -*- mode: c++ ; -*- 
/* test_data.h
 * Author (s) : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-20
 * Last modified: 2010-09-20
 * 
 * License: 
 */

/** 
 * Description: 
 * A simple test data structure to perform 
 *  simple cuts.
 * 
 */

#ifndef __cuts__test__test_data_h
#define __cuts__test__test_data_h 1

#include <iostream>
#include <stdexcept>

namespace cuts {

  namespace test {

    using namespace std;

    class data
    {
    public:

      enum color_t
	{
	  BLACK = 0,
	  RED   = 1,
	  GREEN = 2,
	  BLUE  = 3
	};

      double x, y, z;
      int    color;

    public:

      data ()
      {
	x = y = z = 0.0;
	color = BLACK;
      }
      void print (ostream & out_) const
      {
	out_ << x << ' ' << y << ' ' << z << ' ' << color << endl;
      }

    };

  } // end of namespace test

} // end of namespace cuts

#endif // __cuts__test__data_h

// end of data.h
