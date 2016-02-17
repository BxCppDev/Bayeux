//! \file  mctools/mcnp/format.h
//! \brief MNCP input file formating
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

#ifndef MCTOOLS_MCNP_FORMAT_H
#define MCTOOLS_MCNP_FORMAT_H

// Standard library:
#include <string>
#include <iostream>

// Third party:
#include <boost/cstdint.hpp>

namespace mctools {

  namespace mcnp {

    //! \brief MCNP input configuration file formating tools
    class format
    {
    public:

      static const uint32_t MAX_LINE_WIDTH  = 80;
      static const uint32_t HEAD_LINE_WIDTH = 5;

      enum card_flag {
  CARD_COMMENT = 0x1, //!< Comment flag
      };

      static void add_comment_line(std::ostream & out_, const std::string & line_);

      static void add_comment_endl(std::ostream & out_, const std::string & comment_);

      static void format_line(std::ostream & out_, const std::string & line_);

    };

  } // namespace mcnp

} // namespace mctools

#endif // MCTOOLS_MCNP_FORMAT_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
