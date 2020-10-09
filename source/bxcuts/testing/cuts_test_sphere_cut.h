// -*- mode: c++ ; -*-
/* cuts_test_sphere_cut_h
 * Author (s) : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-09-20
 * Last modified: 2010-09-20
 *
 * License:
 *
 */

/**
 * Description:
 *   A cut to test the position of a 3D-point
 *   relatively to a sphere.
 *
 */

#ifndef CUTS_TEST_SPHERE_CUT_H
#define CUTS_TEST_SPHERE_CUT_H 1

#include <iostream>

#include <cuts/i_cut.h>

#include <cuts_test_data.h>

namespace cuts {

  namespace test {

    using namespace std;

    class sphere_cut : public cuts::i_cut
    {

    protected:

      void set_radius (double a_radius);

      void set_center (double a_x0, double a_y0, double a_z0);

      void set_reversed (bool a_reversed);

    public:

      sphere_cut(datatools::logger::priority a_logging_priority =
                 datatools::logger::PRIO_FATAL);

      ~sphere_cut() override;

      /// Initialization
      void initialize(const datatools::properties &,
                      datatools::service_manager &,
                      cuts::cut_handle_dict_type &) override;

      /// Export to a container of properties
      void export_to_config(datatools::properties & config_,
                            uint32_t flags_ = i_cut::EXPORT_CONFIG_DEFAULT,
                            const std::string & prefix_ = "") const override;

      /// Reset
      void reset() override;

    protected :

      /// Selection
      int _accept() override;

    private:

      double _x0_;
      double _y0_;
      double _z0_;
      double _radius_;
      bool   _reversed_;

      // Macro to automate the registration of the cut :
      CUT_REGISTRATION_INTERFACE(sphere_cut)

    };

  } // end of namespace test

} // end of namespace cuts

#endif // CUTS_TEST_SPHERE_CUT_H
