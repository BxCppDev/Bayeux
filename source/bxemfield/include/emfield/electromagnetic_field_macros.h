/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2012-04-24
 * Last modified : 2013-02-24
 *
 * Copyright (C) 2012-2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 *
 *
 * Description:
 *
 *   Macros to help build electromagnetic field classes.
 *
 * History:
 *
 */

/*! \file emfield/electromagnetic_field_macros.h
 * \brief Macros to automatically generate some parts of the electromagnetic
 *        fields interface.
 *
 */

#ifndef EMFIELD_ELECTROMAGNETIC_FIELD_MACROS_H
#define EMFIELD_ELECTROMAGNETIC_FIELD_MACROS_H 1

// Ourselves:
#include <emfield/base_electromagnetic_field.h>

/* #define EMFIELD_CLASS_DECLARE(T)                          \ */
/*   class T : public ::emfield::base_electromagnetic_field  \ */


/* #define EMFIELD_INITIALIZE_DECLARE()                                                  \ */
/*   public:                                                                             \ */
/*   virtual void initialize (const ::datatools::properties &,                           \ */
/*                            ::datatools::service_manager &,                            \ */
/*                            ::emfield::base_electromagnetic_field::field_dict_type &); \ */


/* #define EMFIELD_INITIALIZE_IMPLEMENT_HEAD(T,CONF,SERVICE_MANAGER,DICT)               \ */
/*   void T::initialize (const ::datatools::properties & CONF,                          \ */
/*                       ::datatools::service_manager & SERVICE_MANAGER,                \ */
/*                       ::emfield::base_electromagnetic_field::field_dict_type & DICT) \ */


/* #define EMFIELD_RESET_DECLARE() \ */
/*   public:                       \ */
/*   virtual void reset ();        \ */


/* #define EMFIELD_RESET_IMPLEMENT_HEAD(T) \ */
/*   void T::reset ()                      \ */


/* #define EMFIELD_COMPUTE_EFIELD_DECLARE()                                               \ */
/*   public :                                                                             \ */
/*   virtual int compute_electric_field (const ::geomtools::vector_3d & position_,        \ */
/*                                       double time_,                                    \ */
/*                                       ::geomtools::vector_3d & electric_field_) const; \ */


/* #define EMFIELD_COMPUTE_BFIELD_DECLARE()                                             \ */
/*   public :                                                                           \ */
/*   virtual int compute_magnetic_field (const ::geomtools::vector_3d & position_,      \ */
/*                                       double time_,                                  \ */
/*                                       geomtools::vector_3d & magnetic_field_) const; \ */


/* #define EMFIELD_COMPUTE_EBFIELD_DECLARE()                                                     \ */
/*   public :                                                                                    \ */
/*   virtual int compute_electromagnetic_field (const ::geomtools::vector_3d & position_,        \ */
/*                                              double time_,                                    \ */
/*                                              ::geomtools::vector_3d & electric_field_,        \ */
/*                                              ::geomtools::vector_3d & magnetic_field_) const; \ */


/* #define EMFIELD_COMPUTE_EFIELD_IMPLEMENT_HEAD(T,Position,Time,EField)     \ */
/*   int T::compute_electric_field (const ::geomtools::vector_3d & Position, \ */
/*                                  double Time,                             \ */
/*                                  ::geomtools::vector_3d & EField) const   \ */


/* #define EMFIELD_COMPUTE_BFIELD_IMPLEMENT_HEAD(T,Position,Time,BField)     \ */
/*   int T::compute_magnetic_field (const ::geomtools::vector_3d & Position, \ */
/*                                  double Time,                             \ */
/*                                  ::geomtools::vector_3d & BField) const   \ */


/* #define EMFIELD_COMPUTE_EBFIELD_IMPLEMENT_HEAD(T,Position,Time,EField,BField)    \ */
/*   int T::compute_electromagnetic_field (const ::geomtools::vector_3d & Position, \ */
/*                                         double Time,                             \ */
/*                                         ::geomtools::vector_3d & EField,         \ */
/*                                         ::geomtools::vector_3d & BField) const   \ */


/* #define EMFIELD_CONSTRUCTOR_DECLARE(T) \ */
/*   public:                              \ */
/*   T (uint32_t = 0);                    \ */


/* #define EMFIELD_CONSTRUCTOR_IMPLEMENT_HEAD(T,Flags) \ */
/*   T::T (uint32_t Flags)                             \ */
/*     : ::emfield::base_electromagnetic_field (Flags) \ */


/* #define EMFIELD_DESTRUCTOR_DECLARE(T) \ */
/*   public:                             \ */
/*   virtual ~T ();                      \ */


/* #define EMFIELD_DESTRUCTOR_IMPLEMENT_HEAD(T) \ */
/*   T::~T ()                                   \ */


/* #define EMFIELD_DEFAULT_DESTRUCTOR_IMPLEMENT(T) \ */
/*   EMFIELD_DESTRUCTOR_IMPLEMENT_HEAD (T)         \ */
/*   {                                             \ */
/*     if (is_initialized()) reset();              \ */
/*     return;                                     \ */
/*   }                                             \ */


/* /// Macro for the declaration of the minimal interface */
/* #define EMFIELD_INTERFACE()         \ */
/*   EMFIELD_INITIALIZE_DECLARE()     \ */
/*   EMFIELD_RESET_DECLARE()          \ */
/*   EMFIELD_COMPUTE_EFIELD_DECLARE() \ */
/*   EMFIELD_COMPUTE_BFIELD_DECLARE() \ */


/* /// Macro for the declaration of the minimal interface */
/* #define EMFIELD_INTERFACE_CTOR_DTOR(EMFIELD_CLASS_NAME) \ */
/*   EMFIELD_CONSTRUCTOR_DECLARE(EMFIELD_CLASS_NAME)      \ */
/*   EMFIELD_DESTRUCTOR_DECLARE(EMFIELD_CLASS_NAME)       \ */
/*   EMFIELD_INTERFACE ()                                 \ */


/* Macros for interface/implementation of static creator methods in EM field classes */

/* Registration */

#define EMFIELD_REGISTRATION_INTERFACE(EMFIELD_CLASS_NAME)              \
  private:                                                              \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_INTERFACE(::emfield::base_electromagnetic_field,EMFIELD_CLASS_NAME) \
  public:                                                               \
  std::string get_class_id() const override;                             \


#define EMFIELD_REGISTRATION_IMPLEMENT(EMFIELD_CLASS_NAME,EMFIELD_ID)   \
  DATATOOLS_FACTORY_SYSTEM_AUTO_REGISTRATION_IMPLEMENTATION(::emfield::base_electromagnetic_field,EMFIELD_CLASS_NAME,EMFIELD_ID) \
  std::string EMFIELD_CLASS_NAME::get_class_id() const {                \
    return std::string(EMFIELD_ID);                                     \
  }                                                                     \


#endif // EMFIELD_ELECTROMAGNETIC_FIELD_MACROS_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
