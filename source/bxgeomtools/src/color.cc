// -*- mode: c++ ; -*-
/* color.cc
 */

#include <geomtools/color.h>
#include <iomanip>

#include <stdexcept>

//#include <datatools/properties.h>

namespace geomtools {

  using namespace std;

  // static
  const color::constants &
  color::constants::instance ()
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

  int color::get_color (const string & a_name)
  {
    return color::get_color_db ().get_color (a_name);
  }

  int color::color_db::get_color (const string & a_name) const
  {
    int acolor = (map_of_colors_.find (color::constants::instance().default_color))->second;
    color_map_type::const_iterator i = map_of_colors_.find (a_name);
    if (i != map_of_colors_.end ()) {
      acolor = i->second;
    }
    return acolor;
  }

  color::color_db::color_db ()
  {
    bool devel = false;
    //devel = true;
    // if (devel) {
    //     clog << "DEVEL: color::color_db::color_db: Entering..." << endl;
    //   }
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

    // if (devel)
    //   {
    //     clog << "DEVEL: color::color_db::color_db: Exiting. " << endl;
    //   }
    return;
  }

  color::color_db::~color_db ()
  {
    bool devel = false;
    //devel = true;
    // if (devel)
    //   {
    //     clog << "DEVEL: color::color_db::~color_db: Entering..."
    //          << endl;
    //     clog << "DEVEL: color::color_db::~color_db: Clearing the dictionary of colors..."
    //          << endl;
    //   }
    map_of_colors_.clear ();
    // if (devel)
    //   {
    //     clog << "DEVEL: color::color_db::~color_db: Exiting."
    //          << endl;
    //   }
    return;
  }

  /********************************/

  color::color ()
  {
    //clog << "NOTICE: color::color: Entering..." << endl;
    code = 0;
    name = constants::instance().default_color;
    red_amount = 0;
    green_amount = 0;
    blue_amount = 0;
    alpha_amount = 1;
    return;
  }

  color::color (int a_code, const string & a_name, int a_red, int a_green, int a_blue)
  {
    code = a_code;
    name = a_name;
    red_amount = a_red;
    green_amount = a_green;
    blue_amount = a_blue;
    return;
  }


} // end of namespace geomtools

// end of color.cc
