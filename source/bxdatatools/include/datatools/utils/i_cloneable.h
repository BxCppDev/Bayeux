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
      /** Templatized static method for cloning
       *  copyable objects.
       */
      template<class T>
	static T * clone (const T & a_t)
	{
	  return new T(a_t);
	}  
    };


  } // end of namespace utils 

} // end of namespace datatools 

#define DATATOOLS_CLONEABLE_DECLARATION(T)              \
  virtual T * clone (void) const;                       \
/**/

#define DATATOOLS_CLONEABLE_IMPLEMENTATION(T)           \
  T * T::clone (void) const                             \
  {                                                     \
    return i_cloneable::clone (*this);                  \
  }                                                     \
/**/

#endif // __datatools__utils__i_cloneable_h

/* end of i_cloneable.h */
