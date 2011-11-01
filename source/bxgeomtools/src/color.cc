// -*- mode: c++ ; -*- 
/* color.cc
 */ 

#include <geomtools/color.h>
#include <iomanip>

#include <stdexcept>

#include <datatools/utils/properties.h>

namespace geomtools {

  using namespace std;

  const string color::white   = "white";
  const string color::black   = "black";
  const string color::red     = "red";
  const string color::green   = "green";
  const string color::blue    = "blue";
  const string color::magenta = "magenta";
  const string color::cyan    = "cyan";
  const string color::yellow  = "yellow";
  const string color::orange  = "orange";
  const string color::grey    = "grey";
  const string color::default_color = color::grey;
 
  // static:
  color::scoped_color_db_type color::_g_color_db_ (new color_db ());

  // static:
  const color::color_db & color::get_color_db ()
  {
    if (! _g_color_db_) 
      {
	throw runtime_error ("color::get_color_db: Library build critical bug !");
      }
    return *(_g_color_db_.get ());
  }

  int color::get_color (const string & a_name) 
  {
    return color::get_color_db ().get_color (a_name);
  }

  int color::color_db::get_color (const string & a_name) const
  {
    int acolor = (map_of_colors_.find (color::default_color))->second;
    color_map_type::const_iterator i = map_of_colors_.find (a_name);
    if (i != map_of_colors_.end ())
      {
	acolor = i->second;
      }
    return acolor;
  }
 
  color::color_db::color_db ()
  {
    bool devel = false;
    //devel = true;
    if (devel)
      {
	clog << "DEVEL: color::color_db::color_db: Entering..." << endl; 
      }
    map_of_colors_[color::white]   = -2;
    map_of_colors_[color::black]   = -1;
    map_of_colors_[color::grey]    = 0; 
    map_of_colors_[color::red]     = 1;
    map_of_colors_[color::green]   = 2;
    map_of_colors_[color::blue]    = 3;
    map_of_colors_[color::magenta] = 4;
    map_of_colors_[color::cyan]    = 5;
    map_of_colors_[color::yellow]  = 6;
    map_of_colors_[color::orange]  = 8;
 
    if (devel)
      {
	clog << "DEVEL: color::color_db::color_db: Exiting. " << endl;
      }
    return;
  }

  color::color_db::~color_db ()
  {
    bool devel = false;
    //devel = true;
    if (devel)
      {
	clog << "DEVEL: color::color_db::~color_db: Entering..." 
	     << endl;
	clog << "DEVEL: color::color_db::~color_db: Clearing the dictionary of colors..." 
	     << endl;
      }
    map_of_colors_.clear ();
    if (devel)
      {
	clog << "DEVEL: color::color_db::~color_db: Exiting." 
	     << endl;
      }
    return;
  }

  /********************************/

  color::color ()
  {
    //clog << "NOTICE: color::color: Entering..." << endl;
    code = 0;
    name = default_color;
    red_amount = 0;
    green_amount = 0;
    blue_amount = 0;
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
