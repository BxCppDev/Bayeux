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

  // static
  const double real_range::default_precision()
  {
    static const double _eps(5.e-15);
    return _eps;
  }

  // Ctor:
  real_range::real_range() {
    abs_precision_ = std::numeric_limits<double>::quiet_NaN();
    this->reset();
  }


  // Ctor:
  real_range::real_range(double from, double to,
                         range_bound_info_type from_policy,
                         range_bound_info_type to_policy) {
    abs_precision_ = std::numeric_limits<double>::quiet_NaN();
    this->set(from, to, from_policy, to_policy);
  }


  bool real_range::is_valid() const {
    return (lower_flag_ != range_bound_undefined)
      && (upper_flag_ != range_bound_undefined);
  }


  void real_range::invalidate() {
    this->reset();
  }


  void real_range::reset_lower() {
    lower_      = -std::numeric_limits<double>::max();
    lower_flag_ = range_bound_undefined;
  }


  void real_range::reset_upper() {
    upper_      = std::numeric_limits<double>::max();
    upper_flag_ = range_bound_undefined;
  }


  void real_range::reset() {
    this->reset_lower();
    this->reset_upper();
    this->reset_unit_label();
    this->reset_preferred_unit();
    this->reset_abs_precision();
  }


  void real_range::set_lower(double from, range_bound_info_type policy) {
    if (policy == range_bound_unbounded) {
      // "(,..."
      lower_      = -std::numeric_limits<double>::infinity();
      lower_flag_ = policy;
    } else if (policy == range_bound_included || policy == range_bound_excluded) {
      // "[value,..." || "(value,..."
      lower_      = from;
      lower_flag_ = policy;
    } else {
      DT_THROW_IF(true, std::logic_error, "Invalid lower bound policy !");
    }
    if (!has_abs_precision()) {
      set_abs_precision(0.0);
    }
    return;
  }


  void real_range::set_upper(double value, range_bound_info_type policy) {
    if (policy == range_bound_unbounded) {
      // "...,...)"
      upper_      = +std::numeric_limits<double>::infinity();
      upper_flag_ = policy;
    } else if (policy == range_bound_included || policy == range_bound_excluded) {
      // "...,value]" || "...,value)"
      upper_      = value;
      upper_flag_ = policy;
    } else {
      DT_THROW_IF(true, std::logic_error, "Invalid upper bound policy !");
    }
    if (!has_abs_precision()) {
      set_abs_precision(0.0);
    }
  }


  void real_range::set(double from, double to,
                       range_bound_info_type from_policy,
                       range_bound_info_type to_policy) {
    this->set_lower(from, from_policy);
    this->set_upper(to, to_policy);
    if (!has_abs_precision()) {
      set_abs_precision(compute_precision(lower_, upper_));
    }
    if (!has_abs_precision()) {
      set_abs_precision(0.0);
    }
  }

  void real_range::set_abs_precision(double abseps_)
  {
    if (abs_precision_ <= 0.0) {
      abs_precision_ = compute_precision(lower_, upper_);
    } else {
      abs_precision_ = abseps_;
    }
    return;
  }

  bool real_range::has_abs_precision() const
  {
    return datatools::is_valid(abs_precision_);
  }

  void real_range::reset_abs_precision()
  {
    datatools::invalidate(abs_precision_);
    return;
  }

  double real_range::get_abs_precision() const
  {
    return abs_precision_;
  }

  bool real_range::is_lower_bounded() const {
    return lower_flag_ > 0 ;
  }

  bool real_range::is_upper_bounded() const {
    return upper_flag_ > 0 ;
  }

  bool real_range::is_half_bounded() const {
    return
      (this->is_lower_bounded() && !this->is_upper_bounded()) ||
      (!this->is_lower_bounded() && this->is_upper_bounded());
  }


  bool real_range::is_bounded() const {
    return this->is_lower_bounded() && this->is_upper_bounded();
  }


  double real_range::get_lower() const {
    DT_THROW_IF (!this->is_lower_bounded(),std::logic_error,"No lower bound !");
    return lower_;
  }


  double real_range::get_upper() const {
    DT_THROW_IF (!this->is_upper_bounded(),std::logic_error,"No upper bound !");
    return upper_;
  }

  double real_range::width() const
  {
    DT_THROW_IF (!this->is_bounded(),std::logic_error,"Unbounded interval !");
    double w = upper_ - lower_;
    if (!is_lower_included()) {
      w -= abs_precision_;
    }
    if (!is_upper_included()) {
      w -= abs_precision_;
    }
    if (w < - 1e-7 * abs_precision_) {
      datatools::invalidate(w);
    } else if (w < abs_precision_ * std::numeric_limits<double>::epsilon()) {
      w = 0.0;
    }
    return w;
  }

  bool real_range::is_singleton() const
  {
    return is_bounded() && (width() == 0.0);
  }

  double real_range::singleton() const
  {
    DT_THROW_IF (!this->is_singleton(),std::logic_error,"Not a singleton !");
    return 0.5 * (first() + last());
  }

  bool real_range::is_empty() const {
    return ! datatools::is_valid(this->width());
  }

  bool real_range::is_lower_included() const {
    DT_THROW_IF (!this->is_lower_bounded(),std::logic_error,"No lower bound !");
    return (lower_flag_ == range_bound_included);
  }


  bool real_range::is_upper_included() const {
    DT_THROW_IF (!this->is_upper_bounded(),std::logic_error,"No upper bound !");
    return (upper_flag_ == range_bound_included);
  }


  void real_range::make_empty() {
    this->set(0, 0, range_bound_excluded, range_bound_excluded);
  }


  void real_range::make_full() {
    this->set(-std::numeric_limits<double>::max(),
              std::numeric_limits<double>::max(),
              range_bound_included,
              range_bound_included);
  }


  void real_range::make_full_positive() {
    this->set(0,
              std::numeric_limits<double>::max(),
              range_bound_included,
              range_bound_included);
  }


  void real_range::make_full_negative() {
    this->set(-std::numeric_limits<double>::max(),
              0,
              range_bound_included,
              range_bound_included);
  }


  void real_range::make_upper_unbounded(double from, bool inclusive) {
    this->set(from,
              +std::numeric_limits<double>::infinity(),
              inclusive ? range_bound_included : range_bound_excluded ,
              range_bound_unbounded);
  }


  void real_range::make_lower_unbounded(double to, bool inclusive) {
    this->set(-std::numeric_limits<double>::infinity(),
              to,
              range_bound_unbounded,
              inclusive ? range_bound_included : range_bound_excluded);
  }


  void real_range::make_bounded(double from, double to,
                                bool lower_included, bool upper_included) {
    this->set(from,
              to,
              lower_included ? range_bound_included : range_bound_excluded,
              upper_included ? range_bound_included : range_bound_excluded);
  }


  double real_range::compute_precision(double a_lower, double a_upper) {
    double eps;
    double l_eps = std::numeric_limits<double>::quiet_NaN();
    double u_eps = std::numeric_limits<double>::quiet_NaN();
    if (datatools::is_valid(a_lower)) {
      double scale = std::abs(a_lower);
      l_eps = default_precision() * scale;
    }

    if (datatools::is_valid(a_upper)) {
      double scale = std::abs(a_upper);
      u_eps = default_precision() * scale;
    }

    if (datatools::is_valid(l_eps) && datatools::is_valid(u_eps)) {
      eps = std::max(l_eps, u_eps);
    } else if (datatools::is_valid(l_eps)) {
      eps = l_eps;
    } else if (datatools::is_valid(u_eps)) {
      eps = u_eps;
    } else {
      eps = default_precision();
    }

    return eps;
  }

  bool real_range::has(const real_range & rr) const
  {
    if (!rr.is_lower_bounded()) {
      if (this->is_lower_bounded()) {
        return false;
      }
    } else {
      int first = rr.first();
      if (!this->has(first)) return false;
    }
    if (!rr.is_upper_bounded()) {
      if (this->is_upper_bounded()) {
        return false;
      }
    } else {
      int last = rr.last();
      if (!this->has(last)) return false;
    }
    return false;
  }


  bool real_range::has(double value) const {
    double eps = abs_precision_;

    if (this->is_lower_bounded()) {
      if (this->is_lower_included()) {
        if (value < (lower_ - eps)) return false;
      } else {
        if (value < (lower_ + eps)) return false;
      }
    }

    if (this->is_upper_bounded()) {
      if (this->is_upper_included()) {
        if (value > (upper_ + eps)) return false;
      } else {
        if (value > (upper_ - eps)) return false;
      }
    }
    return true;
  }


  double real_range::first() const {
    DT_THROW_IF (!this->is_lower_bounded(),
                 std::logic_error,
                 "Not lower bounded !");
    double the_first = lower_;
    if (!this->is_lower_included()) {
      the_first += abs_precision_;
    }
    return the_first;
  }

  double real_range::last() const {
    DT_THROW_IF (!this->is_upper_bounded(),
                 std::logic_error,
                 "Not upper bounded !");
    double the_last = upper_;
    if (!this->is_upper_included()) {
      the_last -= abs_precision_;
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

  void real_range::set_preferred_unit(const std::string & pus_)
  {
    double uv;
    std::string ul;
    DT_THROW_IF(!units::find_unit(pus_, uv, ul), std::logic_error,
                "Preferred unit '" << pus_ << "' is not supported!");
    if (has_unit_label()) {
      DT_THROW_IF(ul != unit_label_, std::logic_error,
                  "Preferred unit '"<< pus_ << "' is not compatible with '" << unit_label_ << "' !");
    } else {
      set_unit_label(ul);
    }
    preferred_unit_ = pus_;
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

  void real_range::set_unit_label(const std::string & ul_)
  {
    if (has_preferred_unit()) {
      double uv;
      std::string ul;
      DT_THROW_IF(units::find_unit(preferred_unit_, uv, ul), std::logic_error,
                  "Preferred unit '" << preferred_unit_ << "' is not supported!");
      DT_THROW_IF(ul != ul_, std::logic_error,
                  "Unit label '" << ul_ << "' is not compatible with preferred unit '"
                  << preferred_unit_ << "' is not supported!");
    } else {
      // std::string dus = unit::get_default_unit_symbol_from_label(ul_);
      // set_preferred_unit(dus);
    }
    unit_label_ = ul_;
    return;
  }

  const std::string & real_range::get_unit_label() const
  {
    return unit_label_;
  }

  void real_range::dump(std::ostream& a_out) const {
    a_out << "real_range: " << std::endl;
    a_out << "|-- " << "Unit label     = '" << this->unit_label_ << "'" << std::endl;
    a_out << "|-- " << "Preferred unit = '" << this->preferred_unit_ << "'" << std::endl;
    if (this->is_valid()) {
      a_out << "|-- " << "Half bounded   = " << this->is_half_bounded() << std::endl;

      a_out << "|-- " << "Lower bounded  = " << this->is_lower_bounded() << std::endl;
      if (this->is_lower_bounded()) {
        a_out << "|   " << "|-- " << "Lower bound    = " << lower_ << std::endl;
        a_out << "|   " << "|-- " << "Lower included = " << this->is_lower_included() << std::endl;
        a_out << "|   " << "`-- " << "First          = " << this->first() << std::endl;
      }
      a_out << "|-- " << "Upper bounded  = " << this->is_upper_bounded() << std::endl;
      if (this->is_upper_bounded()) {
        a_out << "|   " << "|-- " << "Upper bound    = " << upper_ << std::endl;
        a_out << "|   " << "|-- " << "Upper included = " << this->is_upper_included() << std::endl;
        a_out << "|   " << "`-- " << "Last           = " << this->last() << std::endl;
      }
      a_out << "|-- " << "Bounded        = " << this->is_bounded() << std::endl;
      if (this->is_bounded()) {
        a_out << "|   " << "|-- " << "Width          = " << this->width() << std::endl;
        a_out << "|   " << "|-- " << "Empty          = " << this->is_empty() << std::endl;
        a_out << "|   " << "`-- " << "Singleton      = " << this->is_singleton() << std::endl;
      }
    } else {
      a_out << "|   " << "|-- " << "[invalid]" << std::endl;
    }
    a_out << "`-- " << "Rendering      = '" << *this << "'" << std::endl;
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
      a_out << range_tools::token_separator;
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
  std::istream& operator>>(std::istream& in, real_range& a_range)
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
    in.get(open);
    if (!in) {
      return in;
    }
    // Parse the open character : '[' or '('
    if (open == range_tools::token_open_included) {
      lf = range_bound_included;
     } else if (open == range_tools::token_open_excluded) {
      lf = range_bound_excluded;
    } else {
      std::cerr << "DEVEL: " << "bad open!" << std::endl;
      in.setstate(std::ios::failbit);
      return in;
    }
    std::cerr << "DEVEL: " << "open='" << open << "'" << std::endl;
    std::cerr << "DEVEL: " << "lf='" << lf << "'" << std::endl;
    std::string word;
    while(!in.eof()) {
      char dummy = 0;
      in.get(dummy);
      if (!in) {
        return in;
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
        std::cerr << "DEVEL: " << "close='" << dummy << "'" << std::endl;
        std::cerr << "DEVEL: " << "uf='" << uf << "'" << std::endl;
        break;
      } else {
        word.push_back(dummy);
      }
    }
    std::cerr << "DEVEL: " << "word='" << word << "'" << std::endl;
    if (word.length() == 1 &&  word[0] == range_tools::token_invalid) {
      if (lf == range_bound_excluded && uf == range_bound_excluded) {
        return in;
      } else {
        in.setstate(std::ios::failbit);
        return in;
      }
    }
    std::vector<std::string> tokens;
    static std::string separators;
    if (separators.empty()) {
      separators += range_tools::token_separator;
    }
    boost::split(tokens, word, boost::is_any_of(separators));
    if (tokens.size() != 2) {
      in.setstate(std::ios::failbit);
      return in;
    }
    std::string ls = tokens[0];
    std::string us = tokens[1];
    boost::trim(ls);
    boost::trim(us);
    std::cerr << "DEVEL: " << "ls='" << ls << "'" << std::endl;
    std::cerr << "DEVEL: " << "us='" << us << "'" << std::endl;
    if (ls.empty()) {
      if (lf == range_bound_excluded) {
        lf = range_bound_unbounded;
      } else {
        in.setstate(std::ios::failbit);
        return in;
      }
    } else {
      bool l_ok = units::parse_value_with_unit(ls, l, l_unit_symbol, l_unit_label);
      if (! l_ok) {
        // Cannot parse real value from ls
        in.setstate(std::ios::failbit);
        return in;
      }
      std::cerr << "DEVEL: " << "l='" << l << "'" << std::endl;
    }
    if (us.empty()) {
      if (uf == range_bound_excluded) {
        uf = range_bound_unbounded;
      } else {
        in.setstate(std::ios::failbit);
        return in;
      }
    } else {
      bool u_ok = units::parse_value_with_unit(us, u, u_unit_symbol, u_unit_label);
      if (! u_ok) {
        // Cannot parse real value from us
        in.setstate(std::ios::failbit);
        return in;
      }
      std::cerr << "DEVEL: " << "u='" << u << "'" << std::endl;
    }
    if (l_unit_label.empty() && !u_unit_label.empty()) {
      // Lower bound does not have explicit unit !
      in.setstate(std::ios::badbit);
      return in;
    }
    if (!l_unit_label.empty() && u_unit_label.empty()) {
      // Upper bound does not have explicit unit !
      in.setstate(std::ios::badbit);
      return in;
    }
    if (!l_unit_label.empty() && !u_unit_label.empty() && (l_unit_label != u_unit_label)) {
      // Bounds do not have the same unit label !
      in.setstate(std::ios::badbit);
      return in;
    }
    if ((lf != range_bound_unbounded) && (uf != range_bound_unbounded)) {
      if (l > u) {
        in.setstate(std::ios::badbit);
        return in;
      }
    }
    a_range.set(l, u, lf, uf);
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
    return in;
  }

} // namespace datatools
