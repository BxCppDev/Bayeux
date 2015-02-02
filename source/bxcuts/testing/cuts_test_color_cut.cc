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
    color_cut::color_cut(datatools::logger::priority a_logger_priority)
    : i_cut(a_logger_priority)

    {
      _color_ = data::BLACK;
    }

    // dtor:
    color_cut::~color_cut()
    {
      if (is_initialized()) {
        this->color_cut::reset();
      }
    }


    void color_cut::reset()
    {
      _set_initialized (false);
      _color_ = data::BLACK;
      this->i_cut::_reset();
    }

    int color_cut::_accept()
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
    void color_cut::initialize(const datatools::properties& a_configuration,
                               datatools::service_manager& /*a_service_manager*/,
                               cuts::cut_handle_dict_type& /*a_cut_dict*/)
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

    void color_cut::export_to_config(datatools::properties & config_,
                                     uint32_t flags_,
                                     const std::string & prefix_) const
    {
      this->cuts::i_cut::export_to_config(config_,flags_, prefix_);

      std::string color_str = "black";
      if (_color_ == data::BLACK) {
        color_str = "black";
      } else if (_color_ == data::RED) {
        color_str = "red";
      } else if (_color_ == data::GREEN) {
        color_str = "green";
      } else if (_color_ == data::BLUE) {
        color_str = "blue";
      }
      config_.store(prefix_ + "color", color_str, "The requested color for the selected points");
      return;
    }

  } // end of namespace test

} // end of namespace cuts
