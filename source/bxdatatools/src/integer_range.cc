// integer_range.cc

// Ourselves:
#include <datatools/integer_range.h>

// Standard Library:
#include <sstream>
#include <stdexcept>
#include <limits>
#include <vector>

// Third party:
// - Boost
#include <boost/algorithm/string.hpp>

// This Project:
#include <datatools/exception.h>

namespace datatools {

  integer_range::integer_range() {
    this->reset();
  }

  integer_range::integer_range(value_type from, value_type to,
                               range_bound_info_type from_policy,
                               range_bound_info_type to_policy) {
    this->set(from, to, from_policy, to_policy);
  }


  bool integer_range::is_valid() const {
    return (lower_flag_ != range_bound_undefined)
      && (upper_flag_ != range_bound_undefined);
  }


  void integer_range::invalidate () {
    this->reset();
  }


  void integer_range::reset_lower() {
    lower_      = std::numeric_limits<value_type>::min();
    lower_flag_ = range_bound_undefined;
  }


  void integer_range::reset_upper() {
    upper_      = std::numeric_limits<value_type>::max();
    upper_flag_ = range_bound_undefined;
  }


  void integer_range::reset() {
    this->reset_lower();
    this->reset_upper();
  }


  void integer_range::set_lower(value_type from, range_bound_info_type policy) {
    if (policy == range_bound_unbounded) {
      // "];...."
      lower_      = std::numeric_limits<value_type>::min();
      lower_flag_ = policy;
    } else if (policy == range_bound_included || policy == range_bound_excluded) {
      // "[value;...." || "]value;..."
      lower_      = from;
      lower_flag_ = policy;
    } else {
      DT_THROW_IF(true, std::logic_error, "Invalid policy !");
    }
  }


  void integer_range::set_upper(value_type to, range_bound_info_type policy) {
    if (policy == range_bound_unbounded) {
      // "...,)"
      upper_      = std::numeric_limits<value_type>::max();
      upper_flag_ = policy;
    } else if (policy == range_bound_included || policy == range_bound_excluded) {
      // "...,value]" ||  "...,value)"
      if (to == std::numeric_limits<value_type>::max()) {
        DT_THROW_IF(policy == range_bound_included, std::logic_error,
                    "Cannot set included upper value to MAX INT!");
      }
      upper_      = to;
      upper_flag_ = policy;
    } else {
      DT_THROW_IF(true, std::logic_error, "Invalid policy !");
    }
  }


  void integer_range::set(value_type from, value_type to,
                          range_bound_info_type from_policy,
                          range_bound_info_type to_policy) {
    this->set_lower(from, from_policy);
    this->set_upper(to, to_policy);
  }


  bool integer_range::is_lower_bounded() const {
    return (lower_flag_ == range_bound_included) || (lower_flag_ == range_bound_excluded);
  }


  bool integer_range::is_upper_bounded() const {
    return (upper_flag_ == range_bound_included) || (upper_flag_ == range_bound_excluded);
  }


  bool integer_range::is_half_bounded() const {
    return
      (this->is_lower_bounded() && !this->is_upper_bounded()) ||
      (!this->is_lower_bounded() && this->is_upper_bounded());
  }

  bool integer_range::is_bounded() const {
    return this->is_lower_bounded() && this->is_upper_bounded();
  }

  bool integer_range::is_singleton() const
  {
    return is_bounded() && (first() == last());
  }

  integer_range::value_type integer_range::get_lower_bound() const {
    DT_THROW_IF (!this->is_lower_bounded(),
                 std::logic_error,
                 "No lower bound !");
    return lower_;
  }


  integer_range::value_type integer_range::get_upper_bound() const {
    DT_THROW_IF (!this->is_upper_bounded(),
                 std::logic_error,
                 "No upper bound !");
    return upper_;
  }


  bool integer_range::is_lower_included() const {
    DT_THROW_IF (!this->is_lower_bounded(),
                 std::logic_error,
                 "No lower bound !");
    return (lower_flag_ == range_bound_included);
  }


  bool integer_range::is_upper_included() const {
    DT_THROW_IF (!this->is_upper_bounded(),
                 std::logic_error,
                 "No upper bound !");
    return (upper_flag_ == range_bound_included);
  }


  uint64_t integer_range::count() const {
    DT_THROW_IF (!is_bounded(),
                 std::logic_error,
                 "(Semi-)infinite range !");
    int64_t cnt64 = upper_;
    cnt64 -= lower_;
    cnt64 += 1;
    if (!this->is_upper_included()) cnt64--;
    if (!this->is_lower_included()) cnt64--;
    uint64_t ucnt64 = cnt64;
    return ucnt64;
  }


  bool integer_range::is_empty() const {
    return this->count() == 0;
  }


  void integer_range::make_empty() {
    this->set(0, 0, range_bound_excluded, range_bound_excluded);
  }


  void integer_range::make_full() {
    this->set(std::numeric_limits<value_type>::min(),
              std::numeric_limits<value_type>::max() - 1,
              range_bound_included,
              range_bound_included);
  }


  void integer_range::make_full_positive() {
    this->set(0,
              std::numeric_limits<value_type>::max() - 1,
              range_bound_included,
              range_bound_included);
  }


  void integer_range::make_full_negative() {
    this->set(std::numeric_limits<value_type>::min(),
              0,
              range_bound_included,
              range_bound_included);
  }


  void integer_range::make_upper_unbounded(value_type from, bool inclusive) {
    this->set(from,
              0,
              inclusive ? range_bound_included : range_bound_excluded ,
              range_bound_unbounded);
  }


  void integer_range::make_lower_unbounded(value_type to, bool inclusive) {
    this->set(0,
              to,
              range_bound_unbounded,
              inclusive ? range_bound_included : range_bound_excluded);
  }


  void integer_range::make_bounded(value_type from, value_type to,
                                   bool from_included,
                                   bool to_included) {
    this->set(from,
              to,
              from_included ? range_bound_included : range_bound_excluded,
              to_included ? range_bound_included : range_bound_excluded);
  }


  bool integer_range::has(const integer_range & ir) const
  {
    if (!ir.is_lower_bounded()) {
      if (this->is_lower_bounded()) {
        return false;
      }
    } else {
      int first = ir.first();
      if (!this->has(first)) return false;
    }
    if (!ir.is_upper_bounded()) {
      if (this->is_upper_bounded()) {
        return false;
      }
    } else {
      int last = ir.last();
      if (!this->has(last)) return false;
    }
    return true;
  }

  bool integer_range::has(value_type value) const {
    if (this->is_lower_bounded()) {
      if (this->is_lower_included()) {
        if (value < lower_) return false;
      } else {
        if (value <= lower_) return false;
      }
    }

    if (this->is_upper_bounded()) {
      if (this->is_upper_included()) {
        if (value > upper_) return false;
      } else {
        if (value >= upper_) return false;
      }
    }
    return true;
  }


  void integer_range::dump(std::ostream& out) const {
    out << "integer_range: " << std::endl;
    if (this->is_valid()) {
      out << "|-- " << "Half bounded   = " << this->is_half_bounded() << std::endl;

      out << "|-- " << "Lower bounded  = " << this->is_lower_bounded() << std::endl;
      if (this->is_lower_bounded()) {
        out << "|   " << "|-- " << "Lower bound    = " << lower_ << std::endl;
        out << "|   " << "|-- " << "Lower included = " << this->is_lower_included() << std::endl;
        out << "|   " << "`-- " << "First          = " << this->first() << std::endl;
      }

      out << "|-- " << "Upper bounded  = " << this->is_upper_bounded() << std::endl;
      if (this->is_upper_bounded()) {
        out << "|   " << "|-- " << "Upper bound    = " << upper_ << std::endl;
        out << "|   " << "|-- " << "Upper included = " << this->is_upper_included() << std::endl;
        out << "|   " << "`-- " << "Last           = " << this->last() << std::endl;
      }

      out << "|-- " << "Bounded        = " << this->is_bounded() << std::endl;
      if (this->is_bounded()) {
        out << "|   " << "|-- " << "Count          = " << this->count() << std::endl;
        out << "|   " << "|-- " << "Empty          = " << this->is_empty() << std::endl;
        out << "|   " << "|-- " << "Singleton      = " << this->is_singleton() << std::endl;
        out << "|   " << "|-- " << "Begin          = " << this->begin() << std::endl;
        out << "|   " << "`-- " << "End            = " << this->end() << std::endl;
      }

    } else {
      out << "|-- " << "[invalid]" << std::endl;
    }
    out << "`-- " << "Rendering      = '" << *this << "'" << std::endl;
  }


  integer_range::value_type integer_range::begin() const {
    DT_THROW_IF (!this->is_bounded(),
                 std::logic_error,
                 "Not bounded !");
    value_type the_beg = lower_;
    if (!this->is_lower_included()) {
      the_beg++;
    }
    return the_beg;
  }


  integer_range::value_type integer_range::end() const {
    DT_THROW_IF (!this->is_bounded(),
                 std::logic_error,
                 "Not bounded !");
    value_type the_end = upper_;
    // Critical case : the_end != numeric_limits<value_type>::max () ???
    if (this->is_upper_included()) {
      the_end++;
    }
    return the_end;
  }


  integer_range::value_type integer_range::first() const {
    DT_THROW_IF (!this->is_lower_bounded(),
                 std::logic_error,
                 "Not lower bounded !");
    value_type the_first = lower_;
    if (!this->is_lower_included()) {
      the_first++;
    }
    return the_first;
  }


  integer_range::value_type integer_range::last() const {
    DT_THROW_IF (!this->is_upper_bounded(),
                 std::logic_error,
                 "Not upper bounded !");
    value_type the_last = upper_;
    if (!this->is_upper_included()) {
      the_last--;
    }
    return the_last;
  }

  std::istream& operator>>(std::istream& in, integer_range& a_range) {
    range_bound_info_type     lf = range_bound_undefined;
    integer_range::value_type l;
    range_bound_info_type     uf = range_bound_undefined;
    integer_range::value_type u;
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
      // std::cerr << "DEVEL: " << "bad open!" << std::endl;
      in.setstate(std::ios::failbit);
      return in;
    }
    // std::cerr << "DEVEL: " << "open='" << open << "'" << std::endl;
    // std::cerr << "DEVEL: " << "lf='" << lf << "'" << std::endl;
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
    // std::cerr << "DEVEL: " << "ls='" << ls << "'" << std::endl;
    // std::cerr << "DEVEL: " << "us='" << us << "'" << std::endl;
    if (ls.empty()) {
      if (lf == range_bound_excluded) {
        lf = range_bound_unbounded;
      } else {
        in.setstate(std::ios::failbit);
        return in;
      }
    } else {
      std::istringstream ls_iss(ls);
      ls_iss >> l;
      if (!ls_iss) {
        in.setstate(std::ios::failbit);
        return in;
      }
      // std::cerr << "DEVEL: " << "l='" << l << "'" << std::endl;
    }
    if (us.empty()) {
      if (uf == range_bound_excluded) {
        uf = range_bound_unbounded;
      } else {
        in.setstate(std::ios::failbit);
        return in;
      }
    } else {
      std::istringstream us_iss(us);
      us_iss >> u;
      if (!us_iss) {
        in.setstate(std::ios::failbit);
        return in;
      }
      // std::cerr << "DEVEL: " << "u='" << u << "'" << std::endl;
    }
    if ((lf != range_bound_unbounded) && (uf != range_bound_unbounded)) {
      if (l > u) {
        in.setstate(std::ios::badbit);
        return in;
      }
    }
    a_range.set(l, u, lf, uf);
    return in;
  }

  std::ostream& operator<<(std::ostream& out, const integer_range& a_range) {
    if (!a_range.is_valid()) {
      out << range_tools::token_open_excluded << range_tools::token_invalid << range_tools::token_close_excluded;
    } else {
      if (a_range.is_lower_bounded()) {
        if (a_range.is_lower_included()) {
          out << range_tools::token_open_included;
        } else {
          out << range_tools::token_open_excluded;
        }
        out << a_range.get_lower_bound();
      } else {
        out << range_tools::token_open_excluded;
      }
      out << range_tools::token_separator;
      if (a_range.is_upper_bounded()) {
        out << a_range.get_upper_bound();
        if (a_range.is_upper_included()) {
          out << range_tools::token_close_included;
        } else {
          out << range_tools::token_close_excluded;
        }
      } else {
        out << range_tools::token_close_excluded;
      }
    }
    return out;
  }


  int integer_range::compare(const integer_range& a_range) const
  {
    DT_THROW_IF(!is_valid(), std::logic_error, "Invalid integer range!");
    DT_THROW_IF(!a_range.is_valid(), std::logic_error, "Invalid integer range!");
    if (is_lower_bounded()) {
      // #1 ---[f1 ;  ? ]------------>
      int f1 = first();
      if (!a_range.is_lower_bounded()) {
        // #1 ---[f1 ; ? ]------------>
        // #2 ]   ; ? ]--------------->
        return +1;
      }  else {
        // #2 ---[f2 ; ? ]------------>
        int f2 = a_range.first();
        if (f1 < f2) {
          // #1 ---[f1 ; ? ]------------>
          // #2 -----[f2 ; ? ]---------->
          return -1;
        } else if (f1 > f2) {
          // #1 -----[f1 ; ? ]---------->
          // #2 ---[f2 ; ? ]------------>
          return +1;
        } else {
          // f1==f2
          // #1 ---[f1 ; ? ]------------>
          // #2 ---[f2 ; ? ]------------>
          if (is_upper_bounded()) {
            // #1 ---[f1 ; l1]------------>
            int l1 = last();
            if (! a_range.is_upper_bounded()) {
              // #1 ---[f1 ; l1]------------>
              // #2 ---[f2 ;               [>
              return -1;
            } else {
              // #2 ---[f2 ; l2]------------>
              int l2 = a_range.last();
              if (l1 < l2) {
                // #1 ---[f1 ; l1]------------>
                // #2 ---[f2 ;   l2]---------->
                return -1;
              } else if (l1 > l2) {
                // #1 ---[f1 ;   l1]---------->
                // #2 ---[f2 ; l2]------------>
                return +1;
              } else {
                // #1 ---[f1 ; l1]------------>
                // #2 ---[f2 ; l2]------------>
                return 0;
              }
            }
          } else {
            // #1 ---[f1 ;                   ]>
            if (! a_range.is_upper_bounded()) {
              // #1 ---[f1 ;                   ]>
              // #2 ---[f2 ;                   ]>
              return 0;
            } else {
              int l2 = a_range.last();
              // #1 ---[f1 ;                   ]>
              // #2 ---[f2 ; l2]---------------]>
              return +1;
            }
          }
        }
      }
    } else {
      // #1 ]   ; ? ]--------------->
      if (a_range.is_lower_bounded()) {
        // #1 ]   ; ? ]--------------->
        // #2 ---[f2 ; ? ]------------>
        return -1;
      } else {
        // #1 ]   ; ? ]--------------->
        // #2 ]   ; ? ]--------------->
        if (is_upper_bounded()) {
          // #1 ]   ; l1]--------------->
          int l1 = last();
          if (! a_range.is_upper_bounded()) {
            // #1 ]   ; l1]--------------->
            // #2 ]   ;                  [>
            return -1;
          } else {
            int l2 = a_range.last();
            // #2 ]   ; l2]--------------->
            if (l1 < l2) {
              // #1 ]   ; l1]--------------->
              // #2 ]   ;   l2]------------->
              return -1;
            } else if (l1 > l2) {
              // #1 ]   ;   l1]------------->
              // #2 ]   ; l2]--------------->
              return +1;
            } else {
              // #1 ]   ; l1]--------------->
              // #2 ]   ; l2]--------------->
              return 0;
            }
          }
        } else {
         // #1 ]         ;            [>
         // #2 ]   ; ? ]--------------->
         if (a_range.is_upper_bounded()) {
           // #1 ]         ;            [>
           // #2 ]   ; l2 ]-------------->
           return +1;
         } else {
           // #1 ]         ;            [>
           // #2 ]         ;            [>
           return 0;
         }
        }
      }
    }
  }

  bool integer_range::operator<(const integer_range& a_range) const
  {
    return compare(a_range) == -1;
  }

  bool integer_range::operator>(const integer_range& a_range) const
  {
    return compare(a_range) == +1;
  }

  bool integer_range::operator==(const integer_range& a_range) const
  {
    return compare(a_range) == 0;
  }

  void integer_range::make_canonical(integer_range& a_range) const
  {
    a_range = *this;
    if (lower_flag_ == range_bound_excluded) {
      a_range.lower_++;
      a_range.lower_flag_ = range_bound_included;
    }
    if (upper_flag_ == range_bound_excluded) {
      a_range.upper_--;
      a_range.upper_flag_ = range_bound_included;
    }
    return;
  }

  bool integer_range::intersect(const integer_range& a_range) const
  {
    integer_range dummy;
    return intersect(a_range, dummy);
  }

  bool integer_range::intersect(const integer_range& a_range, integer_range& a_inter) const
  {
    DT_THROW_IF(!is_valid(), std::logic_error, "Invalid integer range!");
    DT_THROW_IF(!a_range.is_valid(), std::logic_error, "Invalid integer range!");
    a_inter.reset();

    integer_range i1;
    this->make_canonical(i1);
    integer_range i2;
    a_range.make_canonical(i2);

    range_bound_info_type bf = range_bound_undefined;
    int f;
    if (i1.is_lower_bounded() && i2.is_lower_bounded()) {
      bf = range_bound_included;
      f = std::max(i1.first(), i2.first());
    }
    if (i1.is_lower_bounded() && ! i2.is_lower_bounded()) {
      bf = range_bound_included;
      f = i1.first();
    }
    if (! i1.is_lower_bounded() && i2.is_lower_bounded()) {
      bf = range_bound_included;
      f = i2.first();
    }
    if (! i1.is_lower_bounded() && ! i2.is_lower_bounded()) {
      bf = range_bound_unbounded;
    }

    range_bound_info_type bl = range_bound_undefined;
    int l;
    if (i1.is_upper_bounded() && i2.is_upper_bounded()) {
      bl = range_bound_included;
      l = std::min(i1.last(), i2.last());
    }
    if (i1.is_upper_bounded() && ! i2.is_upper_bounded()) {
      bl = range_bound_included;
      l = i1.last();
    }
    if (! i1.is_upper_bounded() && i2.is_upper_bounded()) {
      bl = range_bound_included;
      l = i2.last();
    }
    if (! i1.is_upper_bounded() && ! i2.is_upper_bounded()) {
      bl = range_bound_unbounded;
    }

    if (bf == range_bound_included && bl == range_bound_included) {
      if (f <= l) {
        a_inter.set(f, l, bf, bl);
        return true;
      } else {
        return false;
      }
    } else {
      a_inter.set(f, l, bf, bl);
      return true;
    }
    return false;
  }

} // namespace datatools
