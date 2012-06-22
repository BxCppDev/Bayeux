/* datatools/utils/range_tools.h
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
#ifndef DATATOOLS_UTILS_RANGE_TOOLS_H_
#define DATATOOLS_UTILS_RANGE_TOOLS_H_

namespace datatools {
namespace utils {
namespace range {
// Enum?
static const char undefined  = -1;
static const char unbound    = 0;
static const char included   = 1;
static const char excluded   = 2;

static const char token_open      = '[';
static const char token_close     = ']';
static const char token_separator = ';';

} // end of namespace range 
} // end of namespace utils 
} // end of namespace datatools 

#endif // DATATOOLS_UTILS_RANGE_TOOLS_H_

