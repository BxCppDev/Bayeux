/* bio_guard.h */
/*
 * Description :
 *
 *  Some useful guard macro related to Boost/Serialisation executable
 *  building and linkage.
 *
 * Copyright (C) 2011-2012 Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef MCTOOLS_BIO_GUARD_H_
#define MCTOOLS_BIO_GUARD_H_

// Standard Library

// Third Party

// Mctools
#include <mctools/mctools_config.h>
#include <mctools/detail/bio_link_guard.h>

#if MCTOOLS_WITH_BIO != 1
#warning This executable is built with its own mctools Boost/Serialization code.
#include <mctools/the_serializable.h>
#else
//#warning This executable must ensure the mctools Boost/Serialization library is loaded.
namespace mctools {

/** \brief Data structure that ensures the invocation of some explicit code
 *         for mctools_bio DLL liking.
 */
struct bio_guard {
  bio_guard() {
    //mctools::detail::serialization::dynamic_link_guard& dlg =
    ::mctools::detail::serialization::dynamic_link_guard::instance();
  }

  static bio_guard _g_trigger_link_guard_;
};

bio_guard bio_guard::_g_trigger_link_guard_;

} // end namespace mctools
#endif // MCTOOLS_WITH_BIO != 1

#endif // MCTOOLS_BIO_GUARD_H_
