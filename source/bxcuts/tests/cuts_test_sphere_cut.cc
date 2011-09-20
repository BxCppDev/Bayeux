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
    CUT_CONSTRUCTOR_IMPLEMENT_HEAD (sphere_cut,
                                    a_debug_devel,
                                    "cuts::test::sphere_cut",
                                    "Not cut",
                                    "1.0")
    {
      _radius_ = 1.0;
      _x0_ = 0.0;
      _y0_ = 0.0;
      _z0_ = 0.0;
      _reversed_ = false;
      return;
    }
  
    // dtor:
    CUT_DEFAULT_DESTRUCTOR_IMPLEMENT (sphere_cut)

    CUT_RESET_IMPLEMENT_HEAD (sphere_cut) 
    {
      _radius_ = 1.0;
      _x0_ = 0.0;
      _y0_ = 0.0;
      _z0_ = 0.0;
      _reversed_ = false;
      set_initialized_ (false);
      return;
    }

    CUT_ACCEPT_IMPLEMENT_HEAD(sphere_cut)
    {
      data * a_data = static_cast<data *> (_get_user_data ());
      double hx = a_data->x - _x0_;
      double hy = a_data->y - _y0_;
      double hz = a_data->z - _z0_;
      double r2 = (hx * hx + hy * hy + hz * hz);
      double rlim2 = _radius_ * _radius_;
      bool result = i_cut::ACCEPTED;
      if (r2 > rlim2) 
        {
          result = i_cut::REJECTED;
        }
      if (_reversed_)
        {
          result = (result == i_cut::REJECTED)? i_cut::ACCEPTED: i_cut::REJECTED;
        }
      return result;
    } 

    // static method used within a cut factory:
    CUT_INITIALIZE_IMPLEMENT_HEAD(sphere_cut,
                                  a_configuration,
                                  a_service_manager,
                                  a_cut_dict)
    {
      using namespace std;
      if (is_initialized ())
        {
          ostringstream message;
          message << "cuts::test::sphere_cut::initialize: "
                  << "Cut '" << get_name () << "' is already initialized ! ";
          throw logic_error (message.str ());
        }

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

      set_initialized_ (true);
      return;   
    }

  } // end of namespace test
  
} // end of namespace cuts

// end of cuts_test_sphere_cut.cc
