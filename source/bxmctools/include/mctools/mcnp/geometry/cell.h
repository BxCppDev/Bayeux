//! \file  mctools/mcnp/geometry/cell.h
//! \brief Description of MNCP cell
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

#ifndef MCTOOLS_MCNP_GEOMETRY_CELL_H
#define MCTOOLS_MCNP_GEOMETRY_CELL_H

// Standard library:
#include <string>
#include <iostream>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

// This project:
#include <mctools/mcnp/geometry/surface_type.h>
#include <mctools/mcnp/utils.h>

namespace mctools {

  namespace mcnp {

    namespace geometry {

      class cell;

      //! Shared pointer on a cell
      typedef boost::shared_ptr<cell> cell_ptr_type;

      //! \brief MCNP cell
      class cell
      {
      public:

        //! Invalid cell identifier
        static const int INVALID_ID =     -1;
        static const int MIN_ID     =      1;
        static const int MAX_ID     =  99999;

        //! Default constructor
        cell();

        //! Destructor
        ~cell();

        //! Set the name
        void set_name(const std::string &);

        //! Return the name
        const std::string & get_name() const;

        //! Set the identifier
        void set_id(int);

        //! Return the identifier
        int get_id() const;

        //! Reset
        void reset();

        //! Check if cell is valid
        bool is_valid() const;

        //! Print MCNP card
        void print_card(std::ostream & out_, uint32_t flags_ = 0) const;

      private:

        std::string  _name_;  //!< Name of the cell
        int          _id_;    //!< Identifier of the cell
  // Material
  // Combinaison de régions

      };

    } // namespace geometry

  } // namespace mcnp

} // namespace mctools

#endif // MCTOOLS_MCNP_GEOMETRY_CELL_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
