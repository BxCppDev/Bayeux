//! \file  mygsl/high_density_edge_sampling.h
//! \brief Sampling with higher density on the edges
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

#ifndef MYGSL_HIGH_DENSITY_EDGE_SAMPLING_H
#define MYGSL_HIGH_DENSITY_EDGE_SAMPLING_H

// This project:
#include <mygsl/base_sampling.h>
#include <mygsl/polynomial.h>

namespace mygsl {

  /// \brief High density edge sampling model
  ///
  /// Example of sampling:
  /// \code
  ///
  ///      nsteps_edge_left=6          nsteps_medium=4      nsteps_edge_right=3
  ///
  ///     :<- - - - - - - - - >:<- - - - - - - - - - - - ->:<- - - >:   <== domains
  ///     :                    :                           :        :
  ///     :01  2  3   4    5   :   6      7      8     9   :10 11 12:   <== steps
  /// ----[+-+--+---+----+-----#------+------+------+------#---+--+-]------------------> x
  ///     01 2  3   4    5   IL=6     7      8      9   IR=10 11 12 13  <== samples
  ///
  ///   xmin                 xedge                      xedge      xmax
  ///                        left                       right
  ///
  /// \endcode
  ///
  class high_density_edge_sampling
    : public base_sampling
  {
  public:

    /// Default constructor
    high_density_edge_sampling();

    /// Destructor
    virtual ~high_density_edge_sampling();

    /// Make high sampling left/right edges
    void make_left_right(const double xmin_,
                         const double xmax_,
                         const double xedge_left_,
                         const double xedge_right_,
                         const std::size_t nmedium_,
                         const std::size_t nedge_left_,
                         const std::size_t nedge_right_);

    /// Make high sampling left edge
    void make_left(const double xmin_,
                   const double xmax_,
                   const double xedge_left_,
                   const std::size_t nmedium_,
                   const std::size_t nedge_left_);

    /// Make high sampling right edge
    void make_right(const double xmin_,
                    const double xmax_,
                    const double xedge_right_,
                    const std::size_t nmedium_,
                    const std::size_t nedge_right_);

    /// Return the number of steps
    virtual std::size_t get_nsteps() const;

    /// Return the number of samples
    virtual std::size_t get_nsamples() const;

    /// Return the minimum sample
    virtual double get_min() const;

    /// Return the maximum sample
    virtual double get_max() const;

    /// Return the width of the medium step
    double get_medium_step() const;

    /// Check the initialization status of the sampling
    virtual bool is_initialized() const;

    /// Reset/invalidate the sampling
    virtual void initialize(const datatools::properties &);

    /// Reset/invalidate the sampling
    virtual void reset();

    /// Return the index and a classification associated to a value
    virtual bool value_to_index(const double x_,
                                std::size_t & index_,
                                sampling::index_classification_type & classification_flags_) const;

    /// Return the value associated to a given index
    virtual sampling::index_classification_type
    index_to_value(const std::size_t index_,
                   double & value_) const;

  private:

    void _init_();

    void _reset_();

  private:

    // Management:
    bool _initialized_ = false;      ///< Initialization flag

    // Configuration:
    double _xmin_;                   ///< Minimum bound of the sampling domain
    double _xmax_;                   ///< Maximum bound of the sampling domain
    double _xedge_left_;             ///< Bound of the left edge domain
    double _xedge_right_;            ///< Bound of the right edge domain
    std::size_t _nsteps_medium_;     ///< Number of steps in the medium domain
    std::size_t _nsteps_edge_left_;  ///< Number of steps in the left edge domain
    std::size_t _nsteps_edge_right_; ///< Number of steps in the right edge domain

    // Internals:
    std::size_t _nsamples_;
    std::size_t _il_;
    std::size_t _ir_;
    polynomial _left_;
    polynomial _medium_;
    polynomial _right_;

  };

} // end of namespace mygsl

#endif // MYGSL_HIGH_DENSITY_EDGE_SAMPLING_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
