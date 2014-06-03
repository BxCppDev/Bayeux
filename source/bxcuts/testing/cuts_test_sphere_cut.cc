// -*- mode: c++ ; -*-
/* cuts_test_sphere_cut.cc
 */

#include <cuts_test_sphere_cut.h>

#include <stdexcept>
#include <sstream>

namespace cuts {

  namespace test {

    using namespace std;

    // Registration instantiation macro :
    CUT_REGISTRATION_IMPLEMENT(sphere_cut, "cuts::test::sphere_cut");

    void sphere_cut::set_radius (double radius_)
    {
      _radius_ = radius_;
      return;
    }

    void sphere_cut::set_center (double x0_, double y0_, double z0_)
    {
      _x0_ = x0_;
      _y0_ = y0_;
      _z0_ = z0_;
      return;
    }

    void sphere_cut::set_reversed (bool reversed_)
    {
      _reversed_ = reversed_;
      return;
    }

    // ctor:
    sphere_cut::sphere_cut(datatools::logger::priority a_logger_priority)
    : i_cut(a_logger_priority)
    {
      _radius_ = 1.0;
      _x0_ = 0.0;
      _y0_ = 0.0;
      _z0_ = 0.0;
      _reversed_ = false;
    }

    // dtor:
    sphere_cut::~sphere_cut()
    {
      if (is_initialized()) {
        this->sphere_cut::reset();
      }
    }


    void sphere_cut::reset()
    {
      _set_initialized (false);
      _radius_ = 1.0;
      _x0_ = 0.0;
      _y0_ = 0.0;
      _z0_ = 0.0;
      _reversed_ = false;
      i_cut::_reset();
    }

    int sphere_cut::_accept()
    {
      int result = SELECTION_ACCEPTED;
      DT_LOG_TRACE(_logging, "Entering...");
      const data & a_data = get_user_data<data>();
      double hx = a_data.x - _x0_;
      double hy = a_data.y - _y0_;
      double hz = a_data.z - _z0_;
      double r2 = (hx * hx + hy * hy + hz * hz);
      double rlim2 = _radius_ * _radius_;
      if (r2 > rlim2)
        {
          result = SELECTION_REJECTED;
        }
      if (_reversed_)
        {
          result = (result == SELECTION_REJECTED)? SELECTION_ACCEPTED: SELECTION_REJECTED;
        }
      DT_LOG_TRACE(_logging, "Exiting.");
      return result;
    }

    // static method used within a cut factory:
    void sphere_cut::initialize(const datatools::properties& a_configuration,
                                datatools::service_manager& /*a_service_manager*/,
                                cuts::cut_handle_dict_type& /*a_cut_dict*/)
    {
      using namespace std;
      if (is_initialized ())
        {
          ostringstream message;
          message << "cuts::test::sphere_cut::initialize: "
                  << "Cut '" << get_name () << "' is already initialized ! ";
          throw logic_error (message.str ());
        }
      _common_initialize(a_configuration);

      double radius = 1.0;
      double center[3];
      bool reversed = false;
      center[0] = 0.0;
      center[1] = 0.0;
      center[2] = 0.0;

      if (a_configuration.has_key ("center"))
        {
          vector<double> v;
          a_configuration.fetch ("center", v);
          if (v.size () != 3)
            {
              ostringstream message;
              message << "cuts::test::sphere_cut::initialize: "
                      << "Invalid vector size for center !"
                      << endl;
              throw logic_error (message.str ());
            }
          center[0] = v[0];
          center[1] = v[1];
          center[2] = v[2];
        }

      if (a_configuration.has_key ("radius"))
        {
          radius = a_configuration.fetch_real ("radius");
          if (radius <= 0.0)
            {
              ostringstream message;
              message << "cuts::test::sphere_cut::create: "
                      << "Invalid radius !"
                      << endl;
              throw logic_error (message.str ());
            }
        }

      if (a_configuration.has_flag ("reversed"))
        {
          reversed = true;
        }

      this->set_radius (radius);
      this->set_center (center[0], center[1], center[2]);
      this->set_reversed (reversed);

      _set_initialized (true);
      return;
    }

  } // end of namespace test

} // end of namespace cuts
