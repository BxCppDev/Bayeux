// -*- mode: c++ ; -*-
/* color.cc
 */

#include <stdexcept>

#include <geomtools/color.h>

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
  color::scoped_color_db_t color::g__color_db (new color_db ());

  // static:
  const color::color_db & color::get_color_db ()
  {
    if (! g__color_db)
      {
	throw runtime_error ("color::get_color_db: Library build critical bug !");
      }
    return *(g__color_db.get ());
  }

  int color::get_color (const string & name_)
  {
    return color::get_color_db ().get_color (name_);
  }

  int color::color_db::get_color (const string & name_) const
  {
    int acolor = (__map.find (color::default_color))->second;
    color_map_t::const_iterator i = __map.find (name_);
    if (i != __map.end ())
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
    __map[color::white]   = -2;
    __map[color::black]   = -1;
    __map[color::grey]    = 0;
    __map[color::red]     = 1;
    __map[color::green]   = 2;
    __map[color::blue]    = 3;
    __map[color::magenta] = 4;
    __map[color::cyan]    = 5;
    __map[color::yellow]  = 6;
    __map[color::orange]  = 8;

    if (devel)
      {
	clog << "DEVEL: color::color_db::color_db: Exiting. " << endl;
      }
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
    __map.clear ();
    if (devel)
      {
	clog << "DEVEL: color::color_db::~color_db: Exiting."
	     << endl;
      }
  }

  /********************************/

  color::color ()
  {
    //clog << "NOTICE: color::color: Entering..." << endl;
    code = 0;
    name = default_color;
    r = 0;
    g = 0;
    b = 0;
  }

  color::color (int code_, const string & name_, int r_, int g_, int b_)
  {
    code = code_;
    name = name_;
    r = r_;
    g = g_;
    b = b_;
  }


} // end of namespace geomtools

// end of color.cc
