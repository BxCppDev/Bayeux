//! \file  mctools/mcnp/mcsetup.h
//! \brief Description of MNCP simulation setup
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

#ifndef MCTOOLS_MCNP_MCSETUP_H
#define MCTOOLS_MCNP_MCSETUP_H

// Standard library:
#include <string>

namespace mctools {

  namespace mcnp {

    //! \brief Monte-Carlo setup
    class mcsetup
    {
    public:

      //! Default constructor
      mcsetup();

      //! Destructor
      ~mcsetup();

      //! Set the title
      void set_title(const std::string &);

      //! Return the title
      const std::string & get_title() const;

      //! Reset
      void reset();

    private:

      std::string _title_;

    };

  } // namespace mcnp

} // namespace mctools

#endif // MCTOOLS_MCNP_MCSETUP_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
