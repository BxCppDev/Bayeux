/// \file datatools/portable_archives_support.h
/*
 * Description :
 *
 *  Support of portable binary archives.
 *
 * Copyright (C) 2019 Francois Mauger <mauger@lpccaen.in2p3.fr>
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

#ifndef DATATOOLS_PORTABLE_ARCHIVES_SUPPORT_H
#define DATATOOLS_PORTABLE_ARCHIVES_SUPPORT_H

// This project:
#include <datatools/datatools_config.h>

// - Legacy EOS archive (version 5.0)
#if Bayeux_USE_EOS_ARCHIVES == 1
// 2012-06-07 FM : switch to Portable Archive v5.0
#include <datatools/eos/portable_iarchive.hpp>
#include <datatools/eos/portable_oarchive.hpp>
namespace datatools {
  typedef eos::portable_iarchive portable_iarchive;
  typedef eos::portable_oarchive portable_oarchive;
} // end of namespace datatools
#endif // Bayeux_USE_EOS_ARCHIVES == 1

// - EPA archive (version 6.0, candidate for official Boost/Serialization)
#if Bayeux_USE_EPA_ARCHIVES == 1
#include <boost/archive/portable_iarchive.hpp>
#include <boost/archive/portable_oarchive.hpp>
namespace datatools {
  typedef boost::archive::portable_iarchive portable_iarchive;
  typedef boost::archive::portable_oarchive portable_oarchive;
} // end of namespace datatools
#endif // Bayeux_USE_EPA_ARCHIVES == 1

#endif // DATATOOLS_PORTABLE_ARCHIVES_SUPPORT_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
