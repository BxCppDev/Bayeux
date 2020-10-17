//! \file  mygsl/linear_sampling.h
//! \brief Linear sampling
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

#ifndef MYGSL_LINEAR_SAMPLING_H
#define MYGSL_LINEAR_SAMPLING_H

// This project:
#include <mygsl/base_sampling.h>

namespace mygsl {

  /// \brief Linear sampling model
  class linear_sampling
    : public base_sampling
  {
  public:

    /// Default constructor
    linear_sampling();

    /// Destructor
    ~linear_sampling() override;

    /// Return the minimum sample
    double get_min() const override;

    /// Return the maximum sample
    double get_max() const override;

    /// Return the number of steps
    std::size_t get_nsteps() const override;

    /// Return the number of nsamples
    std::size_t get_nsamples() const override;

    /// Return the step
    double get_step() const;

    /// Return the absolute guard distance around sampling values
    double get_guard() const;

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
    index_to_value(const std::size_t index_, double & value_) const override;

    /// Build sampling from min to max and given number of steps
    ///
    /// \code
    ///                   0     1     2                nsteps-1
    /// ---------------+-----+-----+-----+ - - - +-----+-----+---------> x
    ///               min    <----->            nsteps-2    max
    ///                       [step]
    /// \endcode
    void make_range(const double min_,
                    const double max_,
                    const std::size_t nsteps_,
                    const double guard_ = 0.0);

    /// Build sampling from given min up to max with given number of steps and given step size
    ///
    /// \code
    ///                  step
    ///                +----->----->-----> . . . ------>----->
    ///                :  0     1     2                nsteps-1
    /// ---------------+-----+-----+-----+ - - - +-----+-----+---------> x
    ///               min                       nsteps-2   [max]
    /// \endcode
    void make_from_min(const double min_,
                       const std::size_t nsteps_,
                       const double step_,
                       const double guard_ = 0.0);


    /// Build sampling from given max down to min with given number of steps and given step size
    ///
    /// \code
    ///                                                  step
    ///                <-----<-----<------ . . . <-----<-----+
    ///                   0     1     2                      :
    /// ---------------+-----+-----+-----+ - - - +-----+-----+---------> x
    ///              [min]                             nsteps-1
    ///                                                     max
    /// \endcode
    void make_to_max(const double max_,
                     const std::size_t nsteps_,
                     const double step_,
                     const double guard_ = 0.0);

  private:

    void _set_guard_(double guard_);

    void _init_();

    void _reset_();

  private:

    double      _min_;     ///< Minimum sampling value
    double      _max_;     ///< Maximum sampling value
    std::size_t _nsteps_;  ///< Number of regular sampling intervals
    double      _step_;    ///< Interval width
    double      _guard_;   ///< Absolute guard distance around sampling values

  };

} // end of namespace mygsl

#endif // MYGSL_BASE_SAMPLING_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
