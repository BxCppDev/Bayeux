/* -*- mode: c++; -*- */
/* datatools::reflection_utils.h */
/*
 * Description :
 *
 *  Some useful classes and macros related to datatools/Camp reflection.
 *
 * Copyright (C) 2012 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
#ifndef DATATOOLS_DETAIL_REFLECTION_UTILS_H_
#define DATATOOLS_DETAIL_REFLECTION_UTILS_H_

// Standard Library
#include <string>

// Special Boost headers :
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>

namespace datatools{
  namespace detail {
    /// Nested private namespace of the Bayeux/datatools module library (reflection)
    namespace reflection {
      template<typename Introspectable>
      void implement_reflection (unsigned int);
    }
  }
}

#endif // DATATOOLS_DETAIL_REFLECTION_UTILS_H_
