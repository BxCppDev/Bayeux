//! \file    mctools/mcnp/particle.h
//! \brief   Description of MNCP particle
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

#ifndef MCTOOLS_MCNP_PARTICLE_H
#define MCTOOLS_MCNP_PARTICLE_H

// Standard Library:
#include <string>

namespace mctools {

  namespace mcnp {

    //! \brief Particle description
    class particle
    {
    public:

      //! \brief Particle identifier
      enum particle_id_type {
  INVALID  = -1; //!< Invalid particle
  NEUTRON  =  0, //!< Neutron
  PHOTON   =  1, //!< Photon/gamma/X
  ELECTRON =  2  //!< Electron
      };

    };

  } // namespace mcnp

} // namespace mctools

#endif // MCTOOLS_MCNP_PARTICLE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
