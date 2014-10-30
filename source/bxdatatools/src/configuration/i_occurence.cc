// datatools/configuration/i_occurence.cc
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
#include <datatools/configuration/i_occurence.h>

// Standard library:
#include <string>
#include <vector>

// This project (Bayeux/datatools):
#include <datatools/exception.h>

namespace datatools {

  namespace configuration {

    // Abstract mother occurence class:
    i_occurence::i_occurence()
    {
      return;
    }

    i_occurence::~i_occurence()
    {
      return;
    }

    bool i_occurence::is_multiple() const
    {
      return get_number_of_occurences() > 1;
    }

    occurence i_occurence::get_occurence_by_rank(int rank_) const
    {
      occurence occ;
      compute_occurence(rank_, occ);
      return occ;
    }

    // Concrete single occurence class:

    occurence::occurence()
    {
      return;
    }

    occurence::~occurence()
    {
      return;
    }

    size_t occurence::get_dimension() const
    {
      return 0;
    }

    size_t occurence::get_number_of_occurences() const
    {
      return 1;
    }

    void occurence::compute_occurence(int rank_, occurence & occ_) const
    {
      DT_THROW_IF(rank_ != 0, std::logic_error, "Invalid rank '" << rank_ << "' !");
      occ_ = *this;
      return;
    }

    size_t occurence::compute_index_path(std::vector<uint32_t> & path_, int rank_) const
    {
      DT_THROW_IF(rank_ != 0, std::logic_error, "Invalid rank '" << rank_ << "' !");
      path_.clear();
      path_.push_back(0);
      return path_.size();
    }

  }  // end of namespace configuration

}  // end of namespace datatools
