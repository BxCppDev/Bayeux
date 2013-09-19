/* datatools/range_tools.h
 * Author(s):     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date: 2011-09-25
 * Last modified: 2011-09-25
 *
 * License:
 *
 * Copyright (C) 2011 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *   Range tools
 *
 * History:
 *
 */
#ifndef DATATOOLS_RANGE_TOOLS_H_
#define DATATOOLS_RANGE_TOOLS_H_

#include <datatools/bit_mask.h>

namespace datatools {

enum range_bound_info_type {
  range_bound_undefined = -1,
  range_bound_unbound   = 0,
  range_bound_included  = 1,
  range_bound_excluded  = 2
};

//! \brief Utility that hosts constants relative to range and interval objects.
 struct range_tools {
   static const char token_open      = '[';
   static const char token_close     = ']';
   static const char token_separator = ';';
 };

} // end of namespace datatools

#endif // DATATOOLS_RANGE_TOOLS_H_

