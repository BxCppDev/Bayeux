/// \file genbb_help/bio_guard.h
/*
 * Description :
 *
 *  Some useful guard macro related to Boost/Serialisation executable
 *  building and linkage.
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
 *
 *
 * Useful link : http://gcc.gnu.org/onlinedocs/cpp/
 *
 */

#ifndef GENBB_HELP_BIO_GUARD_H
#define GENBB_HELP_BIO_GUARD_H

// This project:
#include <genbb_help/genbb_help_config.h>
#include <genbb_help/detail/bio_link_guard.h>

#if GENBB_HELP_WITH_BIO != 1
#warning This executable is built with its own genbb_help Boost/Serialization code.
#include <genbb_help/the_serializable.h>
#else
//#warning This executable must ensure the genbb_help Boost/Serialization library is loaded.
namespace genbb {

/** \brief Data structure that ensures the invocation of some explicit code
 *         for genbb_help_bio DLL liking.
 */
struct bio_guard {
  bio_guard() {
    // genbb::detail::serialization::dynamic_link_guard& dlg =
    ::genbb::detail::serialization::dynamic_link_guard::instance();
  }

  static bio_guard _g_trigger_link_guard_;
};

bio_guard bio_guard::_g_trigger_link_guard_;

} // end namespace genbb
#endif // GENBB_HELP_WITH_BIO != 1

#endif // GENBB_HELP_BIO_GUARD_H

// Local Variables: --
// mode: c++ --
// End: --
