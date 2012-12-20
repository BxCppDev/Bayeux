// -*- mode: c++; -*-
// ioutils.cc
// Ourselves
#include <datatools/ioutils.h>

// Standard Library
#include <sstream>

// Third Party
// - A

// This Project

namespace datatools {

bool io::g_colored_stream_ = false;
io   io::g_io_;

io::indenter io::indent;


size_t io::indenter::get_width() const {
  return width_;
}


size_t io::indenter::get_level() const {
  return level_;
}

// ctor:
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


bool io::is_colored() {
  return g_colored_stream_;
}


void io::set_colored(bool c) {
  if (io::g_colored_stream_ && c) return;
  if (!io::g_colored_stream_ && !c) return;
  if (c) {
    io::g_colored_stream_ = true;
  } else {
    io::g_colored_stream_ = false;
  }
}

std::ostream& io::normal(std::ostream& out) {
  if (io::is_colored()) {
  }
  return out;
}


std::ostream& io::reverse(std::ostream& out) {
  if (io::is_colored()) {
  }
  return out;
}


std::ostream& io::bold(std::ostream& out) {
  if (io::is_colored()) {
  }
  return out;
}


std::ostream& io::underline(std::ostream& out) {
  if (io::is_colored()) {
  }
  return out;
}

std::ostream& io::red(std::ostream& out) {
  if (io::is_colored()) {
  }
  return out;
}


std::ostream& io::green(std::ostream& out) {
  if (io::is_colored()) {
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

