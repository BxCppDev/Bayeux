//! \file   lahague/dosimetry/dose_data-serial.h
//! \author François Mauger
//! \brief  Serialization of the dose_data class
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

#ifndef LAHAGUE_DOSIMETRY_DOSE_DATA_SERIAL_H
#define LAHAGUE_DOSIMETRY_DOSE_DATA_SERIAL_H

// Ourselves:
#include <lahague/dosimetry/dose_data.h>

// Third party:
// - Boost:
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/nvp.hpp>
// - Bayeux/datatools :
#include <datatools/i_serializable.ipp>
#include <datatools/properties.ipp>
#include <boost/serialization/vector.hpp>
// - Bayeux/geomtools :
#include <geomtools/utils.ipp>

// This project:
// #include <mctools/base_step_hit.ipp>

namespace lahague {

  namespace dosimetry {

  template<class Archive>
    void dose_data::serialize(Archive & ar_, const unsigned int /* version_ */)
  {
    ar_ & DATATOOLS_SERIALIZATION_I_SERIALIZABLE_BASE_OBJECT_NVP;
    ar_ & boost::serialization::make_nvp ("volume_infos", _volume_infos_);
    return;
  }

  } // namespace dosimetry

} // namespace lahague

#endif // LAHAGUE_DOSIMETRY_DOSE_DATA_SERIAL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
