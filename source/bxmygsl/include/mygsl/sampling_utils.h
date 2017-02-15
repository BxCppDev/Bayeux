//! \file  mygsl/sampling_utils.h
//! \brief Sampling utilities
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

#ifndef MYGSL_SAMPLING_UTILS_H
#define MYGSL_SAMPLING_UTILS_H

// Standard library:
#include <limits>
// #include <string>

namespace mygsl {

  /// \brief Constants and resources used for sampling
  struct sampling
  {

    /// Invalid value for an index associated to classification != INDEX_NORMAL
    static const std::size_t INVALID_INDEX = std::numeric_limits<std::size_t>::max();

    /// Minimum number of sampling values in a valid sampling object
    static const std::size_t MIN_NUMBER_OF_SAMPLING_VALUES = 2;

    /// Sampling index classification:
    ///
    /// \code
    ///
    ///    UNDERFLOW            NORMAL                OVERFLOW
    ///  -------------+-----+--+-+----+-----+-+--+--------------------> x
    ///              min                        max
    ///               <-------------------------->
    ///               :  sampling model domain   :
    /// ------------->:                          :<------------------
    /// underflow domain                           overflow domain
    ///
    /// \endcode
    enum index_classification_type {
      INDEX_UNDEFINED  = 0,
      INDEX_NORMAL     = 1,
      INDEX_OVERFLOW   = 2,
      INDEX_UNDERFLOW  = 3
    };

  };

} // end of namespace mygsl

#endif // MYGSL_SAMPLING_UTILS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
