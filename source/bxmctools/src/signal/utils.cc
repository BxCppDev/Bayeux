// utils.cc
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2015-04-10
 * Last modified : 2016-09-27
 *
 * Copyright (C) 2015 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
#include <mctools/signal/utils.h>

namespace mctools {

  namespace signal {

    bool polarity_is_valid(polarity_type pol_)
    {
      if (pol_ == POLARITY_INVALID) return false;
      return true;
    }

    bool polarity_is_set(polarity_type pol_)
    {
      if (pol_ == POLARITY_POSITIVE) return true;
      if (pol_ == POLARITY_NEGATIVE) return true;
      return false;
    }

    const datatools::units::unit & default_time_unit()
    {
      return datatools::units::registry::const_system_registry().get_unit_from_any("ns");
    }

    const datatools::units::unit & default_voltage_unit()
    {
      return datatools::units::registry::const_system_registry().get_unit_from_any("volt");
    }

    const datatools::units::unit & default_charge_unit()
    {
      return datatools::units::registry::const_system_registry().get_unit_from_any("nV.s");
    }

  } // end of namespace signal

} // end of namespace mctools
