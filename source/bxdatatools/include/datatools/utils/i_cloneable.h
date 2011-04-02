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

#include <iostream>
#include <typeinfo>

#include <boost/scoped_ptr.hpp>
#include <datatools/serialization/i_serializable.h>
#include <datatools/utils/caster_utils.h>

namespace datatools {
  
  namespace utils {

    /** This is a pure abstract class that can be
     *  inherited for class cloning service (only for 
     *  copyable objects).
     *
     *  Some macros are provided for declaration and definition 
     *  of a clone method within a class. Should be in public scope.
     *
     *  \b Example:
     *
     *  Header file (my_data.h):
     *  @code
     *  #include <datatools/utils/i_cloneable.h>
     *
     *  class my_data : public datatools::utils::i_cloneable
     *  {
     *    public:
     *      DATATOOLS_CLONEABLE_DECLARATION(my_data)
     *  };
     *  @endcode
     *
     *  Implementation file (my_data.cc):
     *  @code
     *  #include <my_data.h>
     *
     *  DATATOOLS_CLONEABLE_IMPLEMENTATION(my_data) 
     *  @endcode
     *
     * The \t my_data class must have a copy constructor (or a 
     * default copy constructor provided by the compiler).
     *
     */     
    class i_cloneable
    {
    public:

      virtual i_cloneable * clone (void) const = 0;

      virtual ~i_cloneable () {};

    public:
      /** Templatized static method for cloning copyable objects.
       */
      template<class Cloneable>
      static Cloneable * clone_it (const Cloneable & a_cloneable)
      {
	return new Cloneable (a_cloneable);
      }  
    };

  } // end of namespace utils 

} // end of namespace datatools 

#define DATATOOLS_CLONEABLE_DECLARATION(Cloneable)           \
  virtual Cloneable * clone (void) const;                    \
/**/

#define DATATOOLS_CLONEABLE_IMPLEMENTATION(Cloneable)              \
  Cloneable * Cloneable::clone (void) const			   \
  {                                                                \
    return datatools::utils::i_cloneable::clone_it (*this);        \
  }                                                                \
/**/

#define DATATOOLS_CLONEABLE_TO_SERIALIZABLE_CASTER_DECLARATION(Cloneable) \
DATATOOLS_CASTER_DECLARATION(datatools::utils::i_cloneable,datatools::serialization::i_serializable, Cloneable,g_cloneable_to_serializable_caster,get_cloneable_to_serializable_caster) \
/**/

#define DATATOOLS_CLONEABLE_TO_SERIALIZABLE_CASTER_IMPLEMENTATION(Cloneable) \
DATATOOLS_CASTER_IMPLEMENTATION(datatools::utils::i_cloneable,datatools::serialization::i_serializable, Cloneable,g_cloneable_to_serializable_caster,get_cloneable_to_serializable_caster) \
/**/

#endif // __datatools__utils__i_cloneable_h

/* end of i_cloneable.h */
/*
** Local Variables: --
** mode: c++ --
** c-file-style: "gnu" --
** tab-width: 2 --
** End: --
*/
