/// \file datatools/configuration/i_occurence.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2014-05-11
 * Last modified : 2014-08-10
 *
 * Copyright (C) 2014 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   Some utilities for the description of configuration variants and parameters.
 *
 * History:
 *
 */

#ifndef DATATOOLS_CONFIGURATION_I_OCCURENCE_H
#define DATATOOLS_CONFIGURATION_I_OCCURENCE_H

// Standard library:
#include <vector>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>

namespace datatools {

  namespace configuration {

    // Forward declaration:
    class occurence;

    /// \brief Abstract interface for all occurrence objects
    class i_occurence
    {
    public:

      /// Constructor
      i_occurence();

      /// Destructor
      virtual ~i_occurence();

      /// Return the dimension
      virtual size_t get_dimension() const = 0;

      /// Return the number of occurences
      virtual size_t get_number_of_occurences() const = 0;

      /// Compute the occurence associated to a given rank
      virtual void compute_occurence(int rank_, occurence & occ_) const = 0;

      /// Check if occurence is multiple
      bool is_multiple() const;

      /// Compute the occurence associated to a given rank
      occurence get_occurence_by_rank(int rank_) const;

      /// Compute a multidimensional index path from a rank
      virtual size_t compute_index_path(std::vector<uint32_t> & path_, int rank_) const = 0;

    };

    /// \brief Single occurence
    class occurence : public i_occurence
    {
    public:

      /// Constructor
      occurence();

      /// Destructor
      virtual ~occurence();

      /// Return the dimension
      virtual size_t get_dimension() const;

      /// Return the number of occurences
      virtual size_t get_number_of_occurences() const;

      /// Compute the occurence associated to a given rank
      virtual void compute_occurence(int rank_, occurence & occ_) const;

      /// Compute a multidimensional index path from a rank
      virtual size_t compute_index_path(std::vector<uint32_t> & path_, int rank_) const;

    };

  }  // end of namespace configuration

}  // end of namespace datatools

#endif // DATATOOLS_CONFIGURATION_I_OCCURENCE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
