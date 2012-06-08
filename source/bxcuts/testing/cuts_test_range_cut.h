// -*- mode: c++ ; -*- 
/* cuts_test_range_cut_h
 * Author (s) : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-20
 * Last modified: 2010-09-20
 * 
 * License: 
 */

/** 
 * Description: 
 *   A cut to test the position of a 3D-point
 *   relatively to a range on X, Y or Z axis.
 * 
 */

#ifndef __cuts__test__range_cut_h
#define __cuts__test__range_cut_h 1

#include <iostream>

#include <cuts/i_cut.h>
#include <cuts/cut_macros.h>

#include <cuts_test_data.h>

namespace cuts {

  namespace test {

    using namespace std;

    // CUT_CLASS_DECLARE (range_cut)
    class range_cut : public cuts::i_cut
    {
    public:

      enum mode_t
        {
          MODE_X = 0,
          MODE_Y = 1,
          MODE_Z = 2
        };

    public:
 
      void set_mode (int a_mode);
 
      void set_range (double a_min, double a_max);
 
      void set_reversed (bool a_reversed);

      /* The CUT_INTERFACE_CTOR_DTOR macro can be used
       * in place of the full specification of the
       * following methods:
       *  - constructor
       *  - destructor
       *  - initialize
       *  - _accept
       *  - reset
       *
       */
      // CUT_INTERFACE_CTOR_DTOR (range_cut)
      
      // ctor:
      // CUT_CONSTRUCTOR_DECLARE (range_cut)
    public:
      range_cut ();

      // dtor:
      // CUT_DESTRUCTOR_DECLARE (range_cut)

    public:

      virtual ~range_cut ();

      // CUT_INITIALIZE_DECLARE()
      virtual void initialize (const datatools::utils::properties &,  
                               datatools::service::service_manager &, 
                               cuts::cut_handle_dict_type &); 

      // CUT_ACCEPT_DECLARE();  
    protected :                                 
      virtual int _accept ();

      // CUT_RESET_DECLARE();
    public:
      virtual void reset ();

    private:

      int    _mode_;
      double _min_, _max_;
      bool   _reversed_;

      // Macro to automate the registration of the cut :
      CUT_REGISTRATION_INTERFACE(range_cut);
 
    };

  } // end of namespace test

} // end of namespace cuts

#endif // __cuts__test__range_cut_h

// end of cuts_test_range_cut_h
