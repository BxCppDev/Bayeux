/// \file mctools/signal/utils.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2015-04-10
 * Last modified : 2015-04-10
 *
 * Copyright (C) 2015 Francois Mauger <mauger@lpccaen.in2p3.fr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * Description:
 *
 *   Signal and signal shape utilities.
 *
 * History:
 *
 */

#ifndef MCTOOLS_SIGNAL_UTILS_H
#define MCTOOLS_SIGNAL_UTILS_H

namespace mctools {

  namespace signal {

    enum polarity_type {
      POL_INVALID  = -1, //!< Invalid polarity
      POL_POSITIVE =  0, //!< Positive polarity
      POL_NEGATIVE =  1, //!< Negative polarity
      POL_UNKNOWN  =  2 //!< Unknown polarity
    };

    bool polarity_is_valid(polarity_type);

    bool polarity_is_set(polarity_type);

  } // end of namespace signal

} // end of namespace mctools

#endif // MCTOOLS_SIGNAL_UTILS_H

/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
