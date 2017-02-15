//! \file  mygsl/base_sampling.h
//! \brief Base sampling
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

#ifndef MYGSL_BASE_SAMPLING_H
#define MYGSL_BASE_SAMPLING_H

// Third party:
// - Bayeux/datatools:
#include <datatools/properties.h>

// This project:
#include <mygsl/sampling_utils.h>

namespace mygsl {

  /// \brief Base class for all sampling models
  class base_sampling
  {
  public:

    /// Default constructor
    base_sampling();

    /// Destructor
    virtual ~base_sampling();

    /// Return the minimum sample
    virtual double get_min() const = 0;

    /// Return the maximum sample
    virtual double get_max() const = 0;

    /// Return the number of steps
    virtual std::size_t get_nsteps() const = 0;

    /// Return the number of samples
    virtual std::size_t get_nsamples() const = 0;

    /// Check the initialization status of the sampling
    virtual bool is_initialized() const = 0;

    /// Initialize from a set of properties
    virtual void initialize(const datatools::properties &) = 0;

    /// Reset/invalidate the sampling
    virtual void reset() = 0;

    /// Return the left sample index and a classification associated to a value
    virtual bool value_to_index(const double x_,
                                std::size_t & left_sample_index_,
                                sampling::index_classification_type & classification_) const = 0;

    /// Return the left sample index associated to a value
    bool value_to_index(const double x_,  std::size_t & left_sample_index_) const;

    /// Return the value associated to a given sample index
    virtual sampling::index_classification_type
    index_to_value(const std::size_t sample_index_, double & value_) const = 0;

    /// Check if the checked value is associated to the given sample index
    virtual bool has_index(const double value_, const std::size_t sample_index_) const;

    /// Return the first sample index
    std::size_t get_first_index() const;

    /// Return the last sample index
    std::size_t get_last_index() const;

    /// Return the first step index
    std::size_t get_first_step_index() const;

    /// Return the last step index
    std::size_t get_last_step_index() const;

    /// Check if a sample index is valid
    bool index_is_valid(const std::size_t sample_index_) const;

    /// Check if a value is in the sampling domain
    bool value_is_valid(const double value_) const;

    /// Compute the bounds of step with given index
    bool compute_step_bounds(const std::size_t step_index_, double & min_, double & max_) const;

  };

} // end of namespace mygsl

#endif // MYGSL_BASE_SAMPLING_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
