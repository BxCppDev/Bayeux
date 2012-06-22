// -*- mode: c++; -*-
// ioutils.h
#ifndef DATATOOLS_UTILS_IOUTILS_H_
#define DATATOOLS_UTILS_IOUTILS_H_

// Standard Library
#include <iostream>

// Third Party
// - Boost
#include <boost/cstdint.hpp>

// This Project


namespace datatools {
namespace utils {
//----------------------------------------------------------------------
// ostream_manipulator class
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
// ostream_manipulator_ref class
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
// io struct
struct io {
 public:
  /// io::indenter class
  class indenter {
   public:
    // ctor:
    indenter();

    size_t get_width() const;
    size_t get_level() const;

    indenter& operator++(int);
    indenter& operator--(int);

    std::ostream& operator()(std::ostream&) const;
    indenter& operator()(size_t);
    friend std::ostream& operator<<(std::ostream &, const indenter &);

   private:
    size_t width_;
    size_t level_;
  };

  // io struct
 public:
  static indenter indent;

 public:
  static bool is_colored();

  static void set_colored(bool);
  
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


 private:
  // ctor:
  io() {}

  // dtor:
  ~io() {
    this->set_colored(false);
  }

 private:
  static bool g_colored_stream_;
  static io g_io_;
};

} // namespace utils
} // namespace datatools

#endif // DATATOOLS_UTILS_IOUTILS_H_

