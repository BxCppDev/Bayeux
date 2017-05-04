// real_range.cc

// Ourselves:
#include <datatools/real_range.h>

// Standard Library:
#include <cmath>
#include <limits>
#include <sstream>
#include <stdexcept>

// Third party:
// - Boost
#include <boost/algorithm/string.hpp>

// This Project:
#include <datatools/utils.h>
#include <datatools/exception.h>
#include <datatools/units.h>

namespace datatools {

  void real_range::_set_defaults()
  {
    unit_label_.clear();
    preferred_unit_.clear();
    lower_         = std::numeric_limits<double>::quiet_NaN();
    lower_flag_    = range_bound_undefined;
    upper_         = std::numeric_limits<double>::quiet_NaN();
    upper_flag_    = range_bound_undefined;
    return;
  }

  real_range::real_range()
  {
    _set_defaults();
    return;
  }

  real_range::real_range(double a_from, double a_to,
                         range_bound_info_type a_from_policy,
                         range_bound_info_type a_to_policy)
  {
    _set_defaults();
    this->set(a_from, a_to, a_from_policy, a_to_policy);
    return;
  }

  bool real_range::is_valid() const
  {
    return (lower_flag_ != range_bound_undefined)
      && (upper_flag_ != range_bound_undefined);
  }

  void real_range::invalidate()
  {
    this->reset();
    return;
  }

  void real_range::reset()
  {
    _set_defaults();
    return;
  }

  void real_range::reset_lower()
  {
    lower_      = std::numeric_limits<double>::quiet_NaN();
    lower_flag_ = range_bound_undefined;
    return;
  }

  void real_range::reset_upper()
  {
    upper_      = std::numeric_limits<double>::quiet_NaN();
    upper_flag_ = range_bound_undefined;
    return;
  }

  void real_range::set_lower(double a_from, range_bound_info_type a_policy)
  {
    DT_THROW_IF(!datatools::is_valid(a_from), std::logic_error, "Invalid lower bound !");
    DT_THROW_IF(a_policy == range_bound_undefined, std::logic_error, "Invalid lower bound policy !");
    if (a_policy == range_bound_unbounded) {
      // "(,..."
      lower_      = -std::numeric_limits<double>::infinity();
      lower_flag_ = range_bound_unbounded;
    } else {
      if (datatools::is_valid(upper_)) {
        DT_THROW_IF(a_from > upper_, std::logic_error, "Invalid lower/upper bounds !");
      }
      // "[value,..." || "(value,..."
      lower_      = a_from;
      lower_flag_ = a_policy;
    }
    return;
  }

  void real_range::set_upper(double a_to, range_bound_info_type a_policy)
  {
    DT_THROW_IF(!datatools::is_valid(a_to), std::logic_error, "Invalid upper bound !");
    DT_THROW_IF(a_policy == range_bound_undefined, std::logic_error, "Invalid upper bound policy !");
    if (a_policy == range_bound_unbounded) {
      // "...,...)"
      upper_      = +std::numeric_limits<double>::infinity();
      upper_flag_ = a_policy;
    } else {
      // "...,a_to]" || "...,a_to)"
      if (datatools::is_valid(lower_)) {
        DT_THROW_IF(a_to < lower_, std::logic_error, "Invalid lower/upper bounds !");
      }
      upper_      = a_to;
      upper_flag_ = a_policy;
    }
     return;
  }

  void real_range::set(double a_from, double a_to,
                       range_bound_info_type a_from_policy,
                       range_bound_info_type a_to_policy)
  {
    reset_lower();
    reset_upper();
    this->set_lower(a_from, a_from_policy);
    this->set_upper(a_to, a_to_policy);
    return;
  }

  bool real_range::is_lower_bounded() const
  {
    return lower_flag_ > 0;
  }

  bool real_range::is_upper_bounded() const
  {
    return upper_flag_ > 0;
  }

  bool real_range::is_half_bounded() const
  {
    return
      (this->is_lower_bounded() && !this->is_upper_bounded()) ||
      (!this->is_lower_bounded() && this->is_upper_bounded());
  }

  bool real_range::is_bounded() const
  {
    return this->is_lower_bounded() && this->is_upper_bounded();
  }

  double real_range::get_lower() const
  {
    DT_THROW_IF (!this->is_lower_bounded(),std::logic_error,"No lower bound !");
    return lower_;
  }

  double real_range::get_upper() const
  {
    DT_THROW_IF (!this->is_upper_bounded(),std::logic_error,"No upper bound !");
    return upper_;
  }

  double real_range::_effective_tolerance(double a_tolerance) const
  {
    double eps = 0.0;
    if (datatools::is_valid(a_tolerance) && a_tolerance >= 0.0) {
      eps = a_tolerance;
    }
    return 0.5 * eps;
  }

  double real_range::width(double a_tolerance) const
  {
    DT_THROW_IF (!this->is_bounded(),std::logic_error, "Unbounded interval !");
    double w = last(a_tolerance) - first(a_tolerance);
    if (datatools::is_valid(w)) {
      double eps = _effective_tolerance(a_tolerance);
      if (w < -eps) {
        datatools::invalidate(w);
      } else if (w < eps) {
        w = 0.0;
      }
    }
    return w;
  }

  bool real_range::is_singleton(double a_tolerance) const
  {
    if (is_bounded()) {
      double w = width(a_tolerance);
      if (datatools::is_valid(w) && w == 0.0) {
        return true;
      }
    }
    return false;
  }

  double real_range::singleton(double a_tolerance) const
  {
    DT_THROW_IF (!this->is_singleton(a_tolerance),std::logic_error,"Not a singleton !");
    if (this->is_lower_included() && !this->is_upper_included()) {
      return first(a_tolerance);
    }
    if (this->is_upper_included() && !this->is_lower_included()) {
      return last(a_tolerance);
    }
    return 0.5 * (first(a_tolerance) + last(a_tolerance));
  }

  bool real_range::is_empty(double a_tolerance) const
  {
    return ! datatools::is_valid(this->width(a_tolerance));
  }

  bool real_range::is_lower_included() const
  {
    DT_THROW_IF (!this->is_lower_bounded(),std::logic_error,"No lower bound !");
    return (lower_flag_ == range_bound_included);
  }

  bool real_range::is_upper_included() const
  {
    DT_THROW_IF (!this->is_upper_bounded(),std::logic_error,"No upper bound !");
    return (upper_flag_ == range_bound_included);
  }

  void real_range::make_singleton(double a_value)
  {
    this->set(a_value, a_value, range_bound_included, range_bound_included);
    return;
  }

  void real_range::make_empty()
  {
    this->set(0, 0, range_bound_excluded, range_bound_excluded);
    return;
  }

  void real_range::make_positive_unbounded() {
    make_upper_unbounded(0, true);
    return;
  }

  void real_range::make_negative_unbounded() {
    make_lower_unbounded(0, true);
    return;
  }

  void real_range::make_upper_unbounded(double a_from, bool inclusive) {
    this->set(a_from,
              +std::numeric_limits<double>::infinity(),
              inclusive ? range_bound_included : range_bound_excluded ,
              range_bound_unbounded);
    return;
  }

  void real_range::make_lower_unbounded(double to, bool inclusive)
  {
    this->set(-std::numeric_limits<double>::infinity(),
              to,
              range_bound_unbounded,
              inclusive ? range_bound_included : range_bound_excluded);
    return;
  }

  void real_range::make_unbounded()
  {
    this->set(-std::numeric_limits<double>::infinity(),
              +std::numeric_limits<double>::infinity(),
              range_bound_unbounded,
              range_bound_unbounded);
    return;
  }

  void real_range::make_bounded(double a_from, double a_to,
                                bool a_lower_included, bool a_upper_included)
  {
    DT_THROW_IF(!datatools::is_valid(a_from), std::logic_error,
                "Invalid lower bound !");
    DT_THROW_IF(!datatools::is_valid(a_to), std::logic_error,
                "Invalid lower bound !");
    DT_THROW_IF(a_from > a_to, std::logic_error,
                "Invalid lower and upper bounds !");
    this->set(a_from,
              a_to,
              a_lower_included ? range_bound_included : range_bound_excluded,
              a_upper_included ? range_bound_included : range_bound_excluded);
    return;
  }

  double real_range::compute_tolerance(double a_lower, double a_upper, double a_relative_tolerance) {
    double abs_eps = std::numeric_limits<double>::quiet_NaN();
    double rel_eps = a_relative_tolerance;
    if (!::datatools::is_valid(rel_eps) || rel_eps <= 0.0) {
      rel_eps = 1.5 * std::numeric_limits<double>::epsilon();
    }
    double l_eps = std::numeric_limits<double>::quiet_NaN();
    double u_eps = std::numeric_limits<double>::quiet_NaN();

    if (::datatools::is_valid(a_lower) && !::datatools::is_infinity(a_lower)) {
      double scale = std::abs(a_lower);
      l_eps = rel_eps * scale;
    }

    if (::datatools::is_valid(a_upper) && !::datatools::is_infinity(a_upper)) {
      double scale = std::abs(a_upper);
      u_eps = rel_eps * scale;
    }

    if (::datatools::is_valid(l_eps) && ::datatools::is_valid(u_eps)) {
      abs_eps = std::max(l_eps, u_eps);
    } else if (::datatools::is_valid(l_eps)) {
      abs_eps = l_eps;
    } else if (::datatools::is_valid(u_eps)) {
      abs_eps = u_eps;
    }

    return abs_eps;
  }

  bool real_range::has(const real_range & rr, double a_tolerance) const
  {
    // std::cerr << "DEVEL: real_range::has: Entering..." << std::endl;
    // std::cerr << "DEVEL: real_range::has:   this = " << *this << std::endl;
    // std::cerr << "DEVEL: real_range::has:   rr   = " << rr << std::endl;
    if (!rr.is_lower_bounded()) {
      if (this->is_lower_bounded()) {
        return false;
      }
    } else {
      double a_first = rr.first(0.0);
      // std::cerr << "DEVEL: real_range::has: first=" << first << std::endl;
      if (!this->has(a_first, a_tolerance)) return false;
    }
    if (!rr.is_upper_bounded()) {
      if (this->is_upper_bounded()) {
        return false;
      }
    } else {
      double a_last = rr.last(0.0);
      // std::cerr << "DEVEL: real_range::has: last=" << last << std::endl;
      if (!this->has(a_last, a_tolerance)) return false;
    }
    return true;
  }

  bool real_range::has(double a_value, double a_tolerance) const
  {
    double eps = _effective_tolerance(a_tolerance);
    // std::cerr.precision(15);

    if (this->is_lower_bounded()) {
      // std::cerr << "DEVEL: real_range::has: is_lower_bounded=" << 1 << std::endl;
      // std::cerr << "DEVEL: real_range::has:   value=" << a_value << std::endl;
      if (this->is_lower_included()) {
        // std::cerr << "DEVEL: real_range::has:   l = " << lower_ << std::endl;
        if (a_value < lower_) return false;
      } else {
        // std::cerr << "DEVEL: real_range::has:   l+e = " << (lower_ + eps) << std::endl;
        if (a_value < (lower_ + eps)) return false;
      }
    }

    if (this->is_upper_bounded()) {
      // std::cerr << "DEVEL: real_range::has: is_upper_bounded=" << 1 << std::endl;
      // std::cerr << "DEVEL: real_range::has:   value=" << a_value << std::endl;
      if (this->is_upper_included()) {
        // std::cerr << "DEVEL: real_range::has:   u = " << upper_ << std::endl;
        if (a_value > upper_) return false;
      } else {
        // std::cerr << "DEVEL: real_range::has:   u-e = " << (upper_ - eps) << std::endl;
        if (a_value > (upper_ - eps)) return false;
      }
    }
    return true;
  }


  double real_range::first(double a_tolerance) const
  {
    DT_THROW_IF (!this->is_lower_bounded(),
                 std::logic_error,
                 "Not lower bounded !");
    double the_first = lower_;
    if (! this->is_lower_included()) {
      // We shift on the right by some epsilon:
      double eps = _effective_tolerance(a_tolerance);
      the_first += eps;
      if (this->is_upper_bounded()) {
        // We check if the 'first' value is still inside the range (upper side):
        if (is_upper_included() && the_first > upper_) {
          // We cannot go behond the upper bound:
          the_first = upper_;
        }
        if (! is_upper_included() && the_first >= upper_) {
          // // We go only half the range:
          // the_first = lower_ + 0.5 * (upper_ - lower_);
          datatools::invalidate(the_first);
        }
      }
    }
    return the_first;
  }

  double real_range::last(double a_tolerance) const
  {
    DT_THROW_IF (!this->is_upper_bounded(),
                 std::logic_error,
                 "Not upper bounded !");
    double the_last = upper_;
    if (! this->is_upper_included()) {
      // We shift on the left by some epsilon:
      double eps = _effective_tolerance(a_tolerance);
      the_last -= eps;
      if (this->is_lower_bounded()) {
        // We check if the 'last' value is still inside the range (lower side):
        if (is_lower_included() && the_last < lower_) {
          // We cannot go behond the lower bound:
          the_last = lower_;
        }
        if (! is_lower_included() && the_last <= lower_) {
          // // We go only half the range:
          // the_last = upper_ - 0.5 * (upper_ - lower_);
          datatools::invalidate(the_last);
        }
      }
    }
    return the_last;
  }

  bool real_range::has_preferred_unit() const
  {
    return !preferred_unit_.empty();
  }

  void real_range::reset_preferred_unit()
  {
    preferred_unit_.clear();
    return;
  }

  void real_range::set_preferred_unit(const std::string & a_pus)
  {
    double uv;
    std::string ul;
    DT_THROW_IF(!units::find_unit(a_pus, uv, ul), std::logic_error,
                "Preferred unit '" << a_pus << "' is not supported!");
    if (has_unit_label()) {
      DT_THROW_IF(ul != unit_label_, std::logic_error,
                  "Preferred unit '"<< a_pus << "' is not compatible with '" << unit_label_ << "' !");
    } else {
      set_unit_label(ul);
    }
    preferred_unit_ = a_pus;
    return;
  }

  const std::string & real_range::get_preferred_unit() const
  {
    return preferred_unit_;
  }

  bool real_range::has_unit_label() const
  {
    return !unit_label_.empty();
  }

  void real_range::reset_unit_label()
  {
    unit_label_.clear();
    return;
  }

  void real_range::set_unit_label(const std::string & a_ul)
  {
    if (has_preferred_unit()) {
      double uv;
      std::string ul;
      DT_THROW_IF(units::find_unit(preferred_unit_, uv, ul), std::logic_error,
                  "Preferred unit '" << preferred_unit_ << "' is not supported!");
      DT_THROW_IF(ul != a_ul, std::logic_error,
                  "Unit label '" << a_ul << "' is not compatible with preferred unit '"
                  << preferred_unit_ << "' is not supported!");
    } else {
      // std::string dus = unit::get_default_unit_symbol_from_label(a_ul);
      // set_preferred_unit(dus);
    }
    unit_label_ = a_ul;
    return;
  }

  const std::string & real_range::get_unit_label() const
  {
    return unit_label_;
  }

  void real_range::dump(std::ostream& a_out, double a_tolerance) const {
    a_out << "real_range: " << std::endl;
    a_out << "|-- " << "Unit label     = '" << this->unit_label_ << "'" << std::endl;
    a_out << "|-- " << "Preferred unit = '" << this->preferred_unit_ << "'" << std::endl;
    if (this->is_valid()) {
      a_out << "|-- " << "Half bounded   = " << this->is_half_bounded() << std::endl;

      a_out << "|-- " << "Lower bounded  = " << this->is_lower_bounded() << std::endl;
      if (this->is_lower_bounded()) {
        a_out << "|   " << "|-- " << "Lower bound    = " << lower_ << std::endl;
        a_out << "|   " << "|-- " << "Lower included = " << this->is_lower_included() << std::endl;
        a_out.precision(15);
        a_out << "|   " << "`-- " << "First          = " << this->first(a_tolerance) << std::endl;
      }
      a_out << "|-- " << "Upper bounded  = " << this->is_upper_bounded() << std::endl;
      if (this->is_upper_bounded()) {
        a_out << "|   " << "|-- " << "Upper bound    = " << upper_ << std::endl;
        a_out << "|   " << "|-- " << "Upper included = " << this->is_upper_included() << std::endl;
        a_out.precision(15);
        a_out << "|   " << "`-- " << "Last           = " << this->last(a_tolerance) << std::endl;
      }
      a_out << "|-- " << "Bounded        = " << this->is_bounded() << std::endl;
      if (this->is_bounded()) {
        a_out.precision(15);
        a_out << "|   " << "|-- " << "Width          = " << this->width(a_tolerance) << std::endl;
        a_out << "|   " << "|-- " << "Empty          = " << this->is_empty(a_tolerance) << std::endl;
        a_out << "|   " << "`-- " << "Singleton      = " << this->is_singleton(a_tolerance) << std::endl;
        if (is_singleton(a_tolerance)) {
          a_out << "|   " << "    " << "`-- " << "Singleton value = " << this->singleton(a_tolerance) << std::endl;
        }
      }
    } else {
      a_out << "|-- " << "[invalid]" << std::endl;
    }
    a_out << "`-- " << "Rendering      = '" << *this << "'" << std::endl;
    return;
  }


  std::ostream& operator<<(std::ostream& a_out, const real_range& a_range) {
    if (!a_range.is_valid()) {
      a_out << range_tools::token_open_excluded << range_tools::token_invalid << range_tools::token_close_excluded;
    } else {
      double unit = 1.0;
      std::string display_unit;
      if (a_range.has_preferred_unit()) {
        display_unit = a_range.get_preferred_unit();
      } else if (a_range.has_unit_label()) {
        display_unit = units::get_default_unit_symbol_from_label(a_range.get_unit_label());
      }
      if (!display_unit.empty()) {
        unit = units::get_unit(display_unit);
      }
      if (a_range.is_lower_bounded()) {
        if (a_range.is_lower_included()) {
          a_out << range_tools::token_open_included;
        } else {
          a_out << range_tools::token_open_excluded;
        }
        a_out << a_range.get_lower() / unit;
        if (!display_unit.empty()) {
          a_out << ' ' << display_unit;
        }
      } else {
        a_out << range_tools::token_open_excluded;
      }
      a_out << range_tools::token_separator2;
      if (a_range.is_upper_bounded()) {
        a_out << a_range.get_upper() / unit;
        if (!display_unit.empty()) {
          a_out << ' ' << display_unit;
        }
        if (a_range.is_upper_included()) {
          a_out << range_tools::token_close_included;
        } else {
          a_out << range_tools::token_close_excluded;
        }
      } else {
        a_out << range_tools::token_close_excluded;
      }
    }
    return a_out;
  }

  // friend
  std::istream& operator>>(std::istream& a_in, real_range& a_range)
  {
    range_bound_info_type     lf = range_bound_undefined;
    double l; // lower value
    std::string l_unit_symbol;
    std::string l_unit_label;
    range_bound_info_type     uf = range_bound_undefined;
    double u; // upper value
    std::string u_unit_symbol;
    std::string u_unit_label;
    a_range.invalidate();
    char open = 0;
    a_in.get(open);
    if (!a_in) {
      return a_in;
    }
    // Parse the open character : '[' or '('
    if (open == range_tools::token_open_included) {
      lf = range_bound_included;
     } else if (open == range_tools::token_open_excluded) {
      lf = range_bound_excluded;
    } else {
      // std::cerr << "DEVEL: " << "bad open!" << std::endl;
      a_in.setstate(std::ios::failbit);
      return a_in;
    }
    // std::cerr << "DEVEL: " << "open='" << open << "'" << std::endl;
    // std::cerr << "DEVEL: " << "lf='" << lf << "'" << std::endl;
    std::string word;
    while(!a_in.eof()) {
      char dummy = 0;
      a_in.get(dummy);
      if (!a_in) {
        return a_in;
      }
      // Parse the close character : ']' or ')'
      if (dummy == range_tools::token_close_included || dummy == range_tools::token_close_excluded) {
        if (dummy == range_tools::token_close_excluded) {
          uf = range_bound_excluded;
        } else if (dummy == range_tools::token_close_included) {
          uf = range_bound_included;
        }
      }
      if (uf != range_bound_undefined) {
        // std::cerr << "DEVEL: " << "close='" << dummy << "'" << std::endl;
        // std::cerr << "DEVEL: " << "uf='" << uf << "'" << std::endl;
        break;
      } else {
        word.push_back(dummy);
      }
    }
    // std::cerr << "DEVEL: " << "word='" << word << "'" << std::endl;
    if (word.length() == 1 &&  word[0] == range_tools::token_invalid) {
      if (lf == range_bound_excluded && uf == range_bound_excluded) {
        return a_in;
      } else {
        a_in.setstate(std::ios::failbit);
        return a_in;
      }
    }
    std::vector<std::string> tokens;
    static std::string separators;
    if (separators.empty()) {
      separators += range_tools::token_separator2;
      separators += range_tools::token_separator;
    }
    boost::split(tokens, word, boost::is_any_of(separators));
    if (tokens.size() != 2) {
      a_in.setstate(std::ios::failbit);
      return a_in;
    }
    std::string ls = tokens[0];
    std::string us = tokens[1];
    boost::trim(ls);
    boost::trim(us);
    // std::cerr << "DEVEL: " << "ls='" << ls << "'" << std::endl;
    // std::cerr << "DEVEL: " << "us='" << us << "'" << std::endl;
    if (ls.empty()) {
      if (lf == range_bound_excluded) {
        lf = range_bound_unbounded;
      } else {
        a_in.setstate(std::ios::failbit);
        return a_in;
      }
    } else {
      bool l_ok = units::parse_value_with_unit(ls, l, l_unit_symbol, l_unit_label);
      if (! l_ok) {
        // Cannot parse real value from ls
        a_in.setstate(std::ios::failbit);
        return a_in;
      }
      // std::cerr << "DEVEL: " << "l='" << l << "'" << std::endl;
    }
    if (us.empty()) {
      if (uf == range_bound_excluded) {
        uf = range_bound_unbounded;
      } else {
        a_in.setstate(std::ios::failbit);
        return a_in;
      }
    } else {
      bool u_ok = units::parse_value_with_unit(us, u, u_unit_symbol, u_unit_label);
      if (! u_ok) {
        // Cannot parse real value from us
        a_in.setstate(std::ios::failbit);
        return a_in;
      }
      // std::cerr << "DEVEL: " << "u='" << u << "'" << std::endl;
    }
    if (l_unit_label.empty() && !u_unit_label.empty()) {
      // Lower bound does not have explicit unit !
      a_in.setstate(std::ios::badbit);
      return a_in;
    }
    if (!l_unit_label.empty() && u_unit_label.empty()) {
      // Upper bound does not have explicit unit !
      a_in.setstate(std::ios::badbit);
      return a_in;
    }
    if (!l_unit_label.empty() && !u_unit_label.empty() && (l_unit_label != u_unit_label)) {
      // Bounds do not have the same unit label !
      a_in.setstate(std::ios::badbit);
      return a_in;
    }
    if ((lf != range_bound_unbounded) && (uf != range_bound_unbounded)) {
      if (l > u) {
        a_in.setstate(std::ios::badbit);
        return a_in;
      }
    }
    if (! l_unit_label.empty()) {
      a_range.set_unit_label(l_unit_label);
    } else  if (! u_unit_label.empty()) {
      a_range.set_unit_label(u_unit_label);
    }
    if (! l_unit_symbol.empty()) {
      a_range.set_preferred_unit(l_unit_symbol);
    } else if (! u_unit_symbol.empty()) {
      a_range.set_preferred_unit(u_unit_symbol);
    }
    a_range.set(l, u, lf, uf);
    return a_in;
  }

  int real_range::compare(const real_range & a_range) const
  {
    DT_THROW_IF(!is_valid(), std::logic_error, "Invalid real range!");
    DT_THROW_IF(!a_range.is_valid(), std::logic_error, "Invalid real range!");
    // std::cerr << "DEVEL: real_range::compare: "
    //           << " this=" << *this << "    other=" << a_range
    //           << std::endl;
    double res = -2;
    if (is_lower_bounded()) {
      // #1 ---[f1 ;  ? ]------------>
      double f1 = first();
      if (!a_range.is_lower_bounded()) {
        // #1 ---[f1 ; ? ]------------>
        // #2 (   ; ? ]--------------->
        res = +1;
      }  else {
        // #2 ---[f2 ; ? ]------------>
        double f2 = a_range.first();
        if (f1 < f2) {
          // #1 ---[f1 ; ? ]------------>
          // #2 -----[f2 ; ? ]---------->
          res = -1;
        } else if (f1 > f2) {
          // #1 -----[f1 ; ? ]---------->
          // #2 ---[f2 ; ? ]------------>
          res = +1;
        } else {
          // f1==f2
          // #1 ---[f1 ; ? ]------------>
          // #2 ---[f2 ; ? ]------------>
          if (is_upper_bounded()) {
            // #1 ---[f1 ; l1]------------>
            double l1 = last();
            if (! a_range.is_upper_bounded()) {
              // #1 ---[f1 ; l1]------------>
              // #2 ---[f2 ;               )>
              res = -1;
            } else {
              // #2 ---[f2 ; l2]------------>
              double l2 = a_range.last();
              if (l1 < l2) {
                // #1 ---[f1 ; l1]------------>
                // #2 ---[f2 ;   l2]---------->
                res = -1;
              } else if (l1 > l2) {
                // #1 ---[f1 ;   l1]---------->
                // #2 ---[f2 ; l2]------------>
                res = +1;
              } else {
                // #1 ---[f1 ; l1]------------>
                // #2 ---[f2 ; l2]------------>
                res = 0;
              }
            }
          } else {
            // #1 ---[f1 ;                   ]>
            if (! a_range.is_upper_bounded()) {
              // #1 ---[f1 ;                   ]>
              // #2 ---[f2 ;                   ]>
              res = 0;
            } else {
              // double l2 = a_range.last();
              // #1 ---[f1 ;                   ]>
              // #2 ---[f2 ; l2]---------------]>
              res = +1;
            }
          }
        }
      }
    } else {
      // #1 (   ; ? ]--------------->
      if (a_range.is_lower_bounded()) {
        // #1 (   ; ? ]--------------->
        // #2 ---[f2 ; ? ]------------>
        res = -1;
      } else {
        // #1 (   ; ? ]--------------->
        // #2 (   ; ? ]--------------->
        if (is_upper_bounded()) {
          // #1 (   ; l1]--------------->
          double l1 = last();
          if (! a_range.is_upper_bounded()) {
            // #1 (   ; l1]--------------->
            // #2 (   ;                  )>
            res = -1;
          } else {
            double l2 = a_range.last();
            // #2 (   ; l2]--------------->
            if (l1 < l2) {
              // #1 (   ; l1]--------------->
              // #2 (   ;   l2]------------->
              res = -1;
            } else if (l1 > l2) {
              // #1 (   ;   l1]------------->
              // #2 (   ; l2]--------------->
              res = +1;
            } else {
              // #1 (   ; l1]--------------->
              // #2 (   ; l2]--------------->
              res = 0;
            }
          }
        } else {
          // #1 (         ;            [>
          // #2 (   ; ? ]--------------->
          if (a_range.is_upper_bounded()) {
            // #1 (         ;            [>
            // #2 (   ; l2 ]-------------->
            res = +1;
          } else {
            // #1 (         ;            [>
            // #2 (         ;            [>
            res = 0;
          }
        }
      }
    }
    // std::cerr << "DEVEL: real_range::compare: "
    //           << " res=" << res
    //           << std::endl;
    return res;
  }

  bool real_range::operator<(const real_range& a_range) const
  {
    return compare(a_range) == -1;
  }

  bool real_range::operator>(const real_range& a_range) const
  {
    return compare(a_range) == +1;
  }

  bool real_range::operator==(const real_range& a_range) const
  {
    return compare(a_range) == 0;
  }

} // namespace datatools
