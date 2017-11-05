/// \file datatools/configuration/single_occurrence.h
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

#ifndef DATATOOLS_CONFIGURATION_SINGLE_OCCURRENCE_H
#define DATATOOLS_CONFIGURATION_SINGLE_OCCURRENCE_H

// Standard library:
// #include <vector>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>

// This project (Bayeux/datatools):
#include <datatools/configuration/i_occurrence.h>

namespace datatools {

  namespace configuration {

    /// \brief Single occurrence for a physical parameter in a variant model
    class single_occurrence : public i_occurrence
    {
    public:

      /// Set the intrinsic id
      void set_id(int);

      /// Return intrinsic id
      int get_id() const;

      /// Constructor
      single_occurrence();

      /// Destructor
      virtual ~single_occurrence();

      /// Return the dimension
      virtual size_t get_dimension() const;

      /// Return the number of occurrences
      virtual size_t get_number_of_occurrences() const;

      /// Compute the occurrence associated to a given rank
      virtual void compute_occurrence(int rank_, single_occurrence & occ_) const;

      /// Compute a multidimensional index path from a rank
      virtual size_t compute_index_path(std::vector<uint32_t> & path_, int rank_) const;

      /// Convert to a string
      virtual std::string to_string() const;

    private:

      int32_t _id_; //!< Identifier

    };

  }  // end of namespace configuration

}  // end of namespace datatools

#endif // DATATOOLS_CONFIGURATION_SINGLE_OCCURRENCE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
