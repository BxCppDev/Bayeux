// datatools/configuration/array_occurrence.cc
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
#include <datatools/configuration/array_occurrence.h>

// This project:
#include <datatools/exception.h>
#include <datatools/configuration/single_occurrence.h>

namespace datatools {

  namespace configuration {

    void array_occurrence::set_size(unsigned int sz_)
    {
      _size_ = sz_;
      return;
    }

    unsigned int array_occurrence::get_size() const
    {
      return _size_;
    }

    void array_occurrence::set_start_id(int sid_)
    {
      _start_id_ = sid_;
      return;
    }

    int array_occurrence::get_start_id() const
    {
      return _start_id_;
    }

    array_occurrence::array_occurrence()
    {
      _size_ = 0;
      _start_id_ = INVALID_ID;
      return;
    }

    array_occurrence::~array_occurrence()
    {
      return;
    }

    size_t array_occurrence::get_dimension() const
    {
      return 1;
    }

    size_t array_occurrence::get_number_of_occurrences() const
    {
      return _size_;
    }

    void array_occurrence::compute_occurrence(int rank_, single_occurrence & occ_) const
    {
      DT_THROW_IF(rank_ < 0 || rank_ >= (int)_size_,
                  std::range_error, "Invalid rank '" << rank_ << "' !");
      int sid = 0;
      if (_start_id_ != INVALID_ID) {
        sid = _start_id_;
      }
      occ_.set_id(sid + rank_);
      return;
    }

    size_t array_occurrence::compute_index_path(std::vector<uint32_t> & path_,
                                               int rank_) const
    {
      DT_THROW_IF(rank_ < 0 || rank_ >= (int)_size_,
                  std::range_error, "Invalid rank '" << rank_ << "' !");
      path_.clear();
      int sid = 0;
      if (_start_id_ != INVALID_ID) {
        sid = _start_id_;
      }
      path_.push_back(sid + rank_);
      return path_.size();
    }

    std::string array_occurrence::to_string() const
    {
      std::ostringstream oss;
      oss << "array";
      oss << ' ' << "size=" << _size_ << ' ' << "start_id=" << _start_id_;
      return oss.str();
    }

  }  // end of namespace configuration

}  // end of namespace datatools
