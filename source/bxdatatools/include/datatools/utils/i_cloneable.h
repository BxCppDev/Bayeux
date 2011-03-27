/* i_cloneable.h */
//! \file datatools/utils/cloneable.h
/* Author(s)     :     Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-03-24
 * Last modified : 2011-03-26
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
 *  An abstract interface for cloneable objects
 *
 */

#ifndef __datatools__utils__i_cloneable_h
#define __datatools__utils__i_cloneable_h 1

namespace datatools {

  namespace utils {

    class i_cloneable
    {
    public:
      virtual i_cloneable * clone (void) const = 0;
    };

  } // end of namespace utils 

} // end of namespace datatools 

#endif // __datatools__utils__i_cloneable_h

/* end of i_cloneable.h */
