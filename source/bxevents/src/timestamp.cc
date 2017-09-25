// timestamp.cc

// Ourselves:
#include <events/timestamp.h>

// Standard library:
#include <cstdlib>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <chrono>

// Third party:
// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/exception.h>
#include <datatools/clhep_units.h>

namespace events {

  // serial tag for datatools::i_serializable interface :
  DATATOOLS_SERIALIZATION_SERIAL_TAG_IMPLEMENTATION(timestamp, "events::timestamp")

  // static:
  const int64_t timestamp::SECONDS_INVALID;
  const int64_t timestamp::SECONDS_MINUS_INFINITY;
  const int64_t timestamp::SECONDS_MIN;
  const int64_t timestamp::SECONDS_ZERO;
  const int64_t timestamp::SECONDS_MAX;
  const int64_t timestamp::SECONDS_PLUS_INFINITY;

  const uint64_t timestamp::FEMTOSECONDS_MIN;
  const uint64_t timestamp::FEMTOSECONDS_MAX;
  const uint64_t timestamp::FEMTOSECONDS_INVALID;

  const char timestamp::IO_FORMAT_OPEN;
  const char timestamp::IO_FORMAT_SEP;
  const char timestamp::IO_FORMAT_CLOSE;
  const char timestamp::IO_FORMAT_INVALID;

  // static:
  bool timestamp::is_finite_seconds(const int64_t sec_)
  {
    if (sec_ == SECONDS_INVALID) return false;
    if (sec_ == SECONDS_MINUS_INFINITY) return false;
    if (sec_ == SECONDS_PLUS_INFINITY) return false;
    return true;
  }

  // static:
  bool timestamp::is_min_seconds(const int64_t sec_)
  {
    if (sec_ == SECONDS_MIN) return true;
    return false;
  }

  // static:
  bool timestamp::is_max_seconds(const int64_t sec_)
  {
    if (sec_ == SECONDS_MAX) return true;
    return false;
  }

  bool timestamp::is_valid() const
  {
    if (!has_seconds()) return false;
    if (!has_femtoseconds()) return false;
    return true;
  }

  void timestamp::invalidate()
  {
    reset_seconds();
    reset_femtoseconds();
    return;
  }

  int timestamp::compare(const timestamp & ts_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid timestamp (this) !");
    DT_THROW_IF(! ts_.is_valid(), std::logic_error, "Invalid timestamp (argument) !");
    if (_seconds_ < ts_._seconds_) return -1;
    if (_seconds_ > ts_._seconds_) return +1;
    if (get_femtoseconds() < ts_.get_femtoseconds()) return -1;
    if (get_femtoseconds() > ts_.get_femtoseconds()) return +1;
    return 0;
  }

  bool timestamp::has_seconds() const
  {
    return _seconds_ != SECONDS_INVALID;
  }

  int64_t timestamp::get_seconds() const
  {
    return _seconds_;
  }

  void timestamp::set_seconds(const int64_t s_)
  {
    _seconds_ = s_;
    return;
  }

  void timestamp::reset_seconds()
  {
    _seconds_ = SECONDS_INVALID;
    return;
  }

  uint64_t timestamp::get_milliseconds() const
  {
    return get_femtoseconds() / 1000000000000;
  }

  uint64_t timestamp::get_microseconds() const
  {
    return get_femtoseconds() / 1000000000;
  }

  uint64_t timestamp::get_nanoseconds() const
  {
    return get_femtoseconds() / 1000000;
  }

  uint64_t timestamp::get_picoseconds() const
  {
    return get_femtoseconds() / 1000;
  }

  uint64_t timestamp::get_femtoseconds() const
  {
    return _femtoseconds_;
  }

  bool timestamp::has_femtoseconds() const
  {
    return _femtoseconds_ != FEMTOSECONDS_INVALID;
  }

  void timestamp::set_milliseconds(const uint64_t ms_)
  {
    set_femtoseconds(ms_ * 1000000000000);
    return;
  }

  void timestamp::set_microseconds(const uint64_t us_)
  {
    set_femtoseconds(us_ * 1000000000);
    return;
  }

  void timestamp::set_nanoseconds(const uint64_t ns_)
  {
    set_femtoseconds(ns_ * 1000000);
    return;
  }

  void timestamp::set_picoseconds(const uint64_t ps_)
  {
    set_femtoseconds(ps_ * 1000);
    return;
  }

  void timestamp::set_femtoseconds(const uint64_t fs_)
  {
    DT_THROW_IF(fs_ > FEMTOSECONDS_MAX, std::range_error,
                "Invalid number of femtoseconds!");
    _femtoseconds_ = fs_;
    return;
  }

  void timestamp::reset_femtoseconds()
  {
    _femtoseconds_ = FEMTOSECONDS_INVALID;
    return;
  }

  double timestamp::to_real(const int64_t time_ref_) const
  {
    DT_THROW_IF(! is_valid(), std::logic_error, "Invalid timestamp!");
    if (! is_valid()) return std::numeric_limits<double>::quiet_NaN();
    if (_seconds_ == SECONDS_MINUS_INFINITY) return -std::numeric_limits<double>::infinity();
    if (_seconds_ == SECONDS_PLUS_INFINITY) return std::numeric_limits<double>::infinity();
    double time = (_seconds_ - time_ref_) * CLHEP::second;
    time += (_femtoseconds_ * 1e-6 * CLHEP::nanosecond);
    return time;
  }

  bool timestamp::from_real(const double time_, const int64_t from_)
  {
    if (!datatools::is_valid(time_)) {
      invalidate();
      return false;
    } else if (time_ == std::numeric_limits<double>::infinity()) {
      set_seconds(SECONDS_PLUS_INFINITY);
    } else if (time_ == -std::numeric_limits<double>::infinity()) {
      set_seconds(SECONDS_MINUS_INFINITY);
      set_femtoseconds(0);
    } else {
      double time_sec = (time_ - from_) / CLHEP::second;
      double time_frac = time_sec - std::floor(time_sec);
      set_seconds(std::floor(time_sec));
      set_femtoseconds(std::floor(time_frac / (1e-6 * CLHEP::nanosecond)));
    }
    return true;
  }

  timestamp::timestamp()
  {
    invalidate();
    return;
  }

  timestamp::timestamp(const int64_t sec_)
  {
    set_seconds(sec_);
    set_femtoseconds(0);
    return;
  }

  timestamp::timestamp(const int64_t sec_, const uint64_t fs_)
  {
    set_seconds(sec_);
    set_femtoseconds(fs_);
    return;
  }

  timestamp::~timestamp()
  {
    return;
  }

  timestamp & timestamp::add_seconds(const int64_t shift_)
  {
    _seconds_ += shift_;
    return *this;
  }

  // void timestamp::now()
  // {
  //   std::chrono::time_point<std::chrono::system_clock> now;
  //   now = std::chrono::system_clock::now();
  //   time_t tt = system_clock::to_time_t(now);
  //   tm utc_tm = *gmtime(&tt);
  //   set_seconds(
  //   return;
  // }

  // timestamp & timestamp::add_femtoseconds(const uint64_t fs_shift_);
  // {
  //   uint64_t fs = _femtoseconds_;
  //   _seconds_ += shift_;
  //   return *this;
  // }

  // timestamp & timestamp::add(const int64_t seconds_, const uint64_t femtosec)
  // {
  // }

  bool operator==(const timestamp & ts1_, const timestamp & ts2_)
  {
    return ts1_.compare(ts2_) == 0;
  }

  bool operator<(const timestamp & ts1_, const timestamp & ts2_)
  {
    return ts1_.compare(ts2_) == -1;
  }

  bool operator>(const timestamp & ts1_, const timestamp & ts2_)
  {
    return ts1_.compare(ts2_) == +1;
  }

  bool operator<=(const timestamp & ts1_, const timestamp & ts2_)
  {
    return ts1_.compare(ts2_) <= 0;
  }

  bool operator>=(const timestamp & ts1_, const timestamp & ts2_)
  {
    return ts1_.compare(ts2_) >= 0;
  }

  void timestamp::to_string(std::string & str_) const
  {
    std::ostringstream out;
    out << *this;
    str_ = out.str();
    return;
  }

  std::string timestamp::to_string() const
  {
    std::ostringstream out;
    out << *this;
    return out.str();
  }

  void timestamp::from_string(const std::string & str_)
  {
    timestamp ts;
    std::istringstream in(str_);
    in >> ts;
    DT_THROW_IF(! in, std::logic_error, "Format error '" << str_ << "'!");
    *this = ts;
    return;
  }

  std::ostream & operator<<(std::ostream & out_, const timestamp & ts_)
  {
    if (!ts_.is_valid()) {
      out_ << timestamp::IO_FORMAT_OPEN;
      out_ << timestamp::IO_FORMAT_INVALID;
      out_ << timestamp::IO_FORMAT_CLOSE;
    } else {
      out_ << timestamp::IO_FORMAT_OPEN;
      out_ << ts_._seconds_;
      out_ << timestamp::IO_FORMAT_SEP;
      // out_.right(15);
      out_ << std::setfill('0') << std::setw(15) << ts_._femtoseconds_;
      out_ << timestamp::IO_FORMAT_CLOSE;
    }
    return out_;
  }

  std::istream & operator>>(std::istream & a_in, timestamp & /*ts_*/)
  {
    /*
      char c = 0;
      a_in >> c;
      if (! a_in) return a_in;
      if (c != timestamp::IO_FORMAT_OPEN) {
      a_in.setstate(std::ios_base::failbit);
      return a_in;
      }
      int64_t s, ps;
      c = 0;

      c = a_in.peek();
      if (c == timestamp::IO_FORMAT_INVALID) {
      s = timestamp::INVALID_SECONDS;
      a_in.get();
      } else {
      a_in >> s;
      if (! a_in) return a_in;
      }

      c = 0;
      a_in >> c;
      if (c != timestamp::IO_FORMAT_SEP) {
      a_in.setstate(std::ios_base::failbit);
      return a_in;
      }

      c = a_in.peek();
      if (c == timestamp::IO_FORMAT_INVALID) {
      ps = timestamp::INVALID_SECONDS;
      a_in.get();
      } else {
      a_in >> ps;
      if (! a_in) return a_in;
      }

      c = 0;
      a_in >> c;
      if (c != timestamp::IO_FORMAT_CLOSE) {
      a_in.setstate(std::ios_base::failbit);
      return a_in;
      }
      ts_.set_seconds(s);
      ts_.set_femtoseconds(ps);
    */
    return a_in;
  }

  void timestamp::tree_dump(std::ostream & out_,
                            const std::string & title_,
                            const std::string & indent_,
                            bool inherit_) const
  {
    if (!title_.empty()) {
      out_ << indent_ << title_ << std::endl;
    }

    out_ << indent_ <<  datatools::i_tree_dumpable::tag
         << "Seconds : ";
    if (has_seconds()) {
      out_ << get_seconds();
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ <<  datatools::i_tree_dumpable::tag
         << "Femtoseconds : ";
    if (has_femtoseconds()) {
      out_ << get_femtoseconds();
    } else {
      out_ << "<none>";
    }
    out_ << std::endl;

    out_ << indent_ <<  datatools::i_tree_dumpable::inherit_tag(inherit_)
         << "Validity : " << std::boolalpha << is_valid() << std::endl;

    return;
  }

} // end of namespace events
