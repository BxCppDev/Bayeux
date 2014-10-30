// -*- mode: c++; -*-
/// \file datatools/ioutils.h
#ifndef DATATOOLS_IOUTILS_H
#define DATATOOLS_IOUTILS_H

// Standard Library:
#include <iostream>
#include <string>

// Third Party:
// - Boost:
#include <boost/cstdint.hpp>

namespace datatools {

  void print_multi_lines(std::ostream & out_,
                         const std::string & text_,
                         const std::string & indent_ = "");

//----------------------------------------------------------------------
/*! \brief ostream_manipulator class
 */
template <class T>
class ostream_manipulator {
 public:
  ostream_manipulator(std::ostream& (*function)(std::ostream&, const T&),
                      const T& value)
      : function_(function), value_(value) {}

  friend std::ostream& operator<<(std::ostream& os,
                                  const ostream_manipulator& manip) {
    return manip.function_(os, manip.value_);
  }


 private:
  std::ostream& (*function_)(std::ostream&, const T&);
  T value_;
};


//----------------------------------------------------------------------
/*! \brief ostream_manipulator_ref class
 */
template <class T>
class ostream_manipulator_ref {
 public:
  ostream_manipulator_ref(std::ostream& (*function)(std::ostream&, T&),
                          T& value)
      : function_(function), value_(value) {}


  friend std::ostream& operator<<(std::ostream& os,
                                  const ostream_manipulator_ref& manip) {
    return manip.function_(os, manip.value_);
  }

 private:
  std::ostream& (*function_)(std::ostream&, T&);
  T& value_;
};


//----------------------------------------------------------------------
/*! \brief I/O utilities
 */
struct io {
 public:

  struct constants {

    /// A portable representation for a NaN
    static const std::string & nan_real_repr();
    /// A portable representation for +infinity
    static const std::string & plus_infinity_real_repr();
    /// A portable representation for -infinity
    static const std::string & minus_infinity_real_repr();
  };

  static const int REAL_PRECISION  = 16; /// Default precision for double
  static const int REAL8_PRECISION = 16; /// Default precision for double
  static const int REAL4_PRECISION = 8;  /// Default precision for float

  /*! \brief I/O indenter class
   */
  class indenter {

   public:

    /// Default constructor
    indenter();

    /// Returns the with of each indentation level
    size_t get_width() const;

    /// Returns the current level of indentation
    size_t get_level() const;

    /// Increment indendation level by one unit
    indenter& operator++(int);

    /// Decrement indendation level by one unit
    indenter& operator--(int);

    std::ostream& operator()(std::ostream&) const;

    indenter& operator()(size_t);

    friend std::ostream& operator<<(std::ostream &, const indenter &);

   private:

    size_t width_; /// Width of each indentation level
    size_t level_; /// Current indentation level

  };

  /// Parse a boolean
  static bool read_boolean(std::istream&, bool&);

  /// Write a boolean
  static void write_boolean(std::ostream& a_out, bool a_bool);

  /// Parse an integer
  static bool read_integer(std::istream&, int&);

  /// Write an integer
  static void write_integer(std::ostream& a_out, int a_integer);

  /// Parse a quoted string
  static bool read_quoted_string(std::istream&, std::string&);

  /// Write a quoted string
  static void write_quoted_string(std::ostream& a_out, const std::string& a_str);

  /// Read a double value from an ASCII stream
  static bool read_real_number(std::istream & in_, double & val_, bool & normal_);

  /// Write a double value in an ASCII stream with unit support
  static void write_real_number(std::ostream & out_,
                                double val_,
                                int precision_ = REAL_PRECISION,
                                const std::string& unit_symbol_ = "",
                                const std::string& unit_label_ = "",
                                double unit_value_ = 1.0);

  bool is_colored() const;

  void set_colored(bool);

  static std::ostream& normal(std::ostream&);

  static std::ostream& reverse(std::ostream&);

  static std::ostream& bold(std::ostream&);

  static std::ostream& underline(std::ostream&);

  static std::ostream& red(std::ostream&);

  static std::ostream& green(std::ostream&);

  static std::ostream& debug(std::ostream&);

  static std::ostream& devel(std::ostream&);

  static std::ostream& notice(std::ostream&);

  static std::ostream& warning(std::ostream&);

  static std::ostream& error(std::ostream&);

  static std::ostream& verbose(std::ostream&);

  static std::ostream& tab(std::ostream&);

  static std::ostream& vtab(std::ostream&);

  static std::ostream& page_break(std::ostream&);

  static std::ostream& left(std::ostream&);

  static std::ostream& right(std::ostream&);

  static std::ostream& internal(std::ostream&);

  static std::ostream& showbase(std::ostream&);

  static std::ostream& ostream_width(std::ostream& os, const int& n);

  static ostream_manipulator<int> width(const int& n);

  static std::ostream& ostream_precision(std::ostream& os, const int& n);

  static ostream_manipulator<int> precision(const int & n);

  static std::string to_binary(const uint32_t& val, bool show_all = false);

  // Return singleton indenter
  static indenter & indent();

  // Return singleton io
  static io & instance();

protected:

  /// \brief Default constructor
  io();

public:

  /// \brief Destructor
  // boost::check_delete<> needs it to be public
  ~io();

private:

  bool _colored_stream_; /// Color flag

};

} // namespace datatools

#endif // DATATOOLS_IOUTILS_H
