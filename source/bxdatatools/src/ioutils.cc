// -*- mode: c++; -*-
// ioutils.cc

#include <datatools/utils/ioutils.h>

#include <sstream>

using namespace std;

namespace datatools {

  namespace utils {

    bool io::g_colored_stream_ = false;
    io   io::g_io_;

#ifdef USING_NCURSES
    SCREEN * io::g_screen_ = 0;
#endif // USING_NCURSES

    io::indenter io::indent;
 
    size_t io::indenter::get_width () const
    {
      return _width_;
    }

    size_t io::indenter::get_level () const
    {
      return _level_;
    }

    // ctor:
    io::indenter::indenter ()
    {
      _width_ = 4;
      _level_ = 0;
    }

    io::indenter & io::indenter::operator () (size_t l_)
    {
      this->_level_ = l_;
      return *this;
    }

    ostream & io::indenter::operator () (ostream & out_) const
    {
      out_ << *this;
      return out_;
    }

    io::indenter & io::indenter::operator++ (int)
    {
      _level_++;
      return *this;
    }

    io::indenter & io::indenter::operator-- (int)
    {
      if (_level_ > 0) _level_--;
      return *this;
    }

    ostream & operator<< (ostream & out_, const io::indenter & indent_)
    {
      for (int i = 0; i < (int) (indent_._width_ * indent_._level_); i++)
	{
	  out_ << ' ';
	}
      return out_;
    }

    ostream & io::ostream_width (ostream & os_, const int & n_)
    {
      os_.width ((int) n_);
      return os_;
    }

    ostream_manipulator<int> io::width (const int & n_)
    {
      return ostream_manipulator<int> (&io::ostream_width, n_);
    }

    ostream & io::ostream_precision (ostream & os_, const int & n_)
    {
      os_.precision ((int) n_);
      return os_;
    }

    ostream_manipulator<int> io::precision( const int & n_ )
    {
      return ostream_manipulator<int> (&io::ostream_precision, n_);
    }

    bool io::is_colored ()
    {
      return g_colored_stream_;
    }

#ifdef USING_NCURSES
    void io::focus ()
    {
      if (g_screen_) set_term (g_screen_);
    }
#endif // USING_NCURSES

    void io::set_colored (bool c_)
    {
      if (io::g_colored_stream_ && c_) return;
      if (! io::g_colored_stream_ && ! c_) return;
      if (c_)
	{
#ifdef USING_NCURSES
	  clog << "io::set_colored: Open curses stuff." << endl;
	  savetty ();
	  string term  = "vt100;";
	  char * term_env = getenv ("TERM");
	  if (term_env != 0)
	    {
	      term = term_env;
	    }
	  g_screen_ = newterm (term.c_str (), stdout, stdin);
	  set_term (g_screen_);
	  initscr ();
	  nocbreak ();
	  echo ();
	  nonl ();
	  intrflush (stdscr, FALSE);
#endif // USING_NCURSES
	  io::g_colored_stream_ = true;
	}
      else
	{
#ifdef USING_NCURSES
	  if (g_screen_)
	    {
	      set_term (g_screen_);
	      endwin ();
	      delscreen (g_screen_);
	      g_screen_ = 0;
	      resetty ();
	      clog << "io::set_colored: Close curses stuff." << endl;
	    }
#endif // USING_NCURSES
	  io::g_colored_stream_ = false;
	}
      return;
    }

    ostream & io::normal (ostream & out_)
    {
      if (io::is_colored ())
	{
#ifdef USING_NCURSES
	  io::focus ();
	  attrset (A_NORMAL);
#else
#endif // USING_NCURSES
	}
      return out_;
    }

    ostream & io::reverse (ostream & out_)
    {
      if (io::is_colored ())
	{
#ifdef USING_NCURSES
	  io::focus ();
	  attrset (A_REVERSE);
#else
#endif // USING_NCURSES
	}
      return out_;
    }

    ostream & io::bold (ostream & out_)
    {
      if (io::is_colored ())
	{
#ifdef USING_NCURSES
	  io::focus ();
	  attrset (A_BOLD);
#endif // USING_NCURSES
	}
      return out_;
    }

    ostream & io::underline (ostream & out_)
    {
      if (io::is_colored ())
	{
#ifdef USING_NCURSES
	  io::focus ();
	  attrset (A_UNDERLINE);
#endif // USING_NCURSES
	}
      return out_;
    }

    ostream & io::red (ostream & out_)
    {
      if (io::is_colored ())
	{
#ifdef USING_NCURSES
	  io::focus ();
#endif // USING_NCURSES
	}
      return out_;
    }

    ostream & io::green (ostream & out_)
    {
      if (io::is_colored ())
	{
#ifdef USING_NCURSES
	  io::focus ();
#endif // USING_NCURSES
	}
      return out_;
    }

    ostream & io::debug (ostream & out_)
    {
      out_ << "DEBUG: ";
      return out_;
    }

    ostream & io::devel (ostream & out_)
    {
      out_ << "DEVEL: ";
      return out_;
    }

    ostream & io::notice (ostream & out_)
    {
      out_ << "NOTICE: ";
      return out_;
    }

    ostream & io::warning (ostream & out_)
    {
      out_ << "WARNING: ";
      return out_;
    }

    ostream & io::error (ostream & out_)
    {
      out_ << "ERROR: ";
      return out_;
    }

    ostream & io::verbose (ostream & out_)
    {
      out_ << "VERBOSE: ";
      return out_;
    }

    ostream & io::tab (ostream & out_)
    {
      out_ << '\t';
      return out_;
    }

    ostream & io::vtab (ostream & out_)
    {
      out_ << '\v';
      return out_;
    }

    ostream & io::page_break (ostream & out_)
    {
      out_ << '\f';
      return out_;
    }

    ostream & io::left (ostream & out_)
    {
      out_.setf (ios::left, ios::adjustfield);
      return out_;
    }

    ostream & io::right (ostream & out_)
    {
      out_.setf (ios::right, ios::adjustfield);
      return out_;
    }

    ostream & io::internal (ostream & out_)
    {
      out_.setf (ios::internal, ios::adjustfield);
      return out_;
    }

    ostream & io::showbase (ostream & out_)
    {
      out_.setf (ios::showbase);
      return out_;
    }

    string io::to_binary (const uint32_t & val_)
    {
      ostringstream oss;
      size_t nbits = sizeof (val_) * 8;
      //clog << "DEVEL: io::to_binary: value=" << val_ << endl;
      //clog << "DEVEL: io::to_binary: nbits=" << nbits << endl;
      oss << '(';
      bool start = false;
      for (int i = (nbits - 1); i >= 0; i--)
	{
	  bool abit;
	  abit = (val_ >> i) & 0x1;
	  //clog << "DEVEL: io::to_binary: bit[" << i << "]=" << abit << endl;
	  if (! start & abit) start = true;
	  if (! abit && ! start) continue;
	  oss << abit;
	}
      oss << ')';
      return oss.str ();
    }

  } // namespace utils

} // namespace datatools

// end of ioutils.cc
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
