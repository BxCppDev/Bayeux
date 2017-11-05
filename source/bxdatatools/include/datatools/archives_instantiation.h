/// \file datatools/archives_instantiation.h
/*
 * Description :
 *
 * Define some macros to instantiate template serialization methods
 * for all archives supported by datatools.
 *
 * Copyright (C) 2011-2015 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
 */

#ifndef DATATOOLS_ARCHIVES_INSTANTIATION_H
#define DATATOOLS_ARCHIVES_INSTANTIATION_H

// Datatools
#include <datatools/archives_list.h>

/// Macro to generate the instantiation declaration of the intrusive serialization method for a given archive
/** T is the class name
 *  A is the archive name
 */
#define DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE(T,A)  \
  template void T::serialize(A& ar, const unsigned int version);  \
  /**/

/// Macro to generate the instantiation declarations of the intrusive serialization methods for all supported input archives
/** T is the class name
 */
#define DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL_IN(T)   \
  DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::polymorphic_iarchive) \
  DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::text_iarchive) \
  DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::xml_iarchive) \
  DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE(T,eos::portable_iarchive) \
  /**/

/// Macro to generate the instantiation declarations of the intrusive serialization methods for all supported output archives
/** T is the class name
 */
#define DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL_OUT(T)  \
  DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::polymorphic_oarchive) \
  DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::text_oarchive) \
  DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::xml_oarchive) \
  DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE(T,eos::portable_oarchive) \
  /**/

/// Macro to generate the instantiation declarations of the intrusive serialization methods for all supported intput and output archives
/** T is the class name
 */
#define DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL(T)  \
  DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL_IN(T)     \
  DATATOOLS_SERIALIZATION_CLASS_SERIALIZE_INSTANTIATE_ALL_OUT(T)    \
  /**/

/// Macro to generate the instantiation declaration of the non intrusive serialization function for a given archive
/** T is the class name
 *  A is the archive name
 */
#define DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE(T,A) \
  template void boost::serialization::serialize(A& ar, T& object, const unsigned int version); \
  /**/

/// Macro to generate the instantiation declarations of the non intrusive serialization functions for all supported input archives
/** T is the class name
 */
#define DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE_ALL_IN(T) \
  DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::polymorphic_iarchive) \
  DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::text_iarchive) \
  DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::xml_iarchive) \
  DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE(T,eos::portable_iarchive) \
  /**/

/// Macro to generate the instantiation declarations of the non intrusive serialization functions for all supported output archives
/** T is the class name
 */
#define DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE_ALL_OUT(T) \
  DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::polymorphic_oarchive) \
  DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::text_oarchive) \
  DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE(T,boost::archive::xml_oarchive) \
  DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE(T,eos::portable_oarchive) \
  /**/

/// Macro to generate the instantiation declarations of the non intrusive serialization functions for all supported input and output archives
/** T is the class name
 */
#define DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE_ALL(T) \
  DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE_ALL_IN(T) \
  DATATOOLS_SERIALIZATION_NON_INTRUSIVE_CLASS_SERIALIZE_INSTANTIATE_ALL_OUT(T) \
  /**/

#endif // DATATOOLS_ARCHIVES_INSTANTIATION_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
