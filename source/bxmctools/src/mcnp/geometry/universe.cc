//! \file  mctools/mcnp/geometry/universe.cc
//! \brief Description of MNCP universe
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
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

// Ourselves:
#include <mctools/mcnp/geometry/universe.h>

// Standard library:
#include <sstream>
#include <iomanip>

// Third party:
#include <datatools/utils.h>
#include <datatools/exception.h>

// This project:
// #include <mctools/mcnp/format.h>

namespace mctools {

  namespace mcnp {

    namespace geometry {

      // static
      const int universe::INVALID_ID;
      const int universe::MIN_ID;
      const int universe::MAX_ID;

      universe::universe()
      {
        _id_ = INVALID_ID;
        return;
      }

      universe::~universe()
      {
        reset();
        return;
      }

      bool universe::is_valid() const
      {
        if (_id_ == INVALID_ID) {
          return false;
        }
        return true;
      }

      void universe::set_name(const std::string & name_)
      {
        _name_ = name_;
        return;
      }

      const std::string & universe::get_name() const
      {
        return _name_;
      }

      void universe::set_id(int id_)
      {
        DT_THROW_IF(id_ < MIN_ID || id_ > MAX_ID, std::range_error,
                    "Invalid universe identifier [" << id_ << "]!");
        _id_ = id_;
        return;
      }

      int universe::get_id() const
      {
        return _id_;
      }

      void universe::reset()
      {
        _name_.clear();
        _id_ = INVALID_ID;
        return;
      }

    } // namespace geometry

  } // namespace mcnp

} // namespace mctools
