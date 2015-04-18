/// \file datatools/integer_range.h
/* Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2011-09-22
 * Last modified: 2014-08-31
 *
 * License:
 *
 * Copyright (C) 2011-2014 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
#ifndef DATATOOLS_INTEGER_RANGE_H
#define DATATOOLS_INTEGER_RANGE_H

// Standard Library:
#include <iostream>
#include <string>
#include <set>

// Third Party:
// - Boost:
#include <boost/cstdint.hpp>

// This Project:
#include <datatools/range_tools.h>

namespace datatools {

  //! \brief A class representing an interval of integer values.
  class integer_range {
  public:

    // Intrinsic integer is 32 bits:
    typedef int32_t value_type;

    /// Default constructor
    integer_range();

    /// Constructor
    integer_range(value_type from, value_type to,
                  range_bound_info_type from_policy = range_bound_included,
                  range_bound_info_type to_policy   = range_bound_included);

    /// Check the validity of the interval
    bool is_valid() const;

    /// Invalidate the interval
    void invalidate();

    /// Reset lower bound
    void reset_lower();

    /// Reset upper bound
    void reset_upper();

    /// Reset the interval
    void reset();

    /// Set the lower bound
    void set_lower(value_type from, range_bound_info_type policy = range_bound_included);

    /// Set the upper bound
    void set_upper(value_type to, range_bound_info_type policy = range_bound_included);

    /// Set the bounds
    void set(value_type from, value_type to,
             range_bound_info_type from_policy = range_bound_included,
             range_bound_info_type to_policy = range_bound_included);

    /// Check if lower bound is defined
    bool is_lower_bounded() const;

    /// Check if upper bound is defined
    bool is_upper_bounded() const;

    /// Check if the intervalis half-bounded
    bool is_half_bounded() const;

    /// Check if both bounds are defined
    bool is_bounded() const;

    /// Check if the interval is degenerated (one unique value)
    bool is_singleton() const;

    /// Check if lower bound is included
    bool is_lower_included() const;

    /// Check if upper bound is included
    bool is_upper_included() const;

    /// Return lower bound
    value_type get_lower_bound() const;

    /// Return upper bound
    value_type get_upper_bound() const;

    /// Return the number of values in the interval
    uint64_t count() const;

    /// Check if interval is empty
    bool is_empty() const;

    // Collections of 'make-methods' :

    /// Build an empty interval
    // "(0,0)"
    void make_empty();

    /// Build a fully bounded interval
    // "[min_int,max_int]"
    void make_full();

    /// Build a fully bounded positive interval starting at 0
    // "[0,max_int]"
    void make_full_positive();

    /// Build a fully bounded  negative interval ending at 0
    // "[min_int,0]"
    void make_full_negative();

    /// Build a half bounded interval with no upper bound
    // "[lower,)" or "(lower,)
    void make_upper_unbounded(value_type from, bool inclusive = true);

    /// Build a half bounded interval with no lower bound
    // "(,upper]" or "(,upper)"
    void make_lower_unbounded(value_type to, bool inclusive = true);

    /// Build a fully bounded interval with specific lower and upper bounds
    // "(lower,upper]" or "(lower,upper)"
    // "[lower,upper]" or "[lower,upper)"
    void make_bounded(value_type from, value_type to,
                      bool lower_included = true,
                      bool upper_included = true);

    /// Check if a value belongs to the interval
    bool has(value_type value) const;

    /// Check if an integer interval belongs to the interval
    bool has(const integer_range & ir) const;

    /// Basic print
    void dump(std::ostream& out = std::clog) const;

    /// Print operator
    friend std::ostream& operator<<(std::ostream& out,
                                    const integer_range& a_range);

    /// Input operator
    friend std::istream& operator>>(std::istream& in,
                                    integer_range& a_range);

    /// Return the first value from the fully bounded interval
    value_type begin() const;

    /// Return the past-the-end value (not belonging) from the fully bounded interval
    value_type end() const;

    /// Return the first value belonging to the half lower bounded interval
    value_type first() const;

    /// Return the last value belonging to the half upper bounded interval
    value_type last() const;

    /// Compare intervals
    int compare(const integer_range& a_range) const;

    /// Comparison operator
    bool operator<(const integer_range& a_range) const;

    /// Comparison operator
    bool operator>(const integer_range& a_range) const;

    /// Comparison operator
    bool operator==(const integer_range& a_range) const;

    /// Build the associated canonical interval (with bounds flagged as included)
    void make_canonical(integer_range& a_range) const;

    /// Check intersect with another interval and build the resulting intersection interval
    bool intersect(const integer_range& a_range, integer_range& a_inter) const;

    /// Check intersect with another interval
    bool intersect(const integer_range& a_range) const;

  private:
    range_bound_info_type lower_flag_; //!< The flag associated to the lower bound
    value_type            lower_;      //!< The lower bound
    range_bound_info_type upper_flag_; //!< The flag associated to the upper bound
    value_type            upper_;      //!< The upper bound
  };

} // end of namespace datatools

#endif // DATATOOLS_INTEGER_RANGE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
