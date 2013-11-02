/* reflection_link_guard.h */
/*
 * Description :
 *
 *  Some useful macros related to Camp reflection.
 *
 * Copyright (C) 2013 Francois Mauger <mauger@lpccaen.in2p3.fr>
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
#ifndef MCTOOLS_DETAIL_REFLECTION_LINK_GUARD_H_
#define MCTOOLS_DETAIL_REFLECTION_LINK_GUARD_H_

namespace mctools {
namespace detail {
/// Nested private namespace of the Bayeux/mctools module library (reflection)
namespace reflection {

/** \brief Data structure to ensure that a given executable is linked
 *        against the mctools_reflection DLL with new gcc 4.6 linking strategy
 *        even if no explicit mctools reflection code is explicitely
 *        invoked from the executable.
 */
class dynamic_link_guard {
 public:
  dynamic_link_guard();
  ~dynamic_link_guard();
  static dynamic_link_guard& instance();
};

} // end namespace reflection
} // end namespace detail
} // end namespace mctools

#endif // MCTOOLS_DETAIL_REFLECTION_LINK_GUARD_H_
