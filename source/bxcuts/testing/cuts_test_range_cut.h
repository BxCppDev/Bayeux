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

#ifndef CUTS_TEST_RANGE_CUT_H
#define CUTS_TEST_RANGE_CUT_H 1

#include <iostream>

#include <cuts/i_cut.h>

#include <cuts_test_data.h>

namespace cuts {

  namespace test {

    using namespace std;

    // CUT_CLASS_DECLARE (range_cut)
    class range_cut : public cuts::i_cut
    {
    public:

      enum mode_t {
        MODE_X = 0,
        MODE_Y = 1,
        MODE_Z = 2
      };

      void set_mode (int a_mode);

      void set_range (double a_min, double a_max);

      void set_reversed (bool a_reversed);

      /* The CUT_INTERFACE_CTOR_DTOR macro can be used
       * in place of the full specification of the
       * following methods:
       *  - constructor
       *  - destructor
       *  - initialize
       *  - reset
       *  - _accept
       *
       */
      // CUT_INTERFACE_CTOR_DTOR (range_cut)

      // CUT_CONSTRUCTOR_DECLARE (range_cut)
      range_cut (datatools::logger::priority a_logging_priority
                 = datatools::logger::PRIO_FATAL);

      // CUT_DESTRUCTOR_DECLARE (range_cut)
      ~range_cut() override;

      // CUT_INITIALIZE_DECLARE()
      void initialize (const datatools::properties &,
                       datatools::service_manager &,
                       cuts::cut_handle_dict_type &) override;

      /// Export to a container of properties
      void export_to_config(datatools::properties & config_,
                            uint32_t flags_ = i_cut::EXPORT_CONFIG_DEFAULT,
                            const std::string & prefix_ = "") const override;

      // CUT_RESET_DECLARE();
      void reset() override;

    protected :

      // CUT_ACCEPT_DECLARE();
      int _accept() override;

    private:

      int    _mode_;
      double _min_;
      double _max_;
      bool   _reversed_;

      // Macro to automate the registration of the cut :
      CUT_REGISTRATION_INTERFACE(range_cut)

    };

  } // end of namespace test

} // end of namespace cuts

#endif // CUTS_TEST_RANGE_CUT_H
