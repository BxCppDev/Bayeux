// -*- mode: c++; -*- 
/* datatools/utils/integer_range.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2011-09-22
 * Last modified: 2011-09-22
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
 *   Integer range class.
 * 
 * History: 
 *  
 */
#ifndef DATATOOLS_UTILS_INTEGER_RANGE_H_
#define DATATOOLS_UTILS_INTEGER_RANGE_H_
// Standard Library
#include <iostream>
#include <string>
#include <set>

// Third Party
// - Boost
#include <boost/cstdint.hpp>

// This Project
#include <datatools/utils/range_tools.h>


namespace datatools {
namespace utils {

class integer_range {
 public:
  typedef int32_t value_type;

 public:
  // Ctor:
  integer_range();

  // Ctor:
  integer_range(value_type from, value_type to, 
                int from_policy = range::included, 
                int to_policy = range::included);


  bool is_valid() const;

  void invalidate();

  void reset_lower();

  void reset_upper();

  void reset();

  void set_lower(value_type from, int policy = range::included);

  void set_upper(value_type to, int policy = range::included);

  void set(value_type from, value_type to, 
           int from_policy = range::included, 
           int to_policy = range::included);

  bool is_lower_bounded() const;

  bool is_upper_bounded() const;

  bool is_bounded() const;

  bool is_lower_included() const;

  bool is_upper_included() const;

  value_type get_lower_bound() const;

  value_type get_upper_bound() const;

  uint64_t count() const;

  bool is_empty() const;

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

  friend std::ostream& operator<<(std::ostream& out, 
                                  const integer_range& a_range); 

  value_type begin() const;

  value_type end() const;

  value_type first() const;

  value_type last() const;

 private:
  char       lower_flag_;
  value_type lower_;
  char       upper_flag_;
  value_type upper_;
};


} // end of namespace utils 
} // end of namespace datatools 

#endif // DATATOOLS_UTILS_INTEGER_RANGE_H_

