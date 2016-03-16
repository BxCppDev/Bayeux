//! \file  mctools/mcnp/utils.h
//! \brief MNCP utils
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

#ifndef MCTOOLS_MCNP_UTILS_H
#define MCTOOLS_MCNP_UTILS_H

// Standard Library:
#include <string>
#include <vector>

// Third party:
#include <boost/any.hpp>

namespace mctools {

  namespace mcnp {

      struct parameter_type {
        std::string name;
        boost::any  value;
        std::string dimension;
      };

      typedef std::vector<parameter_type> parameter_array_type;

  } // namespace mcnp

} // namespace mctools

#endif // MCTOOLS_MCNP_UTILS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
