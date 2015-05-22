// -*- mode: c++ ; -*-
/* gnuplot_i.cc
 */

#include <geomtools/gnuplot_i.h>
#include <geomtools/geomtools_config.h>

#include <datatools/exception.h>

using namespace std;

GnuplotException::GnuplotException(const std::string &msg)
  : std::runtime_error  (msg)
{
}

//------------------------------------------------------------------------------
//
// initialize static data
//
int Gnuplot::_g_tmpfile_num_ = 0;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
std::string Gnuplot::_g_gnuplot_filename_ = "pgnuplot.exe";
std::string Gnuplot::_g_gnuplot_path = "C:/program files/gnuplot/bin/";
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
std::string Gnuplot::_g_gnuplot_filename_ = "gnuplot";
std::string Gnuplot::_g_gnuplot_path = "/usr/local/bin/";
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
std::string Gnuplot::_g_terminal_std_ = "windows";
#elif ( defined(unix) || defined(__unix) || defined(__unix__) ) && !defined(__APPLE__)
std::string Gnuplot::_g_terminal_std_ = "x11";
#elif defined(__APPLE__)
std::string Gnuplot::_g_terminal_std_ = "aqua";
#endif

//------------------------------------------------------------------------------
//
// constructor: set a style during construction
//
Gnuplot::Gnuplot(const std::string &style)
  :_gnucmd_(NULL) ,_valid_(false) ,_two_dim_(false) ,_nplots_(0)

{
  init();
  set_style(style);
}

//------------------------------------------------------------------------------
//
// constructor: open a new session, plot a signal (x)
//
Gnuplot::Gnuplot(const std::vector<double> &x,
                 const std::string &title,
                 const std::string &style,
                 const std::string &labelx,
                 const std::string &labely)
  :_gnucmd_(NULL) ,_valid_(false) ,_two_dim_(false) ,_nplots_(0)
{
  init();

  set_style(style);
  set_xlabel(labelx);
  set_ylabel(labely);

  plot_x(x,title);
}


//------------------------------------------------------------------------------
//
// constructor: open a new session, plot a signal (x,y)
//
Gnuplot::Gnuplot(const std::vector<double> &x,
                 const std::vector<double> &y,
                 const std::string &title,
                 const std::string &style,
                 const std::string &labelx,
                 const std::string &labely)
  :_gnucmd_(NULL) ,_valid_(false) ,_two_dim_(false) ,_nplots_(0)
{
  init();

  set_style(style);
  set_xlabel(labelx);
  set_ylabel(labely);

  plot_xy(x,y,title);
}


//------------------------------------------------------------------------------
//
// constructor: open a new session, plot a signal (x,y,z)
//
Gnuplot::Gnuplot(const std::vector<double> &x,
                 const std::vector<double> &y,
                 const std::vector<double> &z,
                 const std::string &title,
                 const std::string &style,
                 const std::string &labelx,
                 const std::string &labely,
                 const std::string &labelz)
  :_gnucmd_(NULL) ,_valid_(false) ,_two_dim_(false) ,_nplots_(0)
{
  init();

  set_style(style);
  set_xlabel(labelx);
  set_ylabel(labely);
  set_zlabel(labelz);

  plot_xyz(x,y,z,title);
}


//------------------------------------------------------------------------------
//
// define static member function: set Gnuplot path manual
//   for windows: path with slash '/' not backslash '\'
//
bool Gnuplot::set_gnuplot_path(const std::string &path)
{

  std::string tmp = path + "/" + Gnuplot::_g_gnuplot_filename_;


#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
  if ( Gnuplot::file_exists(tmp,0) ) // check existence
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
    if ( Gnuplot::file_exists(tmp,1) ) // check existence and execution permission
#endif
      {
        Gnuplot::_g_gnuplot_path = path;
        return true;
      }
    else
      {
        Gnuplot::_g_gnuplot_path.clear();
        return false;
      }
}


//------------------------------------------------------------------------------
//
// define static member function: set standart terminal, used by showonscreen
//  defaults: Windows - win, Linux - x11, Mac - aqua
//
void Gnuplot::set_terminal_std(const std::string &type)
{
#if defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
  DT_THROW_IF (type.find("x11") != std::string::npos && getenv("DISPLAY") == NULL, GnuplotException, "Can't find DISPLAY variable");
#endif


  Gnuplot::_g_terminal_std_ = type;
  return;
}


//------------------------------------------------------------------------------
//
// A string tokenizer taken from http://www.sunsite.ualberta.ca/Documentation/
// /Gnu/libstdc++-2.90.8/html/21_strings/stringtok_std_h.txt
//
template <typename Container>
void stringtok (Container &container,
                std::string const &in,
                const char * const delimiters = " \t\n")
{
  const std::string::size_type len = in.length();
  std::string::size_type i = 0;

  while ( i < len )
    {
      // eat leading whitespace
      i = in.find_first_not_of (delimiters, i);

      if (i == std::string::npos)
        return;   // nothing left but white space

      // find the end of the token
      std::string::size_type j = in.find_first_of (delimiters, i);

      // push token
      if (j == std::string::npos)
        {
          container.push_back (in.substr(i));
          return;
        }
      else
        container.push_back (in.substr(i, j-i));

      // set up for next loop
      i = j + 1;
    }

  return;
}


//------------------------------------------------------------------------------
//
// Destructor: needed to delete temporary files
//
Gnuplot::~Gnuplot()
{
  remove_tmpfiles();

  // A stream opened by popen() should be closed by pclose()
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
  if (_pclose(_gnucmd_) == -1)
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
    if (pclose(_gnucmd_) == -1)
#endif
      DT_THROW_IF(true, GnuplotException, "Problem closing communication to gnuplot");
}


//------------------------------------------------------------------------------
//
// Resets a gnuplot session (next plot will erase previous ones)
//
Gnuplot& Gnuplot::reset_plot()
{
  remove_tmpfiles();

  _nplots_ = 0;

  return *this;
}


//------------------------------------------------------------------------------
//
// resets a gnuplot session and sets all varibles to default
//
Gnuplot& Gnuplot::reset_all()
{
  remove_tmpfiles();

  _nplots_ = 0;
  cmd("reset");
  cmd("clear");
  _pstyle_ = "points";
  _smooth_ = "";
  showonscreen();

  return *this;
}


//------------------------------------------------------------------------------
//
// Change the plotting style of a gnuplot session
//
Gnuplot& Gnuplot::set_style(const std::string &stylestr)
{
  if (stylestr.find("lines")          == std::string::npos  &&
      stylestr.find("points")         == std::string::npos  &&
      stylestr.find("linespoints")    == std::string::npos  &&
      stylestr.find("impulses")       == std::string::npos  &&
      stylestr.find("dots")           == std::string::npos  &&
      stylestr.find("steps")          == std::string::npos  &&
      stylestr.find("fsteps")         == std::string::npos  &&
      stylestr.find("histeps")        == std::string::npos  &&
      stylestr.find("boxes")          == std::string::npos  &&  // 1-4 columns of data are required
      stylestr.find("filledcurves")   == std::string::npos  &&
      stylestr.find("histograms")     == std::string::npos  )   //only for one data column
    //        stylestr.find("labels")         == std::string::npos  &&  // 3 columns of data are required
    //        stylestr.find("xerrorbars")     == std::string::npos  &&  // 3-4 columns of data are required
    //        stylestr.find("xerrorlines")    == std::string::npos  &&  // 3-4 columns of data are required
    //        stylestr.find("errorbars")      == std::string::npos  &&  // 3-4 columns of data are required
    //        stylestr.find("errorlines")     == std::string::npos  &&  // 3-4 columns of data are required
    //        stylestr.find("yerrorbars")     == std::string::npos  &&  // 3-4 columns of data are required
    //        stylestr.find("yerrorlines")    == std::string::npos  &&  // 3-4 columns of data are required
    //        stylestr.find("boxerrorbars")   == std::string::npos  &&  // 3-5 columns of data are required
    //        stylestr.find("xyerrorbars")    == std::string::npos  &&  // 4,6,7 columns of data are required
    //        stylestr.find("xyerrorlines")   == std::string::npos  &&  // 4,6,7 columns of data are required
    //        stylestr.find("boxxyerrorbars") == std::string::npos  &&  // 4,6,7 columns of data are required
    //        stylestr.find("financebars")    == std::string::npos  &&  // 5 columns of data are required
    //        stylestr.find("candlesticks")   == std::string::npos  &&  // 5 columns of data are required
    //        stylestr.find("vectors")        == std::string::npos  &&
    //        stylestr.find("image")          == std::string::npos  &&
    //        stylestr.find("rgbimage")       == std::string::npos  &&
    //        stylestr.find("pm3d")           == std::string::npos  )
    {
      _pstyle_ = std::string("points");
    }
  else
    {
      _pstyle_ = stylestr;
    }

  return *this;
}


//------------------------------------------------------------------------------
//
// smooth: interpolation and approximation of data
//
Gnuplot& Gnuplot::set_smooth(const std::string &stylestr)
{
  if (stylestr.find("unique")    == std::string::npos  &&
      stylestr.find("frequency") == std::string::npos  &&
      stylestr.find("csplines")  == std::string::npos  &&
      stylestr.find("acsplines") == std::string::npos  &&
      stylestr.find("bezier")    == std::string::npos  &&
      stylestr.find("sbezier")   == std::string::npos  )
    {
      _smooth_ = "";
    }
  else
    {
      _smooth_ = stylestr;
    }

  return *this;
}


//------------------------------------------------------------------------------
//
// sets terminal type to windows / x11
//
Gnuplot& Gnuplot::showonscreen()
{
  cmd("set output");
  cmd("set terminal " + Gnuplot::_g_terminal_std_);

  return *this;
}

//------------------------------------------------------------------------------
//
// saves a gnuplot session to a postscript file
//
Gnuplot& Gnuplot::savetops(const std::string &filename)
{
  cmd("set terminal postscript solid color enhanced");

  std::ostringstream cmdstr;
  cmdstr << "set output \"" << filename << ".ps\"";
  cmd(cmdstr.str());

  return *this;
}

//------------------------------------------------------------------------------
//
// Switches legend on
//
Gnuplot& Gnuplot::set_legend(const std::string &position)
{
  std::ostringstream cmdstr;
  cmdstr << "set key " << position;

  cmd(cmdstr.str());

  return *this;
}

//------------------------------------------------------------------------------
//
// turns on log scaling for the x axis
//
Gnuplot& Gnuplot::set_xlogscale(const double base)
{
  std::ostringstream cmdstr;

  cmdstr << "set logscale x " << base;
  cmd(cmdstr.str());

  return *this;
}

//------------------------------------------------------------------------------
//
// turns on log scaling for the y axis
//
Gnuplot& Gnuplot::set_ylogscale(const double base)
{
  std::ostringstream cmdstr;

  cmdstr << "set logscale y " << base;
  cmd(cmdstr.str());

  return *this;
}

//------------------------------------------------------------------------------
//
// turns on log scaling for the z axis
//
Gnuplot& Gnuplot::set_zlogscale(const double base)
{
  std::ostringstream cmdstr;

  cmdstr << "set logscale z " << base;
  cmd(cmdstr.str());

  return *this;
}

//------------------------------------------------------------------------------
//
// scales the size of the points used in plots
//
Gnuplot& Gnuplot::set_pointsize(const double pointsize)
{
  std::ostringstream cmdstr;
  cmdstr << "set pointsize " << pointsize;
  cmd(cmdstr.str());

  return *this;
}

//------------------------------------------------------------------------------
//
// set isoline density (grid) for plotting functions as surfaces
//
Gnuplot& Gnuplot::set_samples(const int samples)
{
  std::ostringstream cmdstr;
  cmdstr << "set samples " << samples;
  cmd(cmdstr.str());

  return *this;
}


//------------------------------------------------------------------------------
//
// set isoline density (grid) for plotting functions as surfaces
//
Gnuplot& Gnuplot::set_isosamples(const int isolines)
{
  std::ostringstream cmdstr;
  cmdstr << "set isosamples " << isolines;
  cmd(cmdstr.str());

  return *this;
}


//------------------------------------------------------------------------------
//
// enables contour drawing for surfaces set contour {base | surface | both}
//

Gnuplot& Gnuplot::set_contour(const std::string &position)
{
  if (position.find("base")    == std::string::npos  &&
      position.find("surface") == std::string::npos  &&
      position.find("both")    == std::string::npos  )
    {
      cmd("set contour base");
    }
  else
    {
      cmd("set contour " + position);
    }

  return *this;
}

//------------------------------------------------------------------------------
//
// set labels
//
// set the xlabel
Gnuplot& Gnuplot::set_xlabel(const std::string &label)
{
  std::ostringstream cmdstr;

  cmdstr << "set xlabel \"" << label << "\"";
  cmd(cmdstr.str());

  return *this;
}

//------------------------------------------------------------------------------
// set the ylabel
//
Gnuplot& Gnuplot::set_ylabel(const std::string &label)
{
  std::ostringstream cmdstr;

  cmdstr << "set ylabel \"" << label << "\"";
  cmd(cmdstr.str());

  return *this;
}

//------------------------------------------------------------------------------
// set the zlabel
//
Gnuplot& Gnuplot::set_zlabel(const std::string &label)
{
  std::ostringstream cmdstr;

  cmdstr << "set zlabel \"" << label << "\"";
  cmd(cmdstr.str());

  return *this;
}

//------------------------------------------------------------------------------
//
// set range
//
// set the xrange
Gnuplot& Gnuplot::set_xrange(const double iFrom,
                             const double iTo)
{
  std::ostringstream cmdstr;

  cmdstr << "set xrange[" << iFrom << ":" << iTo << "]";
  cmd(cmdstr.str());

  return *this;
}

//------------------------------------------------------------------------------
// set the yrange
//
Gnuplot& Gnuplot::set_yrange(const double iFrom,
                             const double iTo)
{
  std::ostringstream cmdstr;

  cmdstr << "set yrange[" << iFrom << ":" << iTo << "]";
  cmd(cmdstr.str());

  return *this;
}

//------------------------------------------------------------------------------
// set the zrange
//
Gnuplot& Gnuplot::set_zrange(const double iFrom,
                             const double iTo)
{
  std::ostringstream cmdstr;

  cmdstr << "set zrange[" << iFrom << ":" << iTo << "]";
  cmd(cmdstr.str());

  return *this;
}

//------------------------------------------------------------------------------
//
// set the palette range
//
Gnuplot& Gnuplot::set_cbrange(const double iFrom,
                              const double iTo)
{
  std::ostringstream cmdstr;

  cmdstr << "set cbrange[" << iFrom << ":" << iTo << "]";
  cmd(cmdstr.str());

  return *this;
}

//------------------------------------------------------------------------------
//
// Plots a linear equation y=ax+b (where you supply the
// slope a and intercept b)
//
Gnuplot& Gnuplot::plot_slope(const double a,
                             const double b,
                             const std::string &title)
{
  std::ostringstream cmdstr;
  //
  // command to be sent to gnuplot
  //
  if (_nplots_ > 0  &&  _two_dim_ == true)
    cmdstr << "replot ";
  else
    cmdstr << "plot ";

  cmdstr << a << " * x + " << b << " title \"";

  if (title == "")
    cmdstr << "f(x) = " << a << " * x + " << b;
  else
    cmdstr << title;

  cmdstr << "\" with " << _pstyle_;

  //
  // Do the actual plot
  //
  cmd(cmdstr.str());

  return *this;
}

//------------------------------------------------------------------------------
//
// Plot an equation supplied as a std::string y=f(x) (only f(x) expected)
//
Gnuplot& Gnuplot::plot_equation(const std::string &equation,
                                const std::string &title)
{
  std::ostringstream cmdstr;
  //
  // command to be sent to gnuplot
  //
  if (_nplots_ > 0  &&  _two_dim_ == true)
    cmdstr << "replot ";
  else
    cmdstr << "plot ";

  cmdstr << equation << " title \"";

  if (title == "")
    cmdstr << "f(x) = " << equation;
  else
    cmdstr << title;

  cmdstr << "\" with " << _pstyle_;

  //
  // Do the actual plot
  //
  cmd(cmdstr.str());

  return *this;
}

//------------------------------------------------------------------------------
//
// plot an equation supplied as a std::string y=(x)
//
Gnuplot& Gnuplot::plot_equation3d(const std::string &equation,
                                  const std::string &title)
{
  std::ostringstream cmdstr;
  //
  // command to be sent to gnuplot
  //
  if (_nplots_ > 0  &&  _two_dim_ == false)
    cmdstr << "replot ";
  else
    cmdstr << "splot ";

  cmdstr << equation << " title \"";

  if (title == "")
    cmdstr << "f(x,y) = " << equation;
  else
    cmdstr << title;

  cmdstr << "\" with " << _pstyle_;

  //
  // Do the actual plot
  //
  cmd(cmdstr.str());

  return *this;
}


//------------------------------------------------------------------------------
//
// Plots a 2d graph from a list of doubles (x) saved in a file
//
Gnuplot& Gnuplot::plotfile_x(const std::string &filename,
                             const unsigned int column,
                             const std::string &title)
{
  //
  // check if file exists
  //
  file_available(filename);


  std::ostringstream cmdstr;
  //
  // command to be sent to gnuplot
  //
  if (_nplots_ > 0  &&  _two_dim_ == true)
    cmdstr << "replot ";
  else
    cmdstr << "plot ";

  cmdstr << "\"" << filename << "\" using " << column;

  if (title == "")
    cmdstr << " notitle ";
  else
    cmdstr << " title \"" << title << "\" ";

  if(_smooth_ == "")
    cmdstr << "with " << _pstyle_;
  else
    cmdstr << "smooth " << _smooth_;

  //
  // Do the actual plot
  //
  cmd(cmdstr.str()); //_nplots_++; _two_dim_ = true;  already in cmd();

  return *this;
}


//------------------------------------------------------------------------------
//
// Plots a 2d graph from a list of doubles (x y) saved in a file
//
Gnuplot& Gnuplot::plotfile_xy(const std::string &filename,
                              const unsigned int column_x,
                              const unsigned int column_y,
                              const std::string &title)
{
  //
  // check if file exists
  //
  file_available(filename);


  std::ostringstream cmdstr;
  //
  // command to be sent to gnuplot
  //
  if (_nplots_ > 0  &&  _two_dim_ == true)
    cmdstr << "replot ";
  else
    cmdstr << "plot ";

  cmdstr << "\"" << filename << "\" using " << column_x << ":" << column_y;

  if (title == "")
    cmdstr << " notitle ";
  else
    cmdstr << " title \"" << title << "\" ";

  if(_smooth_ == "")
    cmdstr << "with " << _pstyle_;
  else
    cmdstr << "smooth " << _smooth_;

  //
  // Do the actual plot
  //
  cmd(cmdstr.str());

  return *this;
}


//------------------------------------------------------------------------------
//
// Plots a 2d graph from a list of doubles (x y) saved in a file
//
Gnuplot& Gnuplot::plotfile_xy(const std::string &filename,
                              const unsigned int column_x,
                              const unsigned int column_y,
                              const std::string &title,
                              const std::string & file_options_,
                              const std::string & plot_options_)
{
  //
  // check if file exists
  //
  file_available(filename);


  std::ostringstream cmdstr;
  //
  // command to be sent to gnuplot
  //
  if (_nplots_ > 0  &&  _two_dim_ == true)
    cmdstr << "replot ";
  else
    cmdstr << "plot ";

  cmdstr << "\"" << filename << "\" ";
  if (! file_options_.empty ())
    {
      cmdstr << file_options_;
    }
  cmdstr << " using " << column_x << ":" << column_y;

  if (title == "")
    cmdstr << " notitle ";
  else
    cmdstr << " title \"" << title << "\" ";

  if (! plot_options_.empty ())
    {
      if(_smooth_ == "")
        cmdstr << "with " << _pstyle_;
      else
        cmdstr << "smooth " << _smooth_;
    }
  else
    {
      cmdstr << plot_options_;
    }

  //
  // Do the actual plot
  //
  cmd(cmdstr.str());

  return *this;
}

//------------------------------------------------------------------------------
//
// Plots a 2d graph with errorbars from a list of doubles (x y dy) in a file
//
Gnuplot& Gnuplot::plotfile_xy_err(const std::string &filename,
                                  const unsigned int column_x,
                                  const unsigned int column_y,
                                  const unsigned int column_dy,
                                  const std::string &title)
{
  //
  // check if file exists
  //
  file_available(filename);

  std::ostringstream cmdstr;
  //
  // command to be sent to gnuplot
  //
  if (_nplots_ > 0  &&  _two_dim_ == true)
    cmdstr << "replot ";
  else
    cmdstr << "plot ";

  cmdstr << "\"" << filename << "\" using "
         << column_x << ":" << column_y << ":" << column_dy
         << " with errorbars ";

  if (title == "")
    cmdstr << " notitle ";
  else
    cmdstr << " title \"" << title << "\" ";

  //
  // Do the actual plot
  //
  cmd(cmdstr.str());

  return *this;
}


//------------------------------------------------------------------------------
//
// Plots a 3d graph from a list of doubles (x y z) saved in a file
//
Gnuplot& Gnuplot::plotfile_xyz_with_colored_wires (const std::string &filename,
                                                   const std::string &title,
                                                   int color_)
{
  //
  // check if file exists
  //
  file_available(filename);
  unsigned int column_x = 1;
  unsigned int column_y = 2;
  unsigned int column_z = 3;

  std::ostringstream cmdstr;
  //
  // command to be sent to gnuplot
  //
  if (_nplots_ > 0  &&  _two_dim_ == false)
    cmdstr << "replot ";
  else
    cmdstr << "splot ";

  cmdstr << "\"" << filename << "\" using " << column_x << ":" << column_y
         << ":" << column_z;

  std::ostringstream wired_colored_pstyle;
  wired_colored_pstyle << "lines lt " << color_;
  if (title == "")
    cmdstr << " notitle with " << wired_colored_pstyle.str();
  else
    cmdstr << " title \"" << title << "\" with " << wired_colored_pstyle.str();

  //
  // Do the actual plot
  //
  cmd(cmdstr.str());

  return *this;
}

//------------------------------------------------------------------------------
//
// Plots a 3d graph from a list of doubles (x y z) saved in a file
//
Gnuplot& Gnuplot::plotfile_xyz(const std::string &filename,
                               const unsigned int column_x,
                               const unsigned int column_y,
                               const unsigned int column_z,
                               const std::string &title)
{
  //
  // check if file exists
  //
  file_available(filename);

  std::ostringstream cmdstr;
  //
  // command to be sent to gnuplot
  //
  if (_nplots_ > 0  &&  _two_dim_ == false)
    cmdstr << "replot ";
  else
    cmdstr << "splot ";

  cmdstr << "\"" << filename << "\" using " << column_x << ":" << column_y
         << ":" << column_z;

  if (title == "")
    cmdstr << " notitle with " << _pstyle_;
  else
    cmdstr << " title \"" << title << "\" with " << _pstyle_;

  //
  // Do the actual plot
  //
  cmd(cmdstr.str());

  return *this;
}


//------------------------------------------------------------------------------
//
// Plots a 3d graph from a list of doubles (x y z o) saved in a file
//
Gnuplot& Gnuplot::plotfile_xyzo(const std::string &filename,
                                const unsigned int column_x,
                                const unsigned int column_y,
                                const unsigned int column_z,
                                const unsigned int column_o,
                                const std::string &title)
{
  //
  // check if file exists
  //
  file_available(filename);

  std::ostringstream cmdstr;
  //
  // command to be sent to gnuplot
  //
  if (_nplots_ > 0  &&  _two_dim_ == false)
    cmdstr << "replot ";
  else
    cmdstr << "splot ";

  cmdstr << "\"" << filename << "\" using " << column_x << ":" << column_y
         << ":" << column_z << ":" << column_o;

  if (title == "")
    cmdstr << " notitle ";
  else
    cmdstr << " title \"" << title << "\" ";
  cmdstr << " with pm3d ;";
  //
  // Do the actual plot
  //
  cmd(cmdstr.str());

  return *this;
}



//------------------------------------------------------------------------------
//
/// *  note that this function is not valid for versions of GNUPlot below 4.2
//
Gnuplot& Gnuplot::plot_image(const unsigned char * ucPicBuf,
                             const unsigned int iWidth,
                             const unsigned int iHeight,
                             const std::string &title)
{
  std::ofstream tmp;
  std::string name = create_tmpfile(tmp);
  if (name == "")
    return *this;

  //
  // write the data to file
  //
  int iIndex = 0;
  for(size_t iRow = 0; iRow < iHeight; iRow++)
    {
      for(size_t iColumn = 0; iColumn < iWidth; iColumn++)
        {
          tmp << iColumn << " " << iRow  << " "
              << static_cast<float>(ucPicBuf[iIndex++]) << std::endl;
        }
    }

  tmp.flush();
  tmp.close();


  std::ostringstream cmdstr;
  //
  // command to be sent to gnuplot
  //
  if (_nplots_ > 0  &&  _two_dim_ == true)
    cmdstr << "replot ";
  else
    cmdstr << "plot ";

  if (title == "")
    cmdstr << "\"" << name << "\" with image";
  else
    cmdstr << "\"" << name << "\" title \"" << title << "\" with image";

  //
  // Do the actual plot
  //
  cmd(cmdstr.str());

  return *this;
}



//------------------------------------------------------------------------------
//
// Sends a command to an active gnuplot session
//
Gnuplot& Gnuplot::cmd(const std::string &cmdstr)
{
  if( !(_valid_) )
    {
      return *this;
    }


  // int fputs ( const char * str, FILE * stream );
  // writes the string str to the stream.
  // The function begins copying from the address specified (str) until it
  // reaches the terminating null character ('\0'). This final
  // null-character is not copied to the stream.
  fputs( (cmdstr+"\n").c_str(), _gnucmd_ );

  // int fflush ( FILE * stream );
  // If the given stream was open for writing and the last i/o operation was
  // an output operation, any unwritten data in the output buffer is written
  // to the file.  If the argument is a null pointer, all open files are
  // flushed.  The stream remains open after this call.
  fflush(_gnucmd_);


  if( cmdstr.find("replot") != std::string::npos )
    {
      return *this;
    }
  else if( cmdstr.find("splot") != std::string::npos )
    {
      _two_dim_ = false;
      _nplots_++;
    }
  else if( cmdstr.find("plot") != std::string::npos )
    {
      _two_dim_ = true;
      _nplots_++;
    }

  return *this;
}



//------------------------------------------------------------------------------
//
// Opens up a gnuplot session, ready to receive commands
//
void Gnuplot::init()
{
  // char * getenv ( const char * name );  get value of environment variable
  // Retrieves a C string containing the value of the environment variable
  // whose name is specified as argument.  If the requested variable is not
  // part of the environment list, the function returns a NULL pointer.
#if ( defined(unix) || defined(__unix) || defined(__unix__) ) && !defined(__APPLE__)
  if (getenv("DISPLAY") == NULL)
    {
      _valid_ = false;
      DT_THROW_IF(true, GnuplotException,"Can't find DISPLAY variable");
    }
#endif


  // if gnuplot not available
  if (!Gnuplot::get_program_path())
    {
      _valid_ = false;
      DT_THROW_IF(true, GnuplotException, "Can't find gnuplot");
    }


  //
  // open pipe
  //
  std::string tmp = Gnuplot::_g_gnuplot_path + "/" +
    Gnuplot::_g_gnuplot_filename_;

  // FILE *popen(const char *command, const char *mode);
  // The popen() function shall execute the command specified by the string
  // command, create a pipe between the calling program and the executed
  // command, and return a pointer to a stream that can be used to either read
  // from or write to the pipe.
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
  _gnucmd_ = _popen(tmp.c_str(),"w");
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
  _gnucmd_ = popen(tmp.c_str(),"w");
#endif

  // popen() shall return a pointer to an open stream that can be used to read
  // or write to the pipe.  Otherwise, it shall return a null pointer and may
  // set errno to indicate the error.
  if (!_gnucmd_)
    {
      _valid_ = false;
      DT_THROW_IF(true, GnuplotException, "Couldn't open connection to gnuplot");
    }

  _nplots_ = 0;
  _valid_ = true;
  _smooth_ = "";

  //set terminal type
  showonscreen();

  return;
}


//------------------------------------------------------------------------------
//
// Find out if a command lives in _g_gnuplot_path or in PATH
//
bool Gnuplot::get_program_path()
{
  //
  // first look in _g_gnuplot_path for Gnuplot
  //
  std::string tmp = Gnuplot::_g_gnuplot_path + "/" +
    Gnuplot::_g_gnuplot_filename_;

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
  if ( Gnuplot::file_exists(tmp,0) ) // check existence
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
    if ( Gnuplot::file_exists(tmp,1) ) // check existence and execution permission
#endif
      {
        return true;
      }


  //
  // second look in PATH for Gnuplot
  //
  char *path;
  // Retrieves a C string containing the value of environment variable PATH
  path = getenv("PATH");


  if (path == NULL)
    {
      DT_THROW_IF(true, GnuplotException,"Path is not set");
      return false;
    }
  else
    {
      std::list<std::string> ls;

      //split path (one long string) into list ls of strings
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
      stringtok(ls,path,";");
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
      stringtok(ls,path,":");
#endif

      // scan list for Gnuplot program files
      for (std::list<std::string>::const_iterator i = ls.begin();
           i != ls.end(); ++i)
        {
          tmp = (*i) + "/" + Gnuplot::_g_gnuplot_filename_;
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
          if ( Gnuplot::file_exists(tmp,0) ) // check existence
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
            if ( Gnuplot::file_exists(tmp,1) ) // check existence and execution permission
#endif
              {
                Gnuplot::_g_gnuplot_path = *i; // set _g_gnuplot_path
                return true;
              }
        }

      tmp = "Can't find gnuplot neither in PATH nor in \"" +
        Gnuplot::_g_gnuplot_path + "\"";
      DT_THROW_IF(true, GnuplotException, tmp);

      Gnuplot::_g_gnuplot_path = "";
      return false;
    }
}



//------------------------------------------------------------------------------
//
// check if file exists
//
bool Gnuplot::file_exists(const std::string &filename, int mode)
{
  DT_THROW_IF ( mode < 0 || mode > 7,
                std::domain_error,
                "In function \"Gnuplot::file_exists\": mode\
                has to be an integer between 0 and 7");

  // int _access(const char *path, int mode);
  //  returns 0 if the file has the given mode,
  //  it returns -1 if the named file does not exist or is not accessible in
  //  the given mode
  // mode = 0 (F_OK) (default): checks file for existence only
  // mode = 1 (X_OK): execution permission
  // mode = 2 (W_OK): write permission
  // mode = 4 (R_OK): read permission
  // mode = 6       : read and write permission
  // mode = 7       : read, write and execution permission
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
  if (_access(filename.c_str(), mode) == 0)
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
    if (access(filename.c_str(), mode) == 0)
#endif
      {
        return true;
      }
    else
      {
        return false;
      }

}

bool Gnuplot::file_available(const std::string &filename){
  if( Gnuplot::file_exists(filename,0) ) // check existence
    {
      if( !(Gnuplot::file_exists(filename,4)) ){// check read permission
        DT_THROW_IF(true, GnuplotException,
                    "No read permission for File \"" << filename << "\" !");
        return false;
      }

      return true;
    }
  else{
    DT_THROW_IF(true, GnuplotException,
                "File \"" << filename << "\" does not exist !");
    return false;
  }
}



//------------------------------------------------------------------------------
//
// Opens a temporary file
//
std::string Gnuplot::create_tmpfile(std::ofstream &tmp)
{

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
  char name[] = "gnuplotiXXXXXX"; //tmp file in working directory
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
  char name[] = "/tmp/gnuplotiXXXXXX"; // tmp file in /tmp
#endif

  //
  // check if maximum number of temporary files reached
  //
  DT_THROW_IF (Gnuplot::_g_tmpfile_num_ == GP_MAX_TMP_FILES - 1,
               GnuplotException,
               "Maximum number of temporary files reached ("
               << GP_MAX_TMP_FILES << "): cannot open more files !");


  // int mkstemp(char *name);
  // shall replace the contents of the string pointed to by "name" by a unique
  // filename, and return a file descriptor for the file open for reading and
  // writing.  Otherwise, -1 shall be returned if no suitable file could be
  // created.  The string in template should look like a filename with six
  // trailing 'X' s; mkstemp() replaces each 'X' with a character from the
  // portable filename character set.  The characters are chosen such that the
  // resulting name does not duplicate the name of an existing file at the
  // time of a call to mkstemp()


  //
  // open temporary files for output
  //
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
  if (_mktemp(name) == NULL)
#elif defined(unix) || defined(__unix) || defined(__unix__) || defined(__APPLE__)
    if (mkstemp(name) == -1)
#endif
      {
        DT_THROW_IF(true,GnuplotException,
                    "Cannot create temporary file \"" << name << "\" !");
      }

  tmp.open(name);
  DT_THROW_IF (tmp.bad(),
               GnuplotException,
               "Cannot create temporary file \"" << name << "\" !");
  //
  // Save the temporary filename
  //
  _tmpfile_list_.push_back(name);
  Gnuplot::_g_tmpfile_num_++;

  return name;
}

void Gnuplot::remove_tmpfiles()
{
  if ((_tmpfile_list_).size() > 0)
    {
      for (unsigned int i = 0; i < _tmpfile_list_.size(); i++)
        {
          remove( _tmpfile_list_[i].c_str() );
        }

      Gnuplot::_g_tmpfile_num_ -= _tmpfile_list_.size();
    }
  return;
}

// end of gnuplot_i.cc
