// -*- mode: c++; -*-
// real_range.cc
// Ourselves 
#include <datatools/real_range.h>

// Standard Library
#include <cmath>
#include <limits>
#include <sstream>
#include <stdexcept>

// Third Party
// - A

// This Project


namespace datatools {

const double real_range::DEFAULT_EPSILON = 5.e-15;

// Ctor:
real_range::real_range() {
  abs_epsilon_ = std::numeric_limits<value_type>::quiet_NaN();
  this->reset();
}

// Ctor:
real_range::real_range(value_type from, value_type to, 
                       int from_policy, int to_policy) {
  abs_epsilon_ = std::numeric_limits<value_type>::quiet_NaN();
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
  lower_      = -std::numeric_limits<value_type>::max();
  lower_flag_ = range_bound_undefined;
}


void real_range::reset_upper() {
  upper_      = std::numeric_limits<value_type>::max();
  upper_flag_ = range_bound_undefined;
}


void real_range::reset() {
  abs_epsilon_ = std::numeric_limits<double>::quiet_NaN();
  this->reset_lower();
  this->reset_upper();
}


void real_range::set_lower(value_type from, int policy) {
  if (policy == range_bound_unbound) {
    // "];...."
    lower_      = -std::numeric_limits<value_type>::infinity();
    lower_flag_ = policy;
  } else if (policy == range_bound_included || policy == range_bound_excluded) {
    // "[value;...." || "]value;...."
    lower_      = from;
    lower_flag_ = policy;
  } else {
    throw std::logic_error("datatools::real_range::set_lower: Invalid lower bound policy !");
  }
}


void real_range::set_upper(value_type value, int policy) {
  if (policy == range_bound_unbound) {
    // "...;...["
    upper_      = +std::numeric_limits<value_type>::infinity();
    upper_flag_ = policy;
  } else if (policy == range_bound_included || policy == range_bound_excluded) {
    // "...;value]" || "...;value["
    upper_      = value;
    upper_flag_ = policy;
  } else {
    throw std::logic_error("datatools::real_range::set_upper: Invalid upper bound policy !");
  }
}


void real_range::set(value_type from, value_type to, int from_policy, 
                     int to_policy) {
  this->set_lower(from, from_policy);
  this->set_upper(to, to_policy);
}


bool real_range::is_lower_bounded() const {
  return lower_flag_ > 0 ;
}


bool real_range::is_upper_bounded() const {
  return upper_flag_ > 0 ;
}

bool real_range::is_bounded() const {
  return this->is_lower_bounded() && this->is_upper_bounded();
}


real_range::value_type real_range::get_lower_bound() const {
  if (!this->is_lower_bounded()) {
    throw std::logic_error("datatools::real_range::get_lower_bound: No lower bound !");
  }
  return lower_;
}


real_range::value_type real_range::get_upper_bound() const {
  if (!this->is_upper_bounded()) {
    throw std::logic_error("datatools::real_range::get_upper_bound: No upper bound !");
  }
  return upper_;
}


bool real_range::is_lower_included() const {
  if (!this->is_lower_bounded()) {
    throw std::logic_error("datatools::real_range::is_lower_included: No lower bound !");
  }
  return (lower_flag_ == range_bound_included);
}


bool real_range::is_upper_included() const {
  if (!this->is_upper_bounded()) {
    throw std::logic_error("datatools::real_range::is_upper_included: No upper bound !");
  }
  return (upper_flag_ == range_bound_included);
}


void real_range::make_empty() {
  this->set(0, 0, range_bound_excluded, range_bound_excluded);
}


void real_range::make_full() {
  this->set(-std::numeric_limits<value_type>::max(), 
            std::numeric_limits<value_type>::max(), 
            range_bound_included, 
            range_bound_included);
}


void real_range::make_full_positive() {
  this->set(0, 
            std::numeric_limits<value_type>::max(), 
            range_bound_included, 
            range_bound_included);
}


void real_range::make_full_negative() {
  this->set(-std::numeric_limits<value_type>::max(), 
            0, 
            range_bound_included, 
            range_bound_included);
}


void real_range::make_upper_unbounded(value_type from, bool inclusive) {
  this->set(from, 
            +std::numeric_limits<value_type>::infinity(), 
            inclusive ? range_bound_included : range_bound_excluded , 
            range_bound_unbound);
}


void real_range::make_lower_unbounded(value_type to, bool inclusive) {
  this->set(-std::numeric_limits<value_type>::infinity(),
            to,
            range_bound_unbound,
            inclusive ? range_bound_included : range_bound_excluded);
}


void real_range::make_bounded(value_type from, value_type to,
                              bool lower_included, bool upper_included) {
  this->set(from,
            to,
            lower_included ? range_bound_included : range_bound_excluded,
            upper_included ? range_bound_included : range_bound_excluded);
}


double real_range::compute_epsilon(double a_lower, double a_upper) {
  double eps;
  double l_eps = std::numeric_limits<double>::quiet_NaN();
  double u_eps = std::numeric_limits<double>::quiet_NaN();
  if (a_lower == a_lower) {
    double scale = std::abs(a_lower);
    l_eps = DEFAULT_EPSILON * scale;
  } 

  if (a_upper == a_upper) {
    double scale = std::abs(a_upper);
    u_eps = DEFAULT_EPSILON * scale;
  } 

  if ((l_eps == l_eps) && (u_eps == u_eps)) {
    eps = std::max(l_eps, u_eps);
  } else if (l_eps == l_eps) {
    eps = l_eps;
  } else {
    eps = u_eps;
  }      

  return eps;
}


bool real_range::has(value_type value) const {
  static bool ok = true;
  double eps = abs_epsilon_;
  if (eps != eps) {
    eps = this->compute_epsilon((lower_ == lower_) ? 
                                lower_ : 
                                std::numeric_limits<double>::quiet_NaN(),
                                (upper_ == upper_) ? 
                                upper_ : 
                                std::numeric_limits<double>::quiet_NaN());
    {
      real_range& rr = const_cast<real_range&>(*this);
      rr.abs_epsilon_ = eps;
    }
    if (ok) {
      std::clog << "real_range::has: abs_epsilon_ = " 
                << abs_epsilon_ << std::endl;
      ok = false;
    }
  }

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


void real_range::dump(std::ostream& a_out) const {
  a_out << "real_range: " << std::endl;
  if (this->is_valid()) {
    a_out << "|-- " << "Lower bounded  = " << this->is_lower_bounded() << std::endl;
    if (this->is_lower_bounded()) {
      a_out << "|-- " << "Lower bound    = " << lower_ << std::endl;
      a_out << "|-- " << "Lower included = " << this->is_lower_included() << std::endl;
    }
    a_out << "|-- " << "Upper bounded  = " << this->is_upper_bounded() << std::endl;
    if (this->is_upper_bounded()) {
      a_out << "|-- " << "Upper bound    = " << upper_ << std::endl;
      a_out << "|-- " << "Upper included = " << this->is_upper_included() << std::endl;
    }
    a_out << "|-- " << "Bounded        = " << this->is_bounded() << std::endl;
  } else {
    a_out << "|-- " << "[invalid]" << std::endl;
  }
  a_out << "`-- " << "Rendering      = '" << *this << "'" << std::endl; 
}


std::ostream& operator<<(std::ostream& a_out, const real_range& a_range) {
  if (!a_range.is_valid()) {
    a_out << "]?[";
  } else {
    if (a_range.is_lower_bounded()) {
      if (a_range.is_lower_included()) {
        a_out << range_tools::token_open;
      } else {
        a_out << range_tools::token_close;
      }
      a_out << a_range.get_lower_bound();
    } else {
      a_out << range_tools::token_close;
    }
    a_out << ';';
    if (a_range.is_upper_bounded()) {
      a_out << a_range.get_upper_bound();
      if (a_range.is_upper_included()) {
        a_out << range_tools::token_close;
      } else {
        a_out << range_tools::token_open;
      }
    } else {
      a_out << range_tools::token_open;
    }
  }
  return a_out;
}

} // namespace datatools

