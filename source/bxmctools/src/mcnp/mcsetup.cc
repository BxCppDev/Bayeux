//! \file  mctools/mcnp/mcsetup.cc
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
#include <mctools/mcnp/mcsetup.h>

namespace mctools {

  namespace mcnp {

    mcsetup::mcsetup()
    {
      return;
    }

    mcsetup::~mcsetup()
    {
      reset();
      return;
    }

    void mcsetup::set_title(const std::string & title_)
    {
      _title_ = title_;
      return;
    }

    const std::string & mcsetup::get_title() const
    {
      return _title_;
    }

    void mcsetup::reset()
    {
      _title_.clear();
      return;
    }

  } // namespace mcnp

} // namespace mctools
