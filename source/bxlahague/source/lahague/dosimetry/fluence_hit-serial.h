//! \file   lahague/dosimetry/fluence_hit-serial.h
//! \author François Mauger
//! \brief  Serialization of fluence hit class
//
// Copyright (c) 2015 by François Mauger <mauger@lpccaen.in2p3.fr>
//                       and Arnaud Chapon <achapon@cerap.fr>
//
// This file is part of LaHague.
//
// LaHague is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// LaHague is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with LaHague. If not, see <http://www.gnu.org/licenses/>.

#ifndef LAHAGUE_DOSIMETRY_FLUENCE_HIT_REFLECT_H
#define LAHAGUE_DOSIMETRY_FLUENCE_HIT_REFLECT_H

// Ourselves:
#include <lahague/dosimetry/fluence_hit.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
// - Bayeux/geomtools :
#include <geomtools/base_hit.ipp>

namespace lahague {

  namespace dosimetry {

    template<class Archive>
    void fluence_hit::serialize(Archive & ar_, const unsigned int /* version_ */)
    {
      ar_ & boost::serialization::make_nvp("geomtools__base_hit",
                                           boost::serialization::base_object<geomtools::base_hit>(*this));
      if (_store & STORE_PARTICLE_NAME) {
        ar_ & boost::serialization::make_nvp ("particle_name", _particle_name_);
      }
      if (_store & STORE_ENTRANCE_ENERGY) {
        ar_ & boost::serialization::make_nvp ("entrance_energy", _entrance_energy_);
      }
      if (_store & STORE_H10_STAR) {
        ar_ & boost::serialization::make_nvp ("h10_star", _h10_star_);
      }
      return;
    }

  } // namespace dosimetry

} // namespace lahague

#endif // LAHAGUE_DOSIMETRY_FLUENCE_HIT_REFLECT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
