//! \file  mctools/mcnp/units.cc
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

// Ourselves:
#include <mctools/mcnp/units.h>

// Third party:
// - Bayeux/datatools:
#include <datatools/units.h>
#include <datatools/clhep_units.h>

namespace mctools {

  namespace mcnp {

    // static
    void units::register_units()
    {

      datatools::units::registry::system_registry().registration(datatools::units::unit("shake",
                                                                                        "time",
                                                                                        shake()));

      datatools::units::registry::system_registry().registration(datatools::units::unit("cm/shake",
                                                                                        "velocity",
                                                                                        velocity_unit()));



      return;
    }

    // static
    double units::shake()
    {
      static const double _shake = 10.0 * CLHEP::nanosecond;
      return _shake;
    }

    // static
    double units::length_unit()
    {
      return CLHEP::cm;
    }

    // static
    double units::surface_unit()
    {
      return CLHEP::cm2;
    }

    // static
    double units::volume_unit()
    {
      return CLHEP::cm3;
    }

    // static
    double units::energy_unit()
    {
      return CLHEP::MeV;
    }

    // static
    double units::time_unit()
    {
      return shake();
    }

    // static
    double units::velocity_unit()
    {
      return CLHEP::cm / shake();
    }

    // static
    double units::density_unit()
    {
      return CLHEP::g / CLHEP::cm3;
    }

  } // namespace mcnp

} // namespace mctools
