// -*- mode: c++; -*-
// integer_range.cc
// Ourselves 
#include <datatools/integer_range.h>

// Standard Library
#include <sstream>
#include <stdexcept>
#include <limits>

// Third Party
// - A

// This Project

using namespace std;

namespace datatools {

// Ctor:
integer_range::integer_range() {
  this->reset();
}


// Ctor:
integer_range::integer_range(value_type from, value_type to, 
                              int from_policy, int to_policy) {
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
  upper_      = numeric_limits<value_type>::max();
  upper_flag_ = range_bound_undefined;
}


void integer_range::reset() {
  this->reset_lower();
  this->reset_upper();
}


void integer_range::set_lower(value_type from, int policy) {
  if (policy == range_bound_unbound) {
    // "];...."
    lower_      = std::numeric_limits<value_type>::min();
    lower_flag_ = policy;
  } else if (policy == range_bound_included || policy == range_bound_excluded) {
    // "[value;...." || "]value;..."
    lower_      = from;
    lower_flag_ = policy;
  } else {
    throw std::logic_error("datatools::integer_range::set_lower: Invalid policy!");
  }
}


void integer_range::set_upper(value_type to, int policy) {
  if (policy == range_bound_unbound) {
    // "...;...["
    upper_      = std::numeric_limits<value_type>::max();
    upper_flag_ = policy;
  } else if (policy == range_bound_included || policy == range_bound_excluded) {
    // "...;value]" ||  "...;value["
    upper_      = to;
    upper_flag_ = policy;
  } else {
    throw std::logic_error("datatools::integer_range::set_upper: Invalid policy!");
  }
}


void integer_range::set(value_type from, value_type to, 
                         int from_policy, int to_policy) {
  this->set_lower(from, from_policy);
  this->set_upper(to, to_policy);
}


bool integer_range::is_lower_bounded() const {
  return lower_flag_ > 0 ;
}


bool integer_range::is_upper_bounded() const {
  return upper_flag_ > 0 ;
}


bool integer_range::is_bounded() const {
  return this->is_lower_bounded() && this->is_upper_bounded();
}


integer_range::value_type integer_range::get_lower_bound() const {
  if (!this->is_lower_bounded()) {
    throw std::logic_error("datatools::integer_range::get_lower_bound: No lower bound !");
  }
  return lower_;
}


integer_range::value_type integer_range::get_upper_bound() const {
  if (!this->is_upper_bounded()) {
    throw std::logic_error("datatools::integer_range::get_upper_bound: No upper bound !");
  }
  return upper_;
}


bool integer_range::is_lower_included() const {
  if (!this->is_lower_bounded()) {
    throw std::logic_error("datatools::integer_range::is_lower_included: No lower bound !");
  }
  return (lower_flag_ == range_bound_included);
}


bool integer_range::is_upper_included() const {
  if (!this->is_upper_bounded()) {
    throw std::logic_error("datatools::integer_range::is_upper_included: No upper bound !");
  }
  return (upper_flag_ == range_bound_included);
}


uint64_t integer_range::count() const {
  if (!is_bounded()) {
    throw std::logic_error("datatools::integer_range::count: (semi-)infinite range !");
  }

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
            std::numeric_limits<value_type>::max(), 
            range_bound_included, 
            range_bound_included);
}


void integer_range::make_full_positive() {
  this->set(0, 
            std::numeric_limits<value_type>::max(), 
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
            range_bound_unbound);
}


void integer_range::make_lower_unbounded(value_type to, bool inclusive) {
  this->set(0,
            to,
            range_bound_unbound,
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
    out << "|-- " << "Lower bounded  = " << this->is_lower_bounded() << std::endl;
    if (this->is_lower_bounded()) {
      out << "|-- " << "Lower bound    = " << lower_ << std::endl;
      out << "|-- " << "Lower included = " << this->is_lower_included() << std::endl;
      out << "|-- " << "First          = " << this->first() << std::endl;
    }

    out << "|-- " << "Upper bounded  = " << this->is_upper_bounded() << std::endl;

    if (this->is_upper_bounded()) {
      out << "|-- " << "Upper bound    = " << upper_ << std::endl;
      out << "|-- " << "Upper included = " << this->is_upper_included() << std::endl;
      out << "|-- " << "Last           = " << this->last() << std::endl;
    }
    
    out << "|-- " << "Bounded        = " << this->is_bounded() << std::endl;

    if (this->is_bounded()) {
      out << "|-- " << "Count          = " << this->count() << std::endl;
      out << "|-- " << "Empty          = " << this->is_empty() << std::endl;
      out << "|-- " << "Begin          = " << this->begin() << std::endl;
      out << "|-- " << "End            = " << this->end() << std::endl;
    }
  } else {
    out << "|-- " << "[invalid]" << std::endl;
  }
  out << "`-- " << "Rendering      = '" << *this << "'" << std::endl; 
}


integer_range::value_type integer_range::begin() const {
  if (!this->is_bounded()) {
    throw std::logic_error("datatools::integer_range::begin: Not bounded !");
  }
  value_type the_beg = lower_;
  if (!this->is_lower_included()) {
    the_beg++;
  }
  return the_beg;
}


integer_range::value_type integer_range::end() const {
  if (!this->is_bounded()) {
    throw std::logic_error("datatools::integer_range::end: Not bounded !");
  }
  value_type the_end = upper_; 
  // case : the_end != numeric_limits<value_type>::max () ???
  if (this->is_upper_included()) {
    the_end++;
  }
  return the_end;
}


integer_range::value_type integer_range::first() const {
  if (!this->is_lower_bounded()) {
    throw std::logic_error("datatools::integer_range::first: Not lower bounded !");
  }
  value_type the_first = lower_;
  if (!this->is_lower_included()) {
    the_first++;
  }
  return the_first;
}


integer_range::value_type integer_range::last() const {
  if (!this->is_upper_bounded()) {
    throw std::logic_error("datatools::integer_range::last: Not upper bounded !");
  }
  value_type the_last = upper_; 
  if (!this->is_upper_included()) {
    the_last--;
  }
  return the_last;
}


std::ostream& operator<<(std::ostream& out, const integer_range& a_range) {
  if (!a_range.is_valid()) {
    out << "]?[";
  } else {
    if (a_range.is_lower_bounded()) {
      if (a_range.is_lower_included()) {
        out << range_tools::token_open;
      } else {
        out << range_tools::token_close;
      }
      out << a_range.get_lower_bound();
    } else {
      out << range_tools::token_close;
    }

    out << ';';

    if (a_range.is_upper_bounded()) {
      out << a_range.get_upper_bound();
      if (a_range.is_upper_included()) {
        out << range_tools::token_close;
      } else {
        out << range_tools::token_open;
      }
    } else {
      out << range_tools::token_open;
    }
  }
  return out;
}

} // namespace datatools

// end of integer_range.cc
