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

// This Project:
#include <datatools/utils.h>
#include <datatools/units.h>

namespace datatools {

  void print_multi_lines(std::ostream & out_,
                         const std::string & text_,
                         const std::string & indent_)
  {
    if (text_.empty()) return;
    std::vector<std::string> lines;
    boost::split(lines,text_,boost::is_any_of("\n"));
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
                         bool a_bool)
  {
    if (a_bool) {
      a_out << "1";
    } else {
      a_out << "0";
    }
    return;
  }

  bool io::read_boolean(std::istream& a_in,
                        bool& a_bool)
  {
    a_in >> std::ws;
    std::string token;
    a_in >> token;
    if (token.empty()) {
      return false;
    }
    boost::to_upper(token);
    if (token == "0"
        || token == "FALSE" || token == "F"
        || token == "NO" || token == "N") {
      a_bool = false;
      return true;
    } else if (token == "1"
        || token == "TRUE" || token == "T"
        || token == "YES" || token == "Y") {
      a_bool = true;
      return true;
    }
    return false;
  }

  bool io::read_integer(std::istream& a_in, int& a_integer)
  {
    a_in >> std::ws;
    std::string token;
    a_in >> token;
    if (token.empty()) {
      return false;
    }
    std::istringstream iss(token);
    iss >> a_integer;
    if (!iss) {
      return false;
    }
    return false;
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

  bool io::read_quoted_string(std::istream& a_in, std::string& a_str)
  {
    std::string chain = "";
    a_in >> std::ws;
    if (!a_in) {
      a_str = chain;
      return true;
    }

    bool quoted = false;
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
      return false;
    }

    a_str = chain;
    return true;
  }

// static
void io::write_real_number(std::ostream & out_,
                           double val_,
                           int precision_,
                           const std::string& unit_symbol_,
                           const std::string& unit_label_,
                           double unit_value_)
{
  // std::cerr << "DEVEL: io::write_real_number: unit_symbol_ = '" << unit_symbol_ << "'" << std::endl;
  // std::cerr << "DEVEL: io::write_real_number: unit_label_  = '" << unit_label_ << "'" << std::endl;
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
        if (unit_value_ > 0.0) {
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
    int oldprec = out_.precision();
    out_.precision(precision_);
    out_ << (val_ / the_unit_value);
    if (! the_unit_symbol.empty()) {
      out_ << ' ' << the_unit_symbol;
    }
    out_.precision(oldprec);
  } else if (datatools::is_infinity(val_)) {
    if (val_ > 0) out_ << io::constants::plus_infinity_real_repr();
    else out_ << io::constants::minus_infinity_real_repr();
  } else {
    out_ << io::constants::nan_real_repr();
  }
  return;
}

// static
bool io::read_real_number(std::istream & in_, double & val_, bool & normal_)
{
  normal_ = false;
  std::string real_token;
  in_ >> real_token;
  if (!in_) {
    return false;
  }
  if (real_token == io::constants::plus_infinity_real_repr()) {
    datatools::plus_infinity(val_);
  }
  else if (real_token == io::constants::minus_infinity_real_repr()) {
    datatools::minus_infinity(val_);
  }
  else if (real_token == io::constants::nan_real_repr()) {
    datatools::invalidate(val_);
  }
  else {
    std::istringstream iss(real_token);
    iss >> val_;
    if (! iss) {
      return false;
    }
    normal_ = true;
  }
  return true;
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

} // namespace datatools
