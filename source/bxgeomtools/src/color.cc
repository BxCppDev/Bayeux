// -*- mode: c++ ; -*-
/* color.cc
 */

// Ourselves:
#include <geomtools/color.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/logger.h>

// Standard library:
#include <iomanip>
#include <stdexcept>

namespace geomtools {

  // static
  const std::string & color::invalid()
  {
    return constants::instance().invalid;
  }

  // static
  const std::string & color::transparent()
  {
    return constants::instance().transparent;
  }

  // static
  const std::string & color::white()
  {
    return constants::instance().white;
  }

  // static
  const std::string & color::black()
  {
    return constants::instance().black;
  }

  // static
  const std::string & color::grey()
  {
    return constants::instance().grey;
  }

  // static
  const std::string & color::red()
  {
    return constants::instance().red;
  }

  // static
  const std::string & color::green()
  {
    return constants::instance().green;
  }

  // static
  const std::string & color::blue()
  {
    return constants::instance().blue;
  }

  // static
  const std::string & color::magenta()
  {
    return constants::instance().magenta;
  }

  // static
  const std::string & color::cyan()
  {
    return constants::instance().cyan;
  }

  // static
  const std::string & color::yellow()
  {
    return constants::instance().yellow;
  }

  // static
  const std::string & color::orange()
  {
    return constants::instance().orange;
  }

  // static
  const std::string & color::default_color()
  {
    return constants::instance().default_color;
  }

  // static
  const color::constants & color::constants::instance()
  {
    static boost::scoped_ptr<color::constants> g_global_constants(0);
    if ( g_global_constants.get() == 0) {
      g_global_constants.reset(new color::constants);
    }
    return *g_global_constants.get();
  }

  color::constants::constants()
  {
    invalid = "invalid";
    white   = "white";
    black   = "black";
    grey    = "grey";
    red     = "red";
    green   = "green";
    blue    = "blue";
    magenta = "magenta";
    cyan    = "cyan";
    yellow  = "yellow";
    orange  = "orange";
    default_color = grey;
    transparent   = "transparent";
    return;
  }

  // static:
  const color::color_db & color::get_color_db()
  {
    static color::scoped_color_db_type g_color_db(0);
    if (g_color_db.get() == 0)  {
      g_color_db.reset (new color_db());
    }
    return *(g_color_db.get());
  }

  color::code_type color::get_color(const std::string & name_)
  {
    return color::get_color_db().get_color(name_);
  }

  color::code_type color::color_db::get_color(const std::string & name_) const
  {
    color::code_type a_color = (map_of_colors_.find(color::constants::instance().default_color))->second;
    color_map_type::const_iterator i = map_of_colors_.find(name_);
    if (i != map_of_colors_.end()) {
      a_color = i->second;
    }
    return a_color;
  }

  color::color_db::color_db ()
  {
    //datatools::logger::priority local_priority = datatools::logger::PRIO_FATAL;
    //DT_LOG_TRACE (local_priority, "Entering...");
    map_of_colors_[color::constants::instance().white]   = COLOR_WHITE;
    map_of_colors_[color::constants::instance().black]   = COLOR_BLACK;
    map_of_colors_[color::constants::instance().grey]    = COLOR_GREY;
    map_of_colors_[color::constants::instance().red]     = COLOR_RED;
    map_of_colors_[color::constants::instance().green]   = COLOR_GREEN;
    map_of_colors_[color::constants::instance().blue]    = COLOR_BLUE;
    map_of_colors_[color::constants::instance().magenta] = COLOR_MAGENTA;
    map_of_colors_[color::constants::instance().cyan]    = COLOR_CYAN;
    map_of_colors_[color::constants::instance().yellow]  = COLOR_YELLOW;
    map_of_colors_[color::constants::instance().orange]  = COLOR_ORANGE;
    //DT_LOG_TRACE (local_priority, "Exiting.");
    return;
  }

  color::color_db::~color_db ()
  {
    //datatools::logger::priority local_priority = datatools::logger::PRIO_FATAL;
    //DT_LOG_TRACE (local_priority, "Entering...");
    //DT_LOG_TRACE (local_priority, "Clearing the dictionary of colors...");
    map_of_colors_.clear ();
    //DT_LOG_TRACE (local_priority, "Exiting.");
    return;
  }

  color::color()
  {
    code         = COLOR_DEFAULT;
    name         = constants::instance().default_color;
    red_amount   = 0;
    green_amount = 0;
    blue_amount  = 0;
    alpha_amount = 1;
    return;
  }

  color::color(color::code_type code_,
               const std::string & name_,
               int32_t red_, int32_t green_, int32_t blue_,
               int32_t alpha_)
  {
    code         = code_;
    name         = name_;
    red_amount   = red_;
    green_amount = green_;
    blue_amount  = blue_;
    alpha_amount = alpha_;
    return;
  }


  color::context::context()
  {
    reset();
    return;
  }

  void color::context::reset()
  {
    _encoding_     = ENCODING_NONE;
    _color_code_   = COLOR_INVALID;
    _color_value_  = 1.0;
    _color_name_   = "";
    _transparency_ = 1.0;
    return;
  }

  bool color::context::is_activated() const
  {
    return _encoding_ > ENCODING_NONE && _encoding_ <= ENCODING_BY_NAME;
  }

  void color::context::deactivate()
  {
    _encoding_ = ENCODING_NONE;
    return;
  }

  bool color::context::encoded_by_code() const
  {
    return _encoding_ == ENCODING_BY_CODE;
  }

  void color::context::set_color_code(color::code_type code_)
  {
    _color_code_ = code_;
    _encoding_ = ENCODING_BY_CODE;
    return;
  }

  color::code_type color::context::get_color_code() const
  {
    return _color_code_;
  }

  bool color::context::encoded_by_value() const
  {
    return _encoding_ == ENCODING_BY_VALUE;
  }

  void color::context::set_color_value(double value_)
  {
    _color_value_ = value_;
    _encoding_ = ENCODING_BY_VALUE;
    return;
  }

  double color::context::get_color_value() const
  {
    return _color_value_;
  }

  bool color::context::encoded_by_name() const
  {
    return _encoding_ == ENCODING_BY_NAME;
  }

  void color::context::set_color_name(const std::string & name_)
  {
    _color_name_ = name_;
    _encoding_ = ENCODING_BY_NAME;
    return;
  }

  const std::string & color::context::get_color_name() const
  {
    return _color_name_;
  }

  std::string color::context::str() const
  {
    std::ostringstream ss;
    if (encoded_by_code()) {
      ss << _color_code_;
    } else if (encoded_by_value()) {
      ss.precision(15);
      ss << _color_value_;
    } else if (encoded_by_name()) {
      ss << color::get_color(_color_name_);
    }
    return ss.str();
  }

} // end of namespace geomtools
