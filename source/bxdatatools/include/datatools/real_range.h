// -*- mode: c++; -*- 
/* datatools/real_range.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2011-09-25
 * Last modified: 2011-09-25
 * 
 * License: 
 * 
 * Copyright (C) 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, 
 * Boston, MA 02110-1301, USA.
 * 
 * Description: 
 *
 *   Real range/interval class.
 * 
 * History: 
 *  
 */
#ifndef DATATOOLS_REAL_RANGE_H_
#define DATATOOLS_REAL_RANGE_H_
// Standard Library
#include <iostream>
#include <string>
#include <set>

// Third Party
// - A

// This Project
#include <datatools/range_tools.h>


namespace datatools {

//! \brief A class representing an interval of real (double precision) values.
class real_range {
 public:
  typedef double value_type;
  static const double DEFAULT_EPSILON;

 public:
  // Ctor:
  real_range();

  // Ctor:
  real_range(value_type from, value_type to, 
             int from_policy = range_bound_included, 
             int to_policy = range_bound_included);

  bool is_valid() const;

  void invalidate();

  void reset_lower();

  void reset_upper();

  void reset();

  void set_lower(value_type from, int policy = range_bound_included);

  void set_upper(value_type to, int policy = range_bound_included);

  void set(value_type from, value_type to, 
           int from_policy = range_bound_included, 
           int to_policy = range_bound_included);

  bool is_lower_bounded() const;

  bool is_upper_bounded() const;

  bool is_bounded() const;

  bool is_lower_included() const;

  bool is_upper_included() const;

  value_type get_lower_bound() const;

  value_type get_upper_bound() const;

  // Collections of 'make-methods' :

  // "];["
  void make_empty();

  // "[min_int;max_int]"
  void make_full();

  // "[0;max_int]"
  void make_full_positive();

  // "[min_int;0]"
  void make_full_negative();

  // "[lower;[" or "]lower;[
  void make_upper_unbounded(value_type from, bool inclusive = true);

  // "];upper]" or "];upper["
  void make_lower_unbounded(value_type to, bool inclusive = true);

  // "]lower;upper]" or "]lower;upper["
  // "[lower;upper]" or "[lower;upper["
  void make_bounded(value_type from, value_type to,
                    bool lower_included = true, 
                    bool upper_included = true);

  bool has(value_type value) const;

  void dump(std::ostream& out = std::clog) const;

  static double compute_epsilon(double a_lower, double a_upper);

  friend std::ostream& operator<<(std::ostream& a_out, 
                                  const real_range& a_range); 

 private:
  value_type abs_epsilon_;
  char       lower_flag_;
  value_type lower_;
  char       upper_flag_;
  value_type upper_;
};


} // end of namespace datatools 

#endif // DATATOOLS_REAL_RANGE_H_

