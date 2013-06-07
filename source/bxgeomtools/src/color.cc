// -*- mode: c++ ; -*-
/* color.cc
 */

#include <geomtools/color.h>

#include <datatools/logger.h>

#include <iomanip>
#include <stdexcept>

namespace geomtools {

  // static
  const color::constants & color::constants::instance ()
  {
    static boost::scoped_ptr<color::constants> g_global_constants (0);
    if ( g_global_constants.get () == 0) {
      g_global_constants.reset (new color::constants);
    }
    return *g_global_constants.get ();
  }

  color::constants::constants ()
  {
    white   = "white";
    black   = "black";
    red     = "red";
    green   = "green";
    blue    = "blue";
    magenta = "magenta";
    cyan    = "cyan";
    yellow  = "yellow";
    orange  = "orange";
    grey    = "grey";
    default_color = grey;
    transparent = "transparent";
    return;
  }

  // static:
  const color::color_db & color::get_color_db ()
  {
    static color::scoped_color_db_type g_color_db (0);
    if (g_color_db.get () == 0)  {
      g_color_db.reset (new color_db ());
    }
    return *(g_color_db.get ());
  }

  int color::get_color (const std::string & name_)
  {
    return color::get_color_db ().get_color (name_);
  }

  int color::color_db::get_color (const std::string & name_) const
  {
    int a_color = (map_of_colors_.find (color::constants::instance().default_color))->second;
    color_map_type::const_iterator i = map_of_colors_.find (name_);
    if (i != map_of_colors_.end ()) {
      a_color = i->second;
    }
    return a_color;
  }

  color::color_db::color_db ()
  {
    datatools::logger::priority local_priority = datatools::logger::PRIO_FATAL;
    DT_LOG_TRACE (local_priority, "Entering...");
    map_of_colors_[color::constants::instance ().white]   = -2;
    map_of_colors_[color::constants::instance ().black]   = -1;
    map_of_colors_[color::constants::instance ().grey]    = 0;
    map_of_colors_[color::constants::instance ().red]     = 1;
    map_of_colors_[color::constants::instance ().green]   = 2;
    map_of_colors_[color::constants::instance ().blue]    = 3;
    map_of_colors_[color::constants::instance ().magenta] = 4;
    map_of_colors_[color::constants::instance ().cyan]    = 5;
    map_of_colors_[color::constants::instance ().yellow]  = 6;
    map_of_colors_[color::constants::instance ().orange]  = 8;
    DT_LOG_TRACE (local_priority, "Exiting.");
    return;
  }

  color::color_db::~color_db ()
  {
    datatools::logger::priority local_priority = datatools::logger::PRIO_FATAL;
    DT_LOG_TRACE (local_priority, "Entering...");
    DT_LOG_TRACE (local_priority, "Clearing the dictionary of colors...");
    map_of_colors_.clear ();
    DT_LOG_TRACE (local_priority, "Exiting.");
    return;
  }

  /********************************/

  color::color ()
  {
    code         = 0;
    name         = constants::instance().default_color;
    red_amount   = 0;
    green_amount = 0;
    blue_amount  = 0;
    alpha_amount = 1;
    return;
  }

  color::color (int code_, const std::string & name_, int red_, int green_, int blue_)
  {
    code         = code_;
    name         = name_;
    red_amount   = red_;
    green_amount = green_;
    blue_amount  = blue_;
    return;
  }

} // end of namespace geomtools

// end of color.cc
