/// \file datatools/configuration/array_occurrence.h
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
 */

#ifndef DATATOOLS_CONFIGURATION_ARRAY_OCCURRENCE_H
#define DATATOOLS_CONFIGURATION_ARRAY_OCCURRENCE_H

// This project (Bayeux/datatools):
#include <datatools/configuration/i_occurrence.h>

namespace datatools {

  namespace configuration {

    /// \brief Single occurrence
    class array_occurrence
      : public i_occurrence
    {
    public:

      /// Set the size of the array
      void set_size(unsigned int sz_);

      /// Return the size of the array
      unsigned int get_size() const;

      /// Set the starting id
      void set_start_id(int sid_);

      /// Get the starting id
      int get_start_id() const;

      /// Constructor
      array_occurrence();

      /// Destructor
      ~array_occurrence() override;

      /// Return the dimension
      size_t get_dimension() const override;

      /// Return the number of occurrences
      size_t get_number_of_occurrences() const override;

      /// Compute the occurrence associated to a given rank
      void compute_occurrence(int rank_, single_occurrence & occ_) const override;

      /// Compute a multidimensional index path from a rank
      size_t compute_index_path(std::vector<uint32_t> & path_, int rank_) const override;

      /// Convert to a string
      std::string to_string() const override;

    private:

      unsigned int _size_;     //!< Number of occurrences
      int          _start_id_; //!< Starting id

    };

  }  // end of namespace configuration

}  // end of namespace datatools

#endif // DATATOOLS_CONFIGURATION_ARRAY_OCCURRENCE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
