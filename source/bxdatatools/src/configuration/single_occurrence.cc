// datatools/configuration/single_occurrence.cc
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
#include <datatools/configuration/single_occurrence.h>

// Standard library:
#include <string>
#include <vector>

// This project (Bayeux/datatools):
#include <datatools/exception.h>

namespace datatools {

  namespace configuration {

    void single_occurrence::set_id(int id_)
    {
      _id_ = id_;
      return;
    }

    int single_occurrence::get_id() const
    {
      return _id_;
    }

    single_occurrence::single_occurrence()
    {
      _id_ = INVALID_ID;
      return;
    }

    single_occurrence::~single_occurrence()
    {
      return;
    }

    size_t single_occurrence::get_dimension() const
    {
      return 0;
    }

    size_t single_occurrence::get_number_of_occurrences() const
    {
      return 1;
    }

    void single_occurrence::compute_occurrence(int rank_, single_occurrence & occ_) const
    {
      DT_THROW_IF(rank_ != 0, std::range_error, "Invalid rank '" << rank_ << "' !");
      occ_ = *this;
      occ_.set_id(SINGLE_ID);
      return;
    }

    size_t single_occurrence::compute_index_path(std::vector<uint32_t> & path_, int rank_) const
    {
      DT_THROW_IF(rank_ != 0, std::range_error, "Invalid rank '" << rank_ << "' !");
      path_.clear();
      return path_.size();
    }

    std::string single_occurrence::to_string() const
    {
      std::ostringstream oss;
      oss << "single";
      return oss.str();
    }

  }  // end of namespace configuration

}  // end of namespace datatools
