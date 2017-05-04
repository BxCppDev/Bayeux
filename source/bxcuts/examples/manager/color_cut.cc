// -*- mode: c++ ; -*-
/* color_cut.cc
 */

// Ourselves:
#include <color_cut.h>

// Standard library:
#include <stdexcept>
#include <sstream>

// This project:
#include <data.h>

// Registration instantiation macro :
CUT_REGISTRATION_IMPLEMENT(color_cut, "color_cut");

void color_cut::set_color (int color_)
{
  _color_ = color_;
  return;
}

color_cut::color_cut(datatools::logger::priority a_logger_priority)
  : i_cut(a_logger_priority)
{
  _color_ = data::COLOR_BLACK;
  return;
}

color_cut::~color_cut()
{
  if (is_initialized()) {
    this->color_cut::reset();
  }
  return;
}

void color_cut::initialize(const datatools::properties & a_configuration,
                            datatools::service_manager & /*a_service_manager*/,
                            cuts::cut_handle_dict_type & /*a_cut_dict*/)
{
  DT_THROW_IF(is_initialized (),
              std::logic_error,
              "Color cut named '" << (has_name()?get_name ():"?" )
              << "' is already initialized !");
  _common_initialize(a_configuration);

  int color = data::COLOR_BLACK;
  if (a_configuration.has_key ("color")) {
    std::string color_str = a_configuration.fetch_string ("color");
    if (color_str == "black") {
      color = data::COLOR_BLACK;
    } else if (color_str == "red") {
      color = data::COLOR_RED;
    } else if (color_str == "green") {
      color = data::COLOR_GREEN;
    } else if (color_str == "blue") {
      color = data::COLOR_BLUE;
    } else {
      DT_THROW_IF(true,
                  std::domain_error,
                  "Invalid color label '" << color_str << "' !");
    }
  }
  this->set_color (color);
  _set_initialized (true);
  return;
}

void color_cut::reset()
{
  _set_initialized(false);
  _color_ = data::COLOR_BLACK;
  this->i_cut::_reset();
  return;
}

int color_cut::_accept()
{
  DT_THROW_IF(! is_initialized (),
              std::logic_error,
              "Color cut named '" << (has_name()?get_name ():"?" )
              << "' is not initialized !");
  if (_color_ == data::COLOR_INVALID) {
    return cuts::SELECTION_INAPPLICABLE;
  }
  const data & a_data = get_user_data<data>();
  int color = a_data.color;
  int result = cuts::SELECTION_ACCEPTED;
  if (color != _color_) {
    result = cuts::SELECTION_REJECTED;
  }
  return result;
}
