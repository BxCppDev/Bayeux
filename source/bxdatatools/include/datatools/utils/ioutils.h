// -*- mode: c++; -*-
// ioutils.h

#ifndef __datatools__utils__ioutils_h
#define __datatools__utils__ioutils_h 1

#include <iostream>

#include <boost/cstdint.hpp>

// #ifdef USING_NCURSES
// #include <curses.h>
// #endif // USING_NCURSES

//using   namespace std;

namespace datatools {

  namespace utils {

    template <class Type>
    class ostream_manipulator
    {
    private:
      std::ostream & (*_function_) (std::ostream &, const Type &);
      Type        _value_;

    public:
      ostream_manipulator (std::ostream & (*function_) (std::ostream & , const Type &),
                           const Type & value_)
        : _function_ (function_), _value_ (value_)
      {
      }

      friend std::ostream & operator<<(std::ostream & os_,
                                       const ostream_manipulator & os_manip_)
      {
        return os_manip_._function_ (os_, os_manip_._value_);
      }

    };

    template <class Type>
    class ostream_manipulator_ref
    {
    private:
      std::ostream & (*_function_) (std::ostream & , Type &);
      Type    &   _value_;

    public:
      ostream_manipulator_ref (std::ostream & (*function_) (std::ostream &, Type &) ,
                               Type & value_)
        : _function_ (function_) , _value_ (value_)
      {
      }

      friend std::ostream & operator<< (std::ostream & os_,
                                        const ostream_manipulator_ref & os_manip_)
      {
        return os_manip_._function_ (os_, os_manip_._value_);
      }
    };

    struct io
    {
    private:
      static bool g_colored_stream_;
// #ifdef USING_NCURSES
//       static SCREEN * g_screen_;
// #endif // USING_NCURSES
      static io g_io_;

    public:

      class indenter
      {
        size_t _width_;
        size_t _level_;

      public:

        size_t get_width () const;
        size_t get_level () const;
        // ctor:
        indenter ();

        indenter & operator++ (int);
        indenter & operator-- (int);
        std::ostream & operator() (std::ostream &) const;
        indenter & operator () (size_t);

        friend std::ostream & operator<< (std::ostream &,
                                          const indenter &);
      };

      static indenter indent;

      static bool is_colored ();

      static void set_colored (bool);

// #ifdef USING_NCURSES
//       static void focus ();
// #endif // USING_NCURSES

    private:
      // ctor:
      io ()
      {
      }

      // dtor:
      ~io ()
      {
        set_colored (false);
      }

    public:

      static std::ostream & normal (std::ostream &);

      static std::ostream & reverse (std::ostream &);

      static std::ostream & bold (std::ostream &);

      static std::ostream & underline (std::ostream &);

      static std::ostream & red (std::ostream &);

      static std::ostream & green (std::ostream &);

      static std::ostream & debug (std::ostream &);

      static std::ostream & devel (std::ostream &);

      static std::ostream & notice (std::ostream &);

      static std::ostream & warning (std::ostream &);

      static std::ostream & error (std::ostream &);

      static std::ostream & verbose (std::ostream &);

      static std::ostream & tab (std::ostream &);

      static std::ostream & vtab (std::ostream &);

      static std::ostream & page_break (std::ostream &);

      static std::ostream & left (std::ostream &);

      static std::ostream & right (std::ostream &);

      static std::ostream & internal (std::ostream &);

      static std::ostream & showbase (std::ostream &);

      static std::ostream & ostream_width (std::ostream & os_, const int & n_);

      static ostream_manipulator<int> width ( const int & n_);

      static std::ostream & ostream_precision (std::ostream & os_, const int & n_);

      static ostream_manipulator<int> precision (const int & n_);

      static std::string to_binary (const uint32_t & val_, bool show_all_ = false);

    };

  } // namespace utils

} // namespace datatools

#endif // __datatools__utils__ioutils_h

// end of ioutils.h
