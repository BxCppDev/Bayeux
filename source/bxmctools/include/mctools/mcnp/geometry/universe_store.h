//! \file  mctools/mcnp/geometry/universe_store.h
//! \brief MNCP universe store
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

#ifndef MCTOOLS_MCNP_GEOMETRY_UNIVERSE_STORE_H
#define MCTOOLS_MCNP_GEOMETRY_UNIVERSE_STORE_H

// Standard library:
#include <string>
#include <map>
#include <set>

// Third party:
// - Boost:
#include <boost/cstdint.hpp>

// This project:
#include <mctools/mcnp/geometry/universe.h>

namespace mctools {

  namespace mcnp {

    namespace geometry {

      //! \brief Dictionary of universes
      typedef std::map<int, universe_ptr_type> universe_dict_type;

      //! \brief Store of universes
      class universe_store
      {
      public:

        //! Default constructor
        universe_store();

        //! Destructor
        ~universe_store();

        //! Reset
        void reset();

        //! Check universe by identifier
        bool has_universe(int id_) const;

        //! Add a universe with identifier
        universe & add_universe(int id_ = 0);

        //! Remove universe with identifier
        void remove_universe(int id_);

        //! Return the universe pointer with identifier
        const universe_ptr_type & get(int id_) const;

        //! Return the number of universes
        uint32_t size() const;

        //! Build the set of universe identifiers
        void build_ids(std::set<int> & ids_) const;

        //! Print MCNP card
        void print_card(std::ostream & out_, uint32_t flags_ = 0) const;

      public:

        //! Compute a candidate unused universe identifier
        int _candidate_id();

      private:

        universe_dict_type _universes_; //!< Dictionary of universes

      };

    } // namespace geometry

  } // namespace mcnp

} // namespace mctools

#endif // MCTOOLS_MCNP_GEOMETRY_UNIVERSE_STORE_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
