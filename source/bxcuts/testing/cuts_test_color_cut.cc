// -*- mode: c++ ; -*-
/* cuts_test_color_cut.cc
 */

#include <cuts_test_color_cut.h>

#include <stdexcept>
#include <sstream>

namespace cuts {

  namespace test {

  using namespace std;

    // Registration instantiation macro :
    CUT_REGISTRATION_IMPLEMENT(color_cut, "cuts::test::color_cut");

    void color_cut::set_color (int color_)
    {
      _color_ = color_;
      return;
    }

    // ctor:
    CUT_CONSTRUCTOR_IMPLEMENT_HEAD (color_cut,a_logging_priority)
    {
      _color_ = data::BLACK;
      return;
    }

    // dtor:
    CUT_DEFAULT_DESTRUCTOR_IMPLEMENT (color_cut)

    CUT_RESET_IMPLEMENT_HEAD (color_cut)
    {
      _set_initialized (false);
      _color_ = data::BLACK;
      this->i_cut::_reset ();
      return;
    }

    CUT_ACCEPT_IMPLEMENT_HEAD (color_cut)
    {
      int result = SELECTION_ACCEPTED;
      DT_LOG_TRACE(_logging, "Entering...");
      const data & a_data = get_user_data<data>();
      int color = a_data.color;
      if (color != _color_)
        {
          result = SELECTION_REJECTED;
        }
      DT_LOG_TRACE(_logging, "Exiting.");
      return result;
    }

    // static method used within a cut factory:
    CUT_INITIALIZE_IMPLEMENT_HEAD (color_cut,
                                   a_configuration,
                                   a_service_manager,
                                   a_cut_dict)
    {
      using namespace std;
      if (is_initialized ())
        {
          std::ostringstream message;
          message << "cuts::test::color_cut::initialize: "
                  << "Cut '" << get_name () << "' is already initialized !";
          throw std::logic_error (message.str ());
        }
      _common_initialize(a_configuration);

      int color = data::BLACK;

      if (a_configuration.has_key ("color"))
        {
          string color_str = a_configuration.fetch_string ("color");
          if (color_str == "black")
            {
              color = data::BLACK;
            }
          else if (color_str == "red")
            {
              color = data::RED;
            }
          else if (color_str == "green")
            {
              color = data::GREEN;
            }
          else if (color_str == "blue")
            {
              color = data::BLUE;
            }
          else
            {
              ostringstream message;
              message << "cuts::test::color_cut::initialize: "
                      << "Invalid color label !"
                      << endl;
              throw logic_error (message.str ());
            }
        }

      this->set_color (color);

      _set_initialized (true);
      return;
    }

  } // end of namespace test

} // end of namespace cuts

// end of cuts_test_color_cut.cc
