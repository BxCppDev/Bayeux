/// \file datatools/i_cloneable.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-03-24
 * Last modified : 2013-04-22
 *
 * Copyright (C) 2011-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef DATATOOLS_I_CLONEABLE_H
#define DATATOOLS_I_CLONEABLE_H

// Standard Library:
#include <iostream>
#include <typeinfo>

// This Project:
#include <datatools/caster_utils.h>

namespace datatools {

  class i_serializable; // forward declaration

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
   *  #include <datatools/i_cloneable.h>
   *
   *  class my_data : public datatools::i_cloneable {
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
   * The my_data class must have a copy constructor (or a
   * default copy constructor provided by the compiler).
   *
   */
  class i_cloneable
  {
  public:
    /**
     * A pure virtual member.
     * @return a pointer to a new instance of a cloneable class.
     */
    virtual i_cloneable* clone(void) const = 0;

    /** The virtual destructor
     */
    virtual ~i_cloneable()
    {
      return;
    };

    /** Templatized static method for cloning copyable objects.
     */
    template<class Copyable>
    static Copyable* clone_it(const Copyable& a_copyable)
    {
      return new Copyable(a_copyable);
    }

    template<class CandidateType>
    bool is_cloneable(const CandidateType & candidate_)
    {
      const datatools::i_cloneable * tc = dynamic_cast<const datatools::i_cloneable *>(&candidate_);
      return tc != nullptr;
    }

  };

} // end of namespace datatools

//----------------------------------------------------------------------
// i_cloneable macros

#define DATATOOLS_CLONEABLE_DECLARATION(Copyable) \
  public:                                         \
  virtual Copyable* clone(void) const override;            \
  /**/

#define DATATOOLS_CLONEABLE_INLINE(Copyable)                  \
  public:                                                     \
  virtual Copyable* clone(void) const override {                       \
    return datatools::i_cloneable::clone_it<Copyable>(*this); \
  }
/**/

#define DATATOOLS_CLONEABLE_IMPLEMENTATION(Copyable)          \
  Copyable* Copyable::clone(void) const {                     \
    return datatools::i_cloneable::clone_it<Copyable>(*this); \
  }                                                           \
  /**/

#define DATATOOLS_CLONEABLE_TO_SERIALIZABLE_CASTER_DECLARATION(Cloneable) \
  DATATOOLS_CASTER_DECLARATION(datatools::i_cloneable,datatools::i_serializable, Cloneable,g_cloneable_to_serializable_caster,get_cloneable_to_serializable_caster) \
  /**/

#define DATATOOLS_CLONEABLE_TO_SERIALIZABLE_CASTER_IMPLEMENTATION(Cloneable) \
  DATATOOLS_CASTER_IMPLEMENTATION(datatools::i_cloneable,datatools::i_serializable, Cloneable,g_cloneable_to_serializable_caster,get_cloneable_to_serializable_caster) \
  /**/

#endif // DATATOOLS_I_CLONEABLE_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
