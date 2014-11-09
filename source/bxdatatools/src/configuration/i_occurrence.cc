// datatools/configuration/i_occurrence.cc
/*
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
 */

// Ourselves:
#include <datatools/configuration/i_occurrence.h>

// Standard library:
#include <string>
#include <vector>

// This project (Bayeux/datatools):
#include <datatools/exception.h>
#include <datatools/configuration/single_occurrence.h>

namespace datatools {

  namespace configuration {

    i_occurrence::i_occurrence()
    {
      return;
    }

    i_occurrence::~i_occurrence()
    {
      return;
    }

    bool i_occurrence::is_multiple() const
    {
      return get_number_of_occurrences() > 1;
    }

    single_occurrence i_occurrence::get_occurrence_by_rank(int rank_) const
    {
      single_occurrence occ;
      compute_occurrence(rank_, occ);
      return occ;
    }

  }  // end of namespace configuration

}  // end of namespace datatools
