// -*- mode: c++ ; -*- 
/* cuts_test_color_cut_h
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

#ifndef __cuts__test__color_cut_h
#define __cuts__test__color_cut_h 1

#include <iostream>

#include <cuts/i_cut.h>
#include <cuts/cut_macros.h>

#include <cuts_test_data.h>

namespace cuts {

  namespace test {

    using namespace std;

    CUT_CLASS_DECLARE (color_cut)
    {

    public:
 
      void set_color (int a_color);

     public:

      CUT_INTERFACE_CTOR_DTOR (color_cut);

    private:

      int _color_;

    private:
      // Macro to automate the registration of the cut :
      CUT_REGISTRATION_INTERFACE(color_cut);
 
    };

  } // end of namespace test

} // end of namespace cuts

#endif // __cuts__test__color_cut_h

// end of cuts_test_color_cut_h
