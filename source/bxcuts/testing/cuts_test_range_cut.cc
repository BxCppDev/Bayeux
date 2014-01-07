// -*- mode: c++ ; -*-
/* cuts_test_range_cut.cc
 */

#include <cuts_test_range_cut.h>

#include <stdexcept>
#include <sstream>

namespace cuts {

  namespace test {

    using namespace std;

    // Registration instantiation macro :
    CUT_REGISTRATION_IMPLEMENT(range_cut, "cuts::test::range_cut");

    void range_cut::set_mode (int mode_)
    {
      _mode_ = mode_;
      return;
    }

    void range_cut::set_range (double min_, double max_)
    {
      _min_ = min_;
      _max_ = max_;
      return;
    }

    void range_cut::set_reversed (bool reversed_)
    {
      _reversed_ = reversed_;
      return;
    }

    // ctor:
    range_cut::range_cut(datatools::logger::priority a_logger_priority)
    : i_cut(a_logger_priority)
    {
      _mode_ = MODE_X;
      _min_ = 0.0;
      _max_ = 1.0;
      _reversed_ = false;
      return;
    }

    range_cut::~range_cut()
    {
      if (is_initialized()) {
        this->range_cut::reset();
      }
    }


    /*
    CUT_DESTRUCTOR_IMPLEMENT_HEAD(range_cut)
    {
      if (is_initialized ()) {
        this->range_cut::reset ();
      }
      return;
    }
    */

    void range_cut::reset()
    {
      _set_initialized (false);
      _mode_ = MODE_X;
      _min_ = 0.0;
      _max_ = 1.0;
      _reversed_ = false;
      i_cut::_reset();
      return;
    }

    int range_cut::_accept()
    {
      int result = SELECTION_ACCEPTED;
      DT_LOG_TRACE(_logging, "Entering...");
      const data & a_data = get_user_data<data>();
      double t;
      if (_mode_ == MODE_X)
        {
          t = a_data.x;
        }
      else if (_mode_ == MODE_Y)
        {
          t = a_data.y;
        }
      else if (_mode_ == MODE_Z)
        {
          t = a_data.z;
        }

      if (t < _min_)
        {
          result = SELECTION_REJECTED;
        }
      else if (t > _max_)
        {
          result = SELECTION_REJECTED;
        }
      if (_reversed_)
        {
          result = (result == SELECTION_REJECTED)?
            SELECTION_ACCEPTED: SELECTION_REJECTED;
        }
      DT_LOG_TRACE(_logging, "Exiting with selection=" << result << ".");
      return result;
    }

    // static method used within a cut factory:
    /*
     *  CUT_INITIALIZE_IMPLEMENT_HEAD(range_cut,
     *                          a_configuration,
     *                          a_service_manager,
     *                          a_cut_dict)
     */
    void range_cut::initialize (const datatools::properties & a_configuration,
                                datatools::service_manager & a_service_manager,
                                cuts::cut_handle_dict_type & a_cut_dict)
    {
      using namespace std;
      if (is_initialized ())
        {
          ostringstream message;
          message << "cuts::test::range_cut::initialize: "
                  << "Cut '" << get_name () << "' is already initialized !";
          throw logic_error (message.str ());
        }
      _common_initialize(a_configuration);

      int    mode     = range_cut::MODE_X;
      double min      = 0.0;
      double max      = 1.0;
      bool   reversed = false;

      if (a_configuration.has_key ("mode"))
        {
          string mode_str = a_configuration.fetch_string ("mode");
          if (mode_str == "x")
            {
              mode = range_cut::MODE_X;
            }
          else if (mode_str == "y")
            {
              mode = range_cut::MODE_Y;
            }
          else if (mode_str == "z")
            {
              mode = range_cut::MODE_Z;
            }
          else
            {
              ostringstream message;
              message << "cuts::test::range_cut::initialize: "
                      << "Invalid mode '" << mode_str << "' !";
            }
        }

      if (a_configuration.has_key ("min"))
        {
          min = a_configuration.fetch_real ("min");
        }

      if (a_configuration.has_key ("max"))
        {
          max = a_configuration.fetch_real ("max");
        }

      if (a_configuration.has_flag ("reversed"))
        {
          reversed = true;
        }

      this->set_mode (mode);
      this->set_range (min, max);
      this->set_reversed (reversed);

      _set_initialized (true);
      return;
    }

  } // end of namespace test

} // end of namespace cuts

// end of cuts_test_range_cut.cc
