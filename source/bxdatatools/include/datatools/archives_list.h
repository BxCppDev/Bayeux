/// \file datatools/archives_list.h
/*
 * Description :
 *
 *  Convenience header for listing headers for supported archives.
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

#ifndef DATATOOLS_ARCHIVES_LIST_H
#define DATATOOLS_ARCHIVES_LIST_H

// Wrap Boost's archive/polymorphic_iarchive header
// This header, indirectly, causes "unused parameter" warnings from
// a transient include of boost/serialization/smart_cast.hpp.
// This should be an innocuous warning, so remove diagnostic for this
// header only.
// We only use clang pragmas for now because GCC's are highly version
// dependent - so need a bit more thought.
// To be removed when Boost fix their headers...
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#endif
#include <boost/archive/polymorphic_iarchive.hpp>
#include <datatools/eos/portable_iarchive.hpp>
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#include <boost/archive/polymorphic_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

// 2012-06-07 FM : switch to Portable Archive v5.0
#include <datatools/eos/portable_oarchive.hpp>

/// Top-level namespace of the portable binary archives
namespace eos
{
}

/// Top-level namespace of the Boost library
namespace boost
{
  /// Nested namespace of the Boost/archives library
  namespace archive
  {
  }

  /// Nested namespace of the Boost/Serialization library
  namespace serialization
  {
  }

}

#endif // DATATOOLS_ARCHIVES_LIST_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
