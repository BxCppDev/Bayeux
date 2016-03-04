//! \file   lahague/dosimetry/fluence_hit.h
//! \author François Mauger
//! \brief  Information about fluence associated to a physical volume
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

#ifndef LAHAGUE_DOSIMETRY_FLUENCE_HIT_H
#define LAHAGUE_DOSIMETRY_FLUENCE_HIT_H

// Standard Library:
#include <string>

// Third party:
// - Boost :
#include <boost/serialization/access.hpp>
// - Bayeux/datatools :
#include <datatools/bit_mask.h>
// - Bayeux/geomtools :
#include <geomtools/geomtools_config.h>
#include <geomtools/utils.h>
#include <geomtools/base_hit.h>

namespace lahague {

  namespace dosimetry {

    //! \brief A hit in a physical volume with associated fluence and dose
    /**
     *  +-------------+
     *  |             | particle
     *  |            <-----------
     *  |             | energy E
     *  | volume (GID)|
     *  +-------------+
     *
     */
    class fluence_hit : public geomtools::base_hit
    {
    public:

      /// \brief Masks to automatically tag the attributes to be stored
      enum store_mask_type {
        STORE_PARTICLE_NAME = datatools::bit_mask::bit03,   //!< Serialization mask for the particle name
        STORE_ENTRANCE_ENERGY = datatools::bit_mask::bit04, //!< Serialization mask for the entrance energy
        STORE_H10_STAR = datatools::bit_mask::bit05         //!< Serialization mask for the H*(10)
      };

      //! Default constructor
      fluence_hit();

      //! Destructor
      ~fluence_hit();

      //! Check the validity
      bool is_valid() const;

      //! Invalidate
      void invalidate();

      //! Reset
      void reset();

      //! Return the particle name
      const std::string & get_particle_name() const;

      //! Set the particle name
      void set_particle_name(const std::string &);

      //! Invalidate the particle name
      void invalidate_particle_name();

      //! Return the entrance energy
      double get_entrance_energy() const;

      //! Set the entrance energy
      void set_entrance_energy(double);

      //! Invalidate the entrance energy
      void invalidate_entrance_energy();

      //! Return the H*(10)
      double get_h10_star() const;

      //! Set the H*(10)
      void set_h10_star(double);

      //! Invalidate the H*(10)
      void invalidate_h10_star();

      //! Smart print
      virtual void tree_dump(std::ostream & out_ = std::clog,
                             const std::string & title_ = "",
                             const std::string & indent_ = "",
                             bool inherit_ = false) const;

    private:

      std::string        _particle_name_;   //!< The particle name
      double             _entrance_energy_; //!< The particle energy at volume entrance
      double             _h10_star_;        //!< H*(10)

      //! Serialization interface
      DATATOOLS_SERIALIZATION_DECLARATION()

    };

  } // namespace dosimetry

} // namespace lahague

#endif // LAHAGUE_DOSIMETRY_FLUENCE_HIT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
