//! \file   lahague/the_serializable.h
//! \author François Mauger
//! \brief  LaHague serializable classes
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

#ifndef LAHAGUE_THE_SERIALIZABLE_H
#define LAHAGUE_THE_SERIALIZABLE_H

// Third party:
// - Boost :
#include <boost/serialization/export.hpp>
// - Bayeux/datatools :
#include <datatools/archives_instantiation.h>

// This project:
#include <lahague/dosimetry/fluence_hit-serial.h>
DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(lahague::dosimetry::fluence_hit)

// #include <lahague/dosimetry/dose_data-serial.h>
// DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(lahague::dosimetry::dose_data)
// BOOST_CLASS_EXPORT_IMPLEMENT(lahague::dosimetry::dose_data)

#endif // LAHAGUE_THE_SERIALIZABLE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
