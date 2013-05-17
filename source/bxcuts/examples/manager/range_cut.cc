// -*- mode: c++ ; -*-
/* range_cut.cc
 */

#include <range_cut.h>

#include <stdexcept>
#include <sstream>

#include <datatools/exception.h>

#include <data.h>

// Registration instantiation macro :
CUT_REGISTRATION_IMPLEMENT(range_cut, "range_cut");

void range_cut::reset_mode()
{
  _mode_ = MODE_INVALID;
}

void range_cut::set_mode(int mode_)
{
  DT_THROW_IF(mode_ < MODE_X && mode_ > MODE_Z,
              std::domain_error,
              "Range cut named '" <<(has_name()?get_name():"?")
              << "' has invalid mode !");
  _mode_ = mode_;
  return;
}

void range_cut::set_range(double min_, double max_)
{
  DT_THROW_IF(min_ >= max_,
              std::domain_error,
              "Range cut named '" <<(has_name()?get_name():"?")
              << "' has invalid min/max range !");
  _min_ = min_;
  _max_ = max_;
  return;
}

void range_cut::set_reversed(bool reversed_)
{
  _reversed_ = reversed_;
  return;
}


// CUT_CONSTRUCTOR_IMPLEMENT_HEAD(range_cut,a_logger_priority)
range_cut::range_cut(datatools::logger::priority a_logger_priority)
  : i_cut(a_logger_priority)
{
  _mode_ = MODE_X;
  _min_ = 0.0;
  _max_ = 1.0;
  _reversed_ = false;
  return;
}

// CUT_DEFAULT_DESTRUCTOR_IMPLEMENT(range_cut)
range_cut::~range_cut()
{
  if (is_initialized()) {
    this->range_cut::reset();
  }
  return;
}

// static method used within a cut factory:
/*
 *  CUT_INITIALIZE_IMPLEMENT_HEAD(range_cut,
 *                          a_configuration,
 *                          a_service_manager,
 *                          a_cut_dict)
 */
void range_cut::initialize(const datatools::properties & a_configuration,
                            datatools::service_manager & a_service_manager,
                            cuts::cut_handle_dict_type & a_cut_dict)
{
  DT_THROW_IF(is_initialized(),
              std::logic_error,
              "Range cut named '" <<(has_name()?get_name():"?" )
              << "' is already initialized !");
  _common_initialize(a_configuration);

  int    mode     = range_cut::MODE_X;
  double min      = 0.0;
  double max      = 1.0;
  bool   reversed = false;

  if (a_configuration.has_key("mode")) {
    std::string mode_str = a_configuration.fetch_string("mode");
    if (mode_str == "x") {
      mode = range_cut::MODE_X;
    } else if (mode_str == "y") {
      mode = range_cut::MODE_Y;
    } else if (mode_str == "z") {
      mode = range_cut::MODE_Z;
    } else {
      DT_THROW_IF(true,
                  std::domain_error,
                  "Invalid mode '" << mode_str << "' !");
    }
  }

  if (a_configuration.has_key("min")) {
    min = a_configuration.fetch_real("min");
  }

  if (a_configuration.has_key("max")) {
    max = a_configuration.fetch_real("max");
  }

  if (a_configuration.has_flag("reversed")) {
    reversed = true;
  }

  this->set_mode(mode);
  this->set_range(min, max);
  this->set_reversed(reversed);

  _set_initialized(true);
  return;
}


// CUT_RESET_IMPLEMENT_HEAD(range_cut)
void range_cut::reset()
{
  _set_initialized(false);
  _mode_ = MODE_X;
  _min_ = 0.0;
  _max_ = 1.0;
  _reversed_ = false;
  this->i_cut::_reset();
  return;
}


// CUT_ACCEPT_IMPLEMENT_HEAD(range_cut)
int range_cut::_accept()
{
  DT_THROW_IF(! is_initialized(),
              std::logic_error,
              "Range cut named '" <<(has_name()?get_name():"?" )
              << "' is not initialized !");
  const data & a_data = get_user_data<data>();
  double t;
  if (_mode_ == MODE_X) {
    t = a_data.x;
  } else if (_mode_ == MODE_Y) {
    t = a_data.y;
  } else if (_mode_ == MODE_Z) {
    t = a_data.z;
  } else {
    return cuts::SELECTION_INAPPLICABLE;
  }
  int result = cuts::SELECTION_ACCEPTED;
  if (t < _min_) {
    result = cuts::SELECTION_REJECTED;
  } else if (t > _max_) {
    result = cuts::SELECTION_REJECTED;
  }
  if (_reversed_) {
    result = (result == cuts::SELECTION_REJECTED)? cuts::SELECTION_ACCEPTED: cuts::SELECTION_REJECTED;
  }
  return result;
}

// end of range_cut.cc
