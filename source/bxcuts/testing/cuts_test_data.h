// -*- mode: c++ ; -*-
/* cuts_test_data.h
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

#ifndef CUTS_TEST_TEST_DATA_H
#define CUTS_TEST_TEST_DATA_H

#include <iostream>

namespace cuts {

  namespace test {

    //! \brief A colored point in a 3D space
    class data
    {
    public:

      enum color_type {
        BLACK = 0,
        RED   = 1,
        GREEN = 2,
        BLUE  = 3
      };

      data ()
      {
        x = y = z = 0.0;
        color = BLACK;
      }

      void print (std::ostream & out_) const
      {
        out_ << x << ' ' << y << ' ' << z << ' ' << color << std::endl;
      }

      double x, y, z; //!< Coordinates in a 3D space
      int    color; //!< Color

    };

  } // end of namespace test

} // end of namespace cuts

#endif // CUTS_TESTING_TEST_DATA_H
