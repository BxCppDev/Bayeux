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

  integer_range::integer_range()
  {
    this->reset();
    return;
  }

  integer_range::integer_range(value_type from_, value_type to_,
                               range_bound_info_type from_policy_,
                               range_bound_info_type to_policy_)
  {
    this->set(from_, to_, from_policy_, to_policy_);
    return;
  }

  bool integer_range::is_valid() const
  {
    return (_lower_flag_ != range_bound_undefined)
      && (_upper_flag_ != range_bound_undefined);
  }

  void integer_range::invalidate()
  {
    this->reset();
    return;
  }

  void integer_range::reset_lower()
  {
    _lower_      = std::numeric_limits<value_type>::min();
    _lower_flag_ = range_bound_undefined;
    return;
  }

  void integer_range::reset_upper()
  {
    _upper_      = std::numeric_limits<value_type>::max();
    _upper_flag_ = range_bound_undefined;
    return;
  }

  void integer_range::reset()
  {
    this->reset_lower();
    this->reset_upper();
    return;
  }

  void integer_range::set_lower(value_type from_, range_bound_info_type policy_)
  {
    if (policy_ == range_bound_unbounded) {
      // "];...."
      _lower_      = std::numeric_limits<value_type>::min();
      _lower_flag_ = policy_;
    } else if (policy_ == range_bound_included || policy_ == range_bound_excluded) {
      // "[value;...." || "]value;..."
      _lower_      = from_;
      _lower_flag_ = policy_;
    } else {
      DT_THROW(std::logic_error, "Invalid policy !");
    }
    return;
  }

  void integer_range::set_upper(value_type to_, range_bound_info_type policy_)
  {
    if (policy_ == range_bound_unbounded) {
      // "...,)"
      _upper_      = std::numeric_limits<value_type>::max();
      _upper_flag_ = policy_;
    } else if (policy_ == range_bound_included || policy_ == range_bound_excluded) {
      // "...,value]" ||  "...,value)"
      if (to_ == std::numeric_limits<value_type>::max()) {
        DT_THROW_IF(policy_ == range_bound_included, std::logic_error,
                    "Cannot set included upper value to MAX INT!");
      }
      _upper_      = to_;
      _upper_flag_ = policy_;
    } else {
      DT_THROW(std::logic_error, "Invalid policy !");
    }
    return;
  }

  void integer_range::set(value_type from_, value_type to_,
                          range_bound_info_type from_policy_,
                          range_bound_info_type to_policy_)
  {
    this->set_lower(from_, from_policy_);
    this->set_upper(to_, to_policy_);
    return;
  }

  bool integer_range::is_lower_bounded() const
  {
    return (_lower_flag_ == range_bound_included) || (_lower_flag_ == range_bound_excluded);
  }

  bool integer_range::is_upper_bounded() const
  {
    return (_upper_flag_ == range_bound_included) || (_upper_flag_ == range_bound_excluded);
  }

  bool integer_range::is_half_bounded() const
  {
    return
      (this->is_lower_bounded() && !this->is_upper_bounded()) ||
      (!this->is_lower_bounded() && this->is_upper_bounded());
  }

  bool integer_range::is_bounded() const
  {
    return this->is_lower_bounded() && this->is_upper_bounded();
  }

  bool integer_range::is_singleton() const
  {
    return is_bounded() && (first() == last());
  }

  integer_range::value_type integer_range::get_lower_bound() const
  {
    DT_THROW_IF (!this->is_lower_bounded(),
                 std::logic_error,
                 "No lower bound !");
    return _lower_;
  }

  integer_range::value_type integer_range::get_upper_bound() const
  {
    DT_THROW_IF (!this->is_upper_bounded(),
                 std::logic_error,
                 "No upper bound !");
    return _upper_;
  }

  bool integer_range::is_lower_included() const
  {
    DT_THROW_IF (!this->is_lower_bounded(),
                 std::logic_error,
                 "No lower bound !");
    return (_lower_flag_ == range_bound_included);
  }

  bool integer_range::is_upper_included() const
  {
    DT_THROW_IF (!this->is_upper_bounded(),
                 std::logic_error,
                 "No upper bound !");
    return (_upper_flag_ == range_bound_included);
  }

  uint64_t integer_range::count() const
  {
    DT_THROW_IF (!is_bounded(),
                 std::logic_error,
                 "(Semi-)infinite range !");
    int64_t cnt64 = _upper_;
    cnt64 -= _lower_;
    cnt64 += 1;
    if (!this->is_upper_included()) cnt64--;
    if (!this->is_lower_included()) cnt64--;
    uint64_t ucnt64 = cnt64;
    return ucnt64;
  }

  bool integer_range::is_empty() const
  {
    return this->count() == 0;
  }

  void integer_range::make_empty()
  {
    this->set(0, 0, range_bound_excluded, range_bound_excluded);
    return;
  }

  void integer_range::make_full()
  {
    this->set(std::numeric_limits<value_type>::min(),
              std::numeric_limits<value_type>::max() - 1,
              range_bound_included,
              range_bound_included);
    return;
  }

  void integer_range::make_full_positive()
  {
    this->set(0,
              std::numeric_limits<value_type>::max() - 1,
              range_bound_included,
              range_bound_included);
    return;
  }

  void integer_range::make_full_negative()
  {
    this->set(std::numeric_limits<value_type>::min(),
              0,
              range_bound_included,
              range_bound_included);
    return;
  }


  void integer_range::make_upper_unbounded(value_type from_, bool inclusive_)
  {
    this->set(from_,
              0,
              inclusive_ ? range_bound_included : range_bound_excluded ,
              range_bound_unbounded);
    return;
  }


  void integer_range::make_lower_unbounded(value_type to_, bool inclusive_)
  {
    this->set(0,
              to_,
              range_bound_unbounded,
              inclusive_ ? range_bound_included : range_bound_excluded);
    return;
  }

  void integer_range::make_bounded(value_type from_, value_type to_,
                                   bool from_included_,
                                   bool to_included_)
  {
    this->set(from_,
              to_,
              from_included_ ? range_bound_included : range_bound_excluded,
              to_included_ ? range_bound_included : range_bound_excluded);
    return;
  }

  bool integer_range::has(const integer_range & ir_) const
  {
    if (!ir_.is_lower_bounded()) {
      if (this->is_lower_bounded()) {
        return false;
      }
    } else {
      int local_first = ir_.first();
      if (!this->has(local_first)) return false;
    }
    if (!ir_.is_upper_bounded()) {
      if (this->is_upper_bounded()) {
        return false;
      }
    } else {
      int local_last = ir_.last();
      if (!this->has(local_last)) return false;
    }
    return true;
  }

  bool integer_range::has(value_type value) const {
    if (this->is_lower_bounded()) {
      if (this->is_lower_included()) {
        if (value < _lower_) return false;
      } else {
        if (value <= _lower_) return false;
      }
    }

    if (this->is_upper_bounded()) {
      if (this->is_upper_included()) {
        if (value > _upper_) return false;
      } else {
        if (value >= _upper_) return false;
      }
    }
    return true;
  }


  void integer_range::dump(std::ostream & out_) const
  {
    out_ << "integer_range: " << std::endl;
    if (this->is_valid()) {
      out_ << "|-- " << "Half bounded   = " << this->is_half_bounded() << std::endl;

      out_ << "|-- " << "Lower bounded  = " << this->is_lower_bounded() << std::endl;
      if (this->is_lower_bounded()) {
        out_ << "|   " << "|-- " << "Lower bound    = " << _lower_ << std::endl;
        out_ << "|   " << "|-- " << "Lower included = " << this->is_lower_included() << std::endl;
        out_ << "|   " << "`-- " << "First          = " << this->first() << std::endl;
      }

      out_ << "|-- " << "Upper bounded  = " << this->is_upper_bounded() << std::endl;
      if (this->is_upper_bounded()) {
        out_ << "|   " << "|-- " << "Upper bound    = " << _upper_ << std::endl;
        out_ << "|   " << "|-- " << "Upper included = " << this->is_upper_included() << std::endl;
        out_ << "|   " << "`-- " << "Last           = " << this->last() << std::endl;
      }

      out_ << "|-- " << "Bounded        = " << this->is_bounded() << std::endl;
      if (this->is_bounded()) {
        out_ << "|   " << "|-- " << "Count          = " << this->count() << std::endl;
        out_ << "|   " << "|-- " << "Empty          = " << this->is_empty() << std::endl;
        out_ << "|   " << "|-- " << "Singleton      = " << this->is_singleton() << std::endl;
        out_ << "|   " << "|-- " << "Begin          = " << this->begin() << std::endl;
        out_ << "|   " << "`-- " << "End            = " << this->end() << std::endl;
      }

    } else {
      out_ << "|-- " << "[invalid]" << std::endl;
    }
    out_ << "`-- " << "Rendering      = '" << *this << "'" << std::endl;
    return;
  }

  integer_range::value_type integer_range::begin() const
  {
    DT_THROW_IF (!this->is_bounded(),
                 std::logic_error,
                 "Not bounded !");
    value_type the_beg = _lower_;
    if (!this->is_lower_included()) {
      the_beg++;
    }
    return the_beg;
  }

  integer_range::value_type integer_range::end() const
  {
    DT_THROW_IF (!this->is_bounded(),
                 std::logic_error,
                 "Not bounded !");
    value_type the_end = _upper_;
    // Critical case : the_end != numeric_limits<value_type>::max () ???
    if (this->is_upper_included()) {
      the_end++;
    }
    return the_end;
  }

  integer_range::value_type integer_range::first() const
  {
    DT_THROW_IF (!this->is_lower_bounded(),
                 std::logic_error,
                 "Not lower bounded !");
    value_type the_first = _lower_;
    if (!this->is_lower_included()) {
      the_first++;
    }
    return the_first;
  }

  integer_range::value_type integer_range::last() const
  {
    DT_THROW_IF (!this->is_upper_bounded(),
                 std::logic_error,
                 "Not upper bounded !");
    value_type the_last = _upper_;
    if (!this->is_upper_included()) {
      the_last--;
    }
    return the_last;
  }

  std::istream & operator>>(std::istream & in_, integer_range & range_)
  {
    range_bound_info_type     lf = range_bound_undefined;
    integer_range::value_type l;
    range_bound_info_type     uf = range_bound_undefined;
    integer_range::value_type u;
    range_.invalidate();
    char open = 0;
    in_.get(open);
    if (!in_) {
      return in_;
    }
    // Parse the open character : '[' or '('
    if (open == range_tools::token_open_included) {
      lf = range_bound_included;
    } else if (open == range_tools::token_open_excluded) {
      lf = range_bound_excluded;
    } else {
      // std::cerr << "DEVEL: " << "bad open!" << std::endl;
      in_.setstate(std::ios::failbit);
      return in_;
    }
    // std::cerr << "DEVEL: " << "open='" << open << "'" << std::endl;
    // std::cerr << "DEVEL: " << "lf='" << lf << "'" << std::endl;
    std::string word;
    while(!in_.eof()) {
      char dummy = 0;
      in_.get(dummy);
      if (!in_) {
        return in_;
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
        return in_;
      } else {
        in_.setstate(std::ios::failbit);
        return in_;
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
      in_.setstate(std::ios::failbit);
      return in_;
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
        in_.setstate(std::ios::failbit);
        return in_;
      }
    } else {
      std::istringstream ls_iss(ls);
      ls_iss >> l;
      if (!ls_iss) {
        in_.setstate(std::ios::failbit);
        return in_;
      }
      // std::cerr << "DEVEL: " << "l='" << l << "'" << std::endl;
    }
    if (us.empty()) {
      if (uf == range_bound_excluded) {
        uf = range_bound_unbounded;
      } else {
        in_.setstate(std::ios::failbit);
        return in_;
      }
    } else {
      std::istringstream us_iss(us);
      us_iss >> u;
      if (!us_iss) {
        in_.setstate(std::ios::failbit);
        return in_;
      }
      // std::cerr << "DEVEL: " << "u='" << u << "'" << std::endl;
    }
    if ((lf != range_bound_unbounded) && (uf != range_bound_unbounded)) {
      if (l > u) {
        in_.setstate(std::ios::badbit);
        return in_;
      }
    }
    range_.set(l, u, lf, uf);
    return in_;
  }

  std::ostream& operator<<(std::ostream & out_, const integer_range & range_)
  {
    if (!range_.is_valid()) {
      out_ << range_tools::token_open_excluded << range_tools::token_invalid << range_tools::token_close_excluded;
    } else {
      if (range_.is_lower_bounded()) {
        if (range_.is_lower_included()) {
          out_ << range_tools::token_open_included;
        } else {
          out_ << range_tools::token_open_excluded;
        }
        out_ << range_.get_lower_bound();
      } else {
        out_ << range_tools::token_open_excluded;
      }
      out_ << range_tools::token_separator2;
      if (range_.is_upper_bounded()) {
        out_ << range_.get_upper_bound();
        if (range_.is_upper_included()) {
          out_ << range_tools::token_close_included;
        } else {
          out_ << range_tools::token_close_excluded;
        }
      } else {
        out_ << range_tools::token_close_excluded;
      }
    }
    return out_;
  }

  int integer_range::compare(const integer_range & range_) const
  {
    DT_THROW_IF(!is_valid(), std::logic_error, "Invalid integer range!");
    DT_THROW_IF(!range_.is_valid(), std::logic_error, "Invalid integer range!");
    if (is_lower_bounded()) {
      // #1 ---[f1 ;  ? ]------------>
      int f1 = first();
      if (!range_.is_lower_bounded()) {
        // #1 ---[f1 ; ? ]------------>
        // #2 ]   ; ? ]--------------->
        return +1;
      }  else {
        // #2 ---[f2 ; ? ]------------>
        int f2 = range_.first();
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
            if (! range_.is_upper_bounded()) {
              // #1 ---[f1 ; l1]------------>
              // #2 ---[f2 ;               [>
              return -1;
            } else {
              // #2 ---[f2 ; l2]------------>
              int l2 = range_.last();
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
            if (! range_.is_upper_bounded()) {
              // #1 ---[f1 ;                   ]>
              // #2 ---[f2 ;                   ]>
              return 0;
            } else {
              // int l2 = range_.last();
              // #1 ---[f1 ;                   ]>
              // #2 ---[f2 ; l2]---------------]>
              return +1;
            }
          }
        }
      }
    } else {
      // #1 ]   ; ? ]--------------->
      if (range_.is_lower_bounded()) {
        // #1 ]   ; ? ]--------------->
        // #2 ---[f2 ; ? ]------------>
        return -1;
      } else {
        // #1 ]   ; ? ]--------------->
        // #2 ]   ; ? ]--------------->
        if (is_upper_bounded()) {
          // #1 ]   ; l1]--------------->
          int l1 = last();
          if (! range_.is_upper_bounded()) {
            // #1 ]   ; l1]--------------->
            // #2 ]   ;                  [>
            return -1;
          } else {
            int l2 = range_.last();
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
          if (range_.is_upper_bounded()) {
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

  bool integer_range::operator<(const integer_range& range_) const
  {
    return compare(range_) == -1;
  }

  bool integer_range::operator>(const integer_range& range_) const
  {
    return compare(range_) == +1;
  }

  bool integer_range::operator==(const integer_range & range_) const
  {
    return compare(range_) == 0;
  }

  void integer_range::make_canonical(integer_range& range_) const
  {
    range_ = *this;
    if (_lower_flag_ == range_bound_excluded) {
      range_._lower_++;
      range_._lower_flag_ = range_bound_included;
    }
    if (_upper_flag_ == range_bound_excluded) {
      range_._upper_--;
      range_._upper_flag_ = range_bound_included;
    }
    return;
  }

  bool integer_range::intersect(const integer_range & range_) const
  {
    integer_range dummy;
    return intersect(range_, dummy);
  }

  bool integer_range::intersect(const integer_range & range_, integer_range & inter_) const
  {
    DT_THROW_IF(!is_valid(), std::logic_error, "Invalid integer range!");
    DT_THROW_IF(!range_.is_valid(), std::logic_error, "Invalid integer range!");
    inter_.reset();

    integer_range i1;
    this->make_canonical(i1);
    integer_range i2;
    range_.make_canonical(i2);

    range_bound_info_type bf = range_bound_undefined;
    int f(0);
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
    int l(0);

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
        inter_.set(f, l, bf, bl);
        return true;
      } else {
        return false;
      }
    } else {
      inter_.set(f, l, bf, bl);
      return true;
    }
    return false;
  }

} // namespace datatools
