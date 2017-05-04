// ioutils.cc

// Ourselves:
#include <datatools/ioutils.h>

// Standard Library:
#include <sstream>
#include <vector>

// Third Party:
// - Boost:
#include <boost/scoped_ptr.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/lexical_cast.hpp>

// This Project:
#include <datatools/utils.h>
#include <datatools/units.h>
#include <datatools/exception.h>
#include <datatools/logger.h>

namespace datatools {

  // static
  const int io::REAL_PRECISION;
  const int io::REAL8_PRECISION;
  const int io::REAL4_PRECISION;

  void print_multi_lines(std::ostream & out_,
                         const std::string & text_,
                         const std::string & indent_)
  {
    if (text_.empty()) return;
    std::vector<std::string> lines;
    boost::split(lines,text_, boost::is_any_of("\n"));
    for (size_t il = 0; il < lines.size(); il++) {
      out_ << indent_ << lines[il];
      //if (il != lines.size() - 1)
      out_ << '\n';
    }
    return;
  }

  // static
  io & io::instance()
  {
    static boost::scoped_ptr<io> _io;
    if (!_io) {
      _io.reset(new io);
    }
    return *_io;
  }

  // static
  io::indenter & io::indent()
  {
    static boost::scoped_ptr<indenter> _indenter;
    if (!_indenter) {
      _indenter.reset(new indenter);
    }
    return *_indenter;
  }

  void io::convert_command_line_args(int argc_,
                                     char ** argv_,
                                     std::string & app_name_,
                                     std::vector<std::string> & args_)
  {
    DT_THROW_IF(argc_ < 1, std::logic_error, "Negative number of arguments!");
    DT_THROW_IF(! argv_, std::logic_error, "Missing array of arguments!");
    app_name_ = argv_[0];
    for (int i = 1; i < argc_; i++) {
      args_.push_back(argv_[i]);
    }
    return;
  }

  const std::string & io::constants::nan_real_repr()
  {
    static std::string value("nan");
    return value;
  }

  const std::string & io::constants::plus_infinity_real_repr()
  {
    static std::string value("inf");
    return value;
  }

  const std::string & io::constants::minus_infinity_real_repr()
  {
    static std::string value("-inf");
    return value;
  }

  void io::write_boolean(std::ostream& a_out,
                         bool a_bool, bool as_alpha)
  {
    if (a_bool) {
      if (as_alpha) {
        a_out << "true";
      } else {
        a_out << "1";
      }
    } else {
      if (as_alpha) {
        a_out << "false";
      } else {
        a_out << "0";
      }
    }
    return;
  }

  bool io::read_boolean(std::istream& a_in,
                        bool& a_bool,
                        uint32_t flags)
  {
    bool ok = false;
    bool value = false;
    datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
    bool allow_trailing_characters = false;
    if (flags & reader_allow_trailing_characters) {
      allow_trailing_characters = true;
    }
    if (flags & reader_debug) {
      logging = datatools::logger::PRIO_DEBUG;
    }
    std::string repr;
    a_in >> repr;
    if (!a_in) {
      return false;
    }
    DT_LOG_DEBUG(logging, "boolean representation = '" << repr << "'");
    if (! repr.empty()) {
      // Parsing:
      boost::to_upper(repr);
      if (repr == "0"
          || repr == "FALSE" || repr == "F"
          || repr == "NO" || repr == "N") {
        value = false;
        ok = true;
      } else if (repr == "1"
                 || repr == "TRUE" || repr == "T"
                 || repr == "YES" || repr == "Y") {
        value = true;
        ok = true;
      }
      a_in >> std::ws;
      if (ok && !a_in.eof()) {
        char trailing = 0;
        trailing = a_in.peek();
        if (trailing != 0) {
          DT_LOG_DEBUG(logging, "Found trailing character = '" << trailing << "' !");
          if (! allow_trailing_characters) {
            DT_LOG_DEBUG(logging, "Ooops! Trailing character = '" << trailing << "' is not allowed.");
            ok = false;
            // }
          } else {
            DT_LOG_DEBUG(logging, "Trailing characters are allowed.");
          }
        }
      }
      // if (ok && ! allow_trailing_characters) {
      //   DT_LOG_DEBUG(logging, "Trailing characters are not allowed.");
      //   // Check invalid trailing characters:
      //   repr.clear();
      //   a_in >> repr;
      //   if (!repr.empty()) {
      //     DT_LOG_DEBUG(logging, "Ooops! Found trailing characters '" << repr << "'!");
      //     ok = false;
      //   }
      // }
    } // end parsing
    if (ok) {
      a_bool = value;
    }
    return ok;
  }

  bool io::read_boolean(const std::string & a_word,
                        bool& a_bool,
                        uint32_t flags)
  {
    std::istringstream in(a_word);
    return read_boolean(in, a_bool, flags);
  }

  bool io::read_integer(std::istream& a_in,
                        int& a_integer,
                        uint32_t flags)
  {
    bool ok = false;
    int value = 0;
    datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
    bool allow_trailing_characters = false;
    if (flags & reader_allow_trailing_characters) {
      allow_trailing_characters = true;
    }
    if (flags & reader_debug) {
      logging = datatools::logger::PRIO_DEBUG;
    }
    std::string repr;
    a_in >> repr;
    if (!a_in) {
      return false;
    }
    DT_LOG_DEBUG(logging, "Integer representation = '" << repr << "'");
    if (! repr.empty()) {
      // Parsing:
      try {
        std::istringstream iss(repr);
        std::string int_token;
        iss >> int_token;
        DT_LOG_DEBUG(logging, "Integer token = " << int_token);
        value = boost::lexical_cast<int>(int_token);
        ok = true;
        DT_LOG_DEBUG(logging, "Integer value = " << value);
      } catch (...) {
        DT_LOG_DEBUG(logging, "Cast failed!");
        ok = false;
      }
      a_in >> std::ws;
      if (ok && !a_in.eof()) {
        char trailing = 0;
        trailing = a_in.peek();
        if (trailing != 0) {
          DT_LOG_DEBUG(logging, "Found trailing character = '" << trailing << "' !");
          if (! allow_trailing_characters) {
            DT_LOG_DEBUG(logging, "Ooops! Trailing character = '" << trailing << "' is not allowed.");
            ok = false;
            // }
          } else {
            DT_LOG_DEBUG(logging, "Trailing characters are allowed.");
          }
        }
      }
      // if (ok) {
      //   std::string trailing;
      //   a_in >> trailing;
      //   if (! allow_trailing_characters) {
      //     DT_LOG_DEBUG(logging, "Trailing characters are not allowed.");
      //     // Check invalid trailing characters:
      //     if (!trailing.empty()) {
      //       DT_LOG_DEBUG(logging, "Ooops! Found trailing characters!");
      //       ok = false;
      //     }
      //   }
      // }
    } // end parsing
    if (ok) {
      a_integer = value;
    }
    return ok;
  }

  bool io::read_integer(const std::string & a_word,
                        int& a_int,
                        uint32_t flags)
  {
    std::istringstream in(a_word);
    return read_integer(in, a_int, flags);
  }

  void io::write_integer(std::ostream& a_out, int a_integer)
  {
    a_out << std::dec << a_integer;
    return;
  }

  void io::write_quoted_string(std::ostream& a_out, const std::string& a_str)
  {
    a_out << '"' << a_str << '"';
    return;
  }

  bool io::read_quoted_string(const std::string & a_word,
                              std::string& a_str,
                              uint32_t flags)
  {
    datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
    if (flags & reader_debug) {
      logging = datatools::logger::PRIO_DEBUG;
    }
    logging = datatools::logger::PRIO_DEBUG;
    DT_LOG_DEBUG(logging, "a_word=[" << a_word << "]");
    std::istringstream in(a_word);
    return read_quoted_string(in, a_str, flags);
  }

  bool io::read_quoted_string(std::istream& a_in,
                              std::string& a_str,
                              uint32_t flags)
  {
    bool ok = false;
    std::string chain = "";
    datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
    if (flags & reader_debug) {
      logging = datatools::logger::PRIO_DEBUG;
    }
    // logging = datatools::logger::PRIO_DEBUG;
    bool allow_trailing_characters = false;
    if (flags & reader_allow_trailing_characters) {
      allow_trailing_characters = true;
    }
    bool strict_quoting = false;
    if (flags & reader_strict_quoting) {
      strict_quoting = true;
    }
    a_in >> std::ws;
    if (!a_in) {
      a_str = chain;
      return true;
    }
    bool quoted = false;
    char quoting_char = 0;
    char lastc = 0;
    do {
      char c = 0;
      a_in.get(c);
      if (! a_in) break;
      if (c == '"') {
        if (quoted) {
          quoted=false;
          break;
        } else {
          if (lastc == 0) {
            quoted = true;
            quoting_char = c;
            continue;
          } else {
            chain += c;
            lastc  = c;
          }
        }
      } else {
       if (isspace(c)) {
          if (!quoted) {
            a_in.putback(c);
            break;
          }
        }
        chain += c;
        lastc = c;
      }
    } while(a_in);

    if (quoted) {
      DT_LOG_DEBUG(logging, "Ooops! Unterminated quoted string!");
      ok = false;
    } else {
      ok = true;
    }
    if (quoting_char == 0 && strict_quoting) {
      DT_LOG_DEBUG(logging, "Ooops! Found an unquoted character string!");
      ok = false;
    }
    a_in >> std::ws;
    if (ok && !a_in.eof()) {
      char trailing = 0;
      trailing = a_in.peek();
      if (trailing != 0) {
        DT_LOG_DEBUG(logging, "Found trailing character = '" << trailing << "' !");
        if (! allow_trailing_characters) {
          DT_LOG_DEBUG(logging, "Ooops! Trailing character = '" << trailing << "' is not allowed.");
          ok = false;
          // }
        } else {
          DT_LOG_DEBUG(logging, "Trailing characters are allowed.");
        }
      }
    }
    if (ok) {
      a_str = chain;
    }
    return ok;
  }

  // static
  void io::write_real_number(std::ostream & out_,
                             double val_,
                             int precision_,
                             const std::string& unit_symbol_,
                             const std::string& unit_label_,
                             double unit_value_,
                             uint32_t flags_)
  {
    datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
    char unit_sep = ' ';
    bool quote = false;
    if (flags_ & writer_debug) {
      logging = datatools::logger::PRIO_DEBUG;
    }
    if (flags_ & writer_strict_quoting) {
      quote = true;
    }
    if (flags_ & writer_underscore_sep) {
      unit_sep = '_';
    }
    if (flags_ & writer_tilda_sep) {
      unit_sep = '~';
    }
    DT_LOG_DEBUG(logging, "unit_sep     = \"" << unit_sep << "\"");
    DT_LOG_DEBUG(logging, "unit_symbol_ = '" << unit_symbol_ << "'");
    DT_LOG_DEBUG(logging, "unit_label_  = '" << unit_label_ << "'");
    if (datatools::is_normal(val_)) {
      double      the_unit_value = 1.0;
      std::string the_unit_symbol;
      std::string the_unit_label;
      if (!unit_symbol_.empty()) {
        bool ok = units::find_unit(unit_symbol_, the_unit_value, the_unit_label);
        if (ok) {
          // Unit is recognized :
          DT_THROW_IF(!unit_label_.empty() && the_unit_label != unit_label_,
                      std::logic_error,
                      "Unit '" << unit_symbol_
                      << "' is not compatible with unit label '" << unit_label_ << "'!");
          the_unit_symbol = unit_symbol_;
        } else {
          // Unit is not recognized :
          if (!datatools::is_valid(unit_value_)) {
            the_unit_symbol = unit_symbol_;
            the_unit_value = 1.0;
          } else if (unit_value_ > 0.0) {
            // But at least we provide a unit value (for unregistered units)
            the_unit_symbol = unit_symbol_;
            the_unit_value = unit_value_;
          } else {
            DT_THROW_IF(true, std::logic_error,
                        "Unit symbol '" << unit_symbol_ << "' is not recognized!");
          }
        }
      } else {
        if (!unit_label_.empty()) {
          DT_THROW_IF(! units::is_unit_label_valid(unit_label_),
                      std::logic_error,
                      "Unit label '" << unit_label_ << "' is not recognized!");
          the_unit_symbol = units::get_default_unit_symbol_from_label(unit_label_);
          std::string ul;
          units::find_unit(the_unit_symbol, the_unit_value, ul);
        }
      }
      //int oldprec = out_.precision();
      std::ostringstream oss;
      oss.precision(precision_);
      oss << (val_ / the_unit_value);
      if (! the_unit_symbol.empty()) {
        oss << unit_sep << the_unit_symbol;
      }
      if (quote) {
        out_ << '"';
      }
      out_ << oss.str();
      if (quote) {
        out_ << '"';
      }
    } else if (datatools::is_infinity(val_)) {
      if (val_ > 0) out_ << io::constants::plus_infinity_real_repr();
      else out_ << io::constants::minus_infinity_real_repr();
    } else {
      out_ << io::constants::nan_real_repr();
    }
    return;
  }

  bool io::read_real_number(const std::string & a_word,
                            double & val,
                            bool & normal,
                            uint32_t flags)
  {
    std::istringstream in(a_word);
    return read_real_number(in, val, normal, flags);
  }

  // static
  bool io::read_real_number(std::istream & in_, double & val_, bool & normal_, uint32_t flags)
  {
    bool ok = false;
    bool normal = false;
    double value = datatools::invalid_real();
    datatools::logger::priority logging = datatools::logger::PRIO_FATAL;
    bool allow_trailing_characters = false;
    if (flags & reader_allow_trailing_characters) {
      allow_trailing_characters = true;
    }
    if (flags & reader_debug) {
      logging = datatools::logger::PRIO_DEBUG;
    }
    std::string repr;
    in_ >> repr;
    if (!in_) {
      return false;
    }
    DT_LOG_DEBUG(logging, "Real representation = '" << repr << "'");
    if (! repr.empty()) {
      // Parsing:
      if (repr == io::constants::plus_infinity_real_repr()
          || repr == "+inf" || repr == "infinity" || repr == "+infinity") {
        datatools::plus_infinity(value);
        DT_LOG_DEBUG(logging, "Found +inf");
        ok = true;
      } else if (repr == io::constants::minus_infinity_real_repr()
                 || repr == "-infinity") {
        datatools::minus_infinity(value);
        DT_LOG_DEBUG(logging, "Found -inf");
        ok = true;
      } else if (repr == io::constants::nan_real_repr() || repr == "NaN") {
        datatools::invalidate(value);
        DT_LOG_DEBUG(logging, "Found NaN");
        ok = true;
      } else {
        try {
          std::istringstream iss(repr);
          std::string real_token;
          iss >> real_token;
          DT_LOG_DEBUG(logging, "Real token = " << real_token);
          value = boost::lexical_cast<double>(real_token);
          ok = true;
          normal = true;
          DT_LOG_DEBUG(logging, "Real value = " << value);
        } catch (...) {
          DT_LOG_DEBUG(logging, "Cast failed!");
          ok = false;
        }
      }
      in_ >> std::ws;
      if (ok && !in_.eof()) {
        char trailing = 0;
        trailing = in_.peek();
        if (trailing != 0) {
          DT_LOG_DEBUG(logging, "Found trailing character = '" << trailing << "' !");
          if (! allow_trailing_characters) {
            DT_LOG_DEBUG(logging, "Ooops! Trailing character = '" << trailing << "' is not allowed.");
            ok = false;
            // }
          } else {
            DT_LOG_DEBUG(logging, "Trailing characters are allowed.");
          }
        }
      }
      // if (ok) {
      //   std::string trailing;
      //   in_ >> trailing;
      //   if (! allow_trailing_characters) {
      //     DT_LOG_DEBUG(logging, "Trailing characters are not allowed.");
      //     // Check invalid trailing characters:
      //     if (!trailing.empty()) {
      //       DT_LOG_DEBUG(logging, "Ooops! Found forbidden trailing characters!");
      //       ok = false;
      //     }
      //   }
      // }
    } // end parsing
    if (ok) {
      val_ = value;
      normal_ = normal;
    }
    return ok;
  }

  size_t io::indenter::get_width() const {
    return width_;
  }

  size_t io::indenter::get_level() const {
    return level_;
  }

  io::indenter::indenter() {
    width_ = 4;
    level_ = 0;
  }

  io::indenter& io::indenter::operator()(size_t l) {
    this->level_ = l;
    return *this;
  }

  std::ostream& io::indenter::operator()(std::ostream& out) const {
    out << *this;
    return out;
  }

  io::indenter& io::indenter::operator++(int) {
    level_++;
    return *this;
  }

  io::indenter & io::indenter::operator--(int) {
    if (level_ > 0) level_--;
    return *this;
  }

  std::ostream& operator<<(std::ostream& out, const io::indenter& indent) {
    for (size_t i = 0; i < (indent.width_ * indent.level_); ++i) {
      out << ' ';
    }
    return out;
  }

  std::ostream& io::ostream_width(std::ostream& os, const int& n) {
    os.width((int)n);
    return os;
  }

  ostream_manipulator<int> io::width(const int& n) {
    return ostream_manipulator<int>(&io::ostream_width, n);
  }

  std::ostream& io::ostream_precision(std::ostream& os, const int& n) {
    os.precision((int)n);
    return os;
  }

  ostream_manipulator<int> io::precision(const int& n) {
    return ostream_manipulator<int>(&io::ostream_precision, n);
  }

  bool io::is_colored() const {
    return _colored_stream_;
  }

  void io::set_colored(bool c) {
    if (_colored_stream_ && c) return;
    if (!_colored_stream_ && !c) return;
    if (c) {
      _colored_stream_ = true;
    } else {
      _colored_stream_ = false;
    }
    return;
  }

  io::io()
  {
    _colored_stream_ = false;
    return;
  }

  io::~io()
  {
    if (is_colored()) {
      set_colored(false);
    }
    return;
  }

  std::ostream& io::normal(std::ostream& out) {
    if (io::instance().is_colored()) {
    }
    return out;
  }

  std::ostream& io::reverse(std::ostream& out) {
    if (io::instance().is_colored()) {
    }
    return out;
  }

  std::ostream& io::bold(std::ostream& out) {
    if (io::instance().is_colored()) {
    }
    return out;
  }

  std::ostream& io::underline(std::ostream& out) {
    if (io::instance().is_colored()) {
    }
    return out;
  }

  std::ostream& io::red(std::ostream& out) {
    if (io::instance().is_colored()) {
    }
    return out;
  }

  std::ostream& io::green(std::ostream& out) {
    if (io::instance().is_colored()) {
    }
    return out;
  }

  std::ostream& io::debug(std::ostream& out) {
    out << "DEBUG: ";
    return out;
  }

  std::ostream& io::devel(std::ostream& out) {
    out << "DEVEL: ";
    return out;
  }

  std::ostream& io::notice(std::ostream& out) {
    out << "NOTICE: ";
    return out;
  }

  std::ostream& io::warning(std::ostream& out) {
    out << "WARNING: ";
    return out;
  }

  std::ostream& io::error(std::ostream& out) {
    out << "ERROR: ";
    return out;
  }

  std::ostream& io::verbose(std::ostream& out) {
    out << "VERBOSE: ";
    return out;
  }

  std::ostream& io::tab(std::ostream& out) {
    out << '\t';
    return out;
  }

  std::ostream& io::vtab(std::ostream& out) {
    out << '\v';
    return out;
  }

  std::ostream& io::page_break(std::ostream& out) {
    out << '\f';
    return out;
  }

  std::ostream& io::left(std::ostream& out) {
    out.setf(std::ios::left, std::ios::adjustfield);
    return out;
  }

  std::ostream& io::right(std::ostream& out) {
    out.setf(std::ios::right, std::ios::adjustfield);
    return out;
  }

  std::ostream& io::internal(std::ostream& out) {
    out.setf(std::ios::internal, std::ios::adjustfield);
    return out;
  }

  std::ostream& io::showbase(std::ostream& out) {
    out.setf(std::ios::showbase);
    return out;
  }

  std::string io::to_binary(const uint32_t& val, bool show_all) {
    std::ostringstream oss;
    size_t nbits = sizeof(val) * 8;
    oss << '(';
    bool start = false;
    for (int i = (nbits - 1); i >= 0; i--) {
      bool abit;
      abit = (val >> i) & 0x1;
      if (!start & abit) start = true;
      if (!show_all && !abit && !start) continue;
      oss << abit;
    }
    oss << ')';
    return oss.str();
  }

  std::string io::to_binary_2(const uint32_t& val) {
    std::ostringstream oss;
    size_t nbits = sizeof(val) * 8;
    bool start = false;
    for (int i = (nbits - 1); i >= 0; i--) {
      bool abit;
      abit = (val >> i) & 0x1;
      if (!start & abit) start = true;
      if (!abit && !start) continue;
      oss << abit;
    }
    return oss.str();
  }

} // namespace datatools
