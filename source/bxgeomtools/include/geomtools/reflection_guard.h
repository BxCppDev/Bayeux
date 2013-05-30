/* reflection_guard.h */
/*
 * Description :
 *
 *  Some useful guard material related to Camp/Introspection executable
 *  building and linkage.
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
 *
 *
 * Useful link : http://gcc.gnu.org/onlinedocs/cpp/
 *
 */

#ifndef GEOMTOOLS_REFLECTION_GUARD_H_
#define GEOMTOOLS_REFLECTION_GUARD_H_

// Standard Library

// Third Party

// Geomtools
#include <geomtools/geomtools_config.h>
#include <geomtools/detail/reflection_link_guard.h>

#if GEOMTOOLS_WITH_REFLECTION != 1
#warning This executable is built with its own geomtools reflection code.
#include <geomtools/the_introspectable.h>
#else
#warning This executable must ensure the geomtools reflection library is loaded.

namespace geomtools {

/** \brief Data structure that ensures the invocation of some explicit code
 *         for datatools_reflection DLL liking.
 */
struct reflection_guard {
  reflection_guard() {
    ::geomtools::detail::reflection::dynamic_link_guard& dlg
      = ::geomtools::detail::reflection::dynamic_link_guard::instance();
  }
  static reflection_guard _g_trigger_link_guard_;
};

reflection_guard reflection_guard::_g_trigger_link_guard_;

} // end namespace geomtools
#endif // GEOMTOOLS_WITH_REFLECTION != 1

#endif // GEOMTOOLS_REFLECTION_GUARD_H_
