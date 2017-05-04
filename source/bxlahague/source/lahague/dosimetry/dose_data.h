//! \file   lahague/dosimetry/dose_data.h
//! \author François Mauger
//! \brief  Information about dose associated to a simulated event
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

#ifndef LAHAGUE_DOSIMETRY_DOSE_DATA_H
#define LAHAGUE_DOSIMETRY_DOSE_DATA_H

// Standard Library:
#include <iostream>
#include <vector>

// Third party:
// - Boost :
#include <boost/serialization/access.hpp>
#include <boost/cstdint.hpp>
// - Bayeux/datatools :
#include <datatools/i_serializable.h>
#include <datatools/i_tree_dump.h>

namespace lahague {

  namespace dosimetry {

    //! \brief Dose data associated to simulated data
    class dose_data :
      DATATOOLS_SERIALIZABLE_CLASS,
      public datatools::i_tree_dumpable
    {

    public:

      //! Default constructor
      dose_data();

      //! Destructor
      virtual ~dose_data();

      //! Reset the internal data
      virtual void clear();

      //! Smart print
      virtual void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_ = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const;
    private:

      // Data:
      int _volume_infos_;

      //! Serialization interface
      DATATOOLS_SERIALIZATION_DECLARATION();

    };

  } // namespace dosimetry

} // namespace lahague

#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT_KEY2(lahague::dosimetry::dose_data, "lahague::dosimetry::dose_data")

#endif // LAHAGUE_DOSIMETRY_DOSE_DATA_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
