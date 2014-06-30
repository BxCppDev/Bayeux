// -*- mode: c++; -*-
/// \file geomtools/color.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2010-03-23
 * Last modified: 2014-02-12
 *
 * License:
 *
 * Description:
 *
 *  Utilities for color manipulation.
 *
 * History:
 *
 */

#ifndef GEOMTOOLS_COLOR_H
#define GEOMTOOLS_COLOR_H 1

// Standard library:
#include <iostream>
#include <string>
#include <map>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
#include <boost/scoped_ptr.hpp>

namespace geomtools {

  /// \brief Color
  class color
  {
  public:

    static const int COLOR_TEST = 0;

    /// \brief Color code for gnuplot (x11 terminal)
    // May collide with some macros from ncurses.h
    enum code_type {
      COLOR_INVALID     = -4,
      COLOR_TRANSPARENT = -3,
      COLOR_WHITE       = -2,
      COLOR_BLACK       = -1,
      COLOR_GREY        =  0,
      COLOR_RED         =  1,
      COLOR_GREEN       =  2,
      COLOR_BLUE        =  3,
      COLOR_MAGENTA     =  4,
      COLOR_CYAN        =  5,
      COLOR_YELLOW      =  6,
      COLOR_ORANGE      =  8,
      COLOR_DEFAULT     =  COLOR_GREY
    };

    static const std::string & invalid();
    static const std::string & transparent();
    static const std::string & white();
    static const std::string & black();
    static const std::string & grey();
    static const std::string & red();
    static const std::string & green();
    static const std::string & blue();
    static const std::string & magenta();
    static const std::string & cyan();
    static const std::string & yellow();
    static const std::string & orange();
    static const std::string & default_color();

    /// \brief Color labels
    struct constants
    {
      std::string invalid;
      std::string transparent;
      std::string white;
      std::string black;
      std::string grey;
      std::string red;
      std::string green;
      std::string blue;
      std::string magenta;
      std::string cyan;
      std::string yellow;
      std::string orange;
      std::string default_color;

      /// Constructor
      constants();

      /// Singleton
      static const constants & instance();
    };

  public:

    int32_t     code;         /// Color code
    std::string name;         /// Color name
    int32_t     red_amount;   /// Red channel (RGB encoding)
    int32_t     green_amount; /// Green channel (RGB encoding)
    int32_t     blue_amount;  /// Blue channel (RGB encoding)
    int32_t     alpha_amount; /// Alpha channel

  public:

    /// Constructor
    color();

    /// Constructor
    color(code_type, const std::string &,
          int32_t, int32_t, int32_t, int32_t = 0);

    //// General color map type
    typedef std::map<std::string, code_type> color_map_type;

    /// \brief Color register
    class color_db
    {
    public:

      /// Constructor
      color_db();

      /// Destructor
      virtual ~color_db();

      //bool has_color(const string & id_) const;

      /// Get color code from label
      code_type get_color(const std::string & a_color_id) const;

      //void register_color(color, const string & id_);

      //void dump_colors(ostream & out_ = clog);

    private:

      color_map_type map_of_colors_; /// Dictionary of colors

    };

    /// Type of smart pointer on color register
    typedef boost::scoped_ptr<color_db> scoped_color_db_type;

  public:

    /// Get the global color register
    static const color_db & get_color_db();

    /// Get color code from label
    static code_type get_color(const std::string & a_name);

    /// Color context object
    struct context {

      enum encoding_type {
        ENCODING_NONE     = 0,
        ENCODING_BY_CODE  = 1,
        ENCODING_BY_VALUE = 2,
        ENCODING_BY_NAME  = 3,
        //ENCODING_BY_COLOR = 4,
      };

      /// Constructor
      context();

      /// Reset
      void reset();

      /// Check activation status
      bool is_activated() const;

      /// Deactivate
      void deactivate();

      /// Check if color is encoded by code
      bool encoded_by_code() const;

      /// Set the color code
      void set_color_code(color::code_type);

      /// Return the color code
      color::code_type get_color_code() const;

      /// Check if color is encoded by value
      bool encoded_by_value() const;

      /// Set the color value
      void set_color_value(double);

      /// Return the color value
      double get_color_value() const;

      /// Check if color is encoded by name
      bool encoded_by_name() const;

      /// Set the color name
      void set_color_name(const std::string &);

      /// Return the color name
      const std::string & get_color_name() const;

      /*
      /// Check if color is encoded by color
      bool encoded_by_color() const;

      /// Set the color object
      void set_color(const color &);

      /// Return the color object
      const color & get_color() const;
      */

      /// Generate a string
      std::string str() const;

    private:

      /// Attributes:
      encoding_type    _encoding_;     /// Encoding
      color::code_type _color_code_;   /// Integer code
      double           _color_value_;  /// Color map value
      std::string      _color_name_;   /// Color name
      double           _transparency_; /// Transparency %

    };

  };

} // end of namespace geomtools

#endif // GEOMTOOLS_COLOR_H
