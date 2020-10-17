//! \file  mygsl/tabulated_sampling.h
//! \brief Tabulated sampling
//
// Copyright (c) 2016 by François Mauger <mauger@lpccaen.in2p3.fr>
//
// This file is part of Bayeux.
//
// Bayeux is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bayeux is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Bayeux. If not, see <http://www.gnu.org/licenses/>.

#ifndef MYGSL_TABULATED_SAMPLING_H
#define MYGSL_TABULATED_SAMPLING_H

// Standard library:
#include <vector>

// This project:
#include <mygsl/base_sampling.h>

namespace mygsl {

  /// \brief Tabulated sampling model
  class tabulated_sampling
    : public base_sampling
  {
  public:

    /// Default constructor
    tabulated_sampling();

    /// Destructor
    ~tabulated_sampling() override;

    /// Return the minimum sample
    double get_min() const override;

    /// Return the maximum sample
    double get_max() const override;

    /// Return the number of steps
    std::size_t get_nsteps() const override;

    /// Return the number of nsamples
    std::size_t get_nsamples() const override;

    /// Return the absolute guard distance around sampling alues
    double get_guard() const;

    /// Return the minimum step
    double get_min_step() const;

    /// Return the maximum step
    double get_max_step() const;

    /// Check the initialization status of the sampling
    bool is_initialized() const override;

    /// Reset/invalidate the sampling
    void initialize(const datatools::properties &) override;

    /// Reset/invalidate the sampling
    void reset() override;

    /// Return the index and a classification associated to a value
    bool value_to_index(const double x_,
                                std::size_t & index_,
                                sampling::index_classification_type & classification_flags_) const override;

    /// Return the value associated to a given index
    sampling::index_classification_type
    index_to_value(const std::size_t index_,
                   double & value_) const override;

    /// Set the absolute guard distance around sampling alues
    void set_guard(const double guard_);

    /// Add a tabulated value
    void append(const double value_);

    /// Build sampling from a ordered list of values
    ///
    /// \code
    ///                   0      1      2             nsteps-2  nsteps-1
    /// ---------------+-----+------+--------+ - - - +--------+---------+---------> x
    ///               val0  val1   val2     val3   valN-2   valN-2    valN-1
    ///               min                                              max
    /// \endcode
    void make_ordered(const std::vector<double> & values_, const double guard_ = 0.0);

    /// Lock the sampling table
    void lock();

  private:

    void _set_guard_(double guard_);

    void _check_guard_();

    void _init_();

    void _reset_();

  private:

    // Management:
    bool                _initialized_ = false; ///< Initialization flag

    // Configuration:
    std::vector<double> _table_; ///< Tabulated values
    double              _guard_; ///< Absolute guard distance around sampling values

    // Internals:
    double              _min_step_; ///< Minimum step
    double              _max_step_; ///< Maximum step

  };

} // end of namespace mygsl

#endif // MYGSL_BASE_SAMPLING_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
