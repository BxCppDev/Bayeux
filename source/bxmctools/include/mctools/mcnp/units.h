//! \file  mctools/mcnp/units.h
//! \brief Description of MNCP units
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

#ifndef MCTOOLS_MCNP_UNITS_H
#define MCTOOLS_MCNP_UNITS_H

// Standard Library:
#include <string>

namespace mctools {

  namespace mcnp {

    //! \brief Unit support
    class units
    {
    public:

      //! Register MCNP specific units
      static void register_units();

      //! Return the length unit
      static double length_unit();

      //! Return the surface unit
      static double surface_unit();

      //! Return the volume unit
      static double volume_unit();

      //! Return the energy unit
      static double energy_unit();

      //! Return the time unit
      static double time_unit();

      //! Return the value of the shake time unit
      static double shake();

      //! Return the velocity unit
      static double velocity_unit();

      //! Return the density unit
      static double density_unit();

    };

  } // namespace mcnp

} // namespace mctools

#endif // MCTOOLS_MCNP_UNITS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
