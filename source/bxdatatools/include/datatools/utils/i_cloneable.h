/* datatools/utils/i_cloneable.h */
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

//! \file datatools/utils/i_cloneable.h
#ifndef DATATOOLS_UTILS_I_CLONEABLE_H_
#define DATATOOLS_UTILS_I_CLONEABLE_H_
// Standard Library
#include <iostream>
#include <typeinfo>

// Third Party
// - A

// This Project
#include <datatools/utils/caster_utils.h>


namespace datatools {

namespace serialization {
class i_serializable; // forward declaration
}

namespace utils {
//! \brief A pure abstract class (interface) for inherited cloneable classes.
/**
 * This is a pure abstract class that can be
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
 *  class my_data : public datatools::utils::i_cloneable {
 *   public:
 *    DATATOOLS_CLONEABLE_DECLARATION(my_data)
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
class i_cloneable {
 public:
  /**
   * A pure virtual member.
   * @return a pointer to a new instance of a cloneable class.
   */
  virtual i_cloneable* clone(void /*something*/) const = 0;

  /** The virtual destructor
  */
  virtual ~i_cloneable() {};

 public:
  /** Templatized static method for cloning copyable objects.
  */
  template<class Copyable>
  static Copyable* clone_it(const Copyable& a_copyable) {
    return new Copyable (a_copyable);
  }  
};

} // end of namespace utils 
} // end of namespace datatools 


//----------------------------------------------------------------------
// i_cloneable macros

#define DATATOOLS_CLONEABLE_DECLARATION(Cloneable)	\
    virtual Cloneable* clone (void) const;						\
/**/

#define DATATOOLS_CLONEABLE_IMPLEMENTATION(Copyable)				\
  Copyable* Copyable::clone (void) const {								\
    return datatools::utils::i_cloneable::clone_it (*this);	\
  }																													\
/**/

#define DATATOOLS_CLONEABLE_TO_SERIALIZABLE_CASTER_DECLARATION(Cloneable) \
    DATATOOLS_CASTER_DECLARATION(datatools::utils::i_cloneable,datatools::serialization::i_serializable, Cloneable,g_cloneable_to_serializable_caster,get_cloneable_to_serializable_caster) \
/**/

#define DATATOOLS_CLONEABLE_TO_SERIALIZABLE_CASTER_IMPLEMENTATION(Cloneable) \
    DATATOOLS_CASTER_IMPLEMENTATION(datatools::utils::i_cloneable,datatools::serialization::i_serializable, Cloneable,g_cloneable_to_serializable_caster,get_cloneable_to_serializable_caster) \
/**/


#endif // DATATOOLS_UTILS_I_CLONEABLE_H_

